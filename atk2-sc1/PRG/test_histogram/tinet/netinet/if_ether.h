#ifndef _IF_ETHER_H_
#define _IF_ETHER_H_

#ifdef SUPPORT_ETHER

/*
 *  Ethernet Address Resolution Protocol.
 *
 *  RFC 826 参照
 */

typedef struct t_ether_arp_hdr {
	uint8_t		shost[ETHER_ADDR_LEN];	/* 送信ホストの物理アドレス	*/
	uint8_t		sproto[IPV4_ADDR_LEN];	/* 送信ホストの IP アドレス	*/
	uint8_t		thost[ETHER_ADDR_LEN];	/* 解決対象物理アドレス		*/
	uint8_t		tproto[IPV4_ADDR_LEN];	/* 解決対象 IP アドレス		*/
	} T_ETHER_ARP_HDR;

#define ETHER_ARP_HDR_SIZE		(sizeof(T_ETHER_ARP_HDR))
#define IF_ARP_ETHER_HDR_SIZE		(IF_HDR_SIZE + ARP_HDR_SIZE + ETHER_ARP_HDR_SIZE)

#define GET_ETHER_ARP_HDR(nbuf)		((T_ETHER_ARP_HDR*)((nbuf)->buf + GET_IF_ARP_HDR_SIZE(nbuf)))

/*
 *  ARP キャッシュエントリ
 */

typedef struct arp_entry {
	T_NET_BUF	*hold;		/* ARP の解決待ち出力フレーム	*/
	T_IN4_ADDR	ip_addr;	/* IP アドレス			*/
	uint16_t	expire;		/* 破棄までの時間、0 なら未使用	*/
	uint8_t		mac_addr[ETHER_ADDR_LEN];	/* MAC アドレス	*/
	} T_ARP_ENTRY;

/*
 *  関数
 */

extern ER arp_request (T_IF_ADDR *ifaddr, T_IN4_ADDR dst);
extern void arp_input (T_IF_ADDR *ifaddr, T_NET_BUF *input);
extern bool_t arp_resolve (T_IF_ADDR *ifaddr, T_NET_BUF *output, T_IN4_ADDR gw);
extern void arp_init (void);
extern const T_ARP_ENTRY *arp_get_cache (void);

/*
 *  TINET 独自 API
 */

#ifdef ARP_CFG_CALLBACK_DUPLICATED

extern bool_t arp_callback_duplicated(uint8_t *shost);

#endif	/* of #ifdef ARP_CFG_CALLBACK_DUPLICATED */

#endif	/* of #ifdef SUPPORT_ETHER */

#endif	/* of #ifndef _IF_ETHER_H_ */
