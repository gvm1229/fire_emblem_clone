# Fire Emblem Clone - 게임 실행 흐름 상세

## 개요
이 문서는 프로그램 시작부터 게임 루프, 입력 처리, 렌더링까지의 전체 실행 흐름을 단계별로 상세히 설명합니다. 모든 타일과 유닛은 2x2 멀티라인 ASCII 아트로 렌더링되며, 렌더링 우선순위 시스템(7: 지형, 8: 아이템, 9: 하이라이트, 10: 유닛)을 통해 올바른 레이어 순서를 보장합니다.

---

## 1. 프로그램 시작 (Main.cpp)

### main() 함수 실행 순서

```cpp
int main()
{
    // 1. 메모리 누수 체크 플래그 설정 (디버그 모드)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    
    // 2. 콘솔 UTF-8 인코딩 설정
    SetConsoleOutputCP(CP_UTF8);     // 출력 코드 페이지
    SetConsoleCP(CP_UTF8);           // 입력 코드 페이지
    
    // 3. 가상 터미널 시퀀스 활성화 (UTF-8 박스 문자 지원)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    
    // 4. Engine 인스턴스 생성
    Engine engine;
    
    // 5. BattleLevel 생성
    BattleLevel* battleLevel = new BattleLevel();
    
    // 6. 맵 로딩
    battleLevel->LoadMap("Assets/BattleMap.txt");
    
    // 7. 플레이어 유닛 생성 및 배치
    Unit* lord = new Unit(UnitClass::Lord);
    battleLevel->AddUnit(lord, Vector2(1, 1), Faction::Player, 0);
    
    Unit* cavalier = new Unit(UnitClass::Cavalier);
    battleLevel->AddUnit(cavalier, Vector2(3, 1), Faction::Player, 1);
    
    Unit* archer = new Unit(UnitClass::Archer);
    battleLevel->AddUnit(archer, Vector2(5, 1), Faction::Player, 2);
    
    // 8. 적 유닛 생성 및 배치
    Unit* enemySoldier1 = new Unit(UnitClass::Soldier);
    battleLevel->AddUnit(enemySoldier1, Vector2(10, 10), Faction::Enemy);
    
    Unit* enemySoldier2 = new Unit(UnitClass::Soldier);
    battleLevel->AddUnit(enemySoldier2, Vector2(11, 10), Faction::Enemy);
    
    Unit* enemyArcher = new Unit(UnitClass::Archer);
    battleLevel->AddUnit(enemyArcher, Vector2(12, 10), Faction::Enemy);
    
    // 9. 레벨 설정
    engine.SetNewLevel(battleLevel);
    
    // 10. 게임 루프 시작
    engine.Run();
    
    // 11. 게임 종료 후 자동으로 소멸자 호출
    return 0;
}
```

### 단계별 설명

**1단계: 메모리 누수 체크**
- Visual Studio의 CRT 디버그 힙 기능을 활성화
- 프로그램 종료 시 메모리 누수가 있으면 Output 창에 자동 출력

**2-3단계: 콘솔 UTF-8 설정**
- Windows 콘솔의 입출력 코드 페이지를 UTF-8로 설정
- `ENABLE_VIRTUAL_TERMINAL_PROCESSING` 플래그로 ANSI 이스케이프 시퀀스 지원
- UTF-8 박스 문자(░, ▒, ▓, ♣, ▲ 등)가 올바르게 표시되도록 설정

**4단계: Engine 생성**
- Engine 생성자 호출 (다음 섹션 참조)

**5-6단계: BattleLevel 생성 및 맵 로딩**
- BattleLevel 인스턴스 생성 (기본 15x15 그리드)
- `LoadMap()` 함수로 `Assets/BattleMap.txt` 파일 읽기
- 지형 정보(평지, 숲, 산, 성, 물, 벽 등)를 Grid에 설정

**7-8단계: 유닛 배치**
- 플레이어 유닛 3개 (Lord, Cavalier, Archer)
- 적 유닛 3개 (Soldier x2, Archer x1)
- `AddUnit()` 함수로 그리드 위치, 진영, 유닛 인덱스 설정

**9단계: 레벨 설정**
- `SetNewLevel()`로 battleLevel을 nextLevel에 임시 저장
- 다음 프레임에 mainLevel로 전환됨

**10단계: 게임 루프 시작**
- `engine.Run()` 호출로 무한 루프 진입

---

## 2. Engine 생성 단계 (Engine::Engine())

### 생성자 실행 순서

```cpp
Engine::Engine()
{
    // 1. 싱글톤 인스턴스 설정
    instance = this;
    
    // 2. Input 관리자 생성
    input = new Input();
    
    // 3. 설정 파일 로드
    LoadSetting();
    
    // 4. Renderer 생성
    renderer = new Renderer(Vector2(setting.width, setting.height));
    
    // 5. 콘솔 커서 숨김
    Util::TurnOffCursor();
    
    // 6. 랜덤 시드 설정
    Util::SetRandomSeed();
}
```

### 단계별 설명

**1단계: 싱글톤 설정**
- 전역 정적 변수 `instance`에 자신의 포인터 저장
- 이후 `Engine::Get()`으로 어디서든 접근 가능

**2단계: Input 관리자 생성**
- `Input` 클래스 인스턴스 생성
- 키보드/마우스 입력 상태를 추적할 배열 초기화 (255개 키)
- 싱글톤으로 설정되어 `Input::Get()`으로 접근 가능

