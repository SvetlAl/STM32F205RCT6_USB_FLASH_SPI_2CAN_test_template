#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED
#define STARTUP_TIMEOUT 8000

#define VALUE_SPEEDOMETER_MLT	0
#define VALUE_ODOMETER_MLT		1
#define VALUE_CRUISE_MLT		2
#define VALUE_TYRES_THRESHOLD	3

/****************************************************************
* A device based on STM32F205
*
*
* Alex Svetlichnyy 2022 svetlal@outlook.com
*****************************************************************/


#include "stm32f205xx.h"
#include "stm32f205_clock.h"
#include "stm32f105_usbdfs_cdc.h"
//#include "usb_cdc_desc.h"
#include "stm32f105_usbdfs_macros.h"
#include "el_protocol.h"
#include "flash_mem_chip_spi.h"
#include "spi.h"
#include "string_convertor.h"
#include "user_cmd.h"
//#include "can.h"
#include "watchdog.h"
#include "vehicle.h"
#include "device_model.h"

#include <string.h>
	
#define VEHICLE_SAVE_STATUS							0x00000AA0
#define VEHICLE_SET_MODEL							0x00000A01
//#define VEHICLE_SET_STARTUP_SETTINGS				0x00000A02
#define VEHICLE_SET_SPEEDOMETER_MLT					0x00000A03
#define VEHICLE_SET_ODOMETER_MLT					0x00000A04
#define VEHICLE_SET_CRUISE_MLT						0x00000A05
#define VEHICLE_SET_TYRES_THRESHOLD					0x00000A06
#define VEHICLE_SET_ODOMETER_MODE					0x00000A07
#define VEHICLE_SET_TEST_MODE						0x00000A08

//#define VEHICLE_SET_MODE_B						0x00000A09

#define LOBYTE(x) ((uint8_t)x)
#define HIBYTE(x) ((uint8_t)(x >> 8))

extern void process_protocol_cmd(elp_cmd *command);
extern memchip_status memory_chip_status;
extern el_quadro_buffer quadro_buf;
extern uint8_t dma_spi_buff[1024];


uint8_t *get_vehicle_settings_data(void);
void set_vehicle_value(uint32_t cmd, uint32_t _value);
void initDeviceGeneralPinout(void);
#endif /* MAIN_H_INCLUDED*/
