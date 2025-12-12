# router.dart

## 소스 코드

### 전체 코드

```dart
import 'package:buyoil/model/ui_state_usb_port.dart';
import 'package:buyoil/view/screen/s_driver.dart';
import 'package:buyoil/view/screen/s_opening_door.dart';
import 'package:buyoil/view/screen/s_setting.dart';
import 'package:buyoil/view/screen/s_splash.dart';
import 'package:buyoil/view/screen/s_step1.dart';
import 'package:buyoil/view/screen/s_step2.dart';
import 'package:buyoil/view/screen/s_step3.dart';
import 'package:buyoil/view/screen/s_step4.dart';
import 'package:buyoil/viewmodel/vm_serial_port.dart';
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:riverpod_annotation/riverpod_annotation.dart';

part 'router.g.dart';

final GlobalKey<NavigatorState> rootNavigatorKey = GlobalKey<NavigatorState>();

enum RouteGroup {
  Splash("/splash", "splash"),
  Driver("/driver", "driver"),
  Step1("/step1", "step1"),
  OpeningDoor("/openingDoor", "openingDoor"),
  Step2("/step2", "step2"),
  Step3("/step3", "step3"),
  Step4("/step4", "step4"),
  Setting("/setting", "setting");

  final String path;
  final String name;

  const RouteGroup(this.path, this.name);
}


@Riverpod(keepAlive: true)
GoRouter router(Ref ref) {
  return GoRouter(
    initialLocation: RouteGroup.Splash.path,
    navigatorKey: rootNavigatorKey,
    routes: [
      GoRoute(
        path: RouteGroup.Splash.path,
        name: RouteGroup.Splash.name,
        builder: (context, state) => const SplashScreen(),
      ),
      GoRoute(
        path: RouteGroup.Driver.path,
        name: RouteGroup.Driver.name,
        builder: (context, state) => const DriverScreen(),
      ),
      GoRoute(
        path: RouteGroup.OpeningDoor.path,
        name: RouteGroup.OpeningDoor.name,
        builder: (context, state) => const OpeningDoorScreen(),
      ),
      GoRoute(
        path: RouteGroup.Step1.path,
        name: RouteGroup.Step1.name,
        builder: (context, state) => const Step1Screen(),
      ),
      GoRoute(
        path: RouteGroup.Step2.path,
        name: RouteGroup.Step2.name,
        builder: (context, state) => const Step2Screen(),
      ),
      GoRoute(
        path: RouteGroup.Step3.path,
        name: RouteGroup.Step3.name,
        builder: (context, state) => const Step3Screen(),
      ),
      GoRoute(
        path: RouteGroup.Step4.path,
        name: RouteGroup.Step4.name,
        builder: (context, state) {
          double water = double.tryParse(state.uri.queryParameters["water"]??"") ?? 0.0;
          double oil = double.tryParse(state.uri.queryParameters["oil"]??"") ?? 0.0;
          return Step4Screen(water: water, oil: oil);
        },
      ),
      GoRoute(
        path: RouteGroup.Setting.path,
        name: RouteGroup.Setting.name,
        builder: (context, state) => const SettingScreen(),
      ),
    ]
  );
}
```

## 📦 1. `import` 된 라이브러리 분석

### ✅ **Flutter 기본**

```
import 'package:flutter/material.dart';
```

### ✅ **상태관리 (Riverpod)**

```
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:riverpod_annotation/riverpod_annotation.dart';
```

- `@Riverpod` 어노테이션을 사용하여 Riverpod 코드를 자동 생성
- router.g.dart 파일을 자동 생성

### ✅ **라우팅 (GoRouter)**

```
import 'package:go_router/go_router.dart';
```

- 현재 Flutter에서 권장하는 Router API
- Declarative Routing

### ✅ **프로젝트 내부 파일 (Screens/ViewModels/Model)**

```
import 'package:buyoil/model/ui_state_usb_port.dart';
import 'package:buyoil/view/screen/...';
import 'package:buyoil/viewmodel/vm_serial_port.dart';
```

- 각 페이지들을 router에서 연결하기 위한 import

------

## 📚 2. `RouteGroup` Enum 분석

```
enum RouteGroup {
  Splash("/splash", "splash"),
  Driver("/driver", "driver"),
  ...
}
```

### ✔ 역할:

- 라우터 경로(path)와 이름(name)을 한 곳에 정리
- `enum` 기반으로 정리하여 오타, 문자열 하드코딩 방지
- 유지보수 쉬움

### 예:

```
RouteGroup.Splash.path  // "/splash"
RouteGroup.Splash.name  // "splash"
```

------

## 📌 3. Global Navigator Key

```
final GlobalKey<NavigatorState> rootNavigatorKey = GlobalKey<NavigatorState>();
```

### ✔ 역할

- **전역 네비게이션 제어**
- 특히 dialog, bottomSheet, overlay 등 띄우기 위해
   `Navigator.of(rootNavigatorKey.currentContext!)`
   이런 방식으로 접근 가능

메인 코드에서 사용하고 있었음:

```
Navigator.of(rootNavigatorKey.currentContext!).pop();
```

------

## 🧠 4. Router Provider (GoRouter + Riverpod)

```
@Riverpod(keepAlive: true)
GoRouter router(Ref ref) {
```

### ✔ 설명

