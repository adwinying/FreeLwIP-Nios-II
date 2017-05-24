#ifndef _AVALON_SGDMA_H_
#define _AVALON_SGDMA_H_

#include "alt_errno.h"

#include "avalon_sgdma_regs.h"
#include "avalon_sgdma_descriptor.h"

#if !defined(TOPPERS_MACRO_ONLY) && !defined(_MACRO_ONLY)

/*
 *  ´Ø¿ô
 */

extern int_t avalon_sgdma_do_async_transfer(T_SGDMA_CSR *csr, T_SGDMA_DESCRIPTOR *desc);
extern void avalon_sgdma_construct_descriptor(
	T_SGDMA_DESCRIPTOR	*desc,
	T_SGDMA_DESCRIPTOR	*next,
	uint32_t		*read_addr,
	uint32_t		*write_addr,
	uint16_t		length_or_eop,
	int_t			generate_eop,
	int_t			read_fixed,
	int_t			write_fixed_or_sop);
extern void avalon_sgdma_init(T_SGDMA_CSR *csr);

#endif /* #if !defined(TOPPERS_MACRO_ONLY) && !defined(_MACRO_ONLY) */

#endif	/* of #ifndef _AVALON_SGDMA_H_ */
