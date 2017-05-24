/*
 *		固定長メモリプール機能
 */

#ifndef TOPPERS_MEMPFIX_H
#define TOPPERS_MEMPFIX_H

/*
 *  メモリ領域確保のための型定義
 */
typedef intptr_t	MPF_T;          /* 固定長メモリプール領域を確保するための型 */

/*
 *  メモリ領域確保のためのマクロ
 */
#define COUNT_MPF_T(blksz)	(TOPPERS_COUNT_SZ((blksz), sizeof(MPF_T)))
#define ROUND_MPF_T(blksz)	(TOPPERS_ROUND_SZ((blksz), sizeof(MPF_T)))

/*
 *  特殊なインデックス値の定義
 */
#define INDEX_NULL		(~0U)       /* 空きブロックリストの最後 */
#define INDEX_ALLOC		(~1U)       /* 割当て済みのブロック */

/*
 *  固定長メモリプールIDから固定長メモリプール管理ブロックを取り出すためのマクロ
 */
#define get_mpfcb(mpfid)	(&(mpfcb_table[(mpfid)]))

/*
 *  固定長メモリブロック管理ブロック
 */
typedef struct fixed_memoryblock_management_block {
	uint_t			next;           /* 次の未割当てブロック */
} MPFMB;

/*
 *  固定長メモリプール初期化ブロック
 */
typedef struct fixed_memorypool_initialization_block {
	uint_t			blkcnt;         /* メモリブロック数 */
	uint_t			blksz;          /* メモリブロックのサイズ（丸めた値） */
	void			*mpf;           /* 固定長メモリプール領域の先頭番地 */
	MPFMB			*p_mpfmb;       /* 固定長メモリプール管理領域の先頭番地 */
} MPFINIB;

/*
 *  固定長メモリプール管理ブロック
 */
typedef struct fixed_memorypool_control_block {
	const MPFINIB	*p_mpfinib;     /* 初期化ブロックへのポインタ */
	uint_t			fblkcnt;        /* 未割当てブロック数 */
	uint_t			unused;         /* 未使用ブロックの先頭 */
	uint_t			freelist;       /* 未割当てブロックのリスト */
} MPFCB;

/*
 *  固定長メモリプール機能の初期化
 */
extern void mempfix_initialize(uint_t tnum_mempfix, const MPFINIB mpfinib_table[], MPFCB mpfcb_table[]);

/*
 *  固定長メモリプールからブロックを獲得
 */
extern void	get_mpf_block(MPFCB *p_mpfcb, void **p_blk);

/*
 *  固定長メモリブロックの獲得
 */
extern ER get_mpf(uint_t tnum_mempfix, MPFCB mpfcb_table[], ID mpfid, void **p_blk);

/*
 *  固定長メモリブロックの返却
 */
extern ER rel_mpf(uint_t tnum_mempfix, MPFCB mpfcb_table[], ID mpfid, void *blk);

#endif /* TOPPERS_MEMPFIX_H */
