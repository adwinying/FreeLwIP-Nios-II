#include <string.h>

#ifdef TARGET_KERNEL_ATK2

#include "Os.h"
#include "prc_sil.h"
#include "t_syslog.h"

#include "target_test.h"
#include "histogram.h"

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

#include <tinet_defs.h>
#include <tinet_config.h>
#include <tinet_atk2_cfg.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/net.h>
#include <net/net_timer.h>
#include <net/net_count.h>
#include <net/net_buf.h>

#include "avalon_sgdma_descriptor.h"
#include "avalon_tse.h"

#include "eth_tse_regs.h"
#include "avalon_sgdma_regs.h"
#include "avalon_sgdma.h"
#include "if_tse.h"

#include "ipport.h"

#include "alt_errno.h"

extern ObjectIDType	histid;
extern boolean		meas_flg;

/*
 *  ネットワークインタフェースに依存するソフトウェア情報
 */

typedef struct t_tse_mac_info T_TSE_SOFTC;

/*
 *  ネットワークインタフェースのソフトウェア情報
 */

/* ネットワークインタフェースに依存するソフトウェア情報 */

static T_NET_BUF_IF_PDU rx_nbuf = { };	/* 受信バッファ */

static T_TSE_SOFTC tse_softc = {
	(T_TSE_MAC*)TSE_BASE,			/* TSE MAC のベースアドレス		*/
	8192,					/* 送信 FIFO バッファの深さ		*/
	8192,					/* 受信 FIFO バッファの深さ		*/
	TSE_FULL_MAC,				/* MAC のタイプ				*/

	(T_SGDMA_CSR*)SGDMA_TX_BASE,		/* SGDMA TX のベースアドレス		*/
	(T_SGDMA_CSR*)SGDMA_RX_BASE,		/* SGDMA RX のベースアドレス		*/

	(T_SGDMA_DESCRIPTOR*)DESCRIPTOR_MEMORY_BASE, /* ディスクリプタ・メモリのベースアドレス */

	0x10,					/* PHY の MDIO アドレス			*/
	&MV88E1111,				/* PHY プロファイルを指すポインタ	*/
	&marvell_cfg_rgmii,			/* 追加の設定を行う関数を指すポインタ	*/

	{ 0x00, 0x07, 0xab, 0xf0, 0x0d, 0xba },	/* MAC アドレス				*/

	NULL,					/* 送信バッファ				*/
	(T_NET_BUF*)&rx_nbuf			/* 受信バッファ				*/
	};

/* ネットワークインタフェースに依存しないソフトウェア情報 */

T_IF_SOFTC if_softc = {
	{},					/* ネットワークインタフェースのアドレス	*/
	0,					/* 送信タイムアウト			*/
	&tse_softc,				/* ディバイス依存のソフトウェア情報	*/
	EVT_IF_TSE_SBUF_READY,			/* 送信イベントマスク			*/
	EVT_IF_TSE_RBUF_READY,			/* 受信イベントマスク			*/
	};

/*
 *  tse_sgdma_tx_completions -- SGDMA TX が完了状態かの確認
 */

static int_t
tse_sgdma_tx_completions (T_TSE_SOFTC *sc)
{
	int32_t timeout;

	/* SGDMA コントローラがビジーでないことを確かめる */
	timeout = 0;

	while (sil_rew_iop((void*)&sc->tx_sgdma_csr->status) &
	       AVALON_SGDMA_STATUS_BUSY) {
		if (timeout ++ == TSE_SGDMA_BUSY_TIME_OUT_CNT) {
			syslog(LOG_WARNING, "WARNING : TX SGDMA Timeout");
			return ENP_RESOURCE;		/* ここで立ち往生するのを防ぐ */
			}
		}

	/* ディスクリプタが現在使用中かの確認 */
	if (sil_reb_iop((void*)&(&sc->desc[TSE_FIRST_TX_SGDMA_DESC_OFST])->control) &
	    AVALON_SGDMA_DESCRIPTOR_CONTROL_OWNED_BY_HW) {
		return -EINPROGRESS;
		}

	return SUCCESS;
	}

