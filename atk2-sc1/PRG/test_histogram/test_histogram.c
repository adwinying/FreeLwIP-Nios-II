/*
 *		UDP ECHO ������
 */

#include "Os.h"
#include "t_syslog.h"
#include "t_stdlib.h"
#include "sysmod/serial.h"
#include "sysmod/syslog.h"

#include "sysmod/banner.h"
#include "target_sysmod.h"
#include "target_serial.h"
#include "target_hw_counter.h"

#include <netinet/in.h>
#include <netinet/in_itron.h>
#include <tinet_atk2_cfg.h>

#include "test_histogram.h"

#include "target_test.h"
#include "histogram.h"


/*
 *  �ѿ�
 */

#define BUF_SIZE	2048

static uint32	histarea[1001] = { 0 };
ObjectIDType	histid = 1;
boolean			meas_flg = FALSE;

/*
 *  API���顼�����ޥ���
 *
 *  ErrorHook��ͭ���ξ���ErrorHook����
 *  ���顼��������Ϥ�, ErrorHook��̵���ξ���
 *  �ʲ��Υޥ�����ꥨ�顼�������Ϥ�Ԥ�
 */
#if defined(CFG_USE_ERRORHOOK)
#define error_log(api)	(api)
#else /* !defined( CFG_USE_ERRORHOOK ) */
#define	error_log(api)										   \
	{														   \
		StatusType ercd;									   \
		ercd = api;     /* ��API�¹� */						   \
		if (ercd != E_OK) {									   \
			syslog(LOG_INFO, "Error:%d", atk2_strerror(ercd)); \
		}													   \
	}
#endif /* defined( CFG_USE_ERRORHOOK ) */

/*
 *  �桼���ᥤ��ؿ�
 *
 *  �����ͥ뵯ư
 */
sint32
main(void)
{
	/*
	 *  �����ͥ뵯ư
	 */
	StartOS(AppMode);

	while (1) {
	}
}   /* main */

/*
 *  UDP ECHO �����Х�����
 */

TASK(USRV_TASK)
{
	static uint8_t buffer[BUF_SIZE];

	T_IPV4EP	dst;
	T_IN4_ADDR	addr;
	ER_UINT		len;
	TaskType	tskid;
	boolean		prt_flg = FALSE;

	GetTaskID(&tskid);
	addr = IPV4_ADDR_LOCAL;
	syslog(LOG_INFO, "[UDP ECHO SRV:%d,%d] started, IP Address: %s.",
	                 tskid, USRV_CEPID, ip2str(NULL, &addr));
	while (true) {
		while ((len = udp_rcv_dat(USRV_CEPID, &dst, buffer, sizeof(buffer))) == E_TMOUT) {
			if (histid <= TNUM_HIST) {
				begin_measure(histid);
				meas_flg = TRUE;
			}

			/* 100us �������� */
			SetRelAlarm(ALM_USRV, (TickType)(TIMER_CLOCK_HZ / 10000), (TickType)0);
			WaitEvent(EVT_USRV);
			ClearEvent(EVT_USRV);

			if (histid <= TNUM_HIST && meas_flg == TRUE) {
				end_measure(histid++);
				meas_flg = FALSE;
			}
		}
		if (len >= 0) {
			/* �¹Ի��֤η�¬��λ */
//			if (histid <= TNUM_HIST && meas_flg == TRUE) {
//				end_measure(histid++);
//				meas_flg = FALSE;
//			}
#if 0
			buffer[len] = '\0';
			syslog(LOG_INFO, "[UDP ECHO SRV] recv, addr: %s:%d, len: %d, msg: '%s'",
			                 ip2str(NULL, &dst.ipaddr), dst.portno, len, buffer);
			if ((int_t)len > 0) {
				while ((len = udp_snd_dat(USRV_CEPID, &dst, buffer, len)) == E_TMOUT) {
					/* 0.1s �������� */
					SetRelAlarm(ALM_USRV, (TickType)(TIMER_CLOCK_HZ / 10), (TickType)0);
					WaitEvent(EVT_USRV);
					ClearEvent(EVT_USRV);
				}
				if (len < 0)
					syslog(LOG_INFO, "[UDP ECHO SRV] send, error: %s", atk2_strerror(len));
			}
#endif
		}
		else {
//			syslog(LOG_INFO, "[UDP ECHO SRV] recv, error: %s", atk2_strerror(len));
			udp_can_cep(USRV_CEPID, TFN_UDP_ALL);
		}

		/* ��¬��̤ν��� */
		if (histid > TNUM_HIST && prt_flg == FALSE) {
			prt_flg = TRUE;
			print_hist(TNUM_HIST);
		}
	}

	/*
	 *  �����ˤϤ��ʤ�
	 */
	syslog(LOG_INFO, "[UDP ECHO SRV:%d,%d] terminated.", tskid, USRV_CEPID);
	error_log(TerminateTask());
}   /* TASK( USRV_TASK ) */


