#include "stm32f10x.h"
#include "Infrared.h"

// 红外传感器引脚定义
#define IR_SENSOR_PIN GPIO_Pin_3
#define IR_SENSOR_PORT GPIOA

void Infrared_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_InitStructure.GPIO_Pin = IR_SENSOR_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IR_SENSOR_PORT, &GPIO_InitStructure);
}

uint8_t Infrared_GetState(void)
{
    // 修正逻辑：检测到障碍物时DO输出低电平，表示有入侵
    if (GPIO_ReadInputDataBit(IR_SENSOR_PORT, IR_SENSOR_PIN) == 0) {
        return 1; // 检测到入侵
    }
    return 0; // 正常状态
}
