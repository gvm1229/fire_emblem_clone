# FireEmblemClone - 코드 구조 및 로직 설명

## 프로젝트 개요
Fire Emblem: Shadow Dragon and the Blade of Light의 전투 시스템을 재현한 DOS 기반 C++ 턴제 전략 RPG 프로젝트입니다. 커스텀 게임 엔진을 기반으로 구축되었으며, 그리드 기반 전투, A* 경로 탐색, 지형 시스템, 유닛 관리 등의 핵심 기능을 포함합니다.

---

## Engine 폴더 - 게임 엔진 핵심 시스템

### `Engine/Engine/Engine.h/cpp`
게임 엔진의 핵심 클래스로, 전체 게임 루프와 시스템을 관리합니다. `Run()` 함수에서 무한 루프를 실행하며 `BeginPlay()`, `Tick()`, `Draw()`를 순차적으로 호출하여 게임의 초기화, 업데이트, 렌더링을 처리합니다. `Setting.txt` 파일로부터 화면 크기와 프레임레이트를 로드하며, Input과 Renderer 객체를 생성하고 관리합니다. 싱글톤 패턴으로 구현되어 `Engine::Get()`을 통해 전역 접근이 가능하며, Level 전환 기능(`SetNewLevel`)을 제공하여 게임 씬 관리를 수행합니다.

### `Engine/Level/Level.h/cpp`
게임의 각 씬(레벨)을 나타내는 기본 클래스입니다. 레벨 내의 모든 Actor 객체들을 `std::vector`로 관리하며, `BeginPlay()`, `Tick()`, `Draw()` 이벤트를 순회하며 각 Actor에 전달합니다. `AddNewActor()` 함수는 런타임 중 Actor 추가 요청을 `addRequestedActors` 큐에 저장하고, `ProcessAddAndDestroyActors()` 함수가 프레임 종료 시점에 안전하게 Actor를 추가하거나 삭제 요청된 Actor를 메모리에서 해제합니다. 이를 통해 게임 루프 중 발생할 수 있는 이터레이터 무효화 문제를 방지합니다.

### `Engine/Actor/Actor.h/cpp`
게임 내 모든 오브젝트의 기본 클래스로, 위치(Vector2), 색상(Color), 이미지(문자열) 등의 기본 속성을 가집니다. `BeginPlay()`, `Tick()`, `Draw()` 가상 함수를 제공하여 자식 클래스에서 오버라이드하도록 설계되었습니다. `Draw()` 함수는 기본적으로 Renderer에 자신의 이미지와 위치를 Submit하여 화면에 표시합니다. `Destroy()` 함수를 호출하면 `destroyRequested` 플래그가 설정되어 다음 프레임에 Level에 의해 제거됩니다. RTTI 시스템을 통해 런타임 타입 확인이 가능하며, 충돌 감지(`TestIntersect`) 기능도 제공합니다.

### `Engine/Render/Renderer.h/cpp`
콘솔 화면 렌더링을 담당하는 싱글톤 클래스입니다. `Submit()` 함수를 통해 각 Actor로부터 텍스트, 위치, 색상, 정렬 순서(sortingOrder)를 받아 `renderQueue`에 저장합니다. `Draw()` 함수에서는 renderQueue를 순회하며 UTF-8 문자열을 UTF-16으로 변환(`MultiByteToWideChar`)한 후 `CHAR_INFO` 배열에 기록하고, 더블 버퍼링을 통해 화면 깜빡임 없이 출력합니다. sortingOrder가 낮은 항목은 높은 항목에 의해 덮어써지며, 같은 위치에 여러 문자가 있을 경우 우선순위가 높은 것만 표시됩니다. `Clear()` 함수는 매 프레임 시작 시 renderQueue와 화면을 초기화합니다.

