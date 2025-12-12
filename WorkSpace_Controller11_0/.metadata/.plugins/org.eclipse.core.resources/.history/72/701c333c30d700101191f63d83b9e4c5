#ifndef SLEEP_H
#define SLEEP_H

#include <stdint.h>
#include <stdbool.h>

//======================================================
// 외부 전역 변수
//======================================================
extern bool flag_EXTI;

extern uint8_t LOG_buffer[];
extern uint8_t SIM_buffer[];
extern const uint16_t UART_RX_BUFFER_SIZE;

//======================================================
// GPIO, ADC, I2C, UART 핸들러
//======================================================
extern void *DEVICE_GPIO_Port;
extern uint16_t DEVICE_Pin;
extern void *LED_GPIO_Port;
extern uint16_t LED_Pin;
extern void *LOCK_GPIO_Port;
extern uint16_t LOCK_Pin;
extern void *VALVE_GPIO_Port;
extern uint16_t VALVE_Pin;
extern void *DAEGUN_LOAD_GPIO_Port;
extern uint16_t DAEGUN_LOAD_Pin;
extern void *Door_IN1_GPIO_Port;
extern uint16_t Door_IN1_Pin;
extern void *Door_IN2_GPIO_Port;
extern uint16_t Door_IN2_Pin;
extern void *U_Trig_GPIO_Port;
extern uint16_t U_Trig_Pin;
extern void *U_Echo_GPIO_Port;
extern uint16_t U_Echo_Pin;
extern void *LC_SCK_GPIO_Port;
extern uint16_t LC_SCK_Pin;
extern void *LC_Data1_GPIO_Port;
extern uint16_t LC_Data1_Pin;
extern void *LC_Data2_GPIO_Port;
extern uint16_t LC_Data2_Pin;

extern void *hi2c1;
extern void *hadc1;
extern void *hadc2;
extern void *huart1;
extern void *huart2;

//======================================================
// 외부 함수
//======================================================
extern void LOG(const char *tag, const char *msg);

extern void HAL_GPIO_WritePin(void *GPIO_Port, uint16_t GPIO_Pin, uint8_t PinState);
extern void HAL_ADC_DeInit(void *hadc);
extern void HAL_GPIO_DeInit(void *GPIO_Port, uint16_t GPIO_Pin);
extern void HAL_I2C_DeInit(void *hi2c);
extern void HAL_Delay(uint32_t ms);

extern void HAL_SuspendTick(void);
extern void HAL_ResumeTick(void);
extern void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t Entry);

extern void SystemClock_Config(void);

extern void MX_USART1_UART_Init(void);
extern void MX_USART2_UART_Init(void);
extern void MX_ADC1_Init(void);
extern void MX_ADC2_Init(void);
extern void MX_GPIO_Init(void);
extern void MX_I2C1_Init(void);

#define GPIO_PIN_RESET 0
#define GPIO_PIN_SET   1
#define PWR_LOWPOWERREGULATOR_ON 0
#define PWR_STOPENTRY_WFI 0

//======================================================
// Sleep 모드 함수
//======================================================
void SleepMode(void);

#endif // SLEEP_H