/*
 *  tse_sgdma_read_init -- SGDMA ディスクリプタ・チェインの初期化・設定
 */

static int_t
tse_sgdma_read_init (T_TSE_SOFTC *sc)
{
	if (!sc->rx_nbuf) {		/* 受信バッファを獲得できなかった場合 */
		syslog(LOG_ERROR, "[tse_sgdma_read_init] Fatal error: No free packet buffers for RX");

		return ENP_NOBUFFER;
		}

	avalon_sgdma_construct_descriptor(
		&sc->desc[TSE_FIRST_RX_SGDMA_DESC_OFST],	/* 構築されるディスクリプタ		*/
		&sc->desc[TSE_SECOND_RX_SGDMA_DESC_OFST],	/* "次の" ディスクリプタへのポインタ	*/
		(uint32_t)0x0,					/* Read addr: stream-to-mem モードでは N/A */
		(uint32_t*)sc->rx_nbuf->buf,			/* 最初のライト・アドレス		*/
		0,						/* EOP 信号が受信されるまで転送を継続	*/
		0,						/* Generate EOP: stream-to-mem モードでは N/A */
		0,						/* Read fixed: stream-to-mem モードでは N/A */
		0);						/* 規定のアドレスには書き込まない	*/

	syslog(LOG_DEBUG, "[tse_sgdma_read_init] RX descriptor chain desc created");

	tse_mac_aRxRead(sc, &sc->desc[TSE_FIRST_RX_SGDMA_DESC_OFST]);

	return SUCCESS;
	}

/*
 *  tse_mac_init -- TSE MAC の初期化ルーチン
 */

