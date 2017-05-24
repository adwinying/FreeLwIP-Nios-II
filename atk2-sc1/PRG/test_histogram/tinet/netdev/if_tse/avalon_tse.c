#ifdef TARGET_KERNEL_ATK2

#include "Os.h"
#include "prc_sil.h"
#include "t_syslog.h"

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

#include <tinet_defs.h>
#include <tinet_config.h>
#include <tinet_atk2_cfg.h>

#include "avalon_tse.h"

#include "avalon_sgdma_regs.h"

/*
 *  局所変数
 */

static int32_t tse_phy_rd_mdio_addr (T_TSE_MAC_INFO *pmac_info);
static int32_t tse_phy_wr_mdio_addr (T_TSE_MAC_INFO *pmac_info, uint8_t mdio_address);
static int32_t tse_phy_wr_mdio_reg (T_TSE_MAC_INFO *pmac_info, uint8_t reg_num, uint8_t lsb_num, uint8_t bit_length, uint16_t data);
static uint32_t tse_phy_rd_mdio_reg (T_TSE_MAC_INFO *pmac_info, uint8_t reg_num, uint8_t lsb_num, uint8_t bit_length);
static int32_t tse_mac_init (T_TSE_MAC_INFO *pmac_info);
static int32_t tse_mac_get_phy (T_TSE_MAC_INFO *pmac_info);
static int32_t tse_phy_restart_an (T_TSE_MAC_INFO *pmac_info, uint32_t timeout_threshold);
static int32_t tse_phy_check_link (T_TSE_MAC_INFO *pmac_info, uint32_t timeout_threshold);
static int32_t tse_phy_set_adv_1000 (T_TSE_MAC_INFO *pmac_info, uint8_t enable);
static int32_t tse_phy_set_adv_100 (T_TSE_MAC_INFO *pmac_info, uint8_t enable);
static int32_t tse_phy_set_adv_10 (T_TSE_MAC_INFO *pmac_info, uint8_t enable);

/*
 *  tse_mac_aTxWrite -- バッファメモリから送信 FIFO への非同期 SGDMA コピー
 */

int32_t
tse_mac_aTxWrite (T_TSE_MAC_INFO *pmac_info, T_SGDMA_DESCRIPTOR *txDesc)
{
	int32_t timeout;

	uint8_t result = 0;

	/* SGDMA コントローラがビジーでないことを確かめる */
	timeout = 0;

	while (sil_rew_iop((void*)&pmac_info->tx_sgdma_csr->status) &
	       AVALON_SGDMA_STATUS_BUSY) {
		if (timeout ++ == TSE_SGDMA_BUSY_TIME_OUT_CNT) {
			syslog(LOG_WARNING, "WARNING : TX SGDMA Timeout");
			return ENP_RESOURCE;		/* ここで立ち往生するのを防ぐ */
			}
		}

	/*
	 *  SGDMA の設定
	 *  SGDMA ディスクリプタの status と control ビットをクリア
	 */

	sil_wrw_iop((void*)&pmac_info->tx_sgdma_csr->control, 0);
	sil_wrw_iop((void*)&pmac_info->tx_sgdma_csr->status, 0xff);

	/* SGDMA の転送開始（ノンブロッキングコール）*/
	result = avalon_sgdma_do_async_transfer(
			pmac_info->tx_sgdma_csr,
			txDesc);

	if (result != 0)
		return -1;

	return SUCCESS;
	}

/*
 *  tse_mac_aRxRead -- 受信 FIFO からバッファメモリ領域への非同期 SGDMA コピー
 */

int32_t
tse_mac_aRxRead (T_TSE_MAC_INFO *pmac_info, T_SGDMA_DESCRIPTOR *rxDesc)
{
	int32_t timeout;

	uint8_t result = 0;

	/* SGDMA コントローラがビジーでないことを確かめる */
	timeout = 0;

	while (sil_rew_iop((void*)&pmac_info->rx_sgdma_csr->status) &
	       AVALON_SGDMA_STATUS_BUSY) {
		if (timeout ++ == TSE_SGDMA_BUSY_TIME_OUT_CNT) {
			syslog(LOG_WARNING, "WARNING : RX SGDMA Timeout");
			return ENP_RESOURCE;		/* ここで立ち往生するのを防ぐ */
			}
		}


	/* 受信用に呼び出された SGDMA の操作（ノンブロッキングコール）*/
	result = avalon_sgdma_do_async_transfer(
			pmac_info->rx_sgdma_csr,
			rxDesc);

	if (result != 0)
		return -1;

	return SUCCESS;
	}

