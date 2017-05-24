/*
 *  ATK2用 汎用ネットワークコンフィギュレーションファイル
 */

#include "Os.h"
#include "t_syslog.h"

#include <tinet_atk2_cfg.h>

#include "mempfix.h"

/*
 * ネットワークバッファ
 */

/****** Object MEMPFIX ******/

/*
 *  固定長メモリプール領域
 *  固定長メモリプール管理領域
 */

#if defined(NUM_MPF_NET_BUF_CSEG) && NUM_MPF_NET_BUF_CSEG > 0
static MPF_T MPF_MPF_NET_BUF_CSEG[NUM_MPF_NET_BUF_CSEG * COUNT_MPF_T(sizeof(T_NET_BUF_CSEG))];
static MPFMB MPFMB_MPF_NET_BUF_CSEG[NUM_MPF_NET_BUF_CSEG];
#endif	/* of #if defined(NUM_MPF_NET_BUF_CSEG) && NUM_MPF_NET_BUF_CSEG > 0 */

#if defined(NUM_MPF_NET_BUF_64) && NUM_MPF_NET_BUF_64 > 0
static MPF_T MPF_MPF_NET_BUF_64[NUM_MPF_NET_BUF_64 * COUNT_MPF_T(sizeof(T_NET_BUF_64))];
static MPFMB MPFMB_MPF_NET_BUF_64[NUM_MPF_NET_BUF_64];
#endif	/* of #if defined(NUM_MPF_NET_BUF_64) && NUM_MPF_NET_BUF_64 > 0 */

#if defined(NUM_MPF_NET_BUF_128) && NUM_MPF_NET_BUF_128 > 0
static MPF_T MPF_MPF_NET_BUF_128[NUM_MPF_NET_BUF_128 * COUNT_MPF_T(sizeof(T_NET_BUF_128))];
static MPFMB MPFMB_MPF_NET_BUF_128[NUM_MPF_NET_BUF_128];
#endif	/* of #if defined(NUM_MPF_NET_BUF_128) && NUM_MPF_NET_BUF_128 > 0 */

#if defined(NUM_MPF_NET_BUF_256) && NUM_MPF_NET_BUF_256 > 0
static MPF_T MPF_MPF_NET_BUF_256[NUM_MPF_NET_BUF_256 * COUNT_MPF_T(sizeof(T_NET_BUF_256))];
static MPFMB MPFMB_MPF_NET_BUF_256[NUM_MPF_NET_BUF_256];
#endif	/* of #if defined(NUM_MPF_NET_BUF_256) && NUM_MPF_NET_BUF_256 > 0 */

#if defined(NUM_MPF_NET_BUF_512) && NUM_MPF_NET_BUF_512 > 0
static MPF_T MPF_MPF_NET_BUF_512[NUM_MPF_NET_BUF_512 * COUNT_MPF_T(sizeof(T_NET_BUF_512))];
static MPFMB MPFMB_MPF_NET_BUF_512[NUM_MPF_NET_BUF_512];
#endif	/* of #if defined(NUM_MPF_NET_BUF_512) && NUM_MPF_NET_BUF_512 > 0 */

#if defined(NUM_MPF_NET_BUF_IP_MSS) && NUM_MPF_NET_BUF_IP_MSS > 0
static MPF_T MPF_MPF_NET_BUF_IP_MSS[NUM_MPF_NET_BUF_IP_MSS * COUNT_MPF_T(sizeof(T_NET_BUF_IP_MSS))];
static MPFMB MPFMB_MPF_NET_BUF_IP_MSS[NUM_MPF_NET_BUF_IP_MSS];
#endif	/* of #if defined(NUM_MPF_NET_BUF_IP_MSS) && NUM_MPF_NET_BUF_IP_MSS > 0 */

#if defined(NUM_MPF_NET_BUF_1024) && NUM_MPF_NET_BUF_1024 > 0
static MPF_T MPF_MPF_NET_BUF_1024[NUM_MPF_NET_BUF_1024 * COUNT_MPF_T(sizeof(T_NET_BUF_1024))];
static MPFMB MPFMB_MPF_NET_BUF_1024[NUM_MPF_NET_BUF_1024];
#endif	/* of #if defined(NUM_MPF_NET_BUF_1024) && NUM_MPF_NET_BUF_1024 > 0 */

#if defined(NUM_MPF_NET_BUF_IPV6_MMTU) && NUM_MPF_NET_BUF_IPV6_MMTU > 0
static MPF_T MPF_MPF_NET_BUF_IPV6_MMTU[NUM_MPF_NET_BUF_IPV6_MMTU * COUNT_MPF_T(sizeof(T_NET_BUF_IPV6_MMTU))];
static MPFMB MPFMB_MPF_NET_BUF_IPV6_MMTU[NUM_MPF_NET_BUF_IPV6_MMTU];
#endif	/* of #if defined(NUM_MPF_NET_BUF_IPV6_MMTU) && NUM_MPF_NET_BUF_IPV6_MMTU > 0 */