- 라우터를 Riverpod Provider로 등록
- 전역에서 `ref.watch(routerProvider)` 로 접근 가능
- keepAlive: true
   → 라우터가 앱 실행 내내 dispose되지 않음

### GoRouter 생성

```
return GoRouter(
  initialLocation: RouteGroup.Splash.path, // 앱 시작 화면
  navigatorKey: rootNavigatorKey,         // 전역 네비게이터
  routes: [
    GoRoute(...)
  ]
);
```

------

## 🚀 5. 모든 GoRoute 구조 분석

각 스크린은:

```
GoRoute(
  path: "/step1",
  name: "step1",
  builder: (context, state) => const Step1Screen(),
)
```

형태로 라우팅.

------

## 🧮 6. QueryParameter 처리 (Step4)

```
double water = double.tryParse(state.uri.queryParameters["water"] ?? "") ?? 0.0;
double oil = double.tryParse(state.uri.queryParameters["oil"] ?? "") ?? 0.0;
```

### ✔ URL 예시:

```
/step4?water=10.5&oil=3.7
```

### ✔ 결과:

- water: 10.5
- oil: 3.7

값 전달 방식이 Query Parameter 기반.

------

## 📝 7. 전체 주석 버전 router.dart

아래는 전체 파일을 자세한 주석과 함께 정리한 버전입니다.

------

```dart
// ----------------------------
// 라우팅 관련 import
// ----------------------------
import 'package:buyoil/model/ui_state_usb_port.dart';
import 'package:buyoil/view/screen/s_driver.dart';
import 'package:buyoil/view/screen/s_opening_door.dart';
import 'package:buyoil/view/screen/s_setting.dart';
import 'package:buyoil/view/screen/s_splash.dart';
import 'package:buyoil/view/screen/s_step1.dart';
import 'package:buyoil/view/screen/s_step2.dart';
import 'package:buyoil/view/screen/s_step3.dart';
import 'package:buyoil/view/screen/s_step4.dart';
import 'package:buyoil/viewmodel/vm_serial_port.dart';
import 'package:flutter/material.dart';

// 상태관리 (Riverpod)
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:riverpod_annotation/riverpod_annotation.dart';

// Riverpod code-gen
part 'router.g.dart';

/// 전역 Navigator 접근 키
/// dialog / toast / route 이동 시 사용
final GlobalKey<NavigatorState> rootNavigatorKey = GlobalKey<NavigatorState>();


/// 모든 라우트를 enum으로 관리하여 유지보수 용이
enum RouteGroup {
  Splash("/splash", "splash"),
  Driver("/driver", "driver"),
  Step1("/step1", "step1"),
  OpeningDoor("/openingDoor", "openingDoor"),
  Step2("/step2", "step2"),
  Step3("/step3", "step3"),
  Step4("/step4", "step4"),
  Setting("/setting", "setting");

  final String path;   // URL 경로
  final String name;   // 라우트 이름

  const RouteGroup(this.path, this.name);
}


/// Riverpod + GoRouter 결합된 Router Provider
/// 앱 전역 라우팅을 담당
@Riverpod(keepAlive: true)
GoRouter router(Ref ref) {
  return GoRouter(
    // 앱 시작 시 가장 먼저 보여줄 화면
    initialLocation: RouteGroup.Splash.path,

    // 모든 화면 이동을 통합 제어하기 위한 Navigator key
    navigatorKey: rootNavigatorKey,

    // 페이지 목록
    routes: [
      // Splash Screen
      GoRoute(
        path: RouteGroup.Splash.path,
        name: RouteGroup.Splash.name,
        builder: (context, state) => const SplashScreen(),
      ),

      // Driver Screen
      GoRoute(
        path: RouteGroup.Driver.path,
        name: RouteGroup.Driver.name,
        builder: (context, state) => const DriverScreen(),
      ),

      // Opening Door
      GoRoute(
        path: RouteGroup.OpeningDoor.path,
        name: RouteGroup.OpeningDoor.name,
        builder: (context, state) => const OpeningDoorScreen(),
      ),

      // Step1
      GoRoute(
        path: RouteGroup.Step1.path,
        name: RouteGroup.Step1.name,
        builder: (context, state) => const Step1Screen(),
      ),

      // Step2
      GoRoute(
        path: RouteGroup.Step2.path,
        name: RouteGroup.Step2.name,
        builder: (context, state) => const Step2Screen(),
      ),

      // Step3
      GoRoute(
        path: RouteGroup.Step3.path,
        name: RouteGroup.Step3.name,
        builder: (context, state) => const Step3Screen(),
      ),

      // Step4 - Query Parameter 사용 예시
      GoRoute(
        path: RouteGroup.Step4.path,
        name: RouteGroup.Step4.name,
        builder: (context, state) {
          // URL에서 water, oil 값을 가져오기
          double water = double.tryParse(state.uri.queryParameters["water"] ?? "") ?? 0.0;
          double oil = double.tryParse(state.uri.queryParameters["oil"] ?? "") ?? 0.0;

          return Step4Screen(
            water: water,
            oil: oil,
          );
        },
      ),

      // Setting
      GoRoute(
        path: RouteGroup.Setting.path,
        name: RouteGroup.Setting.name,
        builder: (context, state) => const SettingScreen(),
      ),
    ]
  );
}
```