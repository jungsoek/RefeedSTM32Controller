# custom_toast.dart

## 소스 코드

### 전체 코드

```dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../app_colors.dart';
import '../../app_styles.dart';
import 'toast_provider.dart';

class CustomToast extends ConsumerWidget {
  const CustomToast({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final toastState = ref.watch(toastProvider);

    if (!toastState.isVisible || toastState.message == null) {
      return const SizedBox.shrink(); // 보이지 않을 때는 빈 위젯
    }

    return Positioned(
      top: 57,
      left: 0,
      right: 0,
      child: IgnorePointer( // 토스트 뒤의 UI 터치 가능하도록
        child: Material(
          color: Colors.transparent,
          child: Center(
            child: Container(
              // alignment: Alignment.center,
              padding: EdgeInsets.symmetric(vertical: 23, horizontal: 70),
              decoration: BoxDecoration(
                  borderRadius: BorderRadius.circular(20),
                  border: BoxBorder.all(color: AppColors.D32F2F, width: 1),
                  color: Color(0xfffde3e3)),
              child: Text( toastState.message ?? "",
                  textAlign: TextAlign.center,
                  style: AppStyles.tsStep1Toast),
            ),
          )

          // child: Container(
          //   width: double.maxFinite,
          //   margin: const EdgeInsets.symmetric(horizontal: 20.0),
          //   padding: const EdgeInsets.symmetric(horizontal: 20.0, vertical: 10.0),
          //   decoration: BoxDecoration(
          //       color: const Color(0xFFFFFFFF),
          //       borderRadius: BorderRadius.circular(6.0),
          //       border: Border.all(color: AppColors.TEXT_20, width: 1)
          //   ),
          //   child: Row(
          //     mainAxisSize: MainAxisSize.min,
          //     mainAxisAlignment: MainAxisAlignment.center,
          //     children: <Widget>[
          //       Flexible(
          //         child: Text(
          //           toastState.message ?? "",
          //           style: Theme.of(context).textTheme.displayLarge,
          //           textAlign: TextAlign.center,
          //         ),
          //       ),
          //     ],
          //   ),
          // ),
        )
      ),
    );
  }
}
```

## 1️⃣ 파일 개요

**파일명:** `lib/common/utils/custom_toast.dart`
 **역할:** Flutter에서 사용하는 **커스텀 토스트(Custom Toast) 위젯** 정의
 **주요 목적:**

- 앱 내 공통 **Toast 메시지 UI** 제공
- Riverpod 상태(`toastProvider`)를 구독하여 **토스트 표시 여부와 메시지** 결정
- 토스트 표시 시 **UI 터치 차단 없이 시각적 알림**만 제공

**사용 맥락 예시:**

- 사용자에게 간단한 알림 메시지 표시
- 시스템 상태, 오류, 성공/실패 알림
- 기존 FlutterToast와 다르게 위젯 트리에서 직접 관리 가능

------

## 2️⃣ 주요 기능

1. **Riverpod 상태 구독**
   - `ref.watch(toastProvider)`로 토스트 상태 감시
   - 상태 변경 시 UI 자동 갱신
2. **표시 여부 조건**
   - `isVisible`이 `false`이거나 `message`가 `null`이면 토스트 숨김
   - `SizedBox.shrink()` 사용으로 화면 차지하지 않음
3. **UI 구성**
   - `Positioned`로 화면 상단 고정(`top: 57`)
   - 좌우 끝까지 펼치기(`left:0, right:0`)
   - `IgnorePointer` 사용하여 토스트 뒤 UI 터치 가능
4. **토스트 스타일**
   - `Material(color: Colors.transparent)`
   - `Center` + `Container` 사용
   - 패딩: `vertical:23, horizontal:70`
   - 테두리: `borderRadius: 20, border: AppColors.D32F2F, width:1`
   - 배경색: `Color(0xfffde3e3)`
   - 텍스트 스타일: `AppStyles.tsStep1Toast`
5. **커스터마이징 가능**
   - 메시지 문자열 `toastState.message` 반영
   - 상태관리 기반으로 표시/숨김 제어 가능

------

## 3️⃣ 구조 분석

```
CustomToast (ConsumerWidget)
 └── build(context, ref)
       ├─ ref.watch(toastProvider) → toastState
       ├─ if (!toastState.isVisible || toastState.message == null)
       │    └─ return SizedBox.shrink()
       └─ return Positioned (top:57, left:0, right:0)
             └── IgnorePointer
                   └── Material (color: transparent)
                         └── Center
                               └── Container
                                     ├─ padding: EdgeInsets.symmetric(vertical:23, horizontal:70)
                                     ├─ decoration: BoxDecoration(borderRadius, border, color)
                                     └─ Text(toastState.message, style: AppStyles.tsStep1Toast, textAlign: center)
```

**특징:**

- UI가 간단하고 시각적 요소만 담당
- 상태 관리(Riverpod) 기반으로 표시 여부 결정
- `IgnorePointer`로 터치 이벤트 무시

------

## 4️⃣ 동작 흐름

1. `CustomToast` 위젯 트리에서 렌더링
2. `ref.watch(toastProvider)`로 상태 확인
3. `toastState.isVisible == false`이면 빈 위젯 반환 → 화면에 표시 안 함
4. `toastState.isVisible == true` && `message != null`이면 토스트 표시
5. `Positioned`로 화면 상단 배치, `IgnorePointer`로 터치 이벤트 무시
6. `Container` + `Text`로 스타일 적용

------

## 5️⃣ 장점

- **상태 기반 제어**: Riverpod과 연동되어 메시지 자동 갱신
- **UI 터치 간섭 없음**: IgnorePointer 적용
- **재사용성 높음**: 어디서나 위젯 트리에 포함 가능
- **스타일 일관성**: BoxDecoration과 AppStyles 적용

------

## 6️⃣ 단점 / 개선점

1. **위치 고정**
   - `top: 57` → 화면 크기나 반응형 환경에서 제한적
   - 개선: `MediaQuery` 기반 동적 위치 계산 가능
2. **길이/크기 고정**
   - Padding이 고정, 길이가 긴 메시지 처리 어려움
   - 개선: `Flexible` 또는 `Wrap` 적용
3. **애니메이션 없음**
   - 나타났다 사라지는 효과 없음
   - 개선: `AnimatedOpacity`, `SlideTransition` 등 적용
4. **다중 토스트**
   - 동시에 여러 메시지 처리 불가
   - 개선: Toast Queue 구조로 여러 메시지 순차 처리
5. **커스터마이징 부족**
   - 색상, 폰트, 위치, duration 등 외부에서 설정 불가
   - 개선: 매개변수로 스타일 옵션 전달

------

## 7️⃣ 개선된 구조 예시 (개념)

```
CustomToast(
  top: MediaQuery.of(context).padding.top + 20,
  padding: EdgeInsets.symmetric(vertical: 20, horizontal: 50),
  backgroundColor: Colors.black.withOpacity(0.8),
  textStyle: TextStyle(color: Colors.white, fontSize: 16),
  duration: Duration(seconds: 2),
)
```

- 위치, 스타일, 지속 시간 등을 외부에서 조절 가능
- 반응형 화면과 다중 메시지 지원 가능

