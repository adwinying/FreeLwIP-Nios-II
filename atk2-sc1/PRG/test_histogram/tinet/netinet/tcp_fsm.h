#ifndef _TCP_FSM_H_
#define _TCP_FSM_H_

/*
 *  TCP FSM 状態の定義、RFC 793 参照
 */

#define TCP_FSM_CLOSED		0	/* クローズ				*/
#define TCP_FSM_LISTEN		1	/* 受動オープン				*/
#define TCP_FSM_SYN_SENT	2	/* 能動オープン、SYN 送信済み		*/
#define TCP_FSM_SYN_RECVD	3	/* 受動オープン、SYN 受信済み		*/
#define TCP_FSM_ESTABLISHED	4	/* コネクション開設完了			*/
#define TCP_FSM_CLOSE_WAIT	5	/* 相手から FIN 受信、APP の終了待ち	*/
#define TCP_FSM_FIN_WAIT_1	6	/* APP が終了、FIN 送信済み、ACK 待ち	*/
#define TCP_FSM_CLOSING		7	/* 同時クローズ、FIN 交換済み、ACK 待ち	*/
#define TCP_FSM_LAST_ACK	8	/* APP が終了、ACK 待ち			*/
#define TCP_FSM_FIN_WAIT_2	9	/* 相手からの FIN 待ち			*/
#define TCP_FSM_TIME_WAIT	10	/* 相手からの FIN 受信済み、時間待ち	*/

/*
 *  関数シミュレーションマクロ
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
