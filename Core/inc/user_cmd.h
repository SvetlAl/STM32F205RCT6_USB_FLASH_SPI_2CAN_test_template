/****************************************************************
* 
* User interface command parser
*
* Alex Svetlichnyy 2021 svetlal@outlook.com
*
*****************************************************************/
#ifndef USER_CMD_H_INCLUDED
#define USER_CMD_H_INCLUDED


#include "main.h"

void process_protocol_cmd(elp_cmd *com);
void protocol_response(uint32_t param);

extern void set_vehicle_value(uint32_t count, uint32_t _value);
extern uint8_t *get_vehicle_settings_data(void);

#endif /* USER_CMD_H_INCLUDED*/
