#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>

// 串口接收变量
uint8_t uart_cmd = 0;
uint8_t uart_cmd_ready = 0;

void UART_Init(void)
{
    // 初始化GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    // TX (PA9)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // RX (PA10)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 初始化USART1
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    
    // 使能串口接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    // 设置NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(USART1, ENABLE);
}

void UART_SendString(char *str)
{
    while (*str) {
        USART_SendData(USART1, *str++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
}

void UART_SendByte(uint8_t data)
{
    USART_SendData(USART1, data);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

uint8_t UART_IsCmdReady(void)
{
    return uart_cmd_ready;
}

uint8_t UART_GetCmd(void)
{
    uart_cmd_ready = 0;
    return uart_cmd;
}

// 串口中断服务函数
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        char ch = USART_ReceiveData(USART1);
        
        // 只处理数字字符 '1' 到 '6'
        if (ch >= '1' && ch <= '6') {
            uart_cmd = ch;  // 存储命令数字
            uart_cmd_ready = 1;  // 标记命令就绪
        }
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
