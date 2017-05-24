#ifndef _AVALON_SGDMA_DESCRIPTOR_H_
#define _AVALON_SGDMA_DESCRIPTOR_H_

/*
 *  フィールドビット定義
 */

/* control フィールド */

#define AVALON_SGDMA_DESCRIPTOR_CONTROL_GENERATE_EOP		UINT_C(0x01)
#define AVALON_SGDMA_DESCRIPTOR_CONTROL_READ_FIXED_ADDRESS	UINT_C(0x02)
#define AVALON_SGDMA_DESCRIPTOR_CONTROL_WRITE_FIXED_ADDRESS	UINT_C(0x04)
#define AVALON_SGDMA_DESCRIPTOR_CONTROL_ATLANTIC_CHANNEL	UINT_C(0x08)
#define AVALON_SGDMA_DESCRIPTOR_CONTROL_OWNED_BY_HW		UINT_C(0x80)

/* status フィールド */

#define AVALON_SGDMA_DESCRIPTOR_STATUS_E_CRC			UINT_C(0x01)
#define AVALON_SGDMA_DESCRIPTOR_STATUS_E_PARITY			UINT_C(0x02)
#define AVALON_SGDMA_DESCRIPTOR_STATUS_E_OVERFLOW		UINT_C(0x04)
#define AVALON_SGDMA_DESCRIPTOR_STATUS_E_SYNC			UINT_C(0x08)
#define AVALON_SGDMA_DESCRIPTOR_STATUS_E_UEOP			UINT_C(0x10)
#define AVALON_SGDMA_DESCRIPTOR_STATUS_E_MEOP			UINT_C(0x20)
#define AVALON_SGDMA_DESCRIPTOR_STATUS_E_MSOP			UINT_C(0x40)
#define AVALON_SGDMA_DESCRIPTOR_STATUS_TERMINATED_BY_EOP	UINT_C(0x80)

/*
 *  SGDMA ディスクリプタ構造体
 */

typedef struct t_sgdma_descriptor {
	uint32_t	*read_addr;
	uint32_t	read_addr_pad;

	uint32_t	*write_addr;
	uint32_t	write_addr_pad;

	uint32_t	*next;
	uint32_t	next_pad;

	uint16_t	bytes_to_transfer;
	uint8_t		read_burst;
	uint8_t		write_burst;

	uint16_t	actual_bytes_transferred;
	uint8_t		status;
	uint8_t		control;
	} __attribute__((packed,aligned(1)))T_SGDMA_DESCRIPTOR;

#endif	/* of #ifndef _AVALON_SGDMA_DESCRIPTOR_H_ */
