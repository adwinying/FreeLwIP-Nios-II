#ifndef _TCP_H_
#define _TCP_H_

/*
 *  TCP ヘッダ、RFC 793 参照
 */

typedef struct t_tcp_hdr {
	uint16_t	sport;		/* 送信元ポート番号			*/
	uint16_t	dport;		/* 宛先ポート番号			*/

	uint32_t	seq;		/* SEQ 番号				*/

	uint32_t	ack;		/* ACK 番号				*/

	uint8_t		doff;		/* データオフセット(上位4ビット)	*/
	uint8_t		flags;		/* フラグ(下位6ビット)			*/
	uint16_t	win;		/* ウィンドサイズ			*/

	uint16_t	sum;		/* チェックサム				*/
	uint16_t	urp;		/* 緊急ポインタ				*/
	} T_TCP_HDR;

#define TCP_HDR_SIZE			(sizeof(T_TCP_HDR))

/*
 *  TCP ヘッダ構造体メンバアクセスマクロ
 */

#define TCP_MAKE_DATA_OFF(o)	((uint8_t)(((uint8_t)(o) << 2) & 0xf0))
#define TCP_DATA_OFF(o)		(((uint8_t)(o) & UINT_C(0xf0)) >> 2)
#define TCP_HDR_LEN(o)		(TCP_DATA_OFF(o))

/*
 *  TCP ヘッダのフラグフィールドの値の定義
 */

#define TCP_FLG_FIN		UINT_C(0x01)	/* FIN:	最終			*/
#define TCP_FLG_SYN		UINT_C(0x02)	/* SYN:	同期			*/
#define TCP_FLG_RST		UINT_C(0x04)	/* RST:	コネクションのリセット	*/
#define TCP_FLG_PUSH		UINT_C(0x08)	/* PUSH:プッシュ機能		*/
#define TCP_FLG_ACK		UINT_C(0x10)	/* ACK:	肯定応答		*/
#define TCP_FLG_URG		UINT_C(0x20)	/* URG:	緊急情報		*/

/*
 *  TCP オプションの定義
 */

/* オプションの型 */

#define TCP_OPT_EOL		UINT_C(0)	/* 最終				*/
#define TCP_OPT_NOP		UINT_C(1)	/* 何もしない			*/
#define TCP_OPT_MAXSEG		UINT_C(2)	/* 最大セグメント長		*/
#define TCP_OPT_WINDOW		UINT_C(3)	/* ウィンドスケーリング		*/
#define TCP_OPT_SACK_PERM	UINT_C(4)	/* 選択 ACK 許可		*/
#define TCP_OPT_SACK		UINT_C(5)	/* 選択 ACK			*/
#define TCP_OPT_TIMESTAMP	UINT_C(8)	/* タイムスタンプ		*/
#define TCP_OPT_CC		UINT_C(11)	/* 接続カウント			*/
#define TCP_OPT_CCNEW		UINT_C(12)	/* 接続カウント			*/
#define TCP_OPT_CCECHO		UINT_C(13)	/* 接続カウント			*/

/* オプションの長さ */

#define TCP_OPT_LEN_MAXSEG	4		/* 最大セグメント長		*/
#define TCP_OPT_LEN_WINDOW	3		/* ウィンドスケーリング		*/
#define TCP_OPT_LEN_SACK_PERM	4		/* 選択 ACK 許可		*/
#define TCP_OPT_LEN_SACK	5		/* 選択 ACK			*/
#define TCP_OPT_LEN_TIMESTAMP	10		/* タイムスタンプ		*/
#define TCP_OPT_LEN_CC		6		/* 接続カウント			*/
#define TCP_OPT_LEN_CCNEW	6		/* 接続カウント			*/
#define TCP_OPT_LEN_CCECHO	6		/* 接続カウント			*/

/*
 *  TCP 最大セグメントサイズ
 */

/*
 *  IPv4 の TCP 最大セグメントサイズ
 *  IP4_MSS は 576 で、標準の IPv4 ヘッダ長 (20) と
 *  TCP ヘッダ長 (20) を引いた値 536 であるが、512 を標準とする。
 */

#define TCP_MSS			UINT_C(512)

/*
 *  IPv6 の TCP 最大セグメントサイズ
 *  IPV6_MMTU は 1280 で、標準の IPv6 ヘッダ長 (40) と
 *  TCP ヘッダ長 (20) を引いた値 1220 であるが、1024 を標準とする。
 */

#define TCP6_MSS		UINT_C(1024)

/*
 *  TCP 最小セグメントサイズ
 *
 *  256（最小リンク MTU）- 40 = 216
 */

#define TCP_MINMSS		UINT_C(216)

/*
 *  TCP 最大ウィンドウサイズ
 */

#define MAX_TCP_WIN_SIZE	UINT_C(65535)

#endif	/* of #ifndef _TCP_H_ */
