# w_header.dart

## 소스 코드

### 전체 코드

```dart
import 'package:buyoil/common/app_strings.dart';
import 'package:flutter/material.dart';

import '../../common/app_colors.dart';
import '../../config.dart';
import 'btn_home.dart';

class HeaderWidget extends StatefulWidget {
  const HeaderWidget({super.key});

  @override
  createState() => HeaderWidgetState();
}

class HeaderWidgetState extends State<HeaderWidget> {
  @override
  Widget build(BuildContext context) {
    return Container(
      height: 80,
      width: double.maxFinite,
      color: AppColors.F5F5F5,
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Container(
            width: 106,
            margin: EdgeInsets.only(left: 22),
            child: Image.asset("${AppStrings.assetPath}img_logo.png"),
          ),
          Expanded(child: SizedBox.shrink()),
          ToHomeBtn()
        ],
      ),
    );
  }
}
```

## 1️⃣ 파일 개요

**파일명:** `lib/view/widget/step1/w_header.dart`
 **역할:** 앱 화면 상단에 표시되는 **헤더(Header) 위젯** 제공
 **주요 목적:**

- 앱 로고 표시
- 홈으로 이동할 수 있는 버튼 배치 (`ToHomeBtn`)
- 상단 영역 스타일과 레이아웃 통일

**사용 맥락 예시:**

- 모든 화면 상단에 고정된 헤더
- 브랜드 로고와 홈 이동 버튼 제공

------

## 2️⃣ 주요 기능

1. **로고 표시**
   - `Image.asset("${AppStrings.assetPath}img_logo.png")`
   - 좌측에 고정된 크기(`width: 106`)와 좌측 여백(`margin: left 22`)
2. **홈 버튼**
   - `ToHomeBtn()` 위젯 사용
   - 우측 정렬, 클릭 시 홈 화면으로 이동
3. **레이아웃**
   - `Row` 사용, `mainAxisAlignment: spaceBetween`
   - 좌측 로고, 우측 홈 버튼, 중간 공간 확장(`Expanded(SizedBox.shrink())`)
4. **스타일**
   - 높이 80px, 전체 너비
   - 배경색: `AppColors.F5F5F5` (회색 계열)

------

## 3️⃣ 구조 분석

```
HeaderWidget (StatefulWidget)
 └── HeaderWidgetState (State)
       └─ build(): Container (80px height)
             ├─ Row (spaceBetween)
             │    ├─ 로고: Container(width:106, margin left:22, Image.asset)
             │    ├─ Expanded(child: SizedBox.shrink()) → 중간 빈 공간
             │    └─ ToHomeBtn() → 홈 버튼
```

- **StatefulWidget** 사용: 현재 상태를 관리할 필요는 거의 없음
- **Expanded + SizedBox** → 좌우 간격 조정, 로고와 홈 버튼 분리

------

## 4️⃣ 동작 흐름

1. `HeaderWidget()` 호출
2. 화면 상단에 높이 80px `Container` 렌더링
3. `Row` 레이아웃:
   - 좌측: 로고 이미지
   - 중간: 빈 공간 확장
   - 우측: 홈 버튼(`ToHomeBtn`)
4. 홈 버튼 클릭 시 `ToHomeBtn` 내부 로직에 따라 홈 화면 이동

------

## 5️⃣ 장점

- **간단하고 직관적**: 로고 + 홈 버튼만 있어 가볍고 직관적
- **레이아웃 통일**: 모든 화면 상단 동일 UI 적용 가능
- **확장 용이**: Row에 다른 위젯 추가 가능

------

## 6️⃣ 단점 / 개선점

1. **StatefulWidget 불필요**
   - 현재 상태 관리 필요 없음
   - 개선: `StatelessWidget`으로 변경 가능
2. **반응형 디자인 미지원**
   - 로고 크기와 여백 고정 → 작은 화면/태블릿에서 부적합
   - 개선: `Flexible`, `MediaQuery` 사용
3. **중앙 공간 처리**
   - `Expanded(SizedBox.shrink())` 사용 → 조금 어색
   - 개선: `Spacer()` 사용 시 가독성 향상
4. **테마/스타일 통일**
   - 배경색, 높이, 마진 등이 코드 내 하드코딩
   - 개선: Theme 또는 상수 관리

------

## 7️⃣ 개선된 예시 (Stateless + 반응형)

```
class HeaderWidget extends StatelessWidget {
  const HeaderWidget({super.key});

  @override
  Widget build(BuildContext context) {
    return Container(
      height: 80,
      width: double.infinity,
      color: AppColors.F5F5F5,
      padding: const EdgeInsets.symmetric(horizontal: 22),
      child: Row(
        children: [
          Image.asset("${AppStrings.assetPath}img_logo.png", width: 106),
          Spacer(),
          ToHomeBtn(),
        ],
      ),
    );
  }
}
```

- `StatelessWidget` 사용 → 불필요한 상태 제거
- `Spacer()` → 중간 공간 처리 가독성 향상
- `padding`으로 좌우 여백 통일

------

정리하면, 이 파일은 **앱 상단 헤더 위젯**으로, 좌측 로고와 우측 홈 버튼을 배치하는 단순 UI입니다.
 현재 구조는 간단하지만 **StatefulWidget 불필요, 반응형/테마 개선 가능**합니다.