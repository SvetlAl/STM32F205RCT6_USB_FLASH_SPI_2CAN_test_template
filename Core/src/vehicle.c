#include "vehicle.h"

/***************************************************
****************************************************
************* Vehicle specific vars ****************
****************************************************
***************************************************/
//VehicleStruct theVehicle;

static uint32_t odoInitStatus_LC300 = 0;
static odometerCounter_LC300 realOdo_LC300 = {0,0,0};
static odometerCounter_LC300 calibratedOdo_LC300 = {0,0,0};

static uint32_t tyre_pressure_status_DMAX = TYRE_PRESSURE_STATUS_OK_DMAX;


/***************************************************
****************************************************
************* Standart format vars *****************
****************************************************
***************************************************/


VehicleStructData loadVehicle(uint8_t *buf){
	VehicleStructData load;
	for(uint32_t i = 0; i < VEHICLE_STATUS_SETTINGS_LENGTH; i++){
		load.data[i] = buf[i];
	}	
	load._vehicleStruct.modes = 0x00;
	return load;
}
	

void initVehicle(VehicleStruct *vehicle, uint8_t mode){

	vehicle->startup_settings = STARTUP_SETTINGS_DEFAULT;
	vehicle->modes = 0x00;
	
	/*********** DEFAULT ****************/
	vehicle->id_speedometer = 0xFFF;
	vehicle->id_speedometer_extra = 0xFFF;
	vehicle->id_odometer = 0xFFF;
	vehicle->id_cruise = 0xFFF;
	
	if(mode == VEHICLE_INIT_DEFAULT){
		vehicle->speedometer_mlt = 0x64;
		vehicle->odometer_mlt = 0x64;
		vehicle->cruise_mlt = 0x64;
		vehicle->tyres_threshold = 0x64;
	}
	
	vehicle->calibrate_extra = &dummy_function;
	
	if(vehicle->model == MODEL_TOYOTA_LC300){																												/* MODEL_TOYOTA_LC300 */
		vehicle->startup_settings = STARTUP_SETTINGS_IWDG;
		vehicle->id_speedometer = 0x0D7;
		vehicle->id_odometer = 0x0D7;
		vehicle->id_cruise = 0x399;
			
		if(mode == VEHICLE_INIT_DEFAULT){
			vehicle->speedometer_mlt = 0x069;
			vehicle->odometer_mlt = 0x06D;
			vehicle->cruise_mlt = 0x6E;
		}
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_TOYOTA_LC300;
		vehicle->calibrate_odometer = &calibrateSpeedOdo_TOYOTA_LC300; /* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_TOYOTA_LC300;
		}
	else if(vehicle->model == MODEL_TOYOTA_LC150){																									/* MODEL_TOYOTA_LC150 */
		vehicle->startup_settings = STARTUP_SETTINGS_IWDG;
		vehicle->id_speedometer = 0x0B4;
		vehicle->id_odometer = 0x0B4;
		vehicle->id_cruise = 0x399;
			
		if(mode == VEHICLE_INIT_DEFAULT){
			vehicle->speedometer_mlt = 108;
			vehicle->odometer_mlt = 0x0A;
			vehicle->cruise_mlt = 108;
		}
			
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_TOYOTA_LC150;
		vehicle->calibrate_odometer = &calibrateSpeedOdo_TOYOTA_LC150; /* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_TOYOTA_LC300;
		}
		
	else if(vehicle->model == MODEL_TOYOTA_HILUX){																									/* MODEL_TOYOTA_HILUX */
		vehicle->startup_settings = STARTUP_SETTINGS_IWDG;
		vehicle->id_cruise = 0x399;
			
		if(mode == VEHICLE_INIT_DEFAULT){	
			vehicle->speedometer_mlt = NO_CALIBRATION_RATIO;
			vehicle->odometer_mlt = NO_CALIBRATION_RATIO;
			vehicle->cruise_mlt = 112;
		}
			
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_TOYOTA_LC150; /* dummy */
		vehicle->calibrate_odometer = &calibrateSpeedOdo_TOYOTA_LC150;		/* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_TOYOTA_LC300;
		}
		
	else if(vehicle->model == MODEL_TOYOTA_TUNDRA_2008){																						/* MODEL_TOYOTA_TUNDRA_2008 */
		vehicle->startup_settings = STARTUP_SETTINGS_IWDG;
		vehicle->id_speedometer = 0x0B4;
		vehicle->id_odometer = 0x0B4;
		vehicle->id_cruise = 0x399;

		if(mode == VEHICLE_INIT_DEFAULT){		
			vehicle->speedometer_mlt = 110;
			vehicle->odometer_mlt = 0x0A;
			vehicle->cruise_mlt = 110;
		}
			
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_TUNDRA_2008; 
		vehicle->calibrate_odometer = &calibrateSpeedOdo_TUNDRA_2008;		/* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_TOYOTA_LC300;
		}		
		
	else if(vehicle->model == MODEL_GREAT_WALL_WINGLE7){																						/* MODEL_GREAT_WALL_WINGLE7 */
		vehicle->startup_settings = STARTUP_SETTINGS_IWDG;
		vehicle->id_speedometer = 0x265;
		vehicle->id_odometer = 0x265;
			
		if(mode == VEHICLE_INIT_DEFAULT){
			vehicle->speedometer_mlt = 116;
			vehicle->odometer_mlt = 116;
			vehicle->cruise_mlt = 100;
		}
			
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_GREAT_WALL_WINGLE7; 
		vehicle->calibrate_odometer = &calibrateSpeedOdo_GREAT_WALL_WINGLE7;		/* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_TOYOTA_LC300;							/* dummy */
		}
		
	else if(vehicle->model == MODEL_DODGE_RAM_2013){																						/* MODEL_DODGE_RAM_2013 */
		vehicle->startup_settings = STARTUP_SETTINGS_IWDG;
		vehicle->id_speedometer = 0x11C;
		vehicle->id_odometer = 0x11C;
		vehicle->id_cruise = 0x278;
			
		if(mode == VEHICLE_INIT_DEFAULT){
			vehicle->speedometer_mlt = 0x06E;
			vehicle->odometer_mlt = 0x06E;
			vehicle->cruise_mlt = 0x6C;
		}
			
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_DODGE_RAM_2013; 
		vehicle->calibrate_odometer = &calibrateSpeedOdo_DODGE_RAM_2013;			/* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_DODGE_RAM_2013;							
		}
		
	else if(vehicle->model == MODEL_ISUZU_DMAX_MUX){																						/* MODEL_ISUZU_DMAX_MUX */
		vehicle->startup_settings = STARTUP_SETTINGS_IWDG;
		vehicle->id_speedometer = 0x141;
		vehicle->id_odometer = 0x141;
		vehicle->id_cruise = 0x46C;
			
		if(mode == VEHICLE_INIT_DEFAULT){
			vehicle->speedometer_mlt = 110;
			vehicle->odometer_mlt = 110;			/* dummy */
			vehicle->cruise_mlt = 110;
			vehicle->tyres_threshold = 0x96;
		}
			
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_ISUZU_DMAX_MUX; 
		vehicle->calibrate_odometer = &calibrateSpeedOdo_ISUZU_DMAX_MUX;			/* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_ISUZU_DMAX_MUX;
		vehicle->calibrate_extra = &calibrateExtras_ISUZU_DMAX_MUX;			
		}
		
	else if(vehicle->model == MODEL_LAND_ROVER_DEFENDER_2022){																						/* MODEL_LAND_ROVER_DEFENDER_2022 */
		vehicle->startup_settings = STARTUP_SETTINGS_DEFAULT;
		vehicle->id_speedometer = 0x011;
		vehicle->id_odometer = 0x011;
		vehicle->id_cruise = 0x150;
			
		if(mode == VEHICLE_INIT_DEFAULT){
			vehicle->speedometer_mlt = 0x69;
			vehicle->odometer_mlt = 0x69;			/* dummy */
			vehicle->cruise_mlt = 0x69;
		}
			
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_LAND_ROVER_DEFENDER_2022; 
		vehicle->calibrate_odometer = &calibrateSpeedOdo_LAND_ROVER_DEFENDER_2022;			/* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_LAND_ROVER_DEFENDER_2022;		
		}
	
	else if(vehicle->model == MODEL_MITSUBISHI_L200){																						/* MODEL_MITSUBISHI_L200 */
		vehicle->startup_settings = STARTUP_SETTINGS_IWDG;
		vehicle->id_speedometer = 0x214;
		vehicle->id_speedometer_extra = 0x215;
		vehicle->id_odometer = 0x214;
			
		if(mode == VEHICLE_INIT_DEFAULT){
			vehicle->speedometer_mlt = 0x6E;
			vehicle->odometer_mlt = 0x6E;			
			vehicle->cruise_mlt = 0x69;    /* dummy */
		}
			
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_MITSUBISHI_L200; 
		vehicle->calibrate_odometer = &calibrateSpeedOdo_MITSUBISHI_L200;					/* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_LAND_ROVER_DEFENDER_2022;		/* dummy */
		}

	else if(vehicle->model == MODEL_MITSUBISHI_PAJERO_SPORT){																						/* MODEL_MITSUBISHI_PAJERO_SPORT */
		vehicle->startup_settings = STARTUP_SETTINGS_IWDG;
		vehicle->id_speedometer = 0x214;
		vehicle->id_speedometer_extra = 0x215;
		vehicle->id_odometer = 0x214;
			
		if(mode == VEHICLE_INIT_DEFAULT){
			vehicle->speedometer_mlt = 0x6E;
			vehicle->odometer_mlt = 0x6E;			
			vehicle->cruise_mlt = 0x69;    /* dummy */
		}
			
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_MITSUBISHI_PAJERO_SPORT; 
		vehicle->calibrate_odometer = &calibrateSpeedOdo_MITSUBISHI_PAJERO_SPORT;			/* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_LAND_ROVER_DEFENDER_2022;				/* dummy */	
		}

	else if(vehicle->model == MODEL_GREAT_WALL_POER){																						/* MODEL_GREAT_WALL_POER */
		vehicle->startup_settings = STARTUP_SETTINGS_IWDG;
		vehicle->id_speedometer = 0x265;
		vehicle->id_odometer = 0x265;
			
		if(mode == VEHICLE_INIT_DEFAULT){
			vehicle->speedometer_mlt = 116;
			vehicle->odometer_mlt = 116;
			vehicle->cruise_mlt = 100;
			vehicle->tyres_threshold = 0x60;
		}
			
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_GREAT_WALL_POER; 
		vehicle->calibrate_odometer = &calibrateSpeedOdo_GREAT_WALL_POER;				/* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_TOYOTA_LC300;							/* dummy */
		vehicle->calibrate_extra = &calibrateExtras_GREAT_WALL_POER;	
		}

	else if(vehicle->model == MODEL_TOYOTA_LC200){																									/* MODEL_TOYOTA_LC200 */
		vehicle->startup_settings = STARTUP_SETTINGS_IWDG;
		vehicle->id_speedometer = 0x0B4;
		vehicle->id_odometer = 0x0B4;
		vehicle->id_cruise = 0x399;
			
		if(mode == VEHICLE_INIT_DEFAULT){
			vehicle->speedometer_mlt = 108;
			vehicle->odometer_mlt = 0x0A;
			vehicle->cruise_mlt = 108;
		}
			
		vehicle->calibrate_speedometer = &calibrateSpeedOdo_TOYOTA_LC150;
		vehicle->calibrate_odometer = &calibrateSpeedOdo_TOYOTA_LC150; /* dummy */
		vehicle->calibrate_cruise = &calibrateCruise_TOYOTA_LC300;
		}		
}


