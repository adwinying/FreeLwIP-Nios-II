#ifndef _TCP_SEQ_H_
#define _TCP_SEQ_H_

/*
 *  SEQ �� ACK �ֹ�ϡ�32 �ӥå��������������Ӥ��롣
 */

#define SEQ_LT(a,b)		((int32_t)((a)-(b)) <  0)
#define SEQ_LE(a,b)		((int32_t)((a)-(b)) <= 0)
#define SEQ_GT(a,b)		((int32_t)((a)-(b)) >  0)
#define SEQ_GE(a,b)		((int32_t)((a)-(b)) >= 0)

/*
 *  �ؿ����ߥ�졼�����ޥ���
 */

#define	TCP_ISS_INCR()	((T_TCP_SEQ)122*1024 + (((T_TCP_SEQ)net_rand() >> 14) & 0x3ffff))

/*
 *  �����ѿ�
 */

extern T_TCP_SEQ tcp_iss;		/* TCP �� SEQ �ν���� */

/*
 *  ����饤��ؿ�
 */

/*
 *  init_receive_seq -- ���� SEQ �ֹ���������롣
 */

LOCAL_INLINE void
init_receive_seq (T_TCP_CEP *cep)
{
	cep->rcv_adv = cep->rcv_nxt = cep->irs + 1;
	}

/*
 *  init_send_seq -- ���� SEQ �ֹ���������롣
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
