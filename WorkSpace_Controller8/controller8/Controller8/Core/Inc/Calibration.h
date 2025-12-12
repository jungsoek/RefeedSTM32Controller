
/*
 * 헤더가드
 * 중복선언 방지 */
#ifndef INC_CALIBRATION_H_
#define INC_CALIBRATION_H_

/* 헤더파일 선언 */
#include "HX711.h"
#include "WATERSENSOR.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Max points */
#define CAL_WS_MAX_POINTS  21   // WS index 0..20

/* ================================
 *  Initialization / Reset
 * ================================ */

/**
 * @brief Initialize calibration module.
 *        - Load default tables (or EEPROM tables, if implemented)
 *        - Apply them to HX711 & WATERSENSOR.
 */
void CAL_Init(void);

/**
 * @brief Reset tables to built-in default (factory) values.
 *        - Does NOT erase EEPROM (you can add that if you want).
 */
void CAL_ResetToDefault(void);

/* ================================
 *  Calibration Session (Run-time)
 * ================================ */

/**
 * @brief Begin a new calibration session.
 *
 * @param empty_sum  LC1 + LC2 reading when container is empty.
 *
 * This will:
 *  - Clear working HX711 & WS tables,
 *  - Set first LC point to { sum=0, mass=0 },
 *  - Set WS index 0 mass to 0.
 */
void CAL_Begin(float empty_sum);

/**
 * @brief Add a calibration point.
 *
 * @param current_sum_raw  Current LC1 + LC2 reading (same unit as empty_sum).
 * @param added_mass       Mass (g) user has JUST added at this step.
 * @param ws_index         Current WS last-on index (0..20), or -1 if unchanged.
 *
 * Internally:
 *  - Accumulates total_mass += added_mass
 *  - sum_delta = current_sum_raw - empty_sum
 *  - Append {sum_delta, total_mass} to HX711 working table
 *  - If ws_index >= 0, store ws_mass_table[ws_index] = total_mass
 *
 * @return New number of LC points on success, -1 on error (buffer full).
 */
int CAL_AddPoint(float current_sum_raw, float added_mass, int ws_index);

/**
 * @brief Finalize calibration.
 *
 *  - Applies working HX711 table to HX711 module.
 *  - Applies WS mass table to WATERSENSOR module.
 *  - (Optionally) saves to EEPROM (hook provided).
 *
 * @return Number of LC points applied on success, -1 on error.
 */
int CAL_Finalize(void);

/* ================================
 *  Accessors (for main.c printing)
 * ================================ */

/**
 * @brief Get current HX711 calibration table (working one).
 *
 * @param out_count  If non-NULL, set to number of points.
 * @return Pointer to internal LC_CalibPoint array.
 */
const LC_CalibPoint* CAL_GetLCTable(int *out_count);

/**
 * @brief Get current WS mass table.
 *
 * @param out_count  If non-NULL, set to number of WS points (always 21).
 * @return Pointer to float array size CAL_WS_MAX_POINTS.
 */
const float* CAL_GetWSTable(int *out_count);

/* ================================
 *  EEPROM Hooks (optional)
 * ================================ */

/**
 * @brief Save current calibration tables to EEPROM / Flash.
 *        (Provide your own implementation inside Calibration.c)
 */
void CAL_SaveToEEPROM(void);

/**
 * @brief Load calibration tables from EEPROM / Flash.
 *        If no valid data, fall back to defaults.
 */
void CAL_LoadFromEEPROM(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_CALIBRATION_H_ */