can_message dummy_function(can_message *msg, uint8_t _mlt){
	return *msg;
}



/***************************************************
****************************************************
************* Standart format functions ************
****************************************************
***************************************************/

/***************************************************
****************************************************
****************** Toyota LC300 ********************
****************************************************
***************************************************/

can_message calibrateSpeedOdo_TOYOTA_LC300(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt){
	return *msg;
}

can_message calibrateCruise_TOYOTA_LC300(can_message *msg, uint8_t _cruise_mlt){
			/* Cruise control */
	uint16_t speedCR = (uint16_t)((msg->data[2] << 8) | msg->data[3]);
	speedCR = (uint16_t)(speedCR*_cruise_mlt)/100;
	msg->data[2] = (uint8_t)(speedCR >> 8);
	msg->data[3] = (uint8_t)speedCR &~0xFF00;
	return *msg;
}


/***************************************************
****************************************************
****************** Toyota LC150 ********************
****************************************************
***************************************************/

can_message calibrateSpeedOdo_TOYOTA_LC150(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt){
	if(_speedometer_mlt!= NO_CALIBRATION_RATIO){
		uint16_t speed = (uint16_t)(((msg->data[5] << 8) & ~0x00FF) | 	msg->data[6]);
		speed = (uint16_t)(speed*_speedometer_mlt/100);
		msg->data[5] = (uint8_t)(speed >> 8);
		msg->data[6] = (uint8_t)(speed &~0xFF00);
	}
	
	if(_odometer_mlt == 0x00){
		msg->data[4] = 0;
	}
	else if(msg->data[4]!= 0x02 && _odometer_mlt!= NO_CALIBRATION_RATIO){
		static uint32_t s_odo = 0x00;
		static uint32_t sc_odo = 0x00;
		static uint32_t diff_counter = 0x00;
		
		uint8_t odo_val = msg->data[4];
		if(odo_val != s_odo){
			uint8_t diff = odo_val > s_odo ? (uint8_t)((odo_val - s_odo)/4) : (uint8_t)(((0xFF - s_odo) + (odo_val))/4);
			diff_counter += diff;
			sc_odo += (uint32_t)diff*4;
			
			if(diff_counter > _odometer_mlt){
				uint8_t mlt = (uint8_t)diff_counter/_odometer_mlt;
				sc_odo += (uint8_t)(mlt*4);
				diff_counter -= (uint32_t)mlt*_odometer_mlt;
			}
			s_odo = odo_val;
		}
		msg->data[4] = (uint8_t)sc_odo;
	}
	msg->data[7]=calculateToyotaCheckSum(msg);
	return *msg;
}

