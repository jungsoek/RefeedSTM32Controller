#ifndef POSTPER_H
#define POSTPER_H

#include <stdbool.h>
#include <stddef.h>

// 외부 변수 선언
extern char data_PostSimCom[256];
extern const char *ID;
extern const char *url_postper;
extern int FUNCTION;
extern const int SPACE;
extern char SIM_data[512];


// 외부 함수 선언
float Ultra_ReadDistance(void);
float Read_Voltage(void);
int SIM_Wakeup(int timeout_ms);
int SIMCom_Post(const char *data, const char *url, int timeout_ms);
void SIM_Sleep(int timeout_ms);
void LOG(const char *tag, const char *msg);

// Payload Builder
bool build_post_periodic_payload(char *out, size_t out_size,
                                 const char *id, float volume, float battery);

// 주기 데이터 POST
void PostPeriodic(void);

#endif // POSTPER_H
