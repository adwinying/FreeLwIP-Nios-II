#ifdef TARGET_KERNEL_ATK2

#include "Os.h"
#include "prc_sil.h"

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

#include <tinet_defs.h>
#include <tinet_config.h>
#include <tinet_atk2_cfg.h>

#include <net/if.h>
//#include <net/if_ppp.h>
//#include <net/if_loop.h>
#include <net/ethernet.h>
#include <net/net.h>
#include <net/net_var.h>
#include <net/net_buf.h>
#include <net/net_timer.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
//#include <netinet6/in6.h>
//#include <netinet6/in6_var.h>
#include <netinet/ip.h>
//#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/tcp_fsm.h>
#include <netinet/tcp_seq.h>

#ifdef SUPPORT_TCP

/*
 *  �ɽ�ؿ�
 */

static T_TCP_CEP *tcp_timers (T_TCP_CEP *cep, int_t tix);

#if defined(NUM_TCP_TW_CEP_ENTRY) && NUM_TCP_TW_CEP_ENTRY > 0

static void tcp_tw_timo (void);

#endif	/* of #if defined(NUM_TCP_TW_CEP_ENTRY) && NUM_TCP_TW_CEP_ENTRY > 0 */

/*
 *  �Хå����ջ���
 *
 *  ������Ԥ����Ӥˡ������ॢ���Ȥλ��֤��Ĺ���롣
 */

const static uint8_t tcp_back_off[] = {
	UINT_C(1), 	UINT_C(2), 	UINT_C(4), 	UINT_C(8),
	UINT_C(16), 	UINT_C(32), 	UINT_C(64), 	UINT_C(64),
	UINT_C(64), 	UINT_C(64), 	UINT_C(64), 	UINT_C(64),
	UINT_C(64)
	};

#define TCP_TOTAL_BACK_OFF	511	/* �Хå����ջ��֤ι�� */

/*
 *  tcp_timers -- �����ॢ���Ƚ���
 */

