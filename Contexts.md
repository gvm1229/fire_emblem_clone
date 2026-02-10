# FireEmblemClone - 코드 구조 및 로직 설명

## 프로젝트 개요
Fire Emblem: Shadow Dragon and the Blade of Light의 전투 시스템을 재현한 DOS 기반 C++ 턴제 전략 RPG 프로젝트입니다. 커스텀 게임 엔진을 기반으로 구축되었으며, 그리드 기반 전투, A* 경로 탐색, 지형 시스템, 유닛 관리 등의 핵심 기능을 포함합니다. 프로젝트 네임스페이스는 `FEClone`이며, 엔진 DLL API는 `FECLONE_API` 매크로로 정의됩니다. 모든 타일과 유닛은 2x2 멀티라인 ASCII 아트로 렌더링되어 시각적으로 구분하기 쉽습니다.

---

## Engine 폴더 - 게임 엔진 핵심 시스템

### `Engine/Engine/Engine.h/cpp`
게임 엔진의 핵심 클래스로, 전체 게임 루프와 시스템을 관리합니다. `Run()` 함수에서 무한 루프를 실행하며 `BeginPlay()`, `Tick()`, `Draw()`를 순차적으로 호출하여 게임의 초기화, 업데이트, 렌더링을 처리합니다. `Setting.txt` 파일로부터 화면 크기와 프레임레이트를 로드하며, Input과 Renderer 객체를 생성하고 관리합니다. 싱글톤 패턴으로 구현되어 `Engine::Get()`을 통해 전역 접근이 가능하며, Level 전환 기능(`SetNewLevel`)을 제공하여 게임 씬 관리를 수행합니다.

### `Engine/Level/Level.h/cpp`
게임의 각 씬(레벨)을 나타내는 기본 클래스입니다. 레벨 내의 모든 Actor 객체들을 `std::vector`로 관리하며, `BeginPlay()`, `Tick()`, `Draw()` 이벤트를 순회하며 각 Actor에 전달합니다. `AddNewActor()` 함수는 런타임 중 Actor 추가 요청을 `addRequestedActors` 큐에 저장하고, `ProcessAddAndDestroyActors()` 함수가 프레임 종료 시점에 안전하게 Actor를 추가하거나 삭제 요청된 Actor를 메모리에서 해제합니다. 이를 통해 게임 루프 중 발생할 수 있는 이터레이터 무효화 문제를 방지합니다.

### `Engine/Actor/Actor.h/cpp`
게임 내 모든 오브젝트의 기본 클래스로, 위치(Vector2), 색상(Color), 이미지(문자열) 등의 기본 속성을 가집니다. `BeginPlay()`, `Tick()`, `Draw()` 가상 함수를 제공하여 자식 클래스에서 오버라이드하도록 설계되었습니다. `Draw()` 함수는 기본적으로 Renderer에 자신의 이미지와 위치를 Submit하여 화면에 표시합니다. `Destroy()` 함수를 호출하면 `destroyRequested` 플래그가 설정되어 다음 프레임에 Level에 의해 제거됩니다. RTTI 시스템을 통해 런타임 타입 확인이 가능하며, 충돌 감지(`TestIntersect`) 기능도 제공합니다.

### `Engine/Render/Renderer.h/cpp`
콘솔 화면 렌더링을 담당하는 싱글톤 클래스입니다. `Submit()` 함수를 통해 각 Actor로부터 텍스트, 위치, 색상, 정렬 순서(sortingOrder)를 받아 `renderQueue`에 저장합니다. `Draw()` 함수에서는 renderQueue를 순회하며 UTF-8 문자열을 UTF-16으로 변환(`MultiByteToWideChar`)한 후 `CHAR_INFO` 배열에 기록하고, 더블 버퍼링을 통해 화면 깜빡임 없이 출력합니다. sortingOrder가 낮은 항목은 높은 항목에 의해 덮어써지며, 같은 위치에 여러 문자가 있을 경우 우선순위가 높은 것만 표시됩니다. 렌더링 우선순위는 7(지형), 8(아이템), 9(하이라이트), 10(유닛) 순으로 설정되어 유닛이 항상 최상위에 표시됩니다. `Clear()` 함수는 매 프레임 시작 시 renderQueue와 화면을 초기화합니다.

