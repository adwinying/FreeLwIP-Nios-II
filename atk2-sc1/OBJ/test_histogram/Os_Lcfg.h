/* Os_Lcfg.h */
#ifndef TOPPERS_OS_LCFG_H
#define TOPPERS_OS_LCFG_H

#define TNUM_ALARM				UINT_C(2)
#define TNUM_COUNTER			UINT_C(2)
#define TNUM_HARDCOUNTER		UINT_C(1)
#define TNUM_ISR2				UINT_C(4)
#define TNUM_STD_RESOURCE		UINT_C(8)
#define TNUM_TASK				UINT_C(4)
#define TNUM_EXTTASK			UINT_C(4)
#define TNUM_APP_MODE			UINT_C(1)
#define TNUM_SCHEDULETABLE		UINT_C(0)
#define TNUM_IMPLSCHEDULETABLE	UINT_C(0)

/*
 *  Default Definitions of Trace Log Macros
 */

#ifndef TOPPERS_ENABLE_TRACE
#ifndef LOG_USER_MARK
#define LOG_USER_MARK(str)
#endif /* LOG_USER_MARK */
#endif /* TOPPERS_ENABLE_TRACE */

/****** Object TASK ******/

#define ETHER_OUTPUT_TASK	UINT_C(0)
#define ETHER_INPUT_TASK	UINT_C(1)
#define NET_TIMER_TASK	UINT_C(2)
#define USRV_TASK	UINT_C(3)

/****** Object COUNTER ******/

#define MAIN_HW_COUNTER	UINT_C(0)
#define SysTimerCnt	UINT_C(1)

#define OSMAXALLOWEDVALUE_MAIN_HW_COUNTER	((TickType) 536870911)
#define OSTICKSPERBASE_MAIN_HW_COUNTER	((TickType) 10)
#define OSMINCYCLE_MAIN_HW_COUNTER	((TickType) 4000)
#define OSMAXALLOWEDVALUE_SysTimerCnt	((TickType) 999)
#define OSTICKSPERBASE_SysTimerCnt	((TickType) 1)
#define OSMINCYCLE_SysTimerCnt	((TickType) 1)

#define OS_TICKS2SEC_MAIN_HW_COUNTER(tick)	(((PhysicalTimeType)1.666666e-08 * 1000000000) * (tick) / 1000000000U)
#define OS_TICKS2MS_MAIN_HW_COUNTER(tick)	(((PhysicalTimeType)1.666666e-08 * 1000000000) * (tick) / 1000000U)
#define OS_TICKS2US_MAIN_HW_COUNTER(tick)	(((PhysicalTimeType)1.666666e-08 * 1000000000) * (tick) / 1000U)
#define OS_TICKS2NS_MAIN_HW_COUNTER(tick)	(((PhysicalTimeType)1.666666e-08 * 1000000000) * (tick))

/****** Object ALARM ******/

#define NET_TIMER_ALARM	UINT_C(0)
#define ALM_USRV	UINT_C(1)

/****** Object SCHEDULETABLE ******/


/****** Object RESOURCE ******/

#define RES_IP4_FRAG_QUEUE	UINT_C(0)
#define RES_IN_ROUTING_TBL	UINT_C(1)
#define RES_MAC2STR_BUFF_LOCK	UINT_C(2)
#define RES_ARP_CACHE_LOCK	UINT_C(3)
#define RES_UDP_CEP	UINT_C(4)
#define RES_UDP_CEP_LOCK1	UINT_C(5)
#define RES_CALL_OUT_LOCK	UINT_C(6)
#define RES_IP2STR_BUFF_LOCK	UINT_C(7)

/****** Object ISR ******/

#define if_tse_tx_handler	UINT_C(0)
#define if_tse_rx_handler	UINT_C(1)
#define C2ISR_for_MAIN_HW_COUNTER	UINT_C(2)
#define target_timer_hdr	UINT_C(3)

/****** Object APPMODE ******/

#define AppMode	UINT_C(0)

/****** Object EVENT ******/
#define EVT_ETHER_OUTPUT	UINT_C(0x00000001)
#define EVT_IF_TSE_SBUF_READY	UINT_C(0x00000002)
#define EVT_IF_TSE_RBUF_READY	UINT_C(0x00000001)
#define EVT_CALL_OUT_TIMEOUT	UINT_C(0x00000001)
#define EVT_USRV	UINT_C(0x00000001)



#ifndef TOPPERS_MACRO_ONLY
#ifdef TOPPERS_ENABLE_TRACE
extern const char8 *atk2_appid_str(AppModeType id);
extern const char8 *atk2_tskid_str(TaskType id);
extern const char8 *atk2_isrid_str(ISRType id);
extern const char8 *atk2_cntid_str(CounterType id);
extern const char8 *atk2_almid_str(AlarmType id);
extern const char8 *atk2_resid_str(ResourceType id);
extern const char8 *atk2_schtblid_str(ScheduleTableType id);
extern const char8 *atk2_evtid_str(TaskType task, EventMaskType event);
#endif /* TOPPERS_ENABLE_TRACE */

/****** Object TASK ******/

extern TASK(ETHER_OUTPUT_TASK);
extern TASK(ETHER_INPUT_TASK);
extern TASK(NET_TIMER_TASK);
extern TASK(USRV_TASK);

/****** Object ALARM ******/


/*
 *  Interrupt Management Functions
 */

extern void _kernel_inthdr_13(void);
extern void _kernel_inthdr_12(void);
extern void _kernel_inthdr_16(void);
extern void _kernel_inthdr_0(void);
extern ISR(if_tse_tx_handler);
extern ISR(if_tse_rx_handler);
extern ISR(C2ISR_for_MAIN_HW_COUNTER);
extern ISR(target_timer_hdr);

extern void init_hwcounter_MAIN_HW_COUNTER(TickType maxval, TimeType nspertick);
extern void start_hwcounter_MAIN_HW_COUNTER(void);
extern void stop_hwcounter_MAIN_HW_COUNTER(void);
extern void set_hwcounter_MAIN_HW_COUNTER(TickType exprtick);
extern TickType get_hwcounter_MAIN_HW_COUNTER(void);
extern void cancel_hwcounter_MAIN_HW_COUNTER(void);
extern void trigger_hwcounter_MAIN_HW_COUNTER(void);
extern void int_clear_hwcounter_MAIN_HW_COUNTER(void);
extern void int_cancel_hwcounter_MAIN_HW_COUNTER(void);
extern void increment_hwcounter_MAIN_HW_COUNTER(void);

extern void int_handler_entry_0(void) __attribute__((naked));
extern void int_handler_entry_12(void) __attribute__((naked));
extern void int_handler_entry_13(void) __attribute__((naked));
extern void int_handler_entry_16(void) __attribute__((naked));

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_OS_LCFG_H */