/***************************************************
****************************************************
****************** Toyota TUNDRA_2008 **************
****************************************************
***************************************************/

can_message calibrateSpeedOdo_TUNDRA_2008(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt){
	return *msg;
}
/***************************************************
****************************************************
************ MODEL_GREAT_WALL_WINGLE7 **************
****************************************************
***************************************************/

can_message calibrateSpeedOdo_GREAT_WALL_WINGLE7(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt){
	chinese_speed ch_sp;
	
	if(_speedometer_mlt!= NO_CALIBRATION_RATIO){
		ch_sp.high = msg->data[4];
		ch_sp.low = msg->data[5];
		chinese_speed new_ch_sp = recalculate_chinese_speed(ch_sp, _speedometer_mlt);
		msg->data[4]=new_ch_sp.high;
		msg->data[5]=new_ch_sp.low;
	}
	if(_odometer_mlt == 0x00){
		msg->data[4] = 0x00;
		msg->data[5] = 0x00;
	}
	return *msg;
} 

/***************************************************
****************************************************
************ MODEL_GREAT_WALL_POER **************
****************************************************
***************************************************/

can_message calibrateSpeedOdo_GREAT_WALL_POER(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt){
	return *msg;
} 

can_message calibrateExtras_GREAT_WALL_POER(can_message *msg, uint8_t _mlt){
	static uint8_t wh1_psi = 0xB7;
	static uint8_t wh2_psi = 0xB7;
	static uint8_t wh3_psi = 0xB7;
	static uint8_t wh4_psi = 0xB7;
	
	if((msg->id.std_id.id_highbyte == 0x03) & (msg->id.std_id.id_lowbyte == 0x95 )){
		wh1_psi = msg->data[0];
		wh2_psi = msg->data[2];
		wh3_psi = msg->data[4];
		wh4_psi = msg->data[6];
	}
	if((msg->id.std_id.id_highbyte==0x03) & (msg->id.std_id.id_lowbyte == 0x41 )){
		if((wh4_psi >= _mlt) & (wh3_psi >= _mlt) & (wh2_psi >= _mlt) & (wh1_psi >= _mlt)){
			for(uint32_t i = 0; i < 8; i++){
				msg->data[i] = 0x00;
			}
		}
	}
		
	return *msg;
}

