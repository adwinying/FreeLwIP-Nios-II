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
#include <net/if_arp.h>
#include <net/net.h>
#include <net/net_buf.h>
#include <net/net_var.h>
#include <net/net_timer.h>
#include <net/net_count.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

#include <net/if_var.h>

#if defined(SUPPORT_INET4) && defined(SUPPORT_ETHER)

/*
 *  ARP キャッシュエントリ
 */

static T_ARP_ENTRY arp_cache[NUM_ARP_ENTRY];

/*
 *  関数
 */

static T_ARP_ENTRY *arp_lookup (T_IN4_ADDR addr, bool_t create);
static void in_arpinput (T_IF_ADDR *ifaddr, T_NET_BUF *input);
static void arp_timer (void *ignore);

/*
 *  arp_lookup -- ARP キャッシュの探索と登録
 *
 *    注意: SEM_ARP_CACHE_LOCK を獲得した状態で呼出すこと
 */

static T_ARP_ENTRY *
arp_lookup (T_IN4_ADDR addr, bool_t create)
{
	int_t		ix, six;
	uint16_t	min;

	for (ix = NUM_ARP_ENTRY; ix -- > 0; ) {
		if (arp_cache[ix].expire && arp_cache[ix].ip_addr == addr)
			return &arp_cache[ix];
		}

	/* create が真なら、新たなエントリを登録する。*/
	if (create) {

		/* まず、空きがあれば、その空きを利用する。*/
		for (ix = NUM_ARP_ENTRY; ix -- > 0; ) {
			if (arp_cache[ix].expire == 0) {
				arp_cache[ix].ip_addr = addr;
				return &arp_cache[ix];
				}
			}

		/*
		 *  空きがなければ、タイムアウトまで時間が最短の
		 *  エントリーを破棄して利用する。
		 */
		syslog(LOG_NOTICE, "[ARP] cache busy, size=%d", NUM_ARP_ENTRY);
		min = 0xffff;
		for (six = ix = NUM_ARP_ENTRY; ix -- > 0; )
			if (arp_cache[ix].expire < min) {
				six = ix;
				min = arp_cache[ix].expire;
				}
		arp_cache[six].expire  = 0;
		arp_cache[six].ip_addr = addr;
		return &arp_cache[six];
		}
	else
		return NULL;
	}

/*
 *  in_arpinput -- TCP/IP 用 ARP の入力関数
 */

static void
in_arpinput (T_IF_ADDR *ifaddr, T_NET_BUF *input)
{
	T_NET_BUF	*pending;
	T_ETHER_HDR	*eth;
	T_ARP_HDR	*arph;
	T_ETHER_ARP_HDR	*et_arph;
	T_ARP_ENTRY	*ent;
	T_IFNET		*ifp = IF_GET_IFNET();
	T_IN4_ADDR	saddr, taddr;

	et_arph = GET_ETHER_ARP_HDR(input);

	ntoahl(saddr, et_arph->sproto);		/* 送信元 IP アドレス	*/
	ntoahl(taddr, et_arph->tproto);		/* 解決対象 IP アドレス	*/

	/*
	 *  以下の場合はエラー
	 *    ・送信ホストの物理アドレスが自分
	 *    ・送信ホストの物理アドレスがブロードキャスト
	 */
	if (memcmp(et_arph->shost, ifaddr->lladdr,        ETHER_ADDR_LEN) == 0 ||
	    memcmp(et_arph->shost, ether_broad_cast_addr, ETHER_ADDR_LEN) == 0)
		goto err_ret;

	/*
	 *  送信ホストの IP アドレスが自分の場合は、重複しているので
	 *  相手にも知らせる。
	 */
	if (saddr == ifp->in_ifaddr.addr) {

#ifdef ARP_CFG_CALLBACK_DUPLICATED

		if (arp_callback_duplicated(et_arph->shost)) {
			syslog(LOG_ERROR, "[ARP] IP address duplicated: %s", mac2str(NULL, et_arph->shost));
			taddr = saddr;
			goto reply;
			}
		else
			goto buf_rel;

#else	/* of #ifdef ARP_CFG_CALLBACK_DUPLICATED */

		syslog(LOG_ERROR, "[ARP] IP address duplicated: %s", mac2str(NULL, et_arph->shost));
		taddr = saddr;
		goto reply;

#endif	/* of #ifdef ARP_CFG_CALLBACK_DUPLICATED */

		}

	/*
	 *  以下の場合は何もしない。
	 *    ・解決対象 IP アドレスが自分ではない、注: 元の FreeBSD の
	 *      実装では、ARP PROXY 等のため、自分以外の IP アドレスの
	 *      解決も行っているが、本実装では、自分以外の IP
	 *      アドレスの解決は行わない。
	 */
	if (taddr != ifp->in_ifaddr.addr)
		goto buf_rel;

	/*
	 *  送信元 IP アドレスが ARP キャッシュにあるか調べる。
	 *  キャッシュになければ、新たにエントリを登録する。
	 */
	syscall(GetResource(RES_ARP_CACHE_LOCK));
	ent = arp_lookup(saddr, true);

	memcpy(ent->mac_addr, et_arph->shost, ETHER_ADDR_LEN);
	ent->expire = ARP_CACHE_KEEP;

	/*
	 *  送信がペンデングされているフレームがあれば送信する。
	 */
	if (ent->hold) {

		/* フレームの Ethernet ヘッダを設定する。*/
		memcpy(GET_ETHER_HDR(ent->hold)->dhost, ent->mac_addr, ETHER_ADDR_LEN);

		pending = ent->hold;
		ent->hold = NULL;
		syscall(ReleaseResource(RES_ARP_CACHE_LOCK));

		/* ペンディングされているフレームを送信する。*/
		IF_RAW_OUTPUT(pending);

		}
	else
		syscall(ReleaseResource(RES_ARP_CACHE_LOCK));

reply:

	arph = GET_ARP_HDR(input);

	/* アドレス解決要求でなければ終了 */
	if (ntohs(arph->opcode) != ARPOP_REQUEST)
		goto buf_rel;

	/* Ethernet ARP ヘッダを設定する。*/
	memcpy(et_arph->thost, et_arph->shost, ETHER_ADDR_LEN);
	memcpy(et_arph->shost, ifaddr->lladdr, ETHER_ADDR_LEN);
	memcpy(et_arph->tproto, (uint8_t*)&et_arph->sproto, sizeof(T_IN_ADDR));
	ahtonl(et_arph->sproto, taddr);

	/* Ethernet ARP ヘッダを設定する。*/
	arph->opcode = htons(ARPOP_REPLY);

	/* Ethernet ヘッダを設定する。*/
	eth = GET_ETHER_HDR(input);
	memcpy(eth->dhost, eth->shost,     ETHER_ADDR_LEN);
	memcpy(eth->shost, ifaddr->lladdr, ETHER_ADDR_LEN);

	/* ARP 応答を送信する。*/
	IF_RAW_OUTPUT(input);
	return;

err_ret:
	NET_COUNT_ARP(net_count_arp.in_err_packets, 1);
buf_rel:
	syscall(rel_net_buf(input));
	}

