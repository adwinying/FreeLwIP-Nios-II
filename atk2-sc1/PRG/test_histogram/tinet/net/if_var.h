#ifndef _IF_VAR_H_
#define _IF_VAR_H_

#if defined(SUPPORT_INET4)

/*
 *  ネットワークインタフェース構造体
 */

struct t_ifnet {
	T_IF_SOFTC	*ic;		/* ネットワークインタフェースのソフトウェア情報	*/
	T_IN4_IFADDR	in_ifaddr;	/* IPv4 アドレス情報				*/
	};

#ifndef T_IFNET_DEFINED

typedef struct t_ifnet T_IFNET;

#define T_IFNET_DEFINED

#endif	/* of #ifndef T_IFNET_DEFINED */

#endif	/* of #if defined(SUPPORT_INET4) */

#ifdef SUPPORT_MIB

/*
 *  SNMP の 管理情報ベース (MIB)
 */

typedef struct t_if_stats {
	uint32_t	ifInOctets;
	uint32_t	ifInUcastPkts;
	uint32_t	ifInNUcastPkts;
/*	uint32_t	ifInDiscards;*/
	uint32_t	ifInErrors;
	uint32_t	ifUnknownProtos;
	uint32_t	ifOutOctets;
	uint32_t	ifOutUcastPkts;
	uint32_t	ifOutNUcastPkts;
	uint32_t	ifOutDiscards;
	uint32_t	ifOutErrors;
/*	uint32_t	ifOutQlen;*/
} T_IF_STATS;

extern T_IF_STATS if_stats;

#endif	/* of #ifdef SUPPORT_MIB */

#endif	/* of #ifndef _IF_VAR_H_ */
