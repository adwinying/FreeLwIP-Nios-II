#ifndef _UDP_H_
#define _UDP_H_

/*
 *  UDP �إå���RFC768 ����
 */

typedef struct t_udp_hdr {
	uint16_t	sport;		/* �������ݡ����ֹ�	*/
	uint16_t	dport;		/* ����ݡ����ֹ�	*/

	uint16_t	ulen;		/* �ǡ��������Ĺ	*/
	uint16_t	sum;		/* �����å�����		*/
	} T_UDP_HDR;

#define UDP_HDR_SIZE			(sizeof(T_UDP_HDR))

#endif	/* of #ifndef _UDP_H_ */
