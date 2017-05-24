#ifndef _NET_COUNT_H_
#define _NET_COUNT_H_

/*
 *  �ͥåȥ�����׾���η�¬
 */

#if NET_COUNT_ENABLE != 0

#ifndef _MACRO_ONLY

#ifdef INT64_MAX
typedef uint64_t T_NET_COUNT_VAL;
#else
typedef uint32_t T_NET_COUNT_VAL;
#endif

typedef struct t_net_count {
	T_NET_COUNT_VAL	in_octets;		/* ���������ƥåȿ�	*/
	T_NET_COUNT_VAL	out_octets;		/* ���������ƥåȿ�	*/
	T_NET_COUNT_VAL	in_packets;		/* �����Х��åȿ�	*/
	T_NET_COUNT_VAL	out_packets;		/* �����Х��åȿ�	*/
	T_NET_COUNT_VAL	in_err_packets;		/* �������顼�Х��åȿ�	*/
	T_NET_COUNT_VAL	out_err_packets;	/* �������顼�Х��åȿ�	*/
	} T_NET_COUNT;

#endif	/* of #ifndef _MACRO_ONLY */

#endif	/* of #if NET_COUNT_ENABLE != 0 */

#ifdef SUPPORT_PPP

/* PPP */

#if NET_COUNT_ENABLE & PROTO_FLG_PPP

#define NET_COUNT_PPP(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT	net_count_ppp;
extern T_NET_COUNT_VAL	net_count_ppp_no_bufs;	/* net_buf ������Ƽ��Կ�	*/

#endif	/* of #ifndef _MACRO_ONLY */

#define NC_PPP_SIZE	7			/* �����󥿿� */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP */

#define NET_COUNT_PPP(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP */

/* PPP HDLC */

#if NET_COUNT_ENABLE & PROTO_FLG_PPP_HDLC

#define NET_COUNT_PPP_HDLC(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT	net_count_hdlc;

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_HDLC */

#define NET_COUNT_PPP_HDLC(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_HDLC */

/* PPP LCP */

#if NET_COUNT_ENABLE & PROTO_FLG_PPP_LCP

#define NET_COUNT_PPP_LCP(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT_VAL net_count_ppp_lcp_in_octets;
extern T_NET_COUNT_VAL net_count_ppp_lcp_in_packets;

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_LCP */

#define NET_COUNT_PPP_LCP(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_LCP */

/* PPP IPCP */

#if NET_COUNT_ENABLE & PROTO_FLG_PPP_IPCP

#define NET_COUNT_PPP_IPCP(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT_VAL net_count_ppp_ipcp_in_octets;
extern T_NET_COUNT_VAL net_count_ppp_ipcp_in_packets;

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_IPCP */

#define NET_COUNT_PPP_IPCP(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_IPCP */

/* PPP CCP */

#if NET_COUNT_ENABLE & PROTO_FLG_PPP_CCP

#define NET_COUNT_PPP_CCP(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT_VAL net_count_ppp_ccp_in_octets;
extern T_NET_COUNT_VAL net_count_ppp_ccp_in_packets;

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_CCP */

#define NET_COUNT_PPP_CCP(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_CCP */

/* PPP PAP */

#if NET_COUNT_ENABLE & PROTO_FLG_PPP_PAP

#define NET_COUNT_PPP_PAP(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT_VAL net_count_ppp_upap_in_octets;
extern T_NET_COUNT_VAL net_count_ppp_upap_in_packets;

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_PAP */

#define NET_COUNT_PPP_PAP(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_PPP_PAP */

#endif	/* of #ifdef SUPPORT_PPP */

#ifdef SUPPORT_LOOP

/* �롼�ץХå� */

#if NET_COUNT_ENABLE & PROTO_FLG_LOOP

#define NET_COUNT_LOOP(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT	net_count_loop;

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_LOOP */

#define NET_COUNT_LOOP(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_LOOP */

#endif	/* of #ifdef SUPPORT_LOOP */

#ifdef SUPPORT_ETHER

/* Ethernet */

#if NET_COUNT_ENABLE & PROTO_FLG_ETHER

#define NET_COUNT_ETHER(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT	net_count_ether;

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ETHER */

#define NET_COUNT_ETHER(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ETHER */

/* NIC (if_tse) */

#if NET_COUNT_ENABLE & PROTO_FLG_ETHER_NIC

