#ifndef _TCP_TIMER_H_
#define _TCP_TIMER_H_

/* TCP タイマ変数 */

#define NUM_TCP_TIMERS		4	/* TCP のタイマ数	*/

#define TCP_TIM_REXMT		0	/* 再送タイマ		*/
#define TCP_TIM_PERSIST		1	/* 持続タイマ		*/
#define TCP_TIM_KEEP		2	/* 保留 (keep alive) タイマ */
#define TCP_TIM_2MSL		3	/* 2MSL タイマ		*/

/* MSL: Maximum Segment Lifetime, 最大セグメント生存時間 		*/

/*
 *  TCP/IP のタイマの定義
 */

#define TCP_SLOW_TMO		(ULONG_C(500)/NET_TIMER_CYCLE)	/* 500 [ms]	*/
#define TCP_FAST_TMO		(ULONG_C(200)/NET_TIMER_CYCLE)	/* 200 [ms]	*/

#define TCP_SLOW_HZ		(ULONG_C(1000)/TCP_SLOW_TMO/NET_TIMER_CYCLE)	/* Hz	*/

/* TCP タイマ値 */

#define TCP_TVAL_MSL		(TCP_SLOW_HZ * 30)
					/* 最大セグメント生存時間	*/

#define TCP_TVAL_SRTT_BASE	0	/* 滑らかな往復時間の基準値	*/

#define TCP_TVAL_RTO_BASE	(TCP_SLOW_HZ * 3)
		/* 情報が得られない場合の再送タイムアウトの基準値	*/

#define TCP_TVAL_MIN_PERSIST	(TCP_SLOW_HZ * 5)
					/* 持続タイムアウトの最小値	*/
#define TCP_TVAL_MAX_PERSIST	(TCP_SLOW_HZ * 60)
					/* 持続タイムアウトの最大値	*/

#define TCP_TVAL_KEEP_INIT	(TCP_SLOW_HZ * 75)
					/* 保留タイマの初期値		*/
#define TCP_TVAL_KEEP_IDLE	(TCP_SLOW_HZ * 2 * 60 * 60)
					/* 保留タイマ起動までの標準値	*/
#define TCP_TVAL_KEEP_INTERVAL	(TCP_SLOW_HZ * 75)
					/* 保留タイマインターバルの標準値 */
#define TCP_TVAL_KEEP_COUNT	8	/* 保留タイムアウトの最大値	*/

#define TCP_TVAL_MIN		(TCP_SLOW_HZ * 3)
					/* 時間の最小単位		*/
#define TCP_TVAL_MAX_REXMT	(TCP_SLOW_HZ * 64)
					/* 最大再送時間			*/

/*
 *  以下の変数の値はマクロで設定する。
 *
 *  tcp_maixidle   = tcp_keepcnt * tcp_keepintvl
 *                 = TCP_TVAL_KEEP_COUNT * TCP_TVAL_KEEP_INTERVAL
 *  tcp_keepcnt    = TCP_TVAL_KEEP_COUNT
 *  tcp_keepintval = TCP_TVAL_KEEP_INTERVAL
 *
 */

/*
 *  その他の定数
 */

#define TCP_MAX_REXMT_SHIFT	12	/* 最大再送回数		*/

/* TCP タイマ値の型 */

typedef int16_t T_TCP_TIME;

/*
 *  関数
 */

extern void tcp_slow_timo (void *ignore);
extern void tcp_fast_timo (void *ignore);

extern T_TCP_TIME tcp_range_set (T_TCP_TIME value, T_TCP_TIME tvmin, T_TCP_TIME tvmax);

#endif	/* of #ifndef _TCP_TIMER_H_ */