### `Engine/Render/ScreenBuffer.h/cpp`
Windows 콘솔 API를 사용한 더블 버퍼링 구현을 담당합니다. 두 개의 콘솔 화면 버퍼(`CreateConsoleScreenBuffer`)를 생성하여 백그라운드에서 그리기 작업을 수행한 후 `SetConsoleActiveScreenBuffer`로 순간 전환하여 화면 깜빡임을 방지합니다. `Draw()` 함수는 `WriteConsoleOutputW`를 사용하여 `CHAR_INFO` 배열을 버퍼에 한 번에 기록하며, `Clear()` 함수는 `FillConsoleOutputCharacterW`와 `FillConsoleOutputAttribute`로 화면을 공백 문자로 채웁니다. 생성자에서 `ENABLE_VIRTUAL_TERMINAL_PROCESSING` 플래그를 활성화하여 최신 Windows 콘솔의 UTF-8 지원을 개선합니다.

### `Engine/Core/Input.h/cpp`
키보드 및 마우스 입력을 처리하는 싱글톤 클래스입니다. Windows API의 `GetAsyncKeyState`를 사용하여 255개의 가상 키 코드를 매 프레임 체크하고, 이전 프레임 상태와 비교하여 `GetKeyDown()`(한 번만), `GetKey()`(지속), `GetKeyUp()`(떼는 순간) 이벤트를 구분합니다. 마우스 입력도 동일한 방식으로 처리하며(`GetMouseButtonDown/Up/Button`), `GetConsoleScreenBufferInfo`와 `GetCursorPos`, `ScreenToClient`를 사용하여 마우스 커서의 콘솔 좌표를 계산합니다. `ProcessInput()` 함수가 매 프레임 호출되어 입력 상태를 갱신하고, `SavePreviousInputStates()` 함수가 다음 프레임을 위해 현재 상태를 저장합니다.

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
프로그램의 진입점으로, `main()` 함수에서 Engine 인스턴스를 생성하고 게임 루프를 시작합니다. 콘솔 코드 페이지를 UTF-8로 설정(`SetConsoleOutputCP`, `SetConsoleCP`)하고 가상 터미널 시퀀스를 활성화하여 유니코드 박스 문자가 올바르게 표시되도록 합니다. `BattleLevel` 인스턴스를 생성하여 엔진에 등록하고, 맵 파일(`Assets/BattleMap.txt`)을 로드합니다. 초기 유닛 3개(Lord, Cavalier, Archer)를 플레이어 진영으로, 2개(Soldier, Archer)를 적 진영으로 생성하여 맵의 특정 위치에 배치한 후, 엔진의 `Run()` 함수를 호출하여 게임을 시작합니다.

### `Game/Level/BattleLevel.h/cpp`
Fire Emblem 전투 맵을 구현한 메인 게임 레벨 클래스입니다. `Grid` 객체를 통해 지형 정보를 관리하고, `playerUnits`와 `enemyUnits` 벡터로 아군과 적군 유닛을 분리 관리합니다. `LoadMap()` 함수는 텍스트 파일에서 맵 크기와 지형 데이터를 읽어 Grid를 초기화합니다. `HandleInput()` 함수는 1~9, 0 숫자 키로 플레이어 유닛을 선택하고, 마우스 클릭으로 이동 목적지를 지정하며, ESC 키로 선택 해제를 처리합니다. 유닛 선택 시 `MovementCalculator`를 사용하여 이동 가능 범위를 계산하고, 이동 명령 시 `NavigationSystem`으로 최적 경로를 생성하여 유닛에 전달합니다. `DrawGrid()`, `DrawMovementRange()`, `DrawStatsPanel()` 함수로 맵, 이동 가능 타일 하이라이트, 유닛 스탯 패널을 각각 렌더링하며, `uiBuffers`라는 클래스 멤버 배열을 사용하여 동적 텍스트의 메모리 수명을 보장합니다. 턴 관리 시스템(`isPlayerTurn`, `turnCount`)을 통해 플레이어와 적의 턴을 구분합니다.

