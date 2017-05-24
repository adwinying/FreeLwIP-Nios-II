/*
 *  ノンブロッキングコールを組込まない場合にリンクされる関数の定義
 */

#include <string.h>

#ifdef TARGET_KERNEL_ATK2

#include "Os.h"
#include "prc_sil.h"

#include "target_test.h"
#include "histogram.h"

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
//#include <net/ppp_ipcp.h>

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

/*
 *  IPv4 と IPv6 の切り替えマクロ
 */

#if defined(SUPPORT_INET4)

#define UDP_CRE_CEP	udp_cre_cep
#define UDP_SND_DAT	udp_snd_dat
#define UDP_RCV_DAT	udp_rcv_dat

#endif	/* of #if defined(SUPPORT_INET4) */

#if defined(SUPPORT_INET6)

#define UDP_CRE_CEP	udp6_cre_cep
#define UDP_SND_DAT	udp6_snd_dat
#define UDP_RCV_DAT	udp6_rcv_dat

#endif	/* of #if defined(SUPPORT_INET6) */

/*
 *  TINET をライブラリ化しない場合は、全ての機能を
 *  オブジェクトファイルに出力するため、マクロを有効にする。
 */

#ifndef UDP_CFG_LIBRARY

#define __udp_send_data
#define __udp_cre_cep
#define __udp_del_cep
#define __udp_can_cep
#define __udp_set_opt
#define __udp_get_opt
#define __udp_can_snd
#define __udp_can_rcv
#define __udp_snd_dat
#define __udp_rcv_dat

#endif	/* of #ifndef UDP_CFG_LIBRARY */

extern ObjectIDType	histid;
extern boolean		meas_flg;

#ifdef __udp_send_data

/*
 *  udp_send_data -- パケット送信の主要部
 */

ER_UINT
udp_send_data (T_UDP_CEP *cep, T_IPEP *p_dstaddr, void *data, int_t len)
{
	T_NET_BUF	*output;
	T_UDP_HDR	*udph;
	ER		error = E_OK;

#ifdef UDP_CFG_OUT_CHECKSUM
	uint16_t	sum;
#endif	/* of #ifdef UDP_CFG_OUT_CHECKSUM */

	NET_COUNT_UDP(net_count_udp.out_octets,  len);
	NET_COUNT_UDP(net_count_udp.out_packets, 1);

	/* IP データグラムを割り当てる。*/
	if ((error = IN_GET_DATAGRAM(&output, (uint_t)(UDP_HDR_SIZE + len), 0,
	                             &p_dstaddr->ipaddr,
	                             &cep->myaddr.ipaddr,
	                             IPPROTO_UDP, IP_DEFTTL,
	                             NBA_SEARCH_ASCENT)) != E_OK)
		goto err_ret;

	/* UDP ヘッダに情報を設定する。*/
	udph		= GET_UDP_HDR(output, IF_IP_UDP_HDR_OFFSET);
	udph->sport	= htons(cep->myaddr.portno);
	udph->dport	= htons(p_dstaddr->portno);
	udph->ulen	= htons(UDP_HDR_SIZE + len);
	udph->sum	= 0;

	/* データをコピーする。*/
	memcpy((void*)GET_UDP_SDU(output, IF_IP_UDP_HDR_OFFSET), data, (size_t)len);

#ifdef UDP_CFG_OUT_CHECKSUM

	sum = IN_CKSUM(output, IPPROTO_UDP, IF_IP_UDP_HDR_OFFSET, (uint_t)(UDP_HDR_SIZE + len));

	/* 計算したチェックサムの値が 0 なら 0xffff を入れる。*/
	if (sum == 0)
		sum = 0xffff;
	udph->sum = sum;

#endif	/* of #ifdef UDP_CFG_OUT_CHECKSUM */

	/* ネットワークバッファ長を調整する。*/
	output->len = (uint16_t)(IF_IP_UDP_HDR_SIZE + len);

	/* ネットワーク層 (IP) の出力関数を呼び出す。*/
	if ((error = IP_OUTPUT(output)) == E_OK) {
		NET_COUNT_MIB(udp_stats.udpOutDatagrams, 1);
		cep->snd_tskid = TA_NULL;
		return len;
		}

err_ret:
	NET_COUNT_UDP(net_count_udp.out_err_packets, 1);
	cep->snd_tskid = TA_NULL;
	return error;
	}

