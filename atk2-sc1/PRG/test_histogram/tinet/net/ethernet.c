/*
 *  Ethernet 処理タスクの本体
 */

#include <string.h>

#ifdef TARGET_KERNEL_ATK2

#include "Os.h"
#include "prc_sil.h"
#include "t_syslog.h"

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

#include <tinet_defs.h>
#include <tinet_config.h>
#include <tinet_atk2_cfg.h>

#include <net/if.h>
#include <net/ethernet.h>
//#include <net/if_llc.h>
#include <net/if_arp.h>
#include <net/net.h>
#include <net/net_var.h>
#include <net/net_buf.h>
#include <net/net_timer.h>
#include <net/net_count.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netinet/if_ether.h>
//#include <netinet6/if6_ether.h>
//#include <netinet6/nd6.h>

#include <net/if_var.h>
//#include <net/if6_var.h>

#ifdef SUPPORT_ETHER

/*
 *  変数
 */

#if defined(SUPPORT_INET4)

const uint8_t ether_broad_cast_addr[ETHER_ADDR_LEN] = {
	UINT_C(0xff), UINT_C(0xff), UINT_C(0xff),
	UINT_C(0xff), UINT_C(0xff), UINT_C(0xff)
	};

#endif	/* of #if defined(SUPPORT_INET4) */

/*
 *  変数
 */

/* ネットワークインタフェース構造体 */

#if defined(SUPPORT_INET4)

static T_IFNET ether_ifnet = {
	NULL,			/* ネットワークインタフェースのソフトウェア情報	*/
	{
		IPV4_ADDR_LOCAL,	/* IPv4 アドレス		*/
		IPV4_ADDR_LOCAL_MASK,	/* サブネットマスク		*/
		},
	};

#endif	/* of #if defined(SUPPORT_INET4) */

#if defined(SUPPORT_INET6)

static T_IFNET ether_ifnet = {
	NULL,			/* ネットワークインタフェースのソフトウェア情報	*/
	{},			/* IPv6 アドレス情報				*/
	{},			/* マルチキャスト IPv6 アドレス			*/
	0,			/* フラグ					*/
	};

#endif	/* of #if defined(SUPPORT_INET6) */

#ifdef SUPPORT_MIB

/*
 *  SNMP の 管理情報ベース (MIB)
 */

T_IF_STATS if_stats;

#endif	/* of #ifdef SUPPORT_MIB */

#ifdef ETHER_CFG_802_WARNING

/*
 *  関数
 */

static void ieee_802_input (T_NET_BUF *input);

/*
 *  ieee_802_input -- IEEE 802.3 入力処理
 */

static void
ieee_802_input (T_NET_BUF *input)
{
	T_8023MAC_HDR	*mhdr;
	T_8022SNAP_HDR	*shdr;

	mhdr = GET_8023MAC_HDR(input);
	shdr = GET_8022SNAP_HDR(input);
	syslog(LOG_WARNING, "[ETHER] unexp 802.3 from: %s, proto: 0x%04x.",
	                    mac2str(NULL, mhdr->shost), ntohs(*(uint16_t*)&shdr->type));
	}

#endif	/* of #ifdef ETHER_CFG_802_WARNING */

/*
 *  ether_get_ifnet -- ネットワークインタフェース構造体を返す。
 */

T_IFNET *
ether_get_ifnet (void)
{
	return &ether_ifnet;
	}

#if defined(SUPPORT_INET6)

/*
 *  ether_in6_resolve_multicast -- イーサネットのマルチキャストアドレスへの変換
 */

ER
ether_in6_resolve_multicast (T_ETHER_ADDR *ifaddr, T_IN6_ADDR *maddr)
{
	/* マルチキャストアドレスかチェックする。*/
	if (!IN6_IS_ADDR_MULTICAST(maddr))
		return E_PAR;

	/* イーサネットのマルチキャストアドレスへの変換 */
	ether_map_ipv6_multicast(ifaddr, maddr);

	return E_OK;
	}

#endif	/* of #if defined(SUPPORT_INET6) */

/*
 *  ether_raw_output -- Ethernet インタフェースの出力関数、MAC アドレス解決無し
 */

ER
ether_raw_output (T_NET_BUF *output)
{
	ER error = E_OK;

	/* Ethernet 出力キューに投入する。*/
	if ((error = ethernet_snd_dtq(DTQ_ETHER_OUTPUT, (intptr_t)output)) != E_OK) {
		NET_COUNT_ETHER(net_count_ether.out_err_packets, 1);
		NET_COUNT_MIB(if_stats.ifOutDiscards, 1);

		/* IF でネットワークバッファを開放しないフラグをチェックする。*/
		if ((output->flags & NB_FLG_NOREL_IFOUT) == 0) {
			syscall(rel_net_buf(output));
			}
		else {
			output->flags &= (uint8_t)~NB_FLG_NOREL_IFOUT;
			}
		IF_ETHER_NIC_RESET(IF_ETHER_NIC_GET_SOFTC());
		}
	else
		SetEvent(ETHER_OUTPUT_TASK, EVT_ETHER_OUTPUT);
	return error;
	}

