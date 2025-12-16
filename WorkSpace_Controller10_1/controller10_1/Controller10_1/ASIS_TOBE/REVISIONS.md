# REVISIONS

## WATERSENSOR.c

### WATER_ReadRawFull()

```c
WATER_RawDataFull_t WATER_ReadRawFull(WATER_HandleTypeDef *hws)
{
    WATER_RawDataFull_t ret;
    memset(&ret, 0, sizeof(ret));

    // 핸들 유효성 확인
    if (!hws) {
        LOG("[ERR]", "hws is NULL");
        ret.valid = 0;
        ret.st1 = HAL_ERROR;
        ret.st2 = HAL_ERROR;
        return ret;
    }

    if (!hws->i2c) {
        LOG("[ERR]", "I2C handle is NULL");
        ret.valid = 0;
        ret.st1 = HAL_ERROR;
        ret.st2 = HAL_ERROR;
        return ret;
    }

    uint8_t low_data[8]   = {0};
    uint8_t high_data[12] = {0};

    // I2C 통신
    ret.st1 = i2c_recv(hws->i2c, hws->addr_low,  low_data,  8,  hws->timeout_ms);
    ret.st2 = i2c_recv(hws->i2c, hws->addr_high, high_data, 12, hws->timeout_ms);

    LOG("[DBG]", "I2C Status - Low:%d, High:%d", ret.st1, ret.st2);

    if (ret.st1 != HAL_OK || ret.st2 != HAL_OK) {
        ret.valid = 0;
        LOG("[ERR]", "I2C read failed, raw data invalid");
        return ret;
    }

    // 정상일 경우 raw_phys20 채우기
    memcpy(&ret.data[0], low_data, 8);
    for (uint8_t j = 0; j < 12; j++) {
        ret.data[ HIGH_MAP[j] ] = high_data[j];
    }

    ret.valid = 1;

    // raw 데이터 확인용 로그
    char buf[128];
    int len = 0;
    len += snprintf(buf + len, sizeof(buf) - len, "RAW_DATA: ");
    for (int i = 0; i < 20; i++) {
        len += snprintf(buf + len, sizeof(buf) - len, "%d", ret.data[i]);
        if (i < 19) len += snprintf(buf + len, sizeof(buf) - len, ",");
    }
    LOG("[DBG]", buf);

    return ret;
}
```

### **WATER_ReadLastOn_Threshold()**

#### 변경 사항

1. 물, 기름 층에 맞게 연산식 수정

   예시)

   ↓↓↓↓↓↓↓↓↓

   ```
    순수 기름(40 ~ 80)
    --------
    불순 기름(80 ~ 150)
    --------
    불순 물(150 ~ 220)
    --------
    순수 물(220 ~)
   ```