### `Engine/Render/ScreenBuffer.h/cpp`
Windows 콘솔 API를 사용한 더블 버퍼링 구현을 담당합니다. 두 개의 콘솔 화면 버퍼(`CreateConsoleScreenBuffer`)를 생성하여 백그라운드에서 그리기 작업을 수행한 후 `SetConsoleActiveScreenBuffer`로 순간 전환하여 화면 깜빡임을 방지합니다. `Draw()` 함수는 `WriteConsoleOutputW`를 사용하여 `CHAR_INFO` 배열을 버퍼에 한 번에 기록하며, `Clear()` 함수는 `FillConsoleOutputCharacterW`와 `FillConsoleOutputAttribute`로 화면을 공백 문자로 채웁니다. 생성자에서 `ENABLE_VIRTUAL_TERMINAL_PROCESSING` 플래그를 활성화하여 최신 Windows 콘솔의 UTF-8 지원을 개선합니다.

### `Engine/Core/Input.h/cpp`
키보드 및 마우스 입력을 처리하는 싱글톤 클래스입니다. Windows 콘솔 API의 `ReadConsoleInput`과 `PeekConsoleInput`을 사용하여 키보드와 마우스 이벤트를 받아 처리합니다. `KEY_EVENT`와 `MOUSE_EVENT`를 구분하여 각각 `keyStates` 배열과 `mousePosition`을 갱신하며, 이전 프레임 상태와 비교하여 `GetKeyDown()`(한 번만), `GetKey()`(지속), `GetKeyUp()`(떼는 순간) 이벤트를 구분합니다. 마우스 입력은 `GetMouseButtonDown/Up/Button` 함수로 처리되며, `dwMousePosition`에서 X, Y 좌표를 읽어 `mousePosition`에 저장합니다. `ENABLE_MOUSE_INPUT` 플래그를 설정하고 `ENABLE_QUICK_EDIT_MODE`를 비활성화하여 콘솔에서 마우스 입력을 정상적으로 받을 수 있도록 합니다. `SavePreviousInputStates()` 함수가 다음 프레임을 위해 현재 상태를 저장합니다.

### `Engine/Algorithm/NavigationSystem.h/cpp`
A* 경로 탐색 알고리즘을 구현한 클래스로, AlgorithmPractice 프로젝트의 로직을 기반으로 작성되었습니다. `FindPath()` 함수는 시작 위치와 목적지, 그리고 2D bool 배열(이동 가능 여부 맵)을 입력받아 최적 경로를 `std::deque<Vector2>`로 반환합니다. `std::priority_queue`를 사용한 열린 리스트(openList)에서 fCost(gCost + hCost)가 가장 낮은 노드를 선택하며, 4방향 이동을 고려하여 인접 노드를 탐색합니다. 휴리스틱 함수(`CalculateHeuristic`)는 유클리드 거리를 사용하며, 경로를 찾지 못한 경우 `SetAlternativeDestination`을 호출하여 가장 가까운 도달 가능 지점을 대체 목적지로 설정합니다. `ConstructPath`는 닫힌 리스트를 역추적하여 최종 경로를 생성합니다.

### `Engine/Math/Vector2.h/cpp`
2D 좌표를 표현하는 구조체로, `int x`와 `int y` 멤버 변수를 가집니다. 연산자 오버로딩을 통해 벡터 덧셈(`+`), 뺄셈(`-`), 비교(`==`, `!=`) 등을 지원하며, `Distance()` 정적 함수로 두 벡터 간 유클리드 거리를 계산할 수 있습니다. `Zero`, `One`, `Up`, `Down`, `Left`, `Right` 등의 정적 상수를 제공하여 편리하게 사용 가능합니다. `std::hash<FEClone::Vector2>` 특수화를 통해 `std::unordered_map`의 키로 사용할 수 있도록 구현되었으며, 해시 함수는 x와 y의 해시 값을 XOR 연산으로 결합합니다.