#if defined(NUM_MPF_NET_BUF_IF_PDU) && NUM_MPF_NET_BUF_IF_PDU > 0
static MPF_T MPF_MPF_NET_BUF_IF_PDU[NUM_MPF_NET_BUF_IF_PDU * COUNT_MPF_T(sizeof(T_NET_BUF_IF_PDU))];
static MPFMB MPFMB_MPF_NET_BUF_IF_PDU[NUM_MPF_NET_BUF_IF_PDU];
#endif	/* of #if defined(NUM_MPF_NET_BUF_IF_PDU) && NUM_MPF_NET_BUF_IF_PDU > 0 */

#if defined(NUM_MPF_NET_BUF4_REASSM) && NUM_MPF_NET_BUF4_REASSM > 0
static MPF_T MPF_MPF_NET_BUF_REASSM[NUM_MPF_NET_BUF4_REASSM * COUNT_MPF_T(sizeof(T_NET_BUF4_REASSM))];
static MPFMB MPFMB_MPF_NET_BUF_REASSM[NUM_MPF_NET_BUF4_REASSM];
#endif	/* of #if defined(NUM_MPF_NET_BUF4_REASSM) && NUM_MPF_NET_BUF4_REASSM > 0 */

#if defined(NUM_MPF_NET_BUF6_REASSM) && NUM_MPF_NET_BUF6_REASSM > 0
static MPF_T MPF_MPF_NET_BUF_REASSM[NUM_MPF_NET_BUF6_REASSM * COUNT_MPF_T(sizeof(T_NET_BUF6_REASSM))];
static MPFMB MPFMB_MPF_NET_BUF_REASSM[NUM_MPF_NET_BUF6_REASSM];
#endif	/* of #if defined(NUM_MPF_NET_BUF6_REASSM) && NUM_MPF_NET_BUF6_REASSM > 0 */

#if defined(NUM_MPF_NET_BUF6_65536) && NUM_MPF_NET_BUF6_65536 > 0
static MPF_T MPF_MPF_NET_BUF6_65536[NUM_MPF_NET_BUF6_65536 * COUNT_MPF_T(sizeof(T_NET_BUF6_65536))];
static MPFMB MPFMB_MPF_NET_BUF6_65536[NUM_MPF_NET_BUF6_65536];
#endif	/* of #if defined(NUM_MPF_NET_BUF6_65536) && NUM_MPF_NET_BUF6_65536 > 0 */

/*
 *  固定長メモリプール初期化ブロック
 */

