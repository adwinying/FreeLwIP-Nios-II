#ifndef _IF_ARP_H_
#define _IF_ARP_H_

#ifdef SUPPORT_ETHER

/*
 *  Address Resolution Protocol.
 *
 *  RFC 826 ����
 */

/*
 *  ��¤��
 */

/* ARP �إå� */

typedef struct t_arp_hdr {
	uint16_t	hrd_addr;	/* ʪ�����ɥ쥹�ե����ޥå�	*/
	uint16_t	proto;		/* �ץ�ȥ���			*/
	uint8_t		hdr_len;	/* ʪ�����ɥ쥹Ĺ		*/
	uint8_t		proto_len;	/* �ץ�ȥ���Ĺ			*/
	uint16_t	opcode;		/* ̿�ᥳ����			*/
	} T_ARP_HDR;

#define ARP_HDR_SIZE			(sizeof(T_ARP_HDR))
#define IF_ARP_HDR_SIZE			(IF_HDR_SIZE + ARP_HDR_SIZE)

#define GET_ARP_HDR(nbuf)		((T_ARP_HDR*)((nbuf)->buf + IF_HDR_SIZE))

#define GET_IF_ARP_HDR_SIZE(nbuf)	(IF_HDR_SIZE + ARP_HDR_SIZE)

/* hrd_addr��ʪ�����ɥ쥹�ե����ޥåȤ��� */

#define ARPHRD_ETHER	1	/* Ethernet	*/

/* opcode, ̿�ᥳ���ɤ��� */

#define ARPOP_REQUEST		UINT_C(1)	/* ���ɥ쥹����׵�		*/
#define ARPOP_REPLY		UINT_C(2)	/* ���ɥ쥹������		*/
#define ARPOP_REV_REQUEST	UINT_C(3)	/* �ץ�ȥ��롦���ɥ쥹����׵�	*/
#define ARPOP_REV_REPLY		UINT_C(4)	/* �ץ�ȥ��롦���ɥ쥹����׵�	*/
#define ARPOP_INV_REQUEST	UINT_C(8)	/* ��ü�����׵�			*/
#define ARPOP_INV_REPLY		UINT_C(9)	/* ��ü�����׵�			*/

/*
 *  �������ͥåȥɥ饤�Х⥸�塼��ǻ��Ѥ���� ARP ����
 */

typedef struct arpcom {
	uint8_t		mac_addr[ETHER_ADDR_LEN];
	} T_ARPCOM;

/*
 *  ARP ����å��幹�������ޤ����
 */

#define ARP_TIMER_TMO		(60*NET_TIMER_HZ)	/* �����ॢ���ȡ�60 [s]	*/

/*
 *  �ѿ�
 */

extern const uint8_t	ether_broad_cast_addr[ETHER_ADDR_LEN];

#endif	/* of #ifdef SUPPORT_ETHER */

#endif	/* of #ifndef _IF_ARP_H_ */
