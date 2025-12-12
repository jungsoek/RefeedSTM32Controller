#ifndef __WATERSENSOR_H__
#define __WATERSENSOR_H__

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WATER_ADDR_LOW_DEF    0x77u
#define WATER_ADDR_HIGH_DEF   0x78u
#define WATER_TIMEOUT_DEF     50u

#define WS_MAX_POINTS         21     // index 0..20

typedef struct {
    I2C_HandleTypeDef *i2c;
    uint8_t  addr_low;
    uint8_t  addr_high;
    uint8_t  threshold;
    uint32_t timeout_ms;
} WATER_HandleTypeDef;

/* Initialization */
void WATER_Init(WATER_HandleTypeDef *hws,
                I2C_HandleTypeDef *i2c,
                uint8_t addr_low7,
                uint8_t addr_high7,
                uint8_t default_threshold,
                uint32_t timeout_ms);

void WATER_SetActiveHandle(WATER_HandleTypeDef *hws);
void WATER_InitDefault(I2C_HandleTypeDef *i2c);

/* Raw read */
HAL_StatusTypeDef WATER_ReadRaw(WATER_HandleTypeDef *hws,
                                uint8_t raw_phys20[20]);

/* Convert raw segments to height (mm) using the handle's stored threshold. */
float WATER_ComputeHeightMM(const WATER_HandleTypeDef *hws,
                            const uint8_t raw_phys20[20]);

/* Threshold-explicit APIs */
float WATER_ReadHeightMM_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold);

/* NEW: get WS last-on INDEX (0..20), for calibration use */
int   WATER_ReadLastIndex_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold);

/* Existing: get WS mass (g) with random ±% (for normal operation) */
float WATER_ReadLastOn_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold);

/* Quick APIs using active handle */
float WATER_ReadHeightMM_Quick(uint8_t threshold);
int   WATER_ReadLastIndex_Quick(uint8_t threshold);
float WATER_ReadLastOn_Quick(uint8_t threshold);

/* Macros for user-friendly call style */
#define WATER_ReadHeightMM(threshold)   WATER_ReadHeightMM_Quick((threshold))
#define WATER_ReadLastIndex(threshold)  WATER_ReadLastIndex_Quick((threshold))
#define WATER_ReadLastOn(threshold)     WATER_ReadLastOn_Quick((threshold))

/* Allow external module to set WS mass table */
void WATER_SetMassTable(const float *table, int count);

#ifdef __cplusplus
}
#endif

#endif /* __WATERSENSOR_H__ */
