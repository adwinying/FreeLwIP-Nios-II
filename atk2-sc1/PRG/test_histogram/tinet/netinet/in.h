#ifndef _IN_H_
#define _IN_H_

#include <tinet_kernel_defs.h>

//#include <netinet6/in6.h>

/*
 *  TINET �ΥС���������
 */

#define TINET_PRVER		UINT_C(0x1053)	/* TINET �ΥС�������ֹ� */

/*
 *  ���󥿡��ͥåȥ����ƥ�������RFC790 ����
 */

/*
 *  �ץ�ȥ��� (RFC1700)
 */

#define IPPROTO_IP		UINT_C(0)	/* IP �Υ��ߡ�				*/
#define IPPROTO_HOPOPTS		UINT_C(0)	/* IPv6 ����� (Hop-by-Hop) ���ץ����	*/
#define IPPROTO_ICMP		UINT_C(1)	/* ICMP					*/
#define IPPROTO_TCP		UINT_C(6)	/* TCP					*/
#define IPPROTO_UDP		UINT_C(17)	/* UDP					*/
#define IPPROTO_IPV6		UINT_C(41)	/* IPv6					*/
#define IPPROTO_ROUTING		UINT_C(43)	/* ��ϩ���楪�ץ����			*/
#define IPPROTO_FRAGMENT	UINT_C(44)	/* ���Ҳ����ץ����			*/
#define IPPROTO_RSVP		UINT_C(46)	/* RSVP					*/
#define IPPROTO_ESP		UINT_C(50)	/* IPv6 �Ź沽���ץ����		*/
#define IPPROTO_AH		UINT_C(51)	/* IPv6 ǧ�ڥ��ץ����			*/
#define IPPROTO_ICMPV6		UINT_C(58)	/* ICMPv6				*/
#define IPPROTO_NONE		UINT_C(59)	/* IPv6 ���إå�̵��			*/
#define IPPROTO_DSTOPTS		UINT_C(60)	/* IPv6 �������ץ����			*/
#define IPPROTO_IPCOMP		UINT_C(108)	/* �ڥ����ɰ���			*/
#define IPPROTO_DONE		UINT_C(257)	/* IPv6 �Ǿ�̥ץ�ȥ������Ͻ�λ	*/

/*
 *  IPv4 ���ɥ쥹
 */

/* �������� */

#ifndef T_IN4_ADDR_DEFINED

typedef uint32_t T_IN4_ADDR;

#define T_IN4_ADDR_DEFINED

#endif	/* of #ifndef T_IN4_ADDR_DEFINED */

/*
 *  ITRON TCP/IPv4 ���ɥ쥹/�ݡ����ֹ�����
 */

typedef struct t_ipv4ep {
	uint32_t	ipaddr;		/* IPv4 ���ɥ쥹	*/
	uint16_t	portno;		/* �ݡ����ֹ�		*/
	} T_IPV4EP;

/*
 *  ���ɥ쥹/�ݡ��Ȥ�Ĺ��
 */

#define IPV4_ADDR_LEN		4
#define PORT_NUM_LEN		2

/*
 *  �ݡ����ֹ�����
 */

#define TCP_PORTANY			UINT_C(0x0000)	/* ITRON TCP/IP ɸ��	*/
#define UDP_PORTANY			UINT_C(0x0000)	/* ITRON TCP/IP ɸ��	*/

#define TCP_PORT_LAST_WELL_KNOWN	UINT_C(1023)	/* Well Known ��λ�ֹ�	*/
#define TCP_PORT_FIRST_AUTO		UINT_C(1024)	/* ��ư������Ƴ����ֹ�	*/
#define TCP_PORT_LAST_AUTO		UINT_C(4999)	/* ��ư������ƽ�λ�ֹ�	*/

#define UDP_PORT_LAST_WELL_KNOWN	UINT_C(1023)	/* Well Known ��λ�ֹ�	*/
#define UDP_PORT_FIRST_AUTO		UINT_C(1024)	/* ��ư������Ƴ����ֹ�	*/
#define UDP_PORT_LAST_AUTO		UINT_C(4999)	/* ��ư������ƽ�λ�ֹ�	*/

/*
 *  IP ���ɥ쥹�����
 */

#define IPV4_ADDRANY		ULONG_C(0x00000000)	/* ITRON TCP/IP ɸ��	*/

#define IPV4_ADDR_LOOPBACK	ULONG_C(0x7f000001)
#define IPV4_ADDR_LOOPBACK_MASK	IPV4_CLASS_A_MASK
#define IPV4_ADDR_BROADCAST	ULONG_C(0xffffffff)

#define IPV4_CLASS_A(i)		(((i) & ULONG_C(0x80000000)) == 0)
#define IPV4_CLASS_A_NET	ULONG_C(0xff000000)
#define IPV4_CLASS_A_MASK	ULONG_C(0xff000000)
#define IPV4_CLASS_A_HOST	ULONG_C(0x00ffffff)

