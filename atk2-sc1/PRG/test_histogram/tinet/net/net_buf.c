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
#include <net/net.h>
#include <net/net_buf.h>
#include <net/net_count.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
//#include <netinet6/in6.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
//#include <netinet/ip6.h>
//#include <netinet6/ip6_var.h>
#include <netinet/tcp.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>

/*
 *  関数
 */

extern const char8 *atk2_strerror(StatusType ercd);

/*
 *  変数
 */

static T_NET_BUF_ENTRY net_buf_table[] = {

#if defined(NUM_MPF_NET_BUF6_65536) && NUM_MPF_NET_BUF6_65536 > 0
	{
		MPF_NET_BUF6_65536,
		IF_IP6_HDR_SIZE + 65536,

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF6_65536,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF6_65536) && NUM_MPF_NET_BUF6_65536 > 0 */

#if defined(NUM_MPF_NET_BUF6_REASSM) && NUM_MPF_NET_BUF6_REASSM > 0
	{
		MPF_NET_BUF_REASSM,
		IP6_CFG_FRAG_REASSM_SIZE,

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF6_REASSM,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF6_REASSM) && NUM_MPF_NET_BUF6_REASSM > 0 */

#if defined(NUM_MPF_NET_BUF4_REASSM) && NUM_MPF_NET_BUF4_REASSM > 0
	{
		MPF_NET_BUF_REASSM,
		IP4_CFG_FRAG_REASSM_SIZE,

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF4_REASSM,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF4_REASSM) && NUM_MPF_NET_BUF4_REASSM > 0 */

#if defined(NUM_MPF_NET_BUF_IF_PDU) && NUM_MPF_NET_BUF_IF_PDU > 0
	{
		MPF_NET_BUF_IF_PDU,
		IF_PDU_SIZE,

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF_IF_PDU,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_IF_PDU) && NUM_MPF_NET_BUF_IF_PDU > 0 */

#if defined(NUM_MPF_NET_BUF_IPV6_MMTU) && NUM_MPF_NET_BUF_IPV6_MMTU > 0
	{
		MPF_NET_BUF_IPV6_MMTU,
		IF_HDR_SIZE + IPV6_MMTU,

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF_IPV6_MMTU,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_IPV6_MMTU) && NUM_MPF_NET_BUF_IPV6_MMTU > 0 */

#if defined(NUM_MPF_NET_BUF_1024) && NUM_MPF_NET_BUF_1024 > 0
	{
		MPF_NET_BUF_1024,
		UINT_C(1024),

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF_1024,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_1024) && NUM_MPF_NET_BUF_1024 > 0 */

#if defined(NUM_MPF_NET_BUF_IP_MSS) && NUM_MPF_NET_BUF_IP_MSS > 0
	{
		MPF_NET_BUF_IP_MSS,
		IF_HDR_SIZE + IP4_MSS,

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF_IP_MSS,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_IP_MSS) && NUM_MPF_NET_BUF_IP_MSS > 0 */

#if defined(NUM_MPF_NET_BUF_512) && NUM_MPF_NET_BUF_512 > 0
	{
		MPF_NET_BUF_512,
		UINT_C(512),

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF_512,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_512) && NUM_MPF_NET_BUF_512 > 0 */

#if defined(NUM_MPF_NET_BUF_256) && NUM_MPF_NET_BUF_256 > 0
	{
		MPF_NET_BUF_256,
		UINT_C(256),

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF_256,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_256) && NUM_MPF_NET_BUF_256 > 0 */

#if defined(NUM_MPF_NET_BUF_128) && NUM_MPF_NET_BUF_128 > 0
	{
		MPF_NET_BUF_128,
		UINT_C(128),

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF_128,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_128) && NUM_MPF_NET_BUF_128 > 0 */

#if defined(SUPPORT_INET4)

#if defined(NUM_MPF_NET_BUF_64) && NUM_MPF_NET_BUF_64 > 0
	{
		MPF_NET_BUF_64,
		UINT_C(64),

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF_64,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_64) && NUM_MPF_NET_BUF_64 > 0 */

#endif	/* of #if defined(SUPPORT_INET4) */

#if defined(NUM_MPF_NET_BUF_CSEG) && NUM_MPF_NET_BUF_CSEG > 0
	{
		MPF_NET_BUF_CSEG,
		IF_HDR_SIZE + IP_HDR_SIZE + TCP_HDR_SIZE,

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

		NUM_MPF_NET_BUF_CSEG,

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

		},
#endif	/* of #if defined(NUM_MPF_NET_BUF_CSEG) && NUM_MPF_NET_BUF_CSEG > 0 */

	};

