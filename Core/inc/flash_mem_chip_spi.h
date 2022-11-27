#ifndef FLASH_MEM_CHIP_H_INCLUDED
#define FLASH_MEM_CHIP_H_INCLUDED

/****************************************************************
* 
*  memory chip library
*
* Alex Svetlichnyy 2021 svetlal@outlook.com
*
*
* TODO add SPI3 DMA support
*
*****************************************************************/
#include "stm32f205xx.h"
#include "spi.h"
#include "device_model.h"




	#ifdef MEMCHIP_MX25L323
#define MEMCHIP_WR_TIMEOUT MX25L323_WR_TIMEOUT 			/* programming timeout */
#define MEMCHIP_ER_TIMEOUT MX25L323_ER_TIMEOUT	/* erasing timeout */
	#endif	
	#ifdef MEMCHIP_M45PE16
#define MEMCHIP_WR_TIMEOUT M45PE16_WR_TIMEOUT 			/* programming timeout */
#define MEMCHIP_ER_TIMEOUT M45PE16_ER_TIMEOUT	/* erasing timeout */
	#endif

/******************************************************************
*							MX25L323 Commands
******************************************************************/

#define MX25L323_CMD_READ 	0x03		/* Read */
#define MX25L323_CMD_WREN 	0x06		/* Write enable */
#define MX25L323_CMD_WRDI 	0x04		/* Write disable */
#define MX25L323_CMD_SE		0x20		/* Sector erase */
#define MX25L323_CMD_PP 	0x02		/* Page programm */
#define MX25L323_CMD_CE 	0xC7		/* Chip erase */
#define MX25L323_CMD_RDSR 	0x05		/* Read status register */

#define MX25L323_RDSR_WIP 0x01		/* write in progress bit */
#define MX25L323_RDSR_WRE 0x02		/* write enable bit */

#define MX25L323_WR_TIMEOUT 0xFFFF 			/* programming timeout */
#define MX25L323_ER_TIMEOUT 0xFFFF	/* erasing timeout */

#define MX25L323_CMD_SECTOR_SIZE 			0x1000	/* Sector size 1024 bytes */
#define MX25L323_CMD_SECTOR_COUNT 			1024		/* 1024 sectors on the chip */
#define MX25L323_ADDRESS_FROM_SEC(i) 		(i*MX25L323_CMD_SECTOR_SIZE)	/* pick the start address of a sector #i */

/* service data is stored on the last page */
#define MX25L323_STREAM_DATA_SECTOR				(MX25L323_CMD_SECTOR_COUNT-1)														/* Stream service data is stored at the sector #1023 */
#define MX25L323_STREAM_DATA_ADDRESS 			MX25L323_STREAM_DATA_SECTOR * MX25L323_CMD_SECTOR_SIZE	/* Stream service data start address is 0x3FF000 */
#define MX25L323_STREAM_DATA_SIZE 				4																												/* Stream address is defined as uint32_t (4 bytes) */
/* How many address records can be stored in stream mode on the last chip sector */
/* The less count, the faster initialization on start */
#define MX25L323_STREAM_DATA_PAGE_SIZE 		8*MX25L323_STREAM_DATA_SIZE															/* With max 128 records stream data page is 512 bytes */


/******************************************************************
*							M45PE16 Commands
******************************************************************/

#define M45PE16_CMD_READ 0x03		/* Read */
#define M45PE16_CMD_WREN 0x06		/* Write enable */
#define M45PE16_CMD_WRDI 0x04		/* Write disable */
#define M45PE16_CMD_SE 	0xD8		/* Sector erase */
#define M45PE16_CMD_PP 	0x02		/* Page programm */
#define M45PE16_CMD_CE 	0xC7		/* Chip erase */
#define M45PE16_CMD_RDSR 0x05		/* Read status register */

#define M45PE16_RDSR_WIP 0x01		/* write in progress bit */
#define M45PE16_RDSR_WRE 0x02		/* write enable bit */

#define M45PE16_WR_TIMEOUT 0xFFFF 			/* programming timeout */
#define M45PE16_ER_TIMEOUT 0xFFFF	/* erasing timeout */

