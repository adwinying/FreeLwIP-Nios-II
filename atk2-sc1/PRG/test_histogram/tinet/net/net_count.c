/*
 *  ネットワーク統計情報の計測
 */

#ifdef TARGET_KERNEL_ATK2

#include "Os.h"
#include "prc_sil.h"

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

#include <tinet_defs.h>
#include <tinet_config.h>

#include <net/net.h>
#include <net/net_count.h>

/*
 *  計測用変数
 */

#ifdef SUPPORT_PPP

#if NET_COUNT_ENABLE & PROTO_FLG_PPP

T_NET_COUNT	net_count_ppp;
T_NET_COUNT_VAL	net_count_ppp_no_bufs;		/* net_buf 割り当て失敗数	*/

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP */

#if NET_COUNT_ENABLE & PROTO_FLG_PPP_LCP

T_NET_COUNT_VAL	net_count_ppp_lcp_in_octets;
T_NET_COUNT_VAL	net_count_ppp_lcp_in_packets;

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_LCP */

#if NET_COUNT_ENABLE & PROTO_FLG_PPP_IPCP

T_NET_COUNT_VAL	net_count_ppp_ipcp_in_octets;
T_NET_COUNT_VAL	net_count_ppp_ipcp_in_packets;

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_IPCP */

#if NET_COUNT_ENABLE & PROTO_FLG_PPP_PAP

T_NET_COUNT_VAL	net_count_ppp_upap_in_octets;
T_NET_COUNT_VAL	net_count_ppp_upap_in_packets;

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_PAP */

#if NET_COUNT_ENABLE & PROTO_FLG_PPP_HDLC

T_NET_COUNT	net_count_hdlc;

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_HDLC */

#endif	/* of #ifdef SUPPORT_PPP */

#ifdef SUPPORT_LOOP

#if NET_COUNT_ENABLE & PROTO_FLG_LOOP

T_NET_COUNT	net_count_loop;

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_LOOP */

#endif	/* of #ifdef SUPPORT_LOOP */

#ifdef SUPPORT_ETHER

#if NET_COUNT_ENABLE & PROTO_FLG_ETHER

T_NET_COUNT	net_count_ether;

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ETHER */

#if NET_COUNT_ENABLE & PROTO_FLG_ETHER_NIC

T_NET_COUNT_VAL net_count_ether_nic[NC_ETHER_NIC_SIZE];

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ETHER_NIC */

#endif	/* of #ifdef SUPPORT_ETHER */

#if defined(SUPPORT_INET4)

#if NET_COUNT_ENABLE & PROTO_FLG_ARP

T_NET_COUNT	net_count_arp;

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ARP */

#if NET_COUNT_ENABLE & PROTO_FLG_IP4

T_NET_COUNT_VAL net_count_ip4[NC_IP4_SIZE];

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_IP4 */

#if NET_COUNT_ENABLE & PROTO_FLG_ICMP4

T_NET_COUNT	net_count_icmp4;

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ICMP4 */

#endif	/* of #if defined(SUPPORT_INET4) */

#if defined(SUPPORT_INET6)

#if NET_COUNT_ENABLE & PROTO_FLG_IP6

T_NET_COUNT_VAL net_count_ip6[NC_IP6_SIZE];

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_IP6 */

#if NET_COUNT_ENABLE & PROTO_FLG_ICMP6

T_NET_COUNT_VAL	net_count_icmp6[NC_ICMP6_SIZE];

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ICMP6 */

#if NET_COUNT_ENABLE & PROTO_FLG_ND6

T_NET_COUNT_VAL	net_count_nd6[NC_ND6_SIZE];

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ND6 */

#endif	/* of #if defined(SUPPORT_INET6) */

#ifdef SUPPORT_TCP

#if NET_COUNT_ENABLE & PROTO_FLG_TCP

T_NET_COUNT_VAL net_count_tcp[NC_TCP_SIZE];

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_TCP */

#endif	/* of #ifdef SUPPORT_TCP */

#ifdef SUPPORT_UDP

#if NET_COUNT_ENABLE & PROTO_FLG_UDP

T_NET_COUNT	net_count_udp;

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_UDP */

#endif	/* of #ifdef SUPPORT_PPP */