### `Engine/Math/Color.h`
콘솔 텍스트 색상을 정의한 열거형입니다. Windows 콘솔 API의 색상 비트 플래그를 사용하며, `Red = 0x0004`, `Green = 0x0002`, `Blue = 0x0001`, `Intensity = 0x0008` 등의 기본 색상과 이들의 조합으로 `Yellow = Red | Green`, `Cyan = Blue | Green`, `Orange = Red | Blue`, `White = Red | Green | Blue | Intensity` 등을 정의합니다. 색상은 `CHAR_INFO` 구조체의 Attributes 필드에 적용되어 콘솔 출력 시 텍스트 색상을 결정합니다.

### `Engine/Common/RTTI.h`
런타임 타입 정보(Run-Time Type Information)를 위한 커스텀 시스템입니다. `RTTI_DECLARATIONS` 매크로는 클래스 헤더에서 `TypeIdClass()`, `TypeIdInstance()`, `IsA()`, `As()` 등의 함수를 선언하고, `RTTI_DEFINITIONS` 매크로는 cpp 파일에서 이를 정의합니다(현재는 인라인 구현으로 인해 비어 있음). 이를 통해 C++의 기본 RTTI(`dynamic_cast`, `typeid`)를 대체하여 가볍고 제어 가능한 타입 시스템을 구축하며, 게임 엔진에서 Actor 타입을 런타임에 확인하고 안전하게 캐스팅하는 데 사용됩니다.

---

## Game 폴더 - Fire Emblem 게임 로직

### `Game/Main.cpp`
프로그램의 진입점으로, `main()` 함수에서 Engine 인스턴스를 생성하고 게임 루프를 시작합니다. 콘솔 코드 페이지를 UTF-8로 설정하고 가상 터미널 시퀀스를 활성화하여 유니코드 박스 문자가 올바르게 표시되도록 합니다. `BattleLevel` 인스턴스를 생성하여 엔진에 등록하고, 맵 파일(`Assets/BattleMap.txt`)을 로드합니다. **근접 유닛만** 사용하며(Lord, Cavalier, Soldier), 궁수/마법사는 데이터만 있고 맵에는 배치하지 않습니다. 플레이어 유닛 3개와 적 유닛 3개(Soldier)를 각각 인덱스 0, 1, 2로 부여하여 맵에 배치한 후, 엔진의 `Run()` 함수를 호출하여 게임을 시작합니다.

### `Game/Level/BattleLevel.h/cpp`
Fire Emblem 전투 맵을 구현한 메인 게임 레벨 클래스입니다. `Grid` 객체로 지형을 관리하고, `playerUnits`·`enemyUnits`로 아군/적군을 분리 관리합니다. `LoadMap()`으로 텍스트 맵을 로드하고, `HandleInput()`은 **1~9, 0**으로 플레이어 유닛 선택, **Z,X,C,V,B,N,M,쉼표,마침표,슬래시**로 적 유닛 선택(스탯 보기용), 마우스 클릭으로 이동·공격, ESC로 선택 해제, SPACE로 턴 종료를 처리합니다. **전투 시스템**: 인접 타일에서만 공격 가능하며, 적을 클릭하면 인접 시 즉시 `PerformCombat()`, 비인접이면 이동 가능한 인접 타일로 경로 이동 후 자동 공격합니다. 데미지는 (공격자 STR - 방어자 DEF), 공격자 SPD가 더 크면 2회 타격입니다. **적 AI**(`EnemyAI`)는 인접 시 플레이어 유닛을 공격하고, 그렇지 않으면 이동 범위 내 플레이어 인접 타일로 이동합니다. **게임 종료**: `CheckEndConditions()`로 아군 전멸 또는 로드 전투 불능 시 적군 승리, 적군 전멸 시 아군 승리로 판정합니다. `TriggerGameOver()`에서 모든 유닛 제거·파괴, 벽을 제외한 맵을 평지로 초기화하고, `gameOver` 시 Tick에서는 로직을 건너뛰고 Draw에서 맵 중앙에 승리 진영 문구(PLAYER VICTORY / ENEMY VICTORY)를 진영 색상으로 표시합니다. **이벤트 로그**는 세그먼트별 색상(유닛명·지형명·데미지·턴 종료 등)을 지원하며, `DrawEndConditions()`로 맵과 툴팁 사이에 아군/적군 패배 조건을 진영 색상으로 표시합니다. `DrawStatsPanel()`은 선택된 플레이어 또는 적 유닛의 스탯·지형 정보를 표시하고, `DrawKeyboardTooltip()`으로 조작법과 색상 설명을 띄웁니다.

