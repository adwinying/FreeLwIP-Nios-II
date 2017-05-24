/*
 *		データキュー機能
 */

#ifndef TOPPERS_DATAQUEUE_H
#define TOPPERS_DATAQUEUE_H

/*
 *  データキューIDからデータキュー管理ブロックを取り出すためのマクロ
 */
#define get_dtqcb(dtqid)	(&(dtqcb_table[(dtqid)]))

/*
 *  データ管理ブロック
 */
typedef struct data_management_block {
	intptr_t		data;           /* データ本体 */
} DTQMB;

/*
 *  データキュー初期化ブロック
 */
typedef struct dataqueue_initialization_block {
	uint_t			dtqcnt;         /* データキューの容量 */
	DTQMB			*p_dtqmb;       /* データキュー管理領域の先頭番地 */
} DTQINIB;

/*
 *  データキュー管理ブロック
 */
typedef struct dataqueue_control_block {
	const DTQINIB	*p_dtqinib;     /* 初期化ブロックへのポインタ */
	uint_t			count;          /* データキュー中のデータの数 */
	uint_t			head;           /* 最初のデータの格納場所 */
	uint_t			tail;           /* 最後のデータの格納場所の次 */
} DTQCB;

/*
 *  データキュー機能の初期化
 */
extern void dataqueue_initialize(uint_t tnum_dataqueue, const DTQINIB dtqinib_table[], DTQCB dtqcb_table[]);

/*
 *  データキュー管理領域へのデータの格納
 */
extern void enqueue_data(DTQCB *p_dtqcb, intptr_t data);

/*
 *  データキュー管理領域からのデータの取出し
 */
extern void dequeue_data(DTQCB *p_dtqcb, intptr_t *p_data);

/*
 *  データキューへのデータ送信
 */
extern bool_t send_data(DTQCB *p_dtqcb, intptr_t data);

/*
 *  データキューからのデータ受信
 */
extern bool_t receive_data(DTQCB *p_dtqcb, intptr_t *p_data);

/*
 *  データキューへの送信
 */
extern ER snd_dtq(uint_t tnum_dataqueue, DTQCB dtqcb_table[], ID dtqid, intptr_t data);

/*
 *  データキューからの受信
 */
extern ER rcv_dtq(uint_t tnum_dataqueue, DTQCB dtqcb_table[], ID dtqid, intptr_t *p_data);

#endif /* TOPPERS_DATAQUEUE_H */
