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
#include <net/net_timer.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
//#include <netinet/ip6.h>
//#include <netinet6/ip6_var.h>
#include <netinet/tcp_timer.h>

/*
 *  �ѿ�
 */

static T_NET_CALLOUT callout[NUM_NET_CALLOUT];

/*
 *  �ƽФ�����ȥ���Ͽ
 */

void
timeout (callout_func func, void *arg, RELTIM timeout)
{
	int_t ix;

	/* �ƽФ�����ȥ��������� */
	syscall(GetResource(RES_CALL_OUT_LOCK));
	for (ix = NUM_NET_CALLOUT; callout[-- ix].func != NULL; )
		if (ix == 0) {
			syslog(LOG_WARNING, "[NET] callout busy, size=%d.", NUM_NET_CALLOUT);
			syscall(ReleaseResource(RES_CALL_OUT_LOCK));
			return;
			}

	/* �ƽФ�����ȥ�����ꤹ�� */
	callout[ix].func	= func;
	callout[ix].arg		= arg;
	callout[ix].timeout	= timeout;
	syscall(ReleaseResource(RES_CALL_OUT_LOCK));
	}

/*
 *  �ƽФ�����ȥ���
 */

void
untimeout (callout_func func, void *arg)
{
	int_t ix;

	/* ����ȥ��õ�� */
	for (ix = NUM_NET_CALLOUT; ix -- > 0; ) {
		syscall(GetResource(RES_CALL_OUT_LOCK));
		if (callout[ix].func == func && callout[ix].arg == arg && callout[ix].timeout > 0) {
			callout[ix].func = NULL;
			syscall(ReleaseResource(RES_CALL_OUT_LOCK));
			break;
			}
		syscall(ReleaseResource(RES_CALL_OUT_LOCK));
		}
	}

/*
 *  �ͥåȥ�������ޥ�����
 *
 *    ���: ����������ƥ����ȤǼ¹Ԥ���ɬ�פ����뤿�ᡤ
 *          �����ϥ�ɥ�ϻ��ѤǤ��ʤ���
 */

static char const tinet_banner[] =
"\n"
"TINET %d.%d.%d (" __DATE__ ", " __TIME__ ")\n"
"Copyright (C) 2001-2009 by Dep. of Computer Science and Engineering\n"
"                    Tomakomai National College of Technology, JAPAN\n";

TASK(NET_TIMER_TASK)
{
	TaskType	tskid;
	int_t		ix;
	callout_func	func;
	void		*arg;

	GetTaskID(&tskid);
	syslog(LOG_NOTICE, tinet_banner,
	       (TINET_PRVER   >> 12) & UINT_C(0x0f),
	       (TINET_PRVER   >>  4) & UINT_C(0x0f),
	        TINET_PRVER          & UINT_C(0x0f));
	syslog(LOG_NOTICE, "[NET/TIMER:%d] started.", tskid);

	/* IP ���������롣*/
	IP_INIT();

#ifdef SUPPORT_TCP

	/* TCP ���ϥ�������ư���� */
	syscall(ActivateTask(TCP_OUTPUT_TASK));

#endif	/* of #ifdef SUPPORT_TCP */

#if defined(SUPPORT_UDP) && defined(UDP_CFG_NON_BLOCKING)

	/* UDP ���ϥ�������ư���� */
	syscall(ActivateTask(UDP_OUTPUT_TASK));

#endif	/* of #if defined(SUPPORT_UDP) && defined(UDP_CFG_NON_BLOCKING) */

	while (true) {
		/*
		 *  �����ॢ���Ȥ�������ȥ��ƽФ���
		 *
		 *    NET_TIMER_CYCLE (100ms) ��˸ƽФ���롣
		 */
		syscall(WaitEvent(EVT_CALL_OUT_TIMEOUT));
		syscall(ClearEvent(EVT_CALL_OUT_TIMEOUT));
		for (ix = NUM_NET_CALLOUT; ix -- > 0; ) {
			syscall(GetResource(RES_CALL_OUT_LOCK));
			if (callout[ix].func != NULL && callout[ix].timeout > 0) {
				callout[ix].timeout --;
				if (callout[ix].timeout == 0) {
					func = callout[ix].func;
					arg  = callout[ix].arg;
					callout[ix].func = NULL;
					syscall(ReleaseResource(RES_CALL_OUT_LOCK));
					(func)(arg);
					}
				else
					syscall(ReleaseResource(RES_CALL_OUT_LOCK));
				}
			else
				syscall(ReleaseResource(RES_CALL_OUT_LOCK));
			}
		}
	}
