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
#include <net/net.h>
#include <net/net_buf.h>
#include <net/net_count.h>

#include <netinet/in.h>
//#include <netinet6/in6.h>
#include <netinet/in_var.h>
#include <netinet/in_itron.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/ip_icmp.h>
//#include <netinet6/in6_var.h>
//#include <netinet/ip6.h>
//#include <netinet6/ip6_var.h>
//#include <netinet6/nd6.h>
//#include <netinet/icmp6.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>

#ifdef SUPPORT_UDP

#ifdef UDP_CFG_NON_BLOCKING
#if 0
/*
 *  関数
 */

static void udp_output (T_UDP_CEP *cep);

/*
 *  UDP 出力タスク
 *  ノンブロッキングコールを組み込んだとき使用する。
 */

static void
udp_output (T_UDP_CEP *cep)
{
	T_NET_BUF	*output;
	T_UDP_HDR	*udph;
	ER_UINT		error;

#ifdef UDP_CFG_OUT_CHECKSUM
	uint16_t	sum;
#endif	/* of #ifdef UDP_CFG_OUT_CHECKSUM */

	/* IP データグラムを割り当てる。*/
	if ((error = IN_GET_DATAGRAM(&output, (uint_t)(UDP_HDR_SIZE + cep->snd_len), 0,
	                             &cep->snd_p_dstaddr->ipaddr,
	                             &cep->myaddr.ipaddr,
	                             IPPROTO_UDP, IP_DEFTTL,
	                             NBA_SEARCH_ASCENT)) != E_OK) {
		goto err_ret;
		}

	/* UDP ヘッダに情報を設定する。*/
	udph		= GET_UDP_HDR(output, IF_IP_UDP_HDR_OFFSET);
	udph->sport	= htons(cep->myaddr.portno);
	udph->dport	= htons(cep->snd_p_dstaddr->portno);
	udph->ulen	= htons(UDP_HDR_SIZE + cep->snd_len);
	udph->sum	= 0;

	/* データをコピーする。*/
	memcpy((void*)GET_UDP_SDU(output, IF_IP_UDP_HDR_OFFSET),
	       cep->snd_data, (size_t)cep->snd_len);

#ifdef UDP_CFG_OUT_CHECKSUM

	sum = IN_CKSUM(output, IPPROTO_UDP, IF_IP_UDP_HDR_OFFSET,
	               (uint_t)(UDP_HDR_SIZE + cep->snd_len));

	/* 計算したチェックサムの値が 0 なら 0xffff を入れる。*/
	if (sum == 0)
		sum = UINT_C(0xffff);
	udph->sum = sum;

#endif/* of #ifdef UDP_CFG_OUT_CHECKSUM */

	/* ネットワークバッファ長を調整する。*/
	output->len = (uint16_t)(IF_IP_UDP_HDR_SIZE + cep->snd_len);

	/* ネットワーク層 (IP) の出力関数を呼び出す。*/
	if ((error = IP_OUTPUT(output)) != E_OK)
		goto err_ret;


	NET_COUNT_MIB(udp_stats.udpOutDatagrams, 1);

	if (IS_PTR_DEFINED(cep->callback)) {

		if (error == E_OK)
			error = cep->snd_len;

#ifdef TCP_CFG_NON_BLOCKING_COMPAT14

		(*cep->callback)(GET_UDP_CEPID(cep), TFN_UDP_SND_DAT, (void*)error);

#else	/* of #ifdef TCP_CFG_NON_BLOCKING_COMPAT14 */

		(*cep->callback)(GET_UDP_CEPID(cep), TFN_UDP_SND_DAT, (void*)&error);

#endif	/* of #ifdef TCP_CFG_NON_BLOCKING_COMPAT14 */

		}
	else
		syslog(LOG_WARNING, "[UDP] no call back, CEP: %d.", GET_UDP_CEPID(cep));
	cep->snd_p_dstaddr = NULL;
	return;

err_ret:
	NET_COUNT_UDP(net_count_udp.out_err_packets, 1);
	if (IS_PTR_DEFINED(cep->callback)) {

#ifdef TCP_CFG_NON_BLOCKING_COMPAT14

		(*cep->callback)(GET_UDP_CEPID(cep), TFN_UDP_SND_DAT, (void*)E_NOMEM);

#else	/* of #ifdef TCP_CFG_NON_BLOCKING_COMPAT14 */

		error = E_NOMEM;
		(*cep->callback)(GET_UDP_CEPID(cep), TFN_UDP_SND_DAT, (void*)&error);

#endif	/* of #ifdef TCP_CFG_NON_BLOCKING_COMPAT14 */

		}
	else
		syslog(LOG_WARNING, "[UDP] no call back, CEP: %d.", GET_UDP_CEPID(cep));
	cep->snd_p_dstaddr = NULL;
	return;
	}

/*
 *  UDP 出力タスク
 *  ノンブロッキングコールを組み込んだとき使用する。
 */

TASK(UDP_OUTPUT_TASK)
{
	TaskType	tskid;
	int_t		ix;

	GetTaskID(&tskid);
	syslog(LOG_NOTICE, "[UDP OUTPUT:%d] started.", tskid);


#if defined(SUPPORT_INET6) && !defined(SUPPORT_TCP)

	/* IPv6 のステートレス・アドレス自動設定を実行する。*/
	in6_if_up(IF_GET_IFNET());

#endif	/* of #if defined(SUPPORT_INET6) && !defined(SUPPORT_TCP) */

	while (true) {

		/* 出力がポストされるまで待つ。*/
		syscall(WaitEvent(EVT_UDP_POST_OUTPUT));
		syscall(ClearEvent(EVT_UDP_POST_OUTPUT));

		for (ix = tmax_udp_cepid; ix -- > 0; ) {

			if (udp_cep[ix].flags & UDP_CEP_FLG_POST_OUTPUT) {
				udp_cep[ix].flags &= ~UDP_CEP_FLG_POST_OUTPUT;
				udp_output(&udp_cep[ix]);
				}
			}
		}
	}
#endif
#endif	/* of #ifdef UDP_CFG_NON_BLOCKING */

#endif	/* of #ifdef SUPPORT_UDP */
