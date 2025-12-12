// Arduino Dummy Simulator for STM32 UCO Machine
// STM32 대신 PC/앱 테스트용으로 더미 데이터만 UART로 응답하는 코드
// 프로토콜 포맷: [HEADER]메시지#\r\n  (STM32의 LOG 함수와 동일)

String rxBuffer;

// === 공통 로그 함수 (STM32 의 LOG(header, msg) 에 해당) ===
void sendLog(const char* header, const String& msg) {
  Serial.print(header);
  Serial.print(msg);
  Serial.print("#\r\n");
}

// === 더미 센서/상태 값 생성 함수들 ===
float dummyUltra()      { return 123.4f; }   // 초음파/ToF
float dummyVoltage()    { return 12.3f; }    // 배터리 전압(V)
float dummyLC_Open1()   { return 12.34f; }
float dummyLC_Open2()   { return 12.45f; }
float dummyLC_Close1()  { return 123.4f; }
float dummyLC_Close2()  { return 123.4f; }
float dummyWS_Height()  { return 56.78f; }   // mm
float dummyWS_Mass()    { return 78.91f; }   // g
float dummyLC_Mass()    { return 200.0f; }   // g (LC1+LC2 보간 결과)

// VALIDATION 더미용: 단순 규칙
// - "nouser" 포함 → REJECT
// - "driver" 포함 → DRIVER:TRUE
// - 그 외 → OK
void handleValidation(const String& cmd) {
  if (cmd.indexOf("nouser") != -1) {
    sendLog("[ANS]", "REJECT");
  } else if (cmd.indexOf("driver") != -1) {
    sendLog("[ANS]", "DRIVER:TRUE");
  } else {
    sendLog("[ANS]", "OK");
  }
}

