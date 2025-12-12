#include "HX711.h"

static uint32_t initTime;
static uint32_t data;
// Init
void HX711_Init(HX711_t *hx711, GPIO_TypeDef* dt_port, uint16_t dt_pin, GPIO_TypeDef* sck_port, uint16_t sck_pin, float scale) {
	hx711->data_gpio_port = dt_port;
	hx711->data_pin = dt_pin;
	hx711->sck_gpio_port = sck_port;
	hx711->sck_pin = sck_pin;
	hx711->offset = 0;
	hx711->scale = scale;
}

// Read data
int32_t HX711_Read(HX711_t *hx711, uint32_t timeOut) {
	initTime = HAL_GetTick();

	while (HAL_GPIO_ReadPin(hx711->data_gpio_port, hx711->data_pin) == GPIO_PIN_SET){
        if (HAL_GetTick() - initTime >= timeOut) {
            return -1;
        }
	}

	data = 0;
	HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_RESET);

	for (uint8_t i = 0; i < 24; i++) {
		HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_SET);
		data = data << 1;
		if (HAL_GPIO_ReadPin(hx711->data_gpio_port, hx711->data_pin) == GPIO_PIN_SET) {
			data++;
		}
		HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_RESET);
	}

	HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_RESET);

	if (data & 0x800000) {
		data |= 0xFF000000;
	}

	return (int32_t)data;
}

// Tare
float HX711_Tare(HX711_t *hx711, uint8_t num, uint32_t timeOut) {
	int32_t sum = 0;
	int32_t temp = 0;
	for (uint8_t i = 0; i < num; i++) {
		temp = HX711_Read(hx711, timeOut);
		if (temp == -1){
			return -1.0f;
		}
		sum += temp;
		HAL_Delay(10);
	}
	hx711->offset = sum / num;
	return 0;
}

// Get value
float HX711_Get_Value(HX711_t *hx711, uint8_t num, uint32_t timeOut) {
	int32_t sum = 0;
	int32_t temp = 0;
	for (uint8_t i = 0; i < num; i++) {
		temp = HX711_Read(hx711, timeOut);
		if (temp == -1){
			return -1.0f;
		}
		sum += temp;
		HAL_Delay(10);
	}
	float average = (float)sum / (float)num;
	return (average - hx711->offset) / hx711->scale;
}