**3단계: 설정 파일 로드 (LoadSetting())**
```cpp
void Engine::LoadSetting()
{
    // Config/Setting.txt 파일 열기
    FILE* file = nullptr;
    fopen_s(&file, "../Config/Setting.txt", "rt");
    
    // 파일 내용 읽기
    char buffer[2048] = {};
    fread(buffer, sizeof(char), 2048, file);
    
    // 문자열 파싱 (strtok_s로 줄 단위 분리)
    char* context = nullptr;
    char* token = strtok_s(buffer, "\n", &context);
    
    while (token)
    {
        // "framerate = 60.0" 형식 파싱
        if (strstr(token, "framerate"))
            sscanf_s(token, "framerate = %f", &setting.framerate);
        else if (strstr(token, "width"))
            sscanf_s(token, "width = %d", &setting.width);
        else if (strstr(token, "height"))
            sscanf_s(token, "height = %d", &setting.height);
        
        token = strtok_s(nullptr, "\n", &context);
    }
    
    fclose(file);
}
```

**파싱 결과:**
- `setting.framerate` = 60.0 (FPS)
- `setting.width` = 70 (콘솔 너비)
- `setting.height` = 50 (콘솔 높이 - 2x2 멀티라인 렌더링 고려)

**4단계: Renderer 생성**
- `Renderer` 인스턴스 생성, 화면 크기 전달
- 내부적으로 두 개의 `ScreenBuffer` 생성 (더블 버퍼링)
- 각 ScreenBuffer는 `CreateConsoleScreenBuffer`로 Windows 콘솔 버퍼 생성
- `Frame` 객체 생성 (width × height 크기의 `CHAR_INFO` 배열)

**5단계: 콘솔 커서 숨김**
- `SetConsoleCursorInfo`로 커서 가시성을 FALSE로 설정
- 게임 화면에 깜빡이는 커서가 보이지 않도록 함

**6단계: 랜덤 시드 설정**
- `srand(time(nullptr))`로 랜덤 시드 초기화
- 이후 `rand()` 호출 시 매번 다른 난수 생성

---

## 3. 레벨 설정 단계 (Engine::SetNewLevel())

```cpp
void Engine::SetNewLevel(Level* newLevel)
{
    // 새 레벨을 nextLevel에 임시 저장
    nextLevel = newLevel;
    
    // 실제 전환은 프레임 끝에서 발생 (안전한 타이밍)
}
```

### BattleLevel 생성자에서 발생하는 일

```cpp
BattleLevel::BattleLevel()
    : Level()
    , grid(nullptr)
    , selectedUnit(nullptr)
    , isPlayerTurn(true)
    , turnCount(1)
{
    // 기본 15x15 그리드 생성
    grid = new Grid(15, 15);
}
```

**Grid 생성 과정:**
1. `std::vector<std::vector<Tile*>>` 초기화
2. width × height 개수만큼 Tile 객체 동적 할당
3. 각 Tile은 기본값으로 `TerrainType::Plain` 설정

### LoadMap() 실행 과정

```cpp
bool BattleLevel::LoadMap(const char* mapFilePath)
{
    std::ifstream file(mapFilePath);
    
    // 1. 맵 크기 읽기
    int width, height;
    file >> width >> height;
    
    // 2. 기존 그리드 삭제 후 새로 생성
    delete grid;
    grid = new Grid(width, height);
    
    // 3. 지형 데이터 읽기 (문자 단위)
    for (int y = 0; y < height; ++y)
    {
        std::getline(file, line);
        for (int x = 0; x < width; ++x)
        {
            char ch = line[x];
            
            // 4. 문자에 따라 지형 타입 결정
            switch (ch)
            {
                case '.': type = TerrainType::Plain; break;
                case 'T': type = TerrainType::Forest; break;
                case 'M': type = TerrainType::Mountain; break;
                case 'C': type = TerrainType::Castle; break;
                case 'V': type = TerrainType::Village; break;
                case '~': type = TerrainType::Water; break;
                case '#': type = TerrainType::Wall; break;
            }
            
            // 5. Grid에 타일 설정
            grid->SetTile(x, y, type);
        }
    }
    
    return true;
}
```

### AddUnit() 실행 과정

```cpp
void BattleLevel::AddUnit(Unit* unit, const Vector2& gridPosition, 
                          Faction faction, int unitIndex)
{
    // 1. 유닛 속성 설정
    unit->SetGridPosition(gridPosition);
    unit->SetFaction(faction);
    unit->SetUnitIndex(unitIndex);
    
    // 2. 타일에 유닛 존재 표시
    Tile* tile = grid->GetTile(gridPosition);
    if (tile)
        tile->SetHasUnit(true);
    
    // 3. Level의 Actor 목록에 추가
    AddNewActor(unit);
    
    // 4. 진영별 목록에 추가
    if (faction == Faction::Player)
        playerUnits.push_back(unit);
    else if (faction == Faction::Enemy)
        enemyUnits.push_back(unit);
}
```

### Unit 생성자에서 발생하는 일

