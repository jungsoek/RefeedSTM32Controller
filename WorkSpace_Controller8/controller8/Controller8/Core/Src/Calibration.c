#include "Calibration.h"
#include <string.h>

/* ================================
 *  Internal Working State
 * ================================ */

/* 전역 테이블: 초기값 없음 → .bss 에 올라가고 FLASH(.data) 안 씀 */
static LC_CalibPoint g_lc_table_work[HX711_MAX_CAL_POINTS];
static int           g_lc_count;              // 0으로 자동 클리어(.bss)

static float g_ws_mass_table[CAL_WS_MAX_POINTS];   // 0..20

static float g_empty_sum;    // LC1+LC2 at empty
static float g_total_mass;   // cumulative mass added

/* ================================
 *  Built-in Default (간단 버전)
 *  - 큰 const 배열 없이
 *    코드에서 소량의 기본값만 생성
 * ================================ */

static void copy_default_tables(void)
{
    /*
     * HX711: 아주 단순한 선형 기본값
     * sum ≈ mass 로 가정 (0g, 1000g 두 점만)
     * 실제 캘리브레이션은 [CAL]로 다시 할 거라
     * 여기 값은 대충 동작만 하면 됨
     */
    g_lc_count = 0;
    if (HX711_MAX_CAL_POINTS >= 2) {
        g_lc_table_work[0].sum  = 0.0f;
        g_lc_table_work[0].mass = 0.0f;

        g_lc_table_work[1].sum  = 1000.0f;
        g_lc_table_work[1].mass = 1000.0f;

        g_lc_count = 2;
    }

    /*
     * WATERSENSOR: index 0..20 에 대해
     * 대략 0, 100, 200, ... 2000g 선형 설정
     */
    for (int i = 0; i < CAL_WS_MAX_POINTS; ++i) {
        g_ws_mass_table[i] = (float)(i * 100);  // 0g, 100g, ..., 2000g
    }

    g_empty_sum  = 0.0f;
    g_total_mass = 0.0f;
}

/* ================================
 *  Public API
 * ================================ */

void CAL_Init(void)
{
    /* 나중에 EEPROM 로딩을 붙이고 싶으면
       여기서 CAL_LoadFromEEPROM() 호출하면 됨.
       지금은 간단 기본값만 사용 */
    copy_default_tables();

    /* Apply tables to HX711 & WATERSENSOR */
    HX711_SetCalibTable(g_lc_table_work, g_lc_count);
    WATER_SetMassTable(g_ws_mass_table, CAL_WS_MAX_POINTS);
}

void CAL_ResetToDefault(void)
{
    copy_default_tables();
    HX711_SetCalibTable(g_lc_table_work, g_lc_count);
    WATER_SetMassTable(g_ws_mass_table, CAL_WS_MAX_POINTS);
}

void CAL_Begin(float empty_sum)
{
    g_empty_sum  = empty_sum;
    g_total_mass = 0.0f;

    /* Reset LC working table */
    g_lc_count = 0;
    if (HX711_MAX_CAL_POINTS > 0) {
        g_lc_table_work[0].sum  = 0.0f;
        g_lc_table_work[0].mass = 0.0f;
        g_lc_count = 1;
    }

    /* Reset WS table */
    for (int i = 0; i < CAL_WS_MAX_POINTS; ++i) {
        g_ws_mass_table[i] = 0.0f;
    }
    g_ws_mass_table[0] = 0.0f;
}

int CAL_AddPoint(float current_sum_raw, float added_mass, int ws_index)
{
    if (g_lc_count >= HX711_MAX_CAL_POINTS) {
        return -1;  // buffer full
    }

    /* 1) Update cumulative mass */
    g_total_mass += added_mass;

    /* 2) Compute LC delta sum */
    float delta_sum = current_sum_raw - g_empty_sum;

    /* 3) Store new LC point */
    g_lc_table_work[g_lc_count].sum  = delta_sum;
    g_lc_table_work[g_lc_count].mass = g_total_mass;
    g_lc_count++;

    /* 4) Update WS table if valid index */
    if (ws_index >= 0 && ws_index < CAL_WS_MAX_POINTS) {
        g_ws_mass_table[ws_index] = g_total_mass;
    }

    return g_lc_count;
}

int CAL_Finalize(void)
{
    if (g_lc_count < 2) {
        return -1;  // not enough points
    }

    /* Apply to HX711 */
    HX711_SetCalibTable(g_lc_table_work, g_lc_count);

    /* Apply WS table */
    WATER_SetMassTable(g_ws_mass_table, CAL_WS_MAX_POINTS);

    /* Optionally: save to EEPROM */
    CAL_SaveToEEPROM();

    return g_lc_count;
}

const LC_CalibPoint* CAL_GetLCTable(int *out_count)
{
    if (out_count) {
        *out_count = g_lc_count;
    }
    return g_lc_table_work;
}

const float* CAL_GetWSTable(int *out_count)
{
    if (out_count) {
        *out_count = CAL_WS_MAX_POINTS;
    }
    return g_ws_mass_table;
}

/* ================================
 *  EEPROM Hooks (stub)
 * ================================ */

void CAL_SaveToEEPROM(void)
{
    /* TODO:
       - Serialize g_lc_count + g_lc_table_work[]
       - Serialize g_ws_mass_table[]
       - Write to Flash/EEPROM
    */
}

void CAL_LoadFromEEPROM(void)
{
    /* TODO:
       - Read data from Flash/EEPROM
       - If valid, fill g_lc_table_work/g_lc_count and g_ws_mass_table[]
       - Then call:
           HX711_SetCalibTable(g_lc_table_work, g_lc_count);
           WATER_SetMassTable(g_ws_mass_table, CAL_WS_MAX_POINTS);
       - If invalid, call CAL_ResetToDefault();
    */
}
