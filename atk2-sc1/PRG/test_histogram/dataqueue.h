/*
 *		�ǡ������塼��ǽ
 */

#ifndef TOPPERS_DATAQUEUE_H
#define TOPPERS_DATAQUEUE_H

/*
 *  �ǡ������塼ID����ǡ������塼�����֥�å�����Ф�����Υޥ���
 */
#define get_dtqcb(dtqid)	(&(dtqcb_table[(dtqid)]))

/*
 *  �ǡ��������֥�å�
 */
typedef struct data_management_block {
	intptr_t		data;           /* �ǡ������� */
} DTQMB;

/*
 *  �ǡ������塼������֥�å�
 */
typedef struct dataqueue_initialization_block {
	uint_t			dtqcnt;         /* �ǡ������塼������ */
	DTQMB			*p_dtqmb;       /* �ǡ������塼�����ΰ����Ƭ���� */
} DTQINIB;

/*
 *  �ǡ������塼�����֥�å�
 */
typedef struct dataqueue_control_block {
	const DTQINIB	*p_dtqinib;     /* ������֥�å��ؤΥݥ��� */
	uint_t			count;          /* �ǡ������塼��Υǡ����ο� */
	uint_t			head;           /* �ǽ�Υǡ����γ�Ǽ��� */
	uint_t			tail;           /* �Ǹ�Υǡ����γ�Ǽ���μ� */
} DTQCB;

/*
 *  �ǡ������塼��ǽ�ν����
 */
extern void dataqueue_initialize(uint_t tnum_dataqueue, const DTQINIB dtqinib_table[], DTQCB dtqcb_table[]);

/*
 *  �ǡ������塼�����ΰ�ؤΥǡ����γ�Ǽ
 */
extern void enqueue_data(DTQCB *p_dtqcb, intptr_t data);

/*
 *  �ǡ������塼�����ΰ褫��Υǡ����μ�Ф�
 */
extern void dequeue_data(DTQCB *p_dtqcb, intptr_t *p_data);

/*
 *  �ǡ������塼�ؤΥǡ�������
 */
extern bool_t send_data(DTQCB *p_dtqcb, intptr_t data);

/*
 *  �ǡ������塼����Υǡ�������
 */
extern bool_t receive_data(DTQCB *p_dtqcb, intptr_t *p_data);

/*
 *  �ǡ������塼�ؤ�����
 */
extern ER snd_dtq(uint_t tnum_dataqueue, DTQCB dtqcb_table[], ID dtqid, intptr_t data);

/*
 *  �ǡ������塼����μ���
 */
extern ER rcv_dtq(uint_t tnum_dataqueue, DTQCB dtqcb_table[], ID dtqid, intptr_t *p_data);

#endif /* TOPPERS_DATAQUEUE_H */