```cpp
Unit::Unit(UnitClass unitClass)
{
    // 1. 기본 스탯 초기화 (UnitStats 구조체)
    // HP: 20, STR: 5, MAG: 0, SKL: 5, SPD: 5
    // LCK: 0, DEF: 3, RES: 0, MOV: 5
    
    // 2. 병과에 따른 표시 문자 설정
    strcpy_s(displayStr, sizeof(displayStr), GetUnitClassString(unitClass));
    
    // 3. Lord 유닛 특별 대우
    if (unitClass == UnitClass::Lord)
    {
        // 모든 스탯 50% 강화
        stats.maxHP = 30;      // 20 * 1.5
        stats.currentHP = 30;
        stats.strength = 7;    // 5 * 1.5
        stats.skill = 7;       // 5 * 1.5
        stats.speed = 7;       // 5 * 1.5
        stats.defense = 4;     // 3 * 1.5
        stats.movement = 7;    // 5 * 1.5
        
        // 0인 스탯은 3으로 설정
        stats.magic = 3;
        stats.luck = 3;
        stats.resistance = 3;
    }
}
```

**AddNewActor() 내부 동작:**
```cpp
void Level::AddNewActor(Actor* newActor)
{
    // 1. 임시 배열에 저장 (즉시 추가하면 이터레이터 무효화 위험)
    addRequestedActors.emplace_back(newActor);
    
    // 2. 오너십 설정 (Actor가 자신을 소유한 Level 알게 함)
    newActor->SetOwner(this);
}
```

---

## 4. 게임 루프 시작 (Engine::Run())

### 초기화 단계

```cpp
void Engine::Run()
{
    // 1. 고해상도 타이머 초기화
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);  // CPU 클럭 주파수
    
    // 2. 프레임레이트 계산
    setting.framerate = (setting.framerate == 0.0f) ? 60.0f : setting.framerate;
    float oneFrameTime = 1.0f / setting.framerate;  // 1/60 = 0.01667초
    
    // 3. 현재/이전 시간 초기화
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    int64_t currentTime = time.QuadPart;
    int64_t previousTime = currentTime;
    
    // 4. 무한 루프 시작
    while (!isQuit)
    {
        // (다음 섹션 참조)
    }
    
    // 5. 종료 시 정리
    Shutdown();
}
```

### 메인 루프 실행 흐름

```cpp
while (!isQuit)
{
    // ========== 1. 시간 계산 ==========
    QueryPerformanceCounter(&time);
    currentTime = time.QuadPart;
    
    float deltaTime = static_cast<float>(currentTime - previousTime);
    deltaTime = deltaTime / static_cast<float>(frequency.QuadPart);  // 초 단위 변환
    
    // ========== 2. 고정 프레임레이트 체크 ==========
    if (deltaTime >= oneFrameTime)  // 1/60초 이상 경과했는가?
    {
        // ========== 3. 입력 처리 ==========
        input->ProcessInput();
        
        // ========== 4. 게임 로직 실행 ==========
        BeginPlay();              // 새 액터 초기화
        Tick(deltaTime);          // 게임 로직 업데이트
        Draw();                   // 렌더링
        
        // ========== 5. 시간 갱신 ==========
        previousTime = currentTime;
        
        // ========== 6. 입력 상태 저장 ==========
        input->SavePreviousInputStates();
        
        // ========== 7. 액터 추가/제거 처리 ==========
        if (mainLevel)
            mainLevel->ProcessAddAndDestroyActors();
        
        // ========== 8. 레벨 전환 처리 ==========
        if (nextLevel)
        {
            delete mainLevel;      // 기존 레벨 삭제
            mainLevel = nextLevel; // 새 레벨 설정
            nextLevel = nullptr;   // 포인터 정리
        }
    }
}
```

### 각 단계 상세 설명

#### **3단계: 입력 처리 (Input::ProcessInput())**

```cpp
void Input::ProcessInput()
{
    // 모든 키 상태 체크 (VK_0 ~ VK_9, VK_A ~ VK_Z, 마우스 버튼 등)
    for (int i = 0; i < 255; ++i)
    {
        // GetAsyncKeyState: Windows API로 현재 키 상태 확인
        SHORT keyState = GetAsyncKeyState(i);
        keyStates[i].isKeyDown = (keyState & 0x8000) != 0;
    }
    
    // 마우스 위치 계산
    POINT cursorPos;
    GetCursorPos(&cursorPos);                           // 화면 좌표
    ScreenToClient(GetConsoleWindow(), &cursorPos);     // 클라이언트 좌표
    
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    
    // 픽셀 좌표 -> 콘솔 셀 좌표 변환
    mousePosition.x = cursorPos.x / info.dwSize.X;
    mousePosition.y = cursorPos.y / info.dwSize.Y;
}
```

#### **4-1단계: BeginPlay() - 새 액터 초기화**

```cpp
void Engine::BeginPlay()
{
    if (!mainLevel) return;
    mainLevel->BeginPlay();
}

void Level::BeginPlay()
{
    for (Actor* actor : actors)
    {
        // 아직 BeginPlay가 호출되지 않은 액터만 초기화
        if (!actor->HasBeganPlay())
        {
            actor->BeginPlay();  // 가상 함수 호출
        }
    }
}
```

**Unit::BeginPlay() 상세:**
```cpp
void Unit::BeginPlay()
{
    Actor::BeginPlay();
    
    // Actor의 position은 사용하지 않음
    // 렌더링은 gridPosition만 사용 (2x2 좌표 변환)
}
```

#### **4-2단계: Tick() - 게임 로직 업데이트**

```cpp
void Engine::Tick(float deltaTime)
{
    if (!mainLevel) return;
    mainLevel->Tick(deltaTime);
}

void Level::Tick(float deltaTime)
{
    for (Actor* actor : actors)
    {
        actor->Tick(deltaTime);  // 각 액터의 Tick 호출
    }
}
```

