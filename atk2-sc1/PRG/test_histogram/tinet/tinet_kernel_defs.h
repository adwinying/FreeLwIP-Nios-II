/*
 *  �����ͥ�˴ؤ���ߴ�������ե�����
 */

#ifndef _TINET_KERNEL_DEFS_H_
#define _TINET_KERNEL_DEFS_H_

#if defined(TARGET_KERNEL_ATK2)

#ifndef TOPPERS_MACRO_ONLY

/*
 *  �ǡ����������
 */

typedef signed int	int_t;		/* �����ʥ�����������դ����� */
typedef unsigned int	uint_t;		/* �����ʥ����������̵������ */

typedef signed long	long_t;		/* Ĺ��Υ�����������դ����� */
typedef unsigned long	ulong_t;	/* Ĺ��Υ����������̵������ */

typedef sintptr		intptr_t;	/* �ݥ��󥿤ޤ�������դ����� */

typedef boolean		bool_t;		/* ������ */

typedef int_t		FN;		/* ��ǽ������ */
typedef int_t		ER;		/* ���顼������ */
typedef int_t		ID;		/* ���֥������Ȥ�ID�ֹ� */
typedef uint_t		ATR;		/* ���֥������Ȥ�°�� */
typedef	uint_t		MODE;		/* �����ӥ��������ư��⡼�� */
typedef MemorySizeType	SIZE;		/* �����ΰ�Υ����� */

typedef uint_t		RELTIM;		/* ���л��� */
typedef	ulong_t		SYSTIM;		/* �����ƥ���� */

typedef int_t		ER_UINT;	/* ���顼�����ɤޤ������̵������ */

typedef	uint_t		FLGPTN;		/* ���٥�ȥե饰�Υӥåȥѥ����� */

#endif	/* of #ifndef TOPPERS_MACRO_ONLY */

/*
 *  �����������ޥ���
 */
#ifndef ULONG_C
#define ULONG_C(val)	(val ## UL)	/* ulong_t�����������ޥ��� */
#endif /* ULONG_C */

/*
 *  �������
 */
#ifndef NULL
#define NULL		0		/* ̵���ݥ��� */
#endif /* NULL */

#ifndef true
#define true		1		/* �� */
#endif /* true */

#ifndef false
#define false		0		/* �� */
#endif /* false */

/*
 *  �ᥤ�󥨥顼������
 */
#define E_SYS		(-5)		/* �����ƥ२�顼 */
#define E_NOSPT		(-9)		/* ̤���ݡ��ȵ�ǽ */
#define E_RSFN		(-10)		/* ͽ��ǽ������ */
#define E_RSATR		(-11)		/* ͽ��°�� */
#define E_PAR		(-17)		/* �ѥ�᡼�����顼 */
#define E_ID		(-18)		/* ����ID�ֹ� */
#define E_CTX		(-25)		/* ����ƥ����ȥ��顼 */
#define E_MACV		(-26)		/* ���ꥢ��������ȿ */
#define E_OACV		(-27)		/* ���֥������ȥ���������ȿ */
#define E_ILUSE		(-28)		/* �����ӥ��������������� */
#define E_NOMEM		(-33)		/* ������­ */
#define E_NOID		(-34)		/* ID�ֹ���­ */
#define E_NORES		(-35)		/* ����­ */
#define E_OBJ		(-41)		/* ���֥������Ⱦ��֥��顼 */
#define E_NOEXS		(-42)		/* ���֥�������̤���� */
#define E_QOVR		(-43)		/* ���塼���󥰥����С��ե� */
#define E_RLWAI		(-49)		/* �Ԥ����֤ζ������ */
#define E_TMOUT		(-50)		/* �ݡ���󥰼��Ԥޤ��ϥ����ॢ���� */
#define E_DLT		(-51)		/* �Ԥ����֥������Ȥκ�� */
#define E_CLS		(-52)		/* �Ԥ����֥������Ȥξ����Ѳ� */
#define E_WBLK		(-57)		/* �Υ�֥�å��󥰼��դ� */
#define E_BOVR		(-58)		/* �Хåե������С��ե� */

/*
 *  ���֥�������°��
 */
#define TA_NULL		UINT_C(0)	/* ���֥�������°������ꤷ�ʤ� */

/*
 *  ���֥�������°�������
 */
#define TA_ACT		UINT_C(0x02)	/* ��������ư���줿���֤����� */

#define TA_TPRI		UINT_C(0x01)	/* ���������Ԥ������ͥ���ٽ�� */
#define TA_MPRI		UINT_C(0x02)	/* ��å��������塼��ͥ���ٽ�� */

#define TA_WMUL		UINT_C(0x02)	/* ʣ�����Ԥ������� */
#define TA_CLR		UINT_C(0x04)	/* ���٥�ȥե饰�Υ��ꥢ���� */

#define TA_STA		UINT_C(0x02)	/* �����ϥ�ɥ��ư����֤����� */

#define TA_NONKERNEL	UINT_C(0x02)	/* �����ͥ�������γ���� */

#define TA_ENAINT	UINT_C(0x01)	/* ������׵�ػߥե饰�򥯥ꥢ */
#define TA_EDGE		UINT_C(0x02)	/* ���å��ȥꥬ */

#define TA_HLNG		UINT_C(0x00)	/* �������ѥ��󥿥ե����� */
#define TA_TFIFO	UINT_C(0x00)	/* ���������Ԥ������FIFO��� */
#define TA_MFIFO	UINT_C(0x00)	/* ��å��������塼��FIFO��� */
#define TA_WSGL		UINT_C(0x00)	/* �Ԥ���������1�ĤΤ� */

/*
 *  �����ӥ��������ư��⡼�ɤ����
 */
#define TWF_ORW		UINT_C(0x01)	/* ���٥�ȥե饰��OR�Ԥ� */
#define TWF_ANDW	UINT_C(0x02)	/* ���٥�ȥե饰��AND�Ԥ� */

/*
 *  API���顼���ޥ���
 */
#if defined(CFG_USE_ERRORHOOK)
#define syscall(api)	(api)
#else /* !defined( CFG_USE_ERRORHOOK ) */
#define	syscall(api)							   \
	{								   \
		StatusType ercd;					   \
		ercd = api;     /* ��API�¹� */				   \
		if (ercd != E_OK) {					   \
			syslog(LOG_INFO, "Error:%d", atk2_strerror(ercd)); \
		}							   \
	}
#endif /* defined( CFG_USE_ERRORHOOK ) */

#endif	/* of #if defined(TARGET_KERNEL_ATK2) */

#endif	/* of #ifndef _TINET_KERNEL_DEFS_H_ */
