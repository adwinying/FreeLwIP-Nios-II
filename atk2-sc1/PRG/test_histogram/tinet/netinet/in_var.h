#ifndef _IN_VAR_H_
#define _IN_VAR_H_

#if defined(SUPPORT_INET6)

#include <netinet6/in6_var.h>

#endif	/* of #if defined(SUPPORT_INET6) */

/*
 *  �ȼ��Υ��顼������
 */

#define EV_NOPND		(-97)		/* �ڥ�ǥ��󥰤��Ƥ��ʤ� */

/*
 *  �ͥåȥ�����顼������
 */

#define EV_MSIZE		(-98)		/* �ǡ��������ĹĶ��	*/
#define EV_NURCH		(-99)		/* �ͥåȥ����ã��ǽ	*/
#define EV_HDOWN		(-100)		/* �ۥ��ȥ�����		*/
#define EV_HURCH		(-101)		/* �ۥ�����ã��ǽ	*/
#define EV_CNNRF		(-102)		/* ��³��ǽ		*/
#define EV_CNRST		(-103)		/* ��³�ꥻ�å�		*/
#define EV_REXMTMO		(-104)		/* ���������ॢ����	*/

/*
 *  ���̥����ޤε�ư���������
 */

#define IN_TIMER_TMO		(SYSTIM_HZ/NET_TIMER_CYCLE)

/*
 *  ���󥿥ե��������ɥ쥹��¤��
 */

typedef struct t_in4_ifaddr {
	T_IN4_ADDR		addr;		/* IPv4 ���ɥ쥹	*/
	T_IN4_ADDR		mask;		/* ���֥ͥåȥޥ���	*/
	} T_IN4_IFADDR;

/*
 *  �롼�ƥ���ɽ����ȥ깽¤��
 */

typedef struct t_in4_rtentry {
	T_IN4_ADDR	target;		/* ��ɸ�ͥåȥ�����ɥ쥹			*/
	T_IN4_ADDR	mask;		/* ��ɸ�ͥåȥ�����ɥ쥹�Υ��֥ͥåȥޥ���	*/
	T_IN4_ADDR	gateway;	/* �����ȥ������� IP ���ɥ쥹			*/
	uint32_t	expire;		/* ͭ�����֤��ڤ����� [s]			*/
	uint8_t		flags;		/* �ե饰					*/
	} T_IN4_RTENTRY;

/*
 *  TINET 1.1 �Ȥθߴ����Τ�������
 */

#ifdef NUM_ROUTE_ENTRY

#define NUM_STATIC_ROUTE_ENTRY		NUM_ROUTE_ENTRY
#define NUM_REDIRECT_ROUTE_ENTRY	0
#define T_RT_ENTRY			T_IN_RTENTRY

#else	/* of #ifdef NUM_ROUTE_ENTRY */

#define NUM_ROUTE_ENTRY			\
	(NUM_STATIC_ROUTE_ENTRY + NUM_REDIRECT_ROUTE_ENTRY)
					/* �롼�ƥ���ɽ�Υ���ȥ��	*/

#endif	/* of #ifdef NUM_ROUTE_ENTRY */

/*
 *  �롼�ƥ��󥰥���ȥ���֥ե饰
 */

#define IN_RTF_DEFINED		UINT_C(0x01)	/* ����Ѥ�			*/
#define IN_RTF_REDIRECT		UINT_C(0x10)	/* ����ľ���ˤ�꼫ư���ꤵ�줿	*/

/*
 *  IPv4 �� IPv6 �򥳥�ѥ���������򤹤뤿��Υޥ���
 */

#if defined(SUPPORT_INET4)

#define IN_SET_HEADER(nbuf,len,dst,src,proto,ttl)	\
					in4_set_header(nbuf,len,dst,src,proto,ttl)
#define IN_GET_DATAGRAM(nbuf,len,maxlen,dst,src,proto,ttl,nbatr)	\
					in4_get_datagram(nbuf,len,maxlen,dst,src,proto,ttl,nbatr)
#define IN_CKSUM(nbuf,proto,off,len)	in4_cksum(nbuf,proto,off,len)
#define IN_IS_DSTADDR_ACCEPT(myaddr,dstaddr)			\
					in4_is_dstaddr_accept(myaddr,dstaddr)
