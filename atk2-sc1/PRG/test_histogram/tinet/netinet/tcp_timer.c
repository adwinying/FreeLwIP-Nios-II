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
 *  局所関数
 */

static T_TCP_CEP *tcp_timers (T_TCP_CEP *cep, int_t tix);

#if defined(NUM_TCP_TW_CEP_ENTRY) && NUM_TCP_TW_CEP_ENTRY > 0

static void tcp_tw_timo (void);

#endif	/* of #if defined(NUM_TCP_TW_CEP_ENTRY) && NUM_TCP_TW_CEP_ENTRY > 0 */

/*
 *  バックオフ時間
 *
 *  再送を行うたびに、タイムアウトの時間を延長する。
 */

const static uint8_t tcp_back_off[] = {
	UINT_C(1), 	UINT_C(2), 	UINT_C(4), 	UINT_C(8),
	UINT_C(16), 	UINT_C(32), 	UINT_C(64), 	UINT_C(64),
	UINT_C(64), 	UINT_C(64), 	UINT_C(64), 	UINT_C(64),
	UINT_C(64)
	};

#define TCP_TOTAL_BACK_OFF	511	/* バックオフ時間の合計 */

/*
 *  tcp_timers -- タイムアウト処理
 */

static T_TCP_CEP *
tcp_timers (T_TCP_CEP *cep, int_t tix)
{
	uint16_t	win;


	switch (tix) {

	/*
	 *  再送タイマ
	 */
	case TCP_TIM_REXMT:

		/*
		 *  最大再送回数 (TCP_MAX_REXMT_SHIFT、標準 12 回) になったときは、
		 *  コネクションを切断する。
		 */
		if (++ cep->rxtshift > TCP_MAX_REXMT_SHIFT) {
			cep->rxtshift  = TCP_MAX_REXMT_SHIFT;
			cep->net_error = EV_REXMTMO;
			cep = tcp_drop(cep, E_CLS);
			break;
			}

		/*
		 *  再送タイムアウトを計算する。
		 */
		cep->rxtcur = tcp_range_set((T_TCP_TIME)(tcp_rexmt_val(cep) * tcp_back_off[cep->rxtshift]),
		                            (T_TCP_TIME)TCP_TVAL_MIN,
		                            (T_TCP_TIME)TCP_TVAL_MAX_REXMT);
		cep->timer[TCP_TIM_REXMT] = cep->rxtcur;

		/*
		 *  srtt:   平滑化された RTT
		 *  rttvar: 平滑化された分散
		 *
		 *  再送回数が最大再送回数の 1/4 になったときは、
		 *  平滑化された分散 (rttvar) に srtt を加算し、
		 *  平滑化された RTT を 0 にする。
		 *
		 */
		if (cep->rxtshift > TCP_MAX_REXMT_SHIFT / 4) {
			cep->rttvar += (cep->srtt >> TCP_SRTT_SHIFT);
			cep->srtt    = 0;
			}

		/*
		 *  snd_nxt: 次に送信する SEQ、この時点では、前回送信した SEQ
		 *  snd_una: 未確認の最小送信 SEQ	 または、確認された最大送信 SEQ
		 *
		 *  前回送信した SEQ (snd_nxt) を
		 *  確認された最大送信 SEQ (snd_una) まで戻す。
		 */
		cep->snd_nxt = cep->snd_una;
		cep->flags  |= TCP_CEP_FLG_ACK_NOW;

		/*
		 *  rtt: 往復時間の計測を中止する。
		 */
		cep->rtt     = 0;

		/*
		 *  送信ウインドの設定
		 *
		 *  snd_wnd:  相手の受信可能ウィンドサイズ
		 *  snd_cwnd: 輻輳ウィンドサイズ
		 *  maxseg  : 相手の最大受信セグメントサイズ
		 *
		 *  相手の受信可能ウィンドサイズ (snd_wnd) か、
		 *  輻輳ウィンドサイズ (snd_cwnd) の
		 *  どちらか小さいサイズの 1/2 を、更に
		 *  相手の最大受信セグメントサイズ (maxseg) で割った値。
		 *  ただし、2 以上
		 */
		if (cep->snd_wnd < cep->snd_cwnd)
			win = cep->snd_wnd / 2 / cep->maxseg;
		else
			win = cep->snd_cwnd / 2 / cep->maxseg;

		if (win < 2)
			win = 2;

		/*
		 *  輻輳ウィンドサイズ (snd_cwnd) は
		 *  相手の受信可能ウィンドサイズ (snd_wnd) に、
		 *  輻輳ウィンドサイズのしきい値 (snd_ssthresh) は
		 *  相手の受信可能ウィンドサイズ (snd_wnd) の win 倍に
		 *  設定する。
		 */
		cep->snd_cwnd     = cep->maxseg;
		cep->snd_ssthresh = win * cep->maxseg;
		cep->dupacks      = 0;

		/* 出力をポストする。*/
		cep->flags |= TCP_CEP_FLG_POST_OUTPUT;
		SetEvent(TCP_OUTPUT_TASK, EVT_TCP_POST_OUTPUT);
		break;

	/*
	 *  持続タイマ
	 */
	case TCP_TIM_PERSIST:

		/*
		 *  最大再送回数 (TCP_MAX_REXMT_SHIFT、標準 12 回) を超えていて、
		 *  アイドル時間が、保留タイマの標準値 (TCP_TVAL_KEEP_IDLE、
		 *  標準 2 * 60 * 60 秒) 以上か、
		 *  再送タイムアウト値 * バックオフ時間の合計以上なら
		 *  コネクションを切断する。
		 */
		if (cep->rxtshift > TCP_MAX_REXMT_SHIFT &&
		    (cep->idle >= TCP_TVAL_KEEP_IDLE ||
		     cep->idle >= tcp_rexmt_val(cep) * TCP_TOTAL_BACK_OFF)) {
			cep->net_error = EV_REXMTMO;
			cep = tcp_drop(cep, E_CLS);
			break;
			}

		/* 持続タイマを再設定し、出力をポストする。*/
		tcp_set_persist_timer(cep);

		cep->flags |= TCP_CEP_FLG_FORCE | TCP_CEP_FLG_FORCE_CLEAR | TCP_CEP_FLG_POST_OUTPUT;
		SetEvent(TCP_OUTPUT_TASK, EVT_TCP_POST_OUTPUT);
		break;

	/*
	 *  保留 (keep alive) タイマ
	 */
	case TCP_TIM_KEEP:

		/*
		 *  コネクションが開設されるまでにタイムアウトしたら
		 *  コネクションの開設を中止する。
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
	 *  2MSL タイマ
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
 *  tcp_cancel_timers -- 全てのタイマをキャンセルする。
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
 *  tcp_tw_timo -- Time Wait 用 TCP 通信端点のタイムアウト処理
 */

void
tcp_tw_timo (void)
{
	T_TCP_CEP	*cep, *lcep;
	T_TCP_TWCEP*	twcep;
	T_TCP_TIME	longer;
	int_t		frees, waits;

	/*
	 *  Time Wait 用 TCP 通信端点でタイムアウトのエントリを探索し、
	 *  探索したエントリを空きにする。
	 */
	frees = 0;
	for (twcep = &tcp_twcep[NUM_TCP_TW_CEP_ENTRY]; twcep -- != tcp_twcep; )
		if (twcep->fsm_state == TCP_FSM_TIME_WAIT && -- twcep->timer_2msl == 0) {
			twcep->fsm_state = TCP_FSM_CLOSED;
			frees ++;
			}

	/*
	 *  Time Wait 用 TCP 通信端点の空きエントリが発生したら
	 *  標準 TCP 通信端点で Time Wait 中の TCP 通信端点を探索し、
	 *  必要な情報を Time Wait 用 TCP 通信端点に移して、
	 *  標準の TCP 通信端点を開放する。
	 */
	if (frees > 0) {
		waits = 0;
		for (cep = &tcp_cep[tmax_tcp_cepid]; cep -- != tcp_cep; )
			if (cep->fsm_state == TCP_FSM_TIME_WAIT)
				waits ++;
		if (waits > frees)
			waits = frees;

		/*
		 *  もっとも長い間 Time Wait している TCP 通信端点を開放する。
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
 *  tcp_slow_timo -- 500 [ms] 毎に呼出される TCP タイムアウト関数
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
 *  tcp_fast_timo -- 200 [ms] 毎に呼出される TCP タイムアウト関数
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
 *  tcp_range_set -- 時間を指定した範囲に設定する。
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
 *  tcp_set_persist_timer -- 持続タイマの設定
 */

void
tcp_set_persist_timer (T_TCP_CEP *cep)
{
	T_TCP_TIME time;

	/*
	 *  srtt:   平滑化された RTT
	 *  rttvar: 平滑化された分散
	 */
	time = ((cep->srtt >> 2) + cep->rttvar) >> 1;

	/*
	 *  再送タイマも設定されていれば回復不能エラー
	 */
	if (cep->timer[TCP_TIM_REXMT])
		/* %%% panic("tcp_output REXMT"); %%% */;

	/*
	 *  持続タイマを設定する。
	 */
	cep->timer[TCP_TIM_PERSIST] = tcp_range_set((T_TCP_TIME)(time * tcp_back_off[cep->rxtshift]),
	                                            (T_TCP_TIME)TCP_TVAL_MIN_PERSIST,
	                                            (T_TCP_TIME)TCP_TVAL_MAX_PERSIST);

	/*
	 *  再送回数を更新する。
	 */
	if (cep->rxtshift < TCP_MAX_REXMT_SHIFT)
		cep->rxtshift ++;
	}

#endif	/* of #ifdef SUPPORT_TCP */
