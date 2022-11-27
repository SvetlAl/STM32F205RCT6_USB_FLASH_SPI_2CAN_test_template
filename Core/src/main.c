/****************************************************************
* A device based on STM32F205
*
*
* Alex Svetlichnyy 2022 svetlal@outlook.com
*
*****************************************************************/

#include "main.h"

/********************************************************************************************/
/****************************************** Vars ********************************************/
/********************************************************************************************/

static VehicleStructData theVehicle;
memchip_status memory_chip_status;
el_quadro_buffer quadro_buf;							/* Example buffer to store data to push it into DMA+SPI MX25 programming */
uint8_t dma_spi_buff[1024];								/* Example buffer to store data recieved with DMA+SPI MX25 reading */
static uint32_t cmd_fll = ELP_IDLE;						/* cmd_buffer flag. If recieved data contains a message, the flag changes its status */


/********************************************************************************************/
/************************************ Filtered CAN channel **********************************/
/********************************************************************************************/


extern void CAN1_RX0_IRQHandler(void);
void CAN1_RX0_IRQHandler(void) {
	if(CAN1->RF0R & CAN_RF0R_FMP0){
		// Recieve a new message
		can_message newMessage = Can1_Recieve_StdMessage(CAN_FIFO_0);
		
		/*************** Freeze odometer flag ***********************/
		uint8_t odo_mlt = (theVehicle._vehicleStruct.modes & CALIBRATOR_ODOMETER_MODE) ?  0x00 : theVehicle._vehicleStruct.odometer_mlt;

		/*************** ID matches vehicle's speedometer id ***********************/
		if((CAN_ID(newMessage.id.std_id.id_highbyte, newMessage.id.std_id.id_lowbyte) == theVehicle._vehicleStruct.id_speedometer) |
			(CAN_ID(newMessage.id.std_id.id_highbyte, newMessage.id.std_id.id_lowbyte) == theVehicle._vehicleStruct.id_speedometer_extra)){
				
				/*************** CALIBRATOR_TEST_MODE ON ***********************/
			if(theVehicle._vehicleStruct.modes & CALIBRATOR_TEST_MODE){
				/*************** Using different values for GW and Dodge ***********************/
				if((theVehicle._vehicleStruct.model != MODEL_GREAT_WALL_WINGLE7) & (theVehicle._vehicleStruct.model != MODEL_DODGE_RAM_2013)){
					for(uint32_t i = 0; i < newMessage.dlc; i++) newMessage.data[i] = SPEEDOMETER_TEST_VALUE;
				}
				else if (theVehicle._vehicleStruct.model == MODEL_GREAT_WALL_WINGLE7){
					for(uint32_t i = 0; i < newMessage.dlc; i++) newMessage.data[i] = SPEEDOMETER_TEST_VALUE_GW;
				}
				else if (theVehicle._vehicleStruct.model == MODEL_DODGE_RAM_2013){
					for(uint32_t i = 0; i < newMessage.dlc; i++) newMessage.data[i] = SPEEDOMETER_TEST_VALUE_GW;
				}
				
			}
			else{	
				/*************** CALIBRATOR_TEST_MODE OFF ***********************/

				/*************** is the same id used for both speedometer and odometer? ***********************/
					if(theVehicle._vehicleStruct.id_odometer == theVehicle._vehicleStruct.id_speedometer){
						theVehicle._vehicleStruct.calibrate_speedometer(&newMessage, theVehicle._vehicleStruct.speedometer_mlt, odo_mlt);
						}	
					}					
		}
		/*************** ID matches vehicle's odometer id ***********************/
		if(CAN_ID(newMessage.id.std_id.id_highbyte, newMessage.id.std_id.id_lowbyte) == theVehicle._vehicleStruct.id_odometer){
			/*************** Odometer is calibrated with a separate dedicated function? ***********************/
					if(theVehicle._vehicleStruct.id_odometer != theVehicle._vehicleStruct.id_speedometer){
						theVehicle._vehicleStruct.calibrate_odometer(&newMessage, theVehicle._vehicleStruct.speedometer_mlt, odo_mlt);
						}		
		}
		
				/*************** ID matches vehicle's cruise id ***********************/
		if(CAN_ID(newMessage.id.std_id.id_highbyte, newMessage.id.std_id.id_lowbyte) == theVehicle._vehicleStruct.id_cruise){
			theVehicle._vehicleStruct.calibrate_cruise(&newMessage, theVehicle._vehicleStruct.cruise_mlt);
		}
		
		
			/*************** EXTRA calibration for ISUZU and GW ***********************/
		if((theVehicle._vehicleStruct.model == MODEL_ISUZU_DMAX_MUX) |  (theVehicle._vehicleStruct.model == MODEL_GREAT_WALL_POER)){
			theVehicle._vehicleStruct.calibrate_extra(&newMessage, theVehicle._vehicleStruct.tyres_threshold);
		}

		if(Can2_Transmit_StdMessage(newMessage)!=ERR_CAN2_NO_EMPTY_MAILBOX){
			IWDG_reset();
		}								
	}
}


