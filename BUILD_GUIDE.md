# FireEmblemClone 빌드 가이드

## 중요: 프로젝트 정리 안내

이 프로젝트는 최근 대규모 정리 작업을 거쳤습니다:
- **네임스페이스**: `Wanted` → `FE` 변경됨
- **삭제된 파일**: 33개 (슈팅 게임, Sokoban 관련)
- 자세한 내용은 [CLEANUP_SUMMARY.md](CLEANUP_SUMMARY.md) 참조

## 빌드 전 준비사항

### 1. Engine 프로젝트에 새 파일 추가

Visual Studio에서 Engine 프로젝트를 열고 다음 파일들을 추가하세요:

#### 새 폴더 생성: `Engine/Map/`
우클릭 → Add → New Folder → "Map"

#### Map 폴더에 파일 추가:
1. `Grid.h` (Header Files → Map)
2. `Grid.cpp` (Source Files → Map)
3. `Tile.h` (Header Files → Map)
4. `Tile.cpp` (Source Files → Map)

#### 기존 파일 확인:
- `Engine/Core/Input.h` (수정됨)
- `Engine/Core/Input.cpp` (수정됨)

### 2. Game 프로젝트에 새 파일 추가

Visual Studio에서 Game 프로젝트를 열고 다음 파일들을 추가하세요:

#### 새 폴더 생성: 
- `Game/Unit/`
- `Game/Item/`
- `Game/Combat/`
- `Game/AI/`
- `Game/System/`

#### Unit 폴더:
1. `UnitClass.h`
2. `UnitClass.cpp`
3. `Unit.h`
4. `Unit.cpp`

#### Item 폴더:
1. `Weapon.h`
2. `Weapon.cpp`

#### Combat 폴더:
1. `CombatCalculator.h`
2. `CombatCalculator.cpp`
3. `CombatSequence.h`
4. `CombatSequence.cpp`

#### AI 폴더:
1. `AIController.h`
2. `AIController.cpp`

#### System 폴더:
1. `TurnManager.h`
2. `TurnManager.cpp`
3. `PlayerPhaseController.h`
4. `PlayerPhaseController.cpp`
5. `EnemyPhaseController.h`
6. `EnemyPhaseController.cpp`

#### Level 폴더:
1. `MapLevel.h`
2. `MapLevel.cpp`

#### Main.cpp (수정됨)

### 3. 빌드 순서

1. **Engine 프로젝트 빌드**
   - 솔루션 탐색기에서 Engine 프로젝트 우클릭
   - "Build" 선택
   - 에러가 없는지 확인

2. **Game 프로젝트 빌드**
   - 솔루션 탐색기에서 Game 프로젝트 우클릭
   - "Build" 선택
   - 에러가 없는지 확인

3. **전체 솔루션 빌드**
   - Build → Build Solution (Ctrl+Shift+B)

### 4. Assets 폴더 복사

빌드 후, 실행 파일이 있는 디렉터리에 Assets 폴더가 있는지 확인하세요:

```
Bin/x64/Debug/Game/
├─ Game.exe
├─ Engine.dll
└─ Assets/
   ├─ Tutorial.txt
   └─ MainMap.txt
```

Assets 폴더가 없으면 수동으로 복사하세요.

## 빌드 시 발생할 수 있는 오류 및 해결방법

### 1. "Cannot open include file" 오류

**원인**: 프로젝트 Include 경로 설정 문제

**해결방법**:
- Engine 프로젝트: 프로젝트 속성 → C/C++ → General → Additional Include Directories에 `$(ProjectDir)` 추가 확인
- Game 프로젝트: 프로젝트 속성 → C/C++ → General → Additional Include Directories에 `$(ProjectDir)` 및 `..\Engine` 추가 확인

### 2. "Unresolved external symbol" 오류

**원인**: .cpp 파일이 프로젝트에 포함되지 않음

**해결방법**:
- 모든 .cpp 파일이 프로젝트에 추가되었는지 확인
- 솔루션 탐색기에서 해당 파일 우클릭 → Properties → General → Excluded From Build가 "No"인지 확인

### 3. "dynamic_cast" 관련 오류

**원인**: RTTI가 비활성화됨