/*
 *  getPHYSpeed -- PHY のリンク速度の決定
 */

#define TSE_SPEED_DUPLEX(speed,duplex)	\
	((duplex & 0x01)                                | \
	 (((speed == TSE_PHY_SPEED_1000) ? 1 : 0) << 1) | \
	 (((speed == TSE_PHY_SPEED_100) ? 1 : 0) << 2)  | \
	 (((speed == TSE_PHY_SPEED_10) ? 1 : 0) << 3)   | \
	 ((speed == TSE_PHY_SPEED_INVALID) ? TSE_E_INVALID_SPEED : 0))

int32_t
getPHYSpeed (T_TSE_MAC_INFO *pmac_info)
{
	uint8_t speed = TSE_MAC_SPEED_DEFAULT;
	uint8_t duplex = TSE_DUPLEX_MODE_DEFAULT;	/* 1 = full ; 0 = half */
	int32_t result = TSE_SPEED_DUPLEX(speed, duplex);

	if (pmac_info == 0) {
		speed = TSE_MAC_SPEED_DEFAULT;
		duplex = TSE_DUPLEX_MODE_DEFAULT;
		result = TSE_SPEED_DUPLEX(speed, duplex) | TSE_E_NO_PMAC_FOUND;
		syslog(LOG_ERROR, "ERROR   : [getPHYSpeed] pmac not found! Speed = %s Mbps, Duplex = %s",
		       speed == TSE_PHY_SPEED_1000 ? "1000" :
		       speed == TSE_PHY_SPEED_100 ? "100" :
		       speed == TSE_PHY_SPEED_10 ? "10" : "Unknown",
		       duplex == 1 ? "Full" : "Half");
		syslog(LOG_ERROR, "ERROR   : [getPHYSpeed] Please define correctly");
		return result;
		}

	/* "warning: unused variable" メッセージの回避 */
	T_TSE_MAC *pmac = (T_TSE_MAC*)pmac_info->tse_mac_base;

	/* Small MAC */
	if (pmac_info->mac_type == TSE_MACLITE_10_100) {
		tse_phy_set_adv_1000(pmac_info, 0);
		tse_phy_restart_an(pmac_info, AUTONEG_TIMEOUT_THRESHOLD);
		}
	else if (pmac_info->mac_type == TSE_MACLITE_1000) {
		tse_phy_set_adv_100(pmac_info, 0);
		tse_phy_set_adv_10(pmac_info, 0);
		tse_phy_restart_an(pmac_info, AUTONEG_TIMEOUT_THRESHOLD);
		}

	/* PHY のリンク同期のチェック */
	if (tse_phy_check_link(pmac_info, AUTONEG_TIMEOUT_THRESHOLD) == TSE_PHY_AN_NOT_COMPLETE) {
		speed = TSE_MAC_SPEED_DEFAULT;
		duplex = TSE_DUPLEX_MODE_DEFAULT;
		result = TSE_SPEED_DUPLEX(speed, duplex) | TSE_E_AN_NOT_COMPLETE;
		syslog(LOG_WARNING, "WARNING : PHY - Auto-Negotiation not completed! Speed = %s, Duplex = %s",
		       speed == TSE_PHY_SPEED_1000 ? "1000" :
		       speed == TSE_PHY_SPEED_100 ? "100" :
		       speed == TSE_PHY_SPEED_10 ? "10" : "Unknown",
		       duplex == 1 ? "Full" : "Half");
		return result;
		}

	sil_wrw_iop((void*)&pmac->MDIO_ADDR1, pmac_info->mdio_address);

	/* PHY ループバックのディセーブル */
	tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_CONTROL, TSE_PHY_MDIO_CONTROL_LOOPBACK, 1, 0);	/* PHY ループバックのディセーブル */

	/* PHY プロファイル内に PHY が見つからない場合 */
	if (pmac_info->pphy_profile == 0) {
		syslog(LOG_WARNING, "WARNING : PHY - PHY not found in PHY profile");
		speed = TSE_MAC_SPEED_DEFAULT;
		duplex = TSE_DUPLEX_MODE_DEFAULT;
		result = TSE_SPEED_DUPLEX(speed, duplex) | TSE_E_NO_PHY_PROFILE;
		}
	/* PHY の動作速度およびモードの取得 */
	else {
		if (pmac_info->pphy_profile->link_status_read) {
			result = pmac_info->pphy_profile->link_status_read(pmac);
			speed = (result & 0x02) ? TSE_PHY_SPEED_1000 :
			        (result & 0x04) ? TSE_PHY_SPEED_100 :
			        (result & 0x08) ? TSE_PHY_SPEED_10 : TSE_PHY_SPEED_INVALID;
			duplex = (result & 0x01) ? TSE_PHY_DUPLEX_FULL : TSE_PHY_DUPLEX_HALF;

			if (result & TSE_E_INVALID_SPEED) {
				syslog(LOG_WARNING, "WARNING : PHY - Invalid speed read from PHY");
				}
			}
		else if (pmac_info->pphy_profile->status_reg_location == 0) {
			syslog(LOG_WARNING, "WARNING : PHY - PHY Specific Status register information not provided in profile");
			speed = TSE_MAC_SPEED_DEFAULT;
			duplex = TSE_DUPLEX_MODE_DEFAULT;
			result = TSE_SPEED_DUPLEX(speed, duplex) | TSE_E_PROFILE_INCORRECT_DEFINED;
			}
		else {
			/* PHY の動作速度およびモードの設定 */
			speed = tse_phy_rd_mdio_reg(pmac_info, pmac_info->pphy_profile->status_reg_location, pmac_info->pphy_profile->speed_lsb_location, 2);
			duplex = tse_phy_rd_mdio_reg(pmac_info, pmac_info->pphy_profile->status_reg_location, pmac_info->pphy_profile->duplex_bit_location, 1);

			result = TSE_SPEED_DUPLEX(speed, duplex);
			}
		}

	syslog(LOG_DEBUG, "INFO    : PHY - Speed = %s, Duplex = %s",
	       speed == TSE_PHY_SPEED_1000 ? "1000" :
	       speed == TSE_PHY_SPEED_100 ? "100" :
	       speed == TSE_PHY_SPEED_10 ? "10" : "Unknown",
	       duplex == 1 ? "Full" : "Half");

	return result;
	}

