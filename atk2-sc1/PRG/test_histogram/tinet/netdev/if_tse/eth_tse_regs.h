#ifndef _ETH_TSE_REGS_H_
#define _ETH_TSE_REGS_H_

/*
 *  レジスタビット定義
 */

/* MAC レジスタ */

/* command_config レジスタ */

#define TSEMAC_CMD_TX_ENA		UINT_C(0x01)
#define TSEMAC_CMD_RX_ENA		UINT_C(0x02)
#define TSEMAC_CMD_XON_GEN		UINT_C(0x04)
#define TSEMAC_CMD_ETH_SPEED		UINT_C(0x08)
#define TSEMAC_CMD_PROMIS_EN		UINT_C(0x10)
#define TSEMAC_CMD_PAD_EN		UINT_C(0x20)
#define TSEMAC_CMD_CRC_FWD		UINT_C(0x40)
#define TSEMAC_CMD_PAUSE_FWD		UINT_C(0x80)
#define TSEMAC_CMD_PAUSE_IGNORE		UINT_C(0x100)
#define TSEMAC_CMD_TX_ADDR_INS		UINT_C(0x200)
#define TSEMAC_CMD_HD_ENA		UINT_C(0x400)
#define TSEMAC_CMD_EXCESS_COL		UINT_C(0x800)
#define TSEMAC_CMD_LATE_COL		UINT_C(0x1000)
#define TSEMAC_CMD_SW_RESET		UINT_C(0x2000)
#define TSEMAC_CMD_MHASH_SEL		UINT_C(0x4000)
#define TSEMAC_CMD_LOOPBACK		UINT_C(0x8000)

/* Bits (18:16) = address select */

#define TSEMAC_CMD_TX_ADDR_SEL		UINT_C(0x70000)
#define TSEMAC_CMD_MAGIC_ENA		UINT_C(0x80000)
#define TSEMAC_CMD_SLEEP		UINT_C(0x100000)
#define TSEMAC_CMD_WAKEUP		UINT_C(0x200000)
#define TSEMAC_CMD_XOFF_GEN		UINT_C(0x400000)
#define TSEMAC_CMD_CNTL_FRM_ENA		UINT_C(0x800000)
#define TSEMAC_CMD_NO_LENGTH_CHECK	UINT_C(0x1000000)
#define TSEMAC_CMD_ENA_10		UINT_C(0x2000000)
#define TSEMAC_CMD_RX_ERR_DISC		UINT_C(0x4000000)

/* Bits (30..27) reserved */

#define TSEMAC_CMD_CNT_RESET		UINT_C(0x80000000)

/* 送受信コマンド・レジスタ */

#define TSEMAC_TX_CMD_STAT_OMITCRC	UINT_C(0x20000)
#define TSEMAC_TX_CMD_STAT_TXSHIFT16	UINT_C(0x40000)

#define TSEMAC_RX_CMD_STAT_RXSHIFT16	UINT_C(0x2000000)

/* PCS Control レジスタ */

enum {
	PCS_CTL_speed1		= 1<<6,
	PCS_CTL_speed0		= 1<<13,
	PCS_CTL_fullduplex	= 1<<8,
	PCS_CTL_an_restart	= 1<<9,
	PCS_CTL_isolate		= 1<<10,
	PCS_CTL_powerdown	= 1<<11,
	PCS_CTL_an_enable	= 1<<12,
	PCS_CTL_rx_slpbk	= 1<<14,
	PCS_CTL_sw_reset	= 1<<15
	};

/* PCS Status レジスタ */

enum {
	PCS_ST_has_extcap	= 1<<0,
	PCS_ST_rx_sync		= 1<<2,
	PCS_ST_an_ability	= 1<<3,
	PCS_ST_rem_fault	= 1<<4,
	PCS_ST_an_done		= 1<<5
	};

/* PHY Specific Control レジスタ */

enum {
	MARV_CTL_dis_125clk	= 1<<4,
	MARV_CTL_mac_pow_up	= 1<<3,
	MARV_CTL_man_mdi	= 0<<5,
	MARV_CTL_man_mdix	= 1<<5,
	MARV_CTL_ena_auto	= 3<<5
	};

/* PHY Specific Extended Control レジスタ */

enum {
	MARV_EXT_CTL_rx_tim_ct	= 1<<7,
	MARV_EXT_CTL_tx_tim_ct	= 1<<1
	};

/* PHY Specific Extended Status レジスタ */

enum {
	MARV_EXT_ST_fc_auto_sel		= 1<<15,
	MARV_EXT_ST_fc_resolution	= 1<<13,
	MARV_EXT_ST_mode_mask		= 15<<0,
	MARV_EXT_ST_mode_copper_rgmii	= 11<<0,
	MARV_EXT_ST_mode_fiber_rgmii	= 3<<0,
	MARV_EXT_ST_mode_sgmii_no_clk	= 4<<0,
	MARV_EXT_ST_mode_copper_rtbi	= 9<<0
	};

