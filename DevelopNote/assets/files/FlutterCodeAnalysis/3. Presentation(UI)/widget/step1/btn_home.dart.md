# btn_home.dart

## 소스 코드

### 전체 코드

```dart
import 'package:buyoil/viewmodel/vm_serial_port.dart';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../../common/app_strings.dart';
import '../../router.dart';

class ToHomeBtn extends ConsumerWidget {
  const ToHomeBtn({super.key});

  @override
  Widget build(BuildContext context, ref) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: () {
        ref.watch(serialPortVMProvider.notifier).goToSplash().whenComplete(() {
          context.goNamed(RouteGroup.Splash.name);
        });
      },
      child: Container(
        width: 99,
        height: 49,
        margin: EdgeInsets.symmetric(vertical: 15, horizontal: 13),
        child: Image.asset("${AppStrings.assetPath}img_home.png"),
      ),
    );
  }

}
```

## 1. 구조 분석

`ToHomeBtn`은 다음 기능을 제공하는 단일-purpose UI 컴포넌트이다.

- `Riverpod`의 `serialPortVMProvider.notifier` 를 호출하여 시리얼 포트 관련 초기화 또는 종료 처리 실행
- `GestureDetector` 를 사용하여 터치 이벤트를 수신
- Splash 화면으로 라우팅(`go_router`)
- 정해진 크기(99×49)의 홈 버튼 이미지 렌더링

구조적 요소:

```
ConsumerWidget
 └─ GestureDetector
      └─ Container
           └─ Image.asset(img_home)
```

------

## 2. 세부 동작 흐름

1. **build(context, ref)**
    `ConsumerWidget`의 표준 시그니처 (`build(BuildContext context, WidgetRef ref)` 형태가 맞음)
2. **onTap → serialPortVMProvider.notifier.goToSplash() 호출**
    시리얼 포트 VM에서 splash 진입을 위한 백엔드 상태 정리 수행.
3. **whenComplete → context.goNamed(Splash)**
    VM 비동기 작업 완료 이후 Splash 화면으로 이동.
4. UI는 단순한 image 기반의 버튼.

------

## 3. 문제점 및 개선 포인트

### ✔ (1) GestureDetector보다 InkWell/InkResponse 사용이 더 적절

현재는 **Material ripple 효과 없음**, 접근성(A11Y)도 부족하다.

### ✔ (2) ConsumerWidget의 build 시그니처 오류

현재 코드:

```
@override
Widget build(BuildContext context, ref)
```

올바른 형태:

```
@override
Widget build(BuildContext context, WidgetRef ref)
```

IDE에서 경고가 없었던 이유는 dynamic 으로 처리됐기 때문.

### ✔ (3) ref.watch → ref.read 로 변경 필요

행위 함수 호출이므로 `watch` 사용 시 rebuild 불필요하게 발생함.

잘못된 형태:

```
ref.watch(serialPortVMProvider.notifier).goToSplash()
```

권장:

```
ref.read(serialPortVMProvider.notifier).goToSplash()
```

### ✔ (4) Container + Image만 있는 단순 버튼 → StatelessWidget로 충분

추가 상태 없음.

------

## 4. 정제된 공식문서 스타일 리팩토링 코드

```
import 'package:buyoil/viewmodel/vm_serial_port.dart';
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../../common/app_strings.dart';
import '../../router.dart';

class ToHomeBtn extends ConsumerWidget {
  const ToHomeBtn({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    return Material(
      color: Colors.transparent,
      child: InkWell(
        onTap: () {
          ref
              .read(serialPortVMProvider.notifier)
              .goToSplash()
              .whenComplete(() {
            context.goNamed(RouteGroup.Splash.name);
          });
        },
        borderRadius: const BorderRadius.all(Radius.circular(4)),
        child: Container(
          width: 99,
          height: 49,
          margin: const EdgeInsets.symmetric(vertical: 15, horizontal: 13),
          child: Image.asset(
            "${AppStrings.assetPath}img_home.png",
            fit: BoxFit.contain,
          ),
        ),
      ),
    );
  }
}
```

------

## 5. 리팩토링 특징 요약

- `InkWell`로 교체하여 Material ripple 효과 제공
- build 시그니처 수정 (`WidgetRef ref`)
- `ref.watch` → `ref.read` 로 변경
- borderRadius 제공으로 터치 영역 시각적 안정성 확보
- 스타일, 구조, 동작 방식은 기존과 동일하게 유지