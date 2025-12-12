//======================================================
// UnlockDoor.c
//======================================================

#include "Inc/UnlockDoor.h"

//======================================================
// 잠금장치 해제 기능 (UNLOCK DOOR)
//======================================================
void UnlockDoor(void)
{
    // 1) Unlock pulse
    HAL_GPIO_WritePin(LOCK_GPIO_Port, LOCK_Pin, GPIO_PIN_SET);
    HAL_Delay(500);   // unlock 유지
    HAL_GPIO_WritePin(LOCK_GPIO_Port, LOCK_Pin, GPIO_PIN_RESET);

    // 2) 결과 전송
    LOG("[ANS]", "OK");

    FUNCTION = SPACE;
}


//======================================================
//                    LEGACY CODE
//======================================================
///* unlock big door for driver */
//HAL_GPIO_WritePin(LOCK_GPIO_Port, LOCK_Pin, GPIO_PIN_SET);
//HAL_Delay(500);
//HAL_GPIO_WritePin(LOCK_GPIO_Port, LOCK_Pin, GPIO_PIN_RESET);
///*Transmit to header*/
//LOG("[ANS]", "OK");
//FUNCTION = SPACE;
