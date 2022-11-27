//*****************************************************************
// 
// STM32F105
// bxCAN controller functions
// todo: Add Extended id support, RTR
// Alex Svetlichnyy 2019
//*****************************************************************
#ifndef CAN_H_INCLUDED
#define CAN_H_INCLUDED

#include "stm32f205xx.h"
//#include "string_convertor.h"
//#include "usart.h"
//#include "device_init.h"

#define NO_REMAP_CAN1								0
#define REMAP_CAN1									1
#define NO_REMAP_CAN2								0
#define REMAP_CAN2									1
#define CAN_INIT_TIMEOUT_MAX	    	0xfff
#define CAN_BAUDRATE_1000KB    	  	1000
#define CAN_BAUDRATE_500KB    	  	500
#define CAN_BAUDRATE_250KB    	  	250
#define CAN_BAUDRATE_125KB    	  	125
#define CAN_BAUDRATE_83KB    	  	  83

#define CAN_TX_MAILBOX_0 					0
#define CAN_TX_MAILBOX_1 					1	
#define CAN_TX_MAILBOX_2 					2	

#define CAN_EMPTY_MAILBOX_TIMEOUT 0xffff
#define CAN_TRANSMIT_TIMEOUT 0x01
#define ERR_CAN1_NO_EMPTY_MAILBOX 4
#define CAN_TX_OK 						 		5
#define ERR_CAN2_NO_EMPTY_MAILBOX 6

#define CAN_FIFO_0 0	
#define CAN_FIFO_1 1

#define STD_ID	0
#define EXT_ID	1

#define CAN_ID(H,L) (uint16_t)((H << 8) | L)

typedef struct std_can_id{
	uint8_t id_highbyte;	// First number of an id 					(BXX)
	uint8_t id_lowbyte; 	// Second, third numbers of an id (XBB)
	uint16_t unused;		
}std_can_id;

typedef union can_id{
	std_can_id std_id;
	uint32_t ext_id;
}can_id;

// Structure of a  CAN message 
typedef struct can_message{	
	can_id id;
	uint16_t ide;
	uint16_t dlc;					// Message size, number of bytes
	uint8_t data[8];			// Data
} can_message; //creating new type

uint32_t Can1_Initializate_Pins(uint32_t remapConfig); 									// Configure CAN1 pins
uint32_t Can1_Configure_BaudRate(uint32_t baudRate);										// Configure CAN1 baud rate
uint32_t Can1_Initializate(uint32_t remapConfig, uint32_t baudRate);		// General CAN1 initialization	


uint32_t Can2_Initializate_Pins(uint32_t remapConfig); 									// Configure CAN2 pins
uint32_t Can2_Configure_BaudRate(uint32_t baudRate);										// Configure CAN2 baud rate
uint32_t Can2_Initializate(uint32_t remapConfig, uint32_t baudRate);		// General CAN2 initialization	


void Can1_InitializateFilters(void);																		// Initialize CAN1 filters
void Can1_InitializateFilters2(void);
void Can1_InitializateFilters_GatewayTemplate(void);
void Can1_InitializateFilters_GatewayAll(void);



uint32_t Can1_GetEmptyMailbox(void);																							// CAN1:Get an empty transmit mailbox
uint32_t Can1_SetTxMessage(can_message canMessage, uint32_t mailboxNo);	// CAN1:Prepare a CAN transmission act with a message and mailbox number 
uint32_t Can1_Transmit_StdMessage(can_message canMessage);								// CAN1:Transmit a CAN message
can_message Can1_Recieve_StdMessage(uint32_t FifoNo);										// CAN1:Recieve a standart id Message
uint32_t Can1_Check_Fifo0Pending_StdMessage(void);																// CAN1:Check for any messages in FIFO0



uint32_t Can2_GetEmptyMailbox(void);																							// CAN2:Get an empty transmit mailbox
uint32_t Can2_SetTxMessage(can_message canMessage, uint32_t mailboxNo);	// CAN2:Prepare a CAN transmission act with a message and mailbox number 
uint32_t Can2_Transmit_StdMessage(can_message canMessage);								// CAN2:Transmit a CAN message
can_message Can2_Recieve_StdMessage(uint32_t FifoNo);										// CAN2:Recieve a standart id Message
uint32_t Can2_Check_Fifo0Pending_StdMessage(void);																// CAN2:Check for any messages in FIFO0

void FlushCanTxBuffers(CAN_TypeDef *CAN);																					// CAN:Reset TX mailboxes
	
#endif /* CAN_H_INCLUDED*/