#define NC_ETHER_NIC_RESETS		0	/* �ꥻ�åȿ�			*/
#define NC_ETHER_NIC_TXB_QOVRS		1	/* �������ޥե����ֵѾ�¥����С���*/
#define NC_ETHER_NIC_TIMEOUTS		2	/* ���������ॢ���ȿ�		*/
#define NC_ETHER_NIC_COLS		3	/* ���ꥸ����			*/
#define NC_ETHER_NIC_OUT_ERR_PACKETS	4	/* �������顼�Х��åȿ�		*/
#define NC_ETHER_NIC_OUT_PACKETS	5	/* �����Х��åȿ�		*/
#define NC_ETHER_NIC_OUT_OCTETS		6	/* ���������ƥåȿ�		*/
#define NC_ETHER_NIC_RXB_QOVRS		7	/* �������ޥե����ֵѾ�¥����С���*/
#define NC_ETHER_NIC_NO_BUFS		8	/* net_buf ������Ƽ��Կ�	*/
#define NC_ETHER_NIC_IN_ERR_PACKETS	9	/* �������顼�Х��åȿ�		*/
#define NC_ETHER_NIC_IN_PACKETS		10	/* �����Х��åȿ�		*/
#define NC_ETHER_NIC_IN_OCTETS		11	/* ���������ƥåȿ�		*/
#define NC_ETHER_NIC_SIZE		12	/* �����󥿿�			*/

#define NET_COUNT_ETHER_NIC(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT_VAL net_count_ether_nic[NC_ETHER_NIC_SIZE];

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ETHER_NIC */

#define NET_COUNT_ETHER_NIC(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ETHER_NIC */

#endif	/* of #ifdef SUPPORT_ETHER */

/* �ͥåȥ���Хåե� */

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

#define NET_COUNT_NET_BUF(v,c)		((v)+=(c))

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

#define NET_COUNT_NET_BUF(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

#if defined(SUPPORT_INET4)

/* ARP */

#if NET_COUNT_ENABLE & PROTO_FLG_ARP

#define NET_COUNT_ARP(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT	net_count_arp;

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ARP */

#define NET_COUNT_ARP(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ARP */

/* IPv4 */

#if NET_COUNT_ENABLE & PROTO_FLG_IP4

#define NC_IP4_FRAG_OUT_FRAGS		0	/* ʬ���������ե饰���ȿ�	*/
#define NC_IP4_FRAG_OUT			1	/* ʬ��������			*/
#define NC_IP4_OUT_ERR_PACKETS		2	/* �������顼�ǡ���������	*/
#define NC_IP4_OUT_PACKETS		3	/* �����ǡ���������		*/
#define NC_IP4_OUT_OCTETS		4	/* ���������ƥåȿ�		*/
#define NC_IP4_FRAG_IN_TMOUT		5	/* ʬ����������ॢ���ȿ�	*/
#define NC_IP4_FRAG_IN_NO_BUF		6	/* ʬ������Хåե������Ƽ��Կ�	*/
#define NC_IP4_FRAG_IN_DROP		7	/* ʬ������˴���		*/
#define NC_IP4_FRAG_IN_OK		8	/* ʬ������ƹ���������		*/
#define NC_IP4_FRAG_IN_FRAGS		9	/* ʬ������ե饰���ȿ�	*/
#define NC_IP4_FRAG_IN			10	/* ʬ�������			*/
#define NC_IP4_OPTS			11	/* ���ץ�������Ͽ�		*/
#define NC_IP4_IN_ERR_PROTO		12	/* �ץ�ȥ��륨�顼��		*/
#define NC_IP4_IN_ERR_ADDR		13	/* ���ɥ쥹���顼��		*/
#define NC_IP4_IN_ERR_VER		14	/* �С�����󥨥顼��		*/
#define NC_IP4_IN_ERR_SHORT		15	/* Ĺ�����顼��			*/
#define NC_IP4_IN_ERR_CKSUM		16	/* �����å����२�顼��		*/
#define NC_IP4_IN_ERR_PACKETS		17	/* �������顼�ǡ���������	*/
#define NC_IP4_IN_PACKETS		18	/* �����ǡ���������		*/
#define NC_IP4_IN_OCTETS		19	/* ���������ƥåȿ�		*/
#define NC_IP4_SIZE			20	/* �����󥿿�			*/