### `Game/Map/Grid.h/cpp`
2D 그리드 맵을 관리하는 클래스로, `std::vector<std::vector<Tile*>>`로 타일 배열을 저장합니다. 생성자에서 주어진 너비와 높이만큼 Tile 객체를 동적 할당하여 초기화하며, 소멸자에서 모든 Tile을 안전하게 삭제합니다. `GetTile()` 함수는 x, y 좌표 또는 Vector2로 특정 타일을 반환하고, `SetTile()` 함수는 특정 위치의 지형 타입을 변경합니다. `IsValidPosition()` 함수는 주어진 좌표가 그리드 범위 내에 있는지 확인하며, `IsWalkable()` 함수는 해당 타일이 이동 가능하고 유닛이 없는지 검사합니다. `GenerateNavigationMap()` 함수는 NavigationSystem을 위해 bool 2D 배열을 생성하여, true는 이동 가능, false는 장애물을 나타냅니다.

### `Game/Map/Tile.h/cpp`
그리드의 각 타일을 나타내는 클래스로, 지형 타입(`TerrainType`)에 따라 이동 비용, 회피/방어 보너스, 체력 회복 여부 등의 속성을 설정합니다. 생성자에서 `switch` 문으로 지형 타입별 특성을 초기화하며, 평지는 이동 비용 1, 숲은 이동 비용 2와 회피 +20, 산은 이동 비용 3과 회피 +30, 성은 체력 회복 기능, 물과 벽은 통과 불가로 설정됩니다. 각 지형은 2x2 멀티라인 ASCII 아트로 표현되며, `displayStr[4][8]` 배열에 4개의 셀(topLeft, topRight, bottomLeft, bottomRight)을 저장합니다. 평지는 점 4개, 숲은 스페이드(♠) 4개, 산은 삼각형(/\)과 블록(▓), 성은 박스 문자(╔╗╚╝), 마을은 지붕(/\)과 벽(▄), 물은 물결(≈) 4개, 벽은 블록(█) 4개로 표현됩니다. `GetDisplayStrings()` 함수로 4개의 문자열 포인터를 반환하며, `GetDisplayColor()`로 지형별 색상을 제공합니다. `HasUnit()` 플래그로 해당 타일에 유닛이 있는지 추적하여 이동 가능 여부 판단에 활용됩니다.

