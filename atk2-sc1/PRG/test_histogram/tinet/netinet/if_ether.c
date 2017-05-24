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
 *  ARP ����å��奨��ȥ�
 */

static T_ARP_ENTRY arp_cache[NUM_ARP_ENTRY];

/*
 *  �ؿ�
 */

static T_ARP_ENTRY *arp_lookup (T_IN4_ADDR addr, bool_t create);
static void in_arpinput (T_IF_ADDR *ifaddr, T_NET_BUF *input);
static void arp_timer (void *ignore);

/*
 *  arp_lookup -- ARP ����å����õ������Ͽ
 *
 *    ����: SEM_ARP_CACHE_LOCK ������������֤ǸƽФ�����
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

	/* create �����ʤ顢�����ʥ���ȥ����Ͽ���롣*/
	if (create) {

		/* �ޤ�������������С����ζ��������Ѥ��롣*/
		for (ix = NUM_ARP_ENTRY; ix -- > 0; ) {
			if (arp_cache[ix].expire == 0) {
				arp_cache[ix].ip_addr = addr;
				return &arp_cache[ix];
				}
			}

		/*
		 *  �������ʤ���С������ॢ���Ȥޤǻ��֤���û��
		 *  ����ȥ꡼���˴��������Ѥ��롣
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
 *  in_arpinput -- TCP/IP �� ARP �����ϴؿ�
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

	ntoahl(saddr, et_arph->sproto);		/* ������ IP ���ɥ쥹	*/
	ntoahl(taddr, et_arph->tproto);		/* ����о� IP ���ɥ쥹	*/

	/*
	 *  �ʲ��ξ��ϥ��顼
	 *    �������ۥ��Ȥ�ʪ�����ɥ쥹����ʬ
	 *    �������ۥ��Ȥ�ʪ�����ɥ쥹���֥����ɥ��㥹��
	 */
	if (memcmp(et_arph->shost, ifaddr->lladdr,        ETHER_ADDR_LEN) == 0 ||
	    memcmp(et_arph->shost, ether_broad_cast_addr, ETHER_ADDR_LEN) == 0)
		goto err_ret;

	/*
	 *  �����ۥ��Ȥ� IP ���ɥ쥹����ʬ�ξ��ϡ���ʣ���Ƥ���Τ�
	 *  ���ˤ��Τ餻�롣
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
	 *  �ʲ��ξ��ϲ��⤷�ʤ���
	 *    ������о� IP ���ɥ쥹����ʬ�ǤϤʤ�����: ���� FreeBSD ��
	 *      �����Ǥϡ�ARP PROXY ���Τ��ᡢ��ʬ�ʳ��� IP ���ɥ쥹��
	 *      ����ԤäƤ��뤬���ܼ����Ǥϡ���ʬ�ʳ��� IP
	 *      ���ɥ쥹�β��ϹԤ�ʤ���
	 */
	if (taddr != ifp->in_ifaddr.addr)
		goto buf_rel;

	/*
	 *  ������ IP ���ɥ쥹�� ARP ����å���ˤ��뤫Ĵ�٤롣
	 *  ����å���ˤʤ���С������˥���ȥ����Ͽ���롣
	 */
	syscall(GetResource(RES_ARP_CACHE_LOCK));
	ent = arp_lookup(saddr, true);

	memcpy(ent->mac_addr, et_arph->shost, ETHER_ADDR_LEN);
	ent->expire = ARP_CACHE_KEEP;

	/*
	 *  �������ڥ�ǥ󥰤���Ƥ���ե졼�ब������������롣
	 */
	if (ent->hold) {

		/* �ե졼��� Ethernet �إå������ꤹ�롣*/
		memcpy(GET_ETHER_HDR(ent->hold)->dhost, ent->mac_addr, ETHER_ADDR_LEN);

		pending = ent->hold;
		ent->hold = NULL;
		syscall(ReleaseResource(RES_ARP_CACHE_LOCK));

		/* �ڥ�ǥ��󥰤���Ƥ���ե졼����������롣*/
		IF_RAW_OUTPUT(pending);

		}
	else
		syscall(ReleaseResource(RES_ARP_CACHE_LOCK));

reply:

	arph = GET_ARP_HDR(input);

	/* ���ɥ쥹����׵�Ǥʤ���н�λ */
	if (ntohs(arph->opcode) != ARPOP_REQUEST)
		goto buf_rel;

	/* Ethernet ARP �إå������ꤹ�롣*/
	memcpy(et_arph->thost, et_arph->shost, ETHER_ADDR_LEN);
	memcpy(et_arph->shost, ifaddr->lladdr, ETHER_ADDR_LEN);
	memcpy(et_arph->tproto, (uint8_t*)&et_arph->sproto, sizeof(T_IN_ADDR));
	ahtonl(et_arph->sproto, taddr);

	/* Ethernet ARP �إå������ꤹ�롣*/
	arph->opcode = htons(ARPOP_REPLY);

	/* Ethernet �إå������ꤹ�롣*/
	eth = GET_ETHER_HDR(input);
	memcpy(eth->dhost, eth->shost,     ETHER_ADDR_LEN);
	memcpy(eth->shost, ifaddr->lladdr, ETHER_ADDR_LEN);

	/* ARP �������������롣*/
	IF_RAW_OUTPUT(input);
	return;

err_ret:
	NET_COUNT_ARP(net_count_arp.in_err_packets, 1);
buf_rel:
	syscall(rel_net_buf(input));
	}

