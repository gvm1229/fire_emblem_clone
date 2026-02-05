# ShootingGame 프로젝트

## 프로젝트 개요
C++ 콘솔 기반의 슈팅 게임 프로젝트입니다. 자체 제작한 콘솔 게임 엔진(Engine)을 사용하여 구현된 2D 슈팅 게임입니다.

## 프로젝트 구조

### 1. Engine 프로젝트 (게임 엔진)
게임의 핵심 시스템을 담당하는 엔진 라이브러리로, DLL 형태로 빌드됩니다.

#### 주요 컴포넌트:

**Core 시스템:**
- `Engine/Engine/Engine.h/cpp`: 메인 게임 엔진 클래스
  - 게임 루프 관리 (BeginPlay → Tick → Draw)
  - 프레임레이트 제어 (Setting.txt에서 설정 로드)
  - 레벨 관리 (SetNewLevel, 레벨 전환)
  - 싱글톤 패턴으로 전역 접근 제공

- `Engine/Core/Input.h/cpp`: 입력 처리 시스템
  - 키보드 입력 처리 (GetKey, GetKeyDown, GetKeyUp)
  - 마우스 입력 처리 (GetMouseButton, GetMousePosition)
  - 프레임별 입력 상태 업데이트

**렌더링 시스템:**
- `Engine/Render/Renderer.h/cpp`: 렌더링 관리자
  - 더블 버퍼링을 사용한 화면 렌더링
  - 액터 그리기 및 화면 출력
  - 즉시 화면 표시 기능 (PresentImmediately)

- `Engine/Render/ScreenBuffer.h/cpp`: 화면 버퍼 관리
  - 콘솔 화면 버퍼 관리
  - 픽셀 단위 색상 설정

**액터 시스템:**
- `Engine/Actor/Actor.h/cpp`: 게임 오브젝트 기본 클래스
  - 게임 내 모든 오브젝트의 베이스 클래스
  - BeginPlay, Tick, Draw 생명주기 관리
  - 위치, 이미지, 색상 속성
  - 충돌 감지 (TestIntersect)
  - 소유권 관리 (Owner/Level)
  - Destroy/OnDestroy 패턴
  - RTTI(Run-Time Type Information) 지원

**레벨 시스템:**
- `Engine/Level/Level.h/cpp`: 레벨(씬) 기본 클래스
  - 액터 생성 및 관리
  - 액터 생명주기 관리
  - 액터 Tick 및 Draw 호출

**수학 라이브러리:**
- `Engine/Math/Vector2.h/cpp`: 2D 벡터 클래스
- `Engine/Math/Color.h`: 색상 정보 클래스

**유틸리티:**
- `Engine/Common/Common.h`: 공통 헤더 및 매크로
- `Engine/Common/RTTI.h`: RTTI(런타임 타입 정보) 시스템
- `Engine/Util/Util.h`: 유틸리티 함수들

### 2. Game 프로젝트 (게임 로직)
Engine 프로젝트를 사용하여 구현된 실제 슈팅 게임입니다.

#### 게임 레벨:
- `Game/Level/GameLevel.h/cpp`: 메인 게임 레벨
  - 게임 점수 관리
  - 플레이어-적 탄환 충돌 처리
  - 플레이어 탄환-적 충돌 처리
  - 게임 오버 처리
  - 점수 화면 표시

#### 플레이어 관련:
- `Game/Actor/Player.h/cpp`: 플레이어 캐릭터
  - 좌우 이동 (MoveLeft, MoveRight)
  - 단발/연발 사격 모드
  - 사격 쿨타임 관리 (Timer 사용)
  - 키보드 입력 처리

- `Game/Actor/PlayerBullet.h/cpp`: 플레이어 발사체
  - 위로 이동하는 탄환
  - 화면 밖으로 나가면 자동 제거

#### 적 관련:
- `Game/Actor/Enemy.h/cpp`: 적 캐릭터
  - 좌우 이동 (화면 경계에서 방향 전환)
  - 랜덤 시간 간격으로 탄환 발사
  - 데미지 처리 (OnDamaged)
  - 커스터마이징 가능한 이미지 및 Y 위치