/*
 *  MAC レジスタ・スペース内の MDIO レジストリ
 */

typedef volatile struct t_tse_mdio {
	uint32_t	CONTROL;
	uint32_t	STATUS;
	uint32_t	PHY_ID1;
	uint32_t	PHY_ID2;
	uint32_t	ADV;
	uint32_t	REMADV;

	uint32_t	reg6;
	uint32_t	reg7;
	uint32_t	reg8;
	uint32_t	reg9;
	uint32_t	rega;
	uint32_t	regb;
	uint32_t	regc;
	uint32_t	regd;
	uint32_t	rege;
	uint32_t	regf;
	uint32_t	reg10;
	uint32_t	reg11;
	uint32_t	reg12;
	uint32_t	reg13;
	uint32_t	reg14;
	uint32_t	reg15;
	uint32_t	reg16;
	uint32_t	reg17;
	uint32_t	reg18;
	uint32_t	reg19;
	uint32_t	reg1a;
	uint32_t	reg1b;
	uint32_t	reg1c;
	uint32_t	reg1d;
	uint32_t	reg1e;
	uint32_t	reg1f;
	} T_TSE_MDIO;

/*
 *  MAC レジスタ・スペース
 */

typedef volatile struct t_tse_mac {
	uint32_t	REV;
	uint32_t	SCRATCH;
	uint32_t	COMMAND_CONFIG;
	uint32_t	MAC_0;
	uint32_t	MAC_1;
	uint32_t	FRM_LENGTH;
	uint32_t	PAUSE_QUANT;
	uint32_t	RX_SECTION_EMPTY;
	uint32_t	RX_SECTION_FULL;
	uint32_t	TX_SECTION_EMPTY;
	uint32_t	TX_SECTION_FULL;
	uint32_t	RX_ALMOST_EMPTY;
	uint32_t	RX_ALMOST_FULL;
	uint32_t	TX_ALMOST_EMPTY;
	uint32_t	TX_ALMOST_FULL;
	uint32_t	MDIO_ADDR0;
	uint32_t	MDIO_ADDR1;

	uint32_t	reservedx44[5];
	uint32_t	REG_STAT;
	uint32_t	TX_IPG_LENGTH;

	uint32_t	aMACID_1;
	uint32_t	aMACID_2;
	uint32_t	aFramesTransmittedOK;
	uint32_t	aFramesReceivedOK;
	uint32_t	aFramesCheckSequenceErrors;
	uint32_t	aAlignmentErrors;
	uint32_t	aOctetsTransmittedOK;
	uint32_t	aOctetsReceivedOK;
	uint32_t	aTxPAUSEMACCtrlFrames;
	uint32_t	aRxPAUSEMACCtrlFrames;
	uint32_t	ifInErrors;
	uint32_t	ifOutErrors;
	uint32_t	ifInUcastPkts;
	uint32_t	ifInMulticastPkts;
	uint32_t	ifInBroadcastPkts;
	uint32_t	ifOutDiscards;
	uint32_t	ifOutUcastPkts;
	uint32_t	ifOutMulticastPkts;
	uint32_t	ifOutBroadcastPkts;
	uint32_t	etherStatsDropEvent;
	uint32_t	etherStatsOctets;
	uint32_t	etherStatsPkts;
	uint32_t	etherStatsUndersizePkts;
	uint32_t	etherStatsOversizePkts;
	uint32_t	etherStatsPkts64Octets;
	uint32_t	etherStatsPkts65to127Octets;
	uint32_t	etherStatsPkts128to255Octets;
	uint32_t	etherStatsPkts256to511Octets;
	uint32_t	etherStatsPkts512to1023Octets;
	uint32_t	etherStatsPkts1024to1518Octets;
	uint32_t	etherStatsPkts1519toXOctets;
	uint32_t	etherStatsJabbers;
	uint32_t	etherStatsFragments;

	uint32_t	reservedxE4;
	uint32_t	TX_CMD_STAT;
	uint32_t	RX_CMD_STAT;

	uint32_t	msb_aOctetsTransmittedOK;
	uint32_t	msb_aOctetsReceivedOK;
	uint32_t	msb_etherStatsOctets;
	uint32_t	reservedxFC;

	uint32_t	hashtable[64];

	T_TSE_MDIO	mdio0;
	T_TSE_MDIO	mdio1;

	uint32_t	smac0_0;
	uint32_t	smac0_1;
	uint32_t	smac1_0;
	uint32_t	smac1_1;
	uint32_t	smac2_0;
	uint32_t	smac2_1;
	uint32_t	smac3_0;
	uint32_t	smac3_1;

	uint32_t	reservedx320[56];
	} T_TSE_MAC;

#endif	/* of #ifndef _ETH_TSE_REGS_H_ */