static int_t
tse_mac_init (T_IF_SOFTC *ic)
{
	T_TSE_SOFTC	*sc = ic->sc;
	int_t		dat;
	int_t		speed, duplex, result, x;
	int_t		status = SUCCESS;

	syslog(LOG_DEBUG, "[tse_mac_init]");

	/* 送受信 SGDMA アドレスの取得 */
	if (!sc->tx_sgdma_csr) {
		syslog(LOG_ERROR, "[altera_eth_tse_init] Error opening TX SGDMA");
		return ENP_RESOURCE;
		}

	if (!sc->rx_sgdma_csr) {
		syslog(LOG_ERROR, "[altera_eth_tse_init] Error opening RX SGDMA");
		return ENP_RESOURCE;
		}

	/* 受信側 SGDMA のリセット */
	sil_wrw_iop((void*)&sc->rx_sgdma_csr->control,
	            AVALON_SGDMA_CONTROL_SOFTWARERESET);
	sil_wrw_iop((void*)&sc->rx_sgdma_csr->control, 0x0);

	/* 必要がある場合、PHY をリセット */
	result = getPHYSpeed(sc);
	speed = (result >> 1) & 0x07;
	duplex = result & 0x01;

	/* MAC のリセット */
	sil_wrw_iop((void*)&sc->tse_mac_base->COMMAND_CONFIG,
	            TSEMAC_CMD_SW_RESET |
	            TSEMAC_CMD_TX_ENA   |
	            TSEMAC_CMD_RX_ENA);

	x = 0;
	while (sil_rew_iop((void*)&sc->tse_mac_base->COMMAND_CONFIG) &
	       TSEMAC_CMD_SW_RESET) {
		if (x ++ > TSE_SW_RESET_TIME_OUT_CNT) {
			break;
			}
		}
	if (x >= TSE_SW_RESET_TIME_OUT_CNT) {
		syslog(LOG_DEBUG, "TSEMAC SW reset bit never cleared!");
		}

	dat = sil_rew_iop((void*)&sc->tse_mac_base->COMMAND_CONFIG);
	if ((dat & (TSEMAC_CMD_TX_ENA | TSEMAC_CMD_RX_ENA)) != 0) {
		syslog(LOG_WARNING, "WARN: RX/TX not disabled after reset... missing PHY clock? CMD_CONFIG=0x%08x", dat);
		}
	else {
		syslog(LOG_DEBUG, "OK, x=%d, CMD_CONFIG=0x%08x", x, dat);
		}

	/* MAC レジスタの初期化 */
	sil_wrw_iop((void*)&sc->tse_mac_base->FRM_LENGTH, ETHER_MAX_LEN);
	sil_wrw_iop((void*)&sc->tse_mac_base->RX_ALMOST_EMPTY, 8);
	sil_wrw_iop((void*)&sc->tse_mac_base->RX_ALMOST_FULL,  8);
	sil_wrw_iop((void*)&sc->tse_mac_base->TX_ALMOST_EMPTY, 8);
	sil_wrw_iop((void*)&sc->tse_mac_base->TX_ALMOST_FULL,  3);
	sil_wrw_iop((void*)&sc->tse_mac_base->TX_SECTION_EMPTY, sc->tse_tx_depth - 16);
	sil_wrw_iop((void*)&sc->tse_mac_base->TX_SECTION_FULL,  0);		/* start transmit when there are 48 bytes */
	sil_wrw_iop((void*)&sc->tse_mac_base->RX_SECTION_EMPTY, sc->tse_rx_depth - 16);
	sil_wrw_iop((void*)&sc->tse_mac_base->RX_SECTION_FULL,  0);

	/* フレームの先頭から 2 バイトを削除する TX_SHIFT16 のイネーブル */
	if ((IF_ETHER_NIC_HDR_ALIGN != 0) && (IF_ETHER_NIC_HDR_ALIGN != 2)) {
		syslog(LOG_ERROR, "[tse_mac_init] Error: Unsupported Ethernet Header Bias Value, %d", IF_ETHER_NIC_HDR_ALIGN);
		return ENP_PARAM;
		}

	if (IF_ETHER_NIC_HDR_ALIGN == 0) {
		int32_t temp_reg;

		temp_reg = sil_rew_iop((void*)&sc->tse_mac_base->TX_CMD_STAT) & ~TSEMAC_TX_CMD_STAT_TXSHIFT16;
		sil_wrw_iop((void*)&sc->tse_mac_base->TX_CMD_STAT, temp_reg);

		/* MAC が 16 ビットシフトオプションをサポートするかの確認 */
		if (sil_rew_iop((void*)&sc->tse_mac_base->TX_CMD_STAT) &
		    TSEMAC_TX_CMD_STAT_TXSHIFT16) {
			syslog(LOG_ERROR, "[tse_mac_init] Error: Incompactible %d value with TX_CMD_STAT register return TxShift16 value.", IF_ETHER_NIC_HDR_ALIGN);
			return ENP_LOGIC;
			}

		/* 16 ビットに受信フレームの開始アドレスをアラインメントする RX_SHIFT16 のイネーブル */
		temp_reg = sil_rew_iop((void*)&sc->tse_mac_base->RX_CMD_STAT) & ~TSEMAC_RX_CMD_STAT_RXSHIFT16;
		sil_wrw_iop((void*)&sc->tse_mac_base->RX_CMD_STAT, temp_reg);

		/* RX CMD STATUS レジスタで、MAC が 16 ビットシフトオプションをサポートするかの確認 */
		if (sil_rew_iop((void*)&sc->tse_mac_base->RX_CMD_STAT) & TSEMAC_RX_CMD_STAT_RXSHIFT16) {
			syslog(LOG_ERROR, "[tse_mac_init] Error: Incompactible %d value with RX_CMD_STAT register return RxShift16 value.", IF_ETHER_NIC_HDR_ALIGN);
			return ENP_LOGIC;
			}
		}	/* if (IF_ETHER_NIC_HDR_ALIGN == 0) */

	if (IF_ETHER_NIC_HDR_ALIGN == 2) {
		sil_wrw_iop((void*)&sc->tse_mac_base->TX_CMD_STAT, TSEMAC_TX_CMD_STAT_TXSHIFT16);

		/* MAC が 16 ビットシフトオプションをサポートするかの確認 */
		if (!(sil_rew_iop((void*)&sc->tse_mac_base->TX_CMD_STAT) &
		      TSEMAC_TX_CMD_STAT_TXSHIFT16)) {
			syslog(LOG_ERROR, "[tse_mac_init] Error: Incompatible %d value with TX_CMD_STAT register return TxShift16 value.", IF_ETHER_NIC_HDR_ALIGN);
			return ENP_LOGIC;
			}

		/* 16 ビットに受信フレームの開始アドレスをアラインメントする RX_SHIFT16 のイネーブル */
		sil_wrw_iop((void*)&sc->tse_mac_base->RX_CMD_STAT, TSEMAC_RX_CMD_STAT_RXSHIFT16);

		/* RX CMD STATUS レジスタで、MAC が 16 ビットシフトオプションをサポートするかの確認 */
		if (!(sil_rew_iop((void*)&sc->tse_mac_base->RX_CMD_STAT) & TSEMAC_RX_CMD_STAT_RXSHIFT16)) {
			syslog(LOG_ERROR, "[tse_mac_init] Error: Incompatible %d value with RX_CMD_STAT register return RxShift16 value.", IF_ETHER_NIC_HDR_ALIGN);
			return ENP_LOGIC;
			}
		}	/* if (IF_ETHER_NIC_HDR_ALIGN == 2) */

	/* MAC のイネーブル */
	dat = TSEMAC_CMD_TX_ENA      |
	      TSEMAC_CMD_RX_ENA      |
	      TSEMAC_CMD_RX_ERR_DISC |
	      TSEMAC_CMD_TX_ADDR_INS |
	      TSEMAC_CMD_RX_ERR_DISC;	/* CRC エラーのフレームを自動的に破棄 */


	/* 1000 Mbps */
	if (speed == 0x01) {
		dat |= TSEMAC_CMD_ETH_SPEED;
		dat &= ~TSEMAC_CMD_ENA_10;
		}
	/* 100 Mbps */
	else if (speed == 0x02) {
		dat &= ~TSEMAC_CMD_ETH_SPEED;
		dat &= ~TSEMAC_CMD_ENA_10;
		}
	/* 10 Mbps */
	else if (speed == 0x04) {
		dat &= ~TSEMAC_CMD_ETH_SPEED;
		dat |= TSEMAC_CMD_ENA_10;
		}
	/* 無効な速度が返された場合、初期値を 100 Mbps に設定 */
	else {
		dat &= ~TSEMAC_CMD_ETH_SPEED;
		dat &= ~TSEMAC_CMD_ENA_10;
		}

	/* 半二重 */
	if (duplex == TSE_PHY_DUPLEX_HALF) {
		dat |= TSEMAC_CMD_HD_ENA;
		}
	/* 全二重 */
	else {
		dat &= ~TSEMAC_CMD_HD_ENA;
		}

	sil_wrw_iop((void*)&sc->tse_mac_base->COMMAND_CONFIG, dat);
	syslog(LOG_DEBUG, "\nMAC post-initialization: CMD_CONFIG=0x%08x",
	       sil_rew_iop((void*)&sc->tse_mac_base->COMMAND_CONFIG));

	/* MAC アドレスを設定する。*/
	sil_wrw_iop((void*)&sc->tse_mac_base->MAC_0,
	            ((int_t)((unsigned char)ic->ifaddr.lladdr[0])       |
	             (int_t)((unsigned char)ic->ifaddr.lladdr[1] <<  8) |
	             (int_t)((unsigned char)ic->ifaddr.lladdr[2] << 16) |
	             (int_t)((unsigned char)ic->ifaddr.lladdr[3] << 24)));

	sil_wrw_iop((void*)&sc->tse_mac_base->MAC_1,
	            (((int_t)((unsigned char)ic->ifaddr.lladdr[4]) |
	              (int_t)((unsigned char)ic->ifaddr.lladdr[5] <<  8)) & 0xffff));

	status = tse_sgdma_read_init(sc);

	return status;
	}