#endif	/* of #ifdef __udp_send_data */

/*
 *  udp_cre_cep -- UDP 通信端点の生成【拡張機能】
 */
#if 0
#ifdef __udp_cre_cep

#ifdef UDP_CFG_EXTENTIONS

ER
UDP_CRE_CEP (ID cepid, T_UDP_CCEP *pk_ccep)
{
	T_UDP_CEP	*cep;
	ER		error;

	/* UDP 通信端点 ID をチェックする。*/
	if (!VALID_UDP_CEPID(cepid))
		return E_ID;

	/* pk_ccep が NULL ならエラー */
	if (pk_ccep == NULL)
		return E_PAR;

	/* UDP 通信端点を得る。*/
	cep = GET_UDP_CEP(cepid);

	/* UDP 通信端点をチェックする。*/
	if (VALID_UDP_CEP(cep))
		return E_OBJ;

	/* UDP 通信端点が、動的生成用でなければエラー */
	if (!DYNAMIC_UDP_CEP(cep))
		return E_ID;

	/* 通信端点をロックする。*/
	syscall(GetResource(cep->resid_lock));

	/*
	 * UDP 通信端点をチェックする。生成済みであればエラー
	 */
	if (VALID_UDP_CEP(cep))
		error = E_OBJ;
	else {

		/*
		 *  自ポート番号が UDP_PORTANY なら、自動で割り当てる。
		 */
		if (pk_ccep->myaddr.portno == UDP_PORTANY)
			error = udp_alloc_auto_port(cep);
		else
			error = udp_alloc_port(cep, pk_ccep->myaddr.portno);

		if (error == E_OK) {

			/* UDP 通信端点生成情報をコピーする。*/
			cep->cepatr        = pk_ccep->cepatr;			/* 通信端点属性		*/
			cep->myaddr.ipaddr = pk_ccep->myaddr.ipaddr;		/* 自分のアドレス	*/
			cep->callback      = (void*)pk_ccep->callback;		/* コールバック		*/

			/* UDP 通信端点を生成済みにする。*/
			cep->flags |= UDP_CEP_FLG_VALID;
			}
		}

	/* 通信端点のロックを解除する。*/
	syscall(ReleaseResource(cep->resid_lock));

	return error;
	}

#endif	/* of #ifdef UDP_CFG_EXTENTIONS */

#endif	/* of #ifdef __udp_cre_cep */
#endif
/*
 *  udp_del_cep -- UDP 通信端点の削除【拡張機能】
 */
#if 0
#ifdef __udp_del_cep

#ifdef UDP_CFG_EXTENTIONS

ER
udp_del_cep (ID cepid)
{
	T_UDP_CEP	*cep;
	ER		error;

	/* UDP 通信端点 ID をチェックする。*/
	if (!VALID_UDP_CEPID(cepid))
		return E_ID;

	/* UDP 通信端点を得る。*/
	cep = GET_UDP_CEP(cepid);

	/* UDP 通信端点をチェックする。*/
	if (!VALID_UDP_CEP(cep))
		return E_NOEXS;

	/* UDP 通信端点が、動的生成用でなければエラー */
	if (!DYNAMIC_UDP_CEP(cep))
		return E_ID;

	/* 通信端点をロックする。*/
	syscall(GetResource(cep->resid_lock));

	/*
	 * UDP 通信端点をチェックする。未生成の場合はエラー
	 * ・未生成。
	 */
	if (!VALID_UDP_CEP(cep))
		error = E_NOEXS;
	else {
		udp_can_snd(cep, E_DLT);
		udp_can_rcv(cep, E_DLT);

		/* UDP 通信端点を未生成にする。*/
		cep->flags &= ~UDP_CEP_FLG_VALID;
		error = E_OK;
		}

	/* 通信端点のロックを解除する。*/
	syscall(ReleaseResource(cep->resid_lock));

	return error;
	}

#endif	/* of #ifdef UDP_CFG_EXTENTIONS */

#endif	/* of #ifdef __udp_del_cep */
#endif
#ifdef __udp_can_cep

