#include "WATERSENSOR.h"
#include <string.h>

/* Upper-bank index mapping (device order -> physical 8..19) */
static const uint8_t HIGH_MAP[12] = {
    8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
};

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
    return last_on + 1;
}

/* Convert raw segments to height (mm) using the handle's stored threshold. */
int WATER_ComputeHeightMM(const WATER_HandleTypeDef *hws, const uint8_t raw_phys20[20])
{
    if (!hws || !raw_phys20) return -1;

    int last_on = compute_last_on_with_threshold(raw_phys20, hws->threshold);
    if (last_on < 0) return 0;

    /* 5 mm pitch, 3 mm segment thickness */
    return last_on * 5 + 3;
}

/* === threshold-explicit versions === */

int WATER_ReadHeightMM_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold)
{
    uint8_t raw[20] = {0};
    if (WATER_ReadRaw(hws, raw) != HAL_OK) {
        return -1;  /* I2C failure */
    }

    int last_on = compute_last_on_with_threshold(raw, threshold);
    if (last_on < 0) return 0;

    return last_on * 5 + 3;
}

int WATER_ReadLastOn_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold)
{
    uint8_t raw[20];
    if (WATER_ReadRaw(hws, raw) != HAL_OK) {
        return -1;  // I2C failure -> error
    }
    int last_on = compute_last_on_with_threshold(raw, threshold);
    if (last_on < 0) {
        return 0;   // none -> 0
    }
    return last_on + 1; // 1..19
}

/* === Quick APIs (use active handle) === */

int WATER_ReadHeightMM_Quick(uint8_t threshold)
{
    if (!g_active_hws) return -1;
    return WATER_ReadHeightMM_Threshold(g_active_hws, threshold);
}

int WATER_ReadLastOn_Quick(uint8_t threshold)
{
    if (!g_active_hws) return -1; // no active handle -> error
    return WATER_ReadLastOn_Threshold(g_active_hws, threshold);
}
