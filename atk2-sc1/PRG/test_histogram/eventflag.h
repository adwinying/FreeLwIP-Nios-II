/*
 *		イベントフラグ機能
 */

#ifndef TOPPERS_EVENTFLAG_H
#define TOPPERS_EVENTFLAG_H

/*
 *  イベントフラグIDからイベントフラグ管理ブロックを取り出すためのマクロ
 */
#define get_flgcb(flgid)	(&(flgcb_table[(flgid)]))

/*
 *  イベントフラグ初期化ブロック
 */
typedef struct eventflag_initialization_block {
	ATR				flgatr;         /* イベントフラグ属性 */
	FLGPTN			iflgptn;        /* イベントフラグのビットパターンの初期値 */
} FLGINIB;

/*
 *  イベントフラグ管理ブロック
 */
typedef struct eventflag_control_block {
	const FLGINIB	*p_flginib;     /* 初期化ブロックへのポインタ */
	FLGPTN			flgptn;         /* イベントフラグ現在パターン */
} FLGCB;

/*
 *  イベントフラグ機能の初期化
 */
extern void eventflag_initialize(uint_t tnum_eventflag, const FLGINIB flginib_table[], FLGCB flgcb_table[]);

/*
 *  イベントフラグ待ち解除条件のチェック
 */
extern bool_t check_flg_cond(FLGCB *p_flgcb, FLGPTN waiptn,
								MODE wfmode, FLGPTN *p_flgptn);

/*
 *  イベントフラグのセット
 */
extern ER set_flg(uint_t tnum_eventflag, FLGCB flgcb_table[], ID flgid, FLGPTN setptn);

/*
 *  イベントフラグのクリア
 */
extern ER clr_flg(uint_t tnum_eventflag, FLGCB flgcb_table[], ID flgid, FLGPTN clrptn);

/*
 *  イベントフラグ待ち
 */
extern ER wai_flg(uint_t tnum_eventflag, FLGCB flgcb_table[], ID flgid, FLGPTN waiptn,
					MODE wfmode, FLGPTN *p_flgptn);

#endif /* TOPPERS_EVENTFLAG_H */
