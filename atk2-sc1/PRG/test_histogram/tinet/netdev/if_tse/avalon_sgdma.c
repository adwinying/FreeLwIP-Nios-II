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
 *  avalon_sgdma_do_async_transfer -- ディスクリプタ・チェインのノンブロッキング転送を開始
 */

int_t
avalon_sgdma_do_async_transfer (T_SGDMA_CSR *csr, T_SGDMA_DESCRIPTOR *desc)
{
	uint32_t control;

	/* コントローラがビジーの場合、すぐにエラーを返す */
	if (sil_rew_iop((void*)&csr->status) &
	    AVALON_SGDMA_STATUS_BUSY) {
		return -EBUSY;
		}

	/* RUN ビットをクリア */
	sil_wrw_iop((void*)&csr->control,
	            sil_rew_iop((void*)&csr->control)
	            & (~AVALON_SGDMA_CONTROL_RUN));

	/* すべての status レジスタ情報をクリア */
	sil_wrw_iop((void*)&csr->status, 0xff);

	/* ディスクリプタをコントローラに送る */
	sil_wrw_iop((void*)&csr->next, (uint32_t)desc);

	/* コントローラの設定 */
	control = sil_rew_iop((void*)&csr->control);

	control |= (AVALON_SGDMA_CONTROL_IE_CHAIN_COMPLETED |
	            AVALON_SGDMA_CONTROL_IE_GLOBAL          |
	            AVALON_SGDMA_CONTROL_RUN                |
	            AVALON_SGDMA_CONTROL_STOP_DMA_ER);

	sil_wrw_iop((void*)&csr->control, control);

	return 0;
	}

/*
 *  avalon_sgdma_construct_descriptor -- SGDMA ディスクリプタ構築ルーチン
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
	/* "next" ディスクリプタの OWNED_BY_HW ビットをクリア */
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

	/* SGDMA バーストは現在はサポートされていない */
	sil_wrb_iop((void*)&desc->read_burst, 0);
	sil_wrb_iop((void*)&desc->write_burst, 0);

	/* ディスクリプタのコントロール・フィールドの設定 */
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
 *  avalon_sgdma_init -- SGDMA コントローラの初期化
 */

void
avalon_sgdma_init (T_SGDMA_CSR *csr)
{
	syslog(LOG_INFO, "CHECK   : 1 = %08x", (void*)&csr->control);
	syslog(LOG_INFO, "CHECK   : 2 = %08x", AVALON_SGDMA_CONTROL_SOFTWARERESET);
	syslog(LOG_INFO, "CHECK   : 3 = %08x", (void*)&csr->control);
	syslog(LOG_INFO, "CHECK   : 4 = %08x", (void*)&csr->status);
	/* 現在のトランザクションの停止 */
	sil_wrw_iop((void*)&csr->control,
	            AVALON_SGDMA_CONTROL_SOFTWARERESET);
	sil_wrw_iop((void*)&csr->control,
	            AVALON_SGDMA_CONTROL_SOFTWARERESET);

	/*
	 *  割り込みのディセーブル、次のディスクリプタの処理の停止、
	 *  status レジスタの内容のクリア
	 */
	sil_wrw_iop((void*)&csr->control, 0x0);
	sil_wrw_iop((void*)&csr->status, 0xff);
	}
