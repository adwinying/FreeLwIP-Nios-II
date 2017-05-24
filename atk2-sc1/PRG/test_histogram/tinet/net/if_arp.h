#ifndef _IF_ARP_H_
#define _IF_ARP_H_

#ifdef SUPPORT_ETHER

/*
 *  Address Resolution Protocol.
 *
 *  RFC 826 参照
 */

/*
 *  構造体
 */

/* ARP ヘッダ */

typedef struct t_arp_hdr {
	uint16_t	hrd_addr;	/* 物理アドレスフォーマット	*/
	uint16_t	proto;		/* プロトコル			*/
	uint8_t		hdr_len;	/* 物理アドレス長		*/
	uint8_t		proto_len;	/* プロトコル長			*/
	uint16_t	opcode;		/* 命令コード			*/
	} T_ARP_HDR;

#define ARP_HDR_SIZE			(sizeof(T_ARP_HDR))
#define IF_ARP_HDR_SIZE			(IF_HDR_SIZE + ARP_HDR_SIZE)

#define GET_ARP_HDR(nbuf)		((T_ARP_HDR*)((nbuf)->buf + IF_HDR_SIZE))

#define GET_IF_ARP_HDR_SIZE(nbuf)	(IF_HDR_SIZE + ARP_HDR_SIZE)

/* hrd_addr、物理アドレスフォーマットの値 */

#define ARPHRD_ETHER	1	/* Ethernet	*/

/* opcode, 命令コードの値 */

#define ARPOP_REQUEST		UINT_C(1)	/* アドレス解決要求		*/
#define ARPOP_REPLY		UINT_C(2)	/* アドレス解決応答		*/
#define ARPOP_REV_REQUEST	UINT_C(3)	/* プロトコル・アドレス解決要求	*/
#define ARPOP_REV_REPLY		UINT_C(4)	/* プロトコル・アドレス解決要求	*/
#define ARPOP_INV_REQUEST	UINT_C(8)	/* 終端識別要求			*/
#define ARPOP_INV_REPLY		UINT_C(9)	/* 終端識別要求			*/

/*
 *  イーサネットドライバモジュールで使用される ARP 情報
 */

typedef struct arpcom {
	uint8_t		mac_addr[ETHER_ADDR_LEN];
	} T_ARPCOM;

/*
 *  ARP キャッシュ更新タイマの定義
 */

#define ARP_TIMER_TMO		(60*NET_TIMER_HZ)	/* タイムアウト、60 [s]	*/

/*
 *  変数
 */

extern const uint8_t	ether_broad_cast_addr[ETHER_ADDR_LEN];

#endif	/* of #ifdef SUPPORT_ETHER */

#endif	/* of #ifndef _IF_ARP_H_ */
