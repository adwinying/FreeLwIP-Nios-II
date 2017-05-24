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
 *  �l�b�g���[�N�C���^�t�F�[�X�Ɉˑ�����\�t�g�E�F�A���
 */

typedef struct t_tse_mac_info T_TSE_SOFTC;

/*
 *  �l�b�g���[�N�C���^�t�F�[�X�̃\�t�g�E�F�A���
 */

/* �l�b�g���[�N�C���^�t�F�[�X�Ɉˑ�����\�t�g�E�F�A��� */

static T_NET_BUF_IF_PDU rx_nbuf = { };	/* ��M�o�b�t�@ */

static T_TSE_SOFTC tse_softc = {
	(T_TSE_MAC*)TSE_BASE,			/* TSE MAC �̃x�[�X�A�h���X		*/
	8192,					/* ���M FIFO �o�b�t�@�̐[��		*/
	8192,					/* ��M FIFO �o�b�t�@�̐[��		*/
	TSE_FULL_MAC,				/* MAC �̃^�C�v				*/

	(T_SGDMA_CSR*)SGDMA_TX_BASE,		/* SGDMA TX �̃x�[�X�A�h���X		*/
	(T_SGDMA_CSR*)SGDMA_RX_BASE,		/* SGDMA RX �̃x�[�X�A�h���X		*/

	(T_SGDMA_DESCRIPTOR*)DESCRIPTOR_MEMORY_BASE, /* �f�B�X�N���v�^�E�������̃x�[�X�A�h���X */

	0x10,					/* PHY �� MDIO �A�h���X			*/
	&MV88E1111,				/* PHY �v���t�@�C�����w���|�C���^	*/
	&marvell_cfg_rgmii,			/* �ǉ��̐ݒ���s���֐����w���|�C���^	*/

	{ 0x00, 0x07, 0xab, 0xf0, 0x0d, 0xba },	/* MAC �A�h���X				*/

	NULL,					/* ���M�o�b�t�@				*/
	(T_NET_BUF*)&rx_nbuf			/* ��M�o�b�t�@				*/
	};

/* �l�b�g���[�N�C���^�t�F�[�X�Ɉˑ����Ȃ��\�t�g�E�F�A��� */

T_IF_SOFTC if_softc = {
	{},					/* �l�b�g���[�N�C���^�t�F�[�X�̃A�h���X	*/
	0,					/* ���M�^�C���A�E�g			*/
	&tse_softc,				/* �f�B�o�C�X�ˑ��̃\�t�g�E�F�A���	*/
	EVT_IF_TSE_SBUF_READY,			/* ���M�C�x���g�}�X�N			*/
	EVT_IF_TSE_RBUF_READY,			/* ��M�C�x���g�}�X�N			*/
	};

/*
 *  tse_sgdma_tx_completions -- SGDMA TX ��������Ԃ��̊m�F
 */

static int_t
tse_sgdma_tx_completions (T_TSE_SOFTC *sc)
{
	int32_t timeout;

	/* SGDMA �R���g���[�����r�W�[�łȂ����Ƃ��m���߂� */
	timeout = 0;

	while (sil_rew_iop((void*)&sc->tx_sgdma_csr->status) &
	       AVALON_SGDMA_STATUS_BUSY) {
		if (timeout ++ == TSE_SGDMA_BUSY_TIME_OUT_CNT) {
			syslog(LOG_WARNING, "WARNING : TX SGDMA Timeout");
			return ENP_RESOURCE;		/* �����ŗ�����������̂�h�� */
			}
		}

	/* �f�B�X�N���v�^�����ݎg�p�����̊m�F */
	if (sil_reb_iop((void*)&(&sc->desc[TSE_FIRST_TX_SGDMA_DESC_OFST])->control) &
	    AVALON_SGDMA_DESCRIPTOR_CONTROL_OWNED_BY_HW) {
		return -EINPROGRESS;
		}

	return SUCCESS;
	}

/*
 *  tse_sgdma_read_init -- SGDMA �f�B�X�N���v�^�E�`�F�C���̏������E�ݒ�
 */

static int_t
tse_sgdma_read_init (T_TSE_SOFTC *sc)
{
	if (!sc->rx_nbuf) {		/* ��M�o�b�t�@���l���ł��Ȃ������ꍇ */
		syslog(LOG_ERROR, "[tse_sgdma_read_init] Fatal error: No free packet buffers for RX");

		return ENP_NOBUFFER;
		}

	avalon_sgdma_construct_descriptor(
		&sc->desc[TSE_FIRST_RX_SGDMA_DESC_OFST],	/* �\�z�����f�B�X�N���v�^		*/
		&sc->desc[TSE_SECOND_RX_SGDMA_DESC_OFST],	/* "����" �f�B�X�N���v�^�ւ̃|�C���^	*/
		(uint32_t)0x0,					/* Read addr: stream-to-mem ���[�h�ł� N/A */
		(uint32_t*)sc->rx_nbuf->buf,			/* �ŏ��̃��C�g�E�A�h���X		*/
		0,						/* EOP �M������M�����܂œ]�����p��	*/
		0,						/* Generate EOP: stream-to-mem ���[�h�ł� N/A */
		0,						/* Read fixed: stream-to-mem ���[�h�ł� N/A */
		0);						/* �K��̃A�h���X�ɂ͏������܂Ȃ�	*/

	syslog(LOG_DEBUG, "[tse_sgdma_read_init] RX descriptor chain desc created");

	tse_mac_aRxRead(sc, &sc->desc[TSE_FIRST_RX_SGDMA_DESC_OFST]);

	return SUCCESS;
	}

/*
 *  tse_mac_init -- TSE MAC �̏��������[�`��
 */

static int_t
tse_mac_init (T_IF_SOFTC *ic)
{
	T_TSE_SOFTC	*sc = ic->sc;
	int_t		dat;
	int_t		speed, duplex, result, x;
	int_t		status = SUCCESS;

	syslog(LOG_DEBUG, "[tse_mac_init]");

	/* ����M SGDMA �A�h���X�̎擾 */
	if (!sc->tx_sgdma_csr) {
		syslog(LOG_ERROR, "[altera_eth_tse_init] Error opening TX SGDMA");
		return ENP_RESOURCE;
		}

	if (!sc->rx_sgdma_csr) {
		syslog(LOG_ERROR, "[altera_eth_tse_init] Error opening RX SGDMA");
		return ENP_RESOURCE;
		}

	/* ��M�� SGDMA �̃��Z�b�g */
	sil_wrw_iop((void*)&sc->rx_sgdma_csr->control,
	            AVALON_SGDMA_CONTROL_SOFTWARERESET);
	sil_wrw_iop((void*)&sc->rx_sgdma_csr->control, 0x0);

	/* �K�v������ꍇ�APHY �����Z�b�g */
	result = getPHYSpeed(sc);
	speed = (result >> 1) & 0x07;
	duplex = result & 0x01;

	/* MAC �̃��Z�b�g */
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

	/* MAC ���W�X�^�̏����� */
	sil_wrw_iop((void*)&sc->tse_mac_base->FRM_LENGTH, ETHER_MAX_LEN);
	sil_wrw_iop((void*)&sc->tse_mac_base->RX_ALMOST_EMPTY, 8);
	sil_wrw_iop((void*)&sc->tse_mac_base->RX_ALMOST_FULL,  8);
	sil_wrw_iop((void*)&sc->tse_mac_base->TX_ALMOST_EMPTY, 8);
	sil_wrw_iop((void*)&sc->tse_mac_base->TX_ALMOST_FULL,  3);
	sil_wrw_iop((void*)&sc->tse_mac_base->TX_SECTION_EMPTY, sc->tse_tx_depth - 16);
	sil_wrw_iop((void*)&sc->tse_mac_base->TX_SECTION_FULL,  0);		/* start transmit when there are 48 bytes */
	sil_wrw_iop((void*)&sc->tse_mac_base->RX_SECTION_EMPTY, sc->tse_rx_depth - 16);
	sil_wrw_iop((void*)&sc->tse_mac_base->RX_SECTION_FULL,  0);

	/* �t���[���̐擪���� 2 �o�C�g���폜���� TX_SHIFT16 �̃C�l�[�u�� */
	if ((IF_ETHER_NIC_HDR_ALIGN != 0) && (IF_ETHER_NIC_HDR_ALIGN != 2)) {
		syslog(LOG_ERROR, "[tse_mac_init] Error: Unsupported Ethernet Header Bias Value, %d", IF_ETHER_NIC_HDR_ALIGN);
		return ENP_PARAM;
		}

	if (IF_ETHER_NIC_HDR_ALIGN == 0) {
		int32_t temp_reg;

		temp_reg = sil_rew_iop((void*)&sc->tse_mac_base->TX_CMD_STAT) & ~TSEMAC_TX_CMD_STAT_TXSHIFT16;
		sil_wrw_iop((void*)&sc->tse_mac_base->TX_CMD_STAT, temp_reg);

		/* MAC �� 16 �r�b�g�V�t�g�I�v�V�������T�|�[�g���邩�̊m�F */
		if (sil_rew_iop((void*)&sc->tse_mac_base->TX_CMD_STAT) &
		    TSEMAC_TX_CMD_STAT_TXSHIFT16) {
			syslog(LOG_ERROR, "[tse_mac_init] Error: Incompactible %d value with TX_CMD_STAT register return TxShift16 value.", IF_ETHER_NIC_HDR_ALIGN);
			return ENP_LOGIC;
			}

		/* 16 �r�b�g�Ɏ�M�t���[���̊J�n�A�h���X���A���C�������g���� RX_SHIFT16 �̃C�l�[�u�� */
		temp_reg = sil_rew_iop((void*)&sc->tse_mac_base->RX_CMD_STAT) & ~TSEMAC_RX_CMD_STAT_RXSHIFT16;
		sil_wrw_iop((void*)&sc->tse_mac_base->RX_CMD_STAT, temp_reg);

		/* RX CMD STATUS ���W�X�^�ŁAMAC �� 16 �r�b�g�V�t�g�I�v�V�������T�|�[�g���邩�̊m�F */
		if (sil_rew_iop((void*)&sc->tse_mac_base->RX_CMD_STAT) & TSEMAC_RX_CMD_STAT_RXSHIFT16) {
			syslog(LOG_ERROR, "[tse_mac_init] Error: Incompactible %d value with RX_CMD_STAT register return RxShift16 value.", IF_ETHER_NIC_HDR_ALIGN);
			return ENP_LOGIC;
			}
		}	/* if (IF_ETHER_NIC_HDR_ALIGN == 0) */

	if (IF_ETHER_NIC_HDR_ALIGN == 2) {
		sil_wrw_iop((void*)&sc->tse_mac_base->TX_CMD_STAT, TSEMAC_TX_CMD_STAT_TXSHIFT16);

		/* MAC �� 16 �r�b�g�V�t�g�I�v�V�������T�|�[�g���邩�̊m�F */
		if (!(sil_rew_iop((void*)&sc->tse_mac_base->TX_CMD_STAT) &
		      TSEMAC_TX_CMD_STAT_TXSHIFT16)) {
			syslog(LOG_ERROR, "[tse_mac_init] Error: Incompatible %d value with TX_CMD_STAT register return TxShift16 value.", IF_ETHER_NIC_HDR_ALIGN);
			return ENP_LOGIC;
			}

		/* 16 �r�b�g�Ɏ�M�t���[���̊J�n�A�h���X���A���C�������g���� RX_SHIFT16 �̃C�l�[�u�� */
		sil_wrw_iop((void*)&sc->tse_mac_base->RX_CMD_STAT, TSEMAC_RX_CMD_STAT_RXSHIFT16);

		/* RX CMD STATUS ���W�X�^�ŁAMAC �� 16 �r�b�g�V�t�g�I�v�V�������T�|�[�g���邩�̊m�F */
		if (!(sil_rew_iop((void*)&sc->tse_mac_base->RX_CMD_STAT) & TSEMAC_RX_CMD_STAT_RXSHIFT16)) {
			syslog(LOG_ERROR, "[tse_mac_init] Error: Incompatible %d value with RX_CMD_STAT register return RxShift16 value.", IF_ETHER_NIC_HDR_ALIGN);
			return ENP_LOGIC;
			}
		}	/* if (IF_ETHER_NIC_HDR_ALIGN == 2) */

	/* MAC �̃C�l�[�u�� */
	dat = TSEMAC_CMD_TX_ENA      |
	      TSEMAC_CMD_RX_ENA      |
	      TSEMAC_CMD_RX_ERR_DISC |
	      TSEMAC_CMD_TX_ADDR_INS |
	      TSEMAC_CMD_RX_ERR_DISC;	/* CRC �G���[�̃t���[���������I�ɔj�� */


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
	/* �����ȑ��x���Ԃ��ꂽ�ꍇ�A�����l�� 100 Mbps �ɐݒ� */
	else {
		dat &= ~TSEMAC_CMD_ETH_SPEED;
		dat &= ~TSEMAC_CMD_ENA_10;
		}

	/* ����d */
	if (duplex == TSE_PHY_DUPLEX_HALF) {
		dat |= TSEMAC_CMD_HD_ENA;
		}
	/* �S��d */
	else {
		dat &= ~TSEMAC_CMD_HD_ENA;
		}

	sil_wrw_iop((void*)&sc->tse_mac_base->COMMAND_CONFIG, dat);
	syslog(LOG_DEBUG, "\nMAC post-initialization: CMD_CONFIG=0x%08x",
	       sil_rew_iop((void*)&sc->tse_mac_base->COMMAND_CONFIG));

	/* MAC �A�h���X��ݒ肷��B*/
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
 *  tse_get_frame -- Ethernet �t���[������͂���B
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
	 *              t_net_buf �� 4 �I�N�e�b�g���E�ɃA���C������Ă���B
	 *
	 *  tcp_input �� udp_input �ł́A�[���w�b�_�� SDU �Ń`�F�b�N�T����
	 *  �v�Z���邪�An �� 4 �I�N�e�b�g���E�ɂȂ�悤�� SDU �̌��� 0 ��
	 *  �p�b�f�B���O����B���̕����l������ net_buf ���l�����Ȃ���΂Ȃ�Ȃ��B
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
 *  tse_xmit -- �t���[���𑗐M����B
 *
 *    ����: NIC ���荞�݋֎~��ԂŌĂяo�����ƁB
 */

static void
tse_xmit (T_IF_SOFTC *ic)
{
	T_TSE_SOFTC *sc = ic->sc;

	/* FIFO ���ɗ]�n�����邱�Ƃ��m�F���� */
	avalon_sgdma_construct_descriptor(
		&sc->desc[TSE_FIRST_TX_SGDMA_DESC_OFST],	/* �\�z�����f�B�X�N���v�^		*/
		&sc->desc[TSE_SECOND_TX_SGDMA_DESC_OFST],	/* "����" �f�B�X�N���v�^�ւ̃|�C���^	*/
		(uint32_t*)sc->tx_nbuf->buf,			/* �ŏ��̃��[�h�E�A�h���X		*/
		(uint32_t)0x0,					/* Write addr: mem-to-stream ���[�h�ł� N/A */
		sc->tx_nbuf->len,				/* �o�C�g��				*/
		1,						/* EOP �̐���				*/
		0,						/* �K��̃A�h���X����͓ǂݏo���Ȃ�	*/
		1);						/* SOP �̐���				*/

	tse_mac_aTxWrite(sc, &sc->desc[TSE_FIRST_TX_SGDMA_DESC_OFST]);

	/* ���M�^�C���A�E�g��ݒ肷��B*/
	ic->timer = TMO_IF_TSE_XMIT;
	}

/*
 *  tse_stop -- tse �l�b�g���[�N�C���^�t�F�[�X���~����B
 *
 *    ����: NIC ���荞�݋֎~��ԂŌĂяo�����ƁB
 */

static void
tse_stop (T_TSE_SOFTC *sc)
{
	int_t state;

	/* �f�o�C�X�̎�M�p�X�̃f�B�Z�[�u�� */
	state = sil_rew_iop((void*)&sc->tse_mac_base->COMMAND_CONFIG);
	sil_wrw_iop((void*)&sc->tse_mac_base->COMMAND_CONFIG, state & ~TSEMAC_CMD_RX_ENA);
	}

/*
 *  tse_init_sub -- tse �l�b�g���[�N�C���^�t�F�[�X�̏�����
 *
 *    ����: NIC ���荞�݋֎~��ԂŌĂяo�����ƁB
 */

static void
tse_init_sub (T_IF_SOFTC *ic)
{
	T_TSE_SOFTC	*sc = ic->sc;

	/* ���M�^�C���A�E�g�����Z�b�g����B*/
	ic->timer = 0;

	/* SGDMA �R���g���[���̏����� */
	avalon_sgdma_init((void*)sc->tx_sgdma_csr);
	//avalon_sgdma_init((void*)sc->rx_sgdma_csr);

	/* TSE MAC �̏����� */
	tse_mac_init(ic);
	}

/*
 *  tse_reset -- tse �l�b�g���[�N�C���^�t�F�[�X�����Z�b�g����B
 */

void
tse_reset (T_IF_SOFTC *ic)
{
	/* NIC ����̊��荞�݂��֎~����B*/
	SuspendOSInterrupts();

	NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_RESETS], 1);
	tse_stop(ic->sc);
	tse_init_sub(ic);

	/* NIC ����̊��荞�݂�������B*/
	ResumeOSInterrupts();

	/* ���M�\�C�x���g������������B*/
	SetEvent(ETHER_OUTPUT_TASK, ic->mask_txb_ready);
	}

/*
 *  tse_get_softc -- �l�b�g���[�N�C���^�t�F�[�X�̃\�t�g�E�F�A����Ԃ��B
 */

T_IF_SOFTC *
tse_get_softc (void)
{
	return &if_softc;
	}

/*
 *  tse_watchdog -- tse �l�b�g���[�N�C���^�t�F�[�X�̃��b�`�h�b�O�^�C���A�E�g
 */

void
tse_watchdog (T_IF_SOFTC *ic)
{
	NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_OUT_ERR_PACKETS], 1);
	NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_TIMEOUTS], 1);
	tse_reset(ic);
	}

/*
 *  tse_probe -- tse �l�b�g���[�N�C���^�t�F�[�X�̌��o
 */

void
tse_probe (T_IF_SOFTC *ic)
{
	T_TSE_SOFTC	*sc = ic->sc;
	int_t		ix;

	/* MAC �A�h���X��ǂݍ��ށB*/
	for (ix = 0; ix < ETHER_ADDR_LEN; ix ++)
		ic->ifaddr.lladdr[ix] = sc->mac_addr[ix];

	/* PHY �̏����� */
	tse_phy_init(sc);
	}

/*
 *  tse_init -- tse �l�b�g���[�N�C���^�t�F�[�X�̏�����
 */

void
tse_init (T_IF_SOFTC *ic)
{
	/* NIC ����̊��荞�݂��֎~����B*/
	SuspendOSInterrupts();

	/* tse_init �{�̂��Ăяo���B*/
	tse_init_sub(ic);

	/* NIC ����̊��荞�݂�������B*/
	ResumeOSInterrupts();
	}

/*
 *  tse_read -- �t���[���̓ǂݍ���
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

	/* NIC ����̊��荞�݂��֎~����B*/
	SuspendOSInterrupts();

	/* status �r�b�g���f�B�X�N���v�^����l�� */
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
		}	/* if (�f�B�X�N���v�^�ɃG���[���Ȃ��ꍇ) */
	else {
		syslog(LOG_ERROR, "RX descriptor reported error. packet dropped");

		/* ��M�G���[�ƃ��Z�b�g���L�^����B*/
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_IN_ERR_PACKETS], 1);
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_RESETS], 1);
		tse_stop(sc);
		tse_init_sub(ic);

		/* NIC ����̊��荞�݂�������B*/
		ResumeOSInterrupts();

		/* ���M�\�C�x���g������������B*/
		SetEvent(ETHER_OUTPUT_TASK, ic->mask_txb_ready);

		return NULL;
		}

	/* �ȑO�ɍ\�z���� SGDMA �o�b�t�@�𒼐ڍė��p */
	sil_wrw_iop((void*)&currdescriptor_ptr->write_addr,
	            (uint32_t)sc->rx_nbuf->buf);

	/* �Ō�̃f�B�X�N���v�^�E���[�h�̓��e�̘A�� */
	sil_wrb_iop((void*)&currdescriptor_ptr->control,
	            AVALON_SGDMA_DESCRIPTOR_CONTROL_OWNED_BY_HW |
	            AVALON_SGDMA_DESCRIPTOR_CONTROL_GENERATE_EOP);

	/* SGDMA �̍ĊJ */
	avalon_sgdma_do_async_transfer(
		sc->rx_sgdma_csr,
		&sc->desc[TSE_FIRST_RX_SGDMA_DESC_OFST]);

	/* NIC ����̊��荞�݂�������B*/
	ResumeOSInterrupts();

	return input;
	}

