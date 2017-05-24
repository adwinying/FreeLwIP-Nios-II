#ifndef _IP_VAR_H_
#define _IP_VAR_H_

/*
 *  IPv4 ヘッダ・ペイロードアクセスマクロ
 */

#define IF_IP4_HDR_SIZE			(IF_HDR_SIZE + IP4_HDR_SIZE)

#define GET_IP4_HDR(nbuf)		((T_IP4_HDR*)((nbuf)->buf + IF_HDR_SIZE))
#define GET_IP4_SDU(nbuf)		((uint8_t*)((nbuf)->buf) + GET_IF_IP4_HDR_SIZE(nbuf))

#define GET_IP4_HDR_SIZE(iph)		((uint8_t)IP4_VHL_HL((iph)->vhl) << 2)
#define GET_IP4_SDU_SIZE(iph)		(ntohs((iph)->len) - GET_IP4_HDR_SIZE(iph))
#define GET_IF_IP4_HDR_SIZE(nbuf)	(IF_HDR_SIZE + (GET_IP4_HDR_SIZE(GET_IP4_HDR(nbuf))))

#define SET_IP4_SDU_SIZE(iph,slen)	((iph)->len=(uint16_t)htons((slen) + IP4_HDR_SIZE))

/*
 *  IPv4 と IPv6 をコンパイル時に選択するためのマクロ
 */

#if defined(SUPPORT_INET4)

#define T_TCP_IP_Q_HDR			T_TCP_IP4_Q_HDR

#define IF_IP_HDR_SIZE			IF_IP4_HDR_SIZE

#define GET_IP_HDR(nbuf)		GET_IP4_HDR(nbuf)
#define GET_IP_SDU(nbuf)		GET_IP4_SDU(nbuf)

#define GET_IP_HDR_SIZE(iph)		GET_IP4_HDR_SIZE(iph)
#define GET_IP_SDU_SIZE(iph)		GET_IP4_SDU_SIZE(iph)
#define GET_IF_IP_HDR_SIZE(nbuf)	GET_IF_IP4_HDR_SIZE(nbuf)

#define SET_IP_SDU_SIZE(iph,len)	SET_IP4_SDU_SIZE(iph,len)

#define IP_OUTPUT(nbuf)			ip_output(nbuf)
#define IP_INIT()			ip_init()
#define IP_REMOVE_OPTIONS(nbuf)		ip_remove_options(nbuf)

#endif	/* of #if defined(SUPPORT_INET4) */

/*
 *  IP の MSS サイズのネットワークバッファ
 */

typedef struct t_net_buf_ip_mss {
	uint16_t	len;	/* データの長さ		*/
	uint8_t		idix;	/* mpfid のインデックス	*/
	uint8_t		flags;	/* フラグ		*/
#ifdef IF_ETHER_NIC_NET_BUF_ALIGN
	uint8_t		nalign[IF_ETHER_NIC_NET_BUF_ALIGN];
#endif
#if CPU_NET_ALIGN == 4 && IF_HDR_ALIGN != 4
	uint8_t		halign[4 - IF_HDR_ALIGN];
#endif
	uint8_t		buf[IF_HDR_SIZE + IP4_MSS];
				/* バッファ本体		*/
	} T_NET_BUF_IP_MSS;

/*
 *  65536 オクテットサイズのネットワークバッファ
 */

typedef struct t_net_buf4_65536 {
	uint16_t	len;	/* データの長さ		*/
	uint8_t		idix;	/* mpfid のインデックス	*/
	uint8_t		flags;	/* フラグ		*/
#ifdef IF_ETHER_NIC_NET_BUF_ALIGN
	uint8_t		nalign[IF_ETHER_NIC_NET_BUF_ALIGN];
#endif
#if CPU_NET_ALIGN == 4 && IF_HDR_ALIGN != 4
	uint8_t		halign[4 - IF_HDR_ALIGN];
#endif
	uint8_t		buf[IF_IP4_HDR_SIZE + 65536];
				/* バッファ本体		*/
#if defined(IF_PDU_HDR_PADDING)
	uint8_t		padding[IF_PDU_HDR_PADDING];
#endif				/* ヘッダの余分		*/
	} T_NET_BUF4_65536;

#ifdef IP4_CFG_FRAGMENT

/*
 *  再構成キュー用 IPv4 ヘッダの定義
 */

