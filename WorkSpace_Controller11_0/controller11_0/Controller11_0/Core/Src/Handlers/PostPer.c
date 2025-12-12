
//======================================================
// PostPer.c
//======================================================

#include "Inc/PostPer.h"
#include <stdio.h>
#include <string.h>

//======================================================
// Payload Builder (Periodical)
//======================================================
bool build_post_periodic_payload(char *out, size_t out_size,
                                 const char *id, float volume, float battery) {
    if (!out || out_size == 0) return false;

    const char *p_id = id ? id : "";

    int written = snprintf(out, out_size,
            "{\"machineId\":\"%s\",\"volume\":%.2f,\"battery\":%.2f}\r",
            p_id, volume, battery);

    if (written < 0) {
        LOG("[ERR]", "SNPRINTF_FAIL");
        return false;
    }
    if ((size_t)written >= out_size) {
        LOG("[ERR]", "PAYLOAD_OVERFLOW");
        return false;
    }
    return true;
}

//======================================================
// 주기 데이터 POST (기존 SIMCom_Post 기반)
//======================================================
void PostPeriodic(void) {
    memset(data_PostSimCom, 0, sizeof(data_PostSimCom));

    // 1) 센서 값 읽기
    float volume  = Ultra_ReadDistance();
    float battery = Read_Voltage();

    // 2) Payload 생성
    if (!build_post_periodic_payload(data_PostSimCom, sizeof(data_PostSimCom),
                                     ID, volume, battery)) {
        LOG("[ANS]", "PAYLOAD_ERROR");
        FUNCTION = SPACE;
        return;
    }

    LOG("[DEBUG]", data_PostSimCom);

    // 3) SIM 통신
    if (SIM_Wakeup(3000)) {
        LOG("[ANS]", "FAIL");
        FUNCTION = SPACE;
        return;
    }

    if (!SIMCom_Post(data_PostSimCom, url_postper, 5000)) {
        if (strstr((char*)SIM_data, "true")) {
            LOG("[ANS]", "OK");
        } else if (strstr((char*)SIM_data, "false")) {
            LOG("[ANS]", "REJECT");
        } else {
            LOG("[ANS]", "UNDEFINED");
        }
    } else {
        LOG("[ANS]", "FAIL");
    }

    SIM_Sleep(3000);
    FUNCTION = SPACE;
}




//======================================================
//                    LEGACY CODE
//======================================================
//memset(data_PostSimCom, '\0', sizeof(data_PostSimCom));
////Prepare data
//value_VolContainer = Ultra_ReadDistance();
//value_Voltage = Read_Voltage();
////Prepare data
//snprintf(data_PostSimCom, sizeof(data_PostSimCom),
//		"{\"machineId\": \"%s\", \"volume\": %.2f, \"battery\": %.2f}\r",
//		ID, value_VolContainer, value_Voltage);
//// LOG("DEBUG", data_PostSimCom);
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
//FUNCTION = SPACE;
