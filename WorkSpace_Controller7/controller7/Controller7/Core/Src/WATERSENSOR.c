#include "WATERSENSOR.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>



/* Upper-bank index mapping (device order -> physical 8..19) */
static const uint8_t HIGH_MAP[12] = {
    8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
};

static const float ws_mass_table[21] = {
    0.0f,    // 0
    10.0f,   // 1
    30.0f,   // 2
    50.0f,   // 3
    75.0f,   // 4
    105.0f,  // 5
    140.0f,  // 6
    195.0f,  // 7
    245.0f,  // 8
    320.0f,  // 9
    380.0f,  // 10
    450.0f,  // 11
    550.0f,  // 12
    650.0f,  // 13
    850.0f, // 14
    1000.0f, // 15
    1200.0f, // 16
    1500.0f, // 17
    1750.0f, // 18
    1950.0f, // 19
    2200.0f  // 20
};

static float WS_IndexToMass(uint8_t ws_index)
{
    if (ws_index > 20) {
        return -1.0f;   // 범위 밖
    }
    return ws_mass_table[ws_index];
}

/* Active handle used by the Quick APIs */
static WATER_HandleTypeDef *g_active_hws = NULL;

/* HAL expects a 7-bit address shifted by 1 bit on STM32F1 */
static inline uint16_t to_hal_addr(uint8_t addr7) {
    return (uint16_t)(addr7 << 1);
}

void WATER_Init(WATER_HandleTypeDef *hws,
                I2C_HandleTypeDef *i2c,
                uint8_t addr_low7,
                uint8_t addr_high7,
                uint8_t default_threshold,
                uint32_t timeout_ms)
{
    if (!hws) return;

    hws->i2c        = i2c;
    hws->addr_low   = addr_low7   ? addr_low7   : WATER_ADDR_LOW_DEF;
    hws->addr_high  = addr_high7  ? addr_high7  : WATER_ADDR_HIGH_DEF;
    hws->threshold  = default_threshold;                /* can be 0; Quick calls pass threshold anyway */
    hws->timeout_ms = timeout_ms  ? timeout_ms  : WATER_TIMEOUT_DEF;

    /* Register as the active handle by default */
    g_active_hws = hws;
}

void WATER_SetActiveHandle(WATER_HandleTypeDef *hws)
{
    g_active_hws = hws;
}

/* Initialize with built-in defaults and set as active.
 * You still need to provide the I2C handle once from your app. */
void WATER_InitDefault(I2C_HandleTypeDef *i2c)
{
    static WATER_HandleTypeDef s_hws; /* kept internal to this module */
    WATER_Init(&s_hws, i2c, WATER_ADDR_LOW_DEF, WATER_ADDR_HIGH_DEF, 0 /*default thres*/, WATER_TIMEOUT_DEF);
}

static HAL_StatusTypeDef i2c_recv(I2C_HandleTypeDef *i2c, uint8_t addr7, uint8_t *buf, uint16_t len, uint32_t to_ms)
{
    if (!i2c) return HAL_ERROR;
    return HAL_I2C_Master_Receive(i2c, to_hal_addr(addr7), buf, len, to_ms);
}

HAL_StatusTypeDef WATER_ReadRaw(WATER_HandleTypeDef *hws, uint8_t raw_phys20[20])
{
    if (!hws || !hws->i2c || !raw_phys20) return HAL_ERROR;

    uint8_t low_data[8]   = {0};
    uint8_t high_data[12] = {0};

    HAL_StatusTypeDef st1 = i2c_recv(hws->i2c, hws->addr_low,  low_data,  8,  hws->timeout_ms);
    HAL_StatusTypeDef st2 = i2c_recv(hws->i2c, hws->addr_high, high_data, 12, hws->timeout_ms);

    if (st1 != HAL_OK || st2 != HAL_OK) {
        return HAL_ERROR;
    }

    /* Remap to physical order 0..19 */
    memcpy(&raw_phys20[0], low_data, 8);
    for (uint8_t j = 0; j < 12; j++) {
        raw_phys20[ HIGH_MAP[j] ] = high_data[j];
    }

    return HAL_OK;
}

/* Compute last_on using a given threshold against raw segments. */
static int compute_last_on_with_threshold(const uint8_t raw_phys20[20], uint8_t threshold)
{
    int last_on = -1;
    for (uint8_t i = 0; i < 20; i++) {
        if (raw_phys20[i] > threshold) {
            last_on = (int)i;
        }
    }
    return last_on+1;
}

/* Convert raw segments to height (mm) using the handle's stored threshold. */
float WATER_ComputeHeightMM(const WATER_HandleTypeDef *hws, const uint8_t raw_phys20[20])
{
    if (!hws || !raw_phys20) return -1;

    int last_on = compute_last_on_with_threshold(raw_phys20, hws->threshold);
    if (last_on < 0) return 0;

    /* 5 mm pitch, 3 mm segment thickness */
    return last_on * 5 + 3;
}

/* === threshold-explicit versions === */

float WATER_ReadHeightMM_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold)
{
    uint8_t raw[20] = {0};
    if (WATER_ReadRaw(hws, raw) != HAL_OK) {
        return -1;  /* I2C failure */
    }

    int last_on = compute_last_on_with_threshold(raw, threshold);
    if (last_on < 0) return 0;

    return (last_on + 1) * 5;
}

float WATER_ReadLastOn_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold)
{
    if (!hws) return -1.0f;

    enum { N_SAMPLES = 10 };
    uint8_t raw[20];
    int counts[21] = {0};   // last_on 값 0~20 카운트
    int valid_samples = 0;

    for (int n = 0; n < N_SAMPLES; n++) {
        if (WATER_ReadRaw(hws, raw) != HAL_OK) {
            continue;  // I2C 실패 샘플은 스킵
        }

        int last_on = compute_last_on_with_threshold(raw, threshold); // 0~20
        if (last_on < 0) last_on = 0;
        if (last_on > 20) last_on = 20;

        counts[last_on]++;
        valid_samples++;
    }

    if (valid_samples == 0) {
        return -1.0f;  // 전부 I2C 실패
    }

    // 최빈값(last_on) 선택
    int best_last_on = 0;
    int best_count = counts[0];
    for (int i = 1; i <= 20; i++) {
        if (counts[i] > best_count) {
            best_count = counts[i];
            best_last_on = i;
        }
    }

    if (best_last_on <= 0) {
        return 0.0f;   // 아무 것도 감지되지 않음 -> 0 g
    }

    float mass = WS_IndexToMass((uint8_t)best_last_on);
    if (mass < 0.0f) {
        return 0.0f;
    }

    /* ============================
     * Apply random ± percentage (once)
     * ============================ */
    float percent = 0.0f;

    if (mass <= 100.0f) {
        percent = 0.10f;      // ±10%
    } else if (mass <= 1000.0f) {
        percent = 0.05f;      // ±5%
    } else {
        percent = 0.025f;     // ±2.5%
    }

    float r = ((float)(rand() % 2001) - 1000.0f) / 1000.0f; // -1.0 ~ +1.0
    float randomized = mass + (mass * percent * r);

    if (randomized < 0.0f)
        randomized = 0.0f;

    return randomized;
}
/* === Quick APIs (use active handle) === */

float WATER_ReadHeightMM_Quick(uint8_t threshold)
{
    if (!g_active_hws) return -1;
    return WATER_ReadHeightMM_Threshold(g_active_hws, threshold);
}

float WATER_ReadLastOn_Quick(uint8_t threshold)
{
    if (!g_active_hws) return -1; // no active handle -> error
    return WATER_ReadLastOn_Threshold(g_active_hws, threshold);
}
