#ifndef HANDSHAKE_H
#define HANDSHAKE_H

#include "main.h"
#include <stdbool.h>
#include "hx711.h"       // HX711_t 구조체 정의
#include "vl53l0x.h"     // statInfo_t_VL53L0X 정의

extern statInfo_t_VL53L0X distanceStr;
// =======================
// Enum 정의
// =======================
typedef enum {
    HANDSHAKE,
    VALIDATION,
    POST_UCO,
    POST_PER,
    OPEN_INPUT,
    CLOSE_INPUT,
    RECHECK,
    UNLOCK_DOOR,
    SLEEP,
    ALARM_WAKEUP,
    EXTI_WAKEUP,
    OPEN_VALVE,
    CLOSE_VALVE,
    TURN_OFF,
    TEST_PERCENT,
    TEST_MEASURE,
    TEST_SDCLOSE,
    TEST_SDOPEN,
    SPACE,
    SETTING
} Msg_Header;

// 에러코드 비트
#define ERR_SIM         (1 << 0)
#define ERR_LASER       (1 << 1)
#define ERR_ULTRA       (1 << 2)
#define ERR_LOADCELL    (1 << 3)
#define ERR_WATER       (1 << 4)

// =======================
// extern 변수
// =======================
extern Msg_Header FUNCTION;
extern I2C_HandleTypeDef hi2c1;
extern statInfo_t_VL53L0X distanceStr;
extern float calibration_factor;

extern HX711_t scale1;
extern HX711_t scale2;

// =======================
// 함수 프로토타입
// =======================
void Handshake(void);
bool CheckSIM(void);
bool CheckLaser(void);
bool CheckUltra(void);
bool CheckLoadcell(void);
bool CheckWater(void);

// =======================
// 전역 변수 정의
// =======================
Msg_Header FUNCTION;
float calibration_factor = 200.0f;
extern float loadcell_1_handshake;
extern float loadcell_2_handshake;
static uint16_t error_code = 0;

// 외부 정의
extern I2C_HandleTypeDef hi2c1;
extern HX711_t scale1, scale2;

// =======================
// LOG 매크로
// =======================
#define LOG(tag, msg) printf("%s %s\r\n", tag, msg)

HAL_StatusTypeDef I2C_Recover(I2C_HandleTypeDef *hi2c);
void I2C_DumpDevices(I2C_HandleTypeDef *hi2c);

#endif