static T_TCP_CEP *
tcp_timers (T_TCP_CEP *cep, int_t tix)
{
	uint16_t	win;


	switch (tix) {

	/*
	 *  ����������
	 */
	case TCP_TIM_REXMT:

		/*
		 *  ���������� (TCP_MAX_REXMT_SHIFT��ɸ�� 12 ��) �ˤʤä��Ȥ��ϡ�
		 *  ���ͥ����������Ǥ��롣
		 */
		if (++ cep->rxtshift > TCP_MAX_REXMT_SHIFT) {
			cep->rxtshift  = TCP_MAX_REXMT_SHIFT;
			cep->net_error = EV_REXMTMO;
			cep = tcp_drop(cep, E_CLS);
			break;
			}

		/*
		 *  ���������ॢ���Ȥ�׻����롣
		 */
		cep->rxtcur = tcp_range_set((T_TCP_TIME)(tcp_rexmt_val(cep) * tcp_back_off[cep->rxtshift]),
		                            (T_TCP_TIME)TCP_TVAL_MIN,
		                            (T_TCP_TIME)TCP_TVAL_MAX_REXMT);
		cep->timer[TCP_TIM_REXMT] = cep->rxtcur;

		/*
		 *  srtt:   ʿ�경���줿 RTT
		 *  rttvar: ʿ�경���줿ʬ��
		 *
		 *  ��������������������� 1/4 �ˤʤä��Ȥ��ϡ�
		 *  ʿ�경���줿ʬ�� (rttvar) �� srtt ��û�����
		 *  ʿ�경���줿 RTT �� 0 �ˤ��롣
		 *
		 */
		if (cep->rxtshift > TCP_MAX_REXMT_SHIFT / 4) {
			cep->rttvar += (cep->srtt >> TCP_SRTT_SHIFT);
			cep->srtt    = 0;
			}

		/*
		 *  snd_nxt: ������������ SEQ�����λ����Ǥϡ������������� SEQ
		 *  snd_una: ̤��ǧ�κǾ����� SEQ	 �ޤ��ϡ���ǧ���줿�������� SEQ
		 *
		 *  ������������ SEQ (snd_nxt) ��
		 *  ��ǧ���줿�������� SEQ (snd_una) �ޤ��᤹��
		 */
		cep->snd_nxt = cep->snd_una;
		cep->flags  |= TCP_CEP_FLG_ACK_NOW;

		/*
		 *  rtt: �������֤η�¬����ߤ��롣
		 */
		cep->rtt     = 0;

		/*
		 *  ����������ɤ�����
		 *
		 *  snd_wnd:  ���μ�����ǽ������ɥ�����
		 *  snd_cwnd: ���ԥ�����ɥ�����
		 *  maxseg  : ���κ�������������ȥ�����
		 *
		 *  ���μ�����ǽ������ɥ����� (snd_wnd) ����
		 *  ���ԥ�����ɥ����� (snd_cwnd) ��
		 *  �ɤ��餫�������������� 1/2 �򡢹���
		 *  ���κ�������������ȥ����� (maxseg) �ǳ�ä��͡�
		 *  ��������2 �ʾ�
		 */
		if (cep->snd_wnd < cep->snd_cwnd)
			win = cep->snd_wnd / 2 / cep->maxseg;
		else
			win = cep->snd_cwnd / 2 / cep->maxseg;

		if (win < 2)
			win = 2;

		/*
		 *  ���ԥ�����ɥ����� (snd_cwnd) ��
		 *  ���μ�����ǽ������ɥ����� (snd_wnd) �ˡ�
		 *  ���ԥ�����ɥ������Τ������� (snd_ssthresh) ��
		 *  ���μ�����ǽ������ɥ����� (snd_wnd) �� win �ܤ�
		 *  ���ꤹ�롣
		 */
		cep->snd_cwnd     = cep->maxseg;
		cep->snd_ssthresh = win * cep->maxseg;
		cep->dupacks      = 0;

		/* ���Ϥ�ݥ��Ȥ��롣*/
		cep->flags |= TCP_CEP_FLG_POST_OUTPUT;
		SetEvent(TCP_OUTPUT_TASK, EVT_TCP_POST_OUTPUT);
		break;

	/*
	 *  ��³������
	 */
	case TCP_TIM_PERSIST:

		/*
		 *  ���������� (TCP_MAX_REXMT_SHIFT��ɸ�� 12 ��) ��Ķ���Ƥ��ơ�
		 *  �����ɥ���֤�����α�����ޤ�ɸ���� (TCP_TVAL_KEEP_IDLE��
		 *  ɸ�� 2 * 60 * 60 ��) �ʾ夫��
		 *  ���������ॢ������ * �Хå����ջ��֤ι�װʾ�ʤ�
		 *  ���ͥ����������Ǥ��롣
		 */
		if (cep->rxtshift > TCP_MAX_REXMT_SHIFT &&
		    (cep->idle >= TCP_TVAL_KEEP_IDLE ||
		     cep->idle >= tcp_rexmt_val(cep) * TCP_TOTAL_BACK_OFF)) {
			cep->net_error = EV_REXMTMO;
			cep = tcp_drop(cep, E_CLS);
			break;
			}

		/* ��³�����ޤ�����ꤷ�����Ϥ�ݥ��Ȥ��롣*/
		tcp_set_persist_timer(cep);

		cep->flags |= TCP_CEP_FLG_FORCE | TCP_CEP_FLG_FORCE_CLEAR | TCP_CEP_FLG_POST_OUTPUT;
		SetEvent(TCP_OUTPUT_TASK, EVT_TCP_POST_OUTPUT);
		break;

	/*
	 *  ��α (keep alive) ������
	 */
	case TCP_TIM_KEEP:

		/*
		 *  ���ͥ�����󤬳��ߤ����ޤǤ˥����ॢ���Ȥ�����
		 *  ���ͥ������γ��ߤ���ߤ��롣
		 */
		if (cep->fsm_state < TCP_FSM_ESTABLISHED) {
			cep->net_error = EV_REXMTMO;
			cep = tcp_drop(cep, E_CLS);
			break;
			}

#ifdef TCP_CFG_ALWAYS_KEEP

		else if (cep->fsm_state < TCP_FSM_CLOSING) {
			if (cep->idle >= TCP_TVAL_KEEP_IDLE +
			                 TCP_TVAL_KEEP_COUNT * TCP_TVAL_KEEP_INTERVAL) {
				cep->net_error = EV_REXMTMO;
				cep = tcp_drop(cep, E_CLS);
				break;
				}
			else
				tcp_respond(NULL, cep, cep->rcv_nxt, cep->snd_una - 1,
				                       cep->rbufsz - cep->rwbuf_count, 0);
			cep->timer[TCP_TIM_KEEP] = TCP_TVAL_KEEP_INTERVAL;
			}
		else
			cep->timer[TCP_TIM_KEEP] = TCP_TVAL_KEEP_IDLE;

#else	/* of #ifdef TCP_CFG_ALWAYS_KEEP */

		cep->timer[TCP_TIM_KEEP] = TCP_TVAL_KEEP_IDLE;

#endif	/* of #ifdef TCP_CFG_ALWAYS_KEEP */

		break;

	/*
	 *  2MSL ������
	 */
	case TCP_TIM_2MSL:

		if (cep->fsm_state != TCP_FSM_TIME_WAIT &&
		    cep->idle  <= TCP_TVAL_KEEP_COUNT * TCP_TVAL_KEEP_INTERVAL)
			cep->timer[TCP_TIM_2MSL] = TCP_TVAL_KEEP_INTERVAL;
		else
			cep = tcp_close(cep);
		break;
		}

	return cep;
	}