/***************************************************
****************************************************
************  MODEL_DODGE_RAM_2013    **************
****************************************************
***************************************************/

can_message calibrateSpeedOdo_DODGE_RAM_2013(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt){
	if(_speedometer_mlt!= NO_CALIBRATION_RATIO){	
		msg->data[4] = (uint8_t)(msg->data[4]*_speedometer_mlt)/100;
	}
	return *msg;
}

can_message calibrateCruise_DODGE_RAM_2013(can_message *msg, uint8_t _cruise_mlt){
	msg->data[0] = (uint8_t)(msg->data[0]*_cruise_mlt)/100;
	return *msg;
}


/***************************************************
****************************************************
************  MODEL_ISUZU_DMAX_MUX    **************
****************************************************
***************************************************/

can_message calibrateSpeedOdo_ISUZU_DMAX_MUX(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt){
	return *msg;
}

can_message calibrateCruise_ISUZU_DMAX_MUX(can_message *msg, uint8_t _cruise_mlt){
	return *msg;
}

can_message calibrateExtras_ISUZU_DMAX_MUX(can_message *msg, uint8_t _mlt){
	return *msg;
}



/***************************************************
****************************************************
************  MODEL_LAND_ROVER_DEFENDER_2022 *******
****************************************************
***************************************************/

