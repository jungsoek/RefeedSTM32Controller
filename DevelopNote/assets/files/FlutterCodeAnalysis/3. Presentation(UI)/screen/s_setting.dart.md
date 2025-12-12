# s_setting.dart

## 소스 코드

### 전체 코드

```dart
import 'package:buyoil/common/app_strings.dart';
import 'package:buyoil/view/widget/setting/box_setting.dart';
import 'package:easy_localization/easy_localization.dart';
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import '../../common/app_colors.dart';
import '../../common/app_styles.dart';
import '../../viewmodel/vm_setting.dart';
import '../widget/setting/btn_setting.dart';
import '../widget/w_header.dart';

class SettingScreen extends ConsumerStatefulWidget {
  const SettingScreen({Key? key}) : super(key: key);

  @override
  ConsumerState<ConsumerStatefulWidget> createState() => SettingScreenState();
}

class SettingScreenState extends ConsumerState<SettingScreen> {

  @override
  Widget build(BuildContext context) {
    final notifier = ref.watch(settingProvider.notifier);
    final state = ref.watch(settingProvider);
    return Scaffold(
      body: Container(
        width: double.maxFinite,
        height: double.maxFinite,
        child: Column(
          children: [
            HeaderWidget(),
            Expanded(
              child: Column(
                children: [
                  Spacer(flex: 49),
                  Center(
                    child: ElevatedButton(
                        onPressed: () {},
                        style: ButtonStyle(
                            backgroundColor: WidgetStatePropertyAll(AppColors.PRIMARY),
                            padding: WidgetStatePropertyAll(EdgeInsets.symmetric(horizontal: 20)),
                            shape: WidgetStatePropertyAll(RoundedRectangleBorder(borderRadius: BorderRadius.circular(5)))
                        ),
                        child: Container(
                          height: 91,
                          // padding: EdgeInsets.all(20),

                          child: Row(
                            mainAxisSize: MainAxisSize.min,
                            children: [
                              Image.asset(AppStrings.assetPath + "img_gear.png", width: 52.5, height: 52.5,),
                              SizedBox(width: 13.5,),
                              Text(AppStrings.settingsMenu.tr(), style: AppStyles.tsSetting,)
                            ],
                          ),
                        )
                    ),
                  ),
                  Spacer(flex: 163),
                  Container(
                    height: 203 + 31 + 91,
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        SizedBox(width: 40,),
                        Expanded(
                            flex: 1,
                            child: Column(
                              mainAxisSize: MainAxisSize.min,
                              children: [
                                SettingBox(color: AppColors.FF868686, text: AppStrings.oilContainerLabel.tr()),
                                SizedBox(height: 31,),
                                SettingBox(text: "${state.oilContainer}kg", height: 203),
                              ],
                            )
                        ),
                        SizedBox(width: 28,),
                        Expanded(
                            flex: 1,
                            child: Column(
                              mainAxisSize: MainAxisSize.min,
                              children: [
                                SettingBox(color: AppColors.FF868686, text: AppStrings.oilContainerLabel.tr()),
                                SizedBox(height: 31,),
                                SettingBox(text: "Oil:${state.measureOil}g\nWater: ${state.measureWater}g", height: 203),
                              ],
                            )
                        ),
                        SizedBox(width: 28,),
                        Expanded(
                            flex: 1,
                            child: Column(
                              mainAxisSize: MainAxisSize.min,
                              children: [
                                SettingBox(color: AppColors.FF868686, text: AppStrings.oilContainerLabel.tr()),
                                SizedBox(height: 31,),
                                SettingButton(
                                  state: state.isMotorOn,
                                  onTap: () {
                                    notifier.toggleMotor();
                                  },
                                ),
                              ],
                            )
                        ),
                        SizedBox(width: 28,),
                        Expanded(
                            flex: 1,
                            child: Column(
                              mainAxisSize: MainAxisSize.min,
                              children: [
                                SettingBox(color: AppColors.FF868686, text: AppStrings.oilContainerLabel.tr()),
                                SizedBox(height: 31,),
                                SettingButton(
                                    state: state.isValveOn,
                                    onTap: () {
                                      notifier.toggleValve();
                                    },
                                ),
                              ],
                            )
                        ),
                        SizedBox(width: 40,),
                      ],
                    ),
                  ),
                  Spacer(flex: 93),
                ],
              ),
            )
          ],
        ),
      )
    );
  }

  void afterLayout() {
    // ref.listenManual(SettingProvider, (_, state) {
    //   if(state is UIStateSettingCompleted) {
    //     context.goNamed(RouteGroup.Setting.name);
    //   }
    // });
  }
}
```

## SettingScreen 구조 분석

본 파일은 설정 화면을 구성하는 View 레이어 코드이며, Riverpod 기반 상태 관리 구조에서 `settingProvider`의 읽기 및 액션 트리거를 수행한다. UI는 장치의 오일 용기 무게, 측정된 오일/물 값, 모터 및 밸브의 동작 여부를 시각적으로 표현하고, 이에 대한 제어 버튼을 제공하는 형태로 구성된다.

본 파일은 MVVM 아키텍처 관점에서 **View 계층의 UI 렌더링 및 인터랙션 이벤트 전달**만 담당하며, 실제 로직은 `vm_setting.dart`에서 관리된다.

------

