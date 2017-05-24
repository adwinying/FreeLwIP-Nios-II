#ifndef _TINET_CONFIG_H_
#define _TINET_CONFIG_H_

/*
 *  TINET 1.2 との互換性のための定義
 */

/* TCPの受動オープンをサポートする。*/

#define TCP_CFG_PASSIVE_OPEN

/* TCPのノンブロッキングコール機能を組込む。*/

//#define TCP_CFG_NON_BLOCKING

/* UDPのノンブロッキングコール機能を組込む。*/

//#define UDP_CFG_NON_BLOCKING

/*
 *  NIC、システム、CPU、応用プログラムに
 *  依存する定義ファイルをインクルードする。
 */

#if defined(TARGET_KERNEL_ATK2)

#ifdef SUPPORT_ETHER

#include "tinet_nic_config.h"

#endif	/* of #ifdef SUPPORT_ETHER */

#include "tinet_target_config.h"
#include "tinet_app_config.h"

#endif	/* of #if defined(TARGET_KERNEL_ATK2) */

/*
 *  TINET 1.1 との互換性のための定義
 */

#ifdef  TMO_ICMP4_OUTPUT
#define TMO_ICMP_OUTPUT	TMO_ICMP4_OUTPUT
#endif

/*
 *  TINET 1.2 との互換性のための定義
 */

#ifdef  NUM_MPF_NET_BUF_4096
#define NUM_MPF_NET_BUF_REASSM	NUM_MPF_NET_BUF_4096
#endif

/*
 *  TINET 1.3 との互換性のための定義

#ifdef  NUM_MPF_NET_BUF_REASSM
#define NUM_MPF_NET_BUF4_REASSM	NUM_MPF_NET_BUF_REASSM
#endif
 */

/*
 *  TCP に関する定義
 */

/*
 *  MAX_TCP_RCV_SEG: 受信セグメントサイズの最大値
 *
 *    注意: 変更すべきではない。
 */

#ifndef MAX_TCP_RCV_SEG
#define MAX_TCP_RCV_SEG		(IF_MTU - (IP_HDR_SIZE + TCP_HDR_SIZE))
#endif

/*
 *  DEF_TCP_SND_SEG: 送信セグメントサイズの規定値
 */

#ifndef DEF_TCP_SND_SEG
#define DEF_TCP_SND_SEG		MAX_TCP_SND_SEG
#endif

/*
 *  TCP 通信端点の送受信ウィンドバッファの省コピー機能
 */

#ifdef  TCP_CFG_SWBUF_CSAVE_ONLY
#ifndef TCP_CFG_SWBUF_CSAVE
#define TCP_CFG_SWBUF_CSAVE
#endif
#endif	/* of #ifdef  TCP_CFG_SWBUF_CSAVE_ONLY */

#ifdef  TCP_CFG_RWBUF_CSAVE_ONLY
#ifndef TCP_CFG_RWBUF_CSAVE
#define TCP_CFG_RWBUF_CSAVE
#endif
#endif	/* of #ifdef  TCP_CFG_RWBUF_CSAVE_ONLY */

/*
 *  TCP_CFG_URG_OFFSET: 緊急データの最後のバイトのオフセット
 *
 *    -1: BSD の実装、緊急ポインタは、緊急データの最後のバイトの次のバイトを差す。
 *     0: RFC1122 の規定、緊急ポインタは、緊急データの最後のバイトを差す。
 */

#ifndef TCP_CFG_URG_OFFSET
#define TCP_CFG_URG_OFFSET	-1
#endif

/*
 *  ARP に関する定義
 */

#ifndef ARP_CACHE_KEEP
#define ARP_CACHE_KEEP		(20*60*NET_TIMER_HZ)	/* ARP キャッシュのタイムアウト	*/
#endif

/*
 *  ループバックインタフェースに関する定義
 */

#ifndef LOMTU
#define LOMTU			1500	/* ループバックの MTU			*/
#endif

/*
 * TINET サポート関数に関する定義
 */

#ifndef NUM_IPV4ADDR_STR_BUFF		/* ip2str における IP アドレス文字列用バッファ数 */

#if defined(SUPPORT_PPP)

#define NUM_IPV4ADDR_STR_BUFF	2

#else	/* of #if defined(SUPPORT_PPP) */

#define NUM_IPV4ADDR_STR_BUFF	1

#endif	/* of #if defined(SUPPORT_PPP) */

#endif	/* of #ifndef NUM_IPV4ADDR_STR_BUFF */

