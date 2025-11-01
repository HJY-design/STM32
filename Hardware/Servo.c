#include "stm32f10x.h"
#include "Delay.h"

// 舵机PWM引脚定义 - 使用TIM2_CH1 (PA0)
#define SERVO_PIN GPIO_Pin_0
#define SERVO_PORT GPIOA
#define SERVO_TIM TIM2

void Servo_Init(void)
{
    // 初始化GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = SERVO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SERVO_PORT, &GPIO_InitStructure);
    
    // 初始化定时器2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 19999; // 20ms周期
    TIM_TimeBaseInitStructure.TIM_Prescaler = 71; // 72MHz/(71+1)=1MHz, 1MHz/20000=50Hz
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(SERVO_TIM, &TIM_TimeBaseInitStructure);
    
    // 初始化PWM通道
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 1500; // 初始位置1.5ms (90度)
    TIM_OC1Init(SERVO_TIM, &TIM_OCInitStructure);
    
    TIM_Cmd(SERVO_TIM, ENABLE);
    TIM_CtrlPWMOutputs(SERVO_TIM, ENABLE);
}

void Servo_SetAngle(uint8_t angle)
{
    // SG90舵机控制：0.5ms-2.5ms脉冲对应0-180度
    // 1MHz时钟，0.001ms计数，0.5ms=500, 2.5ms=2500
    uint16_t pulse = 500 + (angle * 2000 / 180);
    TIM_SetCompare1(SERVO_TIM, pulse);
}