/*
 *  tse_start -- ���M�t���[�����o�b�t�@�����O����B
 */

void
tse_start (T_IF_SOFTC *ic, T_NET_BUF *output)
{
	T_TSE_SOFTC *sc = ic->sc;

	/* NIC ����̊��荞�݂��֎~����B*/
	SuspendOSInterrupts();

	sc->tx_nbuf = output;

	/* DMA ��p�������M FIFO �ւ̃f�[�^�������� */
	tse_xmit(ic);

	NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_OUT_PACKETS], 1);
	NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_OUT_OCTETS],  output->len);

	/* NIC ����̊��荞�݂�������B*/
	ResumeOSInterrupts();
	}

/*
 *  ���M�����݃n���h��
 */

ISR(if_tse_tx_handler)
{
	T_TSE_SOFTC	*sc;
	T_IF_SOFTC	*ic;
	uint32_t	ready;

	ic = &if_softc;
	sc = ic->sc;

	/* �ۗ���Ԃ̊��荞�݂̃N���A */
	sil_wrw_iop((void*)&sc->tx_sgdma_csr->control,
	            sil_rew_iop((void*)&sc->tx_sgdma_csr->control)
	            | AVALON_SGDMA_CONTROL_CLEAR_INTERRUPT);

	/* ISR ���Ԃ�O�� IRQ �̖������m���Ƃ��邽�߂̃_�~�[�ǂݍ��� */
	sil_rew_iop((void*)&sc->tx_sgdma_csr->control);

	ready = tse_sgdma_tx_completions(sc);

	/* ���M�����o�b�t�@�̊J�� */
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

	/* ���M�^�C���A�E�g�����Z�b�g����B*/
	ic->timer = 0;

	if (SetEvent(ETHER_OUTPUT_TASK, ic->mask_txb_ready) != E_OK)
		NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_TXB_QOVRS], 1);
	}

