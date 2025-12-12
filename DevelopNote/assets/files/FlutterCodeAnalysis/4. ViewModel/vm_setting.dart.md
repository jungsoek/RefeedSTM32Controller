# vm_setting.dart

## 소스 코드

### 전체 코드

```dart
import 'package:buyoil/model/ui_state_setting.dart';
import 'package:riverpod_annotation/riverpod_annotation.dart';

part 'vm_setting.g.dart';

@riverpod
class Setting extends _$Setting {
  @override
  UIStateSetting build() {
    return UIStateSetting.init();
  }

  void toggleMotor() {
    state = state.copyWith(isMotorOn: !state.isMotorOn);
  }

  void toggleValve() {
    state = state.copyWith(isValveOn: !state.isValveOn);
  }
}
```

## 1️⃣ 파일 개요

**클래스:** `Setting extends _$Setting`
 **역할:**

- 앱 설정 상태 관리 (모터/밸브 ON/OFF)
- Riverpod 상태 관리 (`@riverpod`)
- UIStateSetting을 통해 상태 전달

------

## 2️⃣ 상태 정의

**모델:** `UIStateSetting`

- `isMotorOn` : 모터 상태 (bool)
- `isValveOn` : 밸브 상태 (bool)
- `UIStateSetting.init()` : 초기 상태, 모터/밸브 OFF로 초기화

------

## 3️⃣ 주요 기능

| 함수            | 설명                               |
| --------------- | ---------------------------------- |
| `toggleMotor()` | 현재 모터 상태 반전 (`ON <-> OFF`) |
| `toggleValve()` | 현재 밸브 상태 반전 (`ON <-> OFF`) |

- 상태를 변경하면 Riverpod Provider를 통해 UI가 자동 업데이트 됨

------

## 4️⃣ 특징

1. 단순한 ON/OFF 상태 관리
2. Riverpod `AutoDisposeNotifier` 상속 → Provider에서 자동 상태 관리 가능
3. UI와 바인딩하면 토글 버튼 클릭 시 즉시 반영

------

## 💡 **확장 아이디어**

- 모터/밸브 동작 시 실제 하드웨어 명령 전송과 연동 가능 (`SerialPortVM.writeToPort(...)`)
- 추가 설정 항목(예: 모터 속도, 밸브 개폐 시간 등) 추가 가능
- 토글 시 로그 기록, 토스트 메시지 연동 가능