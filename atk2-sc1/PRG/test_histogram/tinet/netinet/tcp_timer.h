#ifndef _TCP_TIMER_H_
#define _TCP_TIMER_H_

/* TCP �������ѿ� */

#define NUM_TCP_TIMERS		4	/* TCP �Υ����޿�	*/

#define TCP_TIM_REXMT		0	/* ����������		*/
#define TCP_TIM_PERSIST		1	/* ��³������		*/
#define TCP_TIM_KEEP		2	/* ��α (keep alive) ������ */
#define TCP_TIM_2MSL		3	/* 2MSL ������		*/

/* MSL: Maximum Segment Lifetime, ���祻��������¸���� 		*/

/*
 *  TCP/IP �Υ����ޤ����
 */

#define TCP_SLOW_TMO		(ULONG_C(500)/NET_TIMER_CYCLE)	/* 500 [ms]	*/
#define TCP_FAST_TMO		(ULONG_C(200)/NET_TIMER_CYCLE)	/* 200 [ms]	*/

#define TCP_SLOW_HZ		(ULONG_C(1000)/TCP_SLOW_TMO/NET_TIMER_CYCLE)	/* Hz	*/

/* TCP �������� */

#define TCP_TVAL_MSL		(TCP_SLOW_HZ * 30)
					/* ���祻��������¸����	*/

#define TCP_TVAL_SRTT_BASE	0	/* ��餫�ʱ������֤δ����	*/

#define TCP_TVAL_RTO_BASE	(TCP_SLOW_HZ * 3)
		/* ���������ʤ����κ��������ॢ���Ȥδ����	*/

#define TCP_TVAL_MIN_PERSIST	(TCP_SLOW_HZ * 5)
					/* ��³�����ॢ���ȤκǾ���	*/
#define TCP_TVAL_MAX_PERSIST	(TCP_SLOW_HZ * 60)
					/* ��³�����ॢ���Ȥκ�����	*/

#define TCP_TVAL_KEEP_INIT	(TCP_SLOW_HZ * 75)
					/* ��α�����ޤν����		*/
#define TCP_TVAL_KEEP_IDLE	(TCP_SLOW_HZ * 2 * 60 * 60)
					/* ��α�����޵�ư�ޤǤ�ɸ����	*/
#define TCP_TVAL_KEEP_INTERVAL	(TCP_SLOW_HZ * 75)
					/* ��α�����ޥ��󥿡��Х��ɸ���� */
#define TCP_TVAL_KEEP_COUNT	8	/* ��α�����ॢ���Ȥκ�����	*/

#define TCP_TVAL_MIN		(TCP_SLOW_HZ * 3)
					/* ���֤κǾ�ñ��		*/
#define TCP_TVAL_MAX_REXMT	(TCP_SLOW_HZ * 64)
					/* �����������			*/

/*
 *  �ʲ����ѿ����ͤϥޥ�������ꤹ�롣
 *
 *  tcp_maixidle   = tcp_keepcnt * tcp_keepintvl
 *                 = TCP_TVAL_KEEP_COUNT * TCP_TVAL_KEEP_INTERVAL
 *  tcp_keepcnt    = TCP_TVAL_KEEP_COUNT
 *  tcp_keepintval = TCP_TVAL_KEEP_INTERVAL
 *
 */

/*
 *  ����¾�����
 */

#define TCP_MAX_REXMT_SHIFT	12	/* ����������		*/

/* TCP �������ͤη� */

typedef int16_t T_TCP_TIME;

/*
 *  �ؿ�
 */

extern void tcp_slow_timo (void *ignore);
extern void tcp_fast_timo (void *ignore);

extern T_TCP_TIME tcp_range_set (T_TCP_TIME value, T_TCP_TIME tvmin, T_TCP_TIME tvmax);

#endif	/* of #ifndef _TCP_TIMER_H_ */