/*
 *  udp_can_cep -- ペンディングしている処理のキャンセル【標準機能】
 */

ER
udp_can_cep (ID cepid, FN fncd)
{
	T_UDP_CEP	*cep;
	ER		error = E_OK, snd_err, rcv_err;

	/* API 機能コードをチェックする。*/
	if (!VALID_TFN_UDP_CAN(fncd))
		return E_PAR;

	/* UDP 通信端点 ID をチェックする。*/
	if (!VALID_UDP_CEPID(cepid))
		return E_ID;

	/* UDP 通信端点を得る。*/
	cep = GET_UDP_CEP(cepid);

	/* UDP 通信端点をチェックする。*/
	if (!VALID_UDP_CEP(cep))
		return E_NOEXS;

	/* 通信端点をロックする。*/
	syscall(GetResource(cep->resid_lock));

	if (fncd == TFN_UDP_ALL) {	/* TFN_UDP_ALL の処理 */

		snd_err = udp_can_snd(cep, E_RLWAI);
		rcv_err = udp_can_rcv(cep, E_RLWAI);

		/*
		 *  snd_err と rcv_err のどちらも EV_NOPND
		 *  なら、ペンディングしていないのでエラー
		 */
		if (snd_err == EV_NOPND && rcv_err == EV_NOPND)
			error = E_OBJ;
		else {
			if (snd_err == EV_NOPND)
				snd_err = E_OK;
			if (rcv_err == EV_NOPND)
				rcv_err = E_OK;

			if (snd_err != E_OK)
				error = snd_err;
			else if (rcv_err != E_OK)
				error = rcv_err;
			}
		}
	else if (fncd == TFN_UDP_SND_DAT) {	/* 送信処理のキャンセル */
		if ((error = udp_can_snd(cep, E_RLWAI)) == EV_NOPND)
			error = E_OBJ;
		}
	else if (fncd == TFN_UDP_RCV_DAT) {	/* 受信処理のキャンセル */
		if ((error = udp_can_rcv(cep, E_RLWAI)) == EV_NOPND)
			error = E_OBJ;
		}
	else
		error = E_PAR;

	/* 通信端点をロックを解除する。*/
	syscall(ReleaseResource(cep->resid_lock));

	return error;
	}

#endif	/* of #ifdef __udp_can_cep */

/*
 *  udp_set_opt -- UDP 通信端点オプションの設定【拡張機能】
 *
 *  注意: 設定可能な UDP 通信端点オプションは無いため、E_PAR が返される。
 */
#if 0
#ifdef __udp_set_opt

#ifdef UDP_CFG_EXTENTIONS

ER
udp_set_opt (ID cepid, int_t optname, void *optval, int_t optlen)
{
	T_UDP_CEP	*cep;

	/* UDP 通信端点 ID をチェックする。*/
	if (!VALID_UDP_CEPID(cepid))
		return E_ID;

	/* UDP 通信端点を得る。*/
	cep = GET_UDP_CEP(cepid);

	/* UDP 通信端点をチェックする。*/
	if (!VALID_UDP_CEP(cep))
		return E_NOEXS;

	return E_PAR;
	}

#endif	/* of #ifdef UDP_CFG_EXTENTIONS */

#endif	/* of #ifdef __udp_set_opt */
#endif
/*
 *  udp_get_opt -- UDP 通信端点オプションの設定【拡張機能】
 *
 *  注意: 設定可能な UDP 通信端点オプションは無いため、E_PAR が返される。
 */
#if 0
#ifdef __udp_get_opt

#ifdef UDP_CFG_EXTENTIONS

ER
udp_get_opt (ID cepid, int_t optname, void *optval, int_t optlen)
{
	T_UDP_CEP	*cep;

	/* UDP 通信端点 ID をチェックする。*/
	if (!VALID_UDP_CEPID(cepid))
		return E_ID;

	/* UDP 通信端点を得る。*/
	cep = GET_UDP_CEP(cepid);

	/* UDP 通信端点をチェックする。*/
	if (!VALID_UDP_CEP(cep))
		return E_NOEXS;

	return E_PAR;
	}

#endif	/* of #ifdef UDP_CFG_EXTENTIONS */

