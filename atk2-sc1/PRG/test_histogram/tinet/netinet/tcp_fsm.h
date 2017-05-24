#ifndef _TCP_FSM_H_
#define _TCP_FSM_H_

/*
 *  TCP FSM ���֤������RFC 793 ����
 */

#define TCP_FSM_CLOSED		0	/* ������				*/
#define TCP_FSM_LISTEN		1	/* ��ư�����ץ�				*/
#define TCP_FSM_SYN_SENT	2	/* ǽư�����ץ�SYN �����Ѥ�		*/
#define TCP_FSM_SYN_RECVD	3	/* ��ư�����ץ�SYN �����Ѥ�		*/
#define TCP_FSM_ESTABLISHED	4	/* ���ͥ�������ߴ�λ			*/
#define TCP_FSM_CLOSE_WAIT	5	/* ��꤫�� FIN ������APP �ν�λ�Ԥ�	*/
#define TCP_FSM_FIN_WAIT_1	6	/* APP ����λ��FIN �����Ѥߡ�ACK �Ԥ�	*/
#define TCP_FSM_CLOSING		7	/* Ʊ����������FIN �򴹺Ѥߡ�ACK �Ԥ�	*/
#define TCP_FSM_LAST_ACK	8	/* APP ����λ��ACK �Ԥ�			*/
#define TCP_FSM_FIN_WAIT_2	9	/* ��꤫��� FIN �Ԥ�			*/
#define TCP_FSM_TIME_WAIT	10	/* ��꤫��� FIN �����Ѥߡ������Ԥ�	*/

/*
 *  �ؿ����ߥ�졼�����ޥ���
 */

#define TCP_FSM_HAVE_RCVD_SYN(s)	((s) >= TCP_FSM_SYN_RECVD)
#define TCP_FSM_HAVE_ESTABLISHED(s)	((s) >= TCP_FSM_ESTABLISHED)
#define TCP_FSM_HAVE_RCVD_FIN(s)	((s) >= TCP_FSM_TIME_WAIT)

#define TCP_FSM_CAN_SEND_MORE(s)	((s) == TCP_FSM_ESTABLISHED || \
					 (s) == TCP_FSM_CLOSE_WAIT)
#define TCP_FSM_CAN_RECV_MORE(s)	((s) == TCP_FSM_ESTABLISHED || \
					 (s) == TCP_FSM_FIN_WAIT_1  || \
					 (s) == TCP_FSM_FIN_WAIT_2)

#endif	/* of #ifndef _TCP_FSM_H_ */
