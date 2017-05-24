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
//#include <net/if_loop.h>
//#include <net/if_ppp.h>
#include <net/ethernet.h>
//#include <net/ppp_ipcp.h>
#include <net/net.h>
#include <net/net_buf.h>
#include <net/net_count.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
//#include <netinet6/in6.h>
//#include <netinet6/in6_var.h>
#include <netinet/in_itron.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
//#include <netinet/ip6.h>
//#include <netinet6/ip6_var.h>
#include <netinet/ip_icmp.h>
//#include <netinet/icmp6.h>

#include <netinet/udp.h>
#include <netinet/udp_var.h>

#include <net/if_var.h>

#ifdef SUPPORT_UDP

#ifdef SUPPORT_MIB

/*
 *  SNMP �� ��������١��� (MIB)
 */

T_UDP_STATS udp_stats;

#endif	/* of #ifdef SUPPORT_MIB */

#if defined(SUPPORT_INET4)

#ifdef DHCP_CFG

/*
 *  udp_dstaddr_accept -- UDP �Τ����襢�ɥ쥹���������������å����롣
 *
 *    DHCP_CFG ���������Ƥ���Ȥ��ϡ��ʲ��Υǡ���������������롣
 *
 *      �������襢�ɥ쥹���֥��ɥ��㥹��
 *      �������륢�ɥ쥹��̤���
 */

static bool_t
udp_is_dstaddr_accept (T_IN4_ADDR *myaddr, T_IN4_ADDR *dstaddr)
{
	T_IFNET	*ifp = IF_GET_IFNET();

	if (ifp->in_ifaddr.addr == IPV4_ADDRANY ||
	    ntohl(*dstaddr) == IPV4_ADDR_BROADCAST ||
	    ntohl(*dstaddr) == ((ifp->in_ifaddr.addr & ifp->in_ifaddr.mask) | ~ifp->in_ifaddr.mask))
		return true;
	else
		return IN_IS_DSTADDR_ACCEPT(myaddr, dstaddr);
	}

#else	/* of #ifdef DHCP_CFG */

/*
 *  udp_dstaddr_accept -- UDP �Τ����襢�ɥ쥹���������������å����롣
 *
 *    �ʲ��ξ���ǡ���������������롣
 *
 *      �������襢�ɥ쥹���֥��ɥ��㥹��
 */

static bool_t
udp_is_dstaddr_accept (T_IN4_ADDR *myaddr, T_IN4_ADDR *dstaddr)
{
	T_IFNET	*ifp = IF_GET_IFNET();

	if (ntohl(*dstaddr) == IPV4_ADDR_BROADCAST ||
	    ntohl(*dstaddr) == ((ifp->in_ifaddr.addr & ifp->in_ifaddr.mask) | ~ifp->in_ifaddr.mask))
		return true;
	else
		return IN_IS_DSTADDR_ACCEPT(myaddr, dstaddr);
	}

#endif	/* of #ifdef DHCP_CFG */

#endif	/* of #if defined(SUPPORT_INET4) */

#if defined(SUPPORT_INET6)

/*
 *  udp_dstaddr_accept -- UDP �Τ����襢�ɥ쥹���������������å����롣
 *
 *    �ʲ��ξ���ǡ���������������롣
 *
 *      �������襢�ɥ쥹���ޥ�����㥹��
 */

static bool_t
udp_is_dstaddr_accept (T_IN6_ADDR *myaddr, T_IN6_ADDR *dstaddr)
{
	if (IN6_IS_ADDR_MULTICAST(dstaddr))
		return true;
	else
		return IN_IS_DSTADDR_ACCEPT(myaddr, dstaddr);
	}

#endif	/* of #if defined(SUPPORT_INET6) */

/*
 *  udp_input -- UDP �����ϴؿ�
 */

