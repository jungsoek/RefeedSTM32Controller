

//======================================================
// PostUCO.c
//======================================================

#include "Inc/PostUCO.h"
#include <stdio.h>
#include <string.h>

//======================================================
// Payload Builder
//======================================================
bool build_post_uco_payload(char *out, size_t out_size,
                            const char *phone, const char *dial,
                            const char *id, float oil, float water) {
    if (!out || out_size == 0) return false;

    const char *p_phone = phone ? phone : "";
    const char *p_dial  = dial  ? dial  : "";
    const char *p_id    = id    ? id    : "";

    int written = snprintf(out, out_size,
            "{\"phoneNumber\":\"%s\",\"countryDialCode\":\"%s\","
            "\"machineId\":\"%s\",\"oil\":%.2f,\"water\":%.2f}\r",
            p_phone, p_dial, p_id, oil, water);

    if (written < 0) {
        LOG("[ERR]", "PAYLOAD_SNPRINTF_FAIL");
        return false;
    }
    if ((size_t)written >= out_size) {
        LOG("[ERR]", "PAYLOAD_OVERFLOW");
        return false;
    }

    return true;
}

//======================================================
// SIM POST 요청
//======================================================
SimPostResult sim_post_request(const char *url, const char *payload,
                               uint32_t timeoutMs) {
    if (!url || !payload) {
        LOG("[SIM]", "BAD_ARGS");
        return SIM_FAIL;
    }

    if (SIM_Wakeup(3000) != 0) {
        LOG("[SIM]", "WAKEUP_FAIL");
        return SIM_FAIL;
    }

    if (SIMCom_Post(payload, url, timeoutMs) != 0) {
        LOG("[SIM]", "POST_FAIL");
        SIM_Sleep(3000);
        return SIM_FAIL;
    }

    char *resp = (char*) SIM_data;
    if (!resp) {
        LOG("[SIM]", "NO_RESPONSE");
        SIM_Sleep(3000);
        return SIM_UNDEFINED;
    }

    if (strstr(resp, "true")) {
        LOG("[SIM]", "SERVER_OK");
        SIM_Sleep(3000);
        return SIM_OK;
    } else if (strstr(resp, "false")) {
        LOG("[SIM]", "SERVER_REJECT");
        SIM_Sleep(3000);
        return SIM_REJECT;
    } else {
        LOG("[SIM]", "SERVER_UNDEFINED");
        SIM_Sleep(3000);
        return SIM_UNDEFINED;
    }
}

//======================================================
// UCO 데이터 POST
//======================================================
void PostUCO(void) {
    memset(data_PostSimCom, 0, sizeof(data_PostSimCom));

    if (!build_post_uco_payload(data_PostSimCom, sizeof(data_PostSimCom),
            PhoneNum, countryDialCode, ID, oil_weight, water_weight)) {
        LOG("[ANS]", "PAYLOAD_ERROR");
        FUNCTION = SPACE;
        return;
    }

    LOG("[DEBUG]", data_PostSimCom);

    SimPostResult r = sim_post_request(url_postuco, data_PostSimCom, 5000);

    switch (r) {
    case SIM_OK:
        LOG("[ANS]", "OK");
        break;
    case SIM_REJECT:
        LOG("[ANS]", "REJECT");
        break;
    case SIM_UNDEFINED:
        LOG("[ANS]", "UNDEFINED");
        break;
    default:
        LOG("[ANS]", "FAIL");
        break;
    }

    FUNCTION = SPACE;
}



//======================================================
//                    LEGACY CODE
//======================================================
//memset(data_PostSimCom, '\0', sizeof(data_PostSimCom));
////prepare data
//snprintf(data_PostSimCom, sizeof(data_PostSimCom),
//		"{\"phoneNumber\": \"%s\", \"countryDialCode\": \"%s\", \"machineId\": \"%s\", \"oil\": %.2f, \"water\": %.2f}\r",
//		PhoneNum, countryDialCode, ID, oil_weight,
//		water_weight);
//// LOG("DEBUG", data_PostSimCom);
////SIM_post
//if (SIM_Wakeup(3000)) {
//	LOG("[ANS]", "FAIL");
//}
//if (!SIMCom_Post(data_PostSimCom, url_postuco, 5000)) {
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