**BattleLevel::Tick() 상세:**
```cpp
void BattleLevel::Tick(float deltaTime)
{
    // 1. 부모 클래스 Tick 호출 (모든 Unit의 Tick 실행)
    Level::Tick(deltaTime);
    
    // 2. 입력 처리 (유닛 선택, 이동 명령 등)
    HandleInput();
}
```

**Unit::Tick() 상세:**
```cpp
void Unit::Tick(float deltaTime)
{
    Actor::Tick(deltaTime);
    
    // 이동 중인 경우 애니메이션 처리
    if (state == UnitState::Moving)
    {
        UpdateMovement(deltaTime);
    }
}

void Unit::UpdateMovement(float deltaTime)
{
    if (movementPath.empty())
    {
        state = UnitState::Done;  // 이동 완료 후 턴 종료
        return;
    }
    
    // 타이머 증가
    moveTimer += deltaTime;
    
    // 다음 타일로 이동할 시간이 되었는가?
    float timePerTile = 1.0f / moveSpeed;
    if (moveTimer >= timePerTile)
    {
        moveTimer = 0.0f;
        
        // 경로의 다음 위치로 이동
        gridPosition = movementPath.front();
        movementPath.pop_front();
        
        // 모든 경로를 이동했으면 턴 종료
        if (movementPath.empty())
        {
            state = UnitState::Done;
        }
    }
}
```

#### **4-3단계: Draw() - 렌더링**

```cpp
void Engine::Draw()
{
    if (!mainLevel) return;
    
    // 1. 레벨의 모든 액터가 렌더 데이터를 제출
    mainLevel->Draw();
    
    // 2. Renderer가 제출된 데이터를 화면에 그리기
    renderer->Draw();
}
```

**BattleLevel::Draw() 상세:**
```cpp
void BattleLevel::Draw()
{
    // 1. 그리드(지형) 렌더링
    DrawGrid();
    
    // 2. 이동 가능 범위 하이라이트
    if (selectedUnit != nullptr)
    {
        DrawMovementRange();
    }
    
    // 3. 모든 유닛 렌더링 (Level::Draw() 호출)
    Level::Draw();
    
    // 4. 스탯 UI 패널
    DrawStatsPanel();
    
    // 5. 턴 정보 표시
    sprintf_s(uiBuffers[10], "Turn:%d Phase:%s", turnCount, 
              isPlayerTurn ? "PLAYER" : "ENEMY");
    Renderer::Get().Submit(uiBuffers[10], Vector2(0, 0), Color::White, 10);
}
```

**DrawGrid() 상세 (2x2 멀티라인 렌더링):**
```cpp
void BattleLevel::DrawGrid()
{
    if (!grid) return;
    
    for (int y = 0; y < grid->GetHeight(); ++y)
    {
        for (int x = 0; x < grid->GetWidth(); ++x)
        {
            Tile* tile = grid->GetTile(x, y);
            if (!tile) continue;
            
            // 2x2 멀티라인 ASCII 아트 가져오기
            const char* topLeft, * topRight, * bottomLeft, * bottomRight;
            tile->GetDisplayStrings(topLeft, topRight, bottomLeft, bottomRight);
            
            // 화면 좌표 변환 (그리드 x,y → 화면 x*2+1, y*2+1)
            int baseX = x * 2 + 1;
            int baseY = y * 2 + 1;
            
            Color tileColor = tile->GetDisplayColor();
            
            // 4개의 셀을 각각 렌더링
            Renderer::Get().Submit(topLeft, Vector2(baseX, baseY), 
                                 tileColor, 7);       // 우선순위 7
            Renderer::Get().Submit(topRight, Vector2(baseX + 1, baseY), 
                                 tileColor, 7);
            Renderer::Get().Submit(bottomLeft, Vector2(baseX, baseY + 1), 
                                 tileColor, 7);
            Renderer::Get().Submit(bottomRight, Vector2(baseX + 1, baseY + 1), 
                                 tileColor, 7);
        }
    }
}
```

**Level::Draw() 상세:**
```cpp
void Level::Draw()
{
    for (Actor* actor : actors)
    {
        if (!actor->IsActive()) continue;
        
        actor->Draw();  // 각 액터의 Draw 호출
    }
}
```

**Unit::Draw() 상세 (2x2 멀티라인 렌더링):**
```cpp
void Unit::Draw()
{
    // gridPosition을 화면 좌표로 변환 (2x2 그리드)
    int baseX = gridPosition.x * 2 + 1;
    int baseY = gridPosition.y * 2 + 1;
    
    Color color = GetDisplayColor();
    
    // 4개의 셀에 동일한 문자 렌더링 (예: "L")
    Renderer::Get().Submit(displayStr, Vector2(baseX, baseY), 
                         color, 10);           // 우선순위 10
    Renderer::Get().Submit(displayStr, Vector2(baseX + 1, baseY), 
                         color, 10);
    Renderer::Get().Submit(displayStr, Vector2(baseX, baseY + 1), 
                         color, 10);
    Renderer::Get().Submit(displayStr, Vector2(baseX + 1, baseY + 1), 
                         color, 10);
}

Color Unit::GetDisplayColor() const
{
    // Lord는 항상 녹색
    if (unitClass == UnitClass::Lord)
        return Color::Green;
    
    // 선택 상태면 노란색
    if (state == UnitState::Selected)
        return FEClone::Color::Yellow;
    
    // 턴 종료 상태면 흰색
    if (state == UnitState::Done)
        return Color::White;
    
    // 진영에 따라 색상 결정
    switch (faction)
    {
        case Faction::Player: return FEClone::Color::Cyan;  // 청록색
        case Faction::Enemy:  return Color::Red;            // 빨간색
        case Faction::Ally:   return Color::Green;          // 초록색
        default:              return Color::White;          // 흰색
    }
}
```