### `Game/Map/Grid.h/cpp`
2D 그리드 맵을 관리하는 클래스로, `std::vector<std::vector<Tile*>>`로 타일 배열을 저장합니다. 생성자에서 주어진 너비와 높이만큼 Tile 객체를 동적 할당하여 초기화하며, 소멸자에서 모든 Tile을 안전하게 삭제합니다. `GetTile()` 함수는 x, y 좌표 또는 Vector2로 특정 타일을 반환하고, `SetTile()` 함수는 특정 위치의 지형 타입을 변경합니다. `IsValidPosition()` 함수는 주어진 좌표가 그리드 범위 내에 있는지 확인하며, `IsWalkable()` 함수는 해당 타일이 이동 가능하고 유닛이 없는지 검사합니다. `GenerateNavigationMap()` 함수는 NavigationSystem을 위해 bool 2D 배열을 생성하여, true는 이동 가능, false는 장애물을 나타냅니다.

### `Game/Map/Tile.h/cpp`
그리드의 각 타일을 나타내는 클래스로, 지형 타입(`TerrainType`)에 따라 이동 비용, 회피/방어 보너스, 체력 회복 여부 등의 속성을 설정합니다. 생성자에서 `switch` 문으로 지형 타입별 특성을 초기화하며, 평지(`.`)는 이동 비용 1, 숲(`♣`)은 이동 비용 2와 회피 +20, 산(`▲`)은 이동 비용 3과 회피 +30, 성(`♦`)은 체력 회복 기능, 물(`≈`)과 벽(`█`)은 통과 불가로 설정됩니다. 각 지형은 고유한 UTF-8 문자(`displayStr`)와 색상(`displayColor`)을 가지며, `GetDisplayString()`과 `GetDisplayColor()` 함수를 통해 렌더링 시 사용됩니다. `HasUnit()` 플래그로 해당 타일에 유닛이 있는지 추적하여 이동 가능 여부 판단에 활용됩니다.

### `Game/Unit/Unit.h/cpp`
Fire Emblem의 유닛(캐릭터)을 나타내는 Actor 자식 클래스입니다. `UnitStats` 구조체로 HP, STR, MAG, SKL, SPD, LCK, DEF, RES, MOV 등의 스탯을 관리하고, `Faction`(Player/Enemy/Ally)으로 진영을 구분하며, `UnitState`(Idle/Selected/Moving/Acting/Done)로 현재 상태를 추적합니다. `UnitClass`(Lord, Cavalier, Knight, Archer 등)에 따라 표시 문자(`displayStr`)가 결정되며, `SetUnitClass()` 함수로 병과 변경 시 문자도 자동으로 갱신됩니다. `SetPath()` 함수는 이동 경로(`std::deque<Vector2>`)를 받아 저장하고, `UpdateMovement()` 함수는 매 프레임 호출되어 타이머 기반으로 경로를 따라 이동하며 목적지 도착 시 다음 웨이포인트로 전환합니다. `GetDisplayColor()` 함수는 진영과 상태에 따라 색상을 반환하며, Done 상태는 회색으로 표시됩니다. `unitIndex`는 플레이어 유닛의 숫자 키(0~9) 매핑에 사용됩니다.

### `Game/Unit/UnitClass.h`
유닛의 병과(클래스)를 정의한 열거형과 유틸리티 함수를 제공합니다. `UnitClass` enum은 Lord(주인공), Cavalier(기마병), Knight(중보병), Archer(궁병), Mage(마법사), Pegasus Knight(천마병), Soldier(병사) 등을 포함합니다. `GetUnitClassString()` 함수는 각 병과를 나타내는 단일 문자 UTF-8 문자열을 반환하며, 예를 들어 Lord는 "L", Cavalier는 "C", Knight는 "K"를 반환합니다. 이 문자들은 콘솔 화면에서 유닛을 시각적으로 구분하는 데 사용되며, 향후 멀티라인 ASCII 아트로 확장할 수 있도록 설계되었습니다.

