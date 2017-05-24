#ifndef _UDP_VAR_H_
#define _UDP_VAR_H_

/*
 *  UDP �إå����ڥ����ɥ��������ޥ���
 */

#define IP_UDP_HDR_SIZE			(IP_HDR_SIZE + UDP_HDR_SIZE)
#define IF_IP_UDP_HDR_SIZE		(IF_IP_HDR_SIZE + UDP_HDR_SIZE)
#define IF_IP_UDP_HDR_OFFSET		(IF_IP_HDR_SIZE)

#define GET_UDP_HDR(nbuf,uhoff)		((T_UDP_HDR*)((uint8_t*)((nbuf)->buf) + uhoff))
#define GET_UDP_SDU(nbuf,uhoff)		((uint8_t*)((nbuf)->buf) + uhoff+ UDP_HDR_SIZE)

#define GET_UDP_HDR_OFFSET(nbuf)	(GET_IF_IP_HDR_SIZE(nbuf))

#define GET_IP_UDP_HDR_SIZE(nbuf)	(GET_IP_HDR_SIZE(GET_IP_HDR(nbuf)) + UDP_HDR_SIZE)
#define GET_IF_IP_UDP_HDR_SIZE(nbuf)	(IF_HDR_SIZE + GET_IP_UDP_HDR_SIZE(nbuf))

/*
 *  TINET 1.1 �Ȥθߴ����Τ�������
 */

#if defined(SUPPORT_INET4)

#define IP4_UDP_HDR_SIZE		IP_UDP_HDR_SIZE
#define IF_IP4_UDP_HDR_SIZE		IF_IP_UDP_HDR_SIZE
#define GET_IP4_UDP_HDR_SIZE(nbuf)	GET_IP_UDP_HDR_SIZE(nbuf)
#define GET_IF_IP4_UDP_HDR_SIZE(nbuf)	GET_IF_IP_UDP_HDR_SIZE(nbuf)

#endif	/* of #if defined(SUPPORT_INET4) */

/*
 *  ������Хå��ؿ������
 */

typedef ER	(*t_udp_callback)(ID cepid, FN fncd, void *p_parblk);

/*
 *  UDP �̿�ü��
 */


/*
 *  IPv4 UDP �̿�ü��
 */
typedef struct t_udp4_cep {

	/*
	 * ITRON TCP/IP API��TCP �̿�ü����Ʊ������
	 */
	ATR		cepatr;		/* UDP �̿�ü��°��		*/
	T_IPV4EP	myaddr;		/* ��ʬ�Υ��ɥ쥹		*/

	/*
	 * TINET �ȼ��Υ���
	 */
	uint16_t	flags;		/* �̿�ü���ե饰		*/
	ResourceType	resid_lock;	/* �̿�ü����å�		*/
	TaskType	snd_tskid;	/* �������������̻�		*/
	TaskType	rcv_tskid;	/* �������������̻�		*/
	ID		rcvqid;		/* �󥳡���Хå��Ѽ������塼���̻� */
	T_NET_BUF	*cb_netbuf;	/* ������Хå��Ѽ����ͥåȥ���Хåե� */

#ifdef UDP_CFG_NON_BLOCKING

	T_IPV4EP 	*snd_p_dstaddr;	/* �������Υ��ɥ쥹�ؤΥݥ��� */
	T_IPV4EP 	*rcv_p_dstaddr;	/* �������Υ��ɥ쥹�ؤΥݥ��� */
	void		*snd_data;	/* �����ǡ����ΰ����Ƭ���ɥ쥹	*/
	int_t		snd_len;	/* �����ǡ����ΰ��Ĺ��		*/
	void		*rcv_data;	/* �����ǡ����ΰ����Ƭ���ɥ쥹	*/
	int_t		rcv_len;	/* �����ǡ����ΰ��Ĺ��		*/

#endif	/* of #ifdef UDP_CFG_NON_BLOCKING */

#ifdef UDP_CFG_EXTENTIONS
	ER		error;		/* �Ԥ����ȯ���������顼	*/
#endif

	} T_UDP4_CEP;

#if defined(SUPPORT_INET4)
#define T_UDP_CEP	T_UDP4_CEP
#endif

/*
 *  IPv6 UDP �̿�ü��
 */
