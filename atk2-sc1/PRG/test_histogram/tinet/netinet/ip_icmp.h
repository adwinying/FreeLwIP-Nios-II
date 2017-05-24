#ifndef _IP_ICMP_H_
#define _IP_ICMP_H_

/*
 *  ICMP ヘッダの定義、RFC 792
 */

typedef struct t_icmp_hdr {
	uint8_t		type;		/* メッセージの型	*/
	uint8_t		code;		/* サブ型		*/
	uint16_t	sum;		/* チェックサム		*/
	union {
		struct id_seq {
			uint16_t	id;
			uint16_t	seq;
			} id_seq;
		T_IN4_ADDR	addr;
		} data;
	} T_ICMP4_HDR;

#define ICMP4_HDR_SIZE			(sizeof(T_ICMP4_HDR))
#define IP4_ICMP4_HDR_SIZE		(IP4_HDR_SIZE + ICMP4_HDR_SIZE)
#define IF_IP4_ICMP4_HDR_SIZE		(IF_IP4_HDR_SIZE + ICMP4_HDR_SIZE)
#define IF_IP4_ICMP4_HDR_OFFSET		(IF_IP4_HDR_SIZE)

#define GET_ICMP4_HDR(nbuf,ihoff)	((T_ICMP4_HDR*)((uint8_t*)((nbuf)->buf) + ihoff))
#define GET_ICMP4_SDU(nbuf,ihoff)	((uint8_t*)((nbuf)->buf) + ihoff + ICMP4_HDR_SIZE)

#define GET_ICMP4_HDR_OFFSET(nbuf)	(GET_IF_IP_HDR_SIZE(nbuf))

#define GET_IP4_ICMP4_HDR_SIZE(nbuf)	(GET_IP4_HDR_SIZE(GET_IP4_HDR(nbuf)) + ICMP4_HDR_SIZE)
#define GET_IF_IP4_ICMP4_HDR_SIZE(nbuf)	(IF_HDR_SIZE + GET_IP4_ICMP4_HDR_SIZE(nbuf))

/*
 *  メッセージの型
 */

#define ICMP4_ECHO_REPLY	UINT_C(0)	/* エコー応答		*/
#define ICMP4_UNREACH		UINT_C(3)	/* エラー		*/
#define ICMP4_SOURCE_QUENCH	UINT_C(4)	/* 発信制限		*/
#define ICMP4_REDIRECT		UINT_C(5)	/* 方向変更		*/
#define ICMP4_ECHO_REQUEST	UINT_C(8)	/* エコー要求		*/
#define ICMP4_TIME_EXCEEDED	UINT_C(11)	/* 発信制限		*/
#define ICMP4_PARAMETER		UINT_C(12)	/* パラメータ異常	*/
#define ICMP4_TIME_REQUEST	UINT_C(13)	/* タイムスタンプ要求	*/
#define ICMP4_TIME_REPLY	UINT_C(14)	/* タイムスタンプ応答	*/
#define ICMP4_MASK_REQUEST	UINT_C(17)	/* アドレスマスク要求	*/
#define ICMP4_MASK_REPLY	UINT_C(18)	/* アドレスマスク応答	*/

/*
 *  サブ型
 */

/* ICMP4_UNREACH (3)	*/

#define ICMP4_UNREACH_NET		UINT_C(0)	/* ネットワーク		*/
#define ICMP4_UNREACH_HOST		UINT_C(1)	/* ホスト		*/
#define ICMP4_UNREACH_PROTOCOL		UINT_C(2)	/* プロトコル		*/
#define ICMP4_UNREACH_PORT		UINT_C(3)	/* ポート		*/
#define ICMP4_UNREACH_FLAG		UINT_C(4)	/* 分割			*/
#define ICMP4_UNREACH_SRC_FAIL		UINT_C(5)	/* ソース経路		*/
#define ICMP4_UNREACH_NET_UNKNOWN	UINT_C(6)	/* ネットワーク不明	*/
#define ICMP4_UNREACH_HOST_UNKNOWN	UINT_C(7)	/* ホスト不明		*/
#define ICMP4_UNREACH_ISOLATED		UINT_C(8)	/* 分離			*/
#define ICMP4_UNREACH_NET_PROHIB	UINT_C(9)	/* 			*/
#define ICMP4_UNREACH_HOST_PROHIB	UINT_C(10)	/* 			*/
#define ICMP4_UNREACH_TOS_NET		UINT_C(11)	/* ネットワークへの不正TOS*/
#define ICMP4_UNREACH_TOS_HOST		UINT_C(12)	/* ホストへの不正TOS	*/
#define ICMP4_UNREACH_FILTER_PROHIB	UINT_C(13)	/* 			*/
#define ICMP4_UNREACH_HOST_PRECEDENCE	UINT_C(14)	/* 			*/
#define ICMP4_UNREACH_PRECEDENCE_CUTOFF	UINT_C(15)	/* 			*/

/* ICMP4_REDIRECT (5)	*/

#define ICMP4_REDIRECT_NET		UINT_C(0)	/* ネットワーク		*/
#define ICMP4_REDIRECT_HOST		UINT_C(1)	/* ホスト		*/
#define ICMP4_REDIRECT_PORT_AND_NET	UINT_C(2)	/* ポート番号とネット	*/
#define ICMP4_REDIRECT_PORT_AND_HOST	UINT_C(3)	/* ポート番号とホスト	*/

/*
 *  全域変数
 */

/*
 *  関数シミュレーションマクロ
 */

#ifndef ICMP_REPLY_ERROR

#define _tinet_icmp_error(code,data)

#endif	/* of #ifdef ICMP_REPLY_ERROR */

/*
 *  関数
 */

extern uint_t icmp_input (T_NET_BUF **inputp, uint_t *offp, uint_t *nextp);

#ifdef ICMP_REPLY_ERROR

extern void icmp_error (uint8_t code, T_NET_BUF *data);

#endif	/* of #ifdef ICMP_REPLY_ERROR */

/*
 *  TINET 独自 API
 */

#ifdef ICMP_CFG_CALLBACK_ECHO_REPLY

extern void icmp_echo_reply (T_NET_BUF *input, uint_t ihoff);

#endif	/* of #ifdef ICMP_CFG_CALLBACK_ECHO_REPLY */

#endif	/* of #ifndef _IP_ICMP_H_ */