/********************************************************************************************/
/************************************ Bypass  CAN channel  **********************************/
/********************************************************************************************/

extern void CAN2_RX0_IRQHandler(void);
void CAN2_RX0_IRQHandler(void) {
	if(CAN2->RF0R & CAN_RF0R_FMP0){
		// Recieve a new message
		can_message newMessage = Can2_Recieve_StdMessage(CAN_FIFO_0);
		if(Can1_Transmit_StdMessage(newMessage)!=ERR_CAN1_NO_EMPTY_MAILBOX){
			IWDG_reset();			 
		}
	}
}

/********************************************************************************************/
/* Recieve data via USB Virtual COM-Port and check, if the recieved bytes contain a command */
/********************************************************************************************/

uint32_t USB_CDC_recieve_data(uint16_t length){
	
	uint8_t *data = EndPoint[1].rxBuffer_ptr;
	cmd_fll = fill_elp_buffer(data, length);
	return length;
}

uint8_t *get_vehicle_settings_data(){
	return &theVehicle.data[0];
}

void set_vehicle_value(uint32_t cmd, uint32_t _value){
	switch(cmd){
		case VEHICLE_SET_MODEL:
			theVehicle._vehicleStruct.model = (uint16_t)_value;
			initVehicle(&theVehicle._vehicleStruct, VEHICLE_INIT_DEFAULT);
		break;
/*		case VEHICLE_SET_STARTUP_SETTINGS:
			theVehicle._vehicleStruct.startup_settings = (uint8_t)(_value & ~0xFFFFFF00);
		break; */
		case VEHICLE_SET_ODOMETER_MODE:			
			if(theVehicle._vehicleStruct.modes & CALIBRATOR_ODOMETER_MODE){
				if(!((uint8_t)_value & CALIBRATOR_ODOMETER_MODE)){
					theVehicle._vehicleStruct.modes &= (uint8_t)~CALIBRATOR_ODOMETER_MODE;
				}
			}
			else{
				if((_value & CALIBRATOR_ODOMETER_MODE)) theVehicle._vehicleStruct.modes |= CALIBRATOR_ODOMETER_MODE;
			}
		break;
		case VEHICLE_SET_TEST_MODE:			
			if(theVehicle._vehicleStruct.modes & CALIBRATOR_TEST_MODE){
				if(!((uint8_t)_value & CALIBRATOR_TEST_MODE)){
					theVehicle._vehicleStruct.modes &= (uint8_t)~CALIBRATOR_TEST_MODE;
				}
			}
			else{
				if((_value & CALIBRATOR_TEST_MODE)) theVehicle._vehicleStruct.modes |= (uint8_t)_value;
			}
		break;
		case VEHICLE_SET_SPEEDOMETER_MLT:
			theVehicle._vehicleStruct.speedometer_mlt = (uint8_t)(_value);
		break;
		case VEHICLE_SET_ODOMETER_MLT:
			theVehicle._vehicleStruct.odometer_mlt = (uint8_t)(_value);
		break;
		case VEHICLE_SET_CRUISE_MLT:
			theVehicle._vehicleStruct.cruise_mlt = (uint8_t)(_value);
		break;
		case VEHICLE_SET_TYRES_THRESHOLD:
			theVehicle._vehicleStruct.tyres_threshold = (uint8_t)(_value);
		break;
		default:
		break;			
	}
}