#define IPV4_CLASS_B(i)		(((i) & ULONG_C(0xc0000000)) == ULONG_C(0x80000000))
#define IPV4_CLASS_B_NET	ULONG_C(0xffff0000)
#define IPV4_CLASS_B_MASK	ULONG_C(0xffff0000)
#define IPV4_CLASS_B_HOST	ULONG_C(0x0000ffff)

#define IPV4_CLASS_C(i)		(((i) & ULONG_C(0xe0000000)) == ULONG_C(0xc0000000))
#define IPV4_CLASS_C_NET	ULONG_C(0xffffff00)
#define IPV4_CLASS_C_MASK	ULONG_C(0xffffff00)
#define IPV4_CLASS_C_HOST	ULONG_C(0x000000ff)

#define IPV4_CLASS_D(i)		(((i) & ULONG_C(0xf0000000)) == ULONG_C(0xe0000000))

#define IN4_IS_ADDR_MULTICAST(i)	IPV4_CLASS_D(i)

#define MAKE_IPV4_LOCAL_BROADCAST(i)	(IPV4_CLASS_A(i)?((i)|IPV4_CLASS_A_HOST):\
					 IPV4_CLASS_B(i)?((i)|IPV4_CLASS_B_HOST):\
					 IPV4_CLASS_C(i)?((i)|IPV4_CLASS_C_HOST):\
					 IPV4_ADDR_BROADCAST)

#define MAKE_IPV4_LOCAL_MASK(i)		(IPV4_CLASS_A(i)?IPV4_CLASS_A_MASK:\
					 IPV4_CLASS_B(i)?IPV4_CLASS_B_MASK:\
					 IPV4_CLASS_C(i)?IPV4_CLASS_C_MASK:\
					 IPV4_ADDRANY)

#define MAKE_IPV4_ADDR(a,b,c,d)		((T_IN4_ADDR)(((uint32_t)(a)<<24)|((uint32_t)(b)<<16)|((uint32_t)(c)<<8)|(d)))

/*
 *  ưŪ������ TCP �̿�ü��
 */

typedef struct t_tcp_ccep {
	/* ɸ�� */
	ATR		cepatr;		/* �̿�ü��°��			*/
	void		*sbuf;		/* �����ѥ�����ɥХåե�	*/
	int_t		sbufsz;		/* �����ѥ�����ɥХåե�������	*/
	void		*rbuf;		/* �����ѥ�����ɥХåե�	*/
	int_t		rbufsz;		/* �����ѥ�����ɥХåե�������	*/
	/* ������¸ */
	} T_TCP_CCEP;

/*
 *  ưŪ������ TCP ���ո�
 */

#if defined(SUPPORT_INET4)

typedef struct t_tcp_crep {
	/* ɸ�� */
	ATR		repatr;		/* ���ո�°��		*/
	T_IPV4EP	myaddr;		/* ��ʬ�Υ��ɥ쥹	*/
	/* ������¸ */
	} T_TCP_CREP;

#endif	/* of #if defined(SUPPORT_INET4) */

/*
 *  ưŪ������ UDP �̿�ü��
 */

#if defined(SUPPORT_INET4)

typedef struct t_udp_ccep {
	/* ɸ�� */
	ATR		cepatr;		/* UDP �̿�ü��°��		*/
	T_IPV4EP	myaddr;		/* ��ʬ�Υ��ɥ쥹		*/
	/* ������¸ */
	} T_UDP_CCEP;

#endif	/* of #if defined(SUPPORT_INET4) */

/*
 *  IPv4 �� IPv6 �򥳥�ѥ���������򤹤뤿��Υޥ���
 */

#if defined(SUPPORT_INET4)

#define T_IN_ADDR			T_IN4_ADDR
#define T_IPEP				T_IPV4EP
#define IP_ADDRANY			IPV4_ADDRANY

#define IN_ARE_ADDR_EQUAL(n,h)		(*(n)==*(h))
#define IN_ARE_NET_ADDR_EQUAL(n,h)	(ntohl(*(n))==*(h))
#define IN_COPY_TO_NET(d,s)		(*(d)=htonl(*(s)))
#define IN_COPY_TO_HOST(d,s)		(*(d)=ntohl(*(s)))
#define IN_IS_ADDR_MULTICAST(a)		IN4_IS_ADDR_MULTICAST(*(a))
#define IN_IS_NET_ADDR_MULTICAST(a)	IN4_IS_ADDR_MULTICAST(ntohl(*(a)))
#define IN_IS_ADDR_ANY(a)		(*(a)==IPV4_ADDRANY)

#endif	/* of #if defined(SUPPORT_INET4) */

/*
 *  TINET �ȼ� API
 */

extern const char *in_strtfn (FN fncd);

#if defined(SUPPORT_INET4)

extern ER in4_add_ifaddr (T_IN4_ADDR addr, T_IN4_ADDR mask);
extern ER in4_add_route (int_t index, T_IN4_ADDR target,
                                      T_IN4_ADDR mask, T_IN4_ADDR gateway);
extern char *ip2str (char *buf, const T_IN4_ADDR *p_ipaddr);
extern const T_IN4_ADDR *in4_get_ifaddr (int_t index);

#endif	/* of #if defined(SUPPORT_INET4) */

#endif	/* of #ifndef _IN_H_ */
