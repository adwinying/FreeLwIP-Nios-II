#ifndef _ETHERNET_CFG_H_
#define _ETHERNET_CFG_H_

/*
 *  Ethernet ����ե�����졼�����ե�����
 */

#ifdef SUPPORT_ETHER

/****** Object DATAQUEUE ******/

enum {
	DTQ_ETHER_OUTPUT,
	TNUM_ETHERNET_DATAQUEUE
	};

/*
 *  �ؿ�
 */

extern void ethernet_initialize (void);
extern ER ethernet_snd_dtq (ID dtqid, intptr_t data);
extern ER ethernet_rcv_dtq (ID dtqid, intptr_t *p_data);

#endif	/* of #ifdef SUPPORT_ETHER */

#endif	/* of #ifndef _ETHERNET_CFG_H_ */
