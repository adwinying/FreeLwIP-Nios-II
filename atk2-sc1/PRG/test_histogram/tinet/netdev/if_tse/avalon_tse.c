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
 *  �ɽ��ѿ�
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
 *  tse_mac_aTxWrite -- �Хåե����꤫������ FIFO �ؤ���Ʊ�� SGDMA ���ԡ�
 */

int32_t
tse_mac_aTxWrite (T_TSE_MAC_INFO *pmac_info, T_SGDMA_DESCRIPTOR *txDesc)
{
	int32_t timeout;

	uint8_t result = 0;

	/* SGDMA ����ȥ��餬�ӥ����Ǥʤ����Ȥ�Τ���� */
	timeout = 0;

	while (sil_rew_iop((void*)&pmac_info->tx_sgdma_csr->status) &
	       AVALON_SGDMA_STATUS_BUSY) {
		if (timeout ++ == TSE_SGDMA_BUSY_TIME_OUT_CNT) {
			syslog(LOG_WARNING, "WARNING : TX SGDMA Timeout");
			return ENP_RESOURCE;		/* ������Ω����������Τ��ɤ� */
			}
		}

	/*
	 *  SGDMA ������
	 *  SGDMA �ǥ�������ץ��� status �� control �ӥåȤ򥯥ꥢ
	 */

	sil_wrw_iop((void*)&pmac_info->tx_sgdma_csr->control, 0);
	sil_wrw_iop((void*)&pmac_info->tx_sgdma_csr->status, 0xff);

	/* SGDMA ��ž�����ϡʥΥ�֥�å��󥰥������*/
	result = avalon_sgdma_do_async_transfer(
			pmac_info->tx_sgdma_csr,
			txDesc);

	if (result != 0)
		return -1;

	return SUCCESS;
	}

/*
 *  tse_mac_aRxRead -- ���� FIFO ����Хåե������ΰ�ؤ���Ʊ�� SGDMA ���ԡ�
 */

int32_t
tse_mac_aRxRead (T_TSE_MAC_INFO *pmac_info, T_SGDMA_DESCRIPTOR *rxDesc)
{
	int32_t timeout;

	uint8_t result = 0;

	/* SGDMA ����ȥ��餬�ӥ����Ǥʤ����Ȥ�Τ���� */
	timeout = 0;

	while (sil_rew_iop((void*)&pmac_info->rx_sgdma_csr->status) &
	       AVALON_SGDMA_STATUS_BUSY) {
		if (timeout ++ == TSE_SGDMA_BUSY_TIME_OUT_CNT) {
			syslog(LOG_WARNING, "WARNING : RX SGDMA Timeout");
			return ENP_RESOURCE;		/* ������Ω����������Τ��ɤ� */
			}
		}


	/* �����Ѥ˸ƤӽФ��줿 SGDMA �����ʥΥ�֥�å��󥰥������*/
	result = avalon_sgdma_do_async_transfer(
			pmac_info->rx_sgdma_csr,
			rxDesc);

	if (result != 0)
		return -1;

	return SUCCESS;
	}

/*
 *  getPHYSpeed -- PHY �Υ��®�٤η���
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

	/* "warning: unused variable" ��å������β��� */
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

	/* PHY �Υ��Ʊ���Υ����å� */
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

	/* PHY �롼�ץХå��Υǥ������֥� */
	tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_CONTROL, TSE_PHY_MDIO_CONTROL_LOOPBACK, 1, 0);	/* PHY �롼�ץХå��Υǥ������֥� */

	/* PHY �ץ�ե�������� PHY �����Ĥ���ʤ���� */
	if (pmac_info->pphy_profile == 0) {
		syslog(LOG_WARNING, "WARNING : PHY - PHY not found in PHY profile");
		speed = TSE_MAC_SPEED_DEFAULT;
		duplex = TSE_DUPLEX_MODE_DEFAULT;
		result = TSE_SPEED_DUPLEX(speed, duplex) | TSE_E_NO_PHY_PROFILE;
		}
	/* PHY ��ư��®�٤���ӥ⡼�ɤμ��� */
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
			/* PHY ��ư��®�٤���ӥ⡼�ɤ����� */
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
 *  tse_phy_init -- ��³���줿 PHY �ν����
 */

int32_t
tse_phy_init (T_TSE_MAC_INFO *pmac_info)
{
	tse_mac_init(pmac_info);

	/*
	 *  TSE MAC �ν����
	 *    ����³���줿 PHY �μ���
	 */
	tse_mac_get_phy(pmac_info);

	return SUCCESS;
	}

