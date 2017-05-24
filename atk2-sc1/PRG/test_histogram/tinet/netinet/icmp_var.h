#ifndef _ICMP_VAR_H_
#define _ICMP_VAR_H_

#ifdef SUPPORT_MIB

/*
 *  SNMP の 管理情報ベース (MIB)
 */

typedef struct t_icmp_stats {
/*	uint32_t	icmpInMsgs;*/
	uint32_t	icmpInErrors;
	uint32_t	icmpInDestUnreachs;
/*	uint32_t	icmpInTimeExcds;*/
/*	uint32_t	icmpInParmProbs;*/
/*	uint32_t	icmpInSrcQuenchs;*/
	uint32_t	icmpInRedirects;
	uint32_t	icmpInEchos;
	uint32_t	icmpInEchoReps;
/*	uint32_t	icmpInTimestamps;*/
/*	uint32_t	icmpInTimestampReps;*/
/*	uint32_t	icmpInAddrMasks;*/
/*	uint32_t	icmpInAddrMaskReps;*/
	uint32_t	icmpOutMsgs;
	uint32_t	icmpOutErrors;
	uint32_t	icmpOutDestUnreachs;
/*	uint32_t	icmpOutTimeExcds;*/
/*	uint32_t	icmpOutParmProbs;*/
/*	uint32_t	icmpOutSrcQuenchs;*/
/*	uint32_t	icmpOutRedirects;*/
	uint32_t	icmpOutEchos;
	uint32_t	icmpOutEchoReps;
/*	uint32_t	icmpOutTimestamps;*/
/*	uint32_t	icmpOutTimestampReps;*/
/*	uint32_t	icmpOutAddrMasks;*/
/*	uint32_t	icmpOutAddrMaskReps;*/
} T_ICMP_STATS;

extern T_ICMP_STATS icmp_stats;

#endif	/* of #ifdef SUPPORT_MIB */

#endif	/* of #ifndef _ICMP_VAR_H_ */
