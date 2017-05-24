/*
 *		データキュー管理モジュール
 */

#include "Os.h"
#include <tinet_kernel_defs.h>
#include "dataqueue.h"

/*
 *  エラーチェック用マクロ
 */
#define VALID_DTQID(dtqid)	((dtqid) < tnum_dataqueue)

#define CHECK_DTQID(dtqid)		   \
	do {						   \
		if (!VALID_DTQID(dtqid)) { \
			ercd = E_ID;		   \
			goto error_exit;	   \
		}						   \
	} while (false)

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_SND_DTQ_ENTER
#define LOG_SND_DTQ_ENTER(dtqid, data)
#endif /* LOG_SND_DTQ_ENTER */

#ifndef LOG_SND_DTQ_LEAVE
#define LOG_SND_DTQ_LEAVE(ercd)
#endif /* LOG_SND_DTQ_LEAVE */

#ifndef LOG_RCV_DTQ_ENTER
#define LOG_RCV_DTQ_ENTER(dtqid, p_data)
#endif /* LOG_RCV_DTQ_ENTER */

#ifndef LOG_RCV_DTQ_LEAVE
#define LOG_RCV_DTQ_LEAVE(ercd, data)
#endif /* LOG_RCV_DTQ_LEAVE */

/*
 *  データキュー機能の初期化
 */
void
dataqueue_initialize(uint_t tnum_dataqueue, const DTQINIB dtqinib_table[], DTQCB dtqcb_table[])
{
	uint_t	i;
	DTQCB	*p_dtqcb;

	for (i = 0U; i < tnum_dataqueue; i++) {
		p_dtqcb = &(dtqcb_table[i]);
		p_dtqcb->p_dtqinib = &(dtqinib_table[i]);
		p_dtqcb->count = 0U;
		p_dtqcb->head = 0U;
		p_dtqcb->tail = 0U;
	}
}

/*
 *  データキュー管理領域へのデータの格納
 */
void
enqueue_data(DTQCB *p_dtqcb, intptr_t data)
{
	(p_dtqcb->p_dtqinib->p_dtqmb + p_dtqcb->tail)->data = data;
	p_dtqcb->count++;
	p_dtqcb->tail++;
	if (p_dtqcb->tail >= p_dtqcb->p_dtqinib->dtqcnt) {
		p_dtqcb->tail = 0U;
	}
}

/*
 *  データキュー管理領域からのデータの取出し
 */
void
dequeue_data(DTQCB *p_dtqcb, intptr_t *p_data)
{
	*p_data = (p_dtqcb->p_dtqinib->p_dtqmb + p_dtqcb->head)->data;
	p_dtqcb->count--;
	p_dtqcb->head++;
	if (p_dtqcb->head >= p_dtqcb->p_dtqinib->dtqcnt) {
		p_dtqcb->head = 0U;
	}
}

/*
 *  データキューへのデータ送信
 */
bool_t
send_data(DTQCB *p_dtqcb, intptr_t data)
{
	if (p_dtqcb->count < p_dtqcb->p_dtqinib->dtqcnt) {
		enqueue_data(p_dtqcb, data);
		return(true);
	}
	else {
		return(false);
	}
}

/*
 *  データキューからのデータ受信
 */
bool_t
receive_data(DTQCB *p_dtqcb, intptr_t *p_data)
{
	if (p_dtqcb->count > 0U) {
		dequeue_data(p_dtqcb, p_data);
		return(true);
	}
	else {
		return(false);
	}
}

/*
 *  データキューへの送信
 */
ER
snd_dtq(uint_t tnum_dataqueue, DTQCB dtqcb_table[], ID dtqid, intptr_t data)
{
	ER		ercd = E_OK;
	DTQCB	*p_dtqcb;

	LOG_SND_DTQ_ENTER(dtqid, data);
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	SuspendOSInterrupts();
	if (send_data(p_dtqcb, data)) {
	}
	else {
		ercd = E_TMOUT;
	}
	ResumeOSInterrupts();

  error_exit:
	LOG_SND_DTQ_LEAVE(ercd);
	return(ercd);
}

/*
 *  データキューからの受信
 */
ER
rcv_dtq(uint_t tnum_dataqueue, DTQCB dtqcb_table[], ID dtqid, intptr_t *p_data)
{
	ER		ercd = E_OK;
	DTQCB	*p_dtqcb;

	LOG_RCV_DTQ_ENTER(dtqid, p_data);
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	SuspendOSInterrupts();
	if (receive_data(p_dtqcb, p_data)) {
	}
	else {
		ercd = E_TMOUT;
	}
	ResumeOSInterrupts();

  error_exit:
	LOG_RCV_DTQ_LEAVE(ercd, *p_data);
	return(ercd);
}
