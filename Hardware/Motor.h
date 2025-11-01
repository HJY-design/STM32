#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
void Motor_SetSpeed(int16_t speed);
void Motor_Brake(void);
void Motor_Standby(uint8_t enable);

#endif