/*
 *  ���顼�եå��롼����
 */
#ifdef CFG_USE_ERRORHOOK
void
ErrorHook(StatusType Error)
{
	/*
	 *  ���顼�װ����ȤΥѥ�᡼����������
	 */
	switch (OSErrorGetServiceId()) {
	case OSServiceId_ActivateTask:
		syslog(LOG_INFO, "Error:%s=ActivateTask(%d)", atk2_strerror(Error), OSError_ActivateTask_TaskID());
		break;
	case OSServiceId_TerminateTask:
		syslog(LOG_INFO, "Error:%s=TerminateTask()", atk2_strerror(Error));
		break;
	case OSServiceId_ChainTask:
		syslog(LOG_INFO, "Error:%s=ChainTask(%d)", atk2_strerror(Error), OSError_ChainTask_TaskID());
		break;
	case OSServiceId_Schedule:
		syslog(LOG_INFO, "Error:%s=Schedule()", atk2_strerror(Error));
		break;
	case OSServiceId_GetTaskID:
		syslog(LOG_INFO, "Error:%s=GetTaskID(0x%p)", atk2_strerror(Error), OSError_GetTaskID_TaskID());
		break;
	case OSServiceId_GetTaskState:
		syslog(LOG_INFO, "Error:%s=GetTaskState(%d, 0x%p)", atk2_strerror(Error),
			   OSError_GetTaskState_TaskID(), OSError_GetTaskState_State());
		break;
	case OSServiceId_EnableAllInterrupts:
		syslog(LOG_INFO, "Error:%s=EnableAllInterrupts()", atk2_strerror(Error));
		break;
	case OSServiceId_DisableAllInterrupts:
		syslog(LOG_INFO, "Error:%s=DisableAllInterrupts()", atk2_strerror(Error));
		break;
	case OSServiceId_ResumeAllInterrupts:
		syslog(LOG_INFO, "Error:%s=ResumeAllInterrupts()", atk2_strerror(Error));
		break;
	case OSServiceId_SuspendAllInterrupts:
		syslog(LOG_INFO, "Error:%s=SuspendAllInterrupts()", atk2_strerror(Error));
		break;
	case OSServiceId_ResumeOSInterrupts:
		syslog(LOG_INFO, "Error:%s=ResumeOSInterrupts()", atk2_strerror(Error));
		break;
	case OSServiceId_SuspendOSInterrupts:
		syslog(LOG_INFO, "Error:%s=SuspendOSInterrupts()", atk2_strerror(Error));
		break;
	case OSServiceId_GetISRID:
		syslog(LOG_INFO, "Error:%s=GetISRID()", atk2_strerror(Error));
		break;
	case OSServiceId_GetResource:
		syslog(LOG_INFO, "Error:%s=GetResource(%d)", atk2_strerror(Error), OSError_GetResource_ResID());
		break;
	case OSServiceId_ReleaseResource:
		syslog(LOG_INFO, "Error:%s=ReleaseResource(%d)", atk2_strerror(Error), OSError_ReleaseResource_ResID());
		break;
	case OSServiceId_SetEvent:
		syslog(LOG_INFO, "Error:%s=SetEvent(%d, 0x%x)", atk2_strerror(Error),
			   OSError_SetEvent_TaskID(), OSError_SetEvent_Mask());
		break;
	case OSServiceId_ClearEvent:
		syslog(LOG_INFO, "Error:%s=ClearEvent(0x%x)", atk2_strerror(Error), OSError_ClearEvent_Mask());
		break;
	case OSServiceId_GetEvent:
		syslog(LOG_INFO, "Error:%s=GetEvent(%d, 0x%p)", atk2_strerror(Error),
			   OSError_GetEvent_TaskID(), OSError_GetEvent_Event());
		break;
	case OSServiceId_WaitEvent:
		syslog(LOG_INFO, "Error:%s=WaitEvent(0x%x)", atk2_strerror(Error), OSError_WaitEvent_Mask());
		break;
	case OSServiceId_GetAlarmBase:
		syslog(LOG_INFO, "Error:%s=GetAlarmBase(0x%p)", atk2_strerror(Error), OSError_GetAlarmBase_AlarmID());
		break;
	case OSServiceId_GetAlarm:
		syslog(LOG_INFO, "Error:%s=GetAlarm(%d, 0x%p)", atk2_strerror(Error),
			   OSError_GetAlarm_AlarmID(), OSError_GetAlarm_Tick());
		break;
	case OSServiceId_SetRelAlarm:
		syslog(LOG_INFO, "Error:%s=SetRelAlarm(%d, %d, %d)", atk2_strerror(Error),
			   OSError_SetRelAlarm_AlarmID(), OSError_SetRelAlarm_increment(), OSError_SetRelAlarm_cycle());
		break;
	case OSServiceId_SetAbsAlarm:
		syslog(LOG_INFO, "Error:%s=SetAbsAlarm(%d, %d, %d)", atk2_strerror(Error),
			   OSError_SetAbsAlarm_AlarmID(), OSError_SetAbsAlarm_start(), OSError_SetAbsAlarm_cycle());
		break;
	case OSServiceId_CancelAlarm:
		syslog(LOG_INFO, "Error:%s=CancelAlarm(%d)", atk2_strerror(Error), OSError_CancelAlarm_AlarmID());
		break;
	case OSServiceId_StartScheduleTableRel:
		syslog(LOG_INFO, "Error:%s=StartScheduleTableRel(%d, %d)", atk2_strerror(Error),
			   OSError_StartScheduleTableRel_ScheduleTableID(), OSError_StartScheduleTableRel_Offset());
		break;
	case OSServiceId_StartScheduleTableAbs:
		syslog(LOG_INFO, "Error:%s=StartScheduleTableAbs(%d, %d)", atk2_strerror(Error),
			   OSError_StartScheduleTableAbs_ScheduleTableID(), OSError_StartScheduleTableAbs_Start());
		break;
	case OSServiceId_StopScheduleTable:
		syslog(LOG_INFO, "Error:%s=StopScheduleTable(%d)", atk2_strerror(Error), OSError_StopScheduleTable_ScheduleTableID());
		break;
	case OSServiceId_NextScheduleTable:
		syslog(LOG_INFO, "Error:%s=NextScheduleTable(%d, %d)", atk2_strerror(Error),
			   OSError_NextScheduleTable_ScheduleTableID_From(), OSError_NextScheduleTable_ScheduleTableID_To());
		break;
	case OSServiceId_GetScheduleTableStatus:
		syslog(LOG_INFO, "Error:%s=GetScheduleTableStatus(%d, 0x%p)", atk2_strerror(Error),
			   OSError_GetScheduleTableStatus_ScheduleTableID(), OSError_GetScheduleTableStatus_ScheduleStatus());
		break;
	case OSServiceId_GetActiveApplicationMode:
		syslog(LOG_INFO, "Error:%s=GetActiveApplicationMode()", atk2_strerror(Error));
		break;
	case OSServiceId_StartOS:
		syslog(LOG_INFO, "Error:%s=StartOS()", atk2_strerror(Error));
		break;
	case OSServiceId_ShutdownOS:
		syslog(LOG_INFO, "Error:%s=ShutdownOS()", atk2_strerror(Error));
		break;
	case OSServiceId_IncrementCounter:
		syslog(LOG_INFO, "Error:%s=IncrementCounter(%d)", atk2_strerror(Error), OSError_IncrementCounter_CounterID());
		break;
	case OSServiceId_TaskMissingEnd:
		syslog(LOG_INFO, "Error:%s=MissingEnd()", atk2_strerror(Error));
		break;
	default:
		syslog(LOG_INFO, "Error:%s=UnKnownFunc()", atk2_strerror(Error));
		break;
	}

}   /* ErrorHook */
#endif /* CFG_USE_ERRORHOOK */

