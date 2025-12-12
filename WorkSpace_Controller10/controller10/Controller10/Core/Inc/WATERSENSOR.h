#ifndef __WATERSENSOR_H__
#define __WATERSENSOR_H__

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== Defaults ===== */
#define WATER_ADDR_LOW_DEF    0x77u
#define WATER_ADDR_HIGH_DEF   0x78u
#define WATER_TIMEOUT_DEF     50u

typedef struct {
    I2C_HandleTypeDef *i2c;   /* I2C handle */
    uint8_t  addr_low;        /* lower-bank 7-bit I2C address */
    uint8_t  addr_high;       /* upper-bank 7-bit I2C address */
    uint8_t  threshold;       /* default threshold (optional convenience) */
    uint32_t timeout_ms;      /* I2C timeout */
} WATER_HandleTypeDef;

/* Full initialization with custom parameters. Also sets the active handle. */
void WATER_Init(WATER_HandleTypeDef *hws,
                I2C_HandleTypeDef *i2c,
                uint8_t addr_low7,
                uint8_t addr_high7,
                uint8_t default_threshold,
                uint32_t timeout_ms);

/* Convenience: bind an already-initialized handle as the active one. */
void WATER_SetActiveHandle(WATER_HandleTypeDef *hws);

/* Convenience: initialize with module defaults (0x77/0x78, threshold=0, timeout=50ms)
 * and set it active. You still need to pass your I2C handle once. */
void WATER_InitDefault(I2C_HandleTypeDef *i2c);

/* Read raw 20 segments in physical order [0..19]. */
HAL_StatusTypeDef WATER_ReadRaw(WATER_HandleTypeDef *hws, uint8_t raw_phys20[20]);

/* Convert raw segments to height (mm) using the handle's stored threshold. */
float WATER_ComputeHeightMM(const WATER_HandleTypeDef *hws, const uint8_t raw_phys20[20]);

/* === APIs where you pass threshold explicitly === */
/* One-shot read: returns height (mm). */
float WATER_ReadHeightMM_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold);
/* One-shot read: returns last_on index (0..19), or -1 if none. */
float WATER_ReadLastOn_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold);

/* === Quick APIs using the active handle (set via Init/InitDefault/SetActiveHandle) === */
float WATER_ReadHeightMM_Quick(uint8_t threshold);
float WATER_ReadLastOn_Quick(uint8_t threshold);

/* Macros to match your desired calling style:
   watersensor_value = WATER_ReadHeightMM(220);
   water_number      = WATER_ReadLastOn(220); */
#define WATER_ReadHeightMM(threshold)  WATER_ReadHeightMM_Quick((threshold))
#define WATER_ReadLastOn(threshold)    WATER_ReadLastOn_Quick((threshold))

#ifdef __cplusplus
}
#endif

#endif /* __WATERSENSOR_H__ */
