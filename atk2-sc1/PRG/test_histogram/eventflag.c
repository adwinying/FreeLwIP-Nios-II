/*
 *		イベントフラグ管理モジュール
 */

#include "Os.h"
#include <tinet_kernel_defs.h>
#include "eventflag.h"

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

#define VALID_FLGID(flgid)	((flgid) < tnum_eventflag)

#define CHECK_FLGID(flgid)		   \
	do {						   \
		if (!VALID_FLGID(flgid)) { \
			ercd = E_ID;		   \
			goto error_exit;	   \
		}						   \
	} while (false)

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_SET_FLG_ENTER
#define LOG_SET_FLG_ENTER(flgid, setptn)
#endif /* LOG_SET_FLG_ENTER */

#ifndef LOG_SET_FLG_LEAVE
#define LOG_SET_FLG_LEAVE(ercd)
#endif /* LOG_SET_FLG_LEAVE */

#ifndef LOG_CLR_FLG_ENTER
#define LOG_CLR_FLG_ENTER(flgid, clrptn)
#endif /* LOG_CLR_FLG_ENTER */

#ifndef LOG_CLR_FLG_LEAVE
#define LOG_CLR_FLG_LEAVE(ercd)
#endif /* LOG_CLR_FLG_LEAVE */

#ifndef LOG_WAI_FLG_ENTER
#define LOG_WAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn)
#endif /* LOG_WAI_FLG_ENTER */

#ifndef LOG_WAI_FLG_LEAVE
#define LOG_WAI_FLG_LEAVE(ercd, flgptn)
#endif /* LOG_WAI_FLG_LEAVE */

/*
 *  イベントフラグ機能の初期化
 */
void
eventflag_initialize(uint_t tnum_eventflag, const FLGINIB flginib_table[], FLGCB flgcb_table[])
{
	uint_t	i;
	FLGCB	*p_flgcb;

	for (i = 0U; i < tnum_eventflag; i++) {
		p_flgcb = &(flgcb_table[i]);
		p_flgcb->p_flginib = &(flginib_table[i]);
		p_flgcb->flgptn = p_flgcb->p_flginib->iflgptn;
	}
}

/*
 *  イベントフラグ待ち解除条件のチェック
 */
bool_t
check_flg_cond(FLGCB *p_flgcb, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn)
{
	if ((wfmode & TWF_ORW) != 0U ? (p_flgcb->flgptn & waiptn) != 0U
									: (p_flgcb->flgptn & waiptn) == waiptn) {
		*p_flgptn = p_flgcb->flgptn;
		if ((p_flgcb->p_flginib->flgatr & TA_CLR) != 0U) {
			p_flgcb->flgptn = 0U;
		}
		return(true);
	}
	return(false);
}

/*
 *  イベントフラグのセット
 */
ER
set_flg(uint_t tnum_eventflag, FLGCB flgcb_table[], ID flgid, FLGPTN setptn)
{
	ER		ercd = E_OK;
	FLGCB	*p_flgcb;

	LOG_SET_FLG_ENTER(flgid, setptn);
	CHECK_FLGID(flgid);
	p_flgcb = get_flgcb(flgid);

	SuspendOSInterrupts();
	p_flgcb->flgptn |= setptn;
	ResumeOSInterrupts();

  error_exit:
	LOG_SET_FLG_LEAVE(ercd);
	return(ercd);
}

/*
 *  イベントフラグのクリア
 */
ER
clr_flg(uint_t tnum_eventflag, FLGCB flgcb_table[], ID flgid, FLGPTN clrptn)
{
	ER		ercd = E_OK;
	FLGCB	*p_flgcb;

	LOG_CLR_FLG_ENTER(flgid, clrptn);
	CHECK_FLGID(flgid);
	p_flgcb = get_flgcb(flgid);

	SuspendOSInterrupts();
	p_flgcb->flgptn &= clrptn;
	ResumeOSInterrupts();

  error_exit:
	LOG_CLR_FLG_LEAVE(ercd);
	return(ercd);
}

/*
 *  イベントフラグ待ち
 */
ER
wai_flg(uint_t tnum_eventflag, FLGCB flgcb_table[], ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn)
{
	ER		ercd = E_OK;
	FLGCB	*p_flgcb;

	LOG_WAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn);
	CHECK_FLGID(flgid);
	CHECK_PAR(waiptn != 0U);
	CHECK_PAR(wfmode == TWF_ORW || wfmode == TWF_ANDW);
	p_flgcb = get_flgcb(flgid);

	SuspendOSInterrupts();
	if (!check_flg_cond(p_flgcb, waiptn, wfmode, p_flgptn)) {
		ercd = E_TMOUT;
	}
	ResumeOSInterrupts();

  error_exit:
	LOG_WAI_FLG_LEAVE(ercd, *p_flgptn);
	return(ercd);
}
