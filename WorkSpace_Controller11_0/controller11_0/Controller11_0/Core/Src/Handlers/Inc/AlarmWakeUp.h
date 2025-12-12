#ifndef ALARMWAKEUP_H
#define ALARMWAKEUP_H

#include "main.h"
#include <stdbool.h>
#include "hx711.h"
#include "vl53l0x.h"
#include "handshake.h"   // Msg_Header 정의 포함


// extern 변수
extern I2C_HandleTypeDef hi2c1;
extern statInfo_t_VL53L0X distanceStr;
extern float calibration_factor;
extern HX711_t scale1;
extern HX711_t scale2;
extern char data_PostSimCom[150];
extern const char ID[];
extern const char url_postper[];

// 함수 프로토타입
void AlarmWakeUpMode(void);

#endif
