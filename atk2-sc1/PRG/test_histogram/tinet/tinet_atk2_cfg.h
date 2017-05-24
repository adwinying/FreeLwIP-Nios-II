#ifndef _TINET_ATK2_CFG_H_
#define _TINET_ATK2_CFG_H_

/*
 *  TINET ��ɸ�ॷ���ƥॳ��ե�����졼�����ե�����
 */

#include "target_sysmod.h"

#include "Std_Types.h"

#include <tinet_defs.h>
#include <tinet_config.h>

#include <net/if.h>
//#include <net/if_ppp.h>
//#include <net/if_loop.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/net.h>
#include <net/net_buf.h>
#include <net/net_timer.h>
//#include <net/ppp_var.h>
#include <net/ether_var.h>
#include <netinet/in.h>
#include <netinet/ip.h>
//#include <netinet/ip6.h>
#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/udp_var.h>
//#include <netinet6/in6.h>
//#include <netinet6/ip6_var.h>

/*
 *  TCP/IP ����ե�����졼�����������줿�Υ����ͥ륪�֥������Ȥλ���
 */

#include "tinet_cfg.h"

/*
 *  TINET �ץ�ȥ��륹���å������Υ����ͥ륪�֥������Ȥλ���
 */

#include "net/net_atk2_cfg.h"

#include "net/ethernet_cfg.h"

#endif	/* of #ifndef _TINET_ATK2_CFG_H_ */
