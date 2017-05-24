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
//#include <net/if_ppp.h>
//#include <net/if_loop.h>
#include <net/ethernet.h>
//#include <net/ppp_ipcp.h>
#include <net/net.h>
#include <net/net_buf.h>
#include <net/net_count.h>
#include <net/net_timer.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>

#if defined(SUPPORT_IPSEC)
//#include <stdio.h>
#include <sys/types.h>
#include <netinet6/ipsec.h>
#include <netinet6/esp.h>
#include <netkey/key.h>
#include <netkey/key_debug.h>
#endif /* of defined(SUPPORT_IPSEC) */

#include <net/if_var.h>

/*
 *  変数
 */

#ifdef SUPPORT_MIB

/*
 *  SNMP の 管理情報ベース (MIB)
 */

T_IP_STATS ip_stats;

#endif	/* of #ifdef SUPPORT_MIB */

#ifdef IP4_CFG_FRAGMENT

/*
 *  データグラム再構成キュー
 */

static T_NET_BUF	*ip_frag_queue[NUM_IP4_FRAG_QUEUE];
static T_IN4_ADDR	    frag_dst  [NUM_IP4_FRAG_QUEUE];

/*
 *  関数
 */

static void ip_freef (int_t ix);
static T_NET_BUF *ip_reass (T_IP4_HDR *ip4h, T_NET_BUF *input);

/*
 *  ip_get_frag_queue -- データグラム再構成キューを獲得する。
 */

const T_NET_BUF **
ip_get_frag_queue (void)
{
	return (const T_NET_BUF **)ip_frag_queue;
	}

/*
 *  ip_freef -- データグラム再構成キューを解放する。
 */

static void
ip_freef (int_t ix)
{
	T_NET_BUF	*frag, *next;

	frag = ip_frag_queue[ix];
	while (frag != NULL) {
		next = GET_QIP4_HDR(frag)->next;
		syscall(rel_net_buf(frag));
		frag = next;
		}
	ip_frag_queue[ix] = NULL;
	}

/*
 *  ip_frag_timer -- データグラム再構成管理タイマ
 */

void
ip_frag_timer (void)
{
	T_NET_BUF	*frag;
	int_t		ix;

	syscall(GetResource(RES_IP4_FRAG_QUEUE));
	for (ix = NUM_IP4_FRAG_QUEUE; ix -- > 0; ) {
		frag = ip_frag_queue[ix];
		if (frag != NULL && GET_QIP4_HDR(frag)->ttl > 0 &&
		    -- GET_QIP4_HDR(frag)->ttl == 0) {
			NET_COUNT_IP4(net_count_ip4[NC_IP4_FRAG_IN_TMOUT], 1);
			NET_COUNT_IP4(net_count_ip4[NC_IP4_FRAG_IN_DROP], 1);
			ip_freef(ix);
			}
		}
	syscall(ReleaseResource(RES_IP4_FRAG_QUEUE));
	}

/*
 *  ip_reass -- データグラムを再構成する。
 */

