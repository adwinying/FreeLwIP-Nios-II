#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#ifdef SUPPORT_ETHER

/*
 *  �ե������Ĺ
 */

#define ETHER_ADDR_LEN		6	/* Ethernet (MAC) Address	*/
#define ETHER_TYPE_LEN		2	/* Type		*/
#define ETHER_CRC_LEN		4	/* CRC		*/

/*
 *  �ե졼��Ĺ (Ethernet �إå��� CRC ��ޤ�)
 */

#define ETHER_MIN_LEN		64
#define ETHER_MAX_LEN		(IF_MTU + sizeof(T_ETHER_HDR) + ETHER_CRC_LEN)

/*
 *  Ethernet �إå�
 */

#ifndef IF_ETHER_NIC_HDR_ALIGN
#error IF_ETHER_NIC_HDR_ALIGN expected.
#endif	/* of #ifndef IF_ETHER_NIC_HDR_ALIGN */

typedef struct t_ether_header {

#if IF_ETHER_NIC_HDR_ALIGN != 0

	uint8_t		align[IF_ETHER_NIC_HDR_ALIGN];	/* ���饤��Ĵ�� */

#endif	/* of #if IF_ETHER_NIC_HDR_ALIGN != 0 */

	uint8_t		dhost[ETHER_ADDR_LEN];
	uint8_t		shost[ETHER_ADDR_LEN];
	uint16_t	type;
	} __attribute__((packed))T_ETHER_HDR;

#define GET_ETHER_HDR(nbuf)		((T_ETHER_HDR*)((nbuf)->buf))

/*
 *  MAC ���ɥ쥹�����
 */

#define ETHER_MCAST_ADDR	UINT_C(0x01)	/* �ޥ�����㥹�ȡ����ɥ쥹	*/
#define ETHER_LOCAL_ADDR	UINT_C(0x02)	/* �����롦���ɥ쥹		*/

typedef struct t_ether_addr {
	uint8_t		lladdr[ETHER_ADDR_LEN];
	} T_ETHER_ADDR;

/*
 *  Type �ե������
 */

#define ETHER_TYPE_IP		UINT_C(0x0800)	/* IPv4	*/
#define ETHER_TYPE_ARP		UINT_C(0x0806)	/* ARP	*/
#define ETHER_TYPE_IPV6		UINT_C(0x86dd)	/* IPv6	*/

/*
 *  ���󥿥ե�����������ޥ���
 */

#define T_IF_HDR		T_ETHER_HDR		/* ���󥿥ե������Υإå�			*/
#define T_IF_ADDR		T_ETHER_ADDR		/* ���󥿥ե������Υ��ɥ쥹			*/
#define IF_HDR_ALIGN		2			/* �إå��Υ��饤��ñ��				*/
#define IF_MTU			1500			/* ���󥿥ե������� MTU				*/

#define IF_OUTPUT(o,d,g)	ether_output(o,d,g)	/* ���󥿥ե������ν��ϴؿ�			*/
#define IF_RAW_OUTPUT(o)	ether_raw_output(o)	/* ���󥿥ե������ν��ϴؿ������ɥ쥹���̵��	*/
#define IF_SET_PROTO(b,p)	(GET_ETHER_HDR(b)->type = htons(p))
							/* ���󥿥ե������Υץ�ȥ�������ؿ�		*/
#define IF_SOFTC_TO_IFADDR(s)	((T_IF_ADDR*)(s)->ifaddr.lladdr)
							/* ���եȥ��������󤫤� MAC ���ɥ쥹����Ф�	*/
#define IF_GET_IFNET()		ether_get_ifnet()	/* �ͥåȥ�����󥿥ե�������¤�Τ��֤���	*/
#define IF_TYPE			IFT_ETHER		/* ���󥿡��ե������η�				*/

/* IPv4 �ط� */

#define IF_PROTO_IP		ETHER_TYPE_IP		/* ���󥿥ե������� IP �ץ�ȥ������		*/
#define IF_PROTO_ARP		ETHER_TYPE_ARP		/* ���󥿥ե������� ARP �ץ�ȥ������		*/

/* IPv6 �ط� */