```c
float WATER_ReadLastOn_Threshold(WATER_HandleTypeDef *hws, uint8_t threshold)
{
    if (!hws) return -1.0f;

    enum { N_SAMPLES = 10 };
    uint8_t raw[20];
    int counts[21] = {0};   // last_on: 0~20
    int valid_samples = 0;

    /* ============================
     * 1. last_on 최빈값 계산
     * ============================ */
    for (int n = 0; n < N_SAMPLES; n++) {
        if (WATER_ReadRaw(hws, raw) != HAL_OK) {
            continue;
        }

        int last_on = compute_last_on_with_threshold(raw, threshold);
        if (last_on < 0)  last_on = 0;
        if (last_on > 20) last_on = 20;

        counts[last_on]++;
        valid_samples++;
    }

    if (valid_samples == 0) {
        return -1.0f;   // I2C 전부 실패
    }

    int best_last_on = 0;
    int best_count = counts[0];
    for (int i = 1; i <= 20; i++) {
        if (counts[i] > best_count) {
            best_count = counts[i];
            best_last_on = i;
        }
    }

    if (best_last_on <= 0) {
        return 0.0f;    // 완전 비어 있음
    }

    /* ============================
     * 2. 층 기반 질량 계산
     * ============================ */
    float total_mass = 0.0f;

    for (int seg = 1; seg <= best_last_on; seg++) {

        float seg_mass = WS_IndexToMass((uint8_t)seg);
        if (seg_mass <= 0.0f)
            continue;

        /* ----------------------------
         * 층 구간 분리 (비율 기반)
         * ---------------------------- */
        float ratio = (float)seg / (float)best_last_on;

        if (ratio <= 0.25f) {
            /* 하단 25% : 순수 물 (100% 물) */
            total_mass += seg_mass;
        }
        else if (ratio <= 0.50f) {
            /* 25~50% : 불순 물 (70% 물) */
            total_mass += seg_mass * 0.7f;
        }
        else if (ratio <= 0.75f) {
            /* 50~75% : 불순 기름 (30% 물) */
            total_mass += seg_mass * 0.3f;
        }
        else {
            /* 상단 25% : 순수 기름 (물 기여 0%) */
            /* do nothing */
        }
    }

    if (total_mass <= 0.0f) {
        return 0.0f;
    }

    /* ============================
     * 3. 오차 모델 적용 (기존 그대로)
     * ============================ */
    float percent;
    if (total_mass <= 100.0f) {
        percent = 0.10f;      // ±10%
    } else if (total_mass <= 1000.0f) {
        percent = 0.05f;      // ±5%
    } else {
        percent = 0.025f;     // ±2.5%
    }

    float r = ((float)(rand() % 2001) - 1000.0f) / 1000.0f; // -1.0 ~ +1.0
    float randomized = total_mass + (total_mass * percent * r);

    if (randomized < 0.0f)
        randomized = 0.0f;

    return randomized;
}
```

## main.c

### TEST_WSRAW case 추가

```c
case TEST_WS_RAW:

    // 1) WATER 센서 초기화
    WATER_HandleTypeDef hws;
    WATER_Init(&hws, &hi2c1, WATER_ADDR_LOW_DEF,
    WATER_ADDR_HIGH_DEF, 0, WATER_TIMEOUT_DEF);

    uint8_t rx_char = 0;

    LOG("[INFO]",
            "Start WATER RAW logging. Press 'q' to stop.\r\n");

    while (FUNCTION == TEST_WS_RAW) {
        // 1) WATER RAW 읽기
        WATER_RawDataFull_t raw = WATER_ReadRawFull(&hws);

        if (raw.valid) {
            char line[256] = { 0 };
            char tmp[16];

            strcat(line, "WS_RAW: ");
            for (int i = 0; i < 20; i++) {
                snprintf(tmp, sizeof(tmp), "[%02d]=%03d", i,
                        raw.data[i]);
                strcat(line, tmp);

                if (i < 19)
                    strcat(line, ", ");
            }
            LOG("[INFO]", line);
            LOG("\r\n", "");
        } else {
            char buf[64];
            snprintf(buf, sizeof(buf),
                    "WATER_ReadRaw FAIL: LOW=%d, HIGH=%d", raw.st1,
                    raw.st2);
            LOG("[ERR]", buf);
            LOG("\r\n", "");
        }

        // 2) UART 입력 확인 (폴링)
        if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE)) {
            HAL_UART_Receive(&huart1, &rx_char, 1, 10);
            if (rx_char == 'q' || rx_char == 'Q') {
                LOG("[INFO]",
                        "Stop command received. Exiting loop.\r\n");
                FUNCTION = SPACE;
                break; // 루프 종료
            }
        }

        HAL_Delay(200);
    }

    LOG("[INFO]", "WATER logging stopped.\r\n");

    break;
```

### TEST_MEASURE

수위센서 raw data 출력 기능 추가

