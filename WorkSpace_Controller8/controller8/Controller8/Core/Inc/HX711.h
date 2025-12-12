#ifndef INC_HX711_H_
#define INC_HX711_H_

#include "main.h"   // includes HAL + stdint

#ifdef __cplusplus
extern "C" {
#endif

/* ================================
 *  Constants
 * ================================ */
#define HX711_MAX_MASS        2500.0f   // Maximum mass limit (g)
#define HX711_MAX_CAL_POINTS  64        // Max number of calibration points

/* ================================
 *  Calibration Point Type
 * ================================ */
typedef struct {
    float sum;    // LC1 + LC2 (raw or scaled) delta value
    float mass;   // Actual mass (g)
} LC_CalibPoint;

/* ================================
 *  HX711 Handle
 * ================================ */

typedef struct {
    GPIO_TypeDef* data_gpio_port;
    uint16_t      data_pin;
    GPIO_TypeDef* sck_gpio_port;
    uint16_t      sck_pin;
    int32_t       offset;
    float         scale;
} HX711_t;

/* ================================
 *  Basic HX711 Functions
 * ================================ */

/**
 * @brief Initialize one HX711 channel.
 */
void HX711_Init(HX711_t *hx711,
                GPIO_TypeDef* dt_port, uint16_t dt_pin,
                GPIO_TypeDef* sck_port, uint16_t sck_pin,
                float scale);

/**
 * @brief Tare (zero) the load cell by averaging N samples.
 *
 * @return 0 on success, negative value on timeout/error.
 */
float HX711_Tare(HX711_t *hx711, uint8_t num, uint32_t timeOut);

/**
 * @brief Get calibrated value from HX711 (raw -> weight unit).
 *
 * @return Calibrated value or negative on error.
 */
float HX711_Get_Value(HX711_t *hx711, uint8_t num, uint32_t timeOut);

/* ================================
 *  Calibration Table Control
 * ================================ */

/**
 * @brief  Set the calibration table used by HX711_Get_CombinedMass().
 *         The table is copied into an internal buffer inside HX711.c.
 *
 * @param  table  Pointer to calibration points array.
 * @param  count  Number of points in the array.
 */
void HX711_SetCalibTable(const LC_CalibPoint *table, int count);

/* ================================
 *  Combined Two Load Cells
 * ================================ */

/**
 * @brief  Read two HX711 channels, add them (v1+v2),
 *         then convert that sum into mass using the current calibration table.
 *
 * @return Calibrated mass (g), or -1.0f on error.
 */
float HX711_Get_CombinedMass(HX711_t *hx1,
                             HX711_t *hx2,
                             uint8_t num,
                             uint32_t timeOut);

#ifdef __cplusplus
}
#endif

#endif /* INC_HX711_H_ */
