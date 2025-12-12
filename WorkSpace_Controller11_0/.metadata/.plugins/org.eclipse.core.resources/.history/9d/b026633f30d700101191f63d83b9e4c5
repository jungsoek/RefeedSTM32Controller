#ifndef VALIDATION_H
#define VALIDATION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// ======================================================
// 외부 전역 변수 선언
// ======================================================
extern char LOG_buffer[];        // UART LOG 수신 버퍼
extern char SIM_data[];          // SIM 응답 데이터 버퍼
extern const char *countryDialCode;
extern const char *ID;
extern const char *url_identity;

extern uint8_t FUNCTION;
extern const uint8_t SPACE;

// ======================================================
// 외부 함수 선언
// ======================================================
extern int SIM_Wakeup(uint32_t timeout);
extern int SIMCom_Post(const char *payload, const char *url, uint32_t timeout);
extern void SIM_Sleep(uint32_t ms);
extern void LOG(const char *tag, const char *msg);

// ======================================================
// 내부 사용 함수 (테스트 가능하도록 공개)
// ======================================================
bool parsePhoneNumber(char *out, size_t outSize);
bool isPhoneNumberValid(const char *num);
bool buildIdentityJSON(char *out, size_t outSize,
                       const char *phone, const char *dial, const char *id);
bool simWakeupCheck(uint32_t timeout);

// SIM 응답 상태 enum
typedef enum {
    SIMRES_FAIL,
    SIMRES_OK,
    SIMRES_REJECT,
    SIMRES_DRIVER_TRUE,
    SIMRES_DRIVER_FALSE
} SimResult;

SimResult simIdentityRequest(const char *json, const char *url, uint32_t timeout);
void printSimResult(SimResult r);

// ======================================================
// 메인 함수
// ======================================================
void Validation(void);

#endif // VALIDATION_H
