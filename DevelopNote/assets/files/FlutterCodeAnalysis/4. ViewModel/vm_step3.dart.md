# vm_step3.dart

## 소스 코드

### 전체 코드

```dart
import 'package:buyoil/model/ui_state_step3.dart';
import 'package:buyoil/viewmodel/vm_serial_port.dart';
import 'package:riverpod_annotation/riverpod_annotation.dart';

part 'vm_step3.g.dart';

@riverpod
class Step3 extends _$Step3 {
  @override
  UIStateStep3 build() {
    return UIStateStep3.init();
  }

  void pressedClose() {
    ref.watch(serialPortVMProvider.notifier).close();
    // // todo ok 처리
    // state = UIStateStep3.closeDoor();
    //
    // Future.delayed(Duration(seconds: 2), () {
    //   state = UIStateStep3.completed();
    // });
  }
}
```

## 1️⃣ 파일 개요

**클래스:** `Step3 extends _$Step3`
 **역할:**

- Step3 화면에서 **문 닫기(Close) 동작** 상태 관리
- Riverpod 상태 관리 (`@riverpod`)
- USB 통신과 직접 연결 (`SerialPortVM.close()` 호출)

------

## 2️⃣ 상태 정의

**모델:** `UIStateStep3`

- Step3 화면에서 필요한 상태 관리
- 초기 상태: `UIStateStep3.init()`
- 현재 코드는 **상태 변경 로직은 주석 처리되어 있음** → 실제 상태 변화는 USB 응답 처리 후 구현 예정

------

## 3️⃣ 주요 기능

| 함수             | 설명                                                         |
| ---------------- | ------------------------------------------------------------ |
| `build()`        | 초기 상태 생성 (`UIStateStep3.init()`)                       |
| `pressedClose()` | Close 버튼 클릭 시 호출:                                     |
|                  | - `SerialPortVM.close()` 호출 → STM32 / USB 포트에 `[CMD]CLOSE#` 전송 |
|                  | - 현재 OK 응답 처리 및 상태 변경은 주석 처리 (`closeDoor`, `completed`) |

------

## 4️⃣ 특징

- Step3는 **UI 동작 → USB 명령 전송 → 상태/화면 전환** 흐름에 집중
- 실제 상태 변화 및 화면 이동은 **SerialPortVM의 응답 처리** 또는 추후 구현될 `UIStateStep3` 상태 전환 로직에서 담당

------

## 5️⃣ 개선/확장 아이디어

- Close 명령 전송 후 **OK 응답 확인 → Step 완료 상태로 전환**
- UI에서 **로딩/버튼 비활성화** 상태 관리 가능
- 실패 시 재시도 로직 또는 **알림/팝업** 추가 가능