/*
 *  tse_phy_init -- 接続された PHY の初期化
 */

int32_t
tse_phy_init (T_TSE_MAC_INFO *pmac_info)
{
	tse_mac_init(pmac_info);

	/*
	 *  TSE MAC の初期化
	 *    ・接続された PHY の取得
	 */
	tse_mac_get_phy(pmac_info);

	return SUCCESS;
	}

/*
 *  tse_phy_rd_mdio_addr -- MDIO レジスタからの PHY アドレスの読み出し
 */

static int32_t
tse_phy_rd_mdio_addr (T_TSE_MAC_INFO *pmac_info)
{
	T_TSE_MAC *pmac = (T_TSE_MAC*)pmac_info->tse_mac_base;
	return sil_rew_iop((void*)&pmac->MDIO_ADDR1);
	}

/*
 *  tse_phy_wr_mdio_addr -- MDIO レジスタへの PHY アドレスの書き込み
 */

static int32_t
tse_phy_wr_mdio_addr (T_TSE_MAC_INFO *pmac_info, uint8_t mdio_address)
{
	T_TSE_MAC *pmac = (T_TSE_MAC*)pmac_info->tse_mac_base;
	sil_wrw_iop((void*)&pmac->MDIO_ADDR1, mdio_address);

	return SUCCESS;
	}

/*
 *  tse_phy_wr_mdio_reg -- MDIO レジスタへの書き込み
 */

