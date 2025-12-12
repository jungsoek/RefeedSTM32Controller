#ifndef POSTUCO_H
#define POSTUCO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//======================================================
// 외부 전역 변수 선언
//======================================================
extern char data_PostSimCom[512];     // SIM POST 전송 버퍼
extern const char *PhoneNum;          // 전화번호
extern const char *countryDialCode;   // 국가 다이얼 코드
extern const char *ID;                // 장치 ID
extern float oil_weight;              // 유량/오일 값
extern float water_weight;            // 물량 값
extern const char *url_postuco;       // POST URL
extern int FUNCTION;
extern const int SPACE;
extern char SIM_data[512];            // SIM 응답 버퍼

//======================================================
// 외부 함수 선언
//======================================================
float Ultra_ReadDistance(void);       // 센서 값 읽기
float Read_Voltage(void);             // 배터리 전압 읽기
int SIM_Wakeup(int timeout_ms);
int SIMCom_Post(const char *data, const char *url, int timeout_ms);
void SIM_Sleep(int timeout_ms);
void LOG(const char *tag, const char *msg);

//======================================================
// Payload Builder
//======================================================
bool build_post_uco_payload(char *out, size_t out_size,
                            const char *phone, const char *dial,
                            const char *id, float oil, float water);

//======================================================
// SIM POST 요청 결과 타입
//======================================================
typedef enum {
    SIM_OK,
    SIM_FAIL,
    SIM_REJECT,
    SIM_UNDEFINED
} SimPostResult;

//======================================================
// SIM POST 요청 처리
//======================================================
SimPostResult sim_post_request(const char *url, const char *payload,
                               uint32_t timeoutMs);

//======================================================
// UCO 데이터 POST
//======================================================
void PostUCO(void);

#endif // POSTUCO_H