- `Game/Actor/EnemyBullet.h/cpp`: 적 발사체
  - 아래로 이동하는 탄환
  - 화면 밖으로 나가면 자동 제거

- `Game/Actor/EnemySpawner.h/cpp`: 적 생성기
  - 일정 시간 간격으로 적 생성
  - 여러 줄의 적 생성 관리

- `Game/Actor/EnemyDestroyEffect.h/cpp`: 적 파괴 이펙트
  - 적이 파괴될 때 재생되는 시각 효과
  - 일정 시간 후 자동 제거

#### 기타:
- `Game/Actor/MouseTester.h/cpp`: 마우스 입력 테스트용 액터
- `Game/Util/Timer.h/cpp`: 타이머 유틸리티 클래스
  - 시간 간격 체크
  - 쿨다운 관리

- `Game/Main.cpp`: 프로그램 진입점
  - 메모리 누수 체크 설정
  - 엔진 초기화 및 게임 레벨 시작

### 3. Assets 폴더
게임에서 사용하는 리소스 파일들:
- `Assets/Map.txt`: 맵 데이터
- `Assets/Stage1.txt`, `Stage2.txt`, `Stage3.txt`: 스테이지별 데이터

### 4. Config 폴더
- `Config/Setting.txt`: 게임 엔진 설정
  - `framerate`: 게임 프레임레이트 (60 FPS)
  - `width`: 콘솔 화면 너비 (40)
  - `height`: 콘솔 화면 높이 (20)

## 주요 기능 (커밋 히스토리 기반)

### 1. 엔진 기본 기능
- **더블 버퍼링**: 화면 깜빡임 방지를 위한 더블 버퍼링 구현
- **입력 처리**: 키보드 및 마우스 입력 처리 시스템
- **게임 루프**: 고정 프레임레이트 기반 게임 루프
- **레벨 시스템**: 씬 관리 및 전환 시스템
- **액터 시스템**: 게임 오브젝트의 생성, 업데이트, 삭제 관리

### 2. 플레이어 기능
- **이동**: 좌우 이동 (키보드 입력)
- **사격**: 
  - 단발 사격 모드
  - 연속 사격 모드
  - 사격 쿨타임 시스템
- **충돌 감지**: 적 탄환과의 충돌 처리

### 3. 적 AI 기능
- **자동 이동**: 화면 좌우로 자동 이동 (경계에서 방향 전환)
- **자동 사격**: 랜덤한 시간 간격으로 탄환 발사
- **적 생성기**: 타이머 기반 적 자동 생성
- **파괴 효과**: 적 파괴 시 시각 효과 재생

### 4. 게임 시스템
- **점수 시스템**: 적 격파 시 점수 획득
- **충돌 시스템**: 
  - 플레이어 탄환 ↔ 적 충돌
  - 플레이어 ↔ 적 탄환 충돌
- **게임 오버**: 플레이어 사망 시 게임 오버 처리
- **화면 렌더링**: 콘솔 기반 ASCII 아트 렌더링

## 기술적 특징

### 디자인 패턴
- **싱글톤**: Engine 클래스 전역 접근
- **팩토리/생성**: Level의 SpawnActor를 통한 액터 생성
- **컴포지트**: Actor/Level 계층 구조
- **템플릿 메서드**: BeginPlay → Tick → Draw 생명주기
- **RTTI**: 런타임 타입 정보 시스템으로 타입 안전한 다운캐스팅

### 메모리 관리
- 명시적 메모리 누수 체크 (`_CRTDBG_MAP_ALLOC`)
- 스마트 포인터 대신 수동 메모리 관리 (학습 목적)
- 소유권 기반 액터 관리 (Level이 Actor 소유)

### 렌더링
- 콘솔 더블 버퍼링으로 깜빡임 방지
- ASCII 아트 기반 그래픽
- 색상 지원 (WORD 타입 색상)
- 정렬 순서(Sorting Order) 기반 렌더링

## 빌드 및 실행

### 요구사항
- Visual Studio 2019 이상
- Windows 10/11
- C++17 이상

### 빌드 순서
1. `Engine.vcxproj` 먼저 빌드 (DLL 생성)
2. `Game.vcxproj` 빌드 (Engine.lib 링크)
3. 실행 시 `Engine.dll`이 Game.exe와 같은 디렉터리에 있어야 함

