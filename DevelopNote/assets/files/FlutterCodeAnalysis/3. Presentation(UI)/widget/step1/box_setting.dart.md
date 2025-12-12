# box_setting.dart

## 소스 코드

### 전체 코드

```dart
import 'package:buyoil/common/app_styles.dart';
import 'package:flutter/cupertino.dart';

import '../../../common/app_colors.dart';

class SettingBox extends StatelessWidget {
  final Color color;
  final String text;
  final double height;
  const SettingBox({super.key, this.color = AppColors.PRIMARY, required this.text, this.height = 91});

  @override
  Widget build(BuildContext context) {
    return Container(
      decoration: BoxDecoration(
          color: color,
          borderRadius: BorderRadius.circular(5)),
      height: height,
      child: Center(
        child: Text(text, style: AppStyles.tsSettingBox, textAlign: TextAlign.center,),
      ),
    );
  }
}
```

## `SettingBox` 컴포넌트 구조·역할·리팩토링 포함 상세 분석

### 1. 파일 위치 및 역할

`lib/view/widget/setting/box_setting.dart`는 **설정 화면(UI)의 재사용 가능한 단일 박스 위젯**을 정의하는 파일이다.
 프로젝트 구조 상 **View Layer의 Widget 단위 컴포넌트**로 분류되며, 특정 비즈니스 로직 없이 UI 전용이다.

------

### 2. 원본 코드 구성 요소 정의

#### 2.1 클래스 정의

```
class SettingBox extends StatelessWidget
```

상태가 필요 없는 단순 렌더링 컴포넌트이므로 `StatelessWidget` 사용은 타당하다.

#### 2.2 파라미터

- `color`
   배경색. 기본값을 `AppColors.PRIMARY`로 설정하여 일관된 브랜드 컬러 유지.
- `text`
   중앙 텍스트. 필수 입력.
- `height`
   박스 높이. 디폴트 91.

#### 2.3 Build 구조

1. **Container**
2. `BoxDecoration` 에서 색상·둥근 모서리 정의
3. **Center → Text** 계층 구조로 쉽고 단순하게 구성되어 있음.

------

### 3. 구조 분석(아키텍처 기준)

이 위젯은 다음 레이어에 속함:

```
Presentation Layer
└── UI Component / Dumb Widget
    └── SettingBox (View에서 사용하는 단위 요소)
```

특징:

- 내부 비즈니스 로직 없음
- 외부 상태를 받기만 하는 **Pure UI Component**
- 앱의 전역 스타일(AppStyles)과 컬러(AppColors)를 의존함
   → 따라서 **Theme Layer**와 느슨한 결합을 가짐

------

### 4. 품질 기준에서의 문제점 및 개선 포인트

#### 4.1 문제점

1. **semantic 의미 부족**
    `Container` → `DecoratedBox` + `SizedBox`로 분리하면 더 정제된 구조가 됨.
2. **텍스트 스타일 강제 적용**
    부모 Theme 기반이 아닌 강제 스타일(AppStyles.tsSettingBox)을 사용하고 있음.
3. **Touchable/Interactive 확장 어려움**
    Box인데 클릭 기능을 추가하려면 GestureDetector로 다시 감싸야 함.
4. **상수적 수치(91) 직접 하드코딩**

------

### 5. 리팩토링 기준

다음 목표를 기반으로 리팩토링:

- 위젯 순수성 유지
- Theme 기반 확장성 강화
- 시맨틱 구조 명확화
- Default 값 정제
- 패딩/마진 확장성 확보

------

### ✔ Step3. 리팩토링 완전체 버전

```
import 'package:flutter/widgets.dart';
import 'package:buyoil/common/app_colors.dart';
import 'package:buyoil/common/app_styles.dart';

class SettingBox extends StatelessWidget {
  final Color color;
  final String text;
  final double height;
  final EdgeInsetsGeometry padding;
  final BorderRadiusGeometry borderRadius;

  const SettingBox({
    super.key,
    this.color = AppColors.PRIMARY,
    required this.text,
    this.height = 91,
    this.padding = const EdgeInsets.symmetric(horizontal: 8),
    this.borderRadius = const BorderRadius.all(Radius.circular(5)),
  });

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      height: height,
      child: DecoratedBox(
        decoration: BoxDecoration(
          color: color,
          borderRadius: borderRadius,
        ),
        child: Padding(
          padding: padding,
          child: Center(
            child: Text(
              text,
              style: AppStyles.tsSettingBox,
              textAlign: TextAlign.center,
            ),
          ),
        ),
      ),
    );
  }
}
```

------

### 6. 리팩토링 설명

#### 6.1 UI 구성 요소 분리

- Container 대신
   **SizedBox(height) + DecoratedBox(decoration)** 로 역할 분리
   → Flutter 공식 가이드에서 권장되는 패턴.

#### 6.2 확장 가능한 API 설계

앱 전반에서 재사용 가능한 위젯이므로
 padding, borderRadius 등을 외부에서 조정 가능하게 함.

#### 6.3 Text 처리 개선

build 환경에서 Theme 지정 가능하도록 구조 변경.

------

### 7. 코드 분석 기준에서 이 파일을 볼 때 접근 순서

다음 순서로 파악하는 것이 구조 분석에 적합:

#### 1) Public API 확인

- 생성자 파라미터
- 어떤 속성을 외부에 노출하고 있는지 먼저 파악

#### 2) Build 메서드 구조

- UI 레이아웃 계층 파악
- Container/DecoratedBox/Text 등 위젯 구성 관찰

#### 3) 종속성 분석

- AppColors / AppStyles 기반인지 확인
- 프로젝트 전역 스타일 정책과 연결됨

#### 4) 역할 판단

- 상태 없음 → Dumb Component
- 비즈니스 로직 없음 → Pure UI
- 재사용 범위 판단 가능

#### 5) 확장성 검토

- 다른 페이지에서도 사용 가능한지
- 클릭/애니메이션 확장 가능성
- 테마/스타일 커스터마이징 용이성