### `Game/Unit/UnitStats.h`
유닛의 전투 스탯을 담는 구조체입니다. `maxHP`와 `currentHP`로 체력을 관리하며, `strength`(물리 공격력), `magic`(마법 공격력), `skill`(명중률/필살률), `speed`(공격 순서/회피), `luck`(필살 회피), `defense`(물리 방어), `resistance`(마법 방어), `movement`(이동력) 등의 스탯을 int 타입으로 저장합니다. 이 값들은 전투 시스템에서 데미지 계산, 명중 판정, 회피 판정 등에 사용되며, 지형 보너스와 결합하여 최종 전투 결과를 결정합니다. 현재는 기본값을 생성자에서 초기화하며, 향후 레벨업이나 아이템 시스템으로 확장 가능합니다.

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
게임 엔진의 초기 설정을 담은 텍스트 파일입니다. `width=70`, `height=30`, `framerate=60.0` 등의 키-값 쌍으로 콘솔 화면 크기와 목표 프레임레이트를 지정합니다. Engine 클래스의 `LoadSetting()` 함수가 시작 시 이 파일을 읽어 파싱하며, 각 설정 값을 `EngineSetting` 구조체에 저장합니다. 화면 너비는 맵과 유닛 정보 패널을 함께 표시할 수 있도록 충분히 설정되어야 하며, 높이는 맵 전체와 상단 UI, 하단 입력 모니터를 표시할 수 있어야 합니다. 프레임레이트는 게임 루프의 `Sleep()` 호출로 제어되어 CPU 사용률을 조절합니다.

---

## 핵심 게임 플레이 흐름

1. **초기화**: `Main.cpp`에서 Engine 생성 → BattleLevel 생성 및 맵 로드 → 유닛 배치
2. **게임 루프**: Engine이 매 프레임 `BeginPlay()` → `Tick()` → `Draw()` 호출
3. **입력 처리**: `Input`이 키보드/마우스 상태 갱신 → BattleLevel의 `HandleInput()`이 유닛 선택/이동 명령 처리
4. **유닛 선택**: 숫자 키 입력 → `SelectUnitByIndex()` → MovementCalculator로 이동 범위 계산 → `reachableTiles` 갱신
5. **유닛 이동**: 마우스 클릭 → `OnMouseClick()` → NavigationSystem으로 경로 생성 → Unit에 경로 설정 → `UpdateMovement()`로 애니메이션 처리
6. **렌더링**: `DrawGrid()`로 지형 표시 → `DrawMovementRange()`로 이동 가능 타일 하이라이트 → 각 Unit의 `Draw()`로 유닛 표시 → `DrawStatsPanel()`로 스탯 UI 출력 → Renderer가 renderQueue 처리 → ScreenBuffer로 더블 버퍼링 출력

---

## 기술적 특징

- **UTF-8 인코딩**: 모든 문자열을 UTF-8로 관리하고, Renderer에서 UTF-16으로 변환하여 Windows 콘솔 API(`WriteConsoleOutputW`)에 전달
- **더블 버퍼링**: 두 개의 콘솔 버퍼를 교대로 사용하여 화면 깜빡임 방지
- **싱글톤 패턴**: Engine, Renderer, Input 등 핵심 시스템은 전역 접근 가능한 싱글톤으로 구현
- **커스텀 RTTI**: 런타임 타입 확인과 안전한 캐스팅을 위한 자체 RTTI 시스템 구축
- **A* 경로 탐색**: 우선순위 큐 기반 A* 알고리즘으로 최적 경로 계산
- **Dijkstra 이동 범위**: 지형 비용을 고려한 Dijkstra 알고리즘으로 이동 가능 영역 계산
- **지형 시스템**: 각 타일이 이동 비용, 회피/방어 보너스, 체력 회복 등의 속성을 가짐
- **턴제 시스템**: 플레이어/적 턴 구분 및 유닛 상태 관리(Idle/Selected/Moving/Done)