static int32_t
tse_phy_wr_mdio_reg (T_TSE_MAC_INFO *pmac_info, uint8_t reg_num, uint8_t lsb_num, uint8_t bit_length, uint16_t data)
{
	uint16_t	temp_data;
	uint16_t	bit_mask;
	int32_t		i;
	T_TSE_MAC	*pmac = (T_TSE_MAC*)pmac_info->tse_mac_base;

	bit_mask = 0x00;

	/*
	 *  bit_length 数の 1 で構成されるマスクの生成
	 *    ・例：bit_length = 3, bit_mask = 0b0000 0000 0000 0111
	 */
	for (i = 0; i < bit_length; i ++) {
		bit_mask <<= 1;
		bit_mask |= 0x01;
		}

	/* bit_num だけマスクを左シフト */
	bit_mask <<= lsb_num;

	/* レジスタの読み出し */
	temp_data = sil_rew_iop((void*)((uint32_t*)&pmac->mdio1 + reg_num));

	/* 書き込まれたビットのクリア */
	temp_data &= ~bit_mask;

	/* 対応するビットの論理和 */
	temp_data |= ((data << lsb_num) & bit_mask);

	/* MDIO レジスタへの書き込み */
	sil_wrw_iop((void*)((uint32_t*)&pmac->mdio1 + reg_num), temp_data);

	return SUCCESS;
	}

/*
 *  tse_phy_rd_mdio_reg -- MDIO レジスタからの bit_length 数のビットの読み出し
 */

static uint32_t
tse_phy_rd_mdio_reg (T_TSE_MAC_INFO *pmac_info, uint8_t reg_num, uint8_t lsb_num, uint8_t bit_length)
{
	uint16_t	temp_data;
	uint32_t	bit_mask;
	int32_t		i;
	T_TSE_MAC	*pmac = (T_TSE_MAC*)pmac_info->tse_mac_base;

	bit_mask = 0x00;

	/*
	 *  bit_length 数の 1 で構成されるマスクの生成
	 *    ・例：bit_length = 3, bit_mask = 0b0000 0000 0000 0111
	 */
	for (i = 0; i < bit_length; i ++) {
		bit_mask <<= 1;
		bit_mask |= 0x01;
		}

	/* レジスタの読み出し */
	temp_data = sil_rew_iop((void*)((uint32_t*)&pmac->mdio1 + reg_num));

	/* データのシフト */
	temp_data >>= lsb_num;

	return (temp_data & bit_mask);
	}

/*
 *  tse_mac_init -- システムで利用可能な MAC の情報の格納
 */

static int32_t
tse_mac_init (T_TSE_MAC_INFO *pmac_info)
{
	/* T_TSE_MAC_INFO が正しく定義されているかの確認 */
	syslog(LOG_DEBUG, "INFO    : pmac_info->tx_sgdma_csr = %s", pmac_info->tx_sgdma_csr);
	syslog(LOG_DEBUG, "INFO    : pmac_info->rx_sgdma_csr = %s", pmac_info->rx_sgdma_csr);
	if ((pmac_info->tx_sgdma_csr == 0) || (pmac_info->rx_sgdma_csr == 0)) {
		syslog(LOG_ERROR, "ERROR   : MAC does not defined correctly!");
		return TSE_SYSTEM_DEF_ERROR;
		}

	switch (pmac_info->mac_type) {
	case TSE_MACLITE_1000:
		syslog(LOG_DEBUG, "INFO    : MAC Type                 = %s", "1000 Mbps Small MAC");
		break;
	case TSE_MACLITE_10_100:
		syslog(LOG_DEBUG, "INFO    : MAC Type                 = %s", "10/100 Mbps Small MAC");
		break;
	case TSE_FULL_MAC:
		syslog(LOG_DEBUG, "INFO    : MAC Type                 = %s", "10/100/1000 Ethernet MAC");
		break;
	default:
		syslog(LOG_DEBUG, "INFO    : MAC Type                 = %s", "Unknown");
		break;
		}

	syslog(LOG_DEBUG, "INFO    : MAC Address              = 0x%08x", (int_t)pmac_info->tse_mac_base);

	return SUCCESS;
	}

