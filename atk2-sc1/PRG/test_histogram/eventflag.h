/*
 *		���٥�ȥե饰��ǽ
 */

#ifndef TOPPERS_EVENTFLAG_H
#define TOPPERS_EVENTFLAG_H

/*
 *  ���٥�ȥե饰ID���饤�٥�ȥե饰�����֥�å�����Ф�����Υޥ���
 */
#define get_flgcb(flgid)	(&(flgcb_table[(flgid)]))

/*
 *  ���٥�ȥե饰������֥�å�
 */
typedef struct eventflag_initialization_block {
	ATR				flgatr;         /* ���٥�ȥե饰°�� */
	FLGPTN			iflgptn;        /* ���٥�ȥե饰�Υӥåȥѥ�����ν���� */
} FLGINIB;

/*
 *  ���٥�ȥե饰�����֥�å�
 */
typedef struct eventflag_control_block {
	const FLGINIB	*p_flginib;     /* ������֥�å��ؤΥݥ��� */
	FLGPTN			flgptn;         /* ���٥�ȥե饰���ߥѥ����� */
} FLGCB;

/*
 *  ���٥�ȥե饰��ǽ�ν����
 */
extern void eventflag_initialize(uint_t tnum_eventflag, const FLGINIB flginib_table[], FLGCB flgcb_table[]);

/*
 *  ���٥�ȥե饰�Ԥ�������Υ����å�
 */
extern bool_t check_flg_cond(FLGCB *p_flgcb, FLGPTN waiptn,
								MODE wfmode, FLGPTN *p_flgptn);

/*
 *  ���٥�ȥե饰�Υ��å�
 */
extern ER set_flg(uint_t tnum_eventflag, FLGCB flgcb_table[], ID flgid, FLGPTN setptn);

/*
 *  ���٥�ȥե饰�Υ��ꥢ
 */
extern ER clr_flg(uint_t tnum_eventflag, FLGCB flgcb_table[], ID flgid, FLGPTN clrptn);

/*
 *  ���٥�ȥե饰�Ԥ�
 */
extern ER wai_flg(uint_t tnum_eventflag, FLGCB flgcb_table[], ID flgid, FLGPTN waiptn,
					MODE wfmode, FLGPTN *p_flgptn);

#endif /* TOPPERS_EVENTFLAG_H */
