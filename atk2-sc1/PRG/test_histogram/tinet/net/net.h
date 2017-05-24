#ifndef _NET_H_
#define _NET_H_

/*
 *  バイトオーダの定義
 */

#define _NET_CFG_LITTLE_ENDIAN	1234
#define _NET_CFG_BIG_ENDIAN	4321

#ifdef TARGET_KERNEL_ATK2

#if defined(SIL_ENDIAN_LITTLE)

#define _NET_CFG_BYTE_ORDER	_NET_CFG_LITTLE_ENDIAN

#elif defined(SIL_ENDIAN_BIG)

#define _NET_CFG_BYTE_ORDER	_NET_CFG_BIG_ENDIAN

#else	/* of #if defined(SIL_ENDIAN_BIG) */

#error "SIL_ENDIAN expected."

#endif	/* of #if defined(SIL_ENDIAN_BIG) */

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

/*
 *  バイトオーダ変換の定義
 */

#if _NET_CFG_BYTE_ORDER == _NET_CFG_BIG_ENDIAN

#define ntohs(n)		((uint16_t)n)
#define htons(h)		((uint16_t)h)
#define ntohl(n)		((uint32_t)n)
#define htonl(h)		((uint32_t)h)

#define NTOHS(n)
#define HTONS(h)
#define NTOHL(n)
#define HTONL(h)

/*
 *  注意: IPヘッダ以降は、4 バイト境界にアラインされている事を
 *        前提としているが、4 バイト境界でアクセスする
 *        プロセッサで、ネットワーク側のデータが、4 バイト境界にアライン
 *        されていない場合は、ntoahl、ahtonl、nahcmpl を使用すること。
 */

#define ntoahl(h,n)		memcpy((uint8_t*)&(h),(n),4)
#define ahtonl(n,h)		memcpy((n),(uint8_t*)&(h),4)

#define nahcmpl(n,h)		memcmp((n),(uint8_t*)&(h),4)

#elif _NET_CFG_BYTE_ORDER == _NET_CFG_LITTLE_ENDIAN

#ifndef NET_REV_ENDIAN_HWORD

#define NET_REV_ENDIAN_HWORD(d)	((uint16_t)((((uint16_t)(d)&0xff)<<8)|(((uint16_t)(d)>>8)&0xff)))

#endif	/* of #ifndef NET_REV_ENDIAN_HWORD */

#ifndef NET_REV_ENDIAN_WORD

#define NET_REV_ENDIAN_WORD(d)	((uint32_t)((((uint32_t)(d)&0xff)<<24)|(((uint32_t)(d)&0xff00)<<8)| \
				            (((uint32_t)(d)>>8)&0xff00)|(((uint32_t)(d)>>24)&0xff)))

#endif	/* of #ifndef NET_REV_ENDIAN_WORD */

#define ntohs(n)		NET_REV_ENDIAN_HWORD(n)
#define htons(h)		NET_REV_ENDIAN_HWORD(h)
#define ntohl(n)		NET_REV_ENDIAN_WORD(n)
#define htonl(h)		NET_REV_ENDIAN_WORD(h)

#define NTOHS(n)		((n)=NET_REV_ENDIAN_HWORD(n))
#define HTONS(h)		((h)=NET_REV_ENDIAN_HWORD(h))
#define NTOHL(n)		((n)=NET_REV_ENDIAN_WORD(n))
#define HTONL(h)		((h)=NET_REV_ENDIAN_WORD(h))

/*
 *  注意: IPヘッダ以降は、4 バイト境界にアラインされている事を
 *        前提としているが、4 バイト境界でアクセスする
 *        プロセッサで、ネットワーク側のデータが、4 バイト境界にアライン
 *        されていない場合は、ntoahl、ahtonl、nahcmpl を使用すること。
 */

#ifndef _MACRO_ONLY

extern void rev_memcpy_word (uint8_t *dst, uint8_t *src);
extern int  rev_memcmp_word (uint8_t *data1, uint8_t *data2);