/*
 *  tse_phy_rd_mdio_addr -- MDIO �쥸��������� PHY ���ɥ쥹���ɤ߽Ф�
 */

static int32_t
tse_phy_rd_mdio_addr (T_TSE_MAC_INFO *pmac_info)
{
	T_TSE_MAC *pmac = (T_TSE_MAC*)pmac_info->tse_mac_base;
	return sil_rew_iop((void*)&pmac->MDIO_ADDR1);
	}

/*
 *  tse_phy_wr_mdio_addr -- MDIO �쥸�����ؤ� PHY ���ɥ쥹�ν񤭹���
 */

static int32_t
tse_phy_wr_mdio_addr (T_TSE_MAC_INFO *pmac_info, uint8_t mdio_address)
{
	T_TSE_MAC *pmac = (T_TSE_MAC*)pmac_info->tse_mac_base;
	sil_wrw_iop((void*)&pmac->MDIO_ADDR1, mdio_address);

	return SUCCESS;
	}

/*
 *  tse_phy_wr_mdio_reg -- MDIO �쥸�����ؤν񤭹���
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
	 *  bit_length ���� 1 �ǹ��������ޥ���������
	 *    ���㡧bit_length = 3, bit_mask = 0b0000 0000 0000 0111
	 */
	for (i = 0; i < bit_length; i ++) {
		bit_mask <<= 1;
		bit_mask |= 0x01;
		}

	/* bit_num �����ޥ����򺸥��ե� */
	bit_mask <<= lsb_num;

	/* �쥸�������ɤ߽Ф� */
	temp_data = sil_rew_iop((void*)((uint32_t*)&pmac->mdio1 + reg_num));

	/* �񤭹��ޤ줿�ӥåȤΥ��ꥢ */
	temp_data &= ~bit_mask;

	/* �б�����ӥåȤ������� */
	temp_data |= ((data << lsb_num) & bit_mask);

	/* MDIO �쥸�����ؤν񤭹��� */
	sil_wrw_iop((void*)((uint32_t*)&pmac->mdio1 + reg_num), temp_data);

	return SUCCESS;
	}

/*
 *  tse_phy_rd_mdio_reg -- MDIO �쥸��������� bit_length ���ΥӥåȤ��ɤ߽Ф�
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
	 *  bit_length ���� 1 �ǹ��������ޥ���������
	 *    ���㡧bit_length = 3, bit_mask = 0b0000 0000 0000 0111
	 */
	for (i = 0; i < bit_length; i ++) {
		bit_mask <<= 1;
		bit_mask |= 0x01;
		}

	/* �쥸�������ɤ߽Ф� */
	temp_data = sil_rew_iop((void*)((uint32_t*)&pmac->mdio1 + reg_num));

	/* �ǡ����Υ��ե� */
	temp_data >>= lsb_num;

	return (temp_data & bit_mask);
	}

/*
 *  tse_mac_init -- �����ƥ�����Ѳ�ǽ�� MAC �ξ���γ�Ǽ
 */

