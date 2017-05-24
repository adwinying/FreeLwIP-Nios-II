/*
 *  TINET カーネル構成ファイル
 */

#include "Os.h"
#include "prc_sil.h"
#include "t_syslog.h"

#include <tinet_defs.h>
#include <tinet_config.h>
#include <tinet_atk2_cfg.h>

#include "tinet_cfg.h"

#include <net/if.h>
//#include <net/if_ppp.h>
//#include <net/if_loop.h>
#include <net/ethernet.h>
#include <net/net.h>
#include <net/net_buf.h>

#include <netinet/in.h>
#include <netinet/in_itron.h>
#include <netinet/ip.h>
//#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/tcp_fsm.h>
#include <netinet/udp_var.h>

#include "dataqueue.h"

#include "test_histogram.h"

/****** UDP (IPv4) 通信端点 ******/

#define TNUM_UDP_CEPID	1

const ID tmax_udp_cepid = (TMIN_UDP_CEPID + TNUM_UDP_CEPID - 1);

T_UDP4_CEP udp_cep[TNUM_UDP_CEPID] = {
	{
		TA_NULL,
		{ IPV4_ADDRANY, 7 },
		UDP_CEP_FLG_VALID,
		RES_UDP_CEP_LOCK1,
		TA_NULL,
		TA_NULL,
		DTQ_UDP_RCVQ1,
		},
	};

/****** Object DATAQUEUE ******/

/*
 *  データキュー管理領域
 */

/* UDP (IPv4) 通信端点 */
static DTQMB DTQMB_DTQ_UDP_RCVQ1[NUM_DTQ_UDP_RCVQ];

/*
 *  データキュー初期化ブロック
 */

const DTQINIB tinet_dtqinib_table[TNUM_TINET_DATAQUEUE] = {
	/* UDP (IPv4) 通信端点 */
	{
		NUM_DTQ_UDP_RCVQ,
		DTQMB_DTQ_UDP_RCVQ1
		}
	};

/*
 *  データキュー管理ブロック
 */

DTQCB tinet_dtqcb_table[TNUM_TINET_DATAQUEUE];

/*
 *  オブジェクトの初期化
 */

void
tinet_initialize(void)
{
	dataqueue_initialize((uint_t)TNUM_TINET_DATAQUEUE, tinet_dtqinib_table, tinet_dtqcb_table);
	}

/*
 *  データキューへの送信
 */

ER
tinet_snd_dtq(ID dtqid, intptr_t data)
{
	return snd_dtq((uint_t)TNUM_TINET_DATAQUEUE, tinet_dtqcb_table, dtqid, data);
	}

/*
 *  データキューからの受信
 */

ER
tinet_rcv_dtq(ID dtqid, intptr_t *p_data)
{
	return rcv_dtq((uint_t)TNUM_TINET_DATAQUEUE, tinet_dtqcb_table, dtqid, p_data);
	}