#define M45PE16_CMD_SECTOR_SIZE 				64000	/* Sector size 64000 bytes */
#define M45PE16_CMD_SECTOR_COUNT 				32		/* 32 sectors on the chip */
#define M45PE16_ADDRESS_FROM_SEC(i) 		(i*M45PE16_CMD_SECTOR_SIZE)	/* pick the start address of a sector #i */

/* service data is stored on the last page */
#define M45PE16_STREAM_DATA_SECTOR				(M45PE16_CMD_SECTOR_COUNT-1)							/* Stream service data is stored at the sector #31 */
#define M45PE16_STREAM_DATA_ADDRESS 			M45PE16_STREAM_DATA_SECTOR * M45PE16_CMD_SECTOR_SIZE	/* Stream service data start address is  */
#define M45PE16_STREAM_DATA_SIZE 					4													/* Stream address is defined as uint32_t (4 bytes) */
/* How many address records can be stored in stream mode on the last chip sector */
/* The less count, the faster initialization on start */
#define M45PE16_STREAM_DATA_PAGE_SIZE 		8*M45PE16_STREAM_DATA_SIZE															/* With max 128 records stream data page is 512 bytes */





/******************************************************************
*							Settings
******************************************************************/

#define VALIDATED_MAX_SIZE					0x40 /* Max chunk size that can be written along with validation */
#define VALIDATE_WRITE						1			/* validation mode */
#define FAST_WRITE							0			/* validation mode */

#define MEMCHIP_OK							1			/* return value */
#define MEMCHIP_FAIL						0			/* return value */

/************************************************/
/****** Address can be stored in the union ******/
/** and accessed as uint32_t or as a byte array */
/************************************************/
typedef union{
	uint32_t adress;
	uint8_t raw_data[4];
} u_memchip_index;

/************************************************/
/************* MemChip status ******************/
/************************************************/
typedef struct {
	u_memchip_index stream_address;					/* The first available address to write stream data (0x0000 with erased chip)*/
	u_memchip_index stream_address_location;	/* Stream index location. (Address of u_mx25l323_index stream_address). 0x3FF000  with erased chip */
	uint32_t memchip_state;											/* How many bytes are pending to read/write. If state==0, a chip is ready */
} memchip_status;

/************************************************/
/*********** MemChip command structure *********/
/************* cmd + 3-bytes address ************/
/************************************************/
typedef struct {
	uint8_t cmd_name;
	uint8_t address[3];
} memchip_cmd;

/************************************************/
/************* Memchip command union ***********/
/*** to access as byte array or as a structure **/
/************************************************/
typedef union{
	memchip_cmd cmd;
	uint8_t raw_data[4];
} u_memchip_cmd_data;

/***** read functions *******/
void read_memchip(memchip_status *status, uint32_t address, uint8_t *buff, uint32_t length, eSpiDmaMode dma_mode);

/***** erase functions *******/
uint32_t erase_sector_memchip(memchip_status *status, uint32_t sector_num, uint32_t address_input);
uint32_t erase_chip_memchip(memchip_status *status);

/***** WRI/WREN functions *******/
void write_disable_memchip(void);
void write_enable_memchip(memchip_status *status, uint32_t length);
uint32_t write_timeout_memchip(void);
uint32_t erase_timeout_memchip(uint32_t timeout);

/***** programm functions *******/
uint32_t programm_memchip(memchip_status *status, uint32_t address, uint8_t *buff_input, uint32_t length, eSpiDmaMode dma_mode);


/* to test */ uint32_t input_stream_memchip(memchip_status *status, uint8_t *buff_input, uint32_t length, uint32_t validation_mode, eSpiDmaMode dma_mode);
/* to test */ uint32_t put_data_memchip(memchip_status *status, uint32_t address, uint8_t *buff_input, uint32_t length, uint32_t validation_mode, eSpiDmaMode dma_mode);

/***** index handlers *******/
uint32_t init_memchip_status(memchip_status *status);
/* to test */ uint32_t update_stream_index(memchip_status *status_for_update, uint32_t length);
uint8_t read_status_reg_memchip(void);


/***** standardization *******/
uint8_t MX_SPI_transmit_byte(uint8_t data);
uint8_t MX_SPI_recieve_byte(void);



#endif /* FLASH_MEM_CHIP_H_INCLUDED*/
