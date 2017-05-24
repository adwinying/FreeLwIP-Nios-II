/*
 *  ルーティング表
 */

#include "Os.h"

#include <tinet_defs.h>
#include <tinet_config.h>

#include <netinet/in.h>
#include <netinet/in_var.h>

/* IPv4 に関する定義 */

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

	/* 異なる LAN、default gateway による間接配送	*/
	{ 0,			0,			IPV4_ADDR_DEFAULT_GW	},

	/* 同一 LAN 内、直接配送			*/
	{ IPV4_ADDR_LOCAL &
	  IPV4_ADDR_LOCAL_MASK,	IPV4_ADDR_LOCAL_MASK,	0			},

	/* 同一 LAN 内へのブロードキャスト、直接配送	*/
	{ 0xffffffff,		0xffffffff,		0			},

	};

#endif	/* of #ifdef SUPPORT_ETHER */

#endif	/* of #ifdef SUPPORT_INET4 */

/* IPv6 に関する定義 */

#ifdef SUPPORT_INET6

#if NUM_ROUTE_ENTRY > 0

T_IN6_RTENTRY routing_tbl[NUM_ROUTE_ENTRY] = { };

#endif	/* of #if NUM_ROUTE_ENTRY > 0 */

#endif	/* of #ifdef SUPPORT_INET6 */
