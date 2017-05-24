#ifndef _TCP_H_
#define _TCP_H_

/*
 *  TCP �إå���RFC 793 ����
 */

typedef struct t_tcp_hdr {
	uint16_t	sport;		/* �������ݡ����ֹ�			*/
	uint16_t	dport;		/* ����ݡ����ֹ�			*/

	uint32_t	seq;		/* SEQ �ֹ�				*/

	uint32_t	ack;		/* ACK �ֹ�				*/

	uint8_t		doff;		/* �ǡ������ե��å�(���4�ӥå�)	*/
	uint8_t		flags;		/* �ե饰(����6�ӥå�)			*/
	uint16_t	win;		/* ������ɥ�����			*/

	uint16_t	sum;		/* �����å�����				*/
	uint16_t	urp;		/* �۵ޥݥ���				*/
	} T_TCP_HDR;

#define TCP_HDR_SIZE			(sizeof(T_TCP_HDR))

/*
 *  TCP �إå���¤�Υ��Х��������ޥ���
 */

#define TCP_MAKE_DATA_OFF(o)	((uint8_t)(((uint8_t)(o) << 2) & 0xf0))
#define TCP_DATA_OFF(o)		(((uint8_t)(o) & UINT_C(0xf0)) >> 2)
#define TCP_HDR_LEN(o)		(TCP_DATA_OFF(o))

/*
 *  TCP �إå��Υե饰�ե�����ɤ��ͤ����
 */

#define TCP_FLG_FIN		UINT_C(0x01)	/* FIN:	�ǽ�			*/
#define TCP_FLG_SYN		UINT_C(0x02)	/* SYN:	Ʊ��			*/
#define TCP_FLG_RST		UINT_C(0x04)	/* RST:	���ͥ������Υꥻ�å�	*/
#define TCP_FLG_PUSH		UINT_C(0x08)	/* PUSH:�ץå��嵡ǽ		*/
#define TCP_FLG_ACK		UINT_C(0x10)	/* ACK:	�������		*/
#define TCP_FLG_URG		UINT_C(0x20)	/* URG:	�۵޾���		*/

/*
 *  TCP ���ץ��������
 */

/* ���ץ����η� */

#define TCP_OPT_EOL		UINT_C(0)	/* �ǽ�				*/
#define TCP_OPT_NOP		UINT_C(1)	/* ���⤷�ʤ�			*/
#define TCP_OPT_MAXSEG		UINT_C(2)	/* ���祻������Ĺ		*/
#define TCP_OPT_WINDOW		UINT_C(3)	/* ������ɥ��������		*/
#define TCP_OPT_SACK_PERM	UINT_C(4)	/* ���� ACK ����		*/
#define TCP_OPT_SACK		UINT_C(5)	/* ���� ACK			*/
#define TCP_OPT_TIMESTAMP	UINT_C(8)	/* �����ॹ�����		*/
#define TCP_OPT_CC		UINT_C(11)	/* ��³�������			*/
#define TCP_OPT_CCNEW		UINT_C(12)	/* ��³�������			*/
#define TCP_OPT_CCECHO		UINT_C(13)	/* ��³�������			*/

/* ���ץ�����Ĺ�� */

#define TCP_OPT_LEN_MAXSEG	4		/* ���祻������Ĺ		*/
#define TCP_OPT_LEN_WINDOW	3		/* ������ɥ��������		*/
#define TCP_OPT_LEN_SACK_PERM	4		/* ���� ACK ����		*/
#define TCP_OPT_LEN_SACK	5		/* ���� ACK			*/
#define TCP_OPT_LEN_TIMESTAMP	10		/* �����ॹ�����		*/
#define TCP_OPT_LEN_CC		6		/* ��³�������			*/
#define TCP_OPT_LEN_CCNEW	6		/* ��³�������			*/
#define TCP_OPT_LEN_CCECHO	6		/* ��³�������			*/

/*
 *  TCP ���祻�����ȥ�����
 */

/*
 *  IPv4 �� TCP ���祻�����ȥ�����
 *  IP4_MSS �� 576 �ǡ�ɸ��� IPv4 �إå�Ĺ (20) ��
 *  TCP �إå�Ĺ (20) ��������� 536 �Ǥ��뤬��512 ��ɸ��Ȥ��롣
 */

#define TCP_MSS			UINT_C(512)

/*
 *  IPv6 �� TCP ���祻�����ȥ�����
 *  IPV6_MMTU �� 1280 �ǡ�ɸ��� IPv6 �إå�Ĺ (40) ��
 *  TCP �إå�Ĺ (20) ��������� 1220 �Ǥ��뤬��1024 ��ɸ��Ȥ��롣
 */

#define TCP6_MSS		UINT_C(1024)

/*
 *  TCP �Ǿ��������ȥ�����
 *
 *  256�ʺǾ���� MTU��- 40 = 216
 */

#define TCP_MINMSS		UINT_C(216)

/*
 *  TCP ���祦����ɥ�������
 */

#define MAX_TCP_WIN_SIZE	UINT_C(65535)

#endif	/* of #ifndef _TCP_H_ */