#ifndef NUM_IPV6ADDR_STR_BUFF
#define NUM_IPV6ADDR_STR_BUFF	2	/* ip62str における IP アドレス文字列用バッファ数 */
#endif

#ifndef NUM_MACADDR_STR_BUFF		/* macstr における MAC アドレス文字列用バッファ数 */

#if defined(SUPPORT_INET4) && defined(ETHER_CFG_ACCEPT_ALL) && defined(ETHER_CFG_MCAST_WARNING)

#define NUM_MACADDR_STR_BUFF	2

#else	/* of #if defined(SUPPORT_INET4) && defined(ETHER_CFG_ACCEPT_ALL) && defined(ETHER_CFG_MCAST_WARNING) */

#define NUM_MACADDR_STR_BUFF	1

#endif	/* of #if defined(SUPPORT_INET4) && defined(ETHER_CFG_ACCEPT_ALL) && defined(ETHER_CFG_MCAST_WARNING) */

#endif	/* of #ifndef NUM_MACADDR_STR_BUFF */

/*
 *  TINET 内部のタイムアウト値の設定
 */

#ifndef TMO_IP4_FRAG_GET_NET_BUF
#define TMO_IP4_FRAG_GET_NET_BUF	ULONG_C(1000)	/* [ms]、IPv4 データグラム再構成用ネットワークバッファ獲得タイムアウト	*/
#endif

#ifndef TMO_IP6_FRAG_GET_NET_BUF
#define TMO_IP6_FRAG_GET_NET_BUF	ULONG_C(1000)	/* [ms]、IPv6 データグラム再構成用ネットワークバッファ獲得タイムアウト	*/
#endif

#ifndef TMO_PPP_GET_NET_BUF
#define TMO_PPP_GET_NET_BUF		ULONG_C(1000)	/* [ms]、PPP 受信用ネットワークバッファ獲得タイムアウト	*/
#endif

#ifndef TMO_ARP_GET_NET_BUF
#define TMO_ARP_GET_NET_BUF		ULONG_C(1000)	/* [ms]、ARP のネットワークバッファ獲得タイムアウト	*/
#endif

#ifndef TMO_TCP_GET_NET_BUF
#define TMO_TCP_GET_NET_BUF		ULONG_C(1000)	/* [ms]、TCP のネットワークバッファ獲得タイムアウト	*/
#endif

#ifndef TMO_TCP_OUTPUT
#define TMO_TCP_OUTPUT			ULONG_C(1000)	/* [ms]、TCP 出力タイムアウト				*/
#endif

#ifndef TMO_UDP_OUTPUT
#define TMO_UDP_OUTPUT			ULONG_C(1000)	/* [ms]、UDP 出力タイムアウト				*/
#endif

#ifndef TMO_ICMP_OUTPUT
#define TMO_ICMP_OUTPUT			ULONG_C(1000)	/* [ms]、ICMP 出力タイムアウト				*/
#endif

#ifndef TMO_ND6_NS_OUTPUT
#define TMO_ND6_NS_OUTPUT		ULONG_C(1000)	/* [ms]、近隣要請出力タイムアウト			*/
#endif

#ifndef TMO_ND6_NA_OUTPUT
#define TMO_ND6_NA_OUTPUT		ULONG_C(1000)	/* [ms]、近隣通知出力タイムアウト			*/
#endif

#ifndef TMO_ND6_RS_OUTPUT
#define TMO_ND6_RS_OUTPUT		ULONG_C(1000)	/* [ms]、ルータ要請出力タイムアウト			*/
#endif

#ifndef TMO_ARP_OUTPUT
#define TMO_ARP_OUTPUT			ULONG_C(1000)	/* [ms]、アドレス解決要求出力タイムアウト		*/
#endif

#ifndef TMO_ND6_RTR_SOL_DELAY
#define TMO_ND6_RTR_SOL_DELAY		ULONG_C(1000)	/* [ms]、ルータ要請出力遅延				*/
#endif

#ifndef TMO_ND6_RTR_SOL_INTERVAL
#define TMO_ND6_RTR_SOL_INTERVAL	ULONG_C(4000)	/* [ms]、ルータ要請出力間隔				*/
#endif

#ifndef TMO_IN_REDIRECT
					/* [s]、向け直しルーティングエントリタイムアウト*/
#define TMO_IN_REDIRECT			(10*60*NET_TIMER_HZ)
#endif

/*
 *  TINET 内部のタスクのスタックサイズの設定
 */

