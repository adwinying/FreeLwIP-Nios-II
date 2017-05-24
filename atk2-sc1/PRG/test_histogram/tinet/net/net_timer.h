#ifndef _NET_TIMER_H_
#define _NET_TIMER_H_

#include <tinet_kernel_defs.h>	/* �����ͥ�˴ؤ���ߴ�������ե�����򥤥󥯥롼�ɤ��롣*/

/*
 *  TOPPERS/JSP �����ͥ롢�����ƥ���� (SYSTIM) �μ��ȿ�
 */

#define SYSTIM_HZ		ULONG_C(1000)	/* [ms]					*/

/*
 *  �ͥåȥ�������ޤ����
 */

#define NET_TIMER_CYCLE		ULONG_C(100)	/* [ms]��fast timeout ���ѻ���ư�����	*/
						/* �����ޤμ��ȿ�			*/
#define NET_TIMER_HZ		(SYSTIM_HZ/NET_TIMER_CYCLE)

#ifdef SUPPORT_INET6
#define NUM_NET_CALLOUT		12		/* �����ॢ���ȸƽФ�����10 �ʾ�	*/
#else
#define NUM_NET_CALLOUT		10		/* �����ॢ���ȸƽФ�����10 �ʾ�	*/
#endif

/*
 *  �����ॢ���ȸƽФ�����ȥ�
 */

typedef void	(*callout_func)(void*);

typedef struct t_net_callout {
	callout_func	func;		/* �ƽФ��ؿ�		*/
	void		*arg;		/* �ؿ��ؤΰ���		*/
	RELTIM		timeout;	/* �����ॢ����		*/
	} T_NET_CALLOUT;

/*
 *  �ؿ�
 */

extern void timeout (callout_func func, void *arg, RELTIM timeout);
extern void untimeout (callout_func func, void *arg);

#endif	/* of #ifndef _NET_TIMER_H_ */
