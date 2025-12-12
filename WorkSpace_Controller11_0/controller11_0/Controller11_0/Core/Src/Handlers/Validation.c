

//======================================================
// Validation.c
//======================================================


#include "Inc/Validation.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// ======================================================
// 1) PhoneNumber Parsing
// ======================================================
bool parsePhoneNumber(char *out, size_t outSize)
{
    if (!LOG_buffer || !out || outSize == 0) return false;

    char *start = strstr(LOG_buffer, "[VALID]");
    char *end   = strstr(LOG_buffer, "ENDSTR");

    if (!start || !end || end <= start) return false;

    start += strlen("[VALID]");

    while (start < end && isspace((unsigned char)*start))
        start++;

    size_t len = end - start;

    while (len > 0 && isspace((unsigned char)start[len - 1]))
        len--;

    if (len == 0) return false;
    if (len >= outSize) len = outSize - 1;

    memcpy(out, start, len);
    out[len] = '\0';

    return true;
}

// ======================================================
// 2) Phone number validation
// ======================================================
bool isPhoneNumberValid(const char *num)
{
    if (!num || strlen(num) == 0) return false;

    for (size_t i = 0; num[i]; i++)
        if (!isdigit((unsigned char)num[i]))
            return false;

    return true;
}

// ======================================================
// 3) Build identity JSON
// ======================================================
bool buildIdentityJSON(char *out, size_t outSize,
                       const char *phone, const char *dial, const char *id)
{
    int written = snprintf(out, outSize,
        "{\"phoneNumber\":\"%s\",\"countryDialCode\":\"%s\",\"machineId\":\"%s\"}\r",
        phone ? phone : "",
        dial ? dial : "",
        id ? id : "");

    if (written < 0 || (size_t)written >= outSize) {
        LOG("[ERR]", "JSON_TRUNCATED");
        return false;
    }
    return true;
}

// ======================================================
// 4) SIM Wakeup wrapper
// ======================================================
bool simWakeupCheck(uint32_t timeout)
{
    return (SIM_Wakeup(timeout) == 0);
}

// ======================================================
// 5) SIM identity request + parsing
// ======================================================
SimResult simIdentityRequest(const char *json, const char *url, uint32_t timeout)
{
    if (!json || !url) return SIMRES_FAIL;

    if (SIMCom_Post(json, url, timeout) != 0) {
        LOG("[SIM]", "POST_FAIL");
        return SIMRES_FAIL;
    }

    if (strstr(SIM_data, "\"driver\":false"))               return SIMRES_OK;
    if (strstr(SIM_data, "\"message\":\"no user found\""))  return SIMRES_REJECT;
    if (strstr(SIM_data, "\"driver\":true"))                return SIMRES_DRIVER_TRUE;
    if (strstr(SIM_data, "\"message\":\"not permitted\""))  return SIMRES_DRIVER_FALSE;

    return SIMRES_FAIL;
}

// ======================================================
// 6) Print SIM Result
// ======================================================
void printSimResult(SimResult r)
{
    switch (r) {
        case SIMRES_OK:            LOG("[ANS]", "OK"); break;
        case SIMRES_REJECT:        LOG("[ANS]", "REJECT"); break;
        case SIMRES_DRIVER_TRUE:   LOG("[ANS]", "DRIVER:TRUE"); break;
        case SIMRES_DRIVER_FALSE:  LOG("[ANS]", "DRIVER:FALSE"); break;
        default:                   LOG("[ANS]", "FAIL"); break;
    }
}