/*
 *  tcp_cancel_timers -- ���ƤΥ����ޤ򥭥�󥻥뤹�롣
 */

void
tcp_cancel_timers (T_TCP_CEP *cep)
{
	int_t ix;

	for (ix = NUM_TCP_TIMERS; ix -- > 0; )
		cep->timer[ix] = 0;
	}

#if defined(NUM_TCP_TW_CEP_ENTRY) && NUM_TCP_TW_CEP_ENTRY > 0

/*
 *  tcp_tw_timo -- Time Wait �� TCP �̿�ü���Υ����ॢ���Ƚ���
 */

void
tcp_tw_timo (void)
{
	T_TCP_CEP	*cep, *lcep;
	T_TCP_TWCEP*	twcep;
	T_TCP_TIME	longer;
	int_t		frees, waits;

	/*
	 *  Time Wait �� TCP �̿�ü���ǥ����ॢ���ȤΥ���ȥ��õ������
	 *  õ����������ȥ������ˤ��롣
	 */
	frees = 0;
	for (twcep = &tcp_twcep[NUM_TCP_TW_CEP_ENTRY]; twcep -- != tcp_twcep; )
		if (twcep->fsm_state == TCP_FSM_TIME_WAIT && -- twcep->timer_2msl == 0) {
			twcep->fsm_state = TCP_FSM_CLOSED;
			frees ++;
			}

	/*
	 *  Time Wait �� TCP �̿�ü���ζ�������ȥ꤬ȯ��������
	 *  ɸ�� TCP �̿�ü���� Time Wait ��� TCP �̿�ü����õ������
	 *  ɬ�פʾ���� Time Wait �� TCP �̿�ü���˰ܤ��ơ�
	 *  ɸ��� TCP �̿�ü���������롣
	 */
	if (frees > 0) {
		waits = 0;
		for (cep = &tcp_cep[tmax_tcp_cepid]; cep -- != tcp_cep; )
			if (cep->fsm_state == TCP_FSM_TIME_WAIT)
				waits ++;
		if (waits > frees)
			waits = frees;

		/*
		 *  ��äȤ�Ĺ���� Time Wait ���Ƥ��� TCP �̿�ü���������롣
		 */
		while (waits > 0) {
			longer = TCP_TVAL_MSL * 2;
			lcep   = NULL;
			for (cep = &tcp_cep[tmax_tcp_cepid]; cep -- != tcp_cep; )
				if (cep->fsm_state == TCP_FSM_TIME_WAIT && cep->timer[TCP_TIM_2MSL] < longer) {
					longer = cep->timer[TCP_TIM_2MSL];
					lcep   = cep;
					}
			if (lcep != NULL) {
				tcp_move_twcep(lcep);
				waits --;
				}
			else
				break;
			}
		}
	}

#endif	/* of #if defined(NUM_TCP_TW_CEP_ENTRY) && NUM_TCP_TW_CEP_ENTRY > 0 */

