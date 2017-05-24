#ifdef TARGET_KERNEL_ATK2

#include "Os.h"
#include "prc_sil.h"
#include "t_syslog.h"

#endif	/* of #ifdef TARGET_KERNEL_ATK2 */

#include <tinet_defs.h>
#include <tinet_config.h>
#include <tinet_atk2_cfg.h>

#include "alt_errno.h"

#include "avalon_sgdma.h"
#include "avalon_sgdma_regs.h"

/*
 *  avalon_sgdma_do_async_transfer -- �ǥ�������ץ�����������ΥΥ�֥�å���ž���򳫻�
 */

int_t
avalon_sgdma_do_async_transfer (T_SGDMA_CSR *csr, T_SGDMA_DESCRIPTOR *desc)
{
	uint32_t control;

	/* ����ȥ��餬�ӥ����ξ�硢�����˥��顼���֤� */
	if (sil_rew_iop((void*)&csr->status) &
	    AVALON_SGDMA_STATUS_BUSY) {
		return -EBUSY;
		}

	/* RUN �ӥåȤ򥯥ꥢ */
	sil_wrw_iop((void*)&csr->control,
	            sil_rew_iop((void*)&csr->control)
	            & (~AVALON_SGDMA_CONTROL_RUN));

	/* ���٤Ƥ� status �쥸��������򥯥ꥢ */
	sil_wrw_iop((void*)&csr->status, 0xff);

	/* �ǥ�������ץ��򥳥�ȥ�������� */
	sil_wrw_iop((void*)&csr->next, (uint32_t)desc);

	/* ����ȥ�������� */
	control = sil_rew_iop((void*)&csr->control);

	control |= (AVALON_SGDMA_CONTROL_IE_CHAIN_COMPLETED |
	            AVALON_SGDMA_CONTROL_IE_GLOBAL          |
	            AVALON_SGDMA_CONTROL_RUN                |
	            AVALON_SGDMA_CONTROL_STOP_DMA_ER);

	sil_wrw_iop((void*)&csr->control, control);

	return 0;
	}

/*
 *  avalon_sgdma_construct_descriptor -- SGDMA �ǥ�������ץ����ۥ롼����
 */

void
avalon_sgdma_construct_descriptor (
	T_SGDMA_DESCRIPTOR	*desc,
	T_SGDMA_DESCRIPTOR	*next,
	uint32_t		*read_addr,
	uint32_t		*write_addr,
	uint16_t		length_or_eop,
	int_t			generate_eop,
	int_t			read_fixed,
	int_t			write_fixed_or_sop)
{
	/* "next" �ǥ�������ץ��� OWNED_BY_HW �ӥåȤ򥯥ꥢ */
	sil_wrb_iop((void*)&next->control,
	            sil_reb_iop((void*)&next->control)
	            & (~AVALON_SGDMA_DESCRIPTOR_CONTROL_OWNED_BY_HW));

	sil_wrw_iop((void*)&desc->read_addr, (uint32_t)read_addr);
	sil_wrw_iop((void*)&desc->write_addr, (uint32_t)write_addr);
	sil_wrw_iop((void*)&desc->next, (uint32_t)(uint32_t*)next);
	sil_wrw_iop((void*)&desc->read_addr_pad, 0x0);
	sil_wrw_iop((void*)&desc->write_addr_pad, 0x0);
	sil_wrw_iop((void*)&desc->next_pad, 0x0);
	sil_wrh_iop((void*)&desc->bytes_to_transfer, length_or_eop);
	sil_wrh_iop((void*)&desc->actual_bytes_transferred, 0);
	sil_wrb_iop((void*)&desc->status, 0x0);

	/* SGDMA �С����Ȥϸ��ߤϥ��ݡ��Ȥ���Ƥ��ʤ� */
	sil_wrb_iop((void*)&desc->read_burst, 0);
	sil_wrb_iop((void*)&desc->write_burst, 0);

	/* �ǥ�������ץ��Υ���ȥ��롦�ե�����ɤ����� */
	sil_wrb_iop((void*)&desc->control,
	            (AVALON_SGDMA_DESCRIPTOR_CONTROL_OWNED_BY_HW)              |
	            (generate_eop ?
	             AVALON_SGDMA_DESCRIPTOR_CONTROL_GENERATE_EOP : 0x0)       |
	            (read_fixed ?
	             AVALON_SGDMA_DESCRIPTOR_CONTROL_READ_FIXED_ADDRESS : 0x0) |
	            (write_fixed_or_sop ?
	             AVALON_SGDMA_DESCRIPTOR_CONTROL_WRITE_FIXED_ADDRESS : 0x0));
	}

/*
 *  avalon_sgdma_init -- SGDMA ����ȥ���ν����
 */

void
avalon_sgdma_init (T_SGDMA_CSR *csr)
{
	syslog(LOG_INFO, "CHECK   : 1 = %08x", (void*)&csr->control);
	syslog(LOG_INFO, "CHECK   : 2 = %08x", AVALON_SGDMA_CONTROL_SOFTWARERESET);
	syslog(LOG_INFO, "CHECK   : 3 = %08x", (void*)&csr->control);
	syslog(LOG_INFO, "CHECK   : 4 = %08x", (void*)&csr->status);
	/* ���ߤΥȥ�󥶥���������� */
	sil_wrw_iop((void*)&csr->control,
	            AVALON_SGDMA_CONTROL_SOFTWARERESET);
	sil_wrw_iop((void*)&csr->control,
	            AVALON_SGDMA_CONTROL_SOFTWARERESET);

	/*
	 *  �����ߤΥǥ������֥롢���Υǥ�������ץ��ν�������ߡ�
	 *  status �쥸���������ƤΥ��ꥢ
	 */
	sil_wrw_iop((void*)&csr->control, 0x0);
	sil_wrw_iop((void*)&csr->status, 0xff);
	}
