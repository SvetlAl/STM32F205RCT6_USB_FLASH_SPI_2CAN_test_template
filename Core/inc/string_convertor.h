/****************************************************************
* 
* Some functions to work with the strings, arrays and calculations
*
* Alex Svetlichnyy 2021 svetlal@outlook.com
*
*****************************************************************/

#ifndef STRING_CONVERTOR_H_INCLUDED
#define STRING_CONVERTOR_H_INCLUDED

#include "stm32f205xx.h"


//***************** RGB LED DEFINITIONS *******************
#define RANGE_IN_MIN 100
#define RANGE_IN_MAX 1200
#define RANGE_RGB_MIN 0
#define RANGE_RGB_MAX 255
//***************** RGB LED DEFINITIONS *******************


uint8_t halfbyte_to_hexascii(uint8_t _halfbyte); // Translate a single hex (0-F) into an ascii symbol 
uint8_t hexascii_to_halfbyte(uint8_t _ascii);		 // Translate an ascii symbol into a hex (0-F)
void str_to_ascii(uint8_t *data_in, uint8_t *data_out, uint16_t length);
uint32_t str_to_uint32(uint8_t *data_in);
uint8_t str_to_uint8(uint8_t lowbyte, uint8_t highbyte);

void hexArray2asciiArray(uint8_t *in_buf, uint8_t *out_buf, uint32_t input_length);

#endif /* STRING_CONVERTOR_H_INCLUDED*/
