/****************************************************************
* Embedded light protocol
* Set of commands used for interface implementation
*
* Alex Svetlichnyy 2021 svetlal@outlook.com
*
*
* The command protocol for user interface implementation
* 
* How to use:
*
*
* @I. A command is send (via COM-port or any other periphery) form user side to a device
* 
*
*
* Command format:
* @S0000000A#01@?40@!1234567890123456789012345678901234567890123456789012345678901234@E
* |	|       |  |   |                                                                 |  
* @S - indicates the beginning of a command
*   0000000A 	- command itself 
*           #01 - parameter
*              @?40 - char string length 64 bytes (0x40)
*                  @! - char string start
*                    1234567890123456789012345678901234567890123456789012345678901234 - char string itself
*                                                                                     @E - indicates the end of a command
*
*
* @II. Microcontroller periphery fills   elp_cmd_buffer[ELP_MAX_STR_CMD_LENGTH]   with recieved data (function uint32_t fill_elp_buffer(uint8_t *data, uint16_t length))
* fill_elp_buffer(uint8_t *data, uint16_t length) inside its routine has overflow protection and checks if a previous command was parsed. (status ELP_IDLE) 
* fill_elp_buffer(uint8_t *data, uint16_t length) also returns a result (if recieved string contains a command) 
* The result is written into    uint32_t cmd_fll     var.
*
* @III. User application (RTOS or main cycle) checks   cmd_fll    var.
* it can show, if a recieved command is pending. If this is true, the application invokes   proccess_protocol_cmd   function to process a command.
*
* @IV proccess_protocol_cmd   function processes a command, and then resets command state
*
*****************************************************************/
#ifndef EL_PROTOCOL_H_INCLUDED
#define EL_PROTOCOL_H_INCLUDED

#include "stm32f205xx.h"

/* function return values */
#define ELP_OK 			1
#define ELP_IDLE 		2
#define ELP_ERROR		0


#define ELP_QUADRO_BUF_SIZE								64 /* bytes */
#define ELP_MAX_STR_INPUT_LENGTH						64
#define ELP_MAX_STR_CMD_LENGTH							128		/* Max command length. MAX - 256 */
#define ELP_CMD_STRING_LENGTH							(uint32_t)8U		/* "FFFFFFFF" - 8 bytes */
#define ELP_CMD_START_CMD_LENGTH						2		/* @S */
#define ELP_CMD_END_CMD_LENGTH							2		/* @E */	
#define ELP_CMD_START_STRING_CMD_LENGTH					2		/* @! */
#define ELP_CMD_STRING_CMD_SIZ							4	  /* @?XX */

//#define ELP_CMD_RESET_CMD_STRING_LENGTH	/* 8+2+2=12 */	ELP_CMD_START_CMD_LENGTH + ELP_CMD_END_CMD_LENGTH + ELP_CMD_STRING_LENGTH
//#define ELP_CMD_COUNT /* 128 -8 - 2 - 2 - 2 - 4 - 64 -2 = 46 */(ELP_MAX_STR_CMD_LENGTH - ELP_CMD_STRING_LENGTH - ELP_CMD_START_CMD_LENGTH - ELP_CMD_END_CMD_LENGTH - ELP_CMD_START_STRING_CMD_LENGTH - ELP_CMD_STRING_CMD_SIZ - ELP_QUADRO_BUF_SIZE - 2 /*alignment */)
#define ELP_MAX_CMD_PARAMS 16 /* 110/2=55 */ /*(ELP_MAX_STR_CMD_LENGTH - ELP_CMD_STRING_LENGTH - ELP_CMD_START_CMD_LENGTH - ELP_CMD_END_CMD_LENGTH - ELP_CMD_START_STRING_CMD_LENGTH - ELP_CMD_STRING_CMD_SIZ)/2 */

/***************** Special symbols *****************/

/***************** Dividers symbols ****************/
#define ELP_SPECIAL_SYMBOL_DIVIDER '@'
#define ELP_SPECIAL_SYMBOL_PARAM_DIVIDER '#'
/***************** Dividers values *******************/
#define ELP_SPECIAL_SYMBOL_DIVIDER_START 'S'
#define ELP_SPECIAL_SYMBOL_DIVIDER_END 'E'

#define ELP_SPECIAL_SYMBOL_DIVIDER_START_STRING '!'
#define ELP_SPECIAL_SYMBOL_DIVIDER_STRING_SIZE '?'

