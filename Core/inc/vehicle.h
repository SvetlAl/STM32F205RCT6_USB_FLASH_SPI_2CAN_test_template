#ifndef VEHICLE_H_INCLUDED
#define VEHICLE_H_INCLUDED

/****************************************************************
* 			Vehicle handler
*
*
* Alex Svetlichnyy 2022 svetlal@outlook.com
*****************************************************************/
#include "stm32f205xx.h"
#include "can.h"
#include "string_convertor.h"
#include "main.h"

/***************************************************
****************************************************
************* Standart format defs  ****************
****************************************************
***************************************************/

#define VEHICLE_OK 		0
#define VEHICLE_FAIL 	1

#define VEHICLE_INIT_PRESET 		1
#define VEHICLE_INIT_DEFAULT 		2

#define NO_CALIBRATION_RATIO 					0x64

#define STATUS_STRING_LENGTH 						64
#define VEHICLE_STATUS_SETTINGS_LENGTH	8

#define STARTUP_SETTINGS_DEFAULT				0x00U
#define STARTUP_SETTINGS_IWDG						0x01U

#define CALIBRATOR_DEFAULT_MODE					(uint8_t)0x00U
#define CALIBRATOR_ODOMETER_MODE				(uint8_t)0x01U
#define CALIBRATOR_TEST_MODE					(uint8_t)0x02U

#define SPEEDOMETER_TEST_VALUE							0x2CU
#define SPEEDOMETER_TEST_VALUE_GW						0x9CU

#define MODEL_TOYOTA_LC300									0x00000000U
#define MODEL_TOYOTA_LC150									0x00000001U
#define MODEL_TOYOTA_HILUX									0x00000002U

#define MODEL_TOYOTA_TUNDRA_2008							0x00000003U
#define MODEL_GREAT_WALL_WINGLE7							0x00000004U
#define MODEL_DODGE_RAM_2013								0x00000005U
#define MODEL_ISUZU_DMAX_MUX								0x00000006U
#define MODEL_LAND_ROVER_DEFENDER_2022						0x00000007U
#define MODEL_MITSUBISHI_L200								0x00000008U
#define MODEL_MITSUBISHI_PAJERO_SPORT						0x00000009U
#define MODEL_GREAT_WALL_POER								0x0000000AU
#define MODEL_TOYOTA_LC200									0x0000000BU

#define MODEL_COUNT											0x0000000CU


/***************************************************
****************************************************
************* Vehicle specific defs ****************
****************************************************
***************************************************/

/***********  MODEL_TOYOTA_LC300  ************/
#define ODO_VAL_MAX_LC300			 		0x3F
#define ODO_VAL_MAX_COUNTER_LC300 64*8
 
typedef struct odometerCounter_LC300{	
	uint8_t value; 
	uint8_t start;
	uint32_t counter;
} odometerCounter_LC300; 


/***********  MODEL_TOYOTA_TUNDRA_2008  ************/ 
typedef struct tundra_speed{	
	uint8_t low;
	uint8_t high;
} tundra_speed; //creating new type

typedef union un_tundra_speed{
	tundra_speed spval;
	uint16_t hexval;
}un_tundra_speed;

/***********  MODEL_GREAT_WALL_WINGLE7  ************/ 

typedef struct chinese_speed{	
	uint8_t high;
	uint8_t low;
} chinese_speed; //creating new type

/***********  MODEL_ISUZU_DMAX  ************/ 

#define INCREMENT_BIT_DMAX_1_23 48
#define INCREMENT_BIT_DMAX_23_M 64
#define INCREMENT_BIT_DMAX_ZERO 0x00D5 //*ISUZU_DMAX_MODIFICATOR/100
#define SPEED23_BIT_MASK_DMAX 0x04E8
#define TIRE_THRESHOLD_KPA_DMAX 0x96
#define SPEEDOMETER_PRECISION_DMAX 8
#define SPEEDOMETER_SCALE_DMAX 10

#define TYRE_PRESSURE_STATUS_OK_DMAX 0

 	typedef struct speed_DMAX{	
	uint8_t data[2];			// Data 0 - first 1- second
} speed_DMAX; //creating new type
	

/***********  MODEL_LAND_ROVER_DEFENDER_2022  ************/ 

#define LR_1_KMH 0x49
#define LR_2_KMH 0x6A
#define LR_3_KMH 0x90
#define LR_4_KMH 0xB5
#define LR_5_KMH 0xFF
#define LR_SP_STEP 0x64

typedef struct land_rover_speed{
	uint8_t hb;
	uint8_t lb;
}land_rover_speed;


/***********  MITSUBISHI      ************/ 

 	typedef struct speed_ML200{	
	uint8_t data[2];			// Data 0 - first 1- second
} speed_ML200; //creating new type

/***************************************************
****************************************************
************* Vehicle structure ********************
****************************************************
***************************************************/




typedef struct VehicleStruct{
	uint16_t model;
	
	uint8_t startup_settings;
	uint8_t modes;
		
	uint8_t speedometer_mlt;
	uint8_t odometer_mlt;
	uint8_t cruise_mlt;
	uint8_t tyres_threshold;
	
	uint16_t id_speedometer;
	uint16_t id_speedometer_extra;
	uint16_t id_odometer;
	uint16_t id_cruise;
	
	can_message (*calibrate_speedometer)(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt);
	can_message (*calibrate_odometer)(can_message *msg, uint8_t _speedometer_mlt, uint8_t odometer_mlt);
	can_message (*calibrate_cruise)(can_message *msg, uint8_t _cruise_mlt);
	can_message (*calibrate_extra)(can_message *msg, uint8_t _mlt);
} VehicleStruct; //creating new type