### `Game/Unit/Unit.h/cpp`
Fire Emblem의 유닛(캐릭터)을 나타내는 Actor 자식 클래스입니다. `UnitStats` 구조체로 HP, STR, MAG, SKL, SPD, LCK, DEF, RES, MOV 등의 스탯을 관리하고, `Faction`(Player/Enemy/Ally/Neutral)으로 진영을 구분하며, `UnitState`(Idle/Selected/Moving/Acting/Done)로 현재 상태를 추적합니다. `UnitClass`(Lord, Cavalier, Knight, Archer 등)에 따라 표시 문자(`displayStr`)가 결정되며, Lord 유닛은 생성자에서 모든 스탯이 50% 강화되고(0인 스탯은 3으로 설정), `GetDisplayColor()`에서 항상 녹색(Color::Green)을 반환하여 특별한 주인공임을 시각적으로 표현합니다. `SetPath()` 함수는 이동 경로(`std::deque<Vector2>`)를 받아 저장하고 상태를 Moving으로 변경하며, `UpdateMovement()` 함수는 매 프레임 호출되어 타이머 기반으로 경로를 따라 `gridPosition`을 갱신하고 이동 완료 시 상태를 Done으로 변경합니다. `Draw()` 함수는 `gridPosition`을 2x2 화면 좌표로 변환하여 같은 문자를 4번 렌더링하며, Actor의 `position` 변수는 사용하지 않습니다. 선택 상태는 노란색, Done 상태는 흰색, Player는 파란색, Enemy는 빨간색으로 표시됩니다. `unitIndex`는 플레이어 유닛의 숫자 키(1~9, 0) 매핑에 사용됩니다.

### `Game/Unit/UnitClass.h`
유닛의 병과(클래스)를 정의한 열거형과 유틸리티 함수를 제공합니다. `UnitClass` enum은 Lord(주인공), Cavalier(기마병), Knight(중보병), Archer(궁병), Mage(마법사), Fighter(전사), Soldier(병사), Mercenary(용병), Thief(도적), Cleric(성직자) 등을 포함합니다. `GetUnitClassString()` 함수는 각 병과를 나타내는 단일 문자 UTF-8 문자열을 반환하며(Lord는 "L", Cavalier는 "C" 등), 2x2 멀티라인 렌더링 시 모든 셀에 동일한 문자가 표시됩니다. `GetFullUnitClassString()` 함수는 스탯 패널 표시용으로 전체 클래스 이름("Lord", "Cavalier", "Archer" 등)을 반환하여 플레이어가 병과를 명확하게 인식할 수 있도록 합니다.

### `Game/Unit/UnitStats.h`
유닛의 전투 스탯을 담는 구조체입니다. `maxHP`·`currentHP`로 체력을 관리하며, `strength`(물리 공격력), `defense`(물리 방어), `speed`(2회 타격 여부) 등이 전투에서 사용됩니다. `TakeDamage()`로 피격 시 currentHP가 감소하며, 0 이하가 되면 `IsAlive()`가 false가 되어 전투 불능·게임 종료 조건 판정에 사용됩니다.

### `Game/AI/EnemyAI.h/cpp`
적 유닛의 행동을 처리하는 AI 클래스입니다. `RunAI()`에서 플레이어 유닛에 **인접해 있으면** `performCombat` 콜백으로 한 유닛을 공격한 뒤 턴 종료하고, 그렇지 않으면 `MovementCalculator`·`NavigationSystem`으로 플레이어 유닛 인접 타일 중 이동 가능한 타일을 목표로 경로를 찾아 이동합니다. 이동 시 `onEnemyMoved(enemy, pathCount, terrainType)` 콜백으로 이벤트 로그용 정보를 전달하며, BattleLevel에서 진영별 색상으로 "Enemy Unit #N moved X tiles to …" 형식의 로그를 남깁니다.

### `Game/System/MovementCalculator.h/cpp`
유닛의 이동 가능 범위를 계산하는 클래스로, Dijkstra 알고리즘을 사용합니다. `CalculateReachableTiles()` 함수는 시작 위치, 최대 이동력, Grid를 입력받아 도달 가능한 모든 타일을 `std::vector<Vector2>`로 반환합니다. `std::priority_queue`를 사용하여 비용이 낮은 타일부터 탐색하며, 각 타일의 이동 비용(`Tile::GetMovementCost()`)을 누적하여 최대 이동력을 초과하지 않는 범위 내에서 모든 도달 가능 타일을 찾습니다. 4방향 이동만 지원하며, 유닛이 있거나 통과 불가능한 타일은 제외합니다. `costMap`에 각 위치까지의 실제 이동 비용을 저장하여 `GetMovementCostTo()` 함수로 조회할 수 있으며, 이는 유닛이 실제로 이동할 수 있는 거리와 경로를 판단하는 데 활용됩니다.

