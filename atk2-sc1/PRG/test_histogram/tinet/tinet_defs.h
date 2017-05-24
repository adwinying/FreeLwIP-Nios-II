#ifndef _TINET_DEFS_H_
#define _TINET_DEFS_H_

#include "tinet_kernel_defs.h"

#ifdef SUPPORT_ETHER

#include "tinet_nic_defs.h"

#endif	/* of #ifdef SUPPORT_ETHER */

/*
 *  カーネルの識別
 */

#ifdef TARGET_KERNEL_ATK2

#include "tinet_target_defs.h"

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

#endif /* _TINET_DEFS_H_ */
