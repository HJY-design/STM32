#ifndef __INFRARED_H
#define __INFRARED_H

#include "stm32f10x.h"

void Infrared_Init(void);
uint8_t Infrared_GetState(void);

#endif
