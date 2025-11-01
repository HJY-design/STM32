#include "stm32f10x.h"

// TB6612电机驱动引脚定义
#define MOTOR_PWMA_PIN GPIO_Pin_6
#define MOTOR_PWMA_PORT GPIOA
#define MOTOR_PWMA_TIM TIM3

#define MOTOR_AIN1_PIN GPIO_Pin_4
#define MOTOR_AIN1_PORT GPIOA
#define MOTOR_AIN2_PIN GPIO_Pin_5
#define MOTOR_AIN2_PORT GPIOA

// STBY引脚（可选，如果使用的话）
#define MOTOR_STBY_PIN GPIO_Pin_7
#define MOTOR_STBY_PORT GPIOA

void Motor_Init(void)
{
    // 初始化方向控制引脚 AIN1, AIN2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 初始化AIN1, AIN2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = MOTOR_AIN1_PIN | MOTOR_AIN2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 初始化STBY引脚（如果使用）
    GPIO_InitStructure.GPIO_Pin = MOTOR_STBY_PIN;
    GPIO_Init(MOTOR_STBY_PORT, &GPIO_InitStructure);
    GPIO_SetBits(MOTOR_STBY_PORT, MOTOR_STBY_PIN); // 使能电机驱动
    
    // 初始化PWM引脚 PWMA
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = MOTOR_PWMA_PIN;
    GPIO_Init(MOTOR_PWMA_PORT, &GPIO_InitStructure);
    
    // 初始化定时器3用于PWM
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 999; // 1kHz PWM频率
    TIM_TimeBaseInitStructure.TIM_Prescaler = 71; // 72MHz/72=1MHz, 1MHz/1000=1kHz
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(MOTOR_PWMA_TIM, &TIM_TimeBaseInitStructure);
    
    // 初始化PWM通道1
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0; // 初始占空比为0
    TIM_OC1Init(MOTOR_PWMA_TIM, &TIM_OCInitStructure);
    
    TIM_Cmd(MOTOR_PWMA_TIM, ENABLE);
    
    // 初始状态：停止
    GPIO_ResetBits(MOTOR_AIN1_PORT, MOTOR_AIN1_PIN);
    GPIO_ResetBits(MOTOR_AIN2_PORT, MOTOR_AIN2_PIN);
    TIM_SetCompare1(MOTOR_PWMA_TIM, 0);
}

void Motor_SetSpeed(int16_t speed)
{
    // 限制速度范围 -100 到 +100
    if (speed > 100) speed = 100;
    if (speed < -100) speed = -100;
    
    if (speed > 0) {
        // 正转
        GPIO_SetBits(MOTOR_AIN1_PORT, MOTOR_AIN1_PIN);
        GPIO_ResetBits(MOTOR_AIN2_PORT, MOTOR_AIN2_PIN);
        TIM_SetCompare1(MOTOR_PWMA_TIM, speed * 10); // 映射到0-1000
    } else if (speed < 0) {
        // 反转
        GPIO_ResetBits(MOTOR_AIN1_PORT, MOTOR_AIN1_PIN);
        GPIO_SetBits(MOTOR_AIN2_PORT, MOTOR_AIN2_PIN);
        TIM_SetCompare1(MOTOR_PWMA_TIM, -speed * 10); // 取绝对值
    } else {
        // 停止
        GPIO_ResetBits(MOTOR_AIN1_PORT, MOTOR_AIN1_PIN);
        GPIO_ResetBits(MOTOR_AIN2_PORT, MOTOR_AIN2_PIN);
        TIM_SetCompare1(MOTOR_PWMA_TIM, 0);
    }
}

// 刹车功能
void Motor_Brake(void)
{
    GPIO_SetBits(MOTOR_AIN1_PORT, MOTOR_AIN1_PIN);
    GPIO_SetBits(MOTOR_AIN2_PORT, MOTOR_AIN2_PIN);
    TIM_SetCompare1(MOTOR_PWMA_TIM, 1000); // 全占空比刹车
}

// 待机功能（如果使用STBY引脚）
void Motor_Standby(uint8_t enable)
{
    if (enable) {
        GPIO_SetBits(MOTOR_STBY_PORT, MOTOR_STBY_PIN);
    } else {
        GPIO_ResetBits(MOTOR_STBY_PORT, MOTOR_STBY_PIN);
    }
}
