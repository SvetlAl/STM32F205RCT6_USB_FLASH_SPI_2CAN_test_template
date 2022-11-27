//*****************************************************************
// 
// 
// 
// 
// Alex Svetlichnyy 2019
//*****************************************************************
#ifndef WATCHDOG_H_INCLUDED
#define WATCHDOG_H_INCLUDED

#include "stm32f205xx.h"


void Init_IWDG(uint16_t tw);
void IWDG_reset(void);



#endif /* WATCHDOG_H_INCLUDED */
