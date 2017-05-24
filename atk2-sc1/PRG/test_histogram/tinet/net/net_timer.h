#ifndef _NET_TIMER_H_
#define _NET_TIMER_H_

#include <tinet_kernel_defs.h>	/* カーネルに関する互換性定義ファイルをインクルードする。*/

/*
 *  TOPPERS/JSP カーネル、システム時刻 (SYSTIM) の周波数
 */

#define SYSTIM_HZ		ULONG_C(1000)	/* [ms]					*/

/*
 *  ネットワークタイマの定義
 */

#define NET_TIMER_CYCLE		ULONG_C(100)	/* [ms]、fast timeout 使用時の動作周期	*/
						/* タイマの周波数			*/
#define NET_TIMER_HZ		(SYSTIM_HZ/NET_TIMER_CYCLE)

#ifdef SUPPORT_INET6
#define NUM_NET_CALLOUT		12		/* タイムアウト呼出し数、10 以上	*/
#else
#define NUM_NET_CALLOUT		10		/* タイムアウト呼出し数、10 以上	*/
#endif

/*
 *  タイムアウト呼出しエントリ
 */

typedef void	(*callout_func)(void*);

typedef struct t_net_callout {
	callout_func	func;		/* 呼出す関数		*/
	void		*arg;		/* 関数への引数		*/
	RELTIM		timeout;	/* タイムアウト		*/
	} T_NET_CALLOUT;

/*
 *  関数
 */

extern void timeout (callout_func func, void *arg, RELTIM timeout);
extern void untimeout (callout_func func, void *arg);

#endif	/* of #ifndef _NET_TIMER_H_ */