/*
 *  ��M�����݃n���h��
 */

ISR(if_tse_rx_handler)
{
	T_TSE_SOFTC	*sc;
	T_IF_SOFTC	*ic;
	uint8_t		sgdma_status;

	ic = &if_softc;
	sc = ic->sc;

	/* �ۗ���Ԃ̊��荞�݂̃N���A */
	sil_wrw_iop((void*)&sc->rx_sgdma_csr->control,
	            sil_rew_iop((void*)&sc->rx_sgdma_csr->control)
	            | AVALON_SGDMA_CONTROL_CLEAR_INTERRUPT);

	/* ISR ���Ԃ�O�� IRQ �̖������m���Ƃ��邽�߂̃_�~�[�ǂݍ��� */
	sil_rew_iop((void*)&sc->rx_sgdma_csr->control);

	/* ���s���Ԃ̌v���J�n */
//	if (histid <= TNUM_HIST) {
//		begin_measure(histid);
//		meas_flg = TRUE;
//		}
//	if (histid <= TNUM_HIST && meas_flg == TRUE) {
//		end_measure(histid++);
//		meas_flg = FALSE;
//		}

	/* ���荞�ݗv�������؂��邽�߂� SGDMA status �̊l�� */
	sgdma_status = sil_rew_iop((void*)&sc->rx_sgdma_csr->status);

	if (sgdma_status & (AVALON_SGDMA_STATUS_CHAIN_COMPLETED |
	                    AVALON_SGDMA_STATUS_DESC_COMPLETED)) {
		/* ��M���荞�ݏ��� */
		if (SetEvent(ETHER_INPUT_TASK, ic->mask_rxb_ready) != E_OK)
			NET_COUNT_ETHER_NIC(net_count_ether_nic[NC_ETHER_NIC_RXB_QOVRS], 1);
		}	/* if (�L���� SGDMA ���荞��) */
	}
