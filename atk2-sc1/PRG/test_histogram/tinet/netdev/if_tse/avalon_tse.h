#ifndef _AVALON_TSE_H_
#define _AVALON_TSE_H_

#include "eth_tse_regs.h"
#include "ipport.h"
#include "avalon_sgdma.h"

#define SUCCESS		0	/* whatever the call was, it worked.	*/
#define ENP_RESOURCE	-22	/* ran out of other queue-able resource	*/
#define ENP_PARAM	-10	/* bad parameter			*/

/*
 *  PHY 状態の定義
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
 *  getPHYSpeed が返すエラーの定義
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
 *  システム定数の定義
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
 *  PHY レジスタの定義
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

/* MDIO CONTROL レジスタ */

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

/* MDIO STATUS レジスタ */

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

/* AN Advertisement レジスタ */

enum {
	TSE_PHY_MDIO_ADV_100BASE_T4		= 9,
	TSE_PHY_MDIO_ADV_100BASE_TX_FULL	= 8,
	TSE_PHY_MDIO_ADV_100BASE_TX_HALF	= 7,
	TSE_PHY_MDIO_ADV_10BASE_TX_FULL		= 6,
	TSE_PHY_MDIO_ADV_10BASE_TX_HALF		= 5
	};

/* 1000BASE-T Control レジスタ */

enum {
	TSE_PHY_MDIO_1000BASE_T_CTRL_FULL_ADV	= 9,
	TSE_PHY_MDIO_1000BASE_T_CTRL_HALF_ADV	= 8
	};

/* Extended Status レジスタ */

enum {
	TSE_PHY_MDIO_EXT_STATUS_1000BASE_X_FULL	= 15,
	TSE_PHY_MDIO_EXT_STATUS_1000BASE_X_HALF	= 14,
	TSE_PHY_MDIO_EXT_STATUS_1000BASE_T_FULL	= 13,
	TSE_PHY_MDIO_EXT_STATUS_1000BASE_T_HALF	= 12
	};

/* PHY Specific Status レジスタ */

enum {
	TSE_PHY_MDIO_MARVELL_STATUS_SPEED_100	= 14,
	TSE_PHY_MDIO_MARVELL_STATUS_FULL_DUP	= 13,
	TSE_PHY_MDIO_MARVELL_STATUS_LINK_UP	= 10
	};

/*
 *  PHY 構造体
 */

typedef struct t_tse_phy_profile {
	char		name[80];			/* PHY 名			*/

	uint32_t	oui;				/* PHY OUI			*/

	uint8_t		model_number;			/* PHY モデルナンバー		*/

	uint8_t		revision_number;		/* PHY リビジョンナンバー	*/

	uint8_t		status_reg_location;		/* Status レジスタの場所	*/

	uint8_t		speed_lsb_location;		/* Speed Status の場所		*/

	uint8_t		duplex_bit_location;		/* Duplex Status の場所		*/

	uint8_t		link_bit_location;		/* Link Status の場所		*/

	int32_t		(*phy_cfg)(T_TSE_MAC *pmac);	/* 追加設定をする関数ポインタ	*/

	uint32_t	(*link_status_read)(T_TSE_MAC *pmac); /* link status を読み出す関数ポインタ */
	} T_TSE_PHY_PROFILE;

/*
 *  MAC 情報構造体
 */

typedef struct t_tse_mac_info {
	T_TSE_MAC	*tse_mac_base;			/* TSE MAC のベースアドレス			*/
	uint16_t	tse_tx_depth;			/* 送信 FIFO バッファの深さ			*/
	uint16_t	tse_rx_depth;			/* 受信 FIFO バッファの深さ			*/
	uint8_t		mac_type;			/* MAC のタイプ					*/

	T_SGDMA_CSR	*tx_sgdma_csr;			/* SGDMA TX のベースアドレス			*/
	T_SGDMA_CSR	*rx_sgdma_csr;			/* SGDMA RX のベースアドレス			*/

	T_SGDMA_DESCRIPTOR *desc;			/* ディスクリプタ・メモリのベースアドレス	*/

	uint8_t		mdio_address;			/* PHY の MDIO アドレス				*/
	T_TSE_PHY_PROFILE *pphy_profile;		/* PHY プロファイルを指すポインタ		*/
	int32_t		(*tse_phy_cfg)(T_TSE_MAC *pmac); /* 追加の設定を行う関数を指すポインタ		*/

	char		mac_addr[8];			/* MAC アドレス					*/

	T_NET_BUF	*tx_nbuf;			/* 送信バッファ					*/
	T_NET_BUF	*rx_nbuf;			/* 受信バッファ					*/
	} T_TSE_MAC_INFO;

#if !defined(TOPPERS_MACRO_ONLY) && !defined(_MACRO_ONLY)

/*
 *  変数
 */

extern T_TSE_PHY_PROFILE MV88E1111;	/* PHY の定義 */

/*
 *  関数
 */

extern int32_t tse_mac_aTxWrite(T_TSE_MAC_INFO *pmac_info, T_SGDMA_DESCRIPTOR *txDesc);
extern int32_t tse_mac_aRxRead(T_TSE_MAC_INFO *pmac_info, T_SGDMA_DESCRIPTOR *rxDesc);
extern int32_t getPHYSpeed(T_TSE_MAC_INFO *pmac_info);
extern int32_t tse_phy_init(T_TSE_MAC_INFO *pmac_info);
extern int32_t marvell_cfg_rgmii(T_TSE_MAC *pmac);

#endif /* #if !defined(TOPPERS_MACRO_ONLY) && !defined(_MACRO_ONLY) */

#endif	/* of #ifndef _AVALON_TSE_H_ */