/*
 *  tse_mac_get_phy -- MAC に接続された PHY の情報の格納
 */

static int32_t
tse_mac_get_phy (T_TSE_MAC_INFO *pmac_info)
{
	int32_t		phyid1;
	int32_t		phyid2;

	uint32_t	oui;
	uint8_t		model_number;
	uint8_t		revision_number;

	int32_t		return_value = 0;

	int8_t		phy_count = 0;

	T_TSE_MAC	*pmac = (T_TSE_MAC*)pmac_info->tse_mac_base;

	/* MDIO アドレスは関数の最後に復元 */
	int32_t		mdioadd_prev = sil_rew_iop((void*)&pmac->MDIO_ADDR1);

	sil_wrw_iop((void*)&pmac->MDIO_ADDR1, pmac_info->mdio_address);
	phyid1 = sil_rew_iop((void*)&pmac->mdio1.PHY_ID1);	/* PHY ID の読み出し */
	phyid2 = sil_rew_iop((void*)&pmac->mdio1.PHY_ID2);	/* PHY ID の読み出し */

	/* PHY の検出 */
	if (phyid1 != phyid2) {
		/* PHYID1 と PHYID2 から OUI、モデルナンバー、リビジョンナンバーの取得 */
		oui = (phyid1 << 6) | ((phyid2 >> 10) & 0x3f);
		model_number = (phyid2 >> 4) & 0x3f;
		revision_number = phyid2 & 0x0f;

		syslog(LOG_DEBUG, "INFO    : PHY OUI             =  0x%06x", (int_t)oui);
		syslog(LOG_DEBUG, "INFO    : PHY Model Number    =  0x%02x", model_number);
		syslog(LOG_DEBUG, "INFO    : PHY Revision Number =  0x%01x", revision_number);

		/*
		 *  オート・ネゴシエーションを完了可能にするため、
		 *  PHY ループバックをディセーブル
		 */
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_CONTROL, TSE_PHY_MDIO_CONTROL_LOOPBACK, 1, 0);	/* PHY ループバックのディセーブル */

		/* オート・ネゴシエーションアドバタイズメントのリセット */
		tse_phy_set_adv_1000(pmac_info, 1);
		tse_phy_set_adv_100(pmac_info, 1);
		tse_phy_set_adv_10(pmac_info, 1);

		/* PHY のリンク接続の確認 */
		tse_phy_restart_an(pmac_info, CHECKLINK_TIMEOUT_THRESHOLD);

		/* プロファイル固有の追加の設定を行う */
		if (pmac_info->pphy_profile) {
			if (pmac_info->pphy_profile->phy_cfg) {
				syslog(LOG_DEBUG, "INFO    : Applying additional PHY configuration of %s", pmac_info->pphy_profile->name);
				pmac_info->pphy_profile->phy_cfg(pmac);
				}
			}

		/* PHY の初期化、個々の PHY 固有の T_TSE_MAC_INFO 内のユーザ関数ポインタの実行 */
		if (pmac_info->tse_phy_cfg) {
			syslog(LOG_DEBUG, "INFO    : Applying additional user PHY configuration");
			pmac_info->tse_phy_cfg(pmac);
			}

		syslog(LOG_DEBUG, "");

		phy_count ++;
		}

	/* 接続された PHY の数の確認 */
	if (phy_count == 0) {
		syslog(LOG_ERROR, "ERROR   : MAC - No PHY connected!");
		}

	/* MDIO アドレスの復元 */
	sil_wrw_iop((void*)&pmac->MDIO_ADDR1, mdioadd_prev);

	return return_value;
	}

/*
 *  tse_phy_restart_an -- PHY のオート・ネゴシエーションの再開
 */