### 프로젝트 설정
- Engine 프로젝트: DLL 출력
- Game 프로젝트: Engine 프로젝트에 대한 참조 포함
- Config/Setting.txt에서 게임 설정 조정 가능

## 프로젝트 빌드 설정 상세 (vcxproj)

### Engine 프로젝트 (Engine.vcxproj)

#### 출력 설정
```
ConfigurationType: DynamicLibrary (DLL 출력)
OutDir: $(SolutionDir)Bin\$(Platform)\$(Configuration)\$(ProjectName)\
IntDir: $(SolutionDir)Intermediate\$(Platform)\$(Configuration)\$(ProjectName)\
```

#### 전처리기 정의
```
ENGINE_BUILD_DLL: DLL 빌드 모드 활성화 (WANTED_API 매크로에서 __declspec(dllexport) 사용)
_DEBUG (Debug 모드): 디버그 빌드
NDEBUG (Release 모드): 릴리스 빌드
```

#### 추가 인클루드 디렉터리
```
$(ProjectDir)\: Engine 프로젝트 루트 디렉터리를 인클루드 경로에 추가
```

#### 빌드 이벤트

**PreBuildEvent (빌드 전):**
```batch
xcopy *.h ..\Includes\Engine\ /e /y
xcopy ..\Config\* $(OutDir)..\Config\ /e /y
```
- 모든 헤더 파일(.h)을 `Includes/Engine/` 폴더로 복사 (Game 프로젝트에서 참조 가능하도록)
- Config 폴더의 설정 파일들을 출력 디렉터리의 Config 폴더로 복사

**PostBuildEvent (빌드 후):**
```batch
xcopy $(OutDir)\Engine.dll ..\Library\Engine\$(Platform)\$(Configuration)\ /e /y
xcopy $(OutDir)\Engine.lib ..\Library\Engine\$(Platform)\$(Configuration)\ /e /y
```
- 빌드된 `Engine.dll`을 `Library/Engine/[Platform]/[Configuration]/` 폴더로 복사
- 빌드된 `Engine.lib` (import library)을 동일한 위치로 복사
- Game 프로젝트가 이 라이브러리를 링크할 수 있도록 함

### Game 프로젝트 (Game.vcxproj)

#### 출력 설정
```
ConfigurationType: Application (실행 파일 출력)
OutDir: $(SolutionDir)Bin\$(Platform)\$(Configuration)\$(ProjectName)\
IntDir: $(SolutionDir)Intermediate\$(Platform)\$(Configuration)\$(ProjectName)\
```

#### 추가 인클루드 디렉터리
```
$(ProjectDir)\: Game 프로젝트 루트 디렉터리
..\Engine: Engine 프로젝트 헤더 파일 접근을 위한 경로
```

#### 링커 설정
```
AdditionalDependencies: Engine.lib (Engine DLL의 import library 링크)
AdditionalLibraryDirectories: ..\Library\Engine\$(Platform)\$(Configuration)\
```

#### 빌드 이벤트

**PostBuildEvent (빌드 후):**
```batch
xcopy ..\Library\Engine\$(Platform)\$(Configuration)\Engine.dll $(OutDir)\ /e /y
```
- Engine.dll을 Game.exe가 있는 출력 디렉터리로 복사
- 실행 시 DLL을 찾을 수 있도록 함

### 빌드 출력 구조
```
ShootingGame/
├─ Bin/
│  └─ x64/
│     ├─ Debug/
│     │  ├─ Engine/
│     │  │  ├─ Engine.dll
│     │  │  └─ Engine.lib
│     │  ├─ Game/
│     │  │  ├─ Game.exe
│     │  │  └─ Engine.dll (PostBuildEvent로 복사됨)
│     │  └─ Config/
│     │     └─ Setting.txt
│     └─ Release/
│        └─ (동일한 구조)
├─ Intermediate/ (중간 빌드 파일 .obj 등)
└─ Library/
   └─ Engine/
      └─ x64/
         ├─ Debug/
         │  ├─ Engine.dll
         │  └─ Engine.lib
         └─ Release/
            ├─ Engine.dll
            └─ Engine.lib
```
