#include "main.h"

extern ADC_HandleTypeDef hadc1;

uint16_t readValue;

float Read_Voltage(void){
	 HAL_ADC_PollForConversion(&hadc1,1000);
	 readValue = HAL_ADC_GetValue(&hadc1);
	 return ((float)readValue * (11.67/2065)) + 0.36; //4095*16.5;
	 //return readValue;
}