#endif	/* of #ifdef __udp_get_opt */
#endif
#ifdef UDP_CFG_NON_BLOCKING

#include "udp_usrreq_nblk.c"

#else	/* of #ifdef UDP_CFG_NON_BLOCKING */

#ifdef __udp_can_snd

/*
 *  udp_can_snd -- ペンディングしている送信のキャンセル
 */

ER
udp_can_snd (T_UDP_CEP *cep, ER error)
{
	if (cep->snd_tskid != TA_NULL) {	/* 非ノンブロッキングコールでペンディング中 */

#ifdef UDP_CFG_EXTENTIONS

		/* 待ち中に発生したエラー情報を設定する。*/
		cep->error = error;

#endif	/* of #ifdef UDP_CFG_EXTENTIONS */

		error = E_OK;
		cep->snd_tskid = TA_NULL;
		}
	else					/* どちらでもないならペンディングしていない */
		error = EV_NOPND;

	return error;
	}

#endif	/* of #ifdef __udp_can_snd */

#ifdef __udp_can_rcv

/*
 *  udp_can_rcv -- ペンディングしている受信のキャンセル
 */

ER
udp_can_rcv (T_UDP_CEP *cep, ER error)
{
	if (cep->rcv_tskid != TA_NULL) {	/* 非ノンブロッキングコールでペンディング中 */

#ifdef UDP_CFG_EXTENTIONS

		/* 待ち中に発生したエラー情報を設定する。*/
		cep->error = error;

#endif	/* of #ifdef UDP_CFG_EXTENTIONS */

		error = E_OK;
		cep->rcv_tskid = TA_NULL;
		}
	else					/* どちらでもないならペンディングしていない */
		error = EV_NOPND;

	return error;
	}

#endif	/* of #ifdef __udp_can_rcv */

#ifdef __udp_snd_dat

/*
 *  udp_snd_dat -- パケットの送信【標準機能】
 */

ER_UINT
UDP_SND_DAT (ID cepid, T_IPEP *p_dstaddr, void *data, int_t len)
{
	T_UDP_CEP	*cep;
	ER		error;

	/* p_dstaddr または data が NULL ならエラー */
	if (p_dstaddr == NULL || data == NULL)
		return E_PAR;

	/* データ長をチェックする。*/
	if (len < 0 || len + IP_HDR_SIZE + UDP_HDR_SIZE > IF_MTU)
		return E_PAR;

	/* UDP 通信端点 ID をチェックする。*/
	if (!VALID_UDP_CEPID(cepid))
		return E_ID;

	/* UDP 通信端点を得る。*/
	cep = GET_UDP_CEP(cepid);

	/* UDP 通信端点をチェックする。*/
	if (!VALID_UDP_CEP(cep))
		return E_NOEXS;

	/*
	 *  自ポート番号が UDP_PORTANY なら、自動で割り当てる。
	 */
	if (cep->myaddr.portno == UDP_PORTANY) {
		if ((error = udp_alloc_auto_port(cep)) != E_OK)
			return error;
		}

	/* 通信端点をロックする。*/
	syscall(GetResource(cep->resid_lock));

	if (cep->snd_tskid != TA_NULL) {

		/* 非ノンブロッキングコールでペンディング中 */
		error = E_QOVR;

		/* 通信端点をロックを解除する。*/
		syscall(ReleaseResource(cep->resid_lock));
		}
	else {
		/* 現在のタスク識別子を記録する。*/
		GetTaskID(&(cep->snd_tskid));

#ifdef UDP_CFG_EXTENTIONS

		/* 待ち中に発生したエラー情報をリセットする。*/
		cep->error = E_OK;

#endif	/* of #ifdef UDP_CFG_EXTENTIONS */

		/* 通信端点をロックを解除する。*/
		syscall(ReleaseResource(cep->resid_lock));

		/* パケットを送信する。*/
		error = udp_send_data(cep, p_dstaddr, data, len);

#ifdef UDP_CFG_EXTENTIONS

		/* 待ち中に発生したエラー情報を返す。*/
		if (error == E_RLWAI)
			error = cep->error;

#endif	/* of #ifdef UDP_CFG_EXTENTIONS */
		}

	return error;
	}