static int32_t
tse_phy_restart_an (T_TSE_MAC_INFO *pmac_info, uint32_t timeout_threshold)
{
	/* MDIO アドレスは関数の最後に復元 */
	uint8_t mdioadd_prev = tse_phy_rd_mdio_addr(pmac_info);

	/*
	 *  i 番目の PHY にアクセスするため
	 *  MDIO へ PHY アドレスを書き込む
	 */
	tse_phy_wr_mdio_addr(pmac_info, pmac_info->mdio_address);

	if (!tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_AN_ABILITY, 1)) {
		syslog(LOG_WARNING, "WARNING : PHY - PHY not capable for Auto-Negotiation");

		/* MDIO アドレスの復元 */
		tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

		return TSE_PHY_AN_NOT_CAPABLE;
		}

	/* オート・ネゴシエーションのイネーブル */
	tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_CONTROL, TSE_PHY_MDIO_CONTROL_AN_ENA, 1, 1);

	/* PHY へリセットコマンド送信 */
	tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_CONTROL, TSE_PHY_MDIO_CONTROL_RESTART_AN, 1, 1);
	syslog(LOG_DEBUG, "INFO    : PHY - Restart Auto-Negotiation, checking PHY link...");

	int32_t timeout = 0;
	while (tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_AN_COMPLETE, 1) == 0) {
		if (timeout ++ > timeout_threshold) {
			syslog(LOG_WARNING, "WARNING : PHY - Auto-Negotiation FAILED");

			/* MDIO アドレスの復元 */
			tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

			return TSE_PHY_AN_NOT_COMPLETE;
			}
		}
	syslog(LOG_DEBUG, "INFO    : PHY - Auto-Negotiation PASSED");

	/* MDIO アドレスの復元 */
	tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

	return TSE_PHY_AN_COMPLETE;
	}

/*
 *  tse_phy_check_link -- PHY のリンク状態の確認
 *                        完了していない場合、オート・ネゴシエーションの開始
 */

static int32_t
tse_phy_check_link (T_TSE_MAC_INFO *pmac_info, uint32_t timeout_threshold)
{
	/* MDIO アドレスは関数の最後に復元 */
	uint8_t mdioadd_prev = tse_phy_rd_mdio_addr(pmac_info);

	/*
	 *  i 番目の PHY にアクセスするため
	 *  MDIO へ PHY アドレスを書き込む
	 */
	tse_phy_wr_mdio_addr(pmac_info, pmac_info->mdio_address);

	/*
	 *  オート・ネゴシエーションが完了していない場合
	 *  オート・ネゴシエーションを再開
	 */
	syslog(LOG_DEBUG, "INFO    : PHY - Checking link...");
	if ((tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_CONTROL, TSE_PHY_MDIO_CONTROL_LOOPBACK, 1) != 0) ||
	    (tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_AN_COMPLETE, 1) == 0)) {

		syslog(LOG_DEBUG, "INFO    : PHY - Link not yet established, restart auto-negotiation...");
		/* オート・ネゴシエーションの再開 */
		/* オート・ネゴシエーションが完了しない場合、次の PHY に進む */
		if (tse_phy_restart_an(pmac_info, timeout_threshold) == TSE_PHY_AN_NOT_COMPLETE) {
			syslog(LOG_WARNING, "WARNING : PHY - Link could not established");

			/* MDIO アドレスの復元 */
			tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

			return TSE_PHY_AN_NOT_COMPLETE;
			}
		}
	syslog(LOG_DEBUG, "INFO    : PHY - Link established");

	/* MDIO アドレスの復元 */
	tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

	return TSE_PHY_AN_COMPLETE;
	}

/*
 *  tse_phy_set_adv_1000 -- PHY のアドバタイズメントを 1000 Mbps に設定
 */