static T_NET_BUF *
ip_reass (T_IP4_HDR *ip4h, T_NET_BUF *input)
{
	T_NET_BUF	*frag, *prev;
	T_IN4_ADDR	dst, src;
	int_t		ix;
	uint_t		id, off, len;

	NET_COUNT_IP4(net_count_ip4[NC_IP4_FRAG_IN_FRAGS], 1);
	NET_COUNT_MIB(ip_stats.ipReasmReqds, 1);

	src = ntohl(ip4h->src);
	dst = ntohl(ip4h->dst);
	NTOHS(ip4h->id);
	NTOHS(ip4h->flg_off);
	id = ip4h->id;
	ix = id % NUM_IP4_FRAG_QUEUE;

	syscall(GetResource(RES_IP4_FRAG_QUEUE));

	/*
	 *  ID、IPアドレス、上位プロトコルが異なるフラグメントがキューに有れば破棄する。
	 */
	frag = ip_frag_queue[ix];
	if (frag != NULL &&
	    (id  != GET_IP4_HDR(frag)->id  ||
	     dst != frag_dst[ix] ||
	     src != ntohl(GET_IP4_HDR(frag)->src) ||
	     ip4h->proto != GET_IP4_HDR(frag)->proto)) {
		NET_COUNT_IP4(net_count_ip4[NC_IP4_FRAG_IN_DROP], 1);
		NET_COUNT_MIB(ip_stats.ipReasmFails, 1);
		ip_freef(ix);
		}

	frag = ip_frag_queue[ix];
	if (frag == NULL) {
		NET_COUNT_IP4(net_count_ip4[NC_IP4_FRAG_IN], 1);

		/* 新規の ID なら、宛先アドレスを保存して、キューにつなぐ。*/
		frag_dst  [ix] = dst;
		ip_frag_queue[ix] = input;
		((T_QIP4_HDR *)ip4h)->next = NULL;
		input = NULL;

		/* 再構成タイムアウトを設定する。*/
		ip4h->ttl = IP4_FRAGTTL;
		}
	else {

		/* 正しい位置に挿入する。*/
		prev = NULL;
		while (frag != NULL &&
		       IP4_FLGOFF_OFF(ip4h->flg_off) >
		       IP4_FLGOFF_OFF(GET_QIP4_HDR(frag)->flg_off)) {
			prev = frag;
			frag = GET_QIP4_HDR(frag)->next;
			}
		((T_QIP4_HDR *)ip4h)->next = frag;
		if (prev == NULL) {
			ip4h->ttl      = GET_QIP4_HDR(frag)->ttl;
			ip_frag_queue[ix] = input;
			}
		else
			GET_QIP4_HDR(prev)->next = input;
		input = NULL;

		/* 全てのフラグメントが揃っているか調べる。*/
		off = 0;
		for (frag = ip_frag_queue[ix]; frag != NULL; frag = GET_QIP4_HDR(frag)->next) {
			if ((IP4_FLGOFF_OFF(GET_QIP4_HDR(frag)->flg_off) << 3) != off) {
				/* 途中が抜けていたら終了する。*/
				syscall(ReleaseResource(RES_IP4_FRAG_QUEUE));
				return NULL;
				}
			off += ntohs(GET_QIP4_HDR(frag)->len) - (IP4_VHL_HL(GET_QIP4_HDR(frag)->vhl) << 2);
			prev = frag;
			}

		/* 全てのフラグメントが揃ったら再構成する。*/
		if ((GET_QIP4_HDR(prev)->flg_off & IP4_MF) == 0) {

			/* ネットワークバッファを獲得する。*/
			if (get_net_buf(&input, IF_IP4_HDR_SIZE + off) == E_OK) {
				NET_COUNT_IP4(net_count_ip4[NC_IP4_FRAG_IN_OK], 1);
				NET_COUNT_MIB(ip_stats.ipReasmOKs, 1);

				/* IPv4 ヘッダを設定する。*/
				frag  = ip_frag_queue[ix];
				ip4h  = GET_IP4_HDR(input);
				*ip4h = *GET_IP4_HDR(frag);
				ip4h->dst     = htonl(frag_dst[ix]);
				ip4h->len     = htons(IP4_HDR_SIZE + off);
				ip4h->vhl     = IP4_MAKE_VHL(IPV4_VERSION, IP4_HDR_SIZE >> 2);
				ip4h->ttl     = GET_QIP4_HDR(prev)->ttl;
				ip4h->flg_off = ip4h->id = 0;

				/* データグラムを再構成する。*/
				off = IP4_HDR_SIZE;
				while (frag != NULL) {
					len = ntohs(GET_QIP4_HDR(frag)->len) - (IP4_VHL_HL(GET_QIP4_HDR(frag)->vhl) << 2);
					memcpy((uint8_t *)ip4h + off, GET_QIP4_SDU(frag), len);
					off += len;
					frag = GET_QIP4_HDR(frag)->next;
					}
				}
			else {
				NET_COUNT_IP4(net_count_ip4[NC_IP4_FRAG_IN_NO_BUF], 1);
				NET_COUNT_IP4(net_count_ip4[NC_IP4_FRAG_IN_DROP], 1);
				NET_COUNT_MIB(ip_stats.ipReasmFails, 1);
				}
			/* キューを空にする。*/
			ip_freef(ix);
			}
		}
	syscall(ReleaseResource(RES_IP4_FRAG_QUEUE));
	return input;
	}

#endif	/* of #ifdef IP4_CFG_FRAGMENT */

/*
 *  ip_init -- IP の初期化
 */

void
ip_init (void)
{
	in4_init();
	}

/*
 *  ip_remove_options -- IPv4 ヘッダのオプションを削除し、以降を前に詰める。
 */