/*
 *  arp_timer -- ARP キャッシュの更新
 */

static void
arp_timer (void *ignore)
{
	int_t	ix;

	syscall(GetResource(RES_ARP_CACHE_LOCK));

	for (ix = NUM_ARP_ENTRY; ix -- > 0; ) {
		if (arp_cache[ix].expire) {
			arp_cache[ix].expire -= ARP_TIMER_TMO;
			if (arp_cache[ix].expire == 0) {
				/* 送信がペンデングされているフレームがあれば捨てる。*/
				if (arp_cache[ix].hold) {
					NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_ERR_PACKETS], 1);
					syscall(rel_net_buf(arp_cache[ix].hold));
					}
				memset(&arp_cache[ix], 0, sizeof(T_ARP_ENTRY));
				}
			}
		}

	syscall(ReleaseResource(RES_ARP_CACHE_LOCK));
	timeout(arp_timer, NULL, ARP_TIMER_TMO);
	}

/*
 *  arp_request -- MAC アドレス解決要求
 */

ER
arp_request (T_IF_ADDR *ifaddr, T_IN4_ADDR dst)
{
	ER		error;
	T_IN4_ADDR	src;
	T_ETHER_HDR	*eth;
	T_ARP_HDR	*arph;
	T_ETHER_ARP_HDR	*et_arph;
	T_NET_BUF	*arp_req;

	NET_COUNT_ARP(net_count_arp.out_octets , IF_ARP_ETHER_HDR_SIZE - IF_HDR_SIZE);
	NET_COUNT_ARP(net_count_arp.out_packets, 1);

	if ((error = get_net_buf(&arp_req, IF_ARP_ETHER_HDR_SIZE)) == E_OK) {

		/* イーサネットヘッダを設定する。*/
		eth     = GET_ETHER_HDR(arp_req);
		memcpy(eth->dhost, ether_broad_cast_addr, ETHER_ADDR_LEN);
		memcpy(eth->shost, ifaddr->lladdr,        ETHER_ADDR_LEN);
		eth->type = htons(ETHER_TYPE_ARP);

		/* ARP ヘッダを設定する。*/
		arph    = GET_ARP_HDR(arp_req);
		arph->hrd_addr  = htons(ARPHRD_ETHER);
		arph->proto     = htons(ETHER_TYPE_IP);
		arph->hdr_len   = sizeof(et_arph->shost);
		arph->proto_len = sizeof(et_arph->sproto);
		arph->opcode    = htons(ARPOP_REQUEST);

		/* イーサネット ARP ヘッダを設定する。*/
		et_arph = GET_ETHER_ARP_HDR(arp_req);
		src     = IF_GET_IFNET()->in_ifaddr.addr;
		memcpy(et_arph->shost, ifaddr->lladdr, ETHER_ADDR_LEN);
		memset(et_arph->thost, 0,              ETHER_ADDR_LEN);
		ahtonl(et_arph->sproto, src);
		ahtonl(et_arph->tproto, dst);

		/* 送信する。*/
		error = IF_RAW_OUTPUT(arp_req);
		}
	if (error != E_OK)
		NET_COUNT_ARP(net_count_arp.out_err_packets, 1);
	return error;
	}

