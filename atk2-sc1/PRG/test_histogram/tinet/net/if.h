#ifndef _IF_H_
#define _IF_H_

/*
 *  ネットワークインタフェース用タイマの定義
 */

#define IF_TIMER_TMO		NET_TIMER_HZ	/* タイムアウト、1 [s]		*/
#define IF_TIMER_HZ		1		/* タイマの周波数、1 [s]	*/

/*
 *  ネットワークインタフェースのフラグ
 */

#define IFF_UP			UINT_C(0x0001)	/* 起動済み		*/
#define IFF_RUNNING		UINT_C(0x0002)	/* 動作中		*/

/*
 *  汎用定義
 */

#define IF_HDR_SIZE		(sizeof(T_IF_HDR))
#define IF_PDU_SIZE		(IF_HDR_SIZE + IF_MTU)	/* Protocol Data Unit */

/*
 *  関数
 */

#ifdef SUPPORT_ETHER

extern void ifinit (void);

#else	/* of #ifdef SUPPORT_ETHER */

#define ifinit

#endif	/* of #ifdef SUPPORT_ETHER */

#endif	/* of #ifndef _IF_H_ */
