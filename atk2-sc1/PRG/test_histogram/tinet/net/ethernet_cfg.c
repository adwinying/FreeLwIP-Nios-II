/*
 *  Ethernet コンフィギュレーションファイル
 */

#include "Os.h"
#include "t_syslog.h"

#include <tinet_atk2_cfg.h>

#include "dataqueue.h"

#ifdef SUPPORT_ETHER

/****** Object DATAQUEUE ******/

/*
 *  データキュー管理領域
 */

static DTQMB DTQMB_DTQ_ETHER_OUTPUT[NUM_DTQ_ETHER_OUTPUT];

/*
 *  データキュー初期化ブロック
 */

const DTQINIB ethernet_dtqinib_table[TNUM_ETHERNET_DATAQUEUE] = {
	{
		NUM_DTQ_ETHER_OUTPUT,
		DTQMB_DTQ_ETHER_OUTPUT
		}
	};

/*
 *  データキュー管理ブロック
 */

DTQCB ethernet_dtqcb_table[TNUM_ETHERNET_DATAQUEUE];

/*
 *  オブジェクトの初期化
 */

void
ethernet_initialize (void)
{
	dataqueue_initialize((uint_t)TNUM_ETHERNET_DATAQUEUE, ethernet_dtqinib_table, ethernet_dtqcb_table);
	}

/*
 *  データキューへの送信
 */

ER
ethernet_snd_dtq (ID dtqid, intptr_t data)
{
	return snd_dtq((uint_t)TNUM_ETHERNET_DATAQUEUE, ethernet_dtqcb_table, dtqid, data);
	}

/*
 *  データキューからの受信
 */

ER
ethernet_rcv_dtq (ID dtqid, intptr_t *p_data)
{
	return rcv_dtq((uint_t)TNUM_ETHERNET_DATAQUEUE, ethernet_dtqcb_table, dtqid, p_data);
	}

#endif	/* of #ifdef SUPPORT_ETHER */