/*
 *  arp_get_cache -- ARP キャッシュを獲得する。
 */

const T_ARP_ENTRY *
arp_get_cache (void)
{
	return arp_cache;
	}

/*
 *  arp_input -- ARP の入力関数
 */

void
arp_input (T_IF_ADDR *ifaddr, T_NET_BUF *input)
{
	T_ARP_HDR	*arph;

	NET_COUNT_ARP(net_count_arp.in_octets , input->len - IF_HDR_SIZE);
	NET_COUNT_ARP(net_count_arp.in_packets, 1);

	/* ARP ヘッダの長さをチェックする。*/
	if (input->len < IF_ARP_HDR_SIZE)
		goto buf_rel;

	arph = GET_ARP_HDR(input);

	/*
	 *  物理アドレスフォーマットが Ehternet 以外
	 *  または、プロトコルが IP 以外はエラー。
	 */
	if (ntohs(arph->hrd_addr) != ARPHRD_ETHER ||
	    ntohs(arph->proto)    != ETHER_TYPE_IP)
		goto buf_rel;

	/* ARP ヘッダ + Ether ARP ヘッダの長さをチェックする。*/
	if (input->len < IF_ARP_ETHER_HDR_SIZE)
		goto buf_rel;

	in_arpinput(ifaddr, input);
	return;

buf_rel:
	NET_COUNT_ARP(net_count_arp.in_err_packets, 1);
	syscall(rel_net_buf(input));
	}

/*
 *  arp_resolve -- MAC アドレス解決
 *
 *    アドレス解決ができれば、戻り値は true を返し、できなければ
 *    送信をペンディングして、戻り値は false を返す。
 */

bool_t
arp_resolve (T_IF_ADDR *ifaddr, T_NET_BUF *output, T_IN4_ADDR gw)
{
	T_ARP_ENTRY	*ent;
	T_ETHER_HDR	*eth;
	T_IFNET		*ifp = IF_GET_IFNET();

	eth = GET_ETHER_HDR(output);

	/*
	 *  次の場合は、イーサネットのブロードキャストアドレスを返す。
	 *
	 *    ・全ビットが 1
	 *    ・ホスト部の全ビットが 1 で、ネットワーク部がローカルアドレス
	 */
	if (gw == IPV4_ADDR_BROADCAST ||
	    gw == ((ifp->in_ifaddr.addr & ifp->in_ifaddr.mask) | ~ifp->in_ifaddr.mask)) {
		memcpy(eth->dhost, ether_broad_cast_addr, ETHER_ADDR_LEN);
		return true;
		}

	/* 送信先 GW の IP アドレスが ARP キャッシュにあるか調べる。*/
	syscall(GetResource(RES_ARP_CACHE_LOCK));
	ent = arp_lookup(gw, true);
	if (ent->expire) {
		memcpy(eth->dhost, ent->mac_addr, ETHER_ADDR_LEN);
		syscall(ReleaseResource(RES_ARP_CACHE_LOCK));
		return true;
		}
	else {
	 	/* 送信がペンデングされているフレームがあれば捨てる。*/
		if (ent->hold) {
			NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_ERR_PACKETS], 1);
			syscall(rel_net_buf(ent->hold));
			}

		/*
		 *  送信をペンディングする。
		 *  IF でネットワークバッファを開放しないフラグが設定されているときは、
		 *  送信をペンディングしない。
		 */
		if ((output->flags & NB_FLG_NOREL_IFOUT) == 0)
			ent->hold = output;
		else {
			output->flags &= (uint8_t)~NB_FLG_NOREL_IFOUT;
			ent->hold = NULL;
			}
		syscall(ReleaseResource(RES_ARP_CACHE_LOCK));

		/* アドレス解決要求を送信する。*/
		arp_request(ifaddr, gw);
		return false;
		}
	}

/*
 *  arp_init -- ARP 初期化
 */

void
arp_init (void)
{
	timeout(arp_timer, NULL, ARP_TIMER_TMO);
	}

#endif	/* of #if defined(SUPPORT_INET4) && defined(SUPPORT_ETHER) */
