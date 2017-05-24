#ifndef _IN_VAR_H_
#define _IN_VAR_H_

#if defined(SUPPORT_INET6)

#include <netinet6/in6_var.h>

#endif	/* of #if defined(SUPPORT_INET6) */

/*
 *  独自のエラーコード
 */

#define EV_NOPND		(-97)		/* ペンディングしていない */

/*
 *  ネットワークエラーコード
 */

#define EV_MSIZE		(-98)		/* データグラム長超過	*/
#define EV_NURCH		(-99)		/* ネットワーク到達不能	*/
#define EV_HDOWN		(-100)		/* ホストダウン		*/
#define EV_HURCH		(-101)		/* ホスト到達不能	*/
#define EV_CNNRF		(-102)		/* 接続不能		*/
#define EV_CNRST		(-103)		/* 接続リセット		*/
#define EV_REXMTMO		(-104)		/* 再送タイムアウト	*/

/*
 *  共通タイマの起動周期の定義
 */

#define IN_TIMER_TMO		(SYSTIM_HZ/NET_TIMER_CYCLE)

/*
 *  インタフェースアドレス構造体
 */

typedef struct t_in4_ifaddr {
	T_IN4_ADDR		addr;		/* IPv4 アドレス	*/
	T_IN4_ADDR		mask;		/* サブネットマスク	*/
	} T_IN4_IFADDR;

/*
 *  ルーティング表エントリ構造体
 */

typedef struct t_in4_rtentry {
	T_IN4_ADDR	target;		/* 目標ネットワークアドレス			*/
	T_IN4_ADDR	mask;		/* 目標ネットワークアドレスのサブネットマスク	*/
	T_IN4_ADDR	gateway;	/* ゲートウェイの IP アドレス			*/
	uint32_t	expire;		/* 有効時間が切れる時刻 [s]			*/
	uint8_t		flags;		/* フラグ					*/
	} T_IN4_RTENTRY;

/*
 *  TINET 1.1 との互換性のための定義
 */

#ifdef NUM_ROUTE_ENTRY

#define NUM_STATIC_ROUTE_ENTRY		NUM_ROUTE_ENTRY
#define NUM_REDIRECT_ROUTE_ENTRY	0
#define T_RT_ENTRY			T_IN_RTENTRY

#else	/* of #ifdef NUM_ROUTE_ENTRY */

#define NUM_ROUTE_ENTRY			\
	(NUM_STATIC_ROUTE_ENTRY + NUM_REDIRECT_ROUTE_ENTRY)
					/* ルーティング表のエントリ数	*/

#endif	/* of #ifdef NUM_ROUTE_ENTRY */

/*
 *  ルーティングエントリ状態フラグ
 */

#define IN_RTF_DEFINED		UINT_C(0x01)	/* 定義済み			*/
#define IN_RTF_REDIRECT		UINT_C(0x10)	/* 向け直しにより自動設定された	*/

/*
 *  IPv4 と IPv6 をコンパイル時に選択するためのマクロ
 */

#if defined(SUPPORT_INET4)

#define IN_SET_HEADER(nbuf,len,dst,src,proto,ttl)	\
					in4_set_header(nbuf,len,dst,src,proto,ttl)
#define IN_GET_DATAGRAM(nbuf,len,maxlen,dst,src,proto,ttl,nbatr)	\
					in4_get_datagram(nbuf,len,maxlen,dst,src,proto,ttl,nbatr)
#define IN_CKSUM(nbuf,proto,off,len)	in4_cksum(nbuf,proto,off,len)
#define IN_IS_DSTADDR_ACCEPT(myaddr,dstaddr)			\
					in4_is_dstaddr_accept(myaddr,dstaddr)
#define IN_IFAWITHIFP(ifp,dst)		in4_ifawithifp(ifp,dst)
#define T_IN_IFADDR			T_IN4_IFADDR
#define T_IN_RTENTRY			T_IN4_RTENTRY

#endif	/* of #if defined(SUPPORT_INET4) */

/*
 *  ルーティング表
 */

extern T_IN_RTENTRY routing_tbl[];

/*
 *  前方参照
 */

#ifndef T_NET_BUF_DEFINED
typedef struct t_net_buf T_NET_BUF;
#define T_NET_BUF_DEFINED
#endif

#ifndef T_IFNET_DEFINED
typedef struct t_ifnet T_IFNET;
#define T_IFNET_DEFINED
#endif

/*
 *  関数
 */

extern void ip_input (T_NET_BUF *data);
extern ER in4_set_header (T_NET_BUF *nbuf, uint_t len,
                          T_IN4_ADDR *dstaddr, T_IN4_ADDR *srcaddr, uint8_t proto, uint8_t ttl);
extern ER in4_get_datagram (T_NET_BUF **nbuf, uint_t len, uint_t maxlen,
                            T_IN4_ADDR *dstaddr, T_IN4_ADDR *srcaddr,
                            uint8_t proto, uint8_t ttl, ATR nbatr);
extern uint16_t in4_cksum (T_NET_BUF *nbuf, uint8_t proto, uint_t off, uint_t len);
extern T_IN4_ADDR in4_rtalloc (T_IN4_ADDR dst);
extern void in4_rtredirect (T_IN4_ADDR gateway, T_IN4_ADDR target, uint8_t flags, uint32_t tmo);
extern void in4_init (void);
extern T_IN4_IFADDR *in4_ifawithifp (T_IFNET *ifp, T_IN4_ADDR *dst);
extern T_IN_RTENTRY *in_rtnewentry (uint8_t flags, uint32_t tmo);
extern void in_rttimer (void);
extern void in_rtinit (void);
extern const T_NET_BUF**ip_get_frag_queue (void);

/*
 *  in4_is_dstaddr_accept -- 宛先アドレスとして正しいかチェックする。
 *
 *    注意: dstaddr は、
 *          TINET-1.2 からネットワークバイトオーダ、
 *          TINET-1.1 までは、ホストバイトオーダ
 */
extern bool_t in4_is_dstaddr_accept (T_IN4_ADDR *myaddr, T_IN4_ADDR *dstaddr);

/*
 *  in_cksum -- チェックサム計算関数、IPv4、ICMPv4 用
 *
 *  注意: data は 4 オクテット単位でパディングすること。
 *        data が 2 オクテット単位にアラインされていないと
 *        例外が発生する可能性がある。
 *        len  は 4 オクテット単位にアラインされていること。
 */
extern uint16_t in_cksum(void *data, uint_t len /*オクテット単位*/);

/*
 *  in_cksum_sum -- チェックサムの合計計算関数
 *
 *  注意: data は 4 オクテット単位でパディングすること。
 *        data が 2 オクテット単位にアラインされていないと
 *        例外が発生する可能性がある。
 *        len  は 4 オクテット単位にアラインされていること。
 */
extern uint32_t in_cksum_sum (void *data, uint_t len /*オクテット単位*/);

/*
 *  in_cksum_carry -- チェックサムの桁上げ計算関数
 *
 *  注意: data は 4 オクテット単位でパディングすること。
 *        data が 2 オクテット単位にアラインされていないと
 *        例外が発生する可能性がある。
 *        len  は 4 オクテット単位にアラインされていること。
 */
extern uint32_t in_cksum_carry (uint32_t sum);

#endif	/* of #ifndef _IN_VAR_H_ */