### `Game/Util/Timer.h/cpp`
시간 기반 기능을 제공하는 유틸리티 클래스입니다. Windows API의 `QueryPerformanceCounter`와 `QueryPerformanceFrequency`를 사용하여 고해상도 타이머를 구현하며, `GetDeltaTime()` 함수는 이전 프레임과 현재 프레임 사이의 경과 시간(초 단위)을 반환합니다. `Sleep()` 함수는 지정된 밀리초만큼 대기하며, 엔진의 프레임레이트 제한에 사용됩니다. 게임 루프에서 매 프레임 호출되어 일정한 프레임 속도를 유지하고, deltaTime을 통해 프레임 독립적인 이동 및 애니메이션을 구현할 수 있도록 지원합니다.

---

## Assets 폴더 - 게임 리소스

### `Assets/BattleMap.txt`
전투 맵의 지형 레이아웃을 정의한 텍스트 파일입니다. 첫 줄에 맵의 너비와 높이가 공백으로 구분되어 기록되며, 이후 각 줄은 한 행의 타일들을 나타냅니다. 각 문자는 지형 타입을 의미하며, `.`는 평지, `T`는 숲, `M`은 산, `#`은 벽, `~`는 물 등으로 매핑됩니다. BattleLevel의 `LoadMap()` 함수가 이 파일을 읽어 Grid를 초기화하며, 맵 크기는 `Config/Setting.txt`의 화면 크기와 조화를 이루도록 설계되어야 합니다. 간단한 텍스트 형식으로 맵을 쉽게 편집하고 다양한 전투 시나리오를 테스트할 수 있습니다.

---

## Config 폴더 - 설정 파일

### `Config/Setting.txt`
게임 엔진의 초기 설정을 담은 텍스트 파일입니다. `width=70`, `height=50`, `framerate=60.0` 등의 키-값 쌍으로 콘솔 화면 크기와 목표 프레임레이트를 지정합니다. Engine 클래스의 `LoadSetting()` 함수가 시작 시 이 파일을 읽어 파싱하며, 각 설정 값을 `EngineSetting` 구조체에 저장합니다. 2x2 멀티라인 렌더링으로 인해 15x15 그리드가 30x30 화면 공간을 차지하므로, 화면 높이는 50으로 설정되어 맵(32줄), 구분선(1줄), 툴팁(5줄), 여유 공간을 모두 표시할 수 있습니다. 화면 너비는 맵(30칸) + 여백(6칸) + 유닛 정보 패널(20칸) + 여유를 고려하여 70으로 설정됩니다. 프레임레이트는 게임 루프에서 deltaTime 계산으로 제어되어 60 FPS를 유지하며 CPU 사용률을 조절합니다.

---

## 핵심 게임 플레이 흐름

