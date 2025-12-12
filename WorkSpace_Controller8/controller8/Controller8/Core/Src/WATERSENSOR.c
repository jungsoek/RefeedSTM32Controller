#include "WATERSENSOR.h"
#include <string.h>
#include <stdlib.h>

/* Upper-bank index mapping (device order -> physical 8..19) */
static const uint8_t HIGH_MAP[12] = {
    8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
};

/* Runtime WS mass table (0..20).
 * Initial values will be filled by CAL_Init() via WATER_SetMassTable().
 */
static float g_ws_mass_table[WS_MAX_POINTS] = {0.0f};

/* Active handle used by the Quick APIs */
static WATER_HandleTypeDef *g_active_hws = NULL;

/* HAL expects a 7-bit address shifted by 1 bit on STM32F1 */
static inline uint16_t to_hal_addr(uint8_t addr7) {
    return (uint16_t)(addr7 << 1);
}

/* Index -> mass lookup */
static float WS_IndexToMass(uint8_t ws_index)
{
    if (ws_index >= WS_MAX_POINTS) {
        return -1.0f;
    }
    return g_ws_mass_table[ws_index];
}

/* ================================
 *  Set/Get WS Table
 * ================================ */

void WATER_SetMassTable(const float *table, int count)
{
    if (!table) return;

    int n = (count > WS_MAX_POINTS) ? WS_MAX_POINTS : count;
    for (int i = 0; i < n; ++i) {
        g_ws_mass_table[i] = table[i];
    }
    /* For any remaining entries, keep as is or 0.0f */
    for (int i = n; i < WS_MAX_POINTS; ++i) {
        /* leave as previous or zero */
    }
}

/* ================================
 *  Init / Raw Read
 * ================================ */

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
    hws->threshold  = default_threshold;
    hws->timeout_ms = timeout_ms  ? timeout_ms  : WATER_TIMEOUT_DEF;

    g_active_hws = hws;
}

void WATER_SetActiveHandle(WATER_HandleTypeDef *hws)
{
    g_active_hws = hws;
}

void WATER_InitDefault(I2C_HandleTypeDef *i2c)
{
    static WATER_HandleTypeDef s_hws;
    WATER_Init(&s_hws, i2c,
               WATER_ADDR_LOW_DEF, WATER_ADDR_HIGH_DEF,
               0, WATER_TIMEOUT_DEF);
}

static HAL_StatusTypeDef i2c_recv(I2C_HandleTypeDef *i2c, uint8_t addr7,
                                  uint8_t *buf, uint16_t len, uint32_t to_ms)
{
    if (!i2c) return HAL_ERROR;
    return HAL_I2C_Master_Receive(i2c, to_hal_addr(addr7), buf, len, to_ms);
}

HAL_StatusTypeDef WATER_ReadRaw(WATER_HandleTypeDef *hws,
                                uint8_t raw_phys20[20])
{
    if (!hws || !hws->i2c || !raw_phys20) return HAL_ERROR;

    uint8_t low_data[8]   = {0};
    uint8_t high_data[12] = {0};

    HAL_StatusTypeDef st1 = i2c_recv(hws->i2c, hws->addr_low,
                                     low_data, 8, hws->timeout_ms);
    HAL_StatusTypeDef st2 = i2c_recv(hws->i2c, hws->addr_high,
                                     high_data, 12, hws->timeout_ms);

    if (st1 != HAL_OK || st2 != HAL_OK) {
        return HAL_ERROR;
    }

    memcpy(&raw_phys20[0], low_data, 8);
    for (uint8_t j = 0; j < 12; j++) {
        raw_phys20[ HIGH_MAP[j] ] = high_data[j];
    }

    return HAL_OK;
}

/* ================================
 *  Helper: last-on index
 * ================================ */

static int compute_last_on_with_threshold(const uint8_t raw_phys20[20],
                                          uint8_t threshold)
{
    int last_on = -1;
    for (uint8_t i = 0; i < 20; i++) {
        if (raw_phys20[i] > threshold) {
            last_on = (int)i;
        }
    }
    return last_on + 1;   // 1..20, or 0 if none
}

/* ================================
 *  Height / Index / Mass APIs
 * ================================ */

float WATER_ComputeHeightMM(const WATER_HandleTypeDef *hws,
                            const uint8_t raw_phys20[20])
{
    if (!hws || !raw_phys20) return -1.0f;

    int last_on = compute_last_on_with_threshold(raw_phys20, hws->threshold);
    if (last_on < 0) return 0.0f;

    /* 5 mm pitch, 3 mm segment thickness */
    return last_on * 5 + 3;
}

float WATER_ReadHeightMM_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold)
{
    uint8_t raw[20] = {0};
    if (WATER_ReadRaw(hws, raw) != HAL_OK) {
        return -1.0f;
    }

    int last_on = compute_last_on_with_threshold(raw, threshold);
    if (last_on < 0) return 0.0f;

    return (last_on + 1) * 5;
}

/* Returns index 0..20 (0 means no segment on), or -1 on I2C error. */
int WATER_ReadLastIndex_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold)
{
    uint8_t raw[20];
    if (WATER_ReadRaw(hws, raw) != HAL_OK) {
        return -1;
    }

    int last_on = compute_last_on_with_threshold(raw, threshold);
    if (last_on < 0) {
        return 0;
    }
    return last_on;
}

/* Returns water mass (g) with random ±% for normal operation. */
float WATER_ReadLastOn_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold)
{
    uint8_t raw[20];
    if (WATER_ReadRaw(hws, raw) != HAL_OK) {
        return -1.0f;
    }

    int last_on = compute_last_on_with_threshold(raw, threshold);
    if (last_on < 0) {
        return 0.0f;
    }

    float mass = WS_IndexToMass((uint8_t)last_on);
    if (mass < 0.0f) {
        return 0.0f;
    }

    /* Random ±% noise */
    float percent;
    if (mass <= 100.0f) {
        percent = 0.10f;    // ±10%
    } else if (mass <= 1000.0f) {
        percent = 0.05f;    // ±5%
    } else {
        percent = 0.025f;   // ±2.5%
    }

    float r = ((float)(rand() % 2001) - 1000.0f) / 1000.0f; // -1.0..+1.0
    float randomized = mass + (mass * percent * r);
    if (randomized < 0.0f) randomized = 0.0f;

    return randomized;
}

/* Quick APIs */

float WATER_ReadHeightMM_Quick(uint8_t threshold)
{
    if (!g_active_hws) return -1.0f;
    return WATER_ReadHeightMM_Threshold(g_active_hws, threshold);
}

int WATER_ReadLastIndex_Quick(uint8_t threshold)
{
    if (!g_active_hws) return -1;
    return WATER_ReadLastIndex_Threshold(g_active_hws, threshold);
}

float WATER_ReadLastOn_Quick(uint8_t threshold)
{
    if (!g_active_hws) return -1.0f;
    return WATER_ReadLastOn_Threshold(g_active_hws, threshold);
}