/*
 *  tse_get_frame -- Ethernet フレームを入力する。
 */

static T_NET_BUF *
tse_get_frame (T_TSE_SOFTC *sc, uint16_t len)
{
	T_NET_BUF	*input = NULL;
	uint16_t	align;
	ER		error;

	/*
	 *  +-----------+--------+---------+---------+
	 *  | Ehter HDR | IP HDR | TCP HDR | TCP SDU |
	 *  +-----------+--------+---------+---------+
	 *        14        20        20        n
	 *   <----------------- len ---------------->
	 *              ^
	 *              t_net_buf で 4 オクテット境界にアラインされている。
	 *
	 *  tcp_input と udp_input では、擬似ヘッダと SDU でチェックサムを
	 *  計算するが、n が 4 オクテット境界になるように SDU の後ろに 0 を
	 *  パッディングする。その分を考慮して net_buf を獲得しなければならない。
	 */
	align = ((((len - sizeof(T_IF_HDR)) + 3) >> 2) << 2) + sizeof(T_IF_HDR);

	if ((error = get_net_buf(&input, align)) == E_OK && input != NULL) {
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_IN_PACKETS], 1);
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_IN_OCTETS],  len);
		memcpy(input->buf, sc->rx_nbuf->buf, len);
		}
	else {
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_IN_ERR_PACKETS], 1);
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_NO_BUFS], 1);
		}
	return input;
	}