uint_t
udp_input (T_NET_BUF **inputp, uint_t *offp, uint_t *nextp)
{
	T_NET_BUF	*input = *inputp;
	T_UDP_CEP	*cep;
	T_UDP_HDR	*udph;
	T_IP_HDR	*iph;
	int_t		ix;
	uint_t		len, hlen, ulen;

	hlen = (uint_t)GET_IF_IP_HDR_SIZE(input);

	NET_COUNT_UDP(net_count_udp.in_octets,  input->len - hlen);
	NET_COUNT_UDP(net_count_udp.in_packets, 1);
	NET_COUNT_MIB(udp_stats.udpInDatagrams, 1);

	/* �إå�Ĺ������å����롣*/
	if (input->len < hlen + UDP_HDR_SIZE)
		goto buf_rel;

	udph = (T_UDP_HDR *)(input->buf + *offp);

	/* �ǡ��������Ĺ������å����� */
	ulen  = ntohs(udph->ulen);
	if (ulen != input->len - hlen)
		goto buf_rel;

	/* ����ݡ��Ȥ� 0 �Υǡ����������˴����롣RFC768 */
	if (udph->dport == 0)
		goto buf_rel;

#ifdef UDP_CFG_IN_CHECKSUM

	/* �����å����������å����� */
	if (udph->sum && IN_CKSUM(input, IPPROTO_UDP, *offp, ulen) != 0)
		goto buf_rel;

#endif	/* of #ifdef UDP_CFG_IN_CHECKSUM */

	iph = GET_IP_HDR(input);

	/* ���襢�ɥ쥹�ȥݡ��Ȥ�����å����� */
	for (ix = tmax_udp_cepid; ix -- > 0; ) {
		cep = &udp_cep[ix];
		if (VALID_UDP_CEP(cep) &&
		    udp_is_dstaddr_accept(&cep->myaddr.ipaddr, &iph->dst) &&
		    ntohs(udph->dport) == cep->myaddr.portno) {
			len = (uint_t)(ntohs(udph->ulen) - UDP_HDR_SIZE);

			if (cep->rcv_tskid != TA_NULL) {	/* ��Υ�֥�å��󥰥�����ǥڥ�ǥ����� */
				if (tinet_snd_dtq(cep->rcvqid, (intptr_t)input) != E_OK)
					goto buf_rel;
				}

#ifdef UDP_CFG_NON_BLOCKING

			else if (cep->rcv_p_dstaddr != NULL) {	/* �Υ�֥�å��󥰥�����ǥڥ�ǥ����� */

				/* p_dstaddr �����ꤹ�롣*/
				cep->rcv_p_dstaddr->portno = ntohs(udph->sport);
				IN_COPY_TO_HOST(&cep->rcv_p_dstaddr->ipaddr, &GET_IP_HDR(input)->src);

				/* �ǡ�����Хåե��˰ܤ���*/
				memcpy(cep->rcv_data, GET_UDP_SDU(input, *offp),
				       (size_t)(len < cep->rcv_len ? len : cep->rcv_len));
				syscall(rel_net_buf(input));

				if (IS_PTR_DEFINED(cep->callback))

#ifdef TCP_CFG_NON_BLOCKING_COMPAT14

					(*cep->callback)(GET_UDP_CEPID(cep), TFN_UDP_RCV_DAT, (void*)(uint32_t)len);

#else	/* of #ifdef TCP_CFG_NON_BLOCKING_COMPAT14 */

					(*cep->callback)(GET_UDP_CEPID(cep), TFN_UDP_RCV_DAT, (void*)&len);

#endif	/* of #ifdef TCP_CFG_NON_BLOCKING_COMPAT14 */

				else
					syslog(LOG_WARNING, "[UDP] no call back, CEP: %d.", GET_UDP_CEPID(cep));
				cep->rcv_p_dstaddr = NULL;
				}

#endif	/* of #ifdef UDP_CFG_NON_BLOCKING */

			else
				goto buf_rel;

			return IPPROTO_DONE;
			}
		}

	NET_COUNT_MIB(udp_stats.udpNoPorts, 1);

#if defined(SUPPORT_INET4)

	/* ������ IP ���ɥ쥹���Ϥ����ǡ��������Τ� ICMP ���顼�����Τ��롣*/
	if (ntohl(iph->dst) == IF_GET_IFNET()->in_ifaddr.addr) {
		syslog(LOG_INFO, "[UDP] unexp port: %d.", ntohs(udph->dport));

		icmp_error(ICMP4_UNREACH_PORT, input);
		/* icmp_error �Ǥϡ��ͥåȥ���Хåե� input ���ֵѤ��ʤ��Τǲ���ȴ���롣*/
		}

#endif	/* of #if defined(SUPPORT_INET4) */

#if defined(SUPPORT_INET6)

	/* �ޥ�����㥹�ȥ��ɥ쥹���Ϥ����ǡ��������� ICMP ���顼�����Τ��ʤ���*/
	if (!IN6_IS_ADDR_MULTICAST(&iph->dst)) {
		syslog(LOG_INFO, "[UDP] unexp port: %d.", ntohs(udph->dport));

		/* icmp6_error �ǡ��ͥåȥ���Хåե� input ���ֵѤ��롣*/
		icmp6_error(input, ICMP6_DST_UNREACH, ICMP6_DST_UNREACH_NOPORT, 0);

		NET_COUNT_UDP(net_count_udp.in_err_packets, 1);
		NET_COUNT_MIB(udp_stats.udpInErrors, 1);
		return IPPROTO_DONE;
		}

#endif	/* of #if defined(SUPPORT_INET6) */

buf_rel:
	NET_COUNT_UDP(net_count_udp.in_err_packets, 1);
	NET_COUNT_MIB(udp_stats.udpInErrors, 1);
	syscall(rel_net_buf(input));
	return IPPROTO_DONE;
	}

#endif	/* of #ifdef SUPPORT_UDP */
