#ifndef _NET_ATK2_CFG_H_
#define _NET_ATK2_CFG_H_

/*
 *  ATK2用 汎用ネットワークコンフィギュレーションファイル
 */

/*
 * ネットワークバッファ
 */

/****** Object MEMPFIX ******/

enum {

#if defined(NUM_MPF_NET_BUF_CSEG) && NUM_MPF_NET_BUF_CSEG > 0
	MPF_NET_BUF_CSEG,
#endif	/* of #if defined(NUM_MPF_NET_BUF_CSEG) && NUM_MPF_NET_BUF_CSEG > 0 */

#if defined(NUM_MPF_NET_BUF_64) && NUM_MPF_NET_BUF_64 > 0
	MPF_NET_BUF_64,
#endif	/* of #if defined(NUM_MPF_NET_BUF_64) && NUM_MPF_NET_BUF_64 > 0 */

#if defined(NUM_MPF_NET_BUF_128) && NUM_MPF_NET_BUF_128 > 0
	MPF_NET_BUF_128,
#endif	/* of #if defined(NUM_MPF_NET_BUF_128) && NUM_MPF_NET_BUF_128 > 0 */

#if defined(NUM_MPF_NET_BUF_256) && NUM_MPF_NET_BUF_256 > 0
	MPF_NET_BUF_256,
#endif	/* of #if defined(NUM_MPF_NET_BUF_256) && NUM_MPF_NET_BUF_256 > 0 */

#if defined(NUM_MPF_NET_BUF_512) && NUM_MPF_NET_BUF_512 > 0
	MPF_NET_BUF_512,
#endif	/* of #if defined(NUM_MPF_NET_BUF_512) && NUM_MPF_NET_BUF_512 > 0 */

#if defined(NUM_MPF_NET_BUF_IP_MSS) && NUM_MPF_NET_BUF_IP_MSS > 0
	MPF_NET_BUF_IP_MSS,
#endif	/* of #if defined(NUM_MPF_NET_BUF_IP_MSS) && NUM_MPF_NET_BUF_IP_MSS > 0 */

#if defined(NUM_MPF_NET_BUF_1024) && NUM_MPF_NET_BUF_1024 > 0
	MPF_NET_BUF_1024,
#endif	/* of #if defined(NUM_MPF_NET_BUF_1024) && NUM_MPF_NET_BUF_1024 > 0 */

#if defined(NUM_MPF_NET_BUF_IPV6_MMTU) && NUM_MPF_NET_BUF_IPV6_MMTU > 0
	MPF_NET_BUF_IPV6_MMTU,
#endif	/* of #if defined(NUM_MPF_NET_BUF_IPV6_MMTU) && NUM_MPF_NET_BUF_IPV6_MMTU > 0 */

#if defined(NUM_MPF_NET_BUF_IF_PDU) && NUM_MPF_NET_BUF_IF_PDU > 0
	MPF_NET_BUF_IF_PDU,
#endif	/* of #if defined(NUM_MPF_NET_BUF_IF_PDU) && NUM_MPF_NET_BUF_IF_PDU > 0 */

#if defined(NUM_MPF_NET_BUF4_REASSM) && NUM_MPF_NET_BUF4_REASSM > 0
	MPF_NET_BUF_REASSM,
#endif	/* of #if defined(NUM_MPF_NET_BUF4_REASSM) && NUM_MPF_NET_BUF4_REASSM > 0 */

#if defined(NUM_MPF_NET_BUF6_REASSM) && NUM_MPF_NET_BUF6_REASSM > 0
	MPF_NET_BUF_REASSM,
#endif	/* of #if defined(NUM_MPF_NET_BUF6_REASSM) && NUM_MPF_NET_BUF6_REASSM > 0 */

#if defined(NUM_MPF_NET_BUF6_65536) && NUM_MPF_NET_BUF6_65536 > 0
	MPF_NET_BUF6_65536,
#endif	/* of #if defined(NUM_MPF_NET_BUF6_65536) && NUM_MPF_NET_BUF6_65536 > 0 */

	TNUM_NET_ATK2_MEMPFIX
	};

/*
 *  関数
 */

extern void net_atk2_initialize (void);
extern ER net_atk2_get_mpf (ID mpfid, void **p_blk);
extern ER net_atk2_rel_mpf (ID mpfid, void *blk);

#endif	/* of #ifndef _NET_ATK2_CFG_H_ */