**Renderer::Draw() 상세:**
```cpp
void Renderer::Draw()
{
    // 1. 프레임 초기화
    Clear();
    
    // 2. renderQueue 순회하며 CHAR_INFO 배열에 기록
    for (const RenderCommand& command : renderQueue)
    {
        // UTF-8 문자열을 UTF-16으로 변환
        int wideLength = MultiByteToWideChar(CP_UTF8, 0, command.text, -1, nullptr, 0);
        wchar_t* wideText = new wchar_t[wideLength];
        MultiByteToWideChar(CP_UTF8, 0, command.text, -1, wideText, wideLength);
        
        // CHAR_INFO 배열에 기록
        for (int i = 0; i < wideLength - 1; ++i)
        {
            int x = command.position.x + i;
            int y = command.position.y;
            int index = y * screenSize.x + x;
            
            // sortingOrder 체크 (낮은 우선순위는 덮어쓰지 않음)
            if (command.sortingOrder >= frame->sortingOrderArray[index])
            {
                frame->charInfoArray[index].Char.UnicodeChar = wideText[i];
                frame->charInfoArray[index].Attributes = command.color;
                frame->sortingOrderArray[index] = command.sortingOrder;
            }
        }
        
        delete[] wideText;
    }
    
    // 3. 화면에 출력 (더블 버퍼링)
    Present();
    
    // 4. renderQueue 초기화 (다음 프레임 준비)
    renderQueue.clear();
}

void Renderer::Present()
{
    // 현재 버퍼 가져오기
    ScreenBuffer* buffer = GetCurrentBuffer();
    
    // WriteConsoleOutputW로 한 번에 출력
    buffer->Draw(frame->charInfoArray, screenSize);
    
    // 버퍼 전환 (0 <-> 1)
    currentBufferIndex = 1 - currentBufferIndex;
}
```

**ScreenBuffer::Draw() 상세:**
```cpp
void ScreenBuffer::Draw(CHAR_INFO* charInfoArray, const Vector2& screenSize)
{
    // 출력 영역 설정
    SMALL_RECT writeRegion = { 0, 0, 
                               static_cast<SHORT>(screenSize.x - 1), 
                               static_cast<SHORT>(screenSize.y - 1) };
    
    // WriteConsoleOutputW: CHAR_INFO 배열을 버퍼에 한 번에 기록
    WriteConsoleOutputW(
        buffer,           // 콘솔 버퍼 핸들
        charInfoArray,    // CHAR_INFO 배열
        { static_cast<SHORT>(screenSize.x), static_cast<SHORT>(screenSize.y) },
        { 0, 0 },         // 버퍼 시작 좌표
        &writeRegion      // 출력 영역
    );
    
    // SetConsoleActiveScreenBuffer: 이 버퍼를 활성화 (화면에 표시)
    SetConsoleActiveScreenBuffer(buffer);
}
```

#### **6단계: 입력 상태 저장**

```cpp
void Input::SavePreviousInputStates()
{
    for (int i = 0; i < 255; ++i)
    {
        keyStates[i].wasKeyDown = keyStates[i].isKeyDown;
    }
}
```

**이유:**
- `GetKeyDown()`은 "이전 프레임에 안 눌렸고, 현재 프레임에 눌렸을 때" true 반환
- `wasKeyDown`을 저장해야 다음 프레임에 비교 가능

#### **7단계: 액터 추가/제거 처리**

```cpp
void Level::ProcessAddAndDestroyActors()
{
    // 1. 삭제 요청된 액터 제거
    for (int i = 0; i < actors.size(); )
    {
        if (actors[i]->DestroyRequested())
        {
            delete actors[i];               // 메모리 해제
            actors.erase(actors.begin() + i); // 벡터에서 제거
            continue;  // 인덱스 증가 안함
        }
        ++i;
    }
    
    // 2. 추가 요청된 액터 등록
    for (Actor* actor : addRequestedActors)
    {
        actors.emplace_back(actor);
    }
    
    addRequestedActors.clear();
}
```

**타이밍 중요:**
- 프레임 중간에 액터를 추가/삭제하면 `for` 루프의 이터레이터가 무효화됨
- 프레임 끝에서 일괄 처리하여 안전성 확보

#### **8단계: 레벨 전환 처리**

```cpp
if (nextLevel)
{
    delete mainLevel;      // 기존 레벨 및 모든 액터 삭제
    mainLevel = nextLevel; // 새 레벨 설정
    nextLevel = nullptr;   // 포인터 정리
}
```

---

## 5. 입력 처리 흐름 (BattleLevel::HandleInput())

### 유닛 선택 및 턴 종료 (키보드 입력)