/*
 *  ether_output -- Ethernet インタフェースの出力関数、MAC アドレス解決有り
 */

ER
ether_output (T_NET_BUF *output, void *dst, T_IF_ADDR *gw)
{
	T_IF_SOFTC	*ic;
	ER		error = E_OK;

	/* 送信元 MAC アドレスを設定する。*/
	ic = IF_ETHER_NIC_GET_SOFTC();
	memcpy(GET_ETHER_HDR(output)->shost, ic->ifaddr.lladdr, ETHER_ADDR_LEN);

	switch(ntohs(GET_ETHER_HDR(output)->type)) {

#if defined(SUPPORT_INET4)

	case ETHER_TYPE_IP:		/* IPv4	*/
		if (arp_resolve(&ic->ifaddr, output, *(uint32_t*)dst)) {	/* true ならアドレス解決済 */
			error = ether_raw_output(output);
			}
		break;

#endif	/* of #if defined(SUPPORT_INET4) */

#if defined(SUPPORT_INET6)

	case ETHER_TYPE_IPV6:		/* IPv6	*/
		error = nd6_storelladdr((T_ETHER_ADDR*)GET_ETHER_HDR(output)->dhost,
		                        (T_IN6_ADDR*)dst, gw);
		if (error == E_OK)
			error = ether_raw_output(output);

		/* IF でネットワークバッファを開放しないフラグをチェックする。*/
		else if ((output->flags & NB_FLG_NOREL_IFOUT) == 0) {
			syscall(rel_net_buf(output));
			}
		else {
			output->flags &= (uint8_t)~NB_FLG_NOREL_IFOUT;
			}
		break;

#endif	/* of #if defined(SUPPORT_INET6) */

	default:
		NET_COUNT_MIB(if_stats.ifOutErrors, 1);

		/* IF でネットワークバッファを開放しないフラグをチェックする。*/
		if ((output->flags & NB_FLG_NOREL_IFOUT) == 0) {
			syscall(rel_net_buf(output));
			}
		else {
			output->flags &= (uint8_t)~NB_FLG_NOREL_IFOUT;
			}
		error = E_PAR;
		break;
		}

	if (error != E_OK)
		NET_COUNT_ETHER(net_count_ether.out_err_packets, 1);
	return error;
	}

/*
 *  Ethernet 出力タスク
 */

TASK(ETHER_OUTPUT_TASK)
{
	T_IF_SOFTC	*ic;
	T_NET_BUF	*output;
	TaskType	tskid;

	GetTaskID(&tskid);
	syslog(LOG_NOTICE, "[ETHER OUTPUT:%d] started.", tskid);

	ic = IF_ETHER_NIC_GET_SOFTC();

	while (true) {
		while (ethernet_rcv_dtq(DTQ_ETHER_OUTPUT, (intptr_t*)&output) == E_OK) {
			NET_COUNT_ETHER(net_count_ether.out_octets,  output->len);
			NET_COUNT_MIB(if_stats.ifOutOctets, output->len + 8);
			NET_COUNT_ETHER(net_count_ether.out_packets, 1);

#ifdef SUPPORT_MIB
			if ((*(GET_ETHER_HDR(output)->dhost) & ETHER_MCAST_ADDR) == 0) {
				NET_COUNT_MIB(if_stats.ifOutUcastPkts, 1);
				}
			else {
				NET_COUNT_MIB(if_stats.ifOutNUcastPkts, 1);
				}
#endif	/* of #ifdef SUPPORT_MIB */

			syscall(WaitEvent(ic->mask_txb_ready));
			syscall(ClearEvent(ic->mask_txb_ready));

			IF_ETHER_NIC_START(ic, output);

#ifndef ETHER_NIC_CFG_RELEASE_NET_BUF

			if ((output->flags & NB_FLG_NOREL_IFOUT) == 0) {
				syscall(rel_net_buf(output));
				}
			else {
				output->flags &= (uint8_t)~NB_FLG_NOREL_IFOUT;

#ifdef SUPPORT_TCP
				SetEvent(TCP_OUTPUT_TASK, EVT_TCP_POST_OUTPUT);
#endif	/* of #ifdef SUPPORT_TCP */
				}

#endif	/* of #ifndef ETHER_NIC_CFG_RELEASE_NET_BUF */

			}
		WaitEvent(EVT_ETHER_OUTPUT);
		ClearEvent(EVT_ETHER_OUTPUT);
		}
	}

/*
 *  Ethernet 入力タスク
 */