static int32_t
tse_mac_init (T_TSE_MAC_INFO *pmac_info)
{
	/* T_TSE_MAC_INFO ���������������Ƥ��뤫�γ�ǧ */
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
 *  tse_mac_get_phy -- MAC ����³���줿 PHY �ξ���γ�Ǽ
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

	/* MDIO ���ɥ쥹�ϴؿ��κǸ������ */
	int32_t		mdioadd_prev = sil_rew_iop((void*)&pmac->MDIO_ADDR1);

	sil_wrw_iop((void*)&pmac->MDIO_ADDR1, pmac_info->mdio_address);
	phyid1 = sil_rew_iop((void*)&pmac->mdio1.PHY_ID1);	/* PHY ID ���ɤ߽Ф� */
	phyid2 = sil_rew_iop((void*)&pmac->mdio1.PHY_ID2);	/* PHY ID ���ɤ߽Ф� */

	/* PHY �θ��� */
	if (phyid1 != phyid2) {
		/* PHYID1 �� PHYID2 ���� OUI����ǥ�ʥ�С�����ӥ����ʥ�С��μ��� */
		oui = (phyid1 << 6) | ((phyid2 >> 10) & 0x3f);
		model_number = (phyid2 >> 4) & 0x3f;
		revision_number = phyid2 & 0x0f;

		syslog(LOG_DEBUG, "INFO    : PHY OUI             =  0x%06x", (int_t)oui);
		syslog(LOG_DEBUG, "INFO    : PHY Model Number    =  0x%02x", model_number);
		syslog(LOG_DEBUG, "INFO    : PHY Revision Number =  0x%01x", revision_number);

		/*
		 *  �����ȡ��ͥ�������������λ��ǽ�ˤ��뤿�ᡢ
		 *  PHY �롼�ץХå���ǥ������֥�
		 */
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_CONTROL, TSE_PHY_MDIO_CONTROL_LOOPBACK, 1, 0);	/* PHY �롼�ץХå��Υǥ������֥� */

		/* �����ȡ��ͥ�����������󥢥ɥХ��������ȤΥꥻ�å� */
		tse_phy_set_adv_1000(pmac_info, 1);
		tse_phy_set_adv_100(pmac_info, 1);
		tse_phy_set_adv_10(pmac_info, 1);

		/* PHY �Υ����³�γ�ǧ */
		tse_phy_restart_an(pmac_info, CHECKLINK_TIMEOUT_THRESHOLD);

		/* �ץ�ե������ͭ���ɲä������Ԥ� */
		if (pmac_info->pphy_profile) {
			if (pmac_info->pphy_profile->phy_cfg) {
				syslog(LOG_DEBUG, "INFO    : Applying additional PHY configuration of %s", pmac_info->pphy_profile->name);
				pmac_info->pphy_profile->phy_cfg(pmac);
				}
			}

		/* PHY �ν�������ġ��� PHY ��ͭ�� T_TSE_MAC_INFO ��Υ桼���ؿ��ݥ��󥿤μ¹� */
		if (pmac_info->tse_phy_cfg) {
			syslog(LOG_DEBUG, "INFO    : Applying additional user PHY configuration");
			pmac_info->tse_phy_cfg(pmac);
			}

		syslog(LOG_DEBUG, "");

		phy_count ++;
		}

	/* ��³���줿 PHY �ο��γ�ǧ */
	if (phy_count == 0) {
		syslog(LOG_ERROR, "ERROR   : MAC - No PHY connected!");
		}

	/* MDIO ���ɥ쥹������ */
	sil_wrw_iop((void*)&pmac->MDIO_ADDR1, mdioadd_prev);

	return return_value;
	}

/*
 *  tse_phy_restart_an -- PHY �Υ����ȡ��ͥ������������κƳ�
 */

static int32_t
tse_phy_restart_an (T_TSE_MAC_INFO *pmac_info, uint32_t timeout_threshold)
{
	/* MDIO ���ɥ쥹�ϴؿ��κǸ������ */
	uint8_t mdioadd_prev = tse_phy_rd_mdio_addr(pmac_info);

	/*
	 *  i ���ܤ� PHY �˥����������뤿��
	 *  MDIO �� PHY ���ɥ쥹��񤭹���
	 */
	tse_phy_wr_mdio_addr(pmac_info, pmac_info->mdio_address);

	if (!tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_AN_ABILITY, 1)) {
		syslog(LOG_WARNING, "WARNING : PHY - PHY not capable for Auto-Negotiation");

		/* MDIO ���ɥ쥹������ */
		tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

		return TSE_PHY_AN_NOT_CAPABLE;
		}

	/* �����ȡ��ͥ������������Υ��͡��֥� */
	tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_CONTROL, TSE_PHY_MDIO_CONTROL_AN_ENA, 1, 1);

	/* PHY �إꥻ�åȥ��ޥ������ */
	tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_CONTROL, TSE_PHY_MDIO_CONTROL_RESTART_AN, 1, 1);
	syslog(LOG_DEBUG, "INFO    : PHY - Restart Auto-Negotiation, checking PHY link...");

	int32_t timeout = 0;
	while (tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_AN_COMPLETE, 1) == 0) {
		if (timeout ++ > timeout_threshold) {
			syslog(LOG_WARNING, "WARNING : PHY - Auto-Negotiation FAILED");

			/* MDIO ���ɥ쥹������ */
			tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

			return TSE_PHY_AN_NOT_COMPLETE;
			}
		}
	syslog(LOG_DEBUG, "INFO    : PHY - Auto-Negotiation PASSED");

	/* MDIO ���ɥ쥹������ */
	tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

	return TSE_PHY_AN_COMPLETE;
	}

/*
 *  tse_phy_check_link -- PHY �Υ�󥯾��֤γ�ǧ
 *                        ��λ���Ƥ��ʤ���硢�����ȡ��ͥ������������γ���
 */