#endif	/* of #ifndef _MACRO_ONLY */

#define ntoahl(h,n)		rev_memcpy_word((uint8_t*)&(h),(n))
#define ahtonl(n,h)		rev_memcpy_word((n),(uint8_t*)&(h))
#define nahcmpl(n,h)		rev_memcmp_word((n),(uint8_t*)&(h))

#else	/* #if _NET_CFG_BYTE_ORDER == _NET_CFG_BIG_ENDIAN */

#error "_NET_CFG_BYTE_ORDER expected."

#endif	/* #if _NET_CFG_BYTE_ORDER == _NET_CFG_BIG_ENDIAN */

/*
 *  プロトコルを識別するフラグに関する定義
 *
 *    ・統計情報
 */

#define PROTO_FLG_PPP_HDLC	ULONG_C(0x00000001)	/* ppp_hdlc.c	*/
#define PROTO_FLG_PPP_FSM	ULONG_C(0x00000002)	/* ppp_fsm.c	*/
#define PROTO_FLG_PPP_AUTH	ULONG_C(0x00000004)	/* ppp_auth.c	*/
#define PROTO_FLG_PPP_LCP	ULONG_C(0x00000008)	/* ppp_lcp.c	*/
#define PROTO_FLG_PPP_IPCP	ULONG_C(0x00000010)	/* ppp_ipcp.c	*/
#define PROTO_FLG_PPP_CCP	ULONG_C(0x00000020)	/* ppp_ccp.c	*/
#define PROTO_FLG_PPP_PAP	ULONG_C(0x00000040)	/* ppp_upap.c	*/
#define PROTO_FLG_PPP_MODEM	ULONG_C(0x00000080)	/* ppp_modem.c	*/
#define PROTO_FLG_PPP		ULONG_C(0x00000100)	/* ppp.c	*/

#define PROTO_FLG_ETHER_NIC	ULONG_C(0x00000001)	/* if_??.c	*/

#define PROTO_FLG_LOOP		ULONG_C(0x00000100)	/* if_loop.c	*/

#define PROTO_FLG_ETHER		ULONG_C(0x00000100)	/* ethernet.c	*/

#define PROTO_FLG_IF		ULONG_C(0x00000800)	/* if.c		*/

#define PROTO_FLG_ARP		ULONG_C(0x00008000)	/* if_ether.c	*/

#define PROTO_FLG_IP4		ULONG_C(0x00010000)	/* ip_*.c	*/
#define PROTO_FLG_ICMP4		ULONG_C(0x00040000)	/* ip_icmp.c	*/

#define PROTO_FLG_IP6		ULONG_C(0x00100000)	/* ip6_*.c	*/
#define PROTO_FLG_ICMP6		ULONG_C(0x00400000)	/* icmp6.c	*/
#define PROTO_FLG_ND6		ULONG_C(0x00800000)	/* nd6_*.c	*/

#define PROTO_FLG_UDP		ULONG_C(0x08000000)	/* udp_*.c	*/

#define PROTO_FLG_TCP		ULONG_C(0x01000000)	/* tcp_*.c	*/

#define PROTO_FLG_NET_BUF	ULONG_C(0x10000000)	/* net_buf.c	*/
#define PROTO_FLG_NET		ULONG_C(0x80000000)	/* route.c, net*.c	*/

/*
 *  アドレス形式の定義
 */

#define AT_ETHER		UINT_C(0x01)		/* イーサネット		*/
#define AT_INET4		UINT_C(0x10)		/* IPv4 アドレス	*/
#define AT_INET6		UINT_C(0x20)		/* IPv6 アドレス	*/

#if !defined(TOPPERS_MACRO_ONLY) && !defined(_MACRO_ONLY)

/*
 *  TINET 独自関数
 */

extern char *mac2str (char *buf, uint8_t *macaddr);

#endif /* #if !defined(TOPPERS_MACRO_ONLY) && !defined(_MACRO_ONLY) */

#endif	/* of #ifndef _NET_H_ */