/*
 *  tse_xmit -- フレームを送信する。
 *
 *    注意: NIC 割り込み禁止状態で呼び出すこと。
 */

static void
tse_xmit (T_IF_SOFTC *ic)
{
	T_TSE_SOFTC *sc = ic->sc;

	/* FIFO 内に余地があることを確認する */
	avalon_sgdma_construct_descriptor(
		&sc->desc[TSE_FIRST_TX_SGDMA_DESC_OFST],	/* 構築されるディスクリプタ		*/
		&sc->desc[TSE_SECOND_TX_SGDMA_DESC_OFST],	/* "次の" ディスクリプタへのポインタ	*/
		(uint32_t*)sc->tx_nbuf->buf,			/* 最初のリード・アドレス		*/
		(uint32_t)0x0,					/* Write addr: mem-to-stream モードでは N/A */
		sc->tx_nbuf->len,				/* バイト数				*/
		1,						/* EOP の生成				*/
		0,						/* 規定のアドレスからは読み出さない	*/
		1);						/* SOP の生成				*/

	tse_mac_aTxWrite(sc, &sc->desc[TSE_FIRST_TX_SGDMA_DESC_OFST]);

	/* 送信タイムアウトを設定する。*/
	ic->timer = TMO_IF_TSE_XMIT;
	}

/*
 *  tse_stop -- tse ネットワークインタフェースを停止する。
 *
 *    注意: NIC 割り込み禁止状態で呼び出すこと。
 */

static void
tse_stop (T_TSE_SOFTC *sc)
{
	int_t state;

	/* デバイスの受信パスのディセーブル */
	state = sil_rew_iop((void*)&sc->tse_mac_base->COMMAND_CONFIG);
	sil_wrw_iop((void*)&sc->tse_mac_base->COMMAND_CONFIG, state & ~TSEMAC_CMD_RX_ENA);
	}

/*
 *  tse_init_sub -- tse ネットワークインタフェースの初期化
 *
 *    注意: NIC 割り込み禁止状態で呼び出すこと。
 */

static void
tse_init_sub (T_IF_SOFTC *ic)
{
	T_TSE_SOFTC	*sc = ic->sc;

	/* 送信タイムアウトをリセットする。*/
	ic->timer = 0;

	/* SGDMA コントローラの初期化 */
	avalon_sgdma_init((void*)sc->tx_sgdma_csr);
	//avalon_sgdma_init((void*)sc->rx_sgdma_csr);

	/* TSE MAC の初期化 */
	tse_mac_init(ic);
	}

/*
 *  tse_reset -- tse ネットワークインタフェースをリセットする。
 */

void
tse_reset (T_IF_SOFTC *ic)
{
	/* NIC からの割り込みを禁止する。*/
	SuspendOSInterrupts();

	NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_RESETS], 1);
	tse_stop(ic->sc);
	tse_init_sub(ic);

	/* NIC からの割り込みを許可する。*/
	ResumeOSInterrupts();

	/* 送信可能イベントを初期化する。*/
	SetEvent(ETHER_OUTPUT_TASK, ic->mask_txb_ready);
	}

/*
 *  tse_get_softc -- ネットワークインタフェースのソフトウェア情報を返す。
 */

T_IF_SOFTC *
tse_get_softc (void)
{
	return &if_softc;
	}