static int32_t
tse_phy_check_link (T_TSE_MAC_INFO *pmac_info, uint32_t timeout_threshold)
{
	/* MDIO ���ɥ쥹�ϴؿ��κǸ������ */
	uint8_t mdioadd_prev = tse_phy_rd_mdio_addr(pmac_info);

	/*
	 *  i ���ܤ� PHY �˥����������뤿��
	 *  MDIO �� PHY ���ɥ쥹��񤭹���
	 */
	tse_phy_wr_mdio_addr(pmac_info, pmac_info->mdio_address);

	/*
	 *  �����ȡ��ͥ�����������󤬴�λ���Ƥ��ʤ����
	 *  �����ȡ��ͥ�������������Ƴ�
	 */
	syslog(LOG_DEBUG, "INFO    : PHY - Checking link...");
	if ((tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_CONTROL, TSE_PHY_MDIO_CONTROL_LOOPBACK, 1) != 0) ||
	    (tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_AN_COMPLETE, 1) == 0)) {

		syslog(LOG_DEBUG, "INFO    : PHY - Link not yet established, restart auto-negotiation...");
		/* �����ȡ��ͥ������������κƳ� */
		/* �����ȡ��ͥ�����������󤬴�λ���ʤ���硢���� PHY �˿ʤ� */
		if (tse_phy_restart_an(pmac_info, timeout_threshold) == TSE_PHY_AN_NOT_COMPLETE) {
			syslog(LOG_WARNING, "WARNING : PHY - Link could not established");

			/* MDIO ���ɥ쥹������ */
			tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

			return TSE_PHY_AN_NOT_COMPLETE;
			}
		}
	syslog(LOG_DEBUG, "INFO    : PHY - Link established");

	/* MDIO ���ɥ쥹������ */
	tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

	return TSE_PHY_AN_COMPLETE;
	}

/*
 *  tse_phy_set_adv_1000 -- PHY �Υ��ɥХ��������Ȥ� 1000 Mbps ������
 */

static int32_t
tse_phy_set_adv_1000 (T_TSE_MAC_INFO *pmac_info, uint8_t enable)
{
	uint8_t cap;

	/* MDIO ���ɥ쥹�ϴؿ��κǸ������ */
	uint8_t mdioadd_prev = tse_phy_rd_mdio_addr(pmac_info);

	/*
	 *  i ���ܤ� PHY �˥����������뤿��
	 *  MDIO �� PHY ���ɥ쥹��񤭹���
	 */
	tse_phy_wr_mdio_addr(pmac_info, pmac_info->mdio_address);

	/* enable = 1 �ξ�硢PHY �ε�ǽ�˴�Ť����ɥХ��������Ȥ����� */
	if (enable) {
		cap = tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_EXT_STATUS, TSE_PHY_MDIO_EXT_STATUS_1000BASE_T_FULL, 1);
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_1000BASE_T_CTRL, TSE_PHY_MDIO_1000BASE_T_CTRL_FULL_ADV, 1, cap);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 1000 Base-T Full Duplex set to %d", cap);

		/* TSE MAC �Ǥ� 1000 Mbps Ⱦ��ť⡼�ɤϥ��ݡ��Ȥ���ʤ� */
		cap = tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_EXT_STATUS, TSE_PHY_MDIO_EXT_STATUS_1000BASE_T_HALF, 1);
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_1000BASE_T_CTRL, TSE_PHY_MDIO_1000BASE_T_CTRL_HALF_ADV, 1, cap);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 1000 Base-T Half Duplex set to %d", cap);
		}
	/* ����®�٤Υ��ɥХ��������ȤΥǥ������֥� */
	else {
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_1000BASE_T_CTRL, TSE_PHY_MDIO_1000BASE_T_CTRL_FULL_ADV, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 1000 Base-T Full Duplex set to %d", 0);

		/* TSE MAC �Ǥ� 1000 Mbps Ⱦ��ť⡼�ɤϥ��ݡ��Ȥ���ʤ� */
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_1000BASE_T_CTRL, TSE_PHY_MDIO_1000BASE_T_CTRL_HALF_ADV, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement 1000 Base-T half Duplex set to %d", 0);
		}

	/* MDIO ���ɥ쥹������ */
	tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

	return SUCCESS;
	}

/*
 *  tse_phy_set_adv_100 -- PHY �Υ��ɥХ��������Ȥ� 100 Mbps ������
 */