static int32_t
tse_phy_set_adv_1000 (T_TSE_MAC_INFO *pmac_info, uint8_t enable)
{
	uint8_t cap;

	/* MDIO アドレスは関数の最後に復元 */
	uint8_t mdioadd_prev = tse_phy_rd_mdio_addr(pmac_info);

	/*
	 *  i 番目の PHY にアクセスするため
	 *  MDIO へ PHY アドレスを書き込む
	 */
	tse_phy_wr_mdio_addr(pmac_info, pmac_info->mdio_address);

	/* enable = 1 の場合、PHY の機能に基づきアドバタイズメントを設定 */
	if (enable) {
		cap = tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_EXT_STATUS, TSE_PHY_MDIO_EXT_STATUS_1000BASE_T_FULL, 1);
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_1000BASE_T_CTRL, TSE_PHY_MDIO_1000BASE_T_CTRL_FULL_ADV, 1, cap);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 1000 Base-T Full Duplex set to %d", cap);

		/* TSE MAC では 1000 Mbps 半二重モードはサポートされない */
		cap = tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_EXT_STATUS, TSE_PHY_MDIO_EXT_STATUS_1000BASE_T_HALF, 1);
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_1000BASE_T_CTRL, TSE_PHY_MDIO_1000BASE_T_CTRL_HALF_ADV, 1, cap);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 1000 Base-T Half Duplex set to %d", cap);
		}
	/* この速度のアドバタイズメントのディセーブル */
	else {
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_1000BASE_T_CTRL, TSE_PHY_MDIO_1000BASE_T_CTRL_FULL_ADV, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 1000 Base-T Full Duplex set to %d", 0);

		/* TSE MAC では 1000 Mbps 半二重モードはサポートされない */
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_1000BASE_T_CTRL, TSE_PHY_MDIO_1000BASE_T_CTRL_HALF_ADV, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement 1000 Base-T half Duplex set to %d", 0);
		}

	/* MDIO アドレスの復元 */
	tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

	return SUCCESS;
	}

/*
 *  tse_phy_set_adv_100 -- PHY のアドバタイズメントを 100 Mbps に設定
 */

static int32_t
tse_phy_set_adv_100 (T_TSE_MAC_INFO *pmac_info, uint8_t enable)
{
	uint8_t cap;

	/* MDIO アドレスは関数の最後に復元 */
	uint8_t mdioadd_prev = tse_phy_rd_mdio_addr(pmac_info);

	/*
	 *  i 番目の PHY にアクセスするため
	 *  MDIO へ PHY アドレスを書き込む
	 */
	tse_phy_wr_mdio_addr(pmac_info, pmac_info->mdio_address);

	/* enable = 1 の場合、PHY の機能に基づきアドバタイズメントを設定 */
	if (enable) {
		cap = tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_100BASE_T4, 1);
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_100BASE_T4, 1, cap);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 100 Base-T4 set to %d", cap);

		cap = tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_100BASE_X_FULL, 1);
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_100BASE_TX_FULL, 1, cap);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 100 Base-TX Full Duplex set to %d", cap);

		cap = tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_100BASE_X_HALF, 1);
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_100BASE_TX_HALF, 1, cap);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 100 Base-TX Half Duplex set to %d", cap);
		}
	/* この速度のアドバタイズメントのディセーブル */
	else {
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_100BASE_T4, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 100 Base-T4 set to %d", 0);

		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_100BASE_TX_FULL, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 100 Base-TX Full Duplex set to %d", 0);

		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_100BASE_TX_HALF, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 100 Base-TX Half Duplex set to %d", 0);
		}

	/* MDIO アドレスの復元 */
	tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

	return SUCCESS;
	}

/*
 *  tse_phy_set_adv_10 -- PHY のアドバタイズメントを 10 Mbps に設定
 */

static int32_t
tse_phy_set_adv_10 (T_TSE_MAC_INFO *pmac_info, uint8_t enable)
{
	uint8_t cap;

	/* MDIO アドレスは関数の最後に復元 */
	uint8_t mdioadd_prev = tse_phy_rd_mdio_addr(pmac_info);

	/*
	 *  i 番目の PHY にアクセスするため
	 *  MDIO へ PHY アドレスを書き込む
	 */
	tse_phy_wr_mdio_addr(pmac_info, pmac_info->mdio_address);

	/* enable = 1 の場合、PHY の機能に基づきアドバタイズメントを設定 */
	if (enable) {
		cap = tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_10BASE_T_FULL, 1);
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_10BASE_TX_FULL, 1, cap);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 10 Base-TX Full Duplex set to %d", cap);

		cap = tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_10BASE_T_HALF, 1);
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_10BASE_TX_HALF, 1, cap);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 10 Base-TX Half Duplex set to %d", cap);
		}
	/* この速度のアドバタイズメントのディセーブル */
	else {
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_10BASE_TX_FULL, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 10 Base-TX Full Duplex set to %d", 0);

		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_10BASE_TX_HALF, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 10 Base-TX Half Duplex set to %d", 0);
		}

	/* MDIO アドレスの復元 */
	tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

	return SUCCESS;
	}