/***************** Response values *******************/
#define ELP_RESPONSE_OK_SYMBOL 'O'




/* Commands enumeration */
	/* Hide in comments down from here, if not used */	
#define	ELP_CMD_IDLE																0x00000000
#define	ELP_CMD_ERROR																0x00000001			
#define	ELP_CMD_RESET																0x00000002			
#define	ELP_CMD_PRINT																0x00000003
#define	ELP_CMD_PRINT_STRING														0x00000004
#define	ELP_CMD_SPI_MEMCHIP_READ													0x00000005
#define	ELP_CMD_SPI_MEMCHIP_ERASE_SECTOR											0x00000006
#define	ELP_CMD_SPI_MEMCHIP_ERASE_CHIP												0x00000007
#define	ELP_CMD_SPI_MEMCHIP_PROGRAMM_CHIP											0x00000008
//#define	ELP_CMD_SPI_MEMCHIP_PROGRAMM_CHIP_VAL									0x00000009
//#define	ELP_CMD_SPI_MEMCHIP_INPUT_STREAM										0x0000000A
//#define	ELP_CMD_SPI_MEMCHIP_READ_DMA											0x0000000B
//#define ELP_TEST_CMD_SPI_MX25_PROGRAMM_CHIP_DMA									0x0000000C
//#define ELP_TEST_CMD_SPI_MX25_GET_RDSR											0x0000000D
#define ELP_CMD_SPI_MEMCHIP_PROGRAMM_CHIP_HEX										0x0000000E

	/* Speedometer Calibrator */
#define ELP_VEHICLE_GET_MEMORY_STATUS_STRING					0x00000A00
#define ELP_VEHICLE_GET_CURRENT_STATUS_STRING					0x00000AB0
#define ELP_VEHICLE_SAVE_STATUS									0x00000AA0
#define ELP_VEHICLE_SET_MODEL									0x00000A01
// #define ELP_VEHICLE_SET_STARTUP_SETTINGS						0x00000A02
#define ELP_VEHICLE_SET_SPEEDOMETER_MLT							0x00000A03
#define ELP_VEHICLE_SET_ODOMETER_MLT							0x00000A04
#define ELP_VEHICLE_SET_CRUISE_MLT								0x00000A05
#define ELP_VEHICLE_SET_TYRES_THRESHOLD							0x00000A06

#define ELP_VEHICLE_SET_ODOMETER_MODE							0x00000A07
#define ELP_VEHICLE_SET_TEST_MODE								0x00000A08
//#define ELP_VEHICLE_SET_MODE_B								0x00000A09

#define	ELP_TEST_GENERAL										0xFF000000
#define	ELP_MEMORY_TEST											0xFFF00000


/* An application reads a command string and converts it into a cmd structure */
/* quad_buffer allows treat a byte array as 32-bit array */
typedef union{
	uint32_t raw_data32[ELP_QUADRO_BUF_SIZE/4];
	uint8_t raw_data8[ELP_QUADRO_BUF_SIZE];
} el_quadro_buffer;


/*****************************************/
/************* Command structure *********/
/*****************************************/
typedef struct {
	uint32_t cmd;													/* actually command itself */
	uint32_t string_siz;									/* string length (if a command provides a char string) */
	el_quadro_buffer string_buffer;				/* char string itself. If data length is 4-byte divisible, an application can work faster */ 
	uint32_t param_counter;								/* how many parameters are in a command (if a command provides any parameters)  */
	uint8_t param[ELP_MAX_CMD_PARAMS];		/* parameters array (if a command provides any parameters) */
} elp_cmd;


/**** index of the first and the last bytes  of the cmd line ****/
typedef struct {
	uint8_t  firstByte;
	uint8_t  lastByte;                                                       
} elp_cmd_BufferAddress;


extern elp_cmd global_el_command; /* A global var to store a current cmd readings */

uint32_t fill_elp_buffer(uint8_t *data, uint16_t length);
uint32_t elp_parse_cmd(elp_cmd_BufferAddress addr);
uint32_t pick_cmd(elp_cmd_BufferAddress addr);
uint32_t el_reset_state(void);
elp_cmd el_get_current_cmd(void);

void elp_quadro_copy(el_quadro_buffer *qbuf_in, el_quadro_buffer *qbuf_out);

#endif /* EL_PROTOCOL_H_INCLUDED*/
