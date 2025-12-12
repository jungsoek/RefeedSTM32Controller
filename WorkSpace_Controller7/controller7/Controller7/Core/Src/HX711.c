#include "HX711.h"

static uint32_t initTime;
static uint32_t data;

#define HX711_MAX_MASS  2500.0f   // Maximum mass limit (g unit)

/* ================================
 *  Calibration Table (LC1+LC2 → mass)
 * ================================*/

typedef struct {
    float sum;    // LC1 + LC2 raw sum
    float mass;   // actual mass (g)
} LC_CalibPoint;

static const LC_CalibPoint lc_table[] = {
    {   11.0f,     0.0f },
    {   50.0f,    50.0f },
    {   85.0f,   100.0f },
    {  132.0f,   150.0f },
    {  174.0f,   200.0f },
    {  216.0f,   250.0f },
    {  272.0f,   300.0f },
    {  330.0f,   350.0f },
    {  408.0f,   400.0f },
    {  476.0f,   450.0f },
    {  543.0f,   500.0f },
    {  577.0f,   550.0f },
    {  631.0f,   600.0f },
    {  713.0f,   650.0f },
    {  767.0f,   700.0f },
    {  843.0f,   750.0f },
    {  913.0f,   800.0f },
    {  974.0f,   850.0f },
    { 1040.0f,   900.0f },
    { 1124.0f,   950.0f },
    { 1171.0f,  1000.0f },
    { 1238.0f,  1050.0f },
    { 1276.0f,  1100.0f },
    { 1345.0f,  1150.0f },
    { 1415.0f,  1200.0f },
    { 1487.0f,  1250.0f },
    { 1549.0f,  1300.0f },
    { 1615.0f,  1350.0f },
    { 1689.0f,  1400.0f },
    { 1746.0f,  1450.0f },
    { 1804.0f,  1500.0f },
    { 1847.0f,  1550.0f },
    { 1896.0f,  1600.0f },
    { 1945.0f,  1650.0f },
    { 2005.0f,  1700.0f },
    { 2066.0f,  1750.0f },
    { 2103.0f,  1800.0f },
    { 2162.0f,  1850.0f },
    { 2220.0f,  1900.0f },
    { 2272.0f,  1950.0f },
    { 2316.0f,  2000.0f }
};

static const int LC_TABLE_COUNT = sizeof(lc_table) / sizeof(lc_table[0]);

/* ================================
 *  Table Interpolation Function
 * ================================*/

float HX711_InterpFromTable(float sum)
{
    // 1) If below the first calibration point → clamp to minimum
    if (sum <= lc_table[0].sum)
        return lc_table[0].mass;

    // 2) If above the last calibration point → apply linear extrapolation + max clamp
    if (sum >= lc_table[LC_TABLE_COUNT - 1].sum) {
        float x1 = lc_table[LC_TABLE_COUNT - 2].sum;
        float x2 = lc_table[LC_TABLE_COUNT - 1].sum;
        float y1 = lc_table[LC_TABLE_COUNT - 2].mass;
        float y2 = lc_table[LC_TABLE_COUNT - 1].mass;

        float slope = (y2 - y1) / (x2 - x1);  // slope between last two points
        float dx    = sum - x2;              // distance beyond the last x point
        float y     = y2 + slope * dx;       // extrapolated mass value

        // Clamp to maximum allowed mass
        if (y > HX711_MAX_MASS) {
            y = HX711_MAX_MASS;
        }
        return y;
    }

    // 3) Inside table range → normal linear interpolation
    for (int i = 0; i < LC_TABLE_COUNT - 1; i++) {

        float x1 = lc_table[i].sum;
        float x2 = lc_table[i + 1].sum;

        if (sum >= x1 && sum <= x2) {
            float y1 = lc_table[i].mass;
            float y2 = lc_table[i + 1].mass;

            float t = (sum - x1) / (x2 - x1);
            float y = y1 + t * (y2 - y1);

            // Optional: clamp inside table range if needed
            if (y > HX711_MAX_MASS) {
                y = HX711_MAX_MASS;
            }
            return y;
        }
    }

    // Should never reach here
    return -1.0f;
}



/* ================================
 *  Base HX711 Driver Functions
 * ================================*/

// Init
void HX711_Init(HX711_t *hx711, GPIO_TypeDef* dt_port, uint16_t dt_pin,
                GPIO_TypeDef* sck_port, uint16_t sck_pin, float scale)
{
    hx711->data_gpio_port = dt_port;
    hx711->data_pin = dt_pin;
    hx711->sck_gpio_port = sck_port;
    hx711->sck_pin = sck_pin;
    hx711->offset = 0;
    hx711->scale = scale;
}

// Read raw 24-bit data
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

    HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(hx711->sck_gpio_port, hx711->sck_pin, GPIO_PIN_RESET);

    if (data & 0x800000) {
        data |= 0xFF000000;
    }

    return (int32_t)data;
}

// Tare
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
    return 0;
}

// Get calibrated value (raw → weight)
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
 *  (with interpolation)
 * ================================*/

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

float HX711_CombinedMassFromSum(float sum)
{
    if (sum <= 0.0f) return 0.0f;
    return HX711_InterpFromTable(sum);
}
