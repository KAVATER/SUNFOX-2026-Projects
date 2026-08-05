#include "main.h"
#include <stdint.h>

#include "User_Task.h"

extern ADC_HandleTypeDef hadc1;
 volatile uint32_t adcVal = 0;

#define ADC_PACKET_SIZE 50

uint16_t ADC_Buffer[ADC_PACKET_SIZE];
uint8_t Packet_Ready = 0;

static uint8_t ADC_Sample_Count = 0;

void task_handle(void)
{
    //HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    adcVal = HAL_ADC_GetValue(&hadc1);
    //adc_ble = (uint16_t) adc;
    ADC_Buffer[ADC_Sample_Count] = (uint16_t) adcVal;

    if (ADC_Sample_Count >= (ADC_PACKET_SIZE - 1U)) {
      ADC_Sample_Count = 0;
      Packet_Ready = 1;
    } else {
      ADC_Sample_Count++;
    }
}