#define IN_IFAWITHIFP(ifp,dst)		in4_ifawithifp(ifp,dst)
#define T_IN_IFADDR			T_IN4_IFADDR
#define T_IN_RTENTRY			T_IN4_RTENTRY

#endif	/* of #if defined(SUPPORT_INET4) */

/*
 *  �롼�ƥ���ɽ
 */

extern T_IN_RTENTRY routing_tbl[];

/*
 *  ��������
 */

#ifndef T_NET_BUF_DEFINED
typedef struct t_net_buf T_NET_BUF;
#define T_NET_BUF_DEFINED
#endif

#ifndef T_IFNET_DEFINED
typedef struct t_ifnet T_IFNET;
#define T_IFNET_DEFINED
#endif

/*
 *  �ؿ�
 */

extern void ip_input (T_NET_BUF *data);
extern ER in4_set_header (T_NET_BUF *nbuf, uint_t len,
                          T_IN4_ADDR *dstaddr, T_IN4_ADDR *srcaddr, uint8_t proto, uint8_t ttl);
extern ER in4_get_datagram (T_NET_BUF **nbuf, uint_t len, uint_t maxlen,
                            T_IN4_ADDR *dstaddr, T_IN4_ADDR *srcaddr,
                            uint8_t proto, uint8_t ttl, ATR nbatr);
extern uint16_t in4_cksum (T_NET_BUF *nbuf, uint8_t proto, uint_t off, uint_t len);
extern T_IN4_ADDR in4_rtalloc (T_IN4_ADDR dst);
extern void in4_rtredirect (T_IN4_ADDR gateway, T_IN4_ADDR target, uint8_t flags, uint32_t tmo);
extern void in4_init (void);
extern T_IN4_IFADDR *in4_ifawithifp (T_IFNET *ifp, T_IN4_ADDR *dst);
extern T_IN_RTENTRY *in_rtnewentry (uint8_t flags, uint32_t tmo);
extern void in_rttimer (void);
extern void in_rtinit (void);
extern const T_NET_BUF**ip_get_frag_queue (void);

/*
 *  in4_is_dstaddr_accept -- ���襢�ɥ쥹�Ȥ����������������å����롣
 *
 *    ����: dstaddr �ϡ�
 *          TINET-1.2 ����ͥåȥ���Х��ȥ�������
 *          TINET-1.1 �ޤǤϡ��ۥ��ȥХ��ȥ�����
 */
extern bool_t in4_is_dstaddr_accept (T_IN4_ADDR *myaddr, T_IN4_ADDR *dstaddr);

/*
 *  in_cksum -- �����å�����׻��ؿ���IPv4��ICMPv4 ��
 *
 *  ����: data �� 4 �����ƥå�ñ�̤ǥѥǥ��󥰤��뤳�ȡ�
 *        data �� 2 �����ƥå�ñ�̤˥��饤�󤵤�Ƥ��ʤ���
 *        �㳰��ȯ�������ǽ�������롣
 *        len  �� 4 �����ƥå�ñ�̤˥��饤�󤵤�Ƥ��뤳�ȡ�
 */
extern uint16_t in_cksum(void *data, uint_t len /*�����ƥå�ñ��*/);

/*
 *  in_cksum_sum -- �����å�����ι�׷׻��ؿ�
 *
 *  ����: data �� 4 �����ƥå�ñ�̤ǥѥǥ��󥰤��뤳�ȡ�
 *        data �� 2 �����ƥå�ñ�̤˥��饤�󤵤�Ƥ��ʤ���
 *        �㳰��ȯ�������ǽ�������롣
 *        len  �� 4 �����ƥå�ñ�̤˥��饤�󤵤�Ƥ��뤳�ȡ�
 */
extern uint32_t in_cksum_sum (void *data, uint_t len /*�����ƥå�ñ��*/);

/*
 *  in_cksum_carry -- �����å�����η�夲�׻��ؿ�
 *
 *  ����: data �� 4 �����ƥå�ñ�̤ǥѥǥ��󥰤��뤳�ȡ�
 *        data �� 2 �����ƥå�ñ�̤˥��饤�󤵤�Ƥ��ʤ���
 *        �㳰��ȯ�������ǽ�������롣
 *        len  �� 4 �����ƥå�ñ�̤˥��饤�󤵤�Ƥ��뤳�ȡ�
 */
extern uint32_t in_cksum_carry (uint32_t sum);

#endif	/* of #ifndef _IN_VAR_H_ */