#ifndef _TCP_SEQ_H_
#define _TCP_SEQ_H_

/*
 *  SEQ と ACK 番号は、32 ビット符号付整数で比較する。
 */

#define SEQ_LT(a,b)		((int32_t)((a)-(b)) <  0)
#define SEQ_LE(a,b)		((int32_t)((a)-(b)) <= 0)
#define SEQ_GT(a,b)		((int32_t)((a)-(b)) >  0)
#define SEQ_GE(a,b)		((int32_t)((a)-(b)) >= 0)

/*
 *  関数シミュレーションマクロ
 */

#define	TCP_ISS_INCR()	((T_TCP_SEQ)122*1024 + (((T_TCP_SEQ)net_rand() >> 14) & 0x3ffff))

/*
 *  全域変数
 */

extern T_TCP_SEQ tcp_iss;		/* TCP の SEQ の初期値 */

/*
 *  インライン関数
 */

/*
 *  init_receive_seq -- 受信 SEQ 番号を初期化する。
 */

LOCAL_INLINE void
init_receive_seq (T_TCP_CEP *cep)
{
	cep->rcv_adv = cep->rcv_nxt = cep->irs + 1;
	}

/*
 *  init_send_seq -- 送信 SEQ 番号を初期化する。
 */

LOCAL_INLINE void
init_send_seq (T_TCP_CEP *cep)
{
	cep->snd_una = cep->snd_nxt = cep->snd_max = cep->iss;

#ifdef TCP_CFG_EXTENTIONS
	cep->snd_up = cep->iss;
#endif
	}

#endif	/* of #ifndef _TCP_SEQ_H_ */