```cpp
void BattleLevel::HandleInput()
{
    if (!isPlayerTurn) return;  // 플레이어 턴이 아니면 입력 무시
    
    // 1~9번 키: 유닛 인덱스 0~8
    for (int i = 1; i <= 9; ++i)
    {
        if (Input::Get().GetKeyDown(0x30 + i))  // VK_1 ~ VK_9
        {
            SelectUnitByIndex(i - 1);
            return;
        }
    }
    
    // 0번 키: 유닛 인덱스 9
    if (Input::Get().GetKeyDown(0x30))  // VK_0
    {
        SelectUnitByIndex(9);
        return;
    }
    
    // ESC 키: 선택 해제
    if (Input::Get().GetKeyDown(VK_ESCAPE))
    {
        if (selectedUnit != nullptr)
        {
            selectedUnit->SetState(UnitState::Idle);
            selectedUnit = nullptr;
        }
        reachableTiles.clear();
        return;
    }
    
    // SPACE 키: 턴 종료 및 전환
    if (Input::Get().GetKeyDown(VK_SPACE))
    {
        // 선택 해제
        if (selectedUnit != nullptr)
        {
            selectedUnit->SetState(UnitState::Idle);
            selectedUnit = nullptr;
        }
        reachableTiles.clear();
        
        // 플레이어 턴 ↔ 적 턴 전환
        isPlayerTurn = !isPlayerTurn;
        if (isPlayerTurn)
            turnCount++;  // 새로운 턴 시작
        
        // 모든 유닛 상태 초기화 (Done → Idle)
        for (Unit* unit : playerUnits)
        {
            if (unit->GetState() == UnitState::Done)
                unit->SetState(UnitState::Idle);
        }
        for (Unit* unit : enemyUnits)
        {
            if (unit->GetState() == UnitState::Done)
                unit->SetState(UnitState::Idle);
        }
        return;
    }
    
    // 마우스 클릭: 이동 명령
    if (Input::Get().GetMouseButtonDown(0) && selectedUnit != nullptr)
    {
        Vector2 mousePos = Input::Get().MousePosition();
        OnMouseClick(mousePos);
    }
}
```

### SelectUnitByIndex() 상세

```cpp
void BattleLevel::SelectUnitByIndex(int index)
{
    // 1. 플레이어 유닛 목록에서 검색
    Unit* targetUnit = nullptr;
    for (Unit* unit : playerUnits)
    {
        if (unit->GetUnitIndex() == index)
        {
            targetUnit = unit;
            break;
        }
    }
    
    if (!targetUnit) return;  // 해당 인덱스 유닛 없음
    
    // 2. 이미 행동 완료한 유닛은 선택 불가
    if (targetUnit->HasActedThisTurn())
        return;
    
    // 3. 유닛 선택
    selectedUnit = targetUnit;
    selectedUnit->SetState(UnitState::Selected);
    
    // 4. 이동 가능 범위 계산 (Dijkstra 알고리즘)
    reachableTiles.clear();
    movementCalculator.CalculateReachableTiles(
        selectedUnit->GetGridPosition(),
        selectedUnit->GetStats().movement,  // 이동력
        grid,
        &reachableTiles
    );
}
```

### MovementCalculator::CalculateReachableTiles() 상세

```cpp
void MovementCalculator::CalculateReachableTiles(
    const Vector2& start, int maxMovement, const Grid* grid,
    std::vector<Vector2>* outReachableTiles)
{
    // 1. 초기화
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    costMap.clear();
    
    // 2. 시작 노드 추가
    Node startNode;
    startNode.position = start;
    startNode.cost = 0;
    openList.push(startNode);
    costMap[start] = 0;
    
    // 3. Dijkstra 알고리즘
    while (!openList.empty())
    {
        Node current = openList.top();
        openList.pop();
        
        // 4. 4방향 이웃 탐색
        for (int i = 0; i < 4; ++i)
        {
            Vector2 neighborPos = current.position + directions[i];
            
            // 유효한 위치인가?
            if (!grid->IsValidPosition(neighborPos))
                continue;
            
            Tile* tile = grid->GetTile(neighborPos);
            if (!tile || !tile->IsWalkable() || tile->HasUnit())
                continue;
            
            // 이동 비용 계산
            int newCost = current.cost + tile->GetMovementCost();
            
            // 최대 이동력을 초과하는가?
            if (newCost > maxMovement)
                continue;
            
            // 이미 방문했고 더 낮은 비용으로 도달했는가?
            if (costMap.count(neighborPos) && costMap[neighborPos] <= newCost)
                continue;
            
            // 5. 이웃 노드 추가
            Node neighborNode;
            neighborNode.position = neighborPos;
            neighborNode.cost = newCost;
            openList.push(neighborNode);
            costMap[neighborPos] = newCost;
        }
    }
    
    // 6. 도달 가능한 모든 타일 반환
    for (const auto& pair : costMap)
    {
        if (pair.first != start)  // 시작 위치 제외
            outReachableTiles->push_back(pair.first);
    }
}
```

### OnMouseClick() 상세 (이동 명령 - 2x2 좌표 변환 포함)

