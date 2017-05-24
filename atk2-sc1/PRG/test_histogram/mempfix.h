/*
 *		����Ĺ����ס��뵡ǽ
 */

#ifndef TOPPERS_MEMPFIX_H
#define TOPPERS_MEMPFIX_H

/*
 *  �����ΰ���ݤΤ���η����
 */
typedef intptr_t	MPF_T;          /* ����Ĺ����ס����ΰ����ݤ��뤿��η� */

/*
 *  �����ΰ���ݤΤ���Υޥ���
 */
#define COUNT_MPF_T(blksz)	(TOPPERS_COUNT_SZ((blksz), sizeof(MPF_T)))
#define ROUND_MPF_T(blksz)	(TOPPERS_ROUND_SZ((blksz), sizeof(MPF_T)))

/*
 *  �ü�ʥ���ǥå����ͤ����
 */
#define INDEX_NULL		(~0U)       /* �����֥�å��ꥹ�ȤκǸ� */
#define INDEX_ALLOC		(~1U)       /* �����ƺѤߤΥ֥�å� */

/*
 *  ����Ĺ����ס���ID�������Ĺ����ס�������֥�å�����Ф�����Υޥ���
 */
#define get_mpfcb(mpfid)	(&(mpfcb_table[(mpfid)]))

/*
 *  ����Ĺ����֥�å������֥�å�
 */
typedef struct fixed_memoryblock_management_block {
	uint_t			next;           /* ����̤�����ƥ֥�å� */
} MPFMB;

/*
 *  ����Ĺ����ס��������֥�å�
 */
typedef struct fixed_memorypool_initialization_block {
	uint_t			blkcnt;         /* ����֥�å��� */
	uint_t			blksz;          /* ����֥�å��Υ������ʴݤ᤿�͡� */
	void			*mpf;           /* ����Ĺ����ס����ΰ����Ƭ���� */
	MPFMB			*p_mpfmb;       /* ����Ĺ����ס�������ΰ����Ƭ���� */
} MPFINIB;

/*
 *  ����Ĺ����ס�������֥�å�
 */
typedef struct fixed_memorypool_control_block {
	const MPFINIB	*p_mpfinib;     /* ������֥�å��ؤΥݥ��� */
	uint_t			fblkcnt;        /* ̤�����ƥ֥�å��� */
	uint_t			unused;         /* ̤���ѥ֥�å�����Ƭ */
	uint_t			freelist;       /* ̤�����ƥ֥�å��Υꥹ�� */
} MPFCB;

/*
 *  ����Ĺ����ס��뵡ǽ�ν����
 */
extern void mempfix_initialize(uint_t tnum_mempfix, const MPFINIB mpfinib_table[], MPFCB mpfcb_table[]);

/*
 *  ����Ĺ����ס��뤫��֥�å������
 */
extern void	get_mpf_block(MPFCB *p_mpfcb, void **p_blk);

/*
 *  ����Ĺ����֥�å��γ���
 */
extern ER get_mpf(uint_t tnum_mempfix, MPFCB mpfcb_table[], ID mpfid, void **p_blk);

/*
 *  ����Ĺ����֥�å����ֵ�
 */
extern ER rel_mpf(uint_t tnum_mempfix, MPFCB mpfcb_table[], ID mpfid, void *blk);

#endif /* TOPPERS_MEMPFIX_H */
