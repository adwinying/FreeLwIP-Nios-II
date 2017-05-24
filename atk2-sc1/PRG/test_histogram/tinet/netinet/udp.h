#ifndef _UDP_H_
#define _UDP_H_

/*
 *  UDP ヘッダ、RFC768 参照
 */

typedef struct t_udp_hdr {
	uint16_t	sport;		/* 送信元ポート番号	*/
	uint16_t	dport;		/* 宛先ポート番号	*/

	uint16_t	ulen;		/* データグラム長	*/
	uint16_t	sum;		/* チェックサム		*/
	} T_UDP_HDR;

#define UDP_HDR_SIZE			(sizeof(T_UDP_HDR))

#endif	/* of #ifndef _UDP_H_ */
