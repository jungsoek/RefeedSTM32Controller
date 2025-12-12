#ifndef INC_HX711_H_
#define INC_HX711_H_

#include "main.h"

// Struct
typedef struct {
	GPIO_TypeDef* data_gpio_port;
	uint16_t data_pin;
	GPIO_TypeDef* sck_gpio_port;
	uint16_t sck_pin;
	int32_t offset;
	float scale;
} HX711_t;

// Function
void HX711_Init(HX711_t *hx711, GPIO_TypeDef* dt_port, uint16_t dt_pin, GPIO_TypeDef* sck_port, uint16_t sck_pin, float scale);
float HX711_Tare(HX711_t *hx711, uint8_t num, uint32_t timeOut);
float HX711_Get_Value(HX711_t *hx711, uint8_t num, uint32_t timeOut);
#endif /* INC_HX711_H_ */