// ======================================================
// 7) Validation 메인 함수
// ======================================================
void Validation(void)
{
    char PhoneNum[32] = {0};
    char jsonBuf[256] = {0};

    // 1) parse
    if (!parsePhoneNumber(PhoneNum, sizeof(PhoneNum))) {
        LOG("[ERR]", "PhoneNum parse failed");
        LOG("[ANS]", "FAIL");
        FUNCTION = SPACE;
        return;
    }

    // 2) validate
    if (!isPhoneNumberValid(PhoneNum)) {
        LOG("[ERR]", "Invalid PhoneNum format");
        LOG("[ANS]", "FAIL");
        FUNCTION = SPACE;
        return;
    }

    // 3) JSON
    if (!buildIdentityJSON(jsonBuf, sizeof(jsonBuf), PhoneNum, countryDialCode, ID)) {
        LOG("[ERR]", "JSON build fail");
        LOG("[ANS]", "FAIL");
        FUNCTION = SPACE;
        return;
    }

    // 4) SIM Wakeup
    if (!simWakeupCheck(3000)) {
        LOG("[ERR]", "SIM_Wakeup FAIL");
        LOG("[ANS]", "FAIL");
        FUNCTION = SPACE;
        return;
    }

    // 5) SIM POST
    SimResult r = simIdentityRequest(jsonBuf, url_identity, 5000);

    // 6) Result 출력
    printSimResult(r);

    // 7) SIM Sleep 및 FSM 초기화
    SIM_Sleep(3000);
    FUNCTION = SPACE;
}




//======================================================
//                    LEGACY CODE
//======================================================
// 1) Initializing phone number buffer
//memset(PhoneNum, '\0', sizeof(PhoneNum));
//memset(data_PostSimCom, '\0', sizeof(data_PostSimCom)); // end of 1) Initializing phone number buffer
//
//// 2) Parsing phone number from among [VALID] to ENDSTR
//{
//	char *start_PhoneNum = strstr((char*) LOG_buffer,
//			"[VALID]");
//	char *end_PhoneNum = strstr((char*) LOG_buffer, "ENDSTR");
//
//	if (start_PhoneNum != NULL && end_PhoneNum != NULL
//			&& end_PhoneNum > start_PhoneNum) {
//		start_PhoneNum += 7; // Jumping interval as "[VALID]"'s length.
//
//		size_t len = end_PhoneNum - start_PhoneNum;
//		if (len >= sizeof(PhoneNum)) {
//			len = sizeof(PhoneNum) - 1;
//		}
//		memcpy(PhoneNum, start_PhoneNum, len);
//		PhoneNum[len] = '\0';
//
//		// Print debug log to check the PhoneNum.
//		LOG("[DEBUG]", PhoneNum);
//	} else {
//		LOG("[DEBUG]", "PhoneNum parse error");
//	}
//}
//
//// 3) Generate the JSON paload that send to server.
//snprintf(data_PostSimCom, sizeof(data_PostSimCom),
//		"{\"phoneNumber\":\"%s\",\"countryDialCode\":\"%s\",\"machineId\":\"%s\"}\r",
//		PhoneNum, countryDialCode, ID);
//
//// 4) SIM Communication (It is FAKE SIM. So it isn't sent to server.)
//if (SIM_Wakeup(3000)) {
//	LOG("[ANS]", "FAIL");
//}
//
//if (!SIMCom_Post(data_PostSimCom, url_identity, 5000)) {
//	// LOG("[DEBUG]", (char*)SIM_data); // If you need check this LOG, Uncomment and check the log.
//
//	/* User case */
//	// Allowing the normal users. (driver:false)
//	if (strstr((char*) SIM_data, "\"driver\":false")) {
//		LOG("[ANS]", "OK");
//	}
//	// unsign in users
//	else if (strstr((char*) SIM_data,
//			"\"message\":\"no user found\"")) {
//		LOG("[ANS]", "REJECT");
//	}
//	/* Driver case */
//	// Driver permission allowed.
//	else if (strstr((char*) SIM_data, "\"driver\":true")) {
//		LOG("[ANS]", "DRIVER:TRUE");
//	}
//	// Driver permission denied
//	else if (strstr((char*) SIM_data,
//			"\"message\":\"not permitted\"")) {
//		LOG("[ANS]", "DRIVER:FALSE");
//	}
//	// etc...
//	else {
//		LOG("[ANS]", "FAIL");
//	}
//} else {
//	LOG("[ANS]", "FAIL");
//}
//
//SIM_Sleep(3000);
//FUNCTION = SPACE;