// === 명령 처리 ===
void handleCommand(const String& cmd) {
  // 1) 명령 수신하면 즉시 ACK
  sendLog("[ANS]", "ACK");

  // 2) 명령 종류에 따라 딜레이 결정
  bool isCloseInput = (cmd.indexOf("[CMD]CLOSES") != -1);
  if (isCloseInput) {
    // CLOSES 는 30초 딜레이
    delay(30000);
  } else {
    // 나머지 모든 명령은 2초 딜레이
    delay(2000);
  }

  // -------------------------------------------------------------
  // HANDSHAKE  → [CMD]HANDSHAKE
  // -------------------------------------------------------------
  if (cmd.indexOf("[CMD]HANDSHAKE") != -1) {
    sendLog("[ANS]", "OK");
    return;
  }

  // -------------------------------------------------------------
  // VALIDATION  → [VALID]...ENDSTR
  // -------------------------------------------------------------
  if (cmd.indexOf("[VALID]") != -1 && cmd.indexOf("ENDSTR") != -1) {
    handleValidation(cmd);
    return;
  }

  // -------------------------------------------------------------
  // POST_UCO  → [CMD]POSTDATA
  // -------------------------------------------------------------
  if (cmd.indexOf("[CMD]POSTDATA") != -1) {
    sendLog("[ANS]", "OK");
    return;
  }

  // -------------------------------------------------------------
  // POST_PER → [CMD]POSTPER
  // -------------------------------------------------------------
  if (cmd.indexOf("[CMD]POSTPER") != -1) {
    sendLog("[ANS]", "OK");
    return;
  }

  // -------------------------------------------------------------
  // OPEN_INPUT → [CMD]OPENS  (문 열기, 초기 로드셀 값 측정)
  // -------------------------------------------------------------
  if (cmd.indexOf("[CMD]OPENS") != -1) {
    float lc1_open = dummyLC_Open1();
    float lc2_open = dummyLC_Open2();

    sendLog("[ANS]", "OK");

    char buf[80];
    snprintf(buf, sizeof(buf),
             "loadcell_1_open = %.2f, loadcell_2_open = %.2f",
             lc1_open, lc2_open);
    sendLog("[DEBUG]", String(buf));
    return;
  }

  // -------------------------------------------------------------
  // CLOSE_INPUT → [CMD]CLOSES
  //  (문 닫고 측정 후 oil/water 계산, 딜레이는 위에서 30초 적용됨)
  // -------------------------------------------------------------
  if (isCloseInput) {
    float distance_Tof = dummyUltra();
    float lc1_open  = dummyLC_Open1();
    float lc2_open  = dummyLC_Open2();
    float lc1_close = dummyLC_Close1();
    float lc2_close = dummyLC_Close2();

    float lc1_door = lc1_close - lc1_open;
    float lc2_door = lc2_close - lc2_open;
    float ws_mass  = dummyWS_Mass();
    float lc_mass  = dummyLC_Mass();
    float oil_mass = lc_mass - ws_mass;
    if (oil_mass < 0.0f) oil_mass = 0.0f;

    char buf[128];

    // (1) 최종 oil/water 결과
    snprintf(buf, sizeof(buf), "O%.2fW%.2fE", oil_mass, ws_mass);
    sendLog("[ANS]", String(buf));

    // (2) 원시 raw 값 로그
    snprintf(buf, sizeof(buf),
             "Distance=%.2f mm, LC1_raw=%.2f, LC2_raw=%.2f;",
             distance_Tof, lc1_door, lc2_door);
    sendLog("[ANS]", String(buf));

    snprintf(buf, sizeof(buf),
             "lc1_open=%.2f g, lc2_open=%.2f g, lc1_close=%.2f g, lc2_close=%.2f g;",
             lc1_open, lc2_open, lc1_close, lc2_close);
    sendLog("[ANS]", String(buf));

    snprintf(buf, sizeof(buf),
             "LC_mass=%.1f g, WS_mass=%.1f g, Oil_mass=%.1f g;",
             lc_mass, ws_mass, oil_mass);
    sendLog("[ANS]", String(buf));

    return;
  }

  // -------------------------------------------------------------
  // UNLOCK_DOOR → [CMD]OPENB
  // -------------------------------------------------------------
  if (cmd.indexOf("[CMD]OPENB") != -1) {
    sendLog("[ANS]", "OK");
    return;
  }

  // -------------------------------------------------------------
  // SLEEP → [CMD]SLEEP
  // -------------------------------------------------------------
  if (cmd.indexOf("[CMD]SLEEP") != -1) {
    sendLog("[ANS]", "STM_SLEEP");
    delay(100);
    sendLog("[ANS]", "STM_WAKE_UP");
    return;
  }

  // -------------------------------------------------------------
  // ALARM_WAKEUP → [CMD]ALARM_WAKEUP
  // -------------------------------------------------------------
  if (cmd.indexOf("[CMD]ALARM_WAKEUP") != -1) {
    sendLog("[CMD]", "UPDATE");
    sendLog("[ANS]", "DONE");
    return;
  }

  // -------------------------------------------------------------
  // EXTI_WAKEUP → [CMD]EXTI_WAKEUP
  // -------------------------------------------------------------
  if (cmd.indexOf("[CMD]EXTI_WAKEUP") != -1) {
    sendLog("[ANS]", "EXTI_WAKEUP");
    return;
  }

  // -------------------------------------------------------------
  // OPEN_VALVE / CLOSE_VALVE
  // -------------------------------------------------------------
  if (cmd.indexOf("[CMD]OPENV") != -1) {
    sendLog("[ANS]", "OPEN_VALVE");
    return;
  }

  if (cmd.indexOf("[CMD]CLOSEV") != -1) {
    sendLog("[ANS]", "CLOSE_VALVE");
    return;
  }

  // -------------------------------------------------------------
  // TURN_OFF → [CMD]OFF
  // -------------------------------------------------------------
  if (cmd.indexOf("[CMD]OFF") != -1) {
    sendLog("[ANS]", "TURN_OFF_MOTOR_DOOR");
    return;
  }

  // -------------------------------------------------------------
  // TEST_PERCENT → [TEST]PERCENT
  // -------------------------------------------------------------
  if (cmd.indexOf("[TEST]PERCENT") != -1) {
    float ultra = dummyUltra();
    float value_VolContainer = (525.0f - ultra) / 525.0f * 80.0f;
    if (value_VolContainer < 0.0f) value_VolContainer = 0.0f;
    float battery = dummyVoltage();

    char buf[128];
    snprintf(buf, sizeof(buf),
             "{\"volume\": %.2f,\"battery\": %.2f}",
             value_VolContainer, battery);
    sendLog("TEST", String(buf));
    return;
  }

  // -------------------------------------------------------------
  // TEST_MEASURE → [TEST]MEASURE
  // -------------------------------------------------------------
  if (cmd.indexOf("[TEST]MEASURE") != -1) {
    float ultra = dummyUltra();
    float distance_Tof = dummyUltra();
    float lc1_open = dummyLC_Open1();
    float lc2_open = dummyLC_Open2();
    float lc1_test = lc1_open + 5.0f;
    float lc2_test = lc2_open + 7.0f;

    float lc1_diff = lc1_test - lc1_open;
    float lc2_diff = lc2_test - lc2_open;

    float lc_mass   = dummyLC_Mass();
    float ws_height = dummyWS_Height();
    float ws_mass   = dummyWS_Mass();
    float oil_mass  = lc_mass - ws_mass;
    if (oil_mass < 0.0f) oil_mass = 0.0f;

    char buf[128];

    snprintf(buf, sizeof(buf), "Ultra=%.2f cm", ultra);
    sendLog("[ANS]", String(buf));

    snprintf(buf, sizeof(buf),
             "Distance=%.2f mm, LC1_raw=%.2f, LC2_raw=%.2f;",
             distance_Tof, lc1_diff, lc2_diff);
    sendLog("[ANS]", String(buf));

    snprintf(buf, sizeof(buf),
             "lc1_test=%.2f g, lc2_test=%.2f g, lc1_open=%.2f g, lc2_open=%.2f g;",
             lc1_test, lc2_test, lc1_open, lc2_open);
    sendLog("[ANS]", String(buf));

    snprintf(buf, sizeof(buf),
             "LC_mass=%.1f g, WS_mass=%.1f g, Oil_mass=%.1f g;",
             lc_mass, ws_mass, oil_mass);
    sendLog("[ANS]", String(buf));

    snprintf(buf, sizeof(buf), "WS_height=%.1f mm", ws_height);
    sendLog("[DEBUG]", String(buf));

    return;
  }

  // -------------------------------------------------------------
  // TEST_SDCLOSE / TEST_SDOPEN
  // -------------------------------------------------------------
  if (cmd.indexOf("[TEST]SDCLOSE") != -1) {
    sendLog("[ANS]", "OK");
    return;
  }

  if (cmd.indexOf("[TEST]SDOPEN") != -1) {
    sendLog("[ANS]", "OK");
    return;
  }

  // -------------------------------------------------------------
  // SETTING → [CMD]SETTING
  // -------------------------------------------------------------
  if (cmd.indexOf("[CMD]SETTING") != -1) {
    float ultra = dummyUltra();
    float value_VolContainer = (525.0f - ultra) / 525.0f * 80.0f;
    if (value_VolContainer < 0.0f) value_VolContainer = 0.0f;

    char buf[64];
    snprintf(buf, sizeof(buf),
             "Container=%.2f L", value_VolContainer);
    sendLog("[ANS]", String(buf));
    return;
  }

  // -------------------------------------------------------------
  // UNKNOWN 명령
  // -------------------------------------------------------------
  sendLog("[DEBUG]", "UNKNOWN_CMD");
}

// === setup / loop ===

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }   // Native USB 보드용, 필요 없으면 삭제 가능
  delay(800);
  sendLog("[DEBUG]", "Arduino Dummy Started");
}

void loop() {
  // PC/라즈베리에서 들어오는 한 줄 명령 수신
  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    // '#' 또는 개행 문자 기준으로 한 명령 끝
    if (c == '#' || c == '\n' || c == '\r') {
      if (rxBuffer.length() > 0) {
        handleCommand(rxBuffer);
        rxBuffer = "";
      }
    } else {
      rxBuffer += c;
    }
  }
}
