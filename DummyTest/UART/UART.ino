void setup() {
  // PC와 USB 통신 시작 (시리얼 모니터)
  Serial.begin(115200); 
  // 핀 D0/D1을 통한 STM32 통신 시작 (보드 속도는 STM32와 일치해야 함)
  Serial1.begin(115200); 
}

void loop() {
  // PC로부터 데이터 수신 -> STM32로 전송
  if (Serial.available()) {
    char data = Serial.read();
    Serial1.write(data);
  }

  // STM32로부터 데이터 수신 -> PC로 전송 (로그 버퍼 구현)
  if (Serial1.available()) {
    char stm_data = Serial1.read();
    Serial.write(stm_data);

    if (stm_data == '#') {
      Serial.println(); // Serial.println()은 "\r\n"을 자동으로 추가함
    }
  }


}