/*
 *  arp_timer -- ARP ����å���ι���
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
				/* �������ڥ�ǥ󥰤���Ƥ���ե졼�ब����мΤƤ롣*/
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
 *  arp_request -- MAC ���ɥ쥹����׵�
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

		/* �������ͥåȥإå������ꤹ�롣*/
		eth     = GET_ETHER_HDR(arp_req);
		memcpy(eth->dhost, ether_broad_cast_addr, ETHER_ADDR_LEN);
		memcpy(eth->shost, ifaddr->lladdr,        ETHER_ADDR_LEN);
		eth->type = htons(ETHER_TYPE_ARP);

		/* ARP �إå������ꤹ�롣*/
		arph    = GET_ARP_HDR(arp_req);
		arph->hrd_addr  = htons(ARPHRD_ETHER);
		arph->proto     = htons(ETHER_TYPE_IP);
		arph->hdr_len   = sizeof(et_arph->shost);
		arph->proto_len = sizeof(et_arph->sproto);
		arph->opcode    = htons(ARPOP_REQUEST);

		/* �������ͥå� ARP �إå������ꤹ�롣*/
		et_arph = GET_ETHER_ARP_HDR(arp_req);
		src     = IF_GET_IFNET()->in_ifaddr.addr;
		memcpy(et_arph->shost, ifaddr->lladdr, ETHER_ADDR_LEN);
		memset(et_arph->thost, 0,              ETHER_ADDR_LEN);
		ahtonl(et_arph->sproto, src);
		ahtonl(et_arph->tproto, dst);

		/* �������롣*/
		error = IF_RAW_OUTPUT(arp_req);
		}
	if (error != E_OK)
		NET_COUNT_ARP(net_count_arp.out_err_packets, 1);
	return error;
	}

/*
 *  arp_get_cache -- ARP ����å����������롣
 */

const T_ARP_ENTRY *
arp_get_cache (void)
{
	return arp_cache;
	}

/*
 *  arp_input -- ARP �����ϴؿ�
 */

