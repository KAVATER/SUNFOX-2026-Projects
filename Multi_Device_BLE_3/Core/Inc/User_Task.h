#ifndef USER_TASK_H
#define USER_TASK_H
#define ADC_PACKET_SIZE 50

#include "main.h"
#include <stdint.h>
extern ADC_HandleTypeDef hadc1;
extern volatile uint32_t adcVal;

extern uint16_t ADC_Buffer[ADC_PACKET_SIZE];
extern uint8_t Packet_Ready;

void task_handle(void);
#endif /* USER_TASK_H */