int main(void){
	static volatile uint32_t timeout = 0xFFFF;
   	/* Start HSE, PLL, Flash latency, all the RCC configuration */
	ClockInit(); 	/* Enable all the Peripherial clocks */
	EnablePeripherals();
	Can1_InitializateFilters_GatewayTemplate();
	initDeviceGeneralPinout();

	USB_OTG_FS_init_pinout();
	USB_OTG_FS_init_device();
	
	/* DMA1_IRQn used if MX25L323 handling uses DMA */
	/*
	NVIC_SetPriority(DMA1_Channel2_IRQn, 8);
	NVIC_EnableIRQ(DMA1_Channel2_IRQn);
	NVIC_SetPriority(DMA1_Channel3_IRQn, 8);
	NVIC_EnableIRQ(DMA1_Channel3_IRQn);
	*/
	/* DMA1_IRQn used if MX25L323 handling uses DMA */

	NVIC_EnableIRQ(CAN1_RX0_IRQn);
	NVIC_SetPriority(CAN1_RX0_IRQn, 8);
	NVIC_EnableIRQ(CAN2_RX0_IRQn);
	NVIC_SetPriority(CAN2_RX0_IRQn, 9);
	
	init_memchip_status(&memory_chip_status);
	__enable_irq ();
	

	/********* Load vehicle settings **********/
	uint32_t length = VEHICLE_STATUS_SETTINGS_LENGTH;
	uint32_t addr = 0x000000;
	memory_chip_status.memchip_state = 128;
	
	read_memchip(&memory_chip_status, addr, theVehicle.data, length, SPI_no_Dma);
		
		
	if(theVehicle._vehicleStruct.model < MODEL_COUNT){
		initVehicle(&theVehicle._vehicleStruct, VEHICLE_INIT_PRESET);
	}
	/* If no vehicle information found, start with default settings */
	else if(theVehicle._vehicleStruct.model >= MODEL_COUNT){
		theVehicle._vehicleStruct.model = MODEL_TOYOTA_LC300;
		initVehicle(&theVehicle._vehicleStruct, VEHICLE_INIT_DEFAULT);
	}
	/* If settings preset is supposed to use ind. watchdog */
	if(theVehicle._vehicleStruct.startup_settings & STARTUP_SETTINGS_IWDG){
		Init_IWDG(STARTUP_TIMEOUT);
	}


	uint32_t last_usb_frame_watchdog = USB_OTG_DEVICE->DSTS;
	while(1){
		timeout = 0xFFFF;
		while(timeout-- > 0){}
			/* if usb is plugged in */
			if(last_usb_frame_watchdog != USB_OTG_DEVICE->DSTS){
				last_usb_frame_watchdog = USB_OTG_DEVICE->DSTS;
				IWDG_reset();
			}
			
			if((!(CAN1->ESR & CAN_ESR_LEC)) & (!(CAN2->ESR & CAN_ESR_LEC))){
				IWDG_reset();
			}
			else if((CAN1->ESR & CAN_ESR_LEC_2)/* & ((CAN2->ESR & CAN_ESR_LEC) == 0x02)*/){
				IWDG_reset();
			}
			if(cmd_fll == ELP_OK){
				
				process_protocol_cmd(&global_el_command);
				cmd_fll = ELP_IDLE;
			}
	}
}




