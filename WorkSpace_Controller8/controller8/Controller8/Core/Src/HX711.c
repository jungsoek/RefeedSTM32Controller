#include "HX711.h"

static uint32_t initTime;
static uint32_t data;

/* =========================================
 *  Internal calibration table for HX711
 *  (filled via HX711_SetCalibTable())
 * ========================================= */
static LC_CalibPoint g_hx_calib_table[HX711_MAX_CAL_POINTS];
static int           g_hx_calib_count = 0;

/* ================================
 *  Internal: Interpolation
 * ================================ */
static float HX711_InterpFromTable(float sum)
{
    if (g_hx_calib_count < 2) {
        // No valid table: cannot interpolate
        return -1.0f;
    }

    // 1) If below the first calibration point → clamp to minimum mass
    if (sum <= g_hx_calib_table[0].sum) {
        return g_hx_calib_table[0].mass;
    }

    // 2) If above the last calibration point → linear extrapolation + clamp
    const int last = g_hx_calib_count - 1;
    if (sum >= g_hx_calib_table[last].sum) {
        float x1 = g_hx_calib_table[last - 1].sum;
        float x2 = g_hx_calib_table[last].sum;
        float y1 = g_hx_calib_table[last - 1].mass;
        float y2 = g_hx_calib_table[last].mass;

        float slope = (y2 - y1) / (x2 - x1);
        float dx    = sum - x2;
        float y     = y2 + slope * dx;

        if (y > HX711_MAX_MASS) {
            y = HX711_MAX_MASS;
        }
        return y;
    }

    // 3) Inside table range → normal linear interpolation
    for (int i = 0; i < g_hx_calib_count - 1; i++) {

        float x1 = g_hx_calib_table[i].sum;
        float x2 = g_hx_calib_table[i + 1].sum;

        if (sum >= x1 && sum <= x2) {
            float y1 = g_hx_calib_table[i].mass;
            float y2 = g_hx_calib_table[i + 1].mass;

            float t = (sum - x1) / (x2 - x1);
            float y = y1 + t * (y2 - y1);

            if (y > HX711_MAX_MASS) {
                y = HX711_MAX_MASS;
            }
            return y;
        }
    }

    // Should not reach here if table is sorted correctly
    return -1.0f;
}

/* ================================
 *  Public: Set calibration table
 * ================================ */
void HX711_SetCalibTable(const LC_CalibPoint *table, int count)
{
    if (!table || count <= 0) {
        g_hx_calib_count = 0;
        return;
    }

    if (count > HX711_MAX_CAL_POINTS) {
        count = HX711_MAX_CAL_POINTS;
    }

    for (int i = 0; i < count; i++) {
        g_hx_calib_table[i] = table[i];
    }
    g_hx_calib_count = count;
}

/* ================================
 *  Basic HX711 Driver Functions
 * ================================ */

void HX711_Init(HX711_t *hx711,
                GPIO_TypeDef* dt_port, uint16_t dt_pin,
                GPIO_TypeDef* sck_port, uint16_t sck_pin,
                float scale)
{
    hx711->data_gpio_port = dt_port;
    hx711->data_pin       = dt_pin;
    hx711->sck_gpio_port  = sck_port;
    hx711->sck_pin        = sck_pin;
    hx711->offset         = 0;
    hx711->scale          = scale;
}

/**
 * @brief Read raw 24-bit data from HX711.
 *
 * @return Raw signed 24-bit value, or -1 on timeout.
 */
int32_t HX711_Read(HX711_t *hx711, uint32_t timeOut)
{
    initTime = HAL_GetTick();

    while (HAL_GPIO_ReadPin(hx711->data_gpio_port, hx711->data_pin) == GPIO_PIN_SET) {
        if (HAL_GetTick() - initTime >= timeOut) {
            return -1;
        }
    }

    data = 0;
    HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_RESET);

    for (uint8_t i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_SET);
        data <<= 1;
        if (HAL_GPIO_ReadPin(hx711->data_gpio_port, hx711->data_pin) == GPIO_PIN_SET) {
            data++;
        }
        HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_RESET);
    }

    // Set gain (channel A, 128)
    HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_RESET);

    // Sign-extend 24-bit to 32-bit
    if (data & 0x800000) {
        data |= 0xFF000000;
    }

    return (int32_t)data;
}

float HX711_Tare(HX711_t *hx711, uint8_t num, uint32_t timeOut)
{
    int32_t sum = 0;
    int32_t temp = 0;

    for (uint8_t i = 0; i < num; i++) {
        temp = HX711_Read(hx711, timeOut);
        if (temp == -1) return -1.0f;
        sum += temp;
        HAL_Delay(10);
    }

    hx711->offset = sum / num;
    return 0.0f;
}

float HX711_Get_Value(HX711_t *hx711, uint8_t num, uint32_t timeOut)
{
    int32_t sum = 0;
    int32_t temp = 0;

    for (uint8_t i = 0; i < num; i++) {
        temp = HX711_Read(hx711, timeOut);
        if (temp == -1) return -1.0f;
        sum += temp;
        HAL_Delay(10);
    }

    float average = (float)sum / (float)num;
    return (average - hx711->offset) / hx711->scale;
}

/* ================================
 *  Combined Two HX711 Loadcells
 * ================================ */

float HX711_Get_CombinedMass(HX711_t *hx1, HX711_t *hx2,
                             uint8_t num, uint32_t timeOut)
{
    float v1 = HX711_Get_Value(hx1, num, timeOut);
    if (v1 < 0.0f) return -1.0f;

    float v2 = HX711_Get_Value(hx2, num, timeOut);
    if (v2 < 0.0f) return -1.0f;

    float sum = v1 + v2;
    return HX711_InterpFromTable(sum);
}
