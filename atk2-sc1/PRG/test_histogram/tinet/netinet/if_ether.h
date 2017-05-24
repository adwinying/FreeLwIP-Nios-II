#ifndef _IF_ETHER_H_
#define _IF_ETHER_H_

#ifdef SUPPORT_ETHER

/*
 *  Ethernet Address Resolution Protocol.
 *
 *  RFC 826 ����
 */

typedef struct t_ether_arp_hdr {
	uint8_t		shost[ETHER_ADDR_LEN];	/* �����ۥ��Ȥ�ʪ�����ɥ쥹	*/
	uint8_t		sproto[IPV4_ADDR_LEN];	/* �����ۥ��Ȥ� IP ���ɥ쥹	*/
	uint8_t		thost[ETHER_ADDR_LEN];	/* ����о�ʪ�����ɥ쥹		*/
	uint8_t		tproto[IPV4_ADDR_LEN];	/* ����о� IP ���ɥ쥹		*/
	} T_ETHER_ARP_HDR;

#define ETHER_ARP_HDR_SIZE		(sizeof(T_ETHER_ARP_HDR))
#define IF_ARP_ETHER_HDR_SIZE		(IF_HDR_SIZE + ARP_HDR_SIZE + ETHER_ARP_HDR_SIZE)

#define GET_ETHER_ARP_HDR(nbuf)		((T_ETHER_ARP_HDR*)((nbuf)->buf + GET_IF_ARP_HDR_SIZE(nbuf)))

/*
 *  ARP ����å��奨��ȥ�
 */

typedef struct arp_entry {
	T_NET_BUF	*hold;		/* ARP �β���Ԥ����ϥե졼��	*/
	T_IN4_ADDR	ip_addr;	/* IP ���ɥ쥹			*/
	uint16_t	expire;		/* �˴��ޤǤλ��֡�0 �ʤ�̤����	*/
	uint8_t		mac_addr[ETHER_ADDR_LEN];	/* MAC ���ɥ쥹	*/
	} T_ARP_ENTRY;

/*
 *  �ؿ�
 */

extern ER arp_request (T_IF_ADDR *ifaddr, T_IN4_ADDR dst);
extern void arp_input (T_IF_ADDR *ifaddr, T_NET_BUF *input);
extern bool_t arp_resolve (T_IF_ADDR *ifaddr, T_NET_BUF *output, T_IN4_ADDR gw);
extern void arp_init (void);
extern const T_ARP_ENTRY *arp_get_cache (void);

/*
 *  TINET �ȼ� API
 */

#ifdef ARP_CFG_CALLBACK_DUPLICATED

extern bool_t arp_callback_duplicated(uint8_t *shost);

#endif	/* of #ifdef ARP_CFG_CALLBACK_DUPLICATED */

#endif	/* of #ifdef SUPPORT_ETHER */

#endif	/* of #ifndef _IF_ETHER_H_ */
