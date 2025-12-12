//======================================================
// AlarmWakeUp.c
//======================================================

#include "Inc/AlarmWakeUp.h"

// extern 실제 정의
// calibration_factor는 Handshake.c에서 정의하면 중복 방지
// char data_PostSimCom[150]; // main.c에서 정의

void AlarmWakeUpMode(void)
{
    uint16_t error_code = 0;

    LOG("[CMD]", "UPDATE");

    //--------------------------------------------------
    // 1) 센서 ON
    //--------------------------------------------------
    HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_RESET);

    //--------------------------------------------------
    // 2) 센서 초기화
    //--------------------------------------------------
    initVL53L0X(1, &hi2c1);

    HX711_Init(&scale1,
               LC_Data1_GPIO_Port, LC_Data1_Pin,
               LC_SCK_GPIO_Port, LC_SCK_Pin,
               calibration_factor);

    HX711_Init(&scale2,
               LC_Data2_GPIO_Port, LC_Data2_Pin,
               LC_SCK_GPIO_Port, LC_SCK_Pin,
               calibration_factor);

    //--------------------------------------------------
    // 3) 필수 센서 체크
    //--------------------------------------------------
    if (SIM_Wakeup(3000) != 0)
        error_code += 1;

    if (VL53L0X_OFFSET(&distanceStr) == -1.0f)
        error_code += 10;

    if (Ultra_ReadDistance() == -1.0f)
        error_code += 100;

    if (HX711_Tare(&scale1, 10, 2000) == -1.0f ||
        HX711_Tare(&scale2, 10, 2000) == -1.0f)
        error_code += 1000;

    //--------------------------------------------------
    // 4) 측정 데이터 준비
    //--------------------------------------------------
    float value_VolContainer = Ultra_ReadDistance();
    float value_Voltage      = Read_Voltage();

    snprintf(data_PostSimCom, sizeof(data_PostSimCom),
            "{\"machineId\": \"%s\", \"volume\": %.2f, \"battery\": %.2f}\r", ID, value_VolContainer, value_Voltage);

    //--------------------------------------------------
    // 5) SIM POST
    //--------------------------------------------------
    if (SIM_Wakeup(3000) != 0) {
        LOG("[ANS]", "FAIL");
    }

    if (!SIMCom_Post(data_PostSimCom, url_postper, 5000)) {
        if (strstr((char*)SIM_data, "true"))
            LOG("[ANS]", "OK");
        else if (strstr((char*)SIM_data, "false"))
            LOG("[ANS]", "REJECT");
        else
            LOG("[ANS]", "UNDEFINE");
    } else {
        LOG("[ANS]", "FAIL");
    }

    //--------------------------------------------------
    // 6) SIM Sleep
    //--------------------------------------------------
    SIM_Sleep(3000);

    //--------------------------------------------------
    // 7) 완료
    //--------------------------------------------------
    LOG("[ANS]", "DONE");

    //--------------------------------------------------
    // 8) 다음 상태: SLEEP
    //--------------------------------------------------
    FUNCTION = SLEEP;
}




//======================================================
//                    LEGACY CODE
//======================================================
//uint16_t error_code1 = 0;
//LOG("[CMD]", "UPDATE");
//HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_RESET); //Turn on sensor
//
//initVL53L0X(1, &hi2c1);
//HX711_Init(&scale1, LC_Data1_GPIO_Port, LC_Data1_Pin,
//LC_SCK_GPIO_Port, LC_SCK_Pin, calibration_factor);
//HX711_Init(&scale2, LC_Data2_GPIO_Port, LC_Data2_Pin,
//LC_SCK_GPIO_Port, LC_SCK_Pin, calibration_factor);
//
//if (SIM_Wakeup(3000) != 0) {
//	error_code1 += 1;
//}
//if (VL53L0X_OFFSET(&distanceStr) == -1.0f) {
//	error_code1 += 10;
//}
//if (Ultra_ReadDistance() == -1.0f) {
//	error_code1 += 100;
//}
//if ((HX711_Tare(&scale1, 10, 2000) == -1.0f)
//		|| (HX711_Tare(&scale2, 10, 2000) == -1.0f)) {
//	error_code1 += 1000;
//}
////Prepare data
//value_VolContainer = Ultra_ReadDistance();
//value_Voltage = Read_Voltage();
////Prepare data
//snprintf(data_PostSimCom, sizeof(data_PostSimCom),
//		"{\"machineId\": \"%s\", \"volume\": %.2f, \"battery\": %.2f}\r",
//		ID, value_VolContainer, value_Voltage);
////SIM_post
//if (SIM_Wakeup(3000)) {
//	LOG("[ANS]", "FAIL");
//}
//if (!SIMCom_Post(data_PostSimCom, url_postper, 5000)) {
//	if (strstr((char*) SIM_data, "true")) {
//		LOG("[ANS]", "OK");
//	} else if (strstr((char*) SIM_data, "false")) {
//		LOG("[ANS]", "REJECT");
//	} else {
//		LOG("[ANS]", "UNDEFINE");
//	}
//} else {
//	LOG("[ANS]", "FAIL");
//}
//SIM_Sleep(3000);
//LOG("[ANS]", "DONE");
//FUNCTION = SLEEP;