ER
ip_remove_options (T_NET_BUF *nbuf)
{
	T_IP_HDR	*iph;

	iph  = GET_IP_HDR(nbuf);

	if (GET_IP_HDR_SIZE(iph) > IP4_HDR_SIZE) {
		memmove((char *)iph + IP4_HDR_SIZE, GET_IP_SDU(nbuf),
		        (size_t)(iph->len - GET_IP_HDR_SIZE(iph)));
		iph->vhl   = IP4_MAKE_VHL(IPV4_VERSION, IP4_HDR_SIZE >> 2);
		iph->len  -= (uint16_t)(GET_IP_HDR_SIZE(iph) - IP4_HDR_SIZE);
		nbuf->len -= (uint16_t)(GET_IP_HDR_SIZE(iph) - IP4_HDR_SIZE);
		}

	return E_OK;
	}

/*
 *  ip_input -- IP の入力関数
 */

#if defined(SUPPORT_IPSEC)
/*
 * TODO: IPsec SPを検索し、パケットをリジェクトするかどうか判定する処理を実装する
 */
#endif /* of defined(SUPPORT_IPSEC) */

void
ip_input (T_NET_BUF *input)
{
	T_IP4_HDR	*ip4h;
	T_IFNET		*ifp = IF_GET_IFNET();
	T_IN4_ADDR	dst, src, bc;
	uint_t		hlen, off;

	NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_OCTETS], input->len - IF_HDR_SIZE);
	NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_PACKETS], 1);
	NET_COUNT_MIB(ip_stats.ipInReceives, 1);

	/* IP ヘッダの長さをチェックする。*/
	if (input->len < IF_IP4_HDR_SIZE) {
		NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_ERR_SHORT], 1);
		NET_COUNT_MIB(ip_stats.ipInHdrErrors, 1);
		goto buf_rel;
		}

	ip4h  = GET_IP4_HDR(input);
	hlen = GET_IP4_HDR_SIZE(ip4h);

	/* バージョンをチェックする。*/
	if (IP4_VHL_V(ip4h->vhl) != IPV4_VERSION) {
		NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_ERR_VER], 1);
		NET_COUNT_MIB(ip_stats.ipInHdrErrors, 1);
		goto buf_rel;
		}

	/* IP ヘッダの長さをチェックし、オプションを解析する。*/
	if (hlen > IP4_HDR_SIZE) {
		NET_COUNT_IP4(net_count_ip4[NC_IP4_OPTS], 1);
		/* %%% オプションの解析 %%% */
		}

	/* データグラム長をチェックする。*/
	if (ntohs(ip4h->len) > input->len - IF_HDR_SIZE) {
		NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_ERR_SHORT], 1);
		NET_COUNT_MIB(ip_stats.ipInHdrErrors, 1);
		goto buf_rel;
		}

	/* ネットワークバッファの長さをデータグラム長に調整する。*/
	input->len = (uint16_t)(ntohs(ip4h->len) + IF_HDR_SIZE);

	/* チェックサムをチェックする。*/
	if (in_cksum(ip4h, hlen) != 0) {
		NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_ERR_CKSUM], 1);
		NET_COUNT_MIB(ip_stats.ipInHdrErrors, 1);
		goto buf_rel;
		}

	/* IP ヘッダの長さをチェックし、上位が ICMP 以外はオプションを消去する。*/
	if (hlen > IP4_HDR_SIZE && ip4h->proto != IPPROTO_ICMP) {
		memset((uint8_t*)ip4h + IP4_HDR_SIZE, 0, hlen - IP4_HDR_SIZE);
		}

	/* 送信元アドレスをチェックする。*/
	src = ntohl(ip4h->src);
	bc  = (ifp->in_ifaddr.addr & ifp->in_ifaddr.mask) | ~ifp->in_ifaddr.mask;

#ifdef SUPPORT_LOOP

	if (src == bc || src == IPV4_ADDR_BROADCAST || src == IPV4_ADDRANY) {
		NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_ERR_ADDR], 1);
		NET_COUNT_MIB(ip_stats.ipInAddrErrors, 1);
		goto buf_rel;
		}

#else	/* of #ifdef SUPPORT_LOOP */

	if (src == ifp->in_ifaddr.addr || src == bc || src == IPV4_ADDR_BROADCAST || src == IPV4_ADDRANY) {
		NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_ERR_ADDR], 1);
		NET_COUNT_MIB(ip_stats.ipInAddrErrors, 1);
		goto buf_rel;
		}

#endif	/* of #ifdef SUPPORT_LOOP */

	/* あて先アドレスをチェックする。*/
	dst = ntohl(ip4h->dst);