#define MAX_IF_MADDR_CNT	2			/* ���󥿥ե������Υޥ�����㥹�ȥ��ɥ쥹����κ��祵����	*/
#define IF_MADDR_INIT		{ { { 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, 0, 0, 0 } } }
							/* ���󥿥ե������Υޥ�����㥹�ȥ��ɥ쥹����ν����		*/
#define IF_PROTO_IPV6		ETHER_TYPE_IPV6		/* ���󥿥ե������� IP �ץ�ȥ������				*/
#define IF_ADDMULTI(s)		IF_ETHER_NIC_ADDMULTI(s)
							/* �ޥ�����㥹�ȥ��ɥ쥹����Ͽ					*/
#define IF_IN6_NEED_CACHE(i)	(true)			/* ����õ������å������Ѥ��롣				*/
#define IF_IN6_IFID(i,a)	IF_ETHER_NIC_IN6_IFID(i,a)
							/* ���󥿥ե��������̻Ҥ�����					*/
#define IF_IN6_RESOLVE_MULTICAST(i,m)	\
				ether_in6_resolve_multicast(i,m)
							/* ���󥿥ե������Υޥ�����㥹�ȥ��ɥ쥹�ؤ��Ѵ�		*/

/*
 *  ��������
 */

#ifndef T_IFNET_DEFINED

typedef struct t_ifnet T_IFNET;

#define T_IFNET_DEFINED

#endif	/* of #ifndef T_IFNET_DEFINED */

#ifndef T_IF_SOFTC_DEFINED

typedef struct t_if_softc T_IF_SOFTC;

#define T_IF_SOFTC_DEFINED

#endif	/* of #ifndef T_IF_SOFTC_DEFINED */

#ifndef T_IN4_ADDR_DEFINED

typedef uint32_t T_IN4_ADDR;

#define T_IN4_ADDR_DEFINED

#endif	/* of #ifndef T_IN4_ADDR_DEFINED */

#ifndef T_NET_BUF_DEFINED

typedef struct t_net_buf T_NET_BUF;

#define T_NET_BUF_DEFINED

#endif	/* of #ifndef T_NET_BUF_DEFINED */

/*
 *  �ͥåȥ�����󥿥ե������˰�¸���ʤ����եȥ���������
 */

#ifdef T_IF_ETHER_NIC_SOFTC

struct t_if_softc {
	T_IF_ADDR		ifaddr;			/* �ͥåȥ�����󥿥ե������Υ��ɥ쥹	*/
	uint16_t		timer;			/* ���������ॢ����			*/
	T_IF_ETHER_NIC_SOFTC	*sc;			/* �ǥ��Х�����¸�Υ��եȥ���������	*/
	EventMaskType		mask_txb_ready;		/* �������٥�ȥޥ���			*/
	EventMaskType		mask_rxb_ready;		/* �������٥�ȥޥ���			*/

#ifdef SUPPORT_INET6

	T_IF_ADDR 	maddrs[MAX_IF_MADDR_CNT];	/* �ޥ�����㥹�ȥ��ɥ쥹�ꥹ��	*/

#endif	/* of #ifdef SUPPORT_INET6 */
	};

#endif	/* of #ifdef T_IF_ETHER_NIC_SOFTC */

#ifdef SUPPORT_INET6

/* �������� */

#ifndef T_IN6_ADDR_DEFINED

typedef struct t_in6_addr T_IN6_ADDR;

#define T_IN6_ADDR_DEFINED

#endif	/* of #ifndef T_IN6_ADDR_DEFINED */

/*
 *  �ؿ�
 */

extern ER ether_in6_resolve_multicast(T_ETHER_ADDR *ifaddr, T_IN6_ADDR *maddr);

#endif	/* of #ifdef SUPPORT_INET6 */

/*
 *  �ؿ�
 */

extern T_IFNET *ether_get_ifnet (void);
extern ER ether_output (T_NET_BUF *data, void *dst, T_IF_ADDR *gw);
extern ER ether_raw_output (T_NET_BUF *data);

#endif	/* of #ifdef SUPPORT_ETHER */

#endif	/* of #ifndef _ETHERNET_H_ */