typedef struct t_qip4_hdr {
	uint8_t		vhl;		/* version << 4 | header length >> 2	*/
					/* length は 4 octets 単位		*/
	uint8_t		type;		/* type of services			*/
	uint16_t	len;		/* total length				*/

	uint16_t	id;		/* identification			*/
	uint16_t	flg_off;	/* fragment flag & offset		*/

	uint8_t		ttl;		/* time to live				*/
	uint8_t		proto;		/* protocol				*/
	uint16_t	sum;		/* checksum				*/

	uint32_t	src;		/* source address			*/

	T_NET_BUF	*next;		/* 次の IPv4 フラグメント		*/
	} T_QIP4_HDR;

#define QIP4_HDR_SIZE			(sizeof(T_QIP4_HDR))
#define IF_QIP4_HDR_SIZE		(IF_HDR_SIZE + QIP4_HDR_SIZE)

#define GET_QIP4_HDR(nbuf)		((T_QIP4_HDR*)((nbuf)->buf + IF_HDR_SIZE))
#define GET_QIP4_SDU(nbuf)		((uint8_t*)(nbuf)->buf + GET_IF_QIP4_HDR_SIZE(nbuf))

#define GET_QIP4_HDR_SIZE(iph)		((uint8_t)IP4_VHL_HL((iph)->vhl) << 2)
#define GET_IF_QIP4_HDR_SIZE(nbuf)	(IF_HDR_SIZE + (GET_QIP4_HDR_SIZE(GET_QIP4_HDR(nbuf))))

/*
 *  データグラム再構成用ネットワークバッファ
 */

typedef struct t_net_buf4_reassm {
	uint16_t	len;	/* データの長さ		*/
	uint8_t		idix;	/* mpfid のインデックス	*/
	uint8_t		flags;	/* フラグ		*/
#ifdef IF_ETHER_NIC_NET_BUF_ALIGN
	uint8_t		nalign[IF_ETHER_NIC_NET_BUF_ALIGN];
#endif
#if CPU_NET_ALIGN == 4 && IF_HDR_ALIGN != 4
	uint8_t		align[4 - IF_HDR_ALIGN];
#endif
	uint8_t		buf[IP4_CFG_FRAG_REASSM_SIZE];
				/* バッファ本体		*/
	} T_NET_BUF4_REASSM;

#endif	/* of #ifdef IP4_CFG_FRAGMENT */

/*
 *  TCP の再構成キューにおける IPv4 ヘッダの定義
 */

typedef struct t_tcp_ip4_q_hdr {
	uint8_t		vhl;		/* version << 4 | header length >> 2	*/
					/* length は 4 octets 単位		*/
	uint8_t		type;		/* type of services			*/
	uint16_t	len;		/* total length				*/

	uint16_t	id;		/* identification			*/
	uint16_t	flg_off;	/* fragment flag & offset		*/

	uint8_t		ttl;		/* time to live				*/
	uint8_t		proto;		/* protocol				*/
	uint16_t	thoff;		/* TCP ヘッダのオフセット		*/

	T_IN4_ADDR	src;		/* source address			*/
	T_IN4_ADDR	dst;		/* destination address			*/
	} T_TCP_IP4_Q_HDR;

#ifdef SUPPORT_MIB

/*
 *  SNMP の 管理情報ベース (MIB)
 */

typedef struct t_ip_stats {
	uint32_t	ipInReceives;
	uint32_t	ipInHdrErrors;
	uint32_t	ipInAddrErrors;
/*	uint32_t	ipForwDatagrams;*/
	uint32_t	ipInUnknownProtos;
/*	uint32_t	ipInDiscards;*/
	uint32_t	ipInDelivers;
	uint32_t	ipOutRequests;
	uint32_t	ipOutDiscards;
/*	uint32_t	ipOutNoRoutes;*/

#ifdef IP4_CFG_FRAGMENT

	uint32_t	ipReasmReqds;
	uint32_t	ipReasmOKs;
	uint32_t	ipReasmFails;
	uint32_t	ipFragOKs;
	uint32_t	ipFragFails;
	uint32_t	ipFragCreates;

#else	/* of #ifdef IP4_CFG_FRAGMENT */

	uint32_t	ipReasmReqds;
/*	uint32_t	ipReasmOKs;*/
	uint32_t	ipReasmFails;
/*	uint32_t	ipFragOKs;*/
/*	uint32_t	ipFragFails;*/
/*	uint32_t	ipFragCreates;*/

#endif	/* of #ifdef IP4_CFG_FRAGMENT */

} T_IP_STATS;

extern T_IP_STATS ip_stats;

#endif	/* of #ifdef SUPPORT_MIB */

/*
 *  関数
 */

extern void ip_frag_timer (void);
extern void ip_init (void);
extern ER ip_remove_options (T_NET_BUF *nbuf);
extern ER ip_output (T_NET_BUF *nbuf);

#endif	/* of #ifndef _IP_VAR_H_ */