const MPFINIB net_atk2_mpfinib_table[TNUM_NET_ATK2_MEMPFIX] = {

#if defined(NUM_MPF_NET_BUF_CSEG) && NUM_MPF_NET_BUF_CSEG > 0
	{
		NUM_MPF_NET_BUF_CSEG,
		ROUND_MPF_T(sizeof(T_NET_BUF_CSEG)),
		MPF_MPF_NET_BUF_CSEG,
		MPFMB_MPF_NET_BUF_CSEG
		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_CSEG) && NUM_MPF_NET_BUF_CSEG > 0 */

#if defined(NUM_MPF_NET_BUF_64) && NUM_MPF_NET_BUF_64 > 0
	{
		NUM_MPF_NET_BUF_64,
		ROUND_MPF_T(sizeof(T_NET_BUF_64)),
		MPF_MPF_NET_BUF_64,
		MPFMB_MPF_NET_BUF_64
		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_64) && NUM_MPF_NET_BUF_64 > 0 */

#if defined(NUM_MPF_NET_BUF_128) && NUM_MPF_NET_BUF_128 > 0
	{
		NUM_MPF_NET_BUF_128,
		ROUND_MPF_T(sizeof(T_NET_BUF_128)),
		MPF_MPF_NET_BUF_128,
		MPFMB_MPF_NET_BUF_128
		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_128) && NUM_MPF_NET_BUF_128 > 0 */

#if defined(NUM_MPF_NET_BUF_256) && NUM_MPF_NET_BUF_256 > 0
	{
		NUM_MPF_NET_BUF_256,
		ROUND_MPF_T(sizeof(T_NET_BUF_256)),
		MPF_MPF_NET_BUF_256,
		MPFMB_MPF_NET_BUF_256
		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_256) && NUM_MPF_NET_BUF_256 > 0 */

#if defined(NUM_MPF_NET_BUF_512) && NUM_MPF_NET_BUF_512 > 0
	{
		NUM_MPF_NET_BUF_512,
		ROUND_MPF_T(sizeof(T_NET_BUF_512)),
		MPF_MPF_NET_BUF_512,
		MPFMB_MPF_NET_BUF_512
		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_512) && NUM_MPF_NET_BUF_512 > 0 */

#if defined(NUM_MPF_NET_BUF_IP_MSS) && NUM_MPF_NET_BUF_IP_MSS > 0
	{
		NUM_MPF_NET_BUF_IP_MSS,
		ROUND_MPF_T(sizeof(T_NET_BUF_IP_MSS)),
		MPF_MPF_NET_BUF_IP_MSS,
		MPFMB_MPF_NET_BUF_IP_MSS
		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_IP_MSS) && NUM_MPF_NET_BUF_IP_MSS > 0 */

#if defined(NUM_MPF_NET_BUF_1024) && NUM_MPF_NET_BUF_1024 > 0
	{
		NUM_MPF_NET_BUF_1024,
		ROUND_MPF_T(sizeof(T_NET_BUF_1024)),
		MPF_MPF_NET_BUF_1024,
		MPFMB_MPF_NET_BUF_1024
		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_1024) && NUM_MPF_NET_BUF_1024 > 0 */

#if defined(NUM_MPF_NET_BUF_IPV6_MMTU) && NUM_MPF_NET_BUF_IPV6_MMTU > 0
	{
		NUM_MPF_NET_BUF_IPV6_MMTU,
		ROUND_MPF_T(sizeof(T_NET_BUF_IPV6_MMTU)),
		MPF_MPF_NET_BUF_IPV6_MMTU,
		MPFMB_MPF_NET_BUF_IPV6_MMTU
		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_IPV6_MMTU) && NUM_MPF_NET_BUF_IPV6_MMTU > 0 */

#if defined(NUM_MPF_NET_BUF_IF_PDU) && NUM_MPF_NET_BUF_IF_PDU > 0
	{
		NUM_MPF_NET_BUF_IF_PDU,
		ROUND_MPF_T(sizeof(T_NET_BUF_IF_PDU)),
		MPF_MPF_NET_BUF_IF_PDU,
		MPFMB_MPF_NET_BUF_IF_PDU
		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_IF_PDU) && NUM_MPF_NET_BUF_IF_PDU > 0 */

#if defined(NUM_MPF_NET_BUF4_REASSM) && NUM_MPF_NET_BUF4_REASSM > 0
	{
		NUM_MPF_NET_BUF4_REASSM,
		ROUND_MPF_T(sizeof(T_NET_BUF4_REASSM)),
		MPF_MPF_NET_BUF_REASSM,
		MPFMB_MPF_NET_BUF_REASSM
		},
#endif	/* of #if defined(NUM_MPF_NET_BUF4_REASSM) && NUM_MPF_NET_BUF4_REASSM > 0 */

#if defined(NUM_MPF_NET_BUF6_REASSM) && NUM_MPF_NET_BUF6_REASSM > 0
	{
		NUM_MPF_NET_BUF6_REASSM,
		ROUND_MPF_T(sizeof(T_NET_BUF6_REASSM)),
		MPF_MPF_NET_BUF_REASSM,
		MPFMB_MPF_NET_BUF_REASSM
		},
#endif	/* of #if defined(NUM_MPF_NET_BUF6_REASSM) && NUM_MPF_NET_BUF6_REASSM > 0 */

#if defined(NUM_MPF_NET_BUF6_65536) && NUM_MPF_NET_BUF6_65536 > 0
	{
		NUM_MPF_NET_BUF6_65536,
		ROUND_MPF_T(sizeof(T_NET_BUF6_65536)),
		MPF_MPF_NET_BUF6_65536,
		MPFMB_MPF_NET_BUF6_65536
		}
#endif	/* of #if defined(NUM_MPF_NET_BUF6_65536) && NUM_MPF_NET_BUF6_65536 > 0 */

	};

/*
 *  固定長メモリプール管理ブロック
 */

MPFCB net_atk2_mpfcb_table[TNUM_NET_ATK2_MEMPFIX];

/*
 *  オブジェクトの初期化
 */

void
net_atk2_initialize (void)
{
	mempfix_initialize((uint_t)TNUM_NET_ATK2_MEMPFIX, net_atk2_mpfinib_table, net_atk2_mpfcb_table);
	}

/*
 *  固定長メモリブロックの獲得
 */

ER
net_atk2_get_mpf (ID mpfid, void **p_blk)
{
	return get_mpf((uint_t)TNUM_NET_ATK2_MEMPFIX, net_atk2_mpfcb_table, mpfid, p_blk);
	}

/*
 *  固定長メモリブロックの返却
 */

ER
net_atk2_rel_mpf (ID mpfid, void *blk)
{
	return rel_mpf((uint_t)TNUM_NET_ATK2_MEMPFIX, net_atk2_mpfcb_table, mpfid, blk);
	}
