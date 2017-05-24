#include <string.h>

#ifdef TARGET_KERNEL_ATK2

#include "Os.h"
#include "prc_sil.h"

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

#include <tinet_defs.h>
#include <tinet_config.h>

#include <net/if.h>
//#include <net/if_loop.h>
//#include <net/if_ppp.h>
#include <net/ethernet.h>
//#include <net/ppp.h>
#include <net/net.h>
#include <net/net_buf.h>
#include <net/net_count.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>

#if defined(SUPPORT_IPSEC)
#include <netinet6/ipsec.h>
#endif

static uint16_t frag_id = 0;

/*
 *  IP output -- IP の出力関数
 *
 *    注意: バージョンとヘッダ長は上位層で設定する。
 */

ER
ip_output (T_NET_BUF *output)
{
	T_IP4_HDR	*ip4h;
	ER		error = E_OK;
	T_IN4_ADDR	gw;

#ifdef SUPPORT_IPSEC
	T_SECPOLICY* sp;
	int ipsec_error;
	int policy;
#endif /* SUPPORT_IPSEC */

	NET_COUNT_MIB(ip_stats.ipOutRequests, 1);

#ifdef SUPPORT_IPSEC
	/* XXX: IPsecの処理はパケット断片化の前に行われる */

	/* SPDを取得する */
	sp = ipsec4_getpolicybyaddr(output, IPSEC_DIR_OUTBOUND, &ipsec_error);
	if (sp==NULL) {
		error = ipsec_error;
		return error;
	}

	/* ポリシをチェックする*/
#if 0
	switch (sp->policy) {

	case IPSEC_POLICY_BYPASS:
	case IPSEC_POLICY_NONE:
	case IPSEC_POLICY_TCP:
		goto skip_ipsec;

		break; /* NOTREACHED */

	case  IPSEC_POLICY_IPSEC:
		/* XXX: SH2ではここでillegal Instructionが発生する。コンパイラのバグ? */
		if (sp->req == NULL) {
			/* 鍵交換デーモンに対してSAの取得を要求する */
			goto bad;
		}
		break;

	default:
		/* 不正なポリシ */
		goto bad;
		break;
	}
#else /* 0 */
	/* XXX: コンパイラの不具合回避のため switchをやめif文で書き直した */
	policy = sp->spinitb->policy;

	if (policy == IPSEC_POLICY_BYPASS ||
	    policy == IPSEC_POLICY_NONE ||
	    policy == IPSEC_POLICY_TCP)
		goto skip_ipsec;
	else if (policy == IPSEC_POLICY_IPSEC) {
		if (sp->req == NULL)
			/* 鍵交換デーモンに対してSAの取得を要求する */
			goto bad;
	} else
		goto bad;

#endif /* 0 */

		{
			T_IPSEC_OUTPUT_STATE state;

			state.m = output;
			/* XXX: ipsec4_output()内では、ip->{len,off}をネットワークバイトオーダとする */
			error = ipsec4_output (&state, sp, 0 /* flags */ );
			output = state.m;

			if (error) {
				/* net_buf はipsec4_outputによって既に解放されている */
				switch (error) {
				default:
					syslog (LOG_NOTICE, "ip_output:%d error", __LINE__);
				}
			}
		}

	/* ipsec4_outputに影響を受けた値を更新する */
	/* XXX: 必要か? */

skip_ipsec:
#endif /* SUPPORT_IPSEC */

#ifdef IP4_CFG_FRAGMENT

	ip4h = GET_IP4_HDR(output);

	gw = in4_rtalloc(ntohl(ip4h->dst));

	/*
	 *  データグラムサイズがネットワークの MTU を超えていれば、
	 *  分割して送信する。
	 */
	if (ntohs(ip4h->len) > IF_MTU) {
		T_NET_BUF	*frag;
		T_IP4_HDR	*fip4h;
		uint_t		off, hlen, len, flen, align;

		NET_COUNT_IP4(net_count_ip4[NC_IP4_FRAG_OUT], 1);
		NET_COUNT_MIB(ip_stats.ipFragCreates, 1);

		len  = ntohs(ip4h->len);
		off  = hlen = GET_IP4_HDR_SIZE(ip4h);
		while (off < len) {
			if (off + (IF_MTU - IP4_HDR_SIZE) < len)
				flen = IF_MTU - IP4_HDR_SIZE;
			else
				flen = len - off;

			/* データ長は 4 オクテット境界に調整する。	*/
			align = (flen + 3) >> 2 << 2;

			/* ネットワークバッファを獲得する。*/
			if (get_net_buf(&frag, align + IF_IP4_HDR_SIZE) == E_OK) {

				/* フラグメントをコピーする。*/
				memcpy(GET_IP4_SDU(frag), (uint8_t *)ip4h + off, flen);

				/* ヘッダを埋める。*/
				fip4h		= GET_IP4_HDR(frag);
				*fip4h		= *ip4h;
				fip4h->flg_off	= htons(IP4_MAKE_FLGOFF(off + flen == len ? 0 : IP4_MF,
						                       (off - hlen) >> 3));
				fip4h->len	= htons(flen + IP4_HDR_SIZE);
				fip4h->id	= htons(frag_id);
				fip4h->sum	= 0;
				fip4h->sum	= in_cksum(fip4h, GET_IP4_HDR_SIZE(fip4h));

				NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_OCTETS], ntohs(fip4h->len));
				NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_PACKETS], 1);
				NET_COUNT_IP4(net_count_ip4[NC_IP4_FRAG_OUT_FRAGS], 1);

				IF_SET_PROTO(frag, IF_PROTO_IP);
				if ((error = IF_OUTPUT(frag, &gw, NULL)) != E_OK) {
					syscall(rel_net_buf(output));
					NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_ERR_PACKETS], 1);
					NET_COUNT_MIB(ip_stats.ipFragFails, 1);
					return error;
					}
				}
			else {
				/* 獲得できなければ、送信をあきらめる。*/
				syscall(rel_net_buf(output));
				NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_ERR_PACKETS], 1);
				NET_COUNT_MIB(ip_stats.ipFragFails, 1);
				return E_NOMEM;
				}

			off += IF_MTU - IP4_HDR_SIZE;
			}
		syscall(rel_net_buf(output));
		frag_id ++;
		NET_COUNT_MIB(ip_stats.ipFragOKs, 1);
		}
	else {

		/* ヘッダを埋める。*/
		ip4h->id  = htons(frag_id);
		frag_id ++;
		ip4h->sum = 0;
		ip4h->sum = in_cksum(ip4h, GET_IP4_HDR_SIZE(ip4h));

		NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_OCTETS], ntohs(ip4h->len));
		NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_PACKETS], 1);

		IF_SET_PROTO(output, IF_PROTO_IP);
		if ((error = IF_OUTPUT(output, &gw, NULL)) != E_OK)
			NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_ERR_PACKETS], 1);
		}

#else	/* of #ifdef IP4_CFG_FRAGMENT */

	ip4h = GET_IP4_HDR(output);

	/* データグラムサイズがネットワークの MTU を超えていればエラー */
	if (ntohs(ip4h->len) > IF_MTU)
		return E_PAR;

	/* ヘッダを埋める。*/
	ip4h->id  = htons(frag_id);
	frag_id ++;
	ip4h->sum = 0;
	ip4h->sum = in_cksum(ip4h, (uint_t)GET_IP4_HDR_SIZE(ip4h));

	NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_OCTETS], ntohs(ip4h->len));
	NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_PACKETS], 1);

	gw = in4_rtalloc(ntohl(ip4h->dst));
	IF_SET_PROTO(output, IF_PROTO_IP);
	if ((error = IF_OUTPUT(output, &gw, NULL)) != E_OK)
		NET_COUNT_IP4(net_count_ip4[NC_IP4_OUT_ERR_PACKETS], 1);
		NET_COUNT_MIB(ip_stats.ipOutDiscards, 1);

#endif	/* of #ifdef IP4_CFG_FRAGMENT */

#ifdef SUPPORT_IPSEC
	bad:
#endif /* SUPPORT_IPSEC */

	return error;
	}