#endif	/* of #ifdef __udp_snd_dat */

#ifdef __udp_rcv_dat

/*
 *  udp_rcv_dat -- パケットの受信【標準機能】
 */

ER_UINT
UDP_RCV_DAT (ID cepid, T_IPEP *p_dstaddr, void *data, int_t len)
{
	T_NET_BUF	*input;
	T_UDP_CEP	*cep;
	T_UDP_HDR	*udph;
	ER_UINT		error;
	uint_t		ulen, uhoff;
	TaskType	tskid;

//	if (histid <= TNUM_HIST) {
//		begin_measure(histid);
//		meas_flg = TRUE;
//		}
//	if (histid <= TNUM_HIST && meas_flg == TRUE) {
//		end_measure(histid++);
//		meas_flg = FALSE;
//		}

	/* p_dstaddr または data が NULL か、len < 0 ならエラー */
	if (p_dstaddr == NULL || data == NULL || len < 0)
		return E_PAR;

	/* UDP 通信端点 ID をチェックする。*/
	if (!VALID_UDP_CEPID(cepid))
		return E_ID;

	/* UDP 通信端点を得る。*/
	cep = GET_UDP_CEP(cepid);

	/* UDP 通信端点をチェックする。*/
	if (!VALID_UDP_CEP(cep))
		return E_NOEXS;

	/* 通信端点をロックする。*/
	syscall(GetResource(cep->resid_lock));

	GetTaskID(&tskid);

	if (cep->rcv_tskid != TA_NULL) {
		if (cep->rcv_tskid == tskid)
			goto rel_res;
		else {

			/* 非ノンブロッキングコールでペンディング中 */
			error = E_QOVR;

			/* 通信端点をロックを解除する。*/
			syscall(ReleaseResource(cep->resid_lock));
			}
		}
	else {

		/* 現在のタスク識別子を記録する。*/
		GetTaskID(&(cep->rcv_tskid));

#ifdef UDP_CFG_EXTENTIONS

		/* 待ち中に発生したエラー情報をリセットする。*/
		cep->error = E_OK;

#endif	/* of #ifdef UDP_CFG_EXTENTIONS */

rel_res:
		/* 通信端点をロックを解除する。*/
		syscall(ReleaseResource(cep->resid_lock));

		/* 入力があるまで待つ。*/
		if (cep->cb_netbuf != NULL) {

			/*
			 *  ここにくる場合は、コールバック関数の中から
			 *  udp_rcv_dat を呼び出していることになり、
			 *  すでに入力済みである。
			 */
			input = cep->cb_netbuf;
			cep->cb_netbuf = NULL;
			}
		else if ((error = tinet_rcv_dtq(cep->rcvqid, (intptr_t*)&input)) != E_OK) {
			if (error == E_TMOUT)
				return error;

#ifdef UDP_CFG_EXTENTIONS

			/* 待ち中に発生したエラー情報を返す。*/
			if (error == E_RLWAI)
				error = cep->error;

#endif	/* of #ifdef UDP_CFG_EXTENTIONS */

			cep->rcv_tskid = TA_NULL;
			return error;
			}

		/* p_dstaddr を設定する。*/
		uhoff = (uint_t)GET_UDP_HDR_OFFSET(input);
		udph = GET_UDP_HDR(input, uhoff);
		p_dstaddr->portno = ntohs(udph->sport);
		IN_COPY_TO_HOST(&p_dstaddr->ipaddr, &GET_IP_HDR(input)->src);

		/* データをバッファに移す。*/
		ulen = ntohs(udph->ulen);
		if (ulen - UDP_HDR_SIZE > len)
			error = E_BOVR;
		else {
			len   =  (uint_t)(ulen - UDP_HDR_SIZE);
			error = (ER_UINT)(ulen - UDP_HDR_SIZE);
			}

		memcpy(data, GET_UDP_SDU(input, uhoff), (size_t)len);

		syscall(rel_net_buf(input));

		cep->rcv_tskid = TA_NULL;
		}

	return error;
	}

#endif	/* of #ifdef __udp_rcv_dat */

#endif	/* of #ifdef UDP_CFG_NON_BLOCKING */

#endif	/* of #ifdef SUPPORT_UDP */
