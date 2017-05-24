#ifndef _IP4_H_
#define _IP4_H_

/*
 *  IPv4 �������RFC791 ����
 */

#define IPV4_VERSION		4

/*
 *  IPv4 �إå������
 */

typedef struct t_ip4_hdr {
	uint8_t		vhl;		/* version << 4 | header length >> 2	*/
					/* length �� 4 octets ñ��		*/
	uint8_t		type;		/* type of services			*/
	uint16_t	len;		/* total length				*/

	uint16_t	id;		/* identification			*/
	uint16_t	flg_off;	/* fragment flag & offset		*/

	uint8_t		ttl;		/* time to live				*/
	uint8_t		proto;		/* protocol				*/
	uint16_t	sum;		/* checksum				*/

	T_IN4_ADDR	src;		/* source address			*/
	T_IN4_ADDR	dst;		/* destination address			*/
	} T_IP4_HDR;

#define IP4_HDR_SIZE		(sizeof(T_IP4_HDR))

/*
 *  IPv4 �إå���¤�Υ��Х��������ޥ���
 */

#define IP4_MAKE_VHL(v,hl)	((uint8_t)(((uint8_t)(v)<<4)|(hl)))
#define IP4_VHL_V(vhl)		((uint8_t)((uint8_t)(vhl)>>4))
#define IP4_VHL_HL(vhl)		((uint8_t)((vhl)&0x0f))

#define IP4_MAKE_FLGOFF(f,o)	((uint16_t)((f)|(o)))
#define IP4_FLGOFF_FLG(f)	((uint16_t)((f)&~IP4_OFFMASK))
#define IP4_FLGOFF_OFF(o)	((uint16_t)((o)&IP4_OFFMASK))

/*
 *  IPv4 �إå��Υե�����ɤ��ͤ����
 */

#define IP4_VHL_BORING	UINT_C(0x45)	/* ���ץ���󤬤ʤ� VHL ��	*/
#define IP4_RF		UINT_C(0x8000)	/* off: ͽ��			*/
#define IP4_DF		UINT_C(0x4000)	/* off: ʬ��ػ�		*/
#define IP4_MF		UINT_C(0x2000)	/* off: ʬ�䤢��		*/
#define IP4_OFFMASK	UINT_C(0x1fff)	/* ���ե��å����Υޥ�����	*/

/*
 *  ������
 */

#define IP4_MAXPACKET	UINT_C(65535)	/* ����ѥ��åȥ�����		*/
#define IP4_MAXTTL	UINT_C(255)	/* TTL ������			*/
#define IP4_DEFTTL	UINT_C(64)	/* TTL ������ (RFC1340)		*/
#define IP4_FRAGTTL	UINT_C(60)	/* ʬ�䤵�줿�ǡ�����������¸���֡��á�*/
#define IP4_MSS		UINT_C(576)	/* ���祻�����ȥ������ε�����	*/

/* IPv4 �� IPv6 �򥳥�ѥ���������򤹤뤿��Υޥ��� */

#if defined(SUPPORT_INET4)

#define IP_HDR_SIZE	IP4_HDR_SIZE
#define T_IP_HDR	T_IP4_HDR
#define IP_DEFTTL	IP4_DEFTTL

#endif	/* of #if defined(SUPPORT_INET4) */

#endif	/* of #ifndef _IP4_H_ */