void initDeviceGeneralPinout(void){
	#ifdef ALLIGATOR

	Can1_Initializate(REMAP_CAN1, CAN_BAUDRATE_500KB);  
	Can2_Initializate(REMAP_CAN2, CAN_BAUDRATE_500KB);
	
	GPIOB->MODER &= ~GPIO_MODER_MODER7;
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED7;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD7;
	GPIOB->MODER |= GPIO_MODER_MODER7_0;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD7_1;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_0;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_2;
	
	GPIOB->BSRR |= GPIO_BSRR_BR7;

	SPI1_init_pinout();
	SPI1_start(SPI_master_mode, SPI_polling_mode, SPI_b_mode, SPI_msb_mode, SPI_no_Dma);
	#endif
	
	#ifdef DEVICE_2CAN
	Can1_Initializate(REMAP_CAN1, CAN_BAUDRATE_500KB);  
	Can2_Initializate(REMAP_CAN2, CAN_BAUDRATE_500KB);
		
	GPIOB->CRL	&= ~GPIO_CRL_CNF7;						
	GPIOB->CRL 	|= GPIO_CRL_MODE7_0;					
	GPIOB->CRL 	|= GPIO_CRL_MODE7_1;
	GPIOB->BSRR |= GPIO_BSRR_BR7;						
		
	GPIOB->CRH	&= ~GPIO_CRH_CNF14;					
	GPIOB->CRH 	|= GPIO_CRH_MODE14_0;				
	GPIOB->CRH 	|= GPIO_CRH_MODE14_1;
	GPIOB->BSRR |= GPIO_BSRR_BR14;
		
	SPI1_init_pinout();
	//	SPI1_start(SPI_master_mode, SPI_polling_mode, SPI_b_mode, SPI_msb_mode, SPI_DmaTxRx);
	SPI1_start(SPI_master_mode, SPI_polling_mode, SPI_b_mode, SPI_msb_mode, SPI_no_Dma);
	
	#endif
	
	#ifdef DEVICE_2CAN_BOXED 
	Can1_Initializate(REMAP_CAN1, CAN_BAUDRATE_500KB);  
	Can2_Initializate(NO_REMAP_CAN2, CAN_BAUDRATE_500KB);

	GPIOB->CRH	&= ~GPIO_CRH_CNF11;						
	GPIOB->CRH 	|= GPIO_CRH_MODE11_0;					
	GPIOB->CRH 	|= GPIO_CRH_MODE11_1;
	GPIOB->BSRR |= GPIO_BSRR_BS11;						
		
	GPIOB->CRL	&= ~GPIO_CRL_CNF7;						
	GPIOB->CRL 	|= GPIO_CRL_MODE7_0;					
	GPIOB->CRL 	|= GPIO_CRL_MODE7_1;
	GPIOB->BSRR |= GPIO_BSRR_BS7;

	SPI3_init_pinout();
	AFIO->MAPR  |= AFIO_MAPR_SPI3_REMAP;
	SPI3_start(SPI_master_mode, SPI_polling_mode, SPI_b_mode, SPI_msb_mode, SPI_no_Dma);

	#endif
	
	#ifdef DEVICE_2CAN_TJA1042 
	GPIOB->CRL	&= ~GPIO_CRL_CNF7;						
	GPIOB->CRL 	|= GPIO_CRL_MODE7_0;					
	GPIOB->CRL 	|= GPIO_CRL_MODE7_1;
	GPIOB->BSRR |= GPIO_BSRR_BS7;						
		
	GPIOB->CRL	&= ~GPIO_CRL_CNF4;						
	GPIOB->CRL 	|= GPIO_CRL_MODE4_0;					
	GPIOB->CRL 	|= GPIO_CRL_MODE4_1;
	GPIOB->BSRR |= GPIO_BSRR_BS4;
	Can1_Initializate(REMAP_CAN1, CAN_BAUDRATE_500KB);  
	Can2_Initializate(REMAP_CAN2, CAN_BAUDRATE_500KB);
	SPI1_init_pinout();
	SPI1_start(SPI_master_mode, SPI_polling_mode, SPI_b_mode, SPI_msb_mode, SPI_no_Dma);
	#endif
	
}
