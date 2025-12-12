#ifndef RECHECK_H
#define RECHECK_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>

//======================================================
// 외부 전역 변수
//======================================================
extern int FUNCTION;
extern const int SPACE;

extern float loadcell_1_open;
extern float loadcell_2_open;

extern char data_PostSimCom[512];      // Post buffer 필요시

//======================================================
// 외부 센서/통신 함수
//======================================================
float VL53L0X_ReadDistance(void *dev, int offset);
float HX711_Get_Value(void *scale, int samples, int timeout);
float HX711_InterpFromTable(float value);
float WATER_ReadHeightMM(int timeout);
float WATER_ReadLastOn_Quick(int timeout);

void LOG(const char *tag, const char *msg);
void HAL_GPIO_WritePin(void *GPIO_Port, uint16_t GPIO_Pin, uint8_t PinState);
void HAL_Delay(uint32_t ms);

//======================================================
// I2C 핸들링
//======================================================
typedef enum {
    HAL_OK = 0,
    HAL_ERROR,
    HAL_BUSY,
    HAL_TIMEOUT
} HAL_StatusTypeDef;

extern void __HAL_RCC_I2C1_FORCE_RESET(void);
extern void __HAL_RCC_I2C1_RELEASE_RESET(void);
extern HAL_StatusTypeDef HAL_I2C_IsDeviceReady(void *hi2c, uint16_t devAddr,
                                                uint32_t trials, uint32_t timeout);

extern void *hi2c1;  // I2C 핸들러

//======================================================
// Recheck 함수
//======================================================
void Recheck(void);

#endif // RECHECK_H