```c
case TEST_MEASURE:
    /* ===========================
     * 1) WATER 센서 초기화
     * =========================== */
    WATER_HandleTypeDef hws;
    WATER_Init(&hws, &hi2c1,
    WATER_ADDR_LOW_DEF,
    WATER_ADDR_HIGH_DEF, 0,
    WATER_TIMEOUT_DEF);

    /* ===========================
     * 2) I2C Bus Scan (DEBUG)
     * =========================== */
    for (uint8_t addr7 = 1; addr7 < 128; addr7++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1, (addr7 << 1), 1, 5)
                == HAL_OK) {
            snprintf(data_TransmitHeader,
                    sizeof(data_TransmitHeader),
                    "I2C FOUND: 0x%02X", addr7);
            LOG("[DEBUG]", data_TransmitHeader);
        }
    }

    /* ===========================
     * 3) WATER 센서 ON
     * =========================== */
    HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_RESET);
    HAL_Delay(1500);   // 안정화 대기

    /* ===========================
     * 4) 거리 센서
     * =========================== */
    ultra = Ultra_ReadDistance();
    snprintf(data_TransmitHeader, sizeof(data_TransmitHeader),
            "Ultra=%.2f cm", ultra);
    LOG("[ANS]", data_TransmitHeader);

    /* ===========================
     * 5) Loadcell 측정
     * =========================== */
    loadcell_1_testmeasure = HX711_Get_Value(&scale1, 30, 2000);
    loadcell_2_testmeasure = HX711_Get_Value(&scale2, 30, 2000);

    loadcell_1 = loadcell_1_testmeasure - loadcell_1_open;
    loadcell_2 = loadcell_2_testmeasure - loadcell_2_open;

    if (fabsf(loadcell_1) < 5.0f)
        loadcell_1 = 0.0f;
    if (fabsf(loadcell_2) < 5.0f)
        loadcell_2 = 0.0f;

    loadcell_sum = loadcell_1 + loadcell_2;
    lc_mass = HX711_InterpFromTable(loadcell_sum); // g

    /* ===========================
     * 6) WATER 센서 측정 (개선된 로직)
     * =========================== */
    ws_height = WATER_ReadHeightMM_Quick(ws_threshold);   // mm

    water_weight = WATER_ReadLastOn_Threshold(&hws, ws_threshold); // ✅ 핵심 변경

    if (water_weight < 0.0f)
        water_weight = 0.0f;

    /* ===========================
     * 7) Oil mass 계산
     * =========================== */
    oil_weight = lc_mass - water_weight;
    if (oil_weight < 0.0f)
        oil_weight = 0.0f;

    /* ===========================
     * 8) WATER 센서 OFF
     * =========================== */
    HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_SET);

    /* ===========================
     * 9) Logging
     * =========================== */
    snprintf(data_TransmitHeader, sizeof(data_TransmitHeader),
            "LC1=%.2f g, LC2=%.2f g", loadcell_1, loadcell_2);
    LOG("[ANS]", data_TransmitHeader);

    snprintf(data_TransmitHeader, sizeof(data_TransmitHeader),
            "LC_mass=%.1f g, WS_mass=%.1f g, Oil_mass=%.1f g",
            lc_mass, water_weight, oil_weight);
    LOG("[ANS]", data_TransmitHeader);

    snprintf(data_TransmitHeader, sizeof(data_TransmitHeader),
            "WS_height=%.1f mm", ws_height);
    LOG("[DEBUG]", data_TransmitHeader);

    /* ===========================
     * 10) RAW 데이터 (DEBUG)
     * =========================== */
    WATER_RawDataFull_t raw = WATER_ReadRawFull(&hws);
    if (raw.valid) {
        char line[256] = { 0 };
        char tmp[16];

        strcat(line, "WS_RAW: ");
        for (int i = 0; i < 20; i++) {
            snprintf(tmp, sizeof(tmp), "[%02d]=%03d", i,
                    raw.data[i]);
            strcat(line, tmp);
            if (i < 19)
                strcat(line, ", ");
        }
        LOG("[INFO]", line);
    } else {
        char buf[64];
        snprintf(buf, sizeof(buf),
                "WATER_ReadRaw FAIL: LOW=%d, HIGH=%d", raw.st1,
                raw.st2);
        LOG("[ERR]", buf);
    }

    HAL_Delay(200);
    FUNCTION = SPACE;
    break;
```