**해결방법**:
- 프로젝트 속성 → C/C++ → Language → Enable Run-Time Type Information을 "Yes (/GR)"로 설정

### 4. "sprintf_s" 관련 오류

**원인**: 보안 경고

**해결방법**:
- 이미 코드에서 `sprintf_s`를 사용하고 있으므로 문제없음
- 만약 다른 버전의 Visual Studio를 사용한다면, 프로젝트 속성 → C/C++ → Preprocessor → Preprocessor Definitions에 `_CRT_SECURE_NO_WARNINGS` 추가

### 5. Assets 파일을 찾을 수 없음

**원인**: 실행 파일 경로에 Assets 폴더가 없음

**해결방법**:
```batch
# 프로젝트 루트에서 실행
xcopy Assets Bin\x64\Debug\Game\Assets\ /e /y
xcopy Assets Bin\x64\Release\Game\Assets\ /e /y
```

또는 Game 프로젝트의 Post-Build Event에 추가:
```batch
xcopy ..\Assets $(OutDir)Assets\ /e /y
```

## 실행 방법

### Visual Studio에서 실행
1. Game 프로젝트를 시작 프로젝트로 설정 (우클릭 → Set as StartUp Project)
2. F5 또는 Ctrl+F5로 실행

### 직접 실행
1. `Bin\x64\Debug\Game\` 또는 `Bin\x64\Release\Game\` 폴더로 이동
2. `Game.exe` 실행

## 게임 조작법

### 튜토리얼 맵
- 마우스로 Lord (L) 유닛 클릭
- 이동할 위치 클릭
- A 키로 Attack 선택
- 적 클릭하여 공격
- 적 처치 시 승리

### 공통 조작
- **마우스**: 유닛 선택, 이동, 공격 대상 선택
- **A 키**: Attack
- **W 키**: Wait
- **SPACE**: 플레이어 턴 종료
- **ESC**: 취소
- **임의 키**: 적 페이즈 종료
- **Q 키**: 게임 종료 (승리/패배 후)

## 디버깅 팁

### 콘솔 출력 확인
게임은 콘솔에 많은 디버그 정보를 출력합니다:
- 유닛 선택 정보
- 이동 정보
- 전투 로그 (명중, 데미지, 크리티컬 등)
- AI 행동 로그
- 레벨업 정보

### 일반적인 게임플레이 문제

**문제**: 유닛을 선택할 수 없음
- **해결**: 플레이어 페이즈인지 확인 (콘솔에 "Player Phase" 표시)
- **해결**: 이미 행동을 완료한 유닛은 선택 불가

**문제**: 이동할 수 없음
- **해결**: 이동 가능 범위 내의 빈 타일을 클릭했는지 확인
- **해결**: 다른 유닛이 있는 타일은 이동 불가

**문제**: 공격할 수 없음
- **해결**: 무기의 사거리 내에 적이 있는지 확인
- **해결**: 적 페이즈에는 플레이어가 조작 불가

## 성능 최적화 (선택 사항)

현재 구현은 학습 목적이므로 최적화보다는 가독성을 우선합니다. 하지만 필요하다면:

1. **메모리 누수 체크**: 이미 `_CrtSetDbgFlag`로 활성화됨
2. **프로파일링**: Visual Studio의 Performance Profiler 사용
3. **최적화 빌드**: Release 모드로 빌드 시 자동으로 최적화됨

## 추가 맵 생성

새 맵을 만들려면 `Assets/` 폴더에 `.txt` 파일을 생성하세요:

```
# MyMap.txt
WIDTH 15
HEIGHT 15
TILES
...............
... (지형 데이터)
...............
UNITS
PLAYER 2 12 Lord Sword
ENEMY 12 2 Fighter Axe
VICTORY DEFEAT_ALL_ENEMIES
DEFEAT ALL_PLAYER_UNITS_DEAD
```

`Main.cpp`에서 맵 파일 경로를 변경:
```cpp
MapLevel* level = new MapLevel("Assets/MyMap.txt");
```

## 트러블슈팅

문제가 발생하면:
1. 빌드 출력 창에서 에러 메시지 확인
2. Context.md에서 구현 상태 확인
3. 콘솔 출력에서 런타임 로그 확인
4. Visual Studio 디버거로 중단점 설정하여 디버깅