```cpp
void BattleLevel::OnMouseClick(const Vector2& mousePos)
{
    // 1. 마우스 위치를 그리드 좌표로 변환
    // 2x2 렌더링이므로 ÷2 필요, UI 여백 -1 적용
    int gridX = (mousePos.x - 1) / 2;
    int gridY = (mousePos.y - 1) / 2;
    Vector2 gridPos(gridX, gridY);
    
    // 2. 유효한 그리드 위치인가?
    if (!grid->IsValidPosition(gridPos))
        return;
    
    // 3. 이동 가능한 타일인가?
    bool isReachable = false;
    for (const Vector2& tile : reachableTiles)
    {
        if (tile == gridPos)
        {
            isReachable = true;
            break;
        }
    }
    
    if (!isReachable)
        return;
    
    // 4. A* 경로 탐색
    std::deque<Vector2> path;
    std::vector<std::vector<bool>> navMap = grid->GenerateNavigationMap();
    
    navigationSystem.FindPath(
        selectedUnit->GetGridPosition(),
        gridPos,
        navMap,
        &path
    );
    
    if (path.empty())
        return;
    
    // 5. 이전 타일의 유닛 플래그 해제
    Tile* oldTile = grid->GetTile(selectedUnit->GetGridPosition());
    if (oldTile)
        oldTile->SetHasUnit(false);
    
    // 6. 새 타일의 유닛 플래그 설정 (경로의 마지막 위치)
    Tile* newTile = grid->GetTile(gridPos);
    if (newTile)
        newTile->SetHasUnit(true);
    
    // 7. 유닛에 경로 설정 및 상태 변경
    selectedUnit->SetPath(path);
    // SetPath() 내부에서 state를 Moving으로 변경함
    
    // 8. 선택 해제
    selectedUnit = nullptr;
    reachableTiles.clear();
}
```

### NavigationSystem::FindPath() 상세 (A* 알고리즘)

```cpp
void NavigationSystem::FindPath(
    const Vector2& start, const Vector2& destination,
    const std::vector<std::vector<bool>>& map,
    std::deque<Vector2>* outPath)
{
    // 1. 초기화
    openList = std::priority_queue<Node, std::vector<Node>>();
    closedList.clear();
    
    // 2. 시작 노드 추가
    Node startNode(start, start);
    startNode.gCost = 0.0f;
    startNode.hCost = CalculateHeuristic(start, destination);
    startNode.fCost = startNode.gCost + startNode.hCost;
    openList.push(startNode);
    
    // 3. A* 알고리즘
    while (!openList.empty())
    {
        // 3-1. fCost가 가장 낮은 노드 선택
        Node current = openList.top();
        openList.pop();
        
        // 3-2. 목적지 도착?
        if (current.position == destination)
        {
            ConstructPath(current, start, outPath);
            return;
        }
        
        // 3-3. 닫힌 리스트에 추가
        closedList[current.position] = current;
        
        // 3-4. 4방향 이웃 탐색
        for (int i = 0; i < 4; ++i)
        {
            Vector2 neighborPos = current.position + directions[i];
            
            // 유효한 위치인가?
            if (!IsValidPosition(neighborPos, map))
                continue;
            
            // 이미 방문했는가?
            if (closedList.count(neighborPos))
                continue;
            
            // 3-5. 이웃 노드 생성
            Node neighborNode(neighborPos, current.position);
            neighborNode.gCost = current.gCost + 1.0f;  // 균일 비용
            neighborNode.hCost = CalculateHeuristic(neighborPos, destination);
            neighborNode.fCost = neighborNode.gCost + neighborNode.hCost;
            
            openList.push(neighborNode);
        }
    }
    
    // 4. 경로를 찾지 못한 경우 대체 목적지 설정
    SetAlternativeDestination(start, destination, outPath);
}

float NavigationSystem::CalculateHeuristic(const Vector2& pos, const Vector2& dest)
{
    // 유클리드 거리
    int dx = pos.x - dest.x;
    int dy = pos.y - dest.y;
    return sqrtf(static_cast<float>(dx * dx + dy * dy));
}

void NavigationSystem::ConstructPath(Node goalNode, const Vector2& start, 
                                     std::deque<Vector2>* outPath)
{
    // 역추적으로 경로 생성
    Vector2 current = goalNode.position;
    
    while (current != start)
    {
        outPath->push_front(current);  // 앞에 삽입 (역순 방지)
        
        // 이전 노드로 이동
        current = closedList[current].previousPosition;
    }
}
```

---

## 6. 프로그램 종료

### 종료 트리거

- ESC 키 등으로 `Engine::QuitEngine()` 호출
- `isQuit = true` 설정
- `while (!isQuit)` 루프 탈출

### 정리 순서 (Engine::Shutdown())

```cpp
void Engine::Shutdown()
{
    std::cout << "Engine has been shutdown...." << std::endl;
    
    // 1. 커서 다시 켜기
    Util::TurnOnCursor();
    
    // 2. 소멸자 자동 호출 (스택 객체)
    //    - mainLevel 삭제 (모든 Actor 삭제)
    //    - input 삭제
    //    - renderer 삭제 (ScreenBuffer 삭제)
}
```

### Engine 소멸자

```cpp
Engine::~Engine()
{
    // 1. mainLevel 삭제
    if (mainLevel)
    {
        delete mainLevel;  // Level 소멸자 호출
        mainLevel = nullptr;
    }
    
    // 2. input 삭제
    if (input)
    {
        delete input;
        input = nullptr;
    }
    
    // 3. renderer 삭제
    SafeDelete(renderer);  // Renderer 소멸자 호출
}
```

### Level 소멸자 (연쇄 삭제)

```cpp
Level::~Level()
{
    // 모든 Actor 삭제
    for (Actor*& actor : actors)
    {
        if (actor)
        {
            delete actor;  // Unit 소멸자 호출
            actor = nullptr;
        }
    }
    
    actors.clear();
}
```

### BattleLevel 소멸자

```cpp
BattleLevel::~BattleLevel()
{
    delete grid;  // Grid 소멸자 호출 (모든 Tile 삭제)
}
```

---

## 7. 핵심 데이터 흐름 요약