#if 0
typedef struct t_udp6_cep {

	/*
	 * ITRON TCP/IP API��TCP �̿�ü����Ʊ������
	 */
	ATR		cepatr;		/* UDP �̿�ü��°��		*/
	T_IPV6EP	myaddr;		/* ��ʬ�Υ��ɥ쥹		*/

	/*
	 * TINET �ȼ��Υ���
	 */
	uint16_t	flags;		/* �̿�ü���ե饰		*/
	ResourceType	resid_lock;	/* �̿�ü����å�		*/
	TaskType	snd_tskid;	/* �������������̻�		*/
	TaskType	rcv_tskid;	/* �������������̻�		*/
	ID		rcvqid;		/* �󥳡���Хå��Ѽ������塼���̻� */
	T_NET_BUF	*cb_netbuf;	/* ������Хå��Ѽ����ͥåȥ���Хåե� */

#ifdef UDP_CFG_NON_BLOCKING

	T_IPV6EP 	*snd_p_dstaddr;	/* �������Υ��ɥ쥹�ؤΥݥ��� */
	T_IPV6EP 	*rcv_p_dstaddr;	/* �������Υ��ɥ쥹�ؤΥݥ��� */
	void		*snd_data;	/* �����ǡ����ΰ����Ƭ���ɥ쥹	*/
	int_t		snd_len;	/* �����ǡ����ΰ��Ĺ��		*/
	void		*rcv_data;	/* �����ǡ����ΰ����Ƭ���ɥ쥹	*/
	int_t		rcv_len;	/* �����ǡ����ΰ��Ĺ��		*/

#endif	/* of #ifdef UDP_CFG_NON_BLOCKING */

#ifdef UDP_CFG_EXTENTIONS
	ER		error;		/* �Ԥ����ȯ���������顼	*/
#endif

	} T_UDP6_CEP;
#endif
#if defined(SUPPORT_INET6)
#define T_UDP_CEP	T_UDP6_CEP
#endif

/*
 *  UDP �̿�ü���ե饰
 */

#define UDP_CEP_FLG_POST_OUTPUT	UINT_C(0x0200)	/* �����򳫻Ϥ��롣		*/
#define UDP_CEP_FLG_DYNAMIC	UINT_C(0x0400)	/* ưŪ�����������ǽ�̿�ü����	*/
#define UDP_CEP_FLG_VALID	UINT_C(0x0800)	/* �����Ѥߤ�ͭ�����̿�ü����	*/

/*
 *  ���֥������� ID �κǾ��ͤ����
 */

#define	TMIN_UDP_CEPID		1	/* UDP �̿�ü�� ID �κǾ��� */

#ifdef SUPPORT_MIB

/*
 *  SNMP �� ��������١��� (MIB)
 */

typedef struct t_udp_stats {
	uint32_t	udpInDatagrams;
	uint32_t	udpNoPorts;
	uint32_t	udpInErrors;
	uint32_t	udpOutDatagrams;
} T_UDP_STATS;

#endif	/* of #ifdef SUPPORT_MIB */

/*
 *  �ؿ����ߥ�졼�����ޥ���
 */

#define VALID_UDP_CEPID(id)	(TMIN_UDP_CEPID<=(id)&&(id)<=tmax_udp_cepid)

#define INDEX_UDP_CEP(id)	((id)-TMIN_UDP_CEPID)

#define GET_UDP_CEP(id)		(&(udp_cep[INDEX_UDP_CEP(id)]))

#define GET_UDP_CEPID(cep)	((ID)(((cep)-udp_cep)+TMIN_UDP_CEPID))

#define VALID_TFN_UDP_CAN(t)	((t)==TFN_UDP_SND_DAT||(t)==TFN_UDP_RCV_DAT||\
				 (t)==TFN_UDP_ALL)

#define VALID_UDP_CEP(cep)	(((cep)->flags&UDP_CEP_FLG_VALID)!=0)
#define DYNAMIC_UDP_CEP(cep)	(((cep)->flags&UDP_CEP_FLG_DYNAMIC)!=0)

/*
 *  �����ѿ�
 */

extern T_UDP_CEP udp_cep[];
extern const ID tmax_udp_cepid;

#ifdef SUPPORT_MIB

extern T_UDP_STATS udp_stats;

#endif	/* of #ifdef SUPPORT_MIB */

/*
 *  TINET 1.3 �Ȥθߴ����Τ�������
 */

#define	TMIN_UDP_CCEPID		TMIN_UDP_CEPID
#define	TNUM_UDP_CCEPID		TNUM_UDP_CEPID

/*
 *  �ؿ�
 */

extern uint_t udp_input (T_NET_BUF **inputp, uint_t *offp, uint_t *nextp);
extern ER_UINT udp_send_data (T_UDP_CEP *cep, T_IPEP *p_dstaddr,
                              void *data, int_t len);
extern ER udp_can_snd (T_UDP_CEP *cep, ER error);
extern ER udp_can_rcv (T_UDP_CEP *cep, ER error);
extern ER udp_alloc_auto_port (T_UDP_CEP *cep);
extern ER udp_alloc_port (T_UDP_CEP *cep, uint16_t portno);

#endif	/* of #ifndef _UDP_VAR_H_ */