typedef union VehicleStructData{
	VehicleStruct _vehicleStruct;
	uint8_t data[sizeof(VehicleStruct)];
}VehicleStructData;


VehicleStructData loadVehicle(uint8_t *buf);
void initVehicle(VehicleStruct *vehicle, uint8_t mode);

can_message dummy_function(can_message *msg, uint8_t _mlt);




uint8_t calculateToyotaCheckSum(can_message *msg);

/***************************************************
****************************************************
************* Standart format functions ************
****************************************************
***************************************************/

can_message calibrateSpeedOdo_TOYOTA_LC300(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt);
can_message calibrateCruise_TOYOTA_LC300(can_message *msg, uint8_t _cruise_mlt);

can_message calibrateSpeedOdo_TOYOTA_LC150(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt);

can_message calibrateSpeedOdo_TUNDRA_2008(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt);

can_message calibrateSpeedOdo_GREAT_WALL_WINGLE7(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt);

can_message calibrateSpeedOdo_DODGE_RAM_2013(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt);
can_message calibrateCruise_DODGE_RAM_2013(can_message *msg, uint8_t _cruise_mlt);

can_message calibrateSpeedOdo_ISUZU_DMAX_MUX(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt);
can_message calibrateCruise_ISUZU_DMAX_MUX(can_message *msg, uint8_t _cruise_mlt);
can_message calibrateExtras_ISUZU_DMAX_MUX(can_message *msg, uint8_t _mlt);

can_message calibrateSpeedOdo_LAND_ROVER_DEFENDER_2022(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt);
can_message calibrateCruise_LAND_ROVER_DEFENDER_2022(can_message *msg, uint8_t _cruise_mlt);

can_message calibrateSpeedOdo_MITSUBISHI_L200(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt);

can_message calibrateSpeedOdo_MITSUBISHI_PAJERO_SPORT(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt);

can_message calibrateSpeedOdo_GREAT_WALL_POER(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt);
can_message calibrateExtras_GREAT_WALL_POER(can_message *msg, uint8_t _mlt);

/***************************************************
****************************************************
************* Vehicle specific functions ***********
****************************************************
***************************************************/

/***************************************************
****************************************************
****************** Toyota LC300 ********************
****************************************************
***************************************************/

uint16_t calibrate_speedometer_LC300(uint16_t actualReading, uint32_t mod);
uint16_t convert_KM_H_into_toyota_protocol_LC300(uint32_t speed32);
uint32_t convert_toyota_protocol_into_KM_H_LC300(uint16_t canReading);
uint8_t subtractE3_LC300(uint8_t cur, uint8_t last);
void incE3_LC300(uint8_t *val);
uint8_t calculateOdoValue_LC300(uint32_t counter, uint8_t start);
void increaseOdoCounter_LC300(odometerCounter_LC300 *odo, uint8_t count, uint8_t currentval);
void resetOdoCounter_LC300(odometerCounter_LC300 *odoReal, odometerCounter_LC300 *odoCalibrated, uint32_t mod);
void calibrateOdometer_LC300(odometerCounter_LC300 *odoCalibrated, odometerCounter_LC300 *odoReal, uint32_t mod);

/***************************************************
****************************************************
****************** Toyota TUNDRA2008 ***************
****************************************************
***************************************************/

uint32_t convert_struct_to_kmh(un_tundra_speed sp);
uint16_t convert_kmh_to_struct(const uint32_t speed);
un_tundra_speed recalculate_speed(un_tundra_speed uncal_speed, uint32_t ratio);


/***************************************************
****************************************************
********** MODEL_GREAT_WALL_WINGLE7 ****************
****************************************************
***************************************************/

uint32_t chinese_speed_2_kmh(chinese_speed ch_sp);
chinese_speed kmh_2_chinese_speed(uint32_t kmh);
chinese_speed recalculate_chinese_speed(chinese_speed ch_sp, uint32_t mod);

/***************************************************
****************************************************
**********      MODEL_ISUZU_DMAX    ****************
****************************************************
***************************************************/

speed_DMAX calculate_NewSpeed_DMAX(speed_DMAX value, uint8_t mlt);
uint32_t convertSpeedToUint_DMAX(speed_DMAX sp);	
speed_DMAX convertUintToSpeed_DMAX(uint32_t speed_u32);
uint16_t speed2uint16_DMAX(speed_DMAX *speed);
speed_DMAX uint16_2speed_DMAX(uint16_t val16);

/***************************************************
****************************************************
********** MODEL_LAND_ROVER_DEFENDER_2022 **********
****************************************************
***************************************************/

land_rover_speed calculate_cal_speed_LR(land_rover_speed wrong_speed, uint8_t mlt);
land_rover_speed convert_kmh_to_candata_LR(uint16_t kmh);
uint16_t convert_candata_to_kmh_LR(land_rover_speed sp);

/***************************************************
****************************************************
********** MITSUBISHI                     **********
****************************************************
***************************************************/

uint32_t convertSpeedToUint_ML200(speed_ML200 sp);
speed_ML200 convertUint32toSpeed_ML200(uint32_t sp);	
speed_ML200 calculate_NewSpeed_ML200(speed_ML200 value, uint32_t mod);


#endif /* VEHICLE_H_INCLUDED */
