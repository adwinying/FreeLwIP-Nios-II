#ifndef _TINET_CFG_H_
#define _TINET_CFG_H_

/*
 *  TINET �����ͥ빽���ե�����
 */

#define USRV_CEPID	1

/****** Object DATAQUEUE ******/

enum {
	/* UDP (IPv4) �̿�ü�� */
	DTQ_UDP_RCVQ1,

	TNUM_TINET_DATAQUEUE
	};

/*
 *  �ؿ�
 */

extern void tinet_initialize (void);
extern ER tinet_snd_dtq (ID dtqid, intptr_t data);
extern ER tinet_rcv_dtq (ID dtqid, intptr_t *p_data);

#endif	/* of #ifndef _TINET_CFG_H_ */