1. **초기화**: `Main.cpp`에서 Engine 생성 → BattleLevel 생성 및 맵 로드 → 근접 유닛만 배치(Lord, Cavalier, Soldier / 적 Soldier×3, 인덱스 0~2)
2. **게임 루프**: Engine이 매 프레임 `BeginPlay()` → `Tick()` → `Draw()` 호출. `gameOver` 시 Tick에서는 입력·AI·턴 로직 생략.
3. **입력 처리**: `HandleInput()` — 1~9,0으로 플레이어 유닛 선택, Z,X,C,…,./ 로 적 유닛 선택(스탯 보기), ESC로 선택 해제, SPACE로 턴 종료.
4. **유닛 선택·이동·공격**: 플레이어 유닛 선택 시 이동 범위 계산. 마우스 클릭 시 빈 타일이면 이동, **적 타일이면** 인접 시 즉시 `PerformCombat()`, 비인접이면 이동 가능한 적 인접 타일로 경로 이동 후 `ProcessPendingAttackAfterMove()`에서 공격.
5. **전투**: `PerformCombat(attacker, defender)` — 데미지 = max(0, STR−DEF), SPD 우위 시 2회 타격. 피격 유닛 `TakeDamage()`, 전투 불능 시 타일·리스트에서 제거 후 `Destroy()`, `CheckEndConditions()` 호출.
6. **게임 종료**: 아군 전멸 또는 로드 전투 불능 → 적군 승리. 적군 전멸 → 아군 승리. `TriggerGameOver()`에서 유닛 전원 제거·파괴, 맵(벽 제외) 평지로 초기화. 이후 Draw에서 맵 중앙에 "PLAYER VICTORY" / "ENEMY VICTORY" 진영 색상 표시.
7. **렌더링**: `DrawGrid()` → 이동 범위 하이라이트 → `Level::Draw()`(유닛, gameOver 시 생략) → 게임 종료 시 승리 문구만, 아니면 스탯·이벤트 로그·종료 조건 문구·툴팁·턴 정보.
8. **턴 전환**: SPACE 또는 아군 전원 행동 완료 시 플레이어 턴 종료 → 적 턴. 적 전원 행동 완료 시 적 턴 종료 → 플레이어 턴.

---

## 기술적 특징

- **UTF-8 인코딩**: 모든 문자열을 UTF-8로 관리하고, Renderer에서 UTF-16으로 변환하여 Windows 콘솔 API(`WriteConsoleOutputW`)에 전달
- **더블 버퍼링**: 두 개의 콘솔 버퍼를 교대로 사용하여 화면 깜빡임 방지
- **멀티라인 ASCII 아트**: 모든 타일과 유닛을 2x2 크기로 렌더링하여 시각적 표현력 향상
- **렌더링 우선순위**: sortingOrder 시스템으로 지형(7) < 아이템(8) < 하이라이트(9) < 유닛(10) 계층 구조 구현
- **싱글톤 패턴**: Engine, Renderer, Input 등 핵심 시스템은 전역 접근 가능한 싱글톤으로 구현
- **커스텀 RTTI**: 런타임 타입 확인과 안전한 캐스팅을 위한 자체 RTTI 시스템 구축
- **A* 경로 탐색**: 우선순위 큐 기반 A* 알고리즘으로 최적 경로 계산
- **Dijkstra 이동 범위**: 지형 비용을 고려한 Dijkstra 알고리즘으로 이동 가능 영역 계산
- **지형 시스템**: 각 타일이 이동 비용, 회피/방어 보너스, 체력 회복 등의 속성을 가지며 고유한 2x2 ASCII 아트로 표현
- **턴제 시스템**: 플레이어/적 턴 구분 및 유닛 상태 관리(Idle/Selected/Moving/Done), SPACE 키로 턴 전환
- **전투·종료 조건**: 인접 공격만 지원, STR/DEF/SPD 기반 데미지·2회 타격. 아군 전멸 또는 로드 전투 불능 시 적군 승리, 적 전멸 시 아군 승리. 게임 종료 시 맵 정리 후 승리 문구만 표시하고 루프는 유지
- **이벤트 로그 색상**: 로그 항목을 (문자열, 색상) 세그먼트로 저장하여 유닛명·지형명·데미지·턴 종료 등을 진영별·항목별로 색상 구분
- **좌표 변환 시스템**: 그리드 좌표(논리적) ↔ 화면 좌표(물리적) 변환, 2x2 렌더링 고려
- **UI 버퍼 관리**: `uiBuffers[16][64]` 등으로 동적 텍스트의 메모리 수명 보장
- **마우스 입력**: 콘솔 마우스 이벤트로 유닛 이동·적 클릭 공격, 화면 좌표를 그리드 좌표로 자동 변환
