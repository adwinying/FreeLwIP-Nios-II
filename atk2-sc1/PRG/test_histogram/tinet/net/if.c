#ifdef TARGET_KERNEL_ATK2

#include "Os.h"
#include "prc_sil.h"

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

#include <tinet_defs.h>
#include <tinet_config.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/net.h>
#include <net/net_timer.h>

//#include <netinet6/in6.h>
//#include <netinet6/in6_var.h>

//#include <net/if6_var.h>

#ifdef SUPPORT_ETHER

/*
 *  �ؿ�
 */

static void if_slowtimo (void *ignore);

/*
 *  if_slowtimo -- ���󥿥ե������Υ�å��ɥå�����������
 */

static void
if_slowtimo (void *ignore)
{
	T_IF_SOFTC	*ic;

	ic = IF_ETHER_NIC_GET_SOFTC();

	if (ic->timer > 0) {
		ic->timer --;
		if (ic->timer == 0)
			IF_ETHER_NIC_WATCHDOG(ic);
		}
	timeout(if_slowtimo, NULL, IF_TIMER_TMO);
	}

/*
 *  ifinit -- ���󥿥ե����������ν����
 */

void
ifinit (void)
{
	timeout(if_slowtimo, NULL, IF_TIMER_TMO);
	}

#ifdef SUPPORT_INET6
#if 0
/*
 *  if_addmulti -- �ޥ�����㥹�ȥ��ɥ쥹����Ͽ
 */

ER
if_addmulti (T_IFNET *ifp, void *maddr, uint8_t type)
{
	T_IF_ADDR	llmaddr;
	ER		error = E_OK;
	int_t		ix;

	if (type == AT_INET6) {

		if ((ix = in6_addr2maix((T_IN6_ADDR*)maddr)) == IPV6_MADDR_IX_EXTRA)
			return E_PAR;

		/* IPv6 �ޥ�����㥹�ȥ��ɥ쥹����Ͽ���롣*/
		ifp->in_maddrs[ix] = *(T_IN6_ADDR*)maddr;

		/* ���󥿥ե������Υ��ɥ쥹���Ѵ�������Ͽ���롣*/
		error = IF_IN6_RESOLVE_MULTICAST(&llmaddr, (T_IN6_ADDR*)maddr);
		if (error != E_OK)
			return error;
		ifp->ic->maddrs[ix] = llmaddr;
		}

	return IF_ADDMULTI(ifp->ic);
	}
#endif
#endif	/* of #ifdef SUPPORT_INET6 */

#endif /* of #ifdef SUPPORT_ETHER */