#ifndef TCP_OUT_TASK_STACK_SIZE
#define	TCP_OUT_TASK_STACK_SIZE	1024	/* TCP 出力タスクのスタックサイズ		*/
#endif

#ifndef UDP_OUT_TASK_STACK_SIZE
#define	UDP_OUT_TASK_STACK_SIZE	1024	/* UDP 出力タスクのスタックサイズ		*/
#endif

#ifndef PPP_INPUT_STACK_SIZE
#define	PPP_INPUT_STACK_SIZE	1024	/* PPP タスクのスタックサイズ			*/
#endif

#ifndef PPP_OUTPUT_STACK_SIZE
#define	PPP_OUTPUT_STACK_SIZE	1024	/* PPP 出力タスクのスタックサイズ		*/
#endif

#ifndef ETHER_INPUT_STACK_SIZE
#define	ETHER_INPUT_STACK_SIZE	1024	/* Ethernet 入力タスクのスタックサイズ		*/
#endif

#ifndef ETHER_OUTPUT_STACK_SIZE
#define	ETHER_OUTPUT_STACK_SIZE	1024	/* Ethernet 出力タスクのスタックサイズ		*/
#endif

#ifndef LOOP_INPUT_STACK_SIZE
#define	LOOP_INPUT_STACK_SIZE	1024	/* ループバック入力タスクのスタックサイズ	*/
#endif

#ifndef LOOP_OUTPUT_STACK_SIZE
#define	LOOP_OUTPUT_STACK_SIZE	1024	/* ループバック出力タスクのスタックサイズ	*/
#endif

#ifndef NET_TIMER_STACK_SIZE
#define	NET_TIMER_STACK_SIZE	1024	/* ネットワークタイマタスクのスタックサイズ	*/
#endif

/*
 *  TINET 内部のタスクの優先度の設定
 */

#ifndef TCP_OUT_TASK_PRIORITY
#define TCP_OUT_TASK_PRIORITY	5	/* TCP 出力タスクの優先度		*/
#endif

#ifndef UDP_OUT_TASK_PRIORITY
#define UDP_OUT_TASK_PRIORITY	5	/* UDP 出力タスクの優先度		*/
#endif

#ifndef PPP_INPUT_PRIORITY
#define PPP_INPUT_PRIORITY	5	/* PPP タスクの優先度			*/
#endif

#ifndef PPP_OUTPUT_PRIORITY
#define PPP_OUTPUT_PRIORITY	5	/* PPP タスクの優先度			*/
#endif

#ifndef ETHER_INPUT_PRIORITY
#define ETHER_INPUT_PRIORITY	5	/* Ethernet 入力タスクの優先度		*/
#endif

#ifndef ETHER_OUTPUT_PRIORITY
#define ETHER_OUTPUT_PRIORITY	5	/* Ethernet 出力タスクの優先度		*/
#endif

#ifndef LOOP_INPUT_PRIORITY
#define LOOP_INPUT_PRIORITY	5	/* ループバック入力タスクの優先度	*/
#endif

#ifndef LOOP_OUTPUT_PRIORITY
#define LOOP_OUTPUT_PRIORITY	5	/* ループバック出力タスクの優先度	*/
#endif

#ifndef NET_TIMER_PRIORITY
#define NET_TIMER_PRIORITY	5	/* ネットワークタイマタスクの優先度	*/
#endif

/*
 *  TINET 内部のデータキューのエントリ数の設定
 */

/* UDP 受信キューサイズ */

#ifndef NUM_DTQ_UDP_RCVQ
#define NUM_DTQ_UDP_RCVQ	1	/* 非コールバック用			*/
#endif

#ifndef NUM_DTQ_UDP_OUTPUT
#define NUM_DTQ_UDP_OUTPUT	1	/* UDP 出力データキューサイズ		*/
#endif

#ifndef NUM_DTQ_PPP_OUTPUT
#define NUM_DTQ_PPP_OUTPUT	2	/* PPP 出力データキューサイズ、2 以上	*/
#endif

#ifndef NUM_DTQ_ETHER_OUTPUT
#define NUM_DTQ_ETHER_OUTPUT	2	/* Ethernet 出力データキューサイズ	*/
#endif

#ifndef NUM_DTQ_LOOP_INPUT
#define NUM_DTQ_LOOP_INPUT	0	/* ループバック入力データキューサイズ	*/
#endif

#ifndef NUM_DTQ_LOOP_OUTPUT
#define NUM_DTQ_LOOP_OUTPUT	0	/* ループバック出力データキューサイズ	*/
#endif

#endif /* _TINET_CONFIG_H_ */