/*
 *  marvell_phy_cfg -- Marvell PHY の追加の設定
 */

static int32_t
marvell_phy_cfg (T_TSE_MAC *pmac)
{
	/* リンクがまだない場合、オートクロスオーバーをイネーブルし、PHY をリセット */
	if ((sil_rew_iop((void*)&pmac->mdio1.STATUS) & PCS_ST_an_done) == 0) {
		syslog(LOG_DEBUG, "MARVELL : Enabling auto crossover");
		sil_wrw_iop((void*)((uint32_t*)&pmac->mdio1 + TSE_PHY_MDIO_MARVELL_CONTROL),
		            MARV_CTL_dis_125clk | MARV_CTL_mac_pow_up | MARV_CTL_ena_auto);
		syslog(LOG_DEBUG, "MARVELL : PHY reset");
		sil_wrw_iop((void*)&pmac->mdio1.CONTROL,
		            sil_rew_iop((void*)&pmac->mdio1.CONTROL)
		            | PCS_CTL_sw_reset);
		}

	return 0;
	}

/*
 *  marvell_cfg_rgmii -- Marvell PHY のモードを RGMII に変更
 */

int32_t
marvell_cfg_rgmii (T_TSE_MAC *pmac)
{
	uint16_t dat = sil_rew_iop((void*)((uint32_t*)&pmac->mdio1 + TSE_PHY_MDIO_MARVELL_EXT_STATUS));
	dat &= ~MARV_EXT_ST_mode_mask;

	syslog(LOG_DEBUG, "MARVELL : Mode changed to RGMII/Modified MII to Copper mode");
	sil_wrw_iop((void*)((uint32_t*)&pmac->mdio1 + TSE_PHY_MDIO_MARVELL_EXT_STATUS), dat | MARV_EXT_ST_mode_copper_rgmii);

	syslog(LOG_DEBUG, "MARVELL : Enable RGMII Timing Control");
	dat = sil_rew_iop((void*)((uint32_t*)&pmac->mdio1 + TSE_PHY_MDIO_MARVELL_EXT_CONTROL));
	dat &= ~(MARV_EXT_CTL_rx_tim_ct | MARV_EXT_CTL_tx_tim_ct);
	dat |= MARV_EXT_CTL_rx_tim_ct | MARV_EXT_CTL_tx_tim_ct;
	sil_wrw_iop((void*)((uint32_t*)&pmac->mdio1 + TSE_PHY_MDIO_MARVELL_EXT_CONTROL), dat);

	syslog(LOG_DEBUG, "MARVELL : PHY reset");
	sil_wrw_iop((void*)&pmac->mdio1.CONTROL,
	            sil_rew_iop((void*)&pmac->mdio1.CONTROL)
	            | PCS_CTL_sw_reset);

	return 1;
	}

/*
 *  PHY の定義
 */

T_TSE_PHY_PROFILE MV88E1111 = {
	"Marvell 88E1111",		/* Marvell 88E1111			*/
	MV88E1111_OUI,			/* OUI					*/
	MV88E1111_MODEL,		/* ベンダモデルナンバー			*/
	MV88E1111_REV,			/* モデルリビジョンナンバー		*/
	TSE_PHY_MDIO_MARVELL_STATUS,	/* Status レジスタの場所		*/
	TSE_PHY_MDIO_MARVELL_STATUS_SPEED_100, /* Speed Status の場所		*/
	TSE_PHY_MDIO_MARVELL_STATUS_FULL_DUP, /* Duplex Status の場所		*/
	TSE_PHY_MDIO_MARVELL_STATUS_LINK_UP, /* Link Status の場所		*/
	&marvell_phy_cfg		/* Marvell PHY を設定する関数ポインタ	*/
	};
