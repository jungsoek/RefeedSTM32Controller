# app_strings.dart

## 소스 코드

### 전체 코드

```dart
class AppStrings {
  static const String assetPath = "assets/images/";

  static const String startButton = "startButton";
  static const String exitApp = "exitApp";
  static const String enterPhoneNumber = "enterPhoneNumber";
  static const String enterButton = "enterButton";
  static const String doorOpening = "doorOpening";
  static const String doorClosing = "doorOpening";
  static const String openAction = "openAction";
  static const String closeAction = "closeAction";
  static const String okayButton = "okayButton";
  static const String recheckButton = "recheckButton";
  static const String settingsMenu = "settingsMenu";
  static const String oilContainerLabel = "oilContainerLabel";
  static const String measurementLabel = "measurementLabel";
  static const String motorLabel = "motorLabel";
  static const String valveLabel = "valveLabel";
  static const String stopButton = "stopButton";

  static const String step = "step";
  static const String checkValidationToast = "checkValidationToast";
  static const String showTheProverDriverCode = "showTheProverDriverCode";

  static const String tryToConnect = "Trying to connect;";
  static String cannotCreatePort = "Failed to create port";
  static String cannotOpenPort = "Failed to open port";
  static String trySleep = "120s / Write [CMD]SLEEP#";
  static String noConnectableDevicesFound = "No connectable devices found";
  static String deviceSelectionCanceled = "Device selection canceled";

  static String fullContainer = "The container is full. Please contact staff";
  static String accessDenied = "Access denied. Please check your information.";
  static String systemError = "A system error has occurred. Please try again later.";
  static String notAuthorized = "Not Authorized.";
}
```

## 1️⃣ 파일 개요

**파일명:** `lib/common/app_strings.dart`
 **역할:** Flutter 앱에서 사용하는 **문자열 상수(Strings) 관리**
 **주요 목적:**

- UI, 버튼, 라벨, 토스트, 상태 메시지 등 **앱 전반에서 반복되는 문자열** 관리
- 코드 내 **하드코딩 방지**
- 다국어 대응(i18n) 전 단계에서 **문자열 중앙화**

**사용 맥락 예시:**

- 버튼 텍스트: `AppStrings.startButton`
- 안내 메시지/토스트: `AppStrings.checkValidationToast`
- 장치 연결 상태 표시: `AppStrings.tryToConnect`
- 라벨/스텝 표시: `AppStrings.step`

------

## 2️⃣ 주요 기능

1. **이미지/자산 경로 관리**
   - `assetPath = "assets/images/"`
   - 이미지 파일 경로를 문자열 상수로 정의, 추후 로딩 시 재사용
2. **UI 요소 문자열**
   - 버튼: `startButton`, `exitApp`, `enterButton`, `okayButton`, `recheckButton`, `stopButton`
   - 라벨: `oilContainerLabel`, `measurementLabel`, `motorLabel`, `valveLabel`
   - 메뉴: `settingsMenu`
3. **상태/행동 관련 문자열**
   - 도어 상태: `doorOpening`, `doorClosing`
   - 동작: `openAction`, `closeAction`
4. **단계/검증 관련**
   - 단계 표시: `step`
   - 검증 토스트 메시지: `checkValidationToast`
   - 운전자 코드 표시: `showTheProverDriverCode`
5. **장치/포트 상태 메시지**
   - 연결 시도: `tryToConnect`
   - 포트 생성 실패: `cannotCreatePort`
   - 포트 열기 실패: `cannotOpenPort`
   - 장치 없음/취소: `noConnectableDevicesFound`, `deviceSelectionCanceled`
   - 장치 절전 모드: `trySleep`
6. **오류/권한 메시지**
   - 컨테이너 가득 참: `fullContainer`
   - 접근 거부: `accessDenied`
   - 시스템 오류: `systemError`
   - 인증 실패: `notAuthorized`

------

## 3️⃣ 구조 분석

```
AppStrings (class)
 ├─ 자산 경로
 │    └─ assetPath
 ├─ 버튼 문자열
 │    ├─ startButton, exitApp, enterButton, okayButton, recheckButton, stopButton
 ├─ 라벨/메뉴
 │    ├─ oilContainerLabel, measurementLabel, motorLabel, valveLabel
 │    └─ settingsMenu
 ├─ 단계/검증
 │    ├─ step, checkValidationToast, showTheProverDriverCode
 ├─ 상태/동작
 │    ├─ doorOpening, doorClosing, openAction, closeAction
 └─ 장치/포트 및 오류 메시지
      ├─ tryToConnect, cannotCreatePort, cannotOpenPort, trySleep
      ├─ noConnectableDevicesFound, deviceSelectionCanceled
      └─ fullContainer, accessDenied, systemError, notAuthorized
```

**특징:**

- 문자열을 **중앙화하여 재사용 가능**
- 앱 전반에서 **일관된 메시지/텍스트 유지**
- 포트/장치 관련 메시지, UI 버튼/라벨 문자열 모두 관리

------

## 4️⃣ 동작 흐름 예시

1. 버튼 생성 시:

```
ElevatedButton(
  onPressed: () {},
  child: Text(AppStrings.startButton),
);
```

1. 토스트 메시지 표시 시:

```
ScaffoldMessenger.of(context).showSnackBar(
  SnackBar(content: Text(AppStrings.checkValidationToast)),
);
```

1. 장치 연결 상태 표시:

```
Text(AppStrings.tryToConnect)
```

1. 오류 메시지 처리:

```
if (!authorized) {
  showDialog(
    context: context,
    builder: (_) => AlertDialog(
      title: Text("Error"),
      content: Text(AppStrings.accessDenied),
    ),
  );
}
```

------

## 5️⃣ 장점

- **중앙 관리**: 문자열을 한 곳에서 관리해 변경 용이
- **코드 가독성 향상**: `AppStrings.startButton` → 하드코딩 "startButton" 사용 대비 명확
- **재사용성**: UI, 토스트, 다이얼로그 등 다양한 곳에서 재사용 가능
- **국제화(i18n) 확장 용이**: 추후 `AppStrings`를 로컬라이즈 구조로 교체 가능

------

## 6️⃣ 단점 / 개선점

1. **중복 문자열**
   - `doorOpening`과 `doorClosing`이 둘 다 `"doorOpening"`으로 설정됨
   - 개선: `doorClosing = "doorClosing"`으로 수정 필요
2. **상수/변수 혼합**
   - 일부는 `static const`, 일부는 `static String`
   - 개선: 변경 불가능한 문자열은 `const`로 통일
3. **국제화(i18n) 미적용**
   - 현재 문자열은 단일 언어 고정
   - 개선: Flutter `intl` 패키지와 연동해 다국어 지원
4. **사용자 친화적 메시지 부족**
   - 예: `cannotOpenPort = "Failed to open port"` → UI 레벨에서 좀 더 안내 메시지 가능

------

## 7️⃣ 개선된 사용 예시 (개념)

```
// 버튼 텍스트
TextButton(
  onPressed: () {},
  child: Text(AppStrings.startButton),
);

// 도어 상태 표시
Text(isOpening ? AppStrings.doorOpening : AppStrings.doorClosing),

// 장치 연결 상태
Text(AppStrings.tryToConnect),

// 오류 메시지
showDialog(
  context: context,
  builder: (_) => AlertDialog(
    title: Text("Error"),
    content: Text(AppStrings.systemError),
  ),
);
```

- 문자열 변경 시 **앱 전역에 자동 반영**
- 오류/상태 메시지 통일 가능