void
arp_input (T_IF_ADDR *ifaddr, T_NET_BUF *input)
{
	T_ARP_HDR	*arph;

	NET_COUNT_ARP(net_count_arp.in_octets , input->len - IF_HDR_SIZE);
	NET_COUNT_ARP(net_count_arp.in_packets, 1);

	/* ARP �إå���Ĺ��������å����롣*/
	if (input->len < IF_ARP_HDR_SIZE)
		goto buf_rel;

	arph = GET_ARP_HDR(input);

	/*
	 *  ʪ�����ɥ쥹�ե����ޥåȤ� Ehternet �ʳ�
	 *  �ޤ��ϡ��ץ��ȥ��뤬 IP �ʳ��ϥ��顼��
	 */
	if (ntohs(arph->hrd_addr) != ARPHRD_ETHER ||
	    ntohs(arph->proto)    != ETHER_TYPE_IP)
		goto buf_rel;

	/* ARP �إå� + Ether ARP �إå���Ĺ��������å����롣*/
	if (input->len < IF_ARP_ETHER_HDR_SIZE)
		goto buf_rel;

	in_arpinput(ifaddr, input);
	return;

buf_rel:
	NET_COUNT_ARP(net_count_arp.in_err_packets, 1);
	syscall(rel_net_buf(input));
	}

/*
 *  arp_resolve -- MAC ���ɥ쥹���
 *
 *    ���ɥ쥹��褬�Ǥ���С�����ͤ� true ���֤����Ǥ��ʤ����
 *    ������ڥ�ǥ��󥰤��ơ�����ͤ� false ���֤���
 */

bool_t
arp_resolve (T_IF_ADDR *ifaddr, T_NET_BUF *output, T_IN4_ADDR gw)
{
	T_ARP_ENTRY	*ent;
	T_ETHER_HDR	*eth;
	T_IFNET		*ifp = IF_GET_IFNET();

	eth = GET_ETHER_HDR(output);

	/*
	 *  ���ξ��ϡ��������ͥåȤΥ֥����ɥ��㥹�ȥ��ɥ쥹���֤���
	 *
	 *    �����ӥåȤ� 1
	 *    ���ۥ����������ӥåȤ� 1 �ǡ��ͥåȥ�������������륢�ɥ쥹
	 */
	if (gw == IPV4_ADDR_BROADCAST ||
	    gw == ((ifp->in_ifaddr.addr & ifp->in_ifaddr.mask) | ~ifp->in_ifaddr.mask)) {
		memcpy(eth->dhost, ether_broad_cast_addr, ETHER_ADDR_LEN);
		return true;
		}

	/* ������ GW �� IP ���ɥ쥹�� ARP ����å���ˤ��뤫Ĵ�٤롣*/
	syscall(GetResource(RES_ARP_CACHE_LOCK));
	ent = arp_lookup(gw, true);
	if (ent->expire) {
		memcpy(eth->dhost, ent->mac_addr, ETHER_ADDR_LEN);
		syscall(ReleaseResource(RES_ARP_CACHE_LOCK));
		return true;
		}
	else {
	 	/* �������ڥ�ǥ󥰤���Ƥ���ե졼�ब����мΤƤ롣*/
		if (ent->hold) {
			NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_ERR_PACKETS], 1);
			syscall(rel_net_buf(ent->hold));
			}

		/*
		 *  ������ڥ�ǥ��󥰤��롣
		 *  IF �ǥͥåȥ���Хåե��������ʤ��ե饰�����ꤵ��Ƥ���Ȥ��ϡ�
		 *  ������ڥ�ǥ��󥰤��ʤ���
		 */
		if ((output->flags & NB_FLG_NOREL_IFOUT) == 0)
			ent->hold = output;
		else {
			output->flags &= (uint8_t)~NB_FLG_NOREL_IFOUT;
			ent->hold = NULL;
			}
		syscall(ReleaseResource(RES_ARP_CACHE_LOCK));

		/* ���ɥ쥹����׵���������롣*/
		arp_request(ifaddr, gw);
		return false;
		}
	}

/*
 *  arp_init -- ARP �����
 */

void
arp_init (void)
{
	timeout(arp_timer, NULL, ARP_TIMER_TMO);
	}

#endif	/* of #if defined(SUPPORT_INET4) && defined(SUPPORT_ETHER) */