## 클래스 구성

#### `SettingScreen extends ConsumerStatefulWidget`

- Riverpod `ConsumerStatefulWidget` 형태로, 상태 변화에 반응하는 UI 빌드를 수행한다.
- 외부에서 Key만 전달하며 추가 파라미터는 없다.

#### `SettingScreenState extends ConsumerState<SettingScreen>`

- 화면 렌더링의 중심이 되는 State 클래스이다.
- Provider의 `state` 및 `notifier`를 직접 참조한다.
  - `state = ref.watch(settingProvider)`
  - `notifier = ref.watch(settingProvider.notifier)`
- UI 외의 비즈니스 로직을 포함하지 않는다.

------

## UI 계층 구조

다음은 전체 UI 구조 트리다:

```
Scaffold
└─ Container (full size)
   └─ Column
      ├─ HeaderWidget
      └─ Expanded
         └─ Column
            ├─ Spacer(flex: 49)
            ├─ Center
            │   └─ ElevatedButton (설정 메뉴 버튼)
            ├─ Spacer(flex: 163)
            ├─ Container (설정 카드 묶음 영역)
            │   └─ Row
            │       ├─ Expanded (오일 용기 용량)
            │       │   └─ Column
            │       │       ├─ SettingBox(title)
            │       │       └─ SettingBox(value)
            │       ├─ Expanded (오일/물 측정값)
            │       ├─ Expanded (모터 on/off 버튼)
            │       └─ Expanded (밸브 on/off 버튼)
            └─ Spacer(flex: 93)
```

------

## 주요 구성 요소 분석

### 1) 설정 메뉴 버튼 영역

```
ElevatedButton(
  onPressed: () {},
  style: ButtonStyle(...),
  child: Row(...)
)
```

- 스타일은 `PRIMARY` 색상 기반.
- 아이콘 + 텍스트 조합으로 구성.
- 현재 onPressed는 비어 있으며 추후 기능 확장 가능성을 고려한 placeholder 역할.

### 2) 오일 용기 용량 표시

```
SettingBox(color: ..., text: AppStrings.oilContainerLabel.tr())
SettingBox(text: "${state.oilContainer}kg", height: 203)
```

- 상단 박스는 제목 역할.
- 하단 박스는 실시간 상태 값 표시 역할.
- `state.oilContainer`를 그대로 표기하므로 값 관리는 ViewModel이 담당한다.

### 3) 오일/물 측정값 표시

```
SettingBox(color: ...)
SettingBox(text: "Oil:${state.measureOil}g\nWater: ${state.measureWater}g")
```

- HX711 등을 통해 수집한 측정값으로 보이며, ViewModel에서 받아온 값을 그대로 반영한다.
- 단위(g)는 고정 문자열로 삽입되어 있다.

### 4) 모터 제어 버튼

```
SettingButton(
  state: state.isMotorOn,
  onTap: () {
    notifier.toggleMotor();
  },
)
```

- 현재 모터의 상태 값(state.isMotorOn)을 시각적으로 표현.
- onTap 시 ViewModel의 `toggleMotor()` 호출.
- View는 단순 이벤트 전달 기능만 수행.

### 5) 밸브 제어 버튼

```
SettingButton(
  state: state.isValveOn,
  onTap: () { notifier.toggleValve(); }
)
```

- 모터 제어 버튼과 동일한 구조.
- 내부의 실제 동작(on/off 명령 전송)은 ViewModel에서 수행한다.

------

## ViewModel 연동 구조

본 화면이 참조하는 Provider:

```
final notifier = ref.watch(settingProvider.notifier);
final state = ref.watch(settingProvider);
```

- `state`는 UI가 렌더링할 표시 데이터(oilContainer, measureOil, measureWater, isMotorOn, isValveOn)를 제공한다.
- `notifier`는 장치 제어 계층(toggleMotor, toggleValve)을 호출하는 entry point이다.

본 View는 비즈니스 로직을 전혀 갖지 않으며, 모든 장치 제어는 ViewModel의 책임이다.

------

## 아키텍처상 책임

SettingScreen의 책임은 다음 3개로 명확하게 구분된다:

1. **상태 데이터 표시**
   - ViewModel의 상태 값(oilContainer, measureOil, etc.)을 UI에 반영한다.
2. **사용자 입력 이벤트 전달**
   - 모터 on/off, 밸브 on/off 버튼 터치 이벤트를 ViewModel로 전달한다.
3. **정적 레이아웃 구성**
   - 헤더, 설정 버튼, 4개의 설정 요소 박스 등을 배치한다.

그 외:

- 장치 제어 명령 생성
- Serial 통신 흐름 관리
- 측정값 수집/검증
- 비즈니스 로직
   은 모두 ViewModel 및 Service 레이어에서 수행해야 하며, 본 View는 담당하지 않는다.

------

## 요약

- SettingScreen은 설정 페이지를 담당하는 **View 레이어**이다.
- Riverpod Provider를 기반으로 UI 상태를 렌더링한다.
- 모터·밸브 제어 버튼은 단순히 이벤트를 ViewModel로 전달하는 역할만 수행한다.
- 오일 용기 용량과 측정값은 ViewModel에서 전달받아 표시한다.
- 아키텍처 구조상 본 화면은 **상태 표시 + 이벤트 전달**에만 집중한다.