#ifndef _IF_TSE_H_
#define _IF_TSE_H_

/*
 *  NIC の選択マクロ
 */

#define IF_ETHER_NIC_GET_SOFTC()	tse_get_softc()
#define IF_ETHER_NIC_WATCHDOG(i)	tse_watchdog(i)
#define IF_ETHER_NIC_PROBE(i)		tse_probe(i)
#define IF_ETHER_NIC_INIT(i)		tse_init(i)
#define IF_ETHER_NIC_READ(i)		tse_read(i)
#define IF_ETHER_NIC_RESET(i)		tse_reset(i)
#define IF_ETHER_NIC_START(i,o)		tse_start(i,o)

#define T_IF_ETHER_NIC_SOFTC		struct t_tse_mac_info

#if !defined(TOPPERS_MACRO_ONLY) && !defined(_MACRO_ONLY)

/*
 *  前方参照
 */

#ifndef T_IF_SOFTC_DEFINED

typedef struct t_if_softc T_IF_SOFTC;

#define T_IF_SOFTC_DEFINED

#endif	/* of #ifndef T_IF_SOFTC_DEFINED */

#ifndef T_NET_BUF_DEFINED

typedef struct t_net_buf T_NET_BUF;

#define T_NET_BUF_DEFINED

#endif	/* of #ifndef T_NET_BUF_DEFINED */

/*
 *  関数
 */

extern T_IF_SOFTC *tse_get_softc(void);
extern void tse_watchdog(T_IF_SOFTC *ic);
extern void tse_probe(T_IF_SOFTC *ic);
extern void tse_init(T_IF_SOFTC *ic);
extern void tse_reset(T_IF_SOFTC *ic);
extern T_NET_BUF *tse_read(T_IF_SOFTC *ic);
extern void tse_start(T_IF_SOFTC *ic, T_NET_BUF *output);

#endif /* #if !defined(TOPPERS_MACRO_ONLY) && !defined(_MACRO_ONLY) */

#endif	/* of #ifndef _IF_TSE_H_ */
