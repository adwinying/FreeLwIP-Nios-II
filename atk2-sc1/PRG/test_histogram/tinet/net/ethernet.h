#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#ifdef SUPPORT_ETHER

/*
 *  フィールド長
 */

#define ETHER_ADDR_LEN		6	/* Ethernet (MAC) Address	*/
#define ETHER_TYPE_LEN		2	/* Type		*/
#define ETHER_CRC_LEN		4	/* CRC		*/

/*
 *  フレーム長 (Ethernet ヘッダと CRC を含む)
 */

#define ETHER_MIN_LEN		64
#define ETHER_MAX_LEN		(IF_MTU + sizeof(T_ETHER_HDR) + ETHER_CRC_LEN)

/*
 *  Ethernet ヘッダ
 */

#ifndef IF_ETHER_NIC_HDR_ALIGN
#error IF_ETHER_NIC_HDR_ALIGN expected.
#endif	/* of #ifndef IF_ETHER_NIC_HDR_ALIGN */

typedef struct t_ether_header {

#if IF_ETHER_NIC_HDR_ALIGN != 0

	uint8_t		align[IF_ETHER_NIC_HDR_ALIGN];	/* アライン調整 */

#endif	/* of #if IF_ETHER_NIC_HDR_ALIGN != 0 */

	uint8_t		dhost[ETHER_ADDR_LEN];
	uint8_t		shost[ETHER_ADDR_LEN];
	uint16_t	type;
	} __attribute__((packed))T_ETHER_HDR;

#define GET_ETHER_HDR(nbuf)		((T_ETHER_HDR*)((nbuf)->buf))

/*
 *  MAC アドレスの定義
 */

#define ETHER_MCAST_ADDR	UINT_C(0x01)	/* マルチキャスト・アドレス	*/
#define ETHER_LOCAL_ADDR	UINT_C(0x02)	/* ローカル・アドレス		*/

typedef struct t_ether_addr {
	uint8_t		lladdr[ETHER_ADDR_LEN];
	} T_ETHER_ADDR;

/*
 *  Type フィールド
 */

#define ETHER_TYPE_IP		UINT_C(0x0800)	/* IPv4	*/
#define ETHER_TYPE_ARP		UINT_C(0x0806)	/* ARP	*/
#define ETHER_TYPE_IPV6		UINT_C(0x86dd)	/* IPv6	*/

/*
 *  インタフェースの選択マクロ
 */

#define T_IF_HDR		T_ETHER_HDR		/* インタフェースのヘッダ			*/
#define T_IF_ADDR		T_ETHER_ADDR		/* インタフェースのアドレス			*/
#define IF_HDR_ALIGN		2			/* ヘッダのアライン単位				*/
#define IF_MTU			1500			/* インタフェースの MTU				*/

#define IF_OUTPUT(o,d,g)	ether_output(o,d,g)	/* インタフェースの出力関数			*/
#define IF_RAW_OUTPUT(o)	ether_raw_output(o)	/* インタフェースの出力関数、アドレス解決無し	*/
#define IF_SET_PROTO(b,p)	(GET_ETHER_HDR(b)->type = htons(p))
							/* インタフェースのプロトコル設定関数		*/
#define IF_SOFTC_TO_IFADDR(s)	((T_IF_ADDR*)(s)->ifaddr.lladdr)
							/* ソフトウェア情報から MAC アドレスを取り出す	*/
#define IF_GET_IFNET()		ether_get_ifnet()	/* ネットワークインタフェース構造体を返す。	*/
#define IF_TYPE			IFT_ETHER		/* インターフェースの型				*/

/* IPv4 関係 */

#define IF_PROTO_IP		ETHER_TYPE_IP		/* インタフェースの IP プロトコル指定		*/
#define IF_PROTO_ARP		ETHER_TYPE_ARP		/* インタフェースの ARP プロトコル指定		*/

/* IPv6 関係 */

#define MAX_IF_MADDR_CNT	2			/* インタフェースのマルチキャストアドレス配列の最大サイズ	*/
#define IF_MADDR_INIT		{ { { 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, 0, 0, 0 } } }
							/* インタフェースのマルチキャストアドレス配列の初期化		*/
#define IF_PROTO_IPV6		ETHER_TYPE_IPV6		/* インタフェースの IP プロトコル指定				*/
#define IF_ADDMULTI(s)		IF_ETHER_NIC_ADDMULTI(s)
							/* マルチキャストアドレスの登録					*/
#define IF_IN6_NEED_CACHE(i)	(true)			/* 近隣探索キャッシュを使用する。				*/
#define IF_IN6_IFID(i,a)	IF_ETHER_NIC_IN6_IFID(i,a)
							/* インタフェース識別子の設定					*/
#define IF_IN6_RESOLVE_MULTICAST(i,m)	\
				ether_in6_resolve_multicast(i,m)
							/* インタフェースのマルチキャストアドレスへの変換		*/

/*
 *  前方参照
 */

#ifndef T_IFNET_DEFINED

typedef struct t_ifnet T_IFNET;

#define T_IFNET_DEFINED

#endif	/* of #ifndef T_IFNET_DEFINED */

#ifndef T_IF_SOFTC_DEFINED

typedef struct t_if_softc T_IF_SOFTC;

#define T_IF_SOFTC_DEFINED

#endif	/* of #ifndef T_IF_SOFTC_DEFINED */

#ifndef T_IN4_ADDR_DEFINED

typedef uint32_t T_IN4_ADDR;

#define T_IN4_ADDR_DEFINED

#endif	/* of #ifndef T_IN4_ADDR_DEFINED */

#ifndef T_NET_BUF_DEFINED

typedef struct t_net_buf T_NET_BUF;

#define T_NET_BUF_DEFINED

#endif	/* of #ifndef T_NET_BUF_DEFINED */

/*
 *  ネットワークインタフェースに依存しないソフトウェア情報
 */

#ifdef T_IF_ETHER_NIC_SOFTC

struct t_if_softc {
	T_IF_ADDR		ifaddr;			/* ネットワークインタフェースのアドレス	*/
	uint16_t		timer;			/* 送信タイムアウト			*/
	T_IF_ETHER_NIC_SOFTC	*sc;			/* ディバイス依存のソフトウェア情報	*/
	EventMaskType		mask_txb_ready;		/* 送信イベントマスク			*/
	EventMaskType		mask_rxb_ready;		/* 受信イベントマスク			*/

#ifdef SUPPORT_INET6

	T_IF_ADDR 	maddrs[MAX_IF_MADDR_CNT];	/* マルチキャストアドレスリスト	*/

#endif	/* of #ifdef SUPPORT_INET6 */
	};

#endif	/* of #ifdef T_IF_ETHER_NIC_SOFTC */

#ifdef SUPPORT_INET6

/* 前方参照 */

#ifndef T_IN6_ADDR_DEFINED

typedef struct t_in6_addr T_IN6_ADDR;

#define T_IN6_ADDR_DEFINED

#endif	/* of #ifndef T_IN6_ADDR_DEFINED */

/*
 *  関数
 */

extern ER ether_in6_resolve_multicast(T_ETHER_ADDR *ifaddr, T_IN6_ADDR *maddr);

#endif	/* of #ifdef SUPPORT_INET6 */

/*
 *  関数
 */

extern T_IFNET *ether_get_ifnet (void);
extern ER ether_output (T_NET_BUF *data, void *dst, T_IF_ADDR *gw);
extern ER ether_raw_output (T_NET_BUF *data);

#endif	/* of #ifdef SUPPORT_ETHER */

#endif	/* of #ifndef _ETHERNET_H_ */