/*
 *  tse_watchdog -- tse ネットワークインタフェースのワッチドッグタイムアウト
 */

void
tse_watchdog (T_IF_SOFTC *ic)
{
	NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_OUT_ERR_PACKETS], 1);
	NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_TIMEOUTS], 1);
	tse_reset(ic);
	}

/*
 *  tse_probe -- tse ネットワークインタフェースの検出
 */

void
tse_probe (T_IF_SOFTC *ic)
{
	T_TSE_SOFTC	*sc = ic->sc;
	int_t		ix;

	/* MAC アドレスを読み込む。*/
	for (ix = 0; ix < ETHER_ADDR_LEN; ix ++)
		ic->ifaddr.lladdr[ix] = sc->mac_addr[ix];

	/* PHY の初期化 */
	tse_phy_init(sc);
	}

/*
 *  tse_init -- tse ネットワークインタフェースの初期化
 */

void
tse_init (T_IF_SOFTC *ic)
{
	/* NIC からの割り込みを禁止する。*/
	SuspendOSInterrupts();

	/* tse_init 本体を呼び出す。*/
	tse_init_sub(ic);

	/* NIC からの割り込みを許可する。*/
	ResumeOSInterrupts();
	}

/*
 *  tse_read -- フレームの読み込み
 */

T_NET_BUF *
tse_read (T_IF_SOFTC *ic)
{
	T_TSE_SOFTC	*sc = ic->sc;
	T_NET_BUF	*input = NULL;
	int_t		len;
	uint8_t		desc_status;

	T_SGDMA_DESCRIPTOR *currdescriptor_ptr =
		&sc->desc[TSE_FIRST_RX_SGDMA_DESC_OFST];

	/* NIC からの割り込みを禁止する。*/
	SuspendOSInterrupts();

	/* status ビットをディスクリプタから獲得 */
	desc_status = sil_reb_iop((void*)&currdescriptor_ptr->status);

	len = sil_reh_iop((void*)&currdescriptor_ptr->actual_bytes_transferred);

	if ((desc_status &
	     (AVALON_SGDMA_DESCRIPTOR_STATUS_E_CRC      |
	      AVALON_SGDMA_DESCRIPTOR_STATUS_E_PARITY   |
	      AVALON_SGDMA_DESCRIPTOR_STATUS_E_OVERFLOW |
	      AVALON_SGDMA_DESCRIPTOR_STATUS_E_SYNC     |
	      AVALON_SGDMA_DESCRIPTOR_STATUS_E_UEOP     |
	      AVALON_SGDMA_DESCRIPTOR_STATUS_E_MEOP     |
	      AVALON_SGDMA_DESCRIPTOR_STATUS_E_MSOP)) == 0) {
		input = tse_get_frame(sc, len);
		}	/* if (ディスクリプタにエラーがない場合) */
	else {
		syslog(LOG_ERROR, "RX descriptor reported error. packet dropped");

		/* 受信エラーとリセットを記録する。*/
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_IN_ERR_PACKETS], 1);
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_RESETS], 1);
		tse_stop(sc);
		tse_init_sub(ic);

		/* NIC からの割り込みを許可する。*/
		ResumeOSInterrupts();

		/* 送信可能イベントを初期化する。*/
		SetEvent(ETHER_OUTPUT_TASK, ic->mask_txb_ready);

		return NULL;
		}

	/* 以前に構築した SGDMA バッファを直接再利用 */
	sil_wrw_iop((void*)&currdescriptor_ptr->write_addr,
	            (uint32_t)sc->rx_nbuf->buf);

	/* 最後のディスクリプタ・ワードの内容の連結 */
	sil_wrb_iop((void*)&currdescriptor_ptr->control,
	            AVALON_SGDMA_DESCRIPTOR_CONTROL_OWNED_BY_HW |
	            AVALON_SGDMA_DESCRIPTOR_CONTROL_GENERATE_EOP);

	/* SGDMA の再開 */
	avalon_sgdma_do_async_transfer(
		sc->rx_sgdma_csr,
		&sc->desc[TSE_FIRST_RX_SGDMA_DESC_OFST]);

	/* NIC からの割り込みを許可する。*/
	ResumeOSInterrupts();

	return input;
	}

