#ifndef _TINET_CFG_H_
#define _TINET_CFG_H_

/*
 *  TINET カーネル構成ファイル
 */

#define USRV_CEPID	1

/****** Object DATAQUEUE ******/

enum {
	/* UDP (IPv4) 通信端点 */
	DTQ_UDP_RCVQ1,

	TNUM_TINET_DATAQUEUE
	};

/*
 *  関数
 */

extern void tinet_initialize (void);
extern ER tinet_snd_dtq (ID dtqid, intptr_t data);
extern ER tinet_rcv_dtq (ID dtqid, intptr_t *p_data);

#endif	/* of #ifndef _TINET_CFG_H_ */
