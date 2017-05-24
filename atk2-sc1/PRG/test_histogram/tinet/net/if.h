#ifndef _IF_H_
#define _IF_H_

/*
 *  �ͥåȥ�����󥿥ե������ѥ����ޤ����
 */

#define IF_TIMER_TMO		NET_TIMER_HZ	/* �����ॢ���ȡ�1 [s]		*/
#define IF_TIMER_HZ		1		/* �����ޤμ��ȿ���1 [s]	*/

/*
 *  �ͥåȥ�����󥿥ե������Υե饰
 */

#define IFF_UP			UINT_C(0x0001)	/* ��ư�Ѥ�		*/
#define IFF_RUNNING		UINT_C(0x0002)	/* ư����		*/

/*
 *  �������
 */

#define IF_HDR_SIZE		(sizeof(T_IF_HDR))
#define IF_PDU_SIZE		(IF_HDR_SIZE + IF_MTU)	/* Protocol Data Unit */

/*
 *  �ؿ�
 */

#ifdef SUPPORT_ETHER

extern void ifinit (void);

#else	/* of #ifdef SUPPORT_ETHER */

#define ifinit

#endif	/* of #ifdef SUPPORT_ETHER */

#endif	/* of #ifndef _IF_H_ */