#if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF

/*
 *  nbuf_get_tbl -- ネットワークバッファ管理表を獲得する。
 *
 *    注意: ネットワーク統計情報の計測用
 */

const T_NET_BUF_ENTRY *
nbuf_get_tbl (void)
{
	return net_buf_table;
	}

/*
 *  nbuf_get_tbl_size -- ネットワークバッファ管理エントリ数を獲得する。
 *
 *    注意: ネットワーク統計情報の計測用
 */

uint_t
nbuf_get_tbl_size (void)
{
	return sizeof(net_buf_table) / sizeof(T_NET_BUF_ENTRY);
	}

#endif	/* of #if NET_COUNT_ENABLE & PROTO_FLG_NET_BUF */

/*
 *  get_net_buf_up -- 大きなサイズの方向に探索して、ネットワークバッファを獲得する。
 */

static ER
get_net_buf_up (T_NET_BUF **buf, uint_t minlen, uint_t maxlen)
{
	int_t	ix, req_ix;
	ER	error = E_OK;

	/* 最適なサイズの固定長メモリプールを探す。*/
	ix = sizeof(net_buf_table) / sizeof(T_NET_BUF_ENTRY);
	while (ix -- > 0 && minlen > net_buf_table[ix].size)
		;
	req_ix = ix;

	NET_COUNT_NET_BUF(net_buf_table[req_ix].requests, 1);

	while (1) {
		if ((error = net_atk2_get_mpf((ID)net_buf_table[ix].index, (void*)buf)) == E_OK) {
			(*buf)->idix  = (uint8_t)ix;
			(*buf)->len   = (uint16_t)minlen;
			(*buf)->flags = 0;
			NET_COUNT_NET_BUF(net_buf_table[ix].allocs, 1);
			return error;
			}
		else if (ix == 0 || net_buf_table[ix].size > maxlen)
			break;
		ix --;
		}

	syslog(LOG_WARNING, "[NET BUF] busy, index=%d, len=%4d.", (uint16_t)req_ix, minlen);
	*buf = NULL;
	NET_COUNT_NET_BUF(net_buf_table[req_ix].errors, 1);
	return error;
	}

/*
 *  get_net_buf_down -- 小さなサイズの方向に探索して、ネットワークバッファを獲得する。
 */

static ER
get_net_buf_down (T_NET_BUF **buf, uint_t minlen, uint_t maxlen)
{
	int_t	ix, req_ix;
	ER	error = E_OK;

	/* 最適なサイズの固定長メモリプールを探す。*/
	ix = sizeof(net_buf_table) / sizeof(T_NET_BUF_ENTRY);
	while (ix -- > 0 && maxlen > net_buf_table[ix].size)
		;
	req_ix = ix;

	NET_COUNT_NET_BUF(net_buf_table[req_ix].requests, 1);

	while (1) {
		if ((error = net_atk2_get_mpf((ID)net_buf_table[ix].index, (void*)buf)) == E_OK) {
			(*buf)->idix  = (uint8_t)ix;
			(*buf)->len   = net_buf_table[ix].size;
			(*buf)->flags = 0;
			NET_COUNT_NET_BUF(net_buf_table[ix].allocs, 1);
			return error;
			}
		ix ++;
		if (ix == sizeof(net_buf_table) / sizeof(T_NET_BUF_ENTRY) || net_buf_table[ix].size < minlen)
			break;
		}

	syslog(LOG_WARNING, "[NET BUF] busy, index=%d, len=%4d.", (uint16_t)req_ix, maxlen);
	*buf = NULL;
	NET_COUNT_NET_BUF(net_buf_table[req_ix].errors, 1);
	return error;
	}

/*
 *  get_net_buf_ex -- ネットワークバッファを獲得する（拡張機能）。
 */