### 렌더링 파이프라인

```
Actor::Draw()
    ↓
Renderer::Submit(text, position, color, sortingOrder)
    ↓
renderQueue에 RenderCommand 추가
    ↓
Renderer::Draw()
    ↓
UTF-8 → UTF-16 변환 (MultiByteToWideChar)
    ↓
CHAR_INFO 배열에 기록 (sortingOrder 고려)
    ↓
ScreenBuffer::Draw()
    ↓
WriteConsoleOutputW()
    ↓
SetConsoleActiveScreenBuffer() (버퍼 전환)
    ↓
화면 출력
```

### 입력 처리 파이프라인

```
Input::ProcessInput()
    ↓
ReadConsoleInput() (키보드/마우스 이벤트)
    ↓
keyStates[] / mousePosition 갱신
    ↓
BattleLevel::HandleInput()
    ↓
Input::Get().GetKeyDown('1') → SelectUnitByIndex(0)
    ↓
MovementCalculator::CalculateReachableTiles() (Dijkstra)
    ↓
reachableTiles 벡터 갱신
    ↓
DrawMovementRange() (2x2 청록색 "·" 표시, Priority 9)
    ↓
Input::Get().GetMouseButtonDown(0) → OnMouseClick()
    ↓
화면 좌표 → 그리드 좌표 변환 (÷2)
    ↓
NavigationSystem::FindPath() (A*)
    ↓
Unit::SetPath() → state = Moving
```

### 유닛 이동 파이프라인

```
마우스 클릭 (화면 좌표)
    ↓
BattleLevel::OnMouseClick()
    ↓
좌표 변환: (mousePos.x - 1) / 2, (mousePos.y - 1) / 2
    ↓
NavigationSystem::FindPath() (A*)
    ↓
Unit::SetPath(path)
    ↓
Unit::SetState(UnitState::Moving)
    ↓
(매 프레임) Unit::Tick()
    ↓
Unit::UpdateMovement(deltaTime)
    ↓
moveTimer >= (1.0f / moveSpeed) ?
    ↓ YES
경로의 다음 위치로 이동
    ↓
gridPosition = movementPath.front()
    ↓
movementPath.pop_front()
    ↓
movementPath.empty() ? → state = Done
    ↓
movementPath.empty() ? → state = UnitState::Idle
```

---

## 8. 성능 최적화 포인트

### 1. 고정 프레임레이트
- `deltaTime >= oneFrameTime` 체크로 불필요한 업데이트 방지
- CPU 사용률 절감

### 2. 더블 버퍼링
- 백그라운드 버퍼에 그리기 → 완성 후 순간 전환
- 화면 깜빡임 제거

### 3. sortingOrder 시스템
- 같은 위치에 여러 객체가 있을 때 우선순위 높은 것만 표시
- 불필요한 덮어쓰기 방지

### 4. 지연 추가/삭제
- `addRequestedActors`와 `ProcessAddAndDestroyActors()`
- 프레임 중간 이터레이터 무효화 방지

### 5. A* 경로 탐색 최적화
- `std::priority_queue`로 fCost가 낮은 노드 우선 탐색
- `closedList`로 중복 방문 방지
- 휴리스틱 함수로 탐색 방향 유도

---

## 9. 디버깅 팁

### 메모리 누수 체크
```cpp
_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
```
- 프로그램 종료 시 Output 창에 누수 정보 출력

### Null 포인터 체크
```cpp
if (!instance)
{
    std::cout << "Error: Engine::Get(). instance is null\n";
    __debugbreak();  // 디버거 중단점
}
```

### 콘솔 출력 (디버그 메시지)
```cpp
std::cout << "Unit moved to: " << gridPos.x << ", " << gridPos.y << std::endl;
```

### Visual Studio 중단점
- F9: 중단점 설정
- F5: 디버깅 시작
- F10: 한 줄씩 실행 (Step Over)
- F11: 함수 내부로 진입 (Step Into)

---

## 결론

Fire Emblem Clone은 다음과 같은 흐름으로 실행됩니다:

1. **초기화**: Engine → Input → Renderer → BattleLevel → Grid → Units (Lord 스탯 50% 강화)
2. **게임 루프**: Input → BeginPlay → Tick → Draw → ProcessActors
3. **렌더링**: Submit → renderQueue → UTF-8→UTF-16 변환 → CHAR_INFO → WriteConsoleOutputW → 화면 출력
   - 2x2 멀티라인 ASCII 아트로 각 타일과 유닛 표현
   - 렌더링 우선순위: 지형(7) < 하이라이트(9) < 유닛(10)
4. **입력 처리**: 키보드/마우스 상태 체크 → 유닛 선택(1~9,0 키) → 이동 범위 계산(Dijkstra) → 마우스 클릭으로 경로 탐색(A*) → 이동 애니메이션 → 턴 종료(SPACE)
   - 화면 좌표 ↔ 그리드 좌표 변환 (2x2 고려)
5. **UI 시스템**: 
   - 스탯 패널 (우측, uiBuffers 사용)
   - 키보드 툴팁 (하단, 2열 레이아웃)
   - 턴 정보 표시
6. **종료**: Shutdown → 소멸자 연쇄 호출 → 메모리 정리

각 시스템은 싱글톤 패턴, 이벤트 시스템, 지연 처리, 더블 버퍼링, 렌더링 우선순위 등의 설계 패턴으로 안정적이고 효율적으로 동작합니다.
