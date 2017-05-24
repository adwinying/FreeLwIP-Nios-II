#ifndef _AVALON_TSE_H_
#define _AVALON_TSE_H_

#include "eth_tse_regs.h"
#include "ipport.h"
#include "avalon_sgdma.h"

#define SUCCESS		0	/* whatever the call was, it worked.	*/
#define ENP_RESOURCE	-22	/* ran out of other queue-able resource	*/
#define ENP_PARAM	-10	/* bad parameter			*/

/*
 *  PHY ���֤����
 */

#define TSE_PHY_AN_NOT_COMPLETE		-1
#define TSE_PHY_AN_NOT_CAPABLE		-2
#define TSE_PHY_AN_COMPLETE		0
#define TSE_PHY_SPEED_INVALID		3
#define TSE_PHY_SPEED_1000		2
#define TSE_PHY_SPEED_100		1
#define TSE_PHY_SPEED_10		0
#define TSE_PHY_SPEED_NO_COMMON		-1
#define TSE_PHY_DUPLEX_FULL		1
#define TSE_PHY_DUPLEX_HALF		0

/*
 *  getPHYSpeed ���֤����顼�����
 */

enum {
	TSE_E_NO_PMAC_FOUND		= (1 << 23),
	TSE_E_NO_MDIO			= (1 << 22),
	TSE_E_NO_PHY			= (1 << 21),
	TSE_E_NO_COMMON_SPEED		= (1 << 20),
	TSE_E_AN_NOT_COMPLETE		= (1 << 19),
	TSE_E_NO_PHY_PROFILE		= (1 << 18),
	TSE_E_PROFILE_INCORRECT_DEFINED	= (1 << 17),
	TSE_E_INVALID_SPEED		= (1 << 16)
	};

/*
 *  �����ƥ���������
 */

#define TSE_SW_RESET_TIME_OUT_CNT	10000
#define TSE_SGDMA_BUSY_TIME_OUT_CNT	1000000

#define TSE_FIRST_TX_SGDMA_DESC_OFST	0
#define TSE_SECOND_TX_SGDMA_DESC_OFST	1
#define TSE_FIRST_RX_SGDMA_DESC_OFST	2
#define TSE_SECOND_RX_SGDMA_DESC_OFST	3

#define TSE_FULL_MAC			0
#define TSE_MACLITE_10_100		1
#define TSE_MACLITE_1000		2

#define TSE_NO_INDEX_FOUND		-1
#define TSE_SYSTEM_DEF_ERROR		-1
#define TSE_MALLOC_FAILED		-1

#define TSE_DUPLEX_MODE_DEFAULT		TSE_PHY_DUPLEX_FULL
#define TSE_MAC_SPEED_DEFAULT		TSE_PHY_SPEED_100
#define AUTONEG_TIMEOUT_THRESHOLD	250000
#define CHECKLINK_TIMEOUT_THRESHOLD	1000000
#define NOMDIO_TIMEOUT_THRESHOLD	1000000
#define DISGIGA_TIMEOUT_THRESHOLD	5000000

/*
 *  PHY ID
 */

enum {
	MV88E1111_OUI	= 0x005043,
	MV88E1111_MODEL	= 0x0c,
	MV88E1111_REV	= 0x2
	};

/*
 *  PHY �쥸���������
 */

enum {
	TSE_PHY_MDIO_CONTROL		= 0,
	TSE_PHY_MDIO_STATUS		= 1,
	TSE_PHY_MDIO_PHY_ID1		= 2,
	TSE_PHY_MDIO_PHY_ID2		= 3,
	TSE_PHY_MDIO_ADV		= 4,
	TSE_PHY_MDIO_REMADV		= 5,

	TSE_PHY_MDIO_AN_EXT		= 6,
	TSE_PHY_MDIO_1000BASE_T_CTRL	= 9,
	TSE_PHY_MDIO_1000BASE_T_STATUS	= 10,
	TSE_PHY_MDIO_EXT_STATUS		= 15,

	TSE_PHY_MDIO_MARVELL_CONTROL	= 16,
	TSE_PHY_MDIO_MARVELL_STATUS	= 17,
	TSE_PHY_MDIO_MARVELL_EXT_CONTROL = 20,
	TSE_PHY_MDIO_MARVELL_EXT_STATUS	= 27
	};

/* MDIO CONTROL �쥸���� */

enum {
	TSE_PHY_MDIO_CONTROL_RESET	= 15,
	TSE_PHY_MDIO_CONTROL_LOOPBACK	= 14,
	TSE_PHY_MDIO_CONTROL_SPEED_LSB	= 13,
	TSE_PHY_MDIO_CONTROL_AN_ENA	= 12,
	TSE_PHY_MDIO_CONTROL_POWER_DOWN	= 11,
	TSE_PHY_MDIO_CONTROL_ISOLATE	= 10,
	TSE_PHY_MDIO_CONTROL_RESTART_AN	= 9,
	TSE_PHY_MDIO_CONTROL_DUPLEX	= 8,
	TSE_PHY_MDIO_CONTROL_SPEED_MSB	= 6
	};

/* MDIO STATUS �쥸���� */

enum {
	TSE_PHY_MDIO_STATUS_100BASE_T4		= 15,
	TSE_PHY_MDIO_STATUS_100BASE_X_FULL	= 14,
	TSE_PHY_MDIO_STATUS_100BASE_X_HALF	= 13,
	TSE_PHY_MDIO_STATUS_10BASE_T_FULL	= 12,
	TSE_PHY_MDIO_STATUS_10BASE_T_HALF	= 11,
	TSE_PHY_MDIO_STATUS_100BASE_T2_FULL	= 10,
	TSE_PHY_MDIO_STATUS_100BASE_T2_HALF	= 9,
	TSE_PHY_MDIO_STATUS_EXT_STATUS		= 8,
	TSE_PHY_MDIO_STATUS_AN_COMPLETE		= 5,
	TSE_PHY_MDIO_STATUS_AN_ABILITY		= 3,
	TSE_PHY_MDIO_STATUS_LINK_STATUS		= 2
	};

