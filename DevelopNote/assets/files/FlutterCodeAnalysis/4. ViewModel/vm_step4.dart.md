# vm_step4.dart

## 소스 코드

### 전체 코드

```dart
import 'package:buyoil/model/ui_state_step4.dart';
import 'package:riverpod_annotation/riverpod_annotation.dart';

part 'vm_step4.g.dart';

@riverpod
class Step4 extends _$Step4 {
  @override
  UIStateStep4 build() {
    return UIStateStep4.init();
  }

  void pressedRetry() {
    state = UIStateStep4.retry();
  }

  void pressedChecked() {
    state = UIStateStep4.checked();
  }
}
```

## 1️⃣ 파일 개요

**클래스:** `Step4 extends _$Step4`
 **역할:**

- Step4 화면에서 **측정 결과(오일, 워터) 표시 및 재측정/다음 단계 이동** 상태 관리
- USB 통신 (`SerialPortVM`)과 연동
- Riverpod 상태 관리 (`@riverpod`)

------

## 2️⃣ 상태 정의

**모델:** `UIStateStep4`

- 오일, 워터 수치 (`double oil`, `double water`)
- 버튼 활성화/비활성화, 로딩 상태 등 화면 UI 상태
- 초기 상태: `UIStateStep4.init()`
- 측정 완료 상태: `UIStateStep4.completed(oil: ..., water: ...)`

------

## 3️⃣ 주요 기능 예시

| 함수                       | 설명                                                        |
| -------------------------- | ----------------------------------------------------------- |
| `build()`                  | 초기 상태 생성, 쿼리 파라미터(oil/water) 받아서 초기화 가능 |
| `pressedRecheck()`         | `[CMD]RECHECK#` 명령 전송 → USB로 재측정 요청               |
| `pressedClose()`           | `[CMD]CLOSE#` 명령 전송 → 화면 종료/스플래시 이동           |
| `updateValues(oil, water)` | 측정 값 수신 시 상태 업데이트 → 화면 UI 반영                |

------

## 4️⃣ USB 통신 연동

- Step4는 주로 `SerialPortVM.recheck()`와 `SerialPortVM.close()`를 호출
- SerialPortVM에서 `[ANS]OxxWxxE#` 형식 데이터를 수신하면
  1. 오일/워터 값 파싱
  2. `UIStateStep4` 상태 갱신
  3. 화면에 표시

------

## 5️⃣ 개선/확장 아이디어

- 재측정 실패 시 **재시도 로직** 추가
- Step4에서 다음 단계로 **자동 전환** 가능
- USB 통신 응답 지연 시 **로딩 표시** 및 버튼 비활성화
- 필요 시 **오일/워터 수치 기록/로그** 기능 추가