#define NET_COUNT_IP4(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT_VAL net_count_ip4[NC_IP4_SIZE];

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_IP4 */

#define NET_COUNT_IP4(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_IP4 */

/* ICMP4 */

#if NET_COUNT_ENABLE & PROTO_FLG_ICMP4

#define NET_COUNT_ICMP4(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT	net_count_icmp4;

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ICMP4 */

#define NET_COUNT_ICMP4(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ICMP4 */

#endif	/* of #if defined(SUPPORT_INET4) */

#if defined(SUPPORT_INET6)

/* IPv6 */

#if NET_COUNT_ENABLE & PROTO_FLG_IP6

#define NC_IP6_FRAG_OUT_FRAGS		0	/* ʬ���������ե饰���ȿ�	*/
#define NC_IP6_FRAG_OUT			1	/* ʬ��������			*/
#define NC_IP6_OUT_ERR_PACKETS		2	/* �������顼�ǡ���������	*/
#define NC_IP6_OUT_PACKETS		3	/* �����ǡ���������		*/
#define NC_IP6_OUT_OCTETS		4	/* ���������ƥåȿ�		*/
#define NC_IP6_FRAG_IN_TMOUT		5	/* ʬ����������ॢ���ȿ�	*/
#define NC_IP6_FRAG_IN_NO_BUF		6	/* ʬ������Хåե������Ƽ��Կ�	*/
#define NC_IP6_FRAG_IN_DROP		7	/* ʬ������˴���		*/
#define NC_IP6_FRAG_IN_OK		8	/* ʬ������ƹ���������		*/
#define NC_IP6_FRAG_IN_FRAGS		9	/* ʬ������ե饰���ȿ�	*/
#define NC_IP6_FRAG_IN			10	/* ʬ�������			*/
#define NC_IP6_IN_ERR_PROTO		11	/* �ץ�ȥ��륨�顼��		*/
#define NC_IP6_IN_ERR_ADDR		12	/* ���ɥ쥹���顼��		*/
#define NC_IP6_IN_ERR_VER		13	/* �С�����󥨥顼��		*/
#define NC_IP6_IN_ERR_SHORT		14	/* Ĺ�����顼��			*/
#define NC_IP6_IN_ERR_PACKETS		15	/* �������顼�ǡ���������	*/
#define NC_IP6_IN_PACKETS		16	/* �����ǡ���������		*/
#define NC_IP6_IN_OCTETS		17	/* ���������ƥåȿ�		*/
#define NC_IP6_SIZE			18	/* �����󥿿�			*/

#define NET_COUNT_IP6(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT_VAL net_count_ip6[NC_IP6_SIZE];

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_IP6 */

#define NET_COUNT_IP6(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_IP6 */

#if NET_COUNT_ENABLE & PROTO_FLG_ICMP6

#define NC_ICMP6_OUT_ERR_PACKETS	0	/* �������顼 ICMP �ǡ�����	*/
#define NC_ICMP6_OUT_PACKETS		1	/* ���� ICMP �ǡ�����		*/
#define NC_ICMP6_OUT_OCTETS		2	/* ���� ICMP �����ƥåȿ�	*/
#define NC_ICMP6_IN_ERR_CKSUM		3	/* Ĺ�����顼��			*/
#define NC_ICMP6_IN_ERR_PACKETS		4	/* �������顼 ICMP �ǡ�����	*/
#define NC_ICMP6_IN_PACKETS		5	/* ���� ICMP �ǡ�����		*/
#define NC_ICMP6_IN_OCTETS		6	/* ���� ICMP �����ƥåȿ�	*/
#define NC_ICMP6_SIZE			7	/* �����󥿿�			*/

#define NET_COUNT_ICMP6(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT_VAL	net_count_icmp6[NC_ICMP6_SIZE];

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ICMP6 */

#define NET_COUNT_ICMP6(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ICMP6 */

#if NET_COUNT_ENABLE & PROTO_FLG_ND6

