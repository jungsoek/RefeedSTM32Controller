# vm_step1.dart

## 소스 코드

### 전체 코드

```dart
import 'package:buyoil/viewmodel/vm_serial_port.dart';
import 'package:flutter/cupertino.dart';
import 'package:riverpod_annotation/riverpod_annotation.dart';

import '../model/ui_state_step1.dart';

part 'vm_step1.g.dart';

@riverpod
class Step1 extends _$Step1 {
  @override
  UIStateStep1 build() {
    TextEditingController controller = TextEditingController();
    controller.addListener(() {
      state = state.copyWith(phoneNumber: controller.text);
    });
    return UIStateStep1.input(phoneNumber: "", controller: controller);
  }

  void pressedNumber(String buttonText) {
    if(buttonText == '<') {
      if (state.phoneNumber.isNotEmpty) {
        String updatedPhoneNumber = state.phoneNumber.substring(0, state.phoneNumber.length - 1);
        state = state.copyWith(phoneNumber: updatedPhoneNumber);
      }
      return;
    }

    if(buttonText == 'V') {
      ref.watch(serialPortVMProvider.notifier).sendPhoneNumber(state.phoneNumber);
      // // 성공 시 다음 화면 이동
      // state = UIStateStep1.completed(
      //   controller: state.controller,
      //   phoneNumber: state.phoneNumber,
      // );
      return;
    }

    if(state.phoneNumber.length < 11) {
      state = state.copyWith(phoneNumber: state.phoneNumber + buttonText);
      return;
    }

    print("pressedNumber: ${state.phoneNumber}");
  }

  void showErrorToast({int seconds = 2}) {
    print("showErrorToast()");
    state = state.copyWith(showErrorToast: true);
    Future.delayed(Duration(milliseconds: seconds), () {
      if(state.showErrorToast == true) {
        state = state.copyWith(showErrorToast: false);
      }
    });
  }
}
```

## 1️⃣ 파일 개요

**클래스:** `Step1 extends _$Step1`
 **역할:**

- Step1 화면에서 **전화번호 입력 상태 관리**
- 사용자 입력(숫자 버튼/삭제/확인)을 처리
- Riverpod 상태 관리 (`@riverpod`)
- 입력 완료 시 **USB 포트 전송**과 연동 (`SerialPortVM.sendPhoneNumber`)

------

## 2️⃣ 상태 정의

**모델:** `UIStateStep1`

- `phoneNumber` : 현재 입력된 전화번호 문자열
- `controller` : 입력 TextEditingController
- `showErrorToast` : 입력 오류/부적합 표시 여부

------

## 3️⃣ 주요 기능

| 함수                                | 설명                                                      |
| ----------------------------------- | --------------------------------------------------------- |
| `build()`                           | 초기 상태 생성 및 TextEditingController 리스너 등록       |
| `pressedNumber(String buttonText)`  | 입력 버튼 처리:                                           |
|                                     | - `<` : 백스페이스 (마지막 문자 삭제)                     |
|                                     | - `V` : 입력 완료 → `SerialPortVM.sendPhoneNumber()` 호출 |
|                                     | - 나머지 숫자 : 최대 11자리까지 입력 가능                 |
| `showErrorToast({int seconds = 2})` | 오류 토스트 표시 → 지정 시간 후 자동 숨김                 |

------

## 4️⃣ 특징

1. 입력 시 즉시 `state.phoneNumber` 업데이트 → UI 자동 반영
2. `<` 버튼으로 백스페이스 지원
3. `V` 버튼 클릭 시 SerialPortVM에 전화번호 전송 → USB 통신과 연결
4. 토스트/에러 메시지 상태도 Riverpod 상태로 관리 가능

------

## 5️⃣ 개선/확장 아이디어

- 숫자 입력 UI와 완전히 바인딩하여 키패드 버튼 클릭 → 상태 변화
- 입력 길이 제한, 유효성 검사 강화 (예: 010 시작 여부)
- USB 응답에 따라 화면 이동 처리 (Step2) → 이미 `SerialPortVM.listenByPort`와 연동됨
- `showErrorToast`를 Fluttertoast/Scaffold 메시지와 연동 가능