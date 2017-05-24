/*
 *  カーネルに関する互換性定義ファイル
 */

#ifndef _TINET_KERNEL_DEFS_H_
#define _TINET_KERNEL_DEFS_H_

#if defined(TARGET_KERNEL_ATK2)

#ifndef TOPPERS_MACRO_ONLY

/*
 *  データ型の定義
 */

typedef signed int	int_t;		/* 自然なサイズの符号付き整数 */
typedef unsigned int	uint_t;		/* 自然なサイズの符号無し整数 */

typedef signed long	long_t;		/* 長めのサイズの符号付き整数 */
typedef unsigned long	ulong_t;	/* 長めのサイズの符号無し整数 */

typedef sintptr		intptr_t;	/* ポインタまたは符号付き整数 */

typedef boolean		bool_t;		/* 真偽値 */

typedef int_t		FN;		/* 機能コード */
typedef int_t		ER;		/* エラーコード */
typedef int_t		ID;		/* オブジェクトのID番号 */
typedef uint_t		ATR;		/* オブジェクトの属性 */
typedef	uint_t		MODE;		/* サービスコールの動作モード */
typedef MemorySizeType	SIZE;		/* メモリ領域のサイズ */

typedef uint_t		RELTIM;		/* 相対時間 */
typedef	ulong_t		SYSTIM;		/* システム時刻 */

typedef int_t		ER_UINT;	/* エラーコードまたは符号無し整数 */

typedef	uint_t		FLGPTN;		/* イベントフラグのビットパターン */

#endif	/* of #ifndef TOPPERS_MACRO_ONLY */

/*
 *  整数定数を作るマクロ
 */
#ifndef ULONG_C
#define ULONG_C(val)	(val ## UL)	/* ulong_t型の定数を作るマクロ */
#endif /* ULONG_C */

/*
 *  一般定数
 */
#ifndef NULL
#define NULL		0		/* 無効ポインタ */
#endif /* NULL */

#ifndef true
#define true		1		/* 真 */
#endif /* true */

#ifndef false
#define false		0		/* 偽 */
#endif /* false */

/*
 *  メインエラーコード
 */
#define E_SYS		(-5)		/* システムエラー */
#define E_NOSPT		(-9)		/* 未サポート機能 */
#define E_RSFN		(-10)		/* 予約機能コード */
#define E_RSATR		(-11)		/* 予約属性 */
#define E_PAR		(-17)		/* パラメータエラー */
#define E_ID		(-18)		/* 不正ID番号 */
#define E_CTX		(-25)		/* コンテキストエラー */
#define E_MACV		(-26)		/* メモリアクセス違反 */
#define E_OACV		(-27)		/* オブジェクトアクセス違反 */
#define E_ILUSE		(-28)		/* サービスコール不正使用 */
#define E_NOMEM		(-33)		/* メモリ不足 */
#define E_NOID		(-34)		/* ID番号不足 */
#define E_NORES		(-35)		/* 資源不足 */
#define E_OBJ		(-41)		/* オブジェクト状態エラー */
#define E_NOEXS		(-42)		/* オブジェクト未生成 */
#define E_QOVR		(-43)		/* キューイングオーバーフロー */
#define E_RLWAI		(-49)		/* 待ち状態の強制解除 */
#define E_TMOUT		(-50)		/* ポーリング失敗またはタイムアウト */
#define E_DLT		(-51)		/* 待ちオブジェクトの削除 */
#define E_CLS		(-52)		/* 待ちオブジェクトの状態変化 */
#define E_WBLK		(-57)		/* ノンブロッキング受付け */
#define E_BOVR		(-58)		/* バッファオーバーフロー */

/*
 *  オブジェクト属性
 */
#define TA_NULL		UINT_C(0)	/* オブジェクト属性を指定しない */

/*
 *  オブジェクト属性の定義
 */
#define TA_ACT		UINT_C(0x02)	/* タスクを起動された状態で生成 */

#define TA_TPRI		UINT_C(0x01)	/* タスクの待ち行列を優先度順に */
#define TA_MPRI		UINT_C(0x02)	/* メッセージキューを優先度順に */

#define TA_WMUL		UINT_C(0x02)	/* 複数の待ちタスク */
#define TA_CLR		UINT_C(0x04)	/* イベントフラグのクリア指定 */

#define TA_STA		UINT_C(0x02)	/* 周期ハンドラを動作状態で生成 */

#define TA_NONKERNEL	UINT_C(0x02)	/* カーネル管理外の割込み */

#define TA_ENAINT	UINT_C(0x01)	/* 割込み要求禁止フラグをクリア */
#define TA_EDGE		UINT_C(0x02)	/* エッジトリガ */

#define TA_HLNG		UINT_C(0x00)	/* 高級言語用インタフェース */
#define TA_TFIFO	UINT_C(0x00)	/* タスクの待ち行列をFIFO順に */
#define TA_MFIFO	UINT_C(0x00)	/* メッセージキューをFIFO順に */
#define TA_WSGL		UINT_C(0x00)	/* 待ちタスクは1つのみ */

/*
 *  サービスコールの動作モードの定義
 */
#define TWF_ORW		UINT_C(0x01)	/* イベントフラグのOR待ち */
#define TWF_ANDW	UINT_C(0x02)	/* イベントフラグのAND待ち */

/*
 *  APIエラーログマクロ
 */
#if defined(CFG_USE_ERRORHOOK)
#define syscall(api)	(api)
#else /* !defined( CFG_USE_ERRORHOOK ) */
#define	syscall(api)							   \
	{								   \
		StatusType ercd;					   \
		ercd = api;     /* 各API実行 */				   \
		if (ercd != E_OK) {					   \
			syslog(LOG_INFO, "Error:%d", atk2_strerror(ercd)); \
		}							   \
	}
#endif /* defined( CFG_USE_ERRORHOOK ) */

#endif	/* of #if defined(TARGET_KERNEL_ATK2) */

#endif	/* of #ifndef _TINET_KERNEL_DEFS_H_ */
