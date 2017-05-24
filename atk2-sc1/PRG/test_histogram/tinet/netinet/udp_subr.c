#include <string.h>

#ifdef TARGET_KERNEL_ATK2

#include "Os.h"
#include "prc_sil.h"

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

#include <tinet_defs.h>
#include <tinet_config.h>
#include <tinet_atk2_cfg.h>

#include <net/if.h>
//#include <net/if_ppp.h>
//#include <net/if_loop.h>
#include <net/ethernet.h>
#include <net/net.h>
#include <net/net_buf.h>
#include <net/net_count.h>
//#include <net/ppp_ipcp.h>

#include <netinet/in.h>
//#include <netinet6/in6.h>
#include <netinet/in_var.h>
#include <netinet/in_itron.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/ip_icmp.h>
//#include <netinet6/in6_var.h>
//#include <netinet/ip6.h>
//#include <netinet6/ip6_var.h>
//#include <netinet6/nd6.h>
//#include <netinet/icmp6.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>

#ifdef SUPPORT_UDP

/*
 *  局所変数
 */

static uint16_t udp_port_auto = UDP_PORT_FIRST_AUTO;	/* 自動割り当て番号	*/

/*
 *  udp_alloc_auto_port -- 自動割り当てポート番号を設定する。
 */

ER
udp_alloc_auto_port (T_UDP_CEP *cep)
{
	int_t		ix;
	uint16_t	portno, portno_start;

	portno_start = udp_port_auto;
	do {
		portno = udp_port_auto ++;
		if (udp_port_auto > UDP_PORT_LAST_AUTO)
			udp_port_auto = UDP_PORT_FIRST_AUTO;

		syscall(GetResource(RES_UDP_CEP));
		for (ix = tmax_udp_cepid; ix -- > 0; ) {
			if (VALID_UDP_CEP(&udp_cep[ix]) && udp_cep[ix].myaddr.portno == portno) {
				portno = UDP_PORTANY;
				break;
				}
			}

		if (portno != UDP_PORTANY) {
			cep->myaddr.portno = portno;
			syscall(ReleaseResource(RES_UDP_CEP));
			return E_OK;
			}
		syscall(ReleaseResource(RES_UDP_CEP));

		} while (portno_start != udp_port_auto);

	return E_NOID;
	}

/*
 *  udp_alloc_port -- 指定されたポート番号を設定する。
 */

ER
udp_alloc_port (T_UDP_CEP *cep, uint16_t portno)
{
	int_t	ix;

	syscall(GetResource(RES_UDP_CEP));
	for (ix = tmax_udp_cepid; ix -- > 0; )
		if (VALID_UDP_CEP(&udp_cep[ix]) && udp_cep[ix].myaddr.portno == portno) {
			syscall(ReleaseResource(RES_UDP_CEP));
			return E_PAR;
			}
	cep->myaddr.portno = portno;
	syscall(ReleaseResource(RES_UDP_CEP));
	return E_OK;
	}

#endif	/* of #ifdef SUPPORT_UDP */
