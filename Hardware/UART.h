#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"

void UART_Init(void);
void UART_SendString(char *str);
void UART_SendByte(uint8_t data);
uint8_t UART_IsCmdReady(void);
uint8_t UART_GetCmd(void);

#endif
