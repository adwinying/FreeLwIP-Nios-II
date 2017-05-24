/*
 *  �롼�ƥ���ɽ
 */

#include "Os.h"

#include <tinet_defs.h>
#include <tinet_config.h>

#include <netinet/in.h>
#include <netinet/in_var.h>

/* IPv4 �˴ؤ������ */

#ifdef SUPPORT_INET4

#ifdef SUPPORT_LOOP

T_IN4_RTENTRY routing_tbl[NUM_ROUTE_ENTRY] = {
	{ 0, 0, 0 },
	};

#endif	/* of #ifdef SUPPORT_LOOP */

#ifdef SUPPORT_PPP

T_IN4_RTENTRY routing_tbl[NUM_ROUTE_ENTRY] = {
	{ 0, 0, 0 },
	};

#endif	/* of #ifdef SUPPORT_PPP */

#ifdef SUPPORT_ETHER

T_IN4_RTENTRY routing_tbl[NUM_ROUTE_ENTRY] = {

	/* �ۤʤ� LAN��default gateway �ˤ���������	*/
	{ 0,			0,			IPV4_ADDR_DEFAULT_GW	},

	/* Ʊ�� LAN �⡢ľ������			*/
	{ IPV4_ADDR_LOCAL &
	  IPV4_ADDR_LOCAL_MASK,	IPV4_ADDR_LOCAL_MASK,	0			},

	/* Ʊ�� LAN ��ؤΥ֥��ɥ��㥹�ȡ�ľ������	*/
	{ 0xffffffff,		0xffffffff,		0			},

	};

#endif	/* of #ifdef SUPPORT_ETHER */

#endif	/* of #ifdef SUPPORT_INET4 */

/* IPv6 �˴ؤ������ */

#ifdef SUPPORT_INET6

#if NUM_ROUTE_ENTRY > 0

T_IN6_RTENTRY routing_tbl[NUM_ROUTE_ENTRY] = { };

#endif	/* of #if NUM_ROUTE_ENTRY > 0 */

#endif	/* of #ifdef SUPPORT_INET6 */