TASK(ETHER_INPUT_TASK)
{
	T_IF_SOFTC	*ic;
	T_NET_BUF	*input;
	T_ETHER_HDR	*eth;
	TaskType	tskid;
	uint16_t	proto;

	/* ネットワークインタフェース管理を初期化する。*/
	ifinit();

	/* NIC を初期化する。*/
	ic = IF_ETHER_NIC_GET_SOFTC();
	IF_ETHER_NIC_PROBE(ic);
	IF_ETHER_NIC_INIT(ic);

	/* Ethernet 出力タスクを起動する */
	syscall(ActivateTask(ETHER_OUTPUT_TASK));

	/* 送信可能イベントを初期化する。*/
	SetEvent(ETHER_OUTPUT_TASK, ic->mask_txb_ready);

	/* ネットワークタイマタスクを起動する */
	syscall(ActivateTask(NET_TIMER_TASK));

	GetTaskID(&tskid);

	syslog(LOG_NOTICE, "[ETHER INPUT:%2d] started on MAC Addr: %s.",
	                   tskid, mac2str(NULL, ic->ifaddr.lladdr));

#if defined(SUPPORT_INET4)

	/* ARP を初期化する。*/
	arp_init();

#endif	/* of #if defined(SUPPORT_INET4) */

	ether_ifnet.ic = ic;

	while (true) {
		syscall(WaitEvent(ic->mask_rxb_ready));
		syscall(ClearEvent(ic->mask_rxb_ready));
		if ((input = IF_ETHER_NIC_READ(ic)) != NULL) {
			NET_COUNT_ETHER(net_count_ether.in_octets,  input->len);
			NET_COUNT_MIB(if_stats.ifInOctets, input->len + 8);
			NET_COUNT_ETHER(net_count_ether.in_packets, 1);
			eth = GET_ETHER_HDR(input);
			proto = ntohs(eth->type);

#ifdef SUPPORT_MIB
			if ((*eth->dhost & ETHER_MCAST_ADDR) == 0) {
				NET_COUNT_MIB(if_stats.ifInUcastPkts, 1);
				}
			else {
				NET_COUNT_MIB(if_stats.ifInNUcastPkts, 1);
				}
#endif	/* of #ifdef SUPPORT_MIB */

#if defined(SUPPORT_INET4) && defined(ETHER_CFG_ACCEPT_ALL)

			if ((*eth->dhost & ETHER_MCAST_ADDR) && *eth->dhost != 0xff) {

#ifdef ETHER_CFG_MCAST_WARNING

				if (proto <= 1500)
					proto = ntohs(*(uint16_t*)&(GET_8022SNAP_HDR(input)->type));
				syslog(LOG_WARNING, "[ETHER] mcast addr  from: %s, to: %s, proto: 0x%04x.",
				                    mac2str(NULL, eth->shost), mac2str(NULL, eth->dhost), proto);

#endif	/* of #ifdef ETHER_CFG_MCAST_WARNING */

				NET_COUNT_ETHER(net_count_ether.in_err_packets, 1);
				NET_COUNT_MIB(if_stats.ifInErrors, 1);
				syscall(rel_net_buf(input));
				continue;
				}

#endif	/* of #if defined(SUPPORT_INET4) && defined(ETHER_CFG_ACCEPT_ALL) */

			switch(proto) {

#if defined(SUPPORT_INET4)

			case ETHER_TYPE_IP:		/* IP	*/
				ip_input(input);
				break;

			case ETHER_TYPE_ARP:		/* ARP	*/
				arp_input(&ic->ifaddr, input);
				break;

#endif	/* of #if defined(SUPPORT_INET4) */

#if defined(SUPPORT_INET6)

			case ETHER_TYPE_IPV6:		/* IPv6	*/
				ip6_input(input);
				break;

#endif	/* of #if defined(SUPPORT_INET6) */

			default:

#ifdef ETHER_CFG_UNEXP_WARNING

#ifdef ETHER_CFG_802_WARNING

				if (proto <= 1500)
					ieee_802_input(input);
				else
					syslog(LOG_NOTICE, "[ETHER] unexp proto from: %s, proto: 0x%04x.",
					                   mac2str(NULL, GET_ETHER_HDR(input)->shost), proto);

#else	/* of #ifdef ETHER_CFG_802_WARNING */

				syslog(LOG_INFO, "[ETHER] unexp proto from: %s, proto: 0x%04x.",
				                 mac2str(NULL, GET_ETHER_HDR(input)->shost), proto);

#endif	/* of #ifdef ETHER_CFG_802_WARNING */

#endif	/* of #ifdef ETHER_CFG_UNEXP_WARNING */

				NET_COUNT_ETHER(net_count_ether.in_err_packets, 1);
				NET_COUNT_MIB(if_stats.ifUnknownProtos, 1);
				syscall(rel_net_buf(input));
				break;
				}
			}
		}
	}

#endif /* of #ifdef SUPPORT_ETHER */
