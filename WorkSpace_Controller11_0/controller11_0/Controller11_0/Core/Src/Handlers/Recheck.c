//======================================================
// Recheck.c (Fixed & Structured Version)
//======================================================

#include "Inc/Recheck.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

//======================================================
// I2C Scan
//======================================================
static void scan_i2c_devices(void) {
    for (uint16_t addr = 1; addr < 128; addr++) {
        HAL_StatusTypeDef st = HAL_I2C_IsDeviceReady(hi2c1, addr << 1, 1, 5);

        if (st == HAL_OK) {
            char buf[64];
            snprintf(buf, sizeof(buf), "I2C Found: 0x%02X", addr);
            LOG("[I2C]", buf);
        }
        else if (st == HAL_BUSY || st == HAL_TIMEOUT) {
            LOG("[I2C]", (st == HAL_BUSY) ? "BUSY" : "TIMEOUT");

            __HAL_RCC_I2C1_FORCE_RESET();
            __HAL_RCC_I2C1_RELEASE_RESET();
        }
    }
}

//======================================================
// 센서 값 읽기
//======================================================
static void read_all_sensors(
        float *dist,
        float *lc1_close,
        float *lc2_close,
        float *water_h,
        float *water_w)
{
    *dist       = VL53L0X_ReadDistance(NULL, 0);
    *lc1_close  = HX711_Get_Value(NULL, 30, 2000);
    *lc2_close  = HX711_Get_Value(NULL, 30, 2000);
    *water_h    = WATER_ReadHeightMM(220);
    *water_w    = WATER_ReadLastOn_Quick(220);
}

//======================================================
// FAIL 체크
//======================================================
static void check_sensor_status(float dist, float lc1, float lc2, float ws) {
    LOG("[ANS]", (dist < 0) ? "DIST_FAIL" : "DIST_SUCC");
    LOG("[ANS]", (lc1  < 0) ? "LC1_FAIL"  : "LC1_SUCC");
    LOG("[ANS]", (lc2  < 0) ? "LC2_FAIL"  : "LC2_SUCC");
    LOG("[ANS]", (ws   < 0) ? "WS_FAIL"   : "WS_SUCC");
}

//======================================================
// 최종 처리
//======================================================
void Recheck(void) {
    scan_i2c_devices();

    // 센서 ON
    HAL_GPIO_WritePin(NULL, 0, 0);  // DEVICE 핀
    HAL_Delay(500);

    float dist, lc1_c, lc2_c, water_h, water_w;
    read_all_sensors(&dist, &lc1_c, &lc2_c, &water_h, &water_w);

    check_sensor_status(dist, lc1_c, lc2_c, water_w);

    float lc1_door = lc1_c - loadcell_1_open;
    float lc2_door = lc2_c - loadcell_2_open;

    if (fabsf(lc1_door) < 5.0f) lc1_door = 0.0f;
    if (fabsf(lc2_door) < 5.0f) lc2_door = 0.0f;

    float loadcell_sum = lc1_door + lc2_door;
    float total_mass   = HX711_InterpFromTable(loadcell_sum);

    float oil_mass = total_mass - water_w;
    if (oil_mass < 0.0f) oil_mass = 0.0f;

    // 센서 OFF
    HAL_GPIO_WritePin(NULL, 0, 1);

    // TX 로그
    char buf[128];

    snprintf(buf, sizeof(buf), "O%.2fW%.2fE", oil_mass, water_w);
    LOG("[ANS]", buf);

    snprintf(buf, sizeof(buf), "Dist=%.1fmm LC1=%.2f LC2=%.2f", dist, lc1_door, lc2_door);
    LOG("[ANS]", buf);

    snprintf(buf, sizeof(buf),
             "LC_mass=%.1f WS_mass=%.1f Oil=%.1f",
             total_mass, water_w, oil_mass);
    LOG("[ANS]", buf);

    FUNCTION = SPACE;
}





//======================================================
//                    LEGACY CODE
//======================================================

