#include "HX711.h"

static uint32_t initTime;
static uint32_t data;

/* ================================
 *  Calibration Table (LC1+LC2 → mass)
 * ================================*/

typedef struct {
    float sum;    // LC1 + LC2 raw sum
    float mass;   // actual mass (g)
} LC_CalibPoint;

static const LC_CalibPoint lc_table[] = {
    {   0.00f,    0.0f },
    {  11.33f,   50.0f },
    {  29.88f,  100.0f },
    {  61.49f,  150.0f },
    { 106.62f,  200.0f },
    { 146.64f,  250.0f },
    { 231.74f,  300.0f },
    { 266.39f,  350.0f },
    { 305.08f,  400.0f },
    { 346.44f,  450.0f },
    { 389.84f,  500.0f },
    { 432.49f,  550.0f },
    { 473.04f,  600.0f },
    { 518.01f,  650.0f },
    { 613.28f,  700.0f },
    { 647.39f,  750.0f },
    { 685.11f,  800.0f },
    { 751.84f,  850.0f },
    { 824.85f,  900.0f },
    { 862.03f,  950.0f },
    { 928.00f, 1000.0f },
    { 968.03f, 1050.0f },
    {1024.91f, 1100.0f },
    {1090.72f, 1150.0f },
    {1143.14f, 1200.0f },
    {1193.31f, 1250.0f },
    {1248.53f, 1300.0f },
    {1301.28f, 1350.0f },
    {1362.02f, 1400.0f },
    {1415.05f, 1450.0f },
    {1461.47f, 1500.0f },
    {1511.18f, 1550.0f },
    {1616.11f, 1600.0f },
    {1652.98f, 1650.0f },
    {1682.91f, 1700.0f },
    {1706.55f, 1750.0f },
    {1754.19f, 1800.0f },
    {1786.97f, 1850.0f },
    {1836.79f, 1900.0f },
    {1911.04f, 1950.0f },
    {1946.91f, 2000.0f },
};

static const int LC_TABLE_COUNT = sizeof(lc_table) / sizeof(lc_table[0]);


/* ================================
 *  Table Interpolation Function
 * ================================*/

static float HX711_InterpFromTable(float sum)
{
    if (sum <= lc_table[0].sum)
        return lc_table[0].mass;

    if (sum >= lc_table[LC_TABLE_COUNT - 1].sum)
        return lc_table[LC_TABLE_COUNT - 1].mass;

    for (int i = 0; i < LC_TABLE_COUNT - 1; i++) {

        float x1 = lc_table[i].sum;
        float x2 = lc_table[i + 1].sum;

        if (sum >= x1 && sum <= x2) {

            float y1 = lc_table[i].mass;
            float y2 = lc_table[i + 1].mass;

            float t = (sum - x1) / (x2 - x1);
            return y1 + t * (y2 - y1);
        }
    }

    return -1.0f; // Should never reach here
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