ER
get_net_buf_ex (T_NET_BUF **buf, uint_t minlen, uint_t maxlen, ATR nbatr)
{
	/* 最小サイズが最大サイズを超えていたらエラー */
	if ((nbatr & NBA_SEARCH_DESCENT) != 0 && minlen > maxlen) {
		syslog(LOG_WARNING, "[NET BUF] E_PAR, minlen=%4d > maxlen=%4d.", minlen, maxlen);
		return E_PAR;
		}

	/* 割当て可能な最大のサイズを超えていたらエラー */
	if (minlen > net_buf_table[0].size || maxlen > net_buf_table[0].size) {
		syslog(LOG_WARNING, "[NET BUF] E_PAR, minlen=%4d or maxlen=%4d > %4d",
		                    minlen, maxlen, net_buf_table[0].size);
		return E_PAR;
		}
#if 0
	/* TCP で予約したネットワークバッファを取り出す。*/
	if ((nbatr & NBA_RESERVE_TCP) != 0) {
		if ((*buf = TCP_PULL_RES_NBUF(nbatr)) != NULL) {
			return E_OK;
			}
		}
#endif
	if ((nbatr & NBA_SEARCH_DESCENT) != 0)
		return get_net_buf_down(buf, minlen, maxlen);
	else if ((nbatr & NBA_SEARCH_ASCENT) != 0)
		return get_net_buf_up(buf, minlen, (uint_t)net_buf_table[0].size);
	else {
		syslog(LOG_WARNING, "[NET BUF] E_PAR, nbatr=%08x.", nbatr);
		return E_PAR;
		}
	}

/*
 *  get_net_buf -- ネットワークバッファを獲得する（互換）。
 */

ER
get_net_buf (T_NET_BUF **buf, uint_t len)
{
	return get_net_buf_ex(buf, len, (uint_t)net_buf_table[0].size, NBA_SEARCH_ASCENT);
	}

/*
 * rel_net_buf -- ネットワークバッファを返却する。
 */

ER
rel_net_buf (T_NET_BUF *buf)
{
	ER	error = E_OK;

	/* ネットワークバッファの ID の正当性を検証する。*/
	if ((int_t)buf->idix >= (int_t)(sizeof(net_buf_table) / sizeof(T_NET_BUF_ENTRY))) {
		syslog(LOG_WARNING, "[NET BUF] E_ID, ID=%d.", buf->idix);
		error = E_ID;
		}
	else {
#if 0
		/* TCP で、ネットワークバッファを予約する。*/
		if (TCP_PUSH_RES_NBUF(buf) == NULL)
			return E_OK;
#endif
		/* 固定メモリプールに返す。*/
		if ((error = net_atk2_rel_mpf((ID)net_buf_table[buf->idix].index, buf)) != E_OK)
			syslog(LOG_WARNING, "[NET BUF] %s, ID=%d.", atk2_strerror(error), buf->idix);
		}
	return error;
	}

/*
 * rus_net_buf -- ネットワークバッファを再利用する。
 */

ER
rus_net_buf (T_NET_BUF *buf)
{
	ER	error = E_OK;

	if (buf->idix >= sizeof(net_buf_table) / sizeof(T_NET_BUF_ENTRY)) {
		syslog(LOG_WARNING, "[NET BUF] E_ID, ID=%d.", buf->idix);
		error = E_ID;
		}
	else {
		buf->len   = net_buf_table[buf->idix].size;
		buf->flags = 0;
		error = E_OK;
		}
	return error;
	}

/*
 * net_buf_siz -- ネットワークバッファのサイズを返す。
 */

ER_UINT
net_buf_siz (T_NET_BUF *buf)
{
	if (buf->idix >= sizeof(net_buf_table) / sizeof(T_NET_BUF_ENTRY)) {
		syslog(LOG_WARNING, "[BUF SIZ] E_ID, ID=%d.", buf->idix);
		return E_ID;
		}
	else
		return (ER_UINT)net_buf_table[buf->idix].size;
	}

/*
 * net_buf_max_siz -- ネットワークバッファの最大サイズを返す。
 */

uint_t
net_buf_max_siz (void)
{
	return (uint_t)net_buf_table[0].size;
	}
