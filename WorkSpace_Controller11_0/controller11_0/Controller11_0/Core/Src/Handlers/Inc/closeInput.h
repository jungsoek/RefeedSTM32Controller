#ifndef CLOSEINPUT_H
#define CLOSEINPUT_H

#include "main.h"
#include "hx711.h"
#include "vl53l0x.h"
#include <stdbool.h>
#include <stddef.h>

// 외부 변수
extern I2C_HandleTypeDef hi2c1;
extern HX711_t scale1;
extern HX711_t scale2;
extern statInfo_t_VL53L0X distanceStr;
extern float loadcell_1_open;
extern float loadcell_2_open;
extern float offset_DistanceTof;
extern char i2c_check_buf[256];
extern char data_TransmitHeader[128];
extern float loadcell_1_close;
extern float loadcell_2_close;
extern float loadcell_1_door;
extern float loadcell_2_door;

// CloseInput 프로세스 함수
void DoorClose_Measure(void);

#endif