/*
 *  tcp_slow_timo -- 500 [ms] ��˸ƽФ���� TCP �����ॢ���ȴؿ�
 */

void
tcp_slow_timo (void *ignore)
{
	T_TCP_CEP	*cep;
	int_t		cix, tix;

	for (cix = tmax_tcp_cepid; cix -- > 0; ) {
		cep = &tcp_cep[cix];
		if (!(cep->fsm_state == TCP_FSM_CLOSED || cep->fsm_state == TCP_FSM_LISTEN)) {
			for (tix = NUM_TCP_TIMERS; cep != NULL && tix -- > 0; ) {
				if (cep->timer[tix] != 0 && -- cep->timer[tix] == 0) {
					cep = tcp_timers(cep, tix);
					}
				}
			if (cep != NULL) {
				cep->idle ++;
				if (cep->rtt) {
					cep->rtt ++;
					}
				}
			}
		}

#if defined(NUM_TCP_TW_CEP_ENTRY) && NUM_TCP_TW_CEP_ENTRY > 0

	tcp_tw_timo();

#endif	/* of #if defined(NUM_TCP_TW_CEP_ENTRY) && NUM_TCP_TW_CEP_ENTRY > 0 */

	if (tcp_iss != 0) {
		tcp_iss += TCP_ISS_INCR() / TCP_SLOW_HZ;
		}

	timeout(tcp_slow_timo, NULL, TCP_SLOW_TMO);
	}

#ifdef TCP_CFG_DELAY_ACK

/*
 *  tcp_fast_timo -- 200 [ms] ��˸ƽФ���� TCP �����ॢ���ȴؿ�
 */

void
tcp_fast_timo (void *ignore)
{
	T_TCP_CEP	*cep;
	int_t		cix;

	for (cix = tmax_tcp_cepid; cix -- > 0; ) {
		cep = &tcp_cep[cix];
		if (cep->flags & TCP_CEP_FLG_DEL_ACK) {
			cep->flags &= ~TCP_CEP_FLG_DEL_ACK;
			cep->flags |=  TCP_CEP_FLG_ACK_NOW | TCP_CEP_FLG_POST_OUTPUT;
			SetEvent(TCP_OUTPUT_TASK, EVT_TCP_POST_OUTPUT);
			}
		}

	timeout(tcp_fast_timo, NULL, TCP_FAST_TMO);
	}

#endif/* of #ifdef TCP_CFG_DELAY_ACK */

/*
 *  tcp_range_set -- ���֤���ꤷ���ϰϤ����ꤹ�롣
 */

T_TCP_TIME
tcp_range_set (T_TCP_TIME value, T_TCP_TIME tvmin, T_TCP_TIME tvmax)
{
	if ((uint32_t)value < (uint32_t)tvmin)
		return tvmin;
	else if ((uint32_t)value > (uint32_t)tvmax)
		return tvmax;
	else
		return value;
	}

/*
 *  tcp_set_persist_timer -- ��³�����ޤ�����
 */

void
tcp_set_persist_timer (T_TCP_CEP *cep)
{
	T_TCP_TIME time;

	/*
	 *  srtt:   ʿ�경���줿 RTT
	 *  rttvar: ʿ�경���줿ʬ��
	 */
	time = ((cep->srtt >> 2) + cep->rttvar) >> 1;

	/*
	 *  ���������ޤ����ꤵ��Ƥ���в�����ǽ���顼
	 */
	if (cep->timer[TCP_TIM_REXMT])
		/* %%% panic("tcp_output REXMT"); %%% */;

	/*
	 *  ��³�����ޤ����ꤹ�롣
	 */
	cep->timer[TCP_TIM_PERSIST] = tcp_range_set((T_TCP_TIME)(time * tcp_back_off[cep->rxtshift]),
	                                            (T_TCP_TIME)TCP_TVAL_MIN_PERSIST,
	                                            (T_TCP_TIME)TCP_TVAL_MAX_PERSIST);

	/*
	 *  ��������򹹿����롣
	 */
	if (cep->rxtshift < TCP_MAX_REXMT_SHIFT)
		cep->rxtshift ++;
	}

#endif	/* of #ifdef SUPPORT_TCP */