/*
 *  �ץ쥿�����եå��롼����
 *
 *  ���롼�����ƽФ�
 */
#ifdef CFG_USE_PRETASKHOOK
void
PreTaskHook(void)
{
}   /* PreTaskHook */
#endif /* CFG_USE_PRETASKHOOK */

/*
 *  �ݥ��ȥ������եå��롼����
 *
 *  ���롼�����ƽФ�
 */
#ifdef CFG_USE_POSTTASKHOOK
void
PostTaskHook(void)
{
}   /* PostTaskHook */
#endif /* CFG_USE_POSTTASKHOOK */

/*
 *  �������ȥ��åץեå��롼����
 */
#ifdef CFG_USE_STARTUPHOOK
#ifdef TOPPERS_ENABLE_SYS_TIMER
extern void target_timer_initialize(void);
#endif /* TOPPERS_ENABLE_SYS_TIMER */

void
StartupHook(void)
{
	ObjectIDType i;

	for (i = 0; i < TNUM_HIST; i ++)
		init_hist(i + 1, 1000, histarea);

	tinet_initialize();
	net_atk2_initialize();
	ethernet_initialize();

#ifdef TOPPERS_ENABLE_SYS_TIMER
	target_timer_initialize();
#endif /* TOPPERS_ENABLE_SYS_TIMER */
	syslog_initialize();
	syslog_msk_log(LOG_UPTO(LOG_INFO));
	InitSerial();
	print_banner();
}   /* StartupHook */
#endif /* CFG_USE_STARTUPHOOK */