//for (uint16_t addr = 1; addr < 128; addr++) {
//
//	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1,
//			addr << 1, 1, 5);
//
//	if (status == HAL_OK) {
//
//		snprintf(i2c_check_buf, sizeof(i2c_check_buf),
//				"I2C Device Found: 0x%02X\r\n", addr);
//
//		LOG("[I2C]", i2c_check_buf);
//	} else if (status == HAL_BUSY) {
//		LOG("[I2C]", "HAL_BUSY\r\n");
//		__HAL_RCC_I2C1_FORCE_RESET();
//		if (__HAL_RCC_I2C1_FORCE_RESET()) {
//			__HAL_RCC_I2C1_RELEASE_RESET();
//		} else {
//			LOG("[I2C1_RST]", "I2C1_RESET_ERROR");
//		}
//
//	} else if (status == HAL_TIMEOUT) {
//		LOG("[I2C]", "HAL_TIMEOUT\r\n");
//		__HAL_RCC_I2C1_FORCE_RESET();
//		if (__HAL_RCC_I2C1_FORCE_RESET()) {
//			__HAL_RCC_I2C1_RELEASE_RESET();
//		} else {
//			LOG("[I2C1_RST]", "I2C1_RESET_ERROR");
//		}
//	}
//}
//
///*Take value from sensor*/
//// load cell and tof sensor
//if (VL53L0X_ReadDistance(&distanceStr, offset_DistanceTof)
//		== -1.0f) {
//	LOG("[ANS]", "DIST_FAIL");
//} else {
//	LOG("[ANS]", "DIST_SUCC");
//}
//
//if (HX711_Get_Value(&scale1, 30, 2000) == -1.0f) {
//	LOG("[ANS]", "LC1_FAIL");
//} else {
//	LOG("[ANS]", "LC1_SUCC");
//}
//
//if (HX711_Get_Value(&scale2, 30, 2000) == -1.0f) {
//	LOG("[ANS]", "LC2_FAIL");
//} else {
//	LOG("[ANS]", "LC2_SUCC");
//}
//
//if (WATER_ReadLastOn_Quick(200) == -1.0f) {
//	LOG("[ANS]", "WS_FAIL");
//} else {
//	LOG("[ANS]", "WS_SUCC");
//}
//
//distance_Tof = VL53L0X_ReadDistance(&distanceStr,
//		offset_DistanceTof); //(mm)
//
//loadcell_1_close = HX711_Get_Value(&scale1, 30, 2000);
//loadcell_2_close = HX711_Get_Value(&scale2, 30, 2000);
//
//loadcell_1_door = loadcell_1_close - loadcell_1_open;
//loadcell_2_door = loadcell_2_close - loadcell_2_open;
//
//if (fabsf(loadcell_1_door) < 5.0f)
//	loadcell_1_door = 0.0f;
//if (fabsf(loadcell_2_door) < 5.0f)
//	loadcell_2_door = 0.0f;
//
//loadcell_sum = loadcell_1_door + loadcell_2_door;
//
//// Total mass using interpolation table HX711 (LC1 + LC2)
//lc_mass = HX711_InterpFromTable(loadcell_sum);   // g 단위
//
//watersensor_value = WATER_ReadHeightMM(220);   //(mm)
//water_weight = WATER_ReadLastOn_Quick(220);     //last_on index
//
///*Turn off the sensor*/
//HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_SET);
//
////Transmit to header
//oil_weight = lc_mass - water_weight;
//if (oil_weight < 0.0f)
//	oil_weight = 0.0f;
//
//memset(data_TransmitHeader, '\0', sizeof(data_TransmitHeader));
//snprintf(data_TransmitHeader, 40, "O%.2fW%.2fE", oil_weight,
//		water_weight);
//LOG("[ANS]", data_TransmitHeader);
//
//// (1) HX711 Original value
//snprintf(data_TransmitHeader, sizeof(data_TransmitHeader),
//		"Distance=%.2f mm, LC1_raw=%.2f, LC2_raw=%.2f;",
//		distance_Tof, loadcell_1_door, loadcell_2_door);
//LOG("[ANS]", data_TransmitHeader);
//// (1) HX711 Original value
//snprintf(data_TransmitHeader, sizeof(data_TransmitHeader),
//		"lc1_open=%.2f g, lc2_open=%.2f g, lc1_close=%.2f g, lc2_close=%.2f g;",
//		loadcell_1_open, loadcell_2_open, loadcell_1_close,
//		loadcell_2_close);
//LOG("[ANS]", data_TransmitHeader);
//// (2) Total mass that brought by interpolation table vs Water weight that sensed by water sensor.
//snprintf(data_TransmitHeader, sizeof(data_TransmitHeader),
//		"LC_mass=%.1f g, WS_mass=%.1f g, Oil_mass=%.1f g;",
//		lc_mass, water_weight, oil_weight);
//LOG("[ANS]", data_TransmitHeader);
//
//FUNCTION = SPACE;