can_message calibrateSpeedOdo_LAND_ROVER_DEFENDER_2022(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt){
	return *msg;
}

can_message calibrateCruise_LAND_ROVER_DEFENDER_2022(can_message *msg, uint8_t _cruise_mlt){
	return *msg;
}


/***************************************************
****************************************************
************  MODEL_MITSUBISHI_L200 ****************
****************************************************
***************************************************/

can_message calibrateSpeedOdo_MITSUBISHI_L200(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt){
	return *msg;
}

/***************************************************
****************************************************
************  MODEL_MITSUBISHI_PAJERO_SPORT ********
****************************************************
***************************************************/

can_message calibrateSpeedOdo_MITSUBISHI_PAJERO_SPORT(can_message *msg, uint8_t _speedometer_mlt, uint8_t _odometer_mlt){
	return *msg;
}

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
 
uint8_t calculateToyotaCheckSum(can_message *msg){
	uint32_t sum = 0;
	for(uint8_t i=0; i < (msg->dlc-1); i++){
		sum+=msg->data[i];
	}	 
	uint8_t crc = (uint8_t)((uint32_t)msg->id.std_id.id_highbyte + (uint32_t)msg->id.std_id.id_lowbyte + (uint32_t)msg->dlc + (uint32_t)sum) &~0xFFFFFF00;
	return crc;
}


/***************************************************
****************************************************
*************** Toyota TUNDRA2008 ******************
****************************************************
***************************************************/

/***************************************************
****************************************************
********** MODEL_GREAT_WALL_WINGLE7 ****************
****************************************************
***************************************************/

//uint32_t test2 = 0;
uint32_t chinese_speed_2_kmh(chinese_speed ch_sp){
	if(ch_sp.high < 0x20){
		return 0;
	}
	uint8_t a = (uint8_t)(ch_sp.high - 0x20);
	uint8_t b = ch_sp.low/15;
	uint32_t kmh = (uint32_t)(a*15 + b);
	return kmh;
}

chinese_speed kmh_2_chinese_speed(uint32_t kmh){
	uint32_t a = kmh;
	uint32_t counter = 0;
	while(a > 15){
		counter++;
		a = a - 15;
	}
	chinese_speed ch_sp;
	ch_sp.high = (uint8_t)(0x20 + counter);
	ch_sp.low = (uint8_t)((a*0xFF)/15);
	if(ch_sp.high > 0x9F){
		ch_sp.high = 0x20;
		ch_sp.low = 0;
	}		
	return ch_sp;
}
chinese_speed recalculate_chinese_speed(chinese_speed ch_sp, uint32_t mod){
	uint32_t kmh = chinese_speed_2_kmh(ch_sp);
	uint32_t new_kmh = kmh*mod/100;
	chinese_speed new_ch_sp = kmh_2_chinese_speed(new_kmh);
//	test2 = new_kmh;
	return new_ch_sp;
}

/***************************************************
****************************************************
********** MODEL_ISUZU_DMAX         ****************
****************************************************
***************************************************/


/***************************************************
****************************************************
********** MODEL_LAND_ROVER_DEFENDER_2022  *********
****************************************************
***************************************************/



/***************************************************
****************************************************
**********			MITSUBISHI			************
****************************************************
***************************************************/


	