/* AN Advertisement �쥸���� */

enum {
	TSE_PHY_MDIO_ADV_100BASE_T4		= 9,
	TSE_PHY_MDIO_ADV_100BASE_TX_FULL	= 8,
	TSE_PHY_MDIO_ADV_100BASE_TX_HALF	= 7,
	TSE_PHY_MDIO_ADV_10BASE_TX_FULL		= 6,
	TSE_PHY_MDIO_ADV_10BASE_TX_HALF		= 5
	};

/* 1000BASE-T Control �쥸���� */

enum {
	TSE_PHY_MDIO_1000BASE_T_CTRL_FULL_ADV	= 9,
	TSE_PHY_MDIO_1000BASE_T_CTRL_HALF_ADV	= 8
	};

/* Extended Status �쥸���� */

enum {
	TSE_PHY_MDIO_EXT_STATUS_1000BASE_X_FULL	= 15,
	TSE_PHY_MDIO_EXT_STATUS_1000BASE_X_HALF	= 14,
	TSE_PHY_MDIO_EXT_STATUS_1000BASE_T_FULL	= 13,
	TSE_PHY_MDIO_EXT_STATUS_1000BASE_T_HALF	= 12
	};

/* PHY Specific Status �쥸���� */

enum {
	TSE_PHY_MDIO_MARVELL_STATUS_SPEED_100	= 14,
	TSE_PHY_MDIO_MARVELL_STATUS_FULL_DUP	= 13,
	TSE_PHY_MDIO_MARVELL_STATUS_LINK_UP	= 10
	};

/*
 *  PHY ��¤��
 */

typedef struct t_tse_phy_profile {
	char		name[80];			/* PHY ̾			*/

	uint32_t	oui;				/* PHY OUI			*/

	uint8_t		model_number;			/* PHY ��ǥ�ʥ�С�		*/

	uint8_t		revision_number;		/* PHY ��ӥ����ʥ�С�	*/

	uint8_t		status_reg_location;		/* Status �쥸�����ξ��	*/

	uint8_t		speed_lsb_location;		/* Speed Status �ξ��		*/

	uint8_t		duplex_bit_location;		/* Duplex Status �ξ��		*/

	uint8_t		link_bit_location;		/* Link Status �ξ��		*/

	int32_t		(*phy_cfg)(T_TSE_MAC *pmac);	/* �ɲ�����򤹤�ؿ��ݥ���	*/

	uint32_t	(*link_status_read)(T_TSE_MAC *pmac); /* link status ���ɤ߽Ф��ؿ��ݥ��� */
	} T_TSE_PHY_PROFILE;

/*
 *  MAC ����¤��
 */

typedef struct t_tse_mac_info {
	T_TSE_MAC	*tse_mac_base;			/* TSE MAC �Υ١������ɥ쥹			*/
	uint16_t	tse_tx_depth;			/* ���� FIFO �Хåե��ο���			*/
	uint16_t	tse_rx_depth;			/* ���� FIFO �Хåե��ο���			*/
	uint8_t		mac_type;			/* MAC �Υ�����					*/

	T_SGDMA_CSR	*tx_sgdma_csr;			/* SGDMA TX �Υ١������ɥ쥹			*/
	T_SGDMA_CSR	*rx_sgdma_csr;			/* SGDMA RX �Υ١������ɥ쥹			*/

	T_SGDMA_DESCRIPTOR *desc;			/* �ǥ�������ץ�������Υ١������ɥ쥹	*/

	uint8_t		mdio_address;			/* PHY �� MDIO ���ɥ쥹				*/
	T_TSE_PHY_PROFILE *pphy_profile;		/* PHY �ץ�ե������ؤ��ݥ���		*/
	int32_t		(*tse_phy_cfg)(T_TSE_MAC *pmac); /* �ɲä������Ԥ��ؿ���ؤ��ݥ���		*/

	char		mac_addr[8];			/* MAC ���ɥ쥹					*/

	T_NET_BUF	*tx_nbuf;			/* �����Хåե�					*/
	T_NET_BUF	*rx_nbuf;			/* �����Хåե�					*/
	} T_TSE_MAC_INFO;

#if !defined(TOPPERS_MACRO_ONLY) && !defined(_MACRO_ONLY)

/*
 *  �ѿ�
 */

extern T_TSE_PHY_PROFILE MV88E1111;	/* PHY ����� */

/*
 *  �ؿ�
 */

extern int32_t tse_mac_aTxWrite(T_TSE_MAC_INFO *pmac_info, T_SGDMA_DESCRIPTOR *txDesc);
extern int32_t tse_mac_aRxRead(T_TSE_MAC_INFO *pmac_info, T_SGDMA_DESCRIPTOR *rxDesc);
extern int32_t getPHYSpeed(T_TSE_MAC_INFO *pmac_info);
extern int32_t tse_phy_init(T_TSE_MAC_INFO *pmac_info);
extern int32_t marvell_cfg_rgmii(T_TSE_MAC *pmac);

#endif /* #if !defined(TOPPERS_MACRO_ONLY) && !defined(_MACRO_ONLY) */

#endif	/* of #ifndef _AVALON_TSE_H_ */
