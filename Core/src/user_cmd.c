/****************************************************************
* 
* User interface command parser
*
* Alex Svetlichnyy 2021 svetlal@outlook.com
*
*****************************************************************/

#include "user_cmd.h"
#include "main.h"

/*****************************************************************/
/***********************  DEMONSTRATION **************************/
/**************   The protocol command processing ****************/
/*****************************************************************/
/*****************************************************************/

extern memchip_status memory_chip_status;
extern el_quadro_buffer quadro_buf;
static uint8_t temp_spi_buf[128];

/**
* brief  After user interface command processed, send a response
* param  param
* param  
* param  
* retval 
*/

void protocol_response(uint32_t param){
	if(param == ELP_OK){
		static uint8_t response[4] = {'$','O','K','$'};
		USB_CDC_send_data(response, 4);
	}
}

/**
* brief  Process user interface command
* param  command
* param  
* param  
* retval 
*/

void process_protocol_cmd(elp_cmd *com){
		
	uint32_t response = MEMCHIP_OK;
	
	/*****************************************************************/
	/***********************  ELP_CMD_RESET **************************/
	/*****************************************************************/
	if(com->cmd == ELP_CMD_RESET){	
		el_reset_state();
		protocol_response(response);		
	}
	else if(com->cmd == ELP_MEMORY_TEST){
		el_reset_state();
		const uint32_t addr = 0x00;
		uint8_t memtest[] = "memtest_ok";
		const uint16_t length = 0x0A;
		erase_sector_memchip(&memory_chip_status, 0xFFFFFFFF, addr);
		response = programm_memchip(&memory_chip_status, addr, memtest, length, SPI_no_Dma);

		uint8_t spi_buff[128];

		memory_chip_status.memchip_state = length;

		read_memchip(&memory_chip_status, addr, spi_buff, length, SPI_no_Dma);
		USB_CDC_send_data(spi_buff, (uint16_t)length);
	}
	
	/*****************************************************************/
	/********** ELP_VEHICLE_GET_MEMORY_STATUS_STRING *****************/
	/*****************************************************************/	
	else if(com->cmd == ELP_VEHICLE_GET_MEMORY_STATUS_STRING){
		uint32_t length = STATUS_STRING_LENGTH*2;
		length |= com->param[4]; /* 01 */
		length |= (uint32_t)(com->param[3] << 8); /* 02 */
		
		uint32_t addr = 0x000000;
		memory_chip_status.memchip_state = 128;		
		read_memchip(&memory_chip_status, addr, temp_spi_buf, 128, SPI_no_Dma);
		USB_CDC_send_data(temp_spi_buf, 128);			

		el_reset_state();
	}
	
	/*****************************************************************/
	/********** ELP_VEHICLE_GET_CURRENT_STATUS_STRING ****************/
	/*****************************************************************/	
	else if(com->cmd == ELP_VEHICLE_GET_CURRENT_STATUS_STRING){
		uint8_t *status = get_vehicle_settings_data();
		uint8_t temp[VEHICLE_STATUS_SETTINGS_LENGTH*2];
		
		hexArray2asciiArray(status, temp, VEHICLE_STATUS_SETTINGS_LENGTH);
		
		USB_CDC_send_data(temp, VEHICLE_STATUS_SETTINGS_LENGTH*2);
		el_reset_state();
	}
	
	/*****************************************************************/
	/********** 			ELP_VEHICLE_SAVE_STATUS 				****************/
	/*****************************************************************/		
	else if(com->cmd == ELP_VEHICLE_SAVE_STATUS){	
		uint32_t addr = 0x000000;
		addr |= com->param[2]; /* 01 */
		addr |= (uint32_t)(com->param[1] << 8); /* 02 */
		addr |= (uint32_t)(com->param[0] << 16); /* 03 */
		/* 01 02 03 => 0x00030201 */
		erase_sector_memchip(&memory_chip_status, 0xFFFFFFFF, addr);		

		/* Make timeout and report the result */
		erase_timeout_memchip(MEMCHIP_ER_TIMEOUT);
		
		uint8_t *status = get_vehicle_settings_data();		
		response = programm_memchip(&memory_chip_status, addr, status, (uint16_t)VEHICLE_STATUS_SETTINGS_LENGTH, SPI_no_Dma);
		
		memory_chip_status.memchip_state = 128;		
		read_memchip(&memory_chip_status, addr, temp_spi_buf, 128, SPI_no_Dma);
		USB_CDC_send_data(temp_spi_buf, 128);
		
		el_reset_state();
	}	
	
	/*****************************************************************/
	/********** 				ELP_VEHICLE_SET_MODEL 				****************/
	/*****************************************************************/	
	else if(com->cmd == ELP_VEHICLE_SET_MODEL){
		uint32_t value = str_to_uint32(&com->string_buffer.raw_data8[0]);
		value &= ~0xFFFF0000;
		set_vehicle_value(VEHICLE_SET_MODEL, value);
		protocol_response(response);
		el_reset_state();
	}
	
	/*****************************************************************/
	/********** 		ELP_VEHICLE_SET_SPEEDOMETER_MLT 	  **************/
	/*****************************************************************/		
	else if(com->cmd == ELP_VEHICLE_SET_SPEEDOMETER_MLT){
		uint32_t value = str_to_uint32(&com->string_buffer.raw_data8[0]);
		value &= ~0xFFFFFF00;
		
		set_vehicle_value(ELP_VEHICLE_SET_SPEEDOMETER_MLT, value);
		protocol_response(response);
		el_reset_state();
	}
	
	/*****************************************************************/
	/********** 		ELP_VEHICLE_SET_ODOMETER_MLT		 	  **************/
	/*****************************************************************/		
	else if(com->cmd == ELP_VEHICLE_SET_ODOMETER_MLT){
		uint32_t value = str_to_uint32(&com->string_buffer.raw_data8[0]);
		value &= ~0xFFFFFF00;
		set_vehicle_value(VEHICLE_SET_ODOMETER_MLT, value);		
		protocol_response(response);
		el_reset_state();
	}
	/*****************************************************************/
	/********** 			ELP_VEHICLE_SET_CRUISE_MLT		 	  **************/
	/*****************************************************************/	
	else if(com->cmd == ELP_VEHICLE_SET_CRUISE_MLT){
		uint32_t value = str_to_uint32(&com->string_buffer.raw_data8[0]);
		value &= ~0xFFFFFF00;
		set_vehicle_value(VEHICLE_SET_CRUISE_MLT, value);	
		protocol_response(response);		
		el_reset_state();
	}	
	/*****************************************************************/
	/********** 		ELP_VEHICLE_SET_TYRES_THRESHOLD		  **************/
	/*****************************************************************/	
	else if(com->cmd == ELP_VEHICLE_SET_TYRES_THRESHOLD){
		uint32_t value = str_to_uint32(&com->string_buffer.raw_data8[0]);
		value &= ~0xFFFFFF00;
		set_vehicle_value(VEHICLE_SET_TYRES_THRESHOLD, value);
		protocol_response(response);		
		el_reset_state();
	}	
	/*****************************************************************/
	/********** 		ELP_VEHICLE_SET_ODOMETER_MODE		 		**************/
	/*****************************************************************/		
	else if(com->cmd == ELP_VEHICLE_SET_ODOMETER_MODE){
		uint32_t value = str_to_uint32(&com->string_buffer.raw_data8[0]);
		value &= ~0xFFFFFF00;
		set_vehicle_value(VEHICLE_SET_ODOMETER_MODE, value);	
		protocol_response(response);		
		el_reset_state();
	}
	/*****************************************************************/
	/********** 			ELP_VEHICLE_SET_TEST_MODE		 			**************/
	/*****************************************************************/	
	else if(com->cmd == ELP_VEHICLE_SET_TEST_MODE){
		uint32_t value = str_to_uint32(&com->string_buffer.raw_data8[0]);
		value &= ~0xFFFFFF00;
		set_vehicle_value(VEHICLE_SET_TEST_MODE, value);	
		protocol_response(response);		
		el_reset_state();
	}

	/*****************************************************************/
	/*************************	ELP_CMD_PRINT	************************/
	/*****************************************************************/	
	else if(com->cmd == ELP_CMD_PRINT){
		uint8_t print_buffer[27] = {' ', 'c', 'o', 'm', ' ', 
			halfbyte_to_hexascii((uint8_t)(com->cmd >> 28)),
			halfbyte_to_hexascii((uint8_t)(com->cmd >> 24)),
			halfbyte_to_hexascii((uint8_t)(com->cmd >> 20)),	
			halfbyte_to_hexascii((uint8_t)(com->cmd >> 16)),	
			halfbyte_to_hexascii((uint8_t)(com->cmd >> 12)),
			halfbyte_to_hexascii((uint8_t)(com->cmd >> 8)),
			halfbyte_to_hexascii((uint8_t)(com->cmd >> 4)),
			halfbyte_to_hexascii((uint8_t)(com->cmd >> 0)),
			' ', 'p', 'a', 'r', 'a', 'm', '_', 'c', 'n', 't', ' ',
			halfbyte_to_hexascii((uint8_t)(com->param_counter >> 4)),
			halfbyte_to_hexascii((uint8_t)(com->param_counter >> 0)), ' '};
		
		USB_CDC_send_data(print_buffer, 27);
		el_reset_state();
	}
	/*****************************************************************/
	/*************************	ELP_CMD_PRINT_STRING	****************/
	/*****************************************************************/		
	else if(com->cmd == ELP_CMD_PRINT_STRING){	
		USB_CDC_send_data(&com->string_buffer.raw_data8[0], (uint16_t)com->string_siz);
		el_reset_state();
	}
	/*****************************************************************/
	/**********************	ELP_CMD_SPI_MEMCHIP_READ	****************/
	/*****************************************************************/	
	else if(com->cmd == ELP_CMD_SPI_MEMCHIP_READ){
		uint32_t length = 0x00;
		length |= com->param[4]; /* 01 */
		length |= (uint32_t)(com->param[3] << 8); /* 02 */
		
		uint32_t addr = 0x00;
		addr |= com->param[2]; /* 01 */
		addr |= (uint32_t)(com->param[1] << 8); /* 02 */
		addr |= (uint32_t)(com->param[0] << 16); /* 03 */
		/* 01 02 03 => 0x00030201 */
		uint8_t spi_buff[128];
		
		
		while(length >= 128){		
			memory_chip_status.memchip_state = 128;		
			read_memchip(&memory_chip_status, addr, spi_buff, 128, SPI_no_Dma);
			USB_CDC_send_data(spi_buff, 128);			
			length-=128;
			addr+=128;
		}
		memory_chip_status.memchip_state = length;	
		read_memchip(&memory_chip_status, addr, spi_buff, length, SPI_no_Dma);
		USB_CDC_send_data(spi_buff, (uint16_t)length);	
		
		el_reset_state();
	}
	/*****************************************************************/
	/****************	ELP_CMD_SPI_MEMCHIP_ERASE_SECTOR	**************/
	/*****************************************************************/	
	else if(com->cmd == ELP_CMD_SPI_MEMCHIP_ERASE_SECTOR){
		uint32_t addr = 0x00;
		addr |= com->param[2]; /* 01 */
		addr |= (uint32_t)(com->param[1] << 8); /* 02 */
		addr |= (uint32_t)(com->param[0] << 16); /* 03 */
		/* 01 02 03 => 0x00030201 */
		
		erase_sector_memchip(&memory_chip_status, 0xFFFFFFFF, addr);		

		/* Make timeout and report the result */
		response = erase_timeout_memchip(MEMCHIP_ER_TIMEOUT);
		el_reset_state();		
		protocol_response(response);

	}
	
	/*****************************************************************/
	/****************		ELP_CMD_SPI_MEMCHIP_ERASE_CHIP	**************/
	/*****************************************************************/		
	else if(com->cmd == ELP_CMD_SPI_MEMCHIP_ERASE_CHIP){
		/********************************************************/
		/* IMPORTANT! Full chip erase can take up to 30 seconds */
		/********************************************************/		
		erase_chip_memchip(&memory_chip_status);	
		/* Make timeout and report the result */
		response = erase_timeout_memchip(MEMCHIP_ER_TIMEOUT);
		el_reset_state();		
		protocol_response(response);
	}

	/*****************************************************************/
	/****************		ELP_CMD_SPI_MEMCHIP_PROGRAMM_CHIP	************/
	/*****************************************************************/	
	
	else if(com->cmd == ELP_CMD_SPI_MEMCHIP_PROGRAMM_CHIP){
		uint32_t addr = 0x00;
		addr |= com->param[2]; /* 01 80*/
		addr |= (uint32_t)(com->param[1] << 8); /* 02 00*/
		addr |= (uint32_t)(com->param[0] << 16); /* 03 00*/
		
		response = programm_memchip(&memory_chip_status, addr, &com->string_buffer.raw_data8[0], (uint16_t)com->string_siz, SPI_no_Dma);
		protocol_response(response);
		el_reset_state();
	}
	/*****************************************************************/
	/****************						ELP_TEST_GENERAL					************/
	/*****************************************************************/	
	else if(com->cmd == ELP_TEST_GENERAL){	  /* 0xFF000000  */
		protocol_response(response);
		el_reset_state();
	}	
	/*****************************************************************/
	/********		ELP_CMD_SPI_MEMCHIP_PROGRAMM_CHIP_HEX		**************/
	/*****************************************************************/	
	else if(com->cmd == ELP_CMD_SPI_MEMCHIP_PROGRAMM_CHIP_HEX){
		if((uint16_t)com->string_siz <= ELP_MAX_STR_INPUT_LENGTH){
			uint32_t addr = 0x00;
			addr |= com->param[2]; /* 01 80*/
			addr |= (uint32_t)(com->param[1] << 8); /* 02 00*/
			addr |= (uint32_t)(com->param[0] << 16); /* 03 00*/
			
			// (uint16_t)com->string_siz*2
			str_to_ascii(&com->string_buffer.raw_data8[0], temp_spi_buf, (uint16_t)com->string_siz);
			
			response = programm_memchip(&memory_chip_status, addr, temp_spi_buf, (uint16_t)com->string_siz/2, SPI_no_Dma);
			protocol_response(response);
			el_reset_state();
		}
	}
	
	else{
		el_reset_state();
		protocol_response(response);
	}
}
