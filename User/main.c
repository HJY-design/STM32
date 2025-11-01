#include "stm32f10x.h"
#include "Delay.h"
#include "Key.h"
#include "LED.h"
#include "OLED.h"
#include "Infrared.h"
#include "Servo.h"
#include "Motor.h"
#include "UART.h"
#include <string.h>

// 全局变量
uint8_t alertCount = 0;
uint8_t isAlert = 0;
uint8_t systemState = 0; // 0:待机, 1:警报

void System_Init(void);
void OLED_DisplayStatus(void);
void HandleUARTCommand(void);

int main(void)
{
    System_Init();
    
    // 初始状态显示
    OLED_DisplayStatus();
    Servo_SetAngle(90); // 舵机打开
    Motor_SetSpeed(0);  // 电机关闭

    
    UART_SendString("System Ready\r\n");
    
    while (1) {
        // 检测红外传感器
        uint8_t infraredState = Infrared_GetState();
        
        if (infraredState == 1) {
            // 检测到入侵（有遮挡）
            if (isAlert) {
                // 首次检测到入侵
                isAlert = 0;
                systemState = 0;
                alertCount++;
                
                // 触发警报响应
                OLED_DisplayStatus();
                Servo_SetAngle(90);   // 舵机关闭
                Motor_SetSpeed(0);  // 电机启动
 
                
                UART_SendString("Alert! Intruder detected!\r\n");
            }
        } else {
            // 正常状态（无遮挡）
            if (!isAlert) {
                // 入侵者离开
                isAlert = 1;
                systemState = 1;
                
                // 解除警报
                OLED_DisplayStatus();
                Servo_SetAngle(0); // 舵机打开
                Motor_SetSpeed(80);  // 电机关闭
           
                
                UART_SendString("Alert cleared. System safe.\r\n");
            }
        }
        
        // 按键处理
        uint8_t keyNum = Key_GetNum();
        if (keyNum == 1) {
            // 按钮1：切换电机状态
            static uint8_t motorState = 0;
            motorState = !motorState;
            if (motorState) {
                Motor_SetSpeed(80);
                UART_SendString("Motor manually started\r\n");
            } else {
                Motor_SetSpeed(0);
                UART_SendString("Motor manually stopped\r\n");
            }
        }
        if (keyNum == 2) {
            // 按钮2：切换舵机状态
            static uint8_t servoState = 0;
            servoState = !servoState;
            if (servoState) {
                Servo_SetAngle(0);
                UART_SendString("Servo manually closed\r\n");
            } else {
                Servo_SetAngle(90);
                UART_SendString("Servo manually opened\r\n");
            }
        }
        
        // 串口指令处理
        if (UART_IsCmdReady()) {
            HandleUARTCommand();
        }
        
        Delay_ms(50);
    }
}

void System_Init(void)
{
    Delay_init();
    Key_Init();
    LED_Init();
    OLED_Init();
    Infrared_Init();
    Servo_Init();
    Motor_Init();
    UART_Init();
}

void OLED_DisplayStatus(void)
{
    OLED_Clear();
    
    if (systemState == 1) {
        // 警报状态
        OLED_ShowString(1, 1, "System: Alert");
        OLED_ShowString(2, 1, "Status: Intruder");
        OLED_ShowString(3, 1, "Alerts: ");
        OLED_ShowNum(3, 9, alertCount, 3);
    } else {
        // 待机状态
        OLED_ShowString(1, 1, "System: Ready");
        OLED_ShowString(2, 1, "Status: Safe");
        OLED_ShowString(3, 1, "Alerts: ");
        OLED_ShowNum(3, 9, alertCount, 3);
    }
}

void HandleUARTCommand(void)
{
    uint8_t cmd = UART_GetCmd();  // 获取命令数字
    
    switch(cmd) {
        case '1':  // 启动电机
            Motor_SetSpeed(80);
            UART_SendString("1: Motor ON\r\n");
            break;
            
        case '2':  // 停止电机
            Motor_SetSpeed(0);
            UART_SendString("2: Motor OFF\r\n");
            break;
            
        case '3':  // 舵机打开
            Servo_SetAngle(90);
            UART_SendString("3: Servo OPEN\r\n");
            break;
            
        case '4':  // 舵机关闭
            Servo_SetAngle(0);
            UART_SendString("4: Servo CLOSE\r\n");
            break;
            
        case '5':  // 重置警报计数
            alertCount = 0;
            OLED_DisplayStatus();
            UART_SendString("5: Alert count RESET\r\n");
            break;
            
        case '6':  // 查看系统状态
            UART_SendString("6: System STATUS - ");
            if (systemState == 1) {
                UART_SendString("Alert");
            } else {
                UART_SendString("Safe");
            }
            UART_SendString(", Alerts: ");
            
            // 发送警报计数
            if (alertCount < 10) {
                UART_SendByte('0' + alertCount);
            } else {
                // 简化处理，如果大于9就发送"9+"
                UART_SendString("9+");
            }
            UART_SendString("\r\n");
            break;
            
        default:
            break;
    }
}
