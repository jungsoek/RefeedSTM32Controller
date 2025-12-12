#ifndef UNLOCKDOOR_H
#define UNLOCKDOOR_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"   // STM32F1 기준, MCU에 맞게 수정


//======================================================
// 외부 전역 변수
//======================================================
extern void *LOCK_GPIO_Port;
extern uint16_t LOCK_Pin;

extern uint8_t FUNCTION;
extern const uint8_t SPACE;

//======================================================
// 외부 함수
//======================================================
extern void LOG(const char *tag, const char *msg);
extern void HAL_Delay(uint32_t ms);

//======================================================
// UnlockDoor 함수
//======================================================
void UnlockDoor(void);

#endif // UNLOCKDOOR_H
