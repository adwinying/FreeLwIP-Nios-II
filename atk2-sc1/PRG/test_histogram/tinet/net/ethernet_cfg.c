/*
 *  Ethernet ����ե�����졼�����ե�����
 */

#include "Os.h"
#include "t_syslog.h"

#include <tinet_atk2_cfg.h>

#include "dataqueue.h"

#ifdef SUPPORT_ETHER

/****** Object DATAQUEUE ******/

/*
 *  �ǡ������塼�����ΰ�
 */

static DTQMB DTQMB_DTQ_ETHER_OUTPUT[NUM_DTQ_ETHER_OUTPUT];

/*
 *  �ǡ������塼������֥�å�
 */

const DTQINIB ethernet_dtqinib_table[TNUM_ETHERNET_DATAQUEUE] = {
	{
		NUM_DTQ_ETHER_OUTPUT,
		DTQMB_DTQ_ETHER_OUTPUT
		}
	};

/*
 *  �ǡ������塼�����֥�å�
 */

DTQCB ethernet_dtqcb_table[TNUM_ETHERNET_DATAQUEUE];

/*
 *  ���֥������Ȥν����
 */

void
ethernet_initialize (void)
{
	dataqueue_initialize((uint_t)TNUM_ETHERNET_DATAQUEUE, ethernet_dtqinib_table, ethernet_dtqcb_table);
	}

/*
 *  �ǡ������塼�ؤ�����
 */

ER
ethernet_snd_dtq (ID dtqid, intptr_t data)
{
	return snd_dtq((uint_t)TNUM_ETHERNET_DATAQUEUE, ethernet_dtqcb_table, dtqid, data);
	}

/*
 *  �ǡ������塼����μ���
 */

ER
ethernet_rcv_dtq (ID dtqid, intptr_t *p_data)
{
	return rcv_dtq((uint_t)TNUM_ETHERNET_DATAQUEUE, ethernet_dtqcb_table, dtqid, p_data);
	}

#endif	/* of #ifdef SUPPORT_ETHER */