#define NC_ND6_DAD_OUT_PACKETS		0	/* ��ʣ���ɥ쥹����������	*/
#define NC_ND6_NS_OUT_PACKETS		1	/* ��������������		*/
#define NC_ND6_NS_IN_PACKETS		2	/* ��������������		*/
#define NC_ND6_NA_OUT_PACKETS		3	/* ��������������		*/
#define NC_ND6_NA_IN_PACKETS		4	/* �������μ�����		*/
#define NC_ND6_RS_OUT_PACKETS		5	/* �롼���������Ͽ�		*/
#define NC_ND6_RA_IN_PACKETS		6	/* �롼�����μ�����		*/
#define NC_ND6_SIZE			7	/* �����󥿿�			*/

#define NET_COUNT_ND6(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT_VAL	net_count_nd6[NC_ND6_SIZE];

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ND6 */

#define NET_COUNT_ND6(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_ND6 */

#endif	/* of #if defined(SUPPORT_INET6) */

#ifdef SUPPORT_TCP

/* TCP */

#if NET_COUNT_ENABLE & PROTO_FLG_TCP

#define NC_TCP_CONNECTS			0	/* ǽư�����ץ��		*/
#define NC_TCP_ACCEPTS			1	/* ��ư�����ץ��		*/
#define NC_TCP_RTT_UPDATES		2	/* RTT ������			*/
#define NC_TCP_SEND_RSTS		3	/* ������RST ��			*/
#define NC_TCP_SEND_ACKS		4	/* ������ACK ��			*/
#define NC_TCP_SEND_URG_SEGS		5	/* �������۵ޥ������ȿ�	*/
#define NC_TCP_SEND_DATA_SEGS		6	/* �����ǡ����������ȿ�	*/
#define NC_TCP_SEND_REXMIT_SEGS		7	/* �������������ȿ�		*/
#define NC_TCP_SEND_SEGS		8	/* �����������ȿ�		*/
#define NC_TCP_SEND_CNTL_SEGS		9	/* �������楻�����ȿ�		*/
#define NC_TCP_SEND_DATA_OCTETS		10	/* �����ǡ��������ƥåȿ�	*/
#define NC_TCP_FREE_RCV_QUEUE		11	/* �������塼������		*/
#define NC_TCP_RECV_DUP_SEGS		12	/* ������¿�ſ�			*/
#define NC_TCP_RECV_DROP_SEGS		13	/* �������˴���			*/
#define NC_TCP_RECV_RSTS		14	/* ������RST ��			*/
#define NC_TCP_RECV_DUP_ACKS		15	/* ������¿�� ACK ��		*/
#define NC_TCP_RECV_ACKS		16	/* ������ACK ��			*/
#define NC_TCP_RECV_BAD_CKSUMS		17	/* �����������å�����������	*/
#define NC_TCP_RECV_BAD_HEADERS		18	/* �������إå�������		*/
#define NC_TCP_RECV_URG_SEGS		19	/* �������۵ޥ������ȿ�	*/
#define NC_TCP_RECV_DATA_SEGS		20	/* �����ǡ����������ȿ�	*/
#define NC_TCP_RECV_SEGS		21	/* �����������ȿ�		*/
#define NC_TCP_RECV_DATA_OCTETS		22	/* �����ǡ��������ƥåȿ�	*/
#define NC_TCP_RECV_OCTETS		23	/* ���������ƥåȿ�		*/
#define NC_TCP_SIZE			24	/* �����󥿿�			*/

#define NET_COUNT_TCP(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT_VAL net_count_tcp[NC_TCP_SIZE];

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_TCP */

#define NET_COUNT_TCP(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_TCP */

#endif	/* of #ifdef SUPPORT_TCP */

#ifdef SUPPORT_UDP

/* UDP */

#if NET_COUNT_ENABLE & PROTO_FLG_UDP

#define NET_COUNT_UDP(v,c)	((v)+=(c))

#ifndef _MACRO_ONLY

extern T_NET_COUNT	net_count_udp;

#endif	/* of #ifndef _MACRO_ONLY */

#else	/* of #if NET_COUNT_ENABLE & PROTO_FLG_UDP */

#define NET_COUNT_UDP(v,c)

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_UDP */

#endif	/* of #ifdef SUPPORT_UDP */

#ifdef SUPPORT_MIB

/*
 *  SNMP �� ��������١��� (MIB)
 */

#define NET_COUNT_MIB(v, c)		((v)+=(c))

#else	/* of #ifdef SUPPORT_MIB */

#define NET_COUNT_MIB(v, c)

#endif	/* of #ifdef SUPPORT_MIB */

#endif	/* of #ifndef _NET_COUNT_H_ */