/*
 *  tse_start -- 送信フレームをバッファリングする。
 */

void
tse_start (T_IF_SOFTC *ic, T_NET_BUF *output)
{
	T_TSE_SOFTC *sc = ic->sc;

	/* NIC からの割り込みを禁止する。*/
	SuspendOSInterrupts();

	sc->tx_nbuf = output;

	/* DMA を用いた送信 FIFO へのデータ書き込み */
	tse_xmit(ic);

	NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_OUT_PACKETS], 1);
	NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_OUT_OCTETS],  output->len);

	/* NIC からの割り込みを許可する。*/
	ResumeOSInterrupts();
	}

/*
 *  送信割込みハンドラ
 */

ISR(if_tse_tx_handler)
{
	T_TSE_SOFTC	*sc;
	T_IF_SOFTC	*ic;
	uint32_t	ready;

	ic = &if_softc;
	sc = ic->sc;

	/* 保留状態の割り込みのクリア */
	sil_wrw_iop((void*)&sc->tx_sgdma_csr->control,
	            sil_rew_iop((void*)&sc->tx_sgdma_csr->control)
	            | AVALON_SGDMA_CONTROL_CLEAR_INTERRUPT);

	/* ISR が返る前に IRQ の無効を確実とするためのダミー読み込み */
	sil_rew_iop((void*)&sc->tx_sgdma_csr->control);

	ready = tse_sgdma_tx_completions(sc);

	/* 送信したバッファの開放 */
	if (ready == SUCCESS) {
#ifdef ETHER_NIC_CFG_RELEASE_NET_BUF

		if ((sc->tx_nbuf->flags & NB_FLG_NOREL_IFOUT) == 0) {
			syscall(rel_net_buf(sc->tx_nbuf));
			}
		else {
			sc->tx_nbuf->flags &= (uint8_t)~NB_FLG_NOREL_IFOUT;

#ifdef SUPPORT_TCP
			SetEvent(TCP_OUTPUT_TASK, EVT_TCP_POST_OUTPUT);
#endif	/* of #ifdef SUPPORT_TCP */
			}

#endif	/* of #ifdef ETHER_NIC_CFG_RELEASE_NET_BUF */
		}

	/* 送信タイムアウトをリセットする。*/
	ic->timer = 0;

	if (SetEvent(ETHER_OUTPUT_TASK, ic->mask_txb_ready) != E_OK)
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_TXB_QOVRS], 1);
	}

/*
 *  受信割込みハンドラ
 */

ISR(if_tse_rx_handler)
{
	T_TSE_SOFTC	*sc;
	T_IF_SOFTC	*ic;
	uint8_t		sgdma_status;

	ic = &if_softc;
	sc = ic->sc;

	/* 保留状態の割り込みのクリア */
	sil_wrw_iop((void*)&sc->rx_sgdma_csr->control,
	            sil_rew_iop((void*)&sc->rx_sgdma_csr->control)
	            | AVALON_SGDMA_CONTROL_CLEAR_INTERRUPT);

	/* ISR が返る前に IRQ の無効を確実とするためのダミー読み込み */
	sil_rew_iop((void*)&sc->rx_sgdma_csr->control);

	/* 実行時間の計測開始 */
//	if (histid <= TNUM_HIST) {
//		begin_measure(histid);
//		meas_flg = TRUE;
//		}
//	if (histid <= TNUM_HIST && meas_flg == TRUE) {
//		end_measure(histid++);
//		meas_flg = FALSE;
//		}

	/* 割り込み要因を検証するための SGDMA status の獲得 */
	sgdma_status = sil_rew_iop((void*)&sc->rx_sgdma_csr->status);

	if (sgdma_status & (AVALON_SGDMA_STATUS_CHAIN_COMPLETED |
	                    AVALON_SGDMA_STATUS_DESC_COMPLETED)) {
		/* 受信割り込み処理 */
		if (SetEvent(ETHER_INPUT_TASK, ic->mask_rxb_ready) != E_OK)
			NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_RXB_QOVRS], 1);
		}	/* if (有効な SGDMA 割り込み) */
	}
