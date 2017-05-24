/*
 *		固定長メモリプール管理モジュール
 */

#include "Os.h"
#include <tinet_kernel_defs.h>
#include "mempfix.h"

/*
 *  エラーチェック用マクロ
 */
#define CHECK_PAR(exp)		 \
	do {					 \
		if (!(exp)) {		 \
			ercd = E_PAR;	 \
			goto error_exit; \
		}					 \
	} while (false)

#define VALID_MPFID(mpfid)	((mpfid) < tnum_mempfix)

#define CHECK_MPFID(mpfid)		   \
	do {						   \
		if (!VALID_MPFID(mpfid)) { \
			ercd = E_ID;		   \
			goto error_exit;	   \
		}						   \
	} while (false)

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_GET_MPF_ENTER
#define LOG_GET_MPF_ENTER(mpfid, p_blk)
#endif /* LOG_GET_MPF_ENTER */

#ifndef LOG_GET_MPF_LEAVE
#define LOG_GET_MPF_LEAVE(ercd, blk)
#endif /* LOG_GET_MPF_LEAVE */

#ifndef LOG_REL_MPF_ENTER
#define LOG_REL_MPF_ENTER(mpfid, blk)
#endif /* LOG_REL_MPF_ENTER */

#ifndef LOG_REL_MPF_LEAVE
#define LOG_REL_MPF_LEAVE(ercd)
#endif /* LOG_REL_MPF_LEAVE */

/*
 *  固定長メモリプール機能の初期化
 */
void
mempfix_initialize(uint_t tnum_mempfix, const MPFINIB mpfinib_table[], MPFCB mpfcb_table[])
{
	uint_t	i;
	MPFCB	*p_mpfcb;

	for (i = 0U; i < tnum_mempfix; i++) {
		p_mpfcb = &(mpfcb_table[i]);
		p_mpfcb->p_mpfinib = &(mpfinib_table[i]);
		p_mpfcb->fblkcnt = p_mpfcb->p_mpfinib->blkcnt;
		p_mpfcb->unused = 0U;
		p_mpfcb->freelist = INDEX_NULL;
	}
}

/*
 *  固定長メモリプールからブロックを獲得
 */
void
get_mpf_block(MPFCB *p_mpfcb, void **p_blk)
{
	uint_t blkidx;

	if (p_mpfcb->freelist != INDEX_NULL) {
		blkidx = p_mpfcb->freelist;
		p_mpfcb->freelist = (p_mpfcb->p_mpfinib->p_mpfmb + blkidx)->next;
	}
	else {
		blkidx = p_mpfcb->unused;
		p_mpfcb->unused++;
	}
	*p_blk = (void *)((char *)(p_mpfcb->p_mpfinib->mpf)
								+ p_mpfcb->p_mpfinib->blksz * blkidx);
	p_mpfcb->fblkcnt--;
	(p_mpfcb->p_mpfinib->p_mpfmb + blkidx)->next = INDEX_ALLOC;
}

/*
 *  固定長メモリブロックの獲得
 */
ER
get_mpf(uint_t tnum_mempfix, MPFCB mpfcb_table[], ID mpfid, void **p_blk)
{
	ER		ercd = E_OK;
	MPFCB	*p_mpfcb;

	LOG_GET_MPF_ENTER(mpfid, p_blk);
	CHECK_MPFID(mpfid);
	p_mpfcb = get_mpfcb(mpfid);

	SuspendOSInterrupts();
	if (p_mpfcb->fblkcnt > 0) {
		get_mpf_block(p_mpfcb, p_blk);
	}
	else {
		ercd = E_TMOUT;
	}
	ResumeOSInterrupts();

  error_exit:
	LOG_GET_MPF_LEAVE(ercd, *p_blk);
	return(ercd);
}

/*
 *  固定長メモリブロックの返却
 */
ER
rel_mpf(uint_t tnum_mempfix, MPFCB mpfcb_table[], ID mpfid, void *blk)
{
	ER		ercd = E_OK;
	SIZE	blkoffset;
	uint_t	blkidx;
	MPFCB	*p_mpfcb;

	LOG_REL_MPF_ENTER(mpfid, blk);
	CHECK_MPFID(mpfid);
	p_mpfcb = get_mpfcb(mpfid);
	CHECK_PAR(p_mpfcb->p_mpfinib->mpf <= blk);
	blkoffset = ((char *) blk) - (char *)(p_mpfcb->p_mpfinib->mpf);
	CHECK_PAR(blkoffset % p_mpfcb->p_mpfinib->blksz == 0U);
	CHECK_PAR(blkoffset / p_mpfcb->p_mpfinib->blksz < p_mpfcb->unused);
	blkidx = (uint_t)(blkoffset / p_mpfcb->p_mpfinib->blksz);
	CHECK_PAR((p_mpfcb->p_mpfinib->p_mpfmb + blkidx)->next == INDEX_ALLOC);

	SuspendOSInterrupts();
	p_mpfcb->fblkcnt++;
	(p_mpfcb->p_mpfinib->p_mpfmb + blkidx)->next = p_mpfcb->freelist;
	p_mpfcb->freelist = blkidx;
	ResumeOSInterrupts();

  error_exit:
	LOG_REL_MPF_LEAVE(ercd);
	return(ercd);
}
