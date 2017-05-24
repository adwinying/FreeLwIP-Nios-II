#ifndef _IN_ITRON_H_
#define _IN_ITRON_H_

/*
 *  API 機能・事象コード
 */

/* TCP 関係 */

#define TFN_TCP_CRE_REP		(-0x201)
#define TFN_TCP_DEL_REP		(-0x202)
#define TFN_TCP_CRE_CEP		(-0x203)
#define TFN_TCP_DEL_CEP		(-0x204)
#define TFN_TCP_ACP_CEP		(-0x205)
#define TFN_TCP_CON_CEP		(-0x206)
#define TFN_TCP_SHT_CEP		(-0x207)
#define TFN_TCP_CLS_CEP		(-0x208)
#define TFN_TCP_SND_DAT		(-0x209)
#define TFN_TCP_RCV_DAT		(-0x20a)
#define TFN_TCP_GET_BUF		(-0x20b)
#define TFN_TCP_SND_BUF		(-0x20c)
#define TFN_TCP_RCV_BUF		(-0x20d)
#define TFN_TCP_REL_BUF		(-0x20e)
#define TFN_TCP_SND_OOB		(-0x20f)
#define TFN_TCP_RCV_OOB		(-0x210)
#define TFN_TCP_CAN_CEP		(-0x211)
#define TFN_TCP_SET_OPT		(-0x212)
#define TFN_TCP_GET_OPT		(-0x213)
#define TFN_TCP_ALL		(0)

#define TEV_TCP_RCV_OOB		(-0x201)

/* UDP 関係 */

#define TFN_UDP_CRE_CEP		(-0x221)
#define TFN_UDP_DEL_CEP		(-0x222)
#define TFN_UDP_SND_DAT		(-0x223)
#define TFN_UDP_RCV_DAT		(-0x224)
#define TFN_UDP_CAN_CEP		(-0x225)
#define TFN_UDP_SET_OPT		(-0x226)
#define TFN_UDP_GET_OPT		(-0x227)
#define TFN_UDP_ALL		(0)

#define TEV_UDP_RCV_DAT		(-0x221)

/* 一般 */

#define NADR			((void*)(-1))	/* 無効アドレス */
#define IS_PTR_DEFINED(a)	((void*)(a)!=NADR&&(void*)(a)!=NULL)
#define TCP_REP_NONE		UINT_C(0)
#define TCP_CEP_NONE		UINT_C(0)
#define UDP_CEP_NONE		UINT_C(0)

/*
 *  関数
 */

/* TCP 関係 */

/* 【標準機能】 */

extern ER	tcp_acp_cep (ID cepid, ID repid, T_IPV4EP *p_dstaddr);
extern ER	tcp_con_cep (ID cepid, T_IPV4EP *p_myaddr, T_IPV4EP *p_dstaddr);
extern ER	tcp_sht_cep (ID cepid);
extern ER	tcp_cls_cep (ID cepid);
extern ER_UINT	tcp_snd_dat (ID cepid, void *data, int_t len);
extern ER_UINT	tcp_rcv_dat (ID cepid, void *data, int_t len);
extern ER_UINT	tcp_get_buf (ID cepid, void **p_buf);
extern ER	tcp_snd_buf (ID cepid, int_t len);
extern ER_UINT	tcp_rcv_buf (ID cepid, void **p_buf);
extern ER	tcp_rel_buf (ID cepid, int_t len);
extern ER	tcp_can_cep (ID cepid, FN fncd);
#if 0
extern ER	tcp6_acp_cep (ID cepid, ID repid, T_IPV6EP *p_dstaddr);
extern ER	tcp6_con_cep (ID cepid, T_IPV6EP *p_myaddr, T_IPV6EP *p_dstaddr);
#endif
/* 【拡張機能】 */

extern ER	tcp_del_rep (ID cepid);
extern ER	tcp_cre_cep (ID cepid, T_TCP_CCEP *pk_crep);
extern ER	tcp_del_cep (ID cepid);
extern ER_UINT	tcp_snd_oob (ID cepid, void *data, int_t len);
extern ER_UINT	tcp_rcv_oob (ID cepid, void *data, int_t len);
extern ER	tcp_set_opt (ID cepid, int_t optname, void *optval, int_t optlen);
extern ER	tcp_get_opt (ID cepid, int_t optname, void *optval, int_t optlen);
extern ER	tcp_cre_rep (ID repid, T_TCP_CREP *pk_crep);
#if 0
extern ER	tcp6_cre_rep (ID repid, T_TCP6_CREP *pk_crep);
#endif
/* UDP 関係 */

/* 【標準機能】 */

extern ER_UINT	udp_snd_dat (ID cepid, T_IPV4EP *p_dstaddr, void *data, int_t len);
extern ER_UINT	udp_rcv_dat (ID cepid, T_IPV4EP *p_dstaddr, void *data, int_t len);
extern ER	udp_can_cep (ID cepid, FN fncd);
#if 0
extern ER_UINT	udp6_snd_dat (ID cepid, T_IPV6EP *p_dstaddr, void *data, int_t len);
extern ER_UINT	udp6_rcv_dat (ID cepid, T_IPV6EP *p_dstaddr, void *data, int_t len);
#endif
/* 【拡張機能】 */

extern ER	udp_del_cep (ID cepid);
extern ER	udp_set_opt (ID cepid, int_t optname, void *optval, int_t optlen);
extern ER	udp_get_opt (ID cepid, int_t optname, void *optval, int_t optlen);
extern ER	udp_cre_cep (ID cepid, T_UDP_CCEP *pk_ccep);
#if 0
extern ER	udp6_cre_cep (ID cepid, T_UDP6_CCEP *pk_ccep);
#endif
#endif	/* of #ifndef _IN_ITRON_H_ */