static int32_t
tse_phy_set_adv_100 (T_TSE_MAC_INFO *pmac_info, uint8_t enable)
{
	uint8_t cap;

	/* MDIO ���ɥ쥹�ϴؿ��κǸ������ */
	uint8_t mdioadd_prev = tse_phy_rd_mdio_addr(pmac_info);

	/*
	 *  i ���ܤ� PHY �˥����������뤿��
	 *  MDIO �� PHY ���ɥ쥹��񤭹���
	 */
	tse_phy_wr_mdio_addr(pmac_info, pmac_info->mdio_address);

	/* enable = 1 �ξ�硢PHY �ε�ǽ�˴�Ť����ɥХ��������Ȥ����� */
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
	/* ����®�٤Υ��ɥХ��������ȤΥǥ������֥� */
	else {
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_100BASE_T4, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 100 Base-T4 set to %d", 0);

		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_100BASE_TX_FULL, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 100 Base-TX Full Duplex set to %d", 0);

		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_100BASE_TX_HALF, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 100 Base-TX Half Duplex set to %d", 0);
		}

	/* MDIO ���ɥ쥹������ */
	tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

	return SUCCESS;
	}

/*
 *  tse_phy_set_adv_10 -- PHY �Υ��ɥХ��������Ȥ� 10 Mbps ������
 */

static int32_t
tse_phy_set_adv_10 (T_TSE_MAC_INFO *pmac_info, uint8_t enable)
{
	uint8_t cap;

	/* MDIO ���ɥ쥹�ϴؿ��κǸ������ */
	uint8_t mdioadd_prev = tse_phy_rd_mdio_addr(pmac_info);

	/*
	 *  i ���ܤ� PHY �˥����������뤿��
	 *  MDIO �� PHY ���ɥ쥹��񤭹���
	 */
	tse_phy_wr_mdio_addr(pmac_info, pmac_info->mdio_address);

	/* enable = 1 �ξ�硢PHY �ε�ǽ�˴�Ť����ɥХ��������Ȥ����� */
	if (enable) {
		cap = tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_10BASE_T_FULL, 1);
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_10BASE_TX_FULL, 1, cap);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 10 Base-TX Full Duplex set to %d", cap);

		cap = tse_phy_rd_mdio_reg(pmac_info, TSE_PHY_MDIO_STATUS, TSE_PHY_MDIO_STATUS_10BASE_T_HALF, 1);
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_10BASE_TX_HALF, 1, cap);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 10 Base-TX Half Duplex set to %d", cap);
		}
	/* ����®�٤Υ��ɥХ��������ȤΥǥ������֥� */
	else {
		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_10BASE_TX_FULL, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 10 Base-TX Full Duplex set to %d", 0);

		tse_phy_wr_mdio_reg(pmac_info, TSE_PHY_MDIO_ADV, TSE_PHY_MDIO_ADV_10BASE_TX_HALF, 1, 0);
		syslog(LOG_DEBUG, "INFO    : PHY - Advertisement of 10 Base-TX Half Duplex set to %d", 0);
		}

	/* MDIO ���ɥ쥹������ */
	tse_phy_wr_mdio_addr(pmac_info, mdioadd_prev);

	return SUCCESS;
	}

/*
 *  marvell_phy_cfg -- Marvell PHY ���ɲä�����
 */

static int32_t
marvell_phy_cfg (T_TSE_MAC *pmac)
{
	/* ��󥯤��ޤ��ʤ���硢�����ȥ��������С��򥤥͡��֥뤷��PHY ��ꥻ�å� */
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
 *  marvell_cfg_rgmii -- Marvell PHY �Υ⡼�ɤ� RGMII ���ѹ�
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
 *  PHY �����
 */

T_TSE_PHY_PROFILE MV88E1111 = {
	"Marvell 88E1111",		/* Marvell 88E1111			*/
	MV88E1111_OUI,			/* OUI					*/
	MV88E1111_MODEL,		/* �٥����ǥ�ʥ�С�			*/
	MV88E1111_REV,			/* ��ǥ��ӥ����ʥ�С�		*/
	TSE_PHY_MDIO_MARVELL_STATUS,	/* Status �쥸�����ξ��		*/
	TSE_PHY_MDIO_MARVELL_STATUS_SPEED_100, /* Speed Status �ξ��		*/
	TSE_PHY_MDIO_MARVELL_STATUS_FULL_DUP, /* Duplex Status �ξ��		*/
	TSE_PHY_MDIO_MARVELL_STATUS_LINK_UP, /* Link Status �ξ��		*/
	&marvell_phy_cfg		/* Marvell PHY �����ꤹ��ؿ��ݥ���	*/
	};