#ifdef DHCP_CFG

	/*
	 *  DHCP_CFG が定義されているときは、ローカルアドレスが未定義の
	 *  場合もデータグラムを受信する。
	 */

	if ((ifp->in_ifaddr.addr != IPV4_ADDRANY) &&
	    (!(dst == ifp->in_ifaddr.addr || dst == bc ||
	       dst == IPV4_ADDR_BROADCAST || dst == IPV4_ADDRANY))) {
		NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_ERR_ADDR], 1);
		NET_COUNT_MIB(ip_stats.ipInAddrErrors, 1);
		goto buf_rel;
		}

#else	/* of #ifdef DHCP_CFG */

	if (!(dst == ifp->in_ifaddr.addr || dst == bc ||
	      dst == IPV4_ADDR_BROADCAST || dst == IPV4_ADDRANY)) {
		NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_ERR_ADDR], 1);
		NET_COUNT_MIB(ip_stats.ipInAddrErrors, 1);
		goto buf_rel;
		}

#endif	/* of #ifdef DHCP_CFG */

#ifdef IP4_CFG_FRAGMENT

	/* 分割されているかチェックする。*/
	if (ntohs(ip4h->flg_off) & (IP4_MF | IP4_OFFMASK)) {
		if ((input = ip_reass(ip4h, input)) == NULL)
			return;
		}

#else	/* of #ifdef IP4_CFG_FRAGMENT */

	/* 分割されているかチェックする。*/
	if (ntohs(ip4h->flg_off) & (IP4_MF | IP4_OFFMASK)) {
		T_IN4_ADDR	src;

		NET_COUNT_MIB(ip_stats.ipReasmReqds, 1);
		if ((ntohs(ip4h->flg_off) & IP4_OFFMASK) == 0) {
			NET_COUNT_MIB(ip_stats.ipReasmFails, 1);
			}
		src = ntohl(ip4h->src);
		syslog(LOG_WARNING, "[IP] flaged src: %s.", ip2str(NULL, &src));
		goto buf_rel;
		}

#endif	/* of #ifdef IP4_CFG_FRAGMENT */

	off  = (uint_t)(GET_IP4_SDU(input) - input->buf);

#if defined(SUPPORT_IPSEC)
	/* ここでipsec4_in_rejectを実行する */
	if ((ip4h->proto != IPPROTO_ESP) && ipsec4_in_reject (input)) {
		goto buf_rel;
	}
#endif /* of #if defined(SUPPORT_IPSEC) */

	/* プロトコルを選択する */
	switch (ip4h->proto) {

#ifdef SUPPORT_UDP
	case IPPROTO_UDP:
		NET_COUNT_MIB(ip_stats.ipInDelivers, 1);
		udp_input(&input, &off, NULL);
		return;
		break;
#endif	/* of #ifdef SUPPORT_UDP */

#ifdef SUPPORT_TCP
	case IPPROTO_TCP:
		NET_COUNT_MIB(ip_stats.ipInDelivers, 1);
		tcp_input(&input, &off, NULL);
		return;
		break;
#endif	/* of #ifdef SUPPORT_UDP */

	case IPPROTO_ICMP:
		NET_COUNT_MIB(ip_stats.ipInDelivers, 1);
		icmp_input(&input, &off, NULL);
		return;
		break;

#if defined(SUPPORT_IPSEC)
	case IPPROTO_ESP:
		NET_COUNT_MIB(ip_stats.ipInDelivers, 1);
		esp4_input(input, &off);
		return;
		break;
#endif /* of #if defined(SUPPORT_IPSEC) */

	default:
		NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_ERR_PROTO], 1);
		NET_COUNT_MIB(ip_stats.ipInUnknownProtos, 1);

		/* ローカル IP アドレスに届いたデータグラムのみ ICMP エラーを通知する。*/
		if (dst == ifp->in_ifaddr.addr) {
			T_IN4_ADDR	src;

			src = ntohl(ip4h->src);
			syslog(LOG_INFO, "[IP] unexp proto: %d, src=%s.", ip4h->proto, ip2str(NULL, &src));
			icmp_error(ICMP4_UNREACH_PROTOCOL, input);
			}
		/*
		 *  icmp_error では、ネットワークバッファ input を返却しないので
		 *  開放してから終了する。
		 */
		break;
		}

buf_rel:
	NET_COUNT_IP4(net_count_ip4[NC_IP4_IN_ERR_PACKETS], 1);
	syscall(rel_net_buf(input));
	}