/*
 *  ����åȥ�����եå��롼����
 */
#ifdef CFG_USE_SHUTDOWNHOOK
#ifdef TOPPERS_ENABLE_SYS_TIMER
extern void target_timer_terminate(void);
#endif /* TOPPERS_ENABLE_SYS_TIMER */

void
ShutdownHook(StatusType Error)
{
	/* ��λ�������� */
	syslog(LOG_INFO, "");
	syslog(LOG_INFO, "Sample System ShutDown");
	syslog(LOG_INFO, "ShutDownCode:%s", atk2_strerror(Error));
	syslog(LOG_INFO, "");

	if (Error == E_OS_SYS_ASSERT_FATAL) {
		syslog(LOG_INFO, "fatal_file_name:%s", fatal_file_name);
		syslog(LOG_INFO, "fatal_line_num:%d", fatal_line_num);
	}

#ifdef TOPPERS_ENABLE_SYS_TIMER
	target_timer_terminate();
#endif /* TOPPERS_ENABLE_SYS_TIMER */
	TermSerial();

}   /* ShutdownHook */
#endif /* CFG_USE_SHUTDOWNHOOK */

/*
 *  �ץ��ƥ������եå��롼����
 */
#ifdef CFG_USE_PROTECTIONHOOK
ProtectionReturnType
ProtectionHook(StatusType FatalError)
{
	StatusType ercd;

	syslog(LOG_INFO, "");
	syslog(LOG_INFO, "ProtectionHook");

	if (FatalError == E_OS_STACKFAULT) {
		syslog(LOG_INFO, "E_OS_STACKFAULT");
		ercd = PRO_SHUTDOWN;
	}
	else if (FatalError == E_OS_PROTECTION_EXCEPTION) {
		syslog(LOG_INFO, "E_OS_PROTECTION_EXCEPTION");
		ercd = PRO_IGNORE;
	}
	else {
		ercd = PRO_SHUTDOWN;
	}

	return(ercd);
}
#endif /* CFG_USE_PROTECTIONHOOK */