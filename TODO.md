# FireEmblemClone 구현 TODO 리스트

## 프로젝트 범위 정의

### 최소한의 Fire Emblem 클론
이 프로젝트는 Fire Emblem의 핵심 게임플레이만을 구현하는 **최소한의 클론**입니다.

**포함되는 기능:**
- 턴제 전략 전투 시스템
- 그리드 기반 맵
- 유닛 이동 및 전투
- 레벨업 시스템 (경험치 획득)
- 인벤토리 및 무기 시스템
- AI 적 유닛
- 2개의 맵 (튜토리얼 + 메인 맵)

**포함되지 않는 기능:**
- ❌ 스토리 및 대화 시스템
- ❌ 클래스 체인지 (유닛은 원래 클래스 유지)
- ❌ 복잡한 챕터 진행 시스템
- ❌ 지원 대화 시스템
- ❌ 세이브/로드 시스템

### 맵 구성
1. **튜토리얼 맵** (`Assets/Tutorial.txt`)
   - 작은 크기의 맵 (예: 10x10)
   - 플레이어 유닛 1명 + 적 유닛 1명
   - 기본 조작 및 전투를 배우는 맵
   
2. **메인 맵** (`Assets/MainMap.txt`)
   - 중간 크기의 맵 (예: 20x15)
   - 다양한 플레이어 유닛 및 아군 유닛
   - 다수의 적 유닛
   - 다양한 지형

### 입력 시스템 (하이브리드 키보드 + 마우스)

**마우스 기반 (주요 입력 방식):**
- 유닛 선택 (맵 상의 유닛 클릭)
- 이동할 타일 선택 (이동 범위 내 타일 클릭)
- 공격 대상 선택 (공격 범위 내 유닛 클릭)
- 메뉴 옵션 클릭 (Attack, Item, Wait 등)
- 인벤토리 아이템 클릭
- UI 버튼 클릭

**키보드 기반 (보조 입력 방식):**
- 메뉴 탐색 (방향키 또는 숫자 키)
- 유닛 능력 선택 (A, I, W 등)
- 인벤토리 탐색 (방향키)
- 아이템 사용 (Enter)
- 취소 (ESC)

**중요**: 모든 키보드 기반 액션은 마우스 클릭으로도 가능해야 함

---

## 현재 프로젝트 상태 분석

### 이미 구현된 기능 (ShootingGame 엔진)
✅ Engine 기본 구조 (싱글톤, 게임 루프)
✅ 입력 처리 (키보드, 마우스)
✅ 렌더링 시스템 (더블 버퍼링, ASCII 출력)
✅ Actor 시스템 (생명주기, 충돌 감지)
✅ Level 시스템 (액터 관리)
✅ 수학 라이브러리 (Vector2, Color)
✅ 프레임레이트 제어

### 재사용 가능한 컴포넌트
- `Engine/Engine/Engine.h/cpp`: 메인 루프 그대로 사용 가능
- `Engine/Core/Input.h/cpp`: 커서 이동 및 선택에 활용
- `Engine/Render/`: 렌더링 시스템 확장 필요
- `Engine/Actor/Actor.h/cpp`: Unit의 베이스 클래스로 활용 가능
- `Engine/Level/Level.h/cpp`: Map/Battle 레벨로 확장
- `Engine/Math/Vector2.h/cpp`: 그리드 좌표로 활용

---

## Phase 1: 엔진 확장 (그리드 시스템 기반)

### 1.1. Grid 시스템 구현
- [ ] `Engine/Map/Grid.h/cpp` 생성
  - [ ] Grid 클래스 구현 (width, height, tile 배열)
  - [ ] GetTile(x, y) 함수
  - [ ] IsValidPosition(x, y) 함수
  - [ ] Grid 렌더링 함수

### 1.2. Tile 시스템 구현
- [ ] `Engine/Map/Tile.h/cpp` 생성
  - [ ] Tile 클래스 (지형 타입, 보너스 스탯)
  - [ ] 지형 타입 enum (Plain, Forest, Mountain, Castle, Village 등)
  - [ ] GetDefenseBonus() 함수
  - [ ] GetAvoidBonus() 함수
  - [ ] GetMoveCost() 함수 (클래스별 이동 비용)
  - [ ] Tile 렌더링 (ASCII 문자로 지형 표현)

### 1.3. 렌더링 시스템 확장
- [ ] `Engine/Render/Renderer.h/cpp` 수정
  - [ ] DrawGrid() 함수 추가
  - [ ] DrawTile() 함수 추가
  - [ ] DrawUI() 함수 추가 (커서, 메뉴 등)
  - [ ] 레이어 시스템 추가 (지형 레이어, 유닛 레이어, UI 레이어)

### 1.4. 입력 시스템 확장 (하이브리드 마우스+키보드)
- [ ] `Engine/Core/Input.h/cpp` 수정
  - [ ] 마우스 클릭을 그리드 좌표로 변환하는 함수
  - [ ] ScreenToGrid(screenX, screenY) 함수
  - [ ] IsGridClicked(gridX, gridY) 함수
  - [ ] GetClickedGridPosition() 함수

### 1.5. Cursor 시스템 구현
- [ ] `Engine/UI/Cursor.h/cpp` 생성
  - [ ] Cursor 위치 (GridX, GridY)
  - [ ] 마우스 클릭으로 커서 이동
  - [ ] 키보드 방향키로 커서 이동 (보조)
  - [ ] 커서 렌더링 (하이라이트)
  - [ ] 현재 커서 위치의 타일/유닛 정보 표시

---

## Phase 2: 유닛 시스템

### 2.1. Unit 기본 클래스
- [ ] `Game/Unit/Unit.h/cpp` 생성 (Actor 상속)
  - [ ] 기본 스탯 (HP, Str, Mag, Skl, Spd, Lck, Def, Res, Mov)
  - [ ] 현재 HP, 최대 HP
  - [ ] Level, Experience
  - [ ] 그리드 좌표 (GridX, GridY) - Vector2 대신 정수 좌표
  - [ ] 소속 (Player, Enemy, Ally, NPC)
  - [ ] 행동 완료 플래그 (IsActionDone)
  - [ ] GetDisplayCharacter() 함수 (ASCII 표현)
  - [ ] CanMoveTo(x, y) 함수
  - [ ] MoveTo(x, y) 함수
  - [ ] TakeDamage(damage) 함수
  - [ ] IsAlive() 함수

### 2.2. Unit 클래스 시스템
- [ ] `Game/Unit/UnitClass.h/cpp` 생성
  - [ ] UnitClass enum (Lord, Cavalier, Knight, Archer, Mage 등)
  - [ ] 클래스별 기본 스탯
  - [ ] 클래스별 성장률
  - [ ] 클래스별 이동 타입 (보행, 기마, 비행)
  - [ ] 클래스별 사용 가능한 무기 타입

### 2.3. 유닛 생성 및 관리
- [ ] `Game/Unit/PlayerUnit.h/cpp` (플레이어 유닛)
- [ ] `Game/Unit/EnemyUnit.h/cpp` (적 유닛)
- [ ] 유닛 스탯 초기화
- [ ] 유닛 렌더링 (그리드 위에 표시)

---

## Phase 3: 전투 시스템

### 3.1. Weapon 시스템
- [ ] `Game/Item/Weapon.h/cpp` 생성
  - [ ] 무기 타입 enum (Sword, Lance, Axe, Bow, Magic)
  - [ ] 무기 스탯 (공격력, 명중률, 크리티컬률, 무게, 사거리)
  - [ ] 무기 내구도 (현재/최대)
  - [ ] 무기 레벨 (E, D, C, B, A, S)
  - [ ] UseWeapon() 함수 (내구도 감소)
  - [ ] IsUsable(unit) 함수 (레벨 체크)

### 3.2. Inventory 시스템
- [ ] `Game/Unit/Inventory.h/cpp` 생성
  - [ ] 아이템 슬롯 배열 (최대 4-6개)
  - [ ] AddItem(item) 함수
  - [ ] RemoveItem(index) 함수
  - [ ] GetEquippedWeapon() 함수
  - [ ] EquipWeapon(index) 함수

### 3.3. Combat 계산 시스템
- [ ] `Game/Combat/CombatCalculator.h/cpp` 생성
  - [ ] CalculateHitRate(attacker, defender, terrain) 함수
  - [ ] CalculateAvoid(unit, terrain) 함수
  - [ ] CalculateDamage(attacker, defender) 함수
  - [ ] CalculateCritical(attacker, defender) 함수
  - [ ] CanDoubleAttack(attacker, defender) 함수
  - [ ] WeaponTriangleBonus(weapon1, weapon2) 함수

### 3.4. Combat 시퀀스
- [ ] `Game/Combat/CombatSequence.h/cpp` 생성
  - [ ] InitiateCombat(attacker, defender) 함수
  - [ ] ExecuteAttack(attacker, defender) 함수
    - [ ] 명중 판정 (난수 생성)
    - [ ] 데미지 계산 및 적용
    - [ ] 크리티컬 판정
  - [ ] ExecuteCounterAttack() 함수 (조건 체크)
  - [ ] ExecuteDoubleAttack() 함수 (조건 체크)
  - [ ] ShowCombatResult() 함수 (간단한 텍스트 출력)

---

## Phase 4: 턴 관리 시스템

### 4.1. Turn Manager
- [ ] `Game/System/TurnManager.h/cpp` 생성
  - [ ] 현재 페이즈 (PlayerPhase, EnemyPhase)
  - [ ] 턴 카운터
  - [ ] StartPlayerPhase() 함수
  - [ ] EndPlayerPhase() 함수
  - [ ] StartEnemyPhase() 함수
  - [ ] EndEnemyPhase() 함수
  - [ ] ResetAllUnitActions() 함수 (페이즈 시작 시)

### 4.2. Player Phase 로직
- [ ] `Game/System/PlayerPhaseController.h/cpp` 생성
  - [ ] **유닛 선택**:
    - [ ] 마우스 클릭으로 플레이어 유닛 선택
    - [ ] 선택한 유닛의 이동 가능 범위 표시
  - [ ] **이동 처리**:
    - [ ] 마우스 클릭으로 이동 목적지 선택
    - [ ] 유닛을 선택한 타일로 이동
  - [ ] **행동 메뉴**:
    - [ ] 행동 메뉴 표시 (Attack, Item, Wait)
    - [ ] 마우스 클릭으로 메뉴 옵션 선택
    - [ ] 키보드로 메뉴 옵션 선택 (보조)
  - [ ] **공격 처리**:
    - [ ] 공격 가능 범위 표시
    - [ ] 마우스 클릭으로 공격 대상 선택
    - [ ] 전투 예측 표시
    - [ ] 전투 실행 확인
  - [ ] **아이템 사용**:
    - [ ] 인벤토리 UI 표시
    - [ ] 마우스 클릭 또는 키보드로 아이템 선택
    - [ ] 아이템 사용 실행
  - [ ] 유닛 행동 완료 처리

### 4.3. Enemy Phase 로직
- [ ] `Game/System/EnemyPhaseController.h/cpp` 생성
  - [ ] AI 유닛 순회
  - [ ] AI 행동 결정 (이동 및 공격)
  - [ ] AI 행동 실행
  - [ ] 자동으로 다음 유닛으로 진행

---

## Phase 5: AI 시스템

### 5.1. AI 기본 구조
- [ ] `Game/AI/AIController.h/cpp` 생성
  - [ ] DecideAction(unit) 함수
  - [ ] FindBestTarget(unit) 함수
  - [ ] FindBestMovePosition(unit, target) 함수
  - [ ] ExecuteAIAction(unit) 함수

### 5.2. AI 패턴 구현
- [ ] **공격형 AI** (기본): 가장 가까운 적 공격
- [ ] **방어형 AI** (선택): 일정 범위 내에서만 반응

### 5.3. Pathfinding
- [ ] `Game/AI/Pathfinding.h/cpp` 생성
  - [ ] FindPath(start, goal, moveRange) 함수
  - [ ] BFS 또는 Dijkstra 알고리즘 구현
  - [ ] 이동 비용 계산 (지형 고려)

---

## Phase 6: 맵 및 레벨 시스템

### 6.1. Map Level
- [ ] `Game/Level/MapLevel.h/cpp` 생성 (Level 상속)
  - [ ] Grid 인스턴스
  - [ ] Unit 배열 (플레이어/적 유닛)
  - [ ] TurnManager 인스턴스
  - [ ] 승리/패배 조건 체크
  - [ ] LoadMap(filepath) 함수 (Assets에서 맵 로드)
  - [ ] SpawnUnit(unitData, x, y) 함수
  - [ ] CheckVictoryCondition() 함수
  - [ ] CheckDefeatCondition() 함수

### 6.2. Map 데이터 로딩
- [ ] **튜토리얼 맵** `Assets/Tutorial.txt` 생성
  - [ ] 작은 맵 크기 (10x10)
  - [ ] 간단한 타일 데이터 (대부분 평지)
  - [ ] 플레이어 유닛 1명 배치 데이터
  - [ ] 적 유닛 1명 배치 데이터
  - [ ] 승리 조건: 적 섬멸
  - [ ] 패배 조건: 플레이어 유닛 사망
  
- [ ] **메인 맵** `Assets/MainMap.txt` 생성
  - [ ] 중간 맵 크기 (20x15)
  - [ ] 다양한 타일 데이터 (평지, 숲, 산, 성 등)
  - [ ] 플레이어 유닛 3-5명 배치 데이터
  - [ ] 아군 NPC 유닛 1-2명 배치 데이터
  - [ ] 적 유닛 5-8명 배치 데이터
  - [ ] 승리 조건: 모든 적 섬멸 또는 특정 타일 점령
  - [ ] 패배 조건: 모든 플레이어 유닛 사망

- [ ] 맵 파일 파싱 로직 구현
  - [ ] 맵 크기 읽기
  - [ ] 타일 데이터 파싱
  - [ ] 유닛 배치 데이터 파싱
  - [ ] 승리/패배 조건 파싱

---

## Phase 7: UI 시스템

### 7.1. 유닛 정보 UI
- [ ] `Game/UI/UnitInfoPanel.h/cpp` 생성
  - [ ] 선택한 유닛의 스탯 표시 (HP, Str, Def 등)
  - [ ] 장비한 무기 표시
  - [ ] 레벨/경험치 표시

### 7.2. 전투 예측 UI
- [ ] `Game/UI/CombatForecast.h/cpp` 생성
  - [ ] 명중률 표시
  - [ ] 예상 데미지 표시
  - [ ] 더블 어택 가능 여부 표시
  - [ ] 크리티컬 확률 표시

### 7.3. 메뉴 시스템
- [ ] `Game/UI/MenuSystem.h/cpp` 생성
  - [ ] 행동 메뉴 (Attack, Item, Wait)
  - [ ] 아이템 메뉴 (인벤토리 표시 및 선택)
  - [ ] **마우스 입력 처리**: 메뉴 옵션 클릭
  - [ ] **키보드 입력 처리**: 방향키/숫자 키로 메뉴 탐색
  - [ ] 메뉴 하이라이트 및 선택 피드백

### 7.4. Range Display
- [ ] `Game/UI/RangeDisplay.h/cpp` 생성
  - [ ] 이동 가능 범위 하이라이트 (파란색)
  - [ ] 공격 가능 범위 하이라이트 (빨간색)
  - [ ] CalculateMoveRange(unit) 함수
  - [ ] CalculateAttackRange(unit) 함수

---

## Phase 8: 게임 진행 시스템

### 8.1. Level Up 시스템
- [ ] `Game/System/LevelUpSystem.h/cpp` 생성
  - [ ] GainExperience(unit, exp) 함수
  - [ ] CheckLevelUp(unit) 함수
  - [ ] RandomStatGrowth(unit) 함수 (클래스별 성장률 기반)
  - [ ] ShowLevelUpResult(unit, statGains) 함수

### 8.2. Victory/Defeat 처리
- [ ] **승리 시**:
  - [ ] 결과 화면 표시 (전투 통계)
  - [ ] 튜토리얼 맵 클리어 시 → 메인 맵으로 이동
  - [ ] 메인 맵 클리어 시 → 게임 클리어 화면
  - [ ] 유닛 및 아이템 상태 유지
- [ ] **패배 시**:
  - [ ] 게임 오버 화면 표시
  - [ ] 재시작 옵션 (현재 맵 처음부터)
  - [ ] 메인 메뉴로 돌아가기 옵션

### 8.3. 맵 전환 시스템 (2개 맵)
- [ ] `Game/System/MapManager.h/cpp` 생성
  - [ ] 현재 맵 상태 관리 (Tutorial 또는 MainMap)
  - [ ] LoadMap(mapName) 함수
  - [ ] 튜토리얼 → 메인 맵 전환 시 유닛/아이템 유지
  - [ ] 메인 메뉴 구현 (맵 선택 또는 게임 시작)

---

## Phase 9: 추가 기능 (선택 사항)

### 9.1. 지형 효과 시스템
- [ ] 지형별 회피/방어 보너스 적용
- [ ] 클래스별 이동 비용 차별화 (기병은 숲에서 불리 등)

### 9.2. 무기 삼각 관계
- [ ] Sword > Axe > Lance > Sword 구현
- [ ] 명중률/회피율 보너스 적용

### 9.3. 아이템 사용 시스템
- [ ] 회복 아이템 (Vulnerary 등)
- [ ] 장비 변경 (무기 교체)

### 9.4. 영구 사망 (Permadeath)
- [ ] 유닛 HP 0 시 영구 사망
- [ ] 다시 사용 불가 처리

### 9.5. 전투 애니메이션 개선
- [ ] 간단한 공격 모션 표시
- [ ] HP 감소 애니메이션
- [ ] 크리티컬 시각 효과

---

## Phase 10: 폴리싱 및 최적화

### 10.1. 애니메이션 추가
- [ ] 유닛 이동 애니메이션 (부드러운 이동)
- [ ] 전투 애니메이션 (간단한 공격 모션)
- [ ] HP 바 애니메이션

### 10.2. 사운드 효과 (선택 사항)
- [ ] 콘솔 Beep 사운드로 간단한 효과음
- [ ] 전투 시 사운드
- [ ] 선택/취소 사운드

### 10.3. 게임 밸런스 조정
- [ ] 유닛 스탯 밸런싱
- [ ] 적 배치 및 난이도 조정
- [ ] 무기 밸런싱

### 10.4. 버그 수정 및 테스트
- [ ] 경계 조건 테스트 (맵 밖 이동 방지 등)
- [ ] 메모리 누수 체크
- [ ] 예외 상황 처리

---

## 구현 우선순위 (추천 순서)

### Milestone 1: 기본 그리드 및 유닛 표시 (튜토리얼 맵)
1. Grid 시스템
2. Tile 시스템 (기본 지형만)
3. 입력 시스템 확장 (마우스 → 그리드 좌표 변환)
4. Cursor 시스템 (마우스 기반)
5. Unit 기본 클래스
6. 튜토리얼 맵 데이터 생성 및 로딩
7. MapLevel에 유닛 배치 및 렌더링

### Milestone 2: 유닛 이동 (마우스 기반)
1. 이동 가능 범위 계산
2. 마우스 클릭으로 유닛 선택
3. 마우스 클릭으로 이동 목적지 선택
4. 간단한 이동 처리 (Pathfinding 없이 직접 이동)
5. 턴 관리 (플레이어 페이즈만)

### Milestone 3: 전투 시스템
1. Weapon 시스템 (기본 무기만)
2. Inventory 시스템
3. Combat Calculator (명중률, 데미지 계산)
4. Combat Sequence (공격, 반격, 더블 어택)
5. 전투 예측 UI
6. 행동 메뉴 UI (Attack, Wait)

### Milestone 4: AI 및 적 페이즈
1. Pathfinding (BFS 기반)
2. Enemy Phase 로직
3. 간단한 AI (공격형 - 가장 가까운 적 공격)
4. 유닛 이동에 Pathfinding 적용

### Milestone 5: 게임 진행 (튜토리얼 → 메인 맵)
1. 승리/패배 조건 체크
2. Level Up 시스템
3. 메인 맵 데이터 생성
4. 맵 전환 시스템 (튜토리얼 → 메인 맵)
5. 결과 화면 및 게임 오버 화면

### Milestone 6: 폴리싱 및 추가 기능
1. UI 개선 (유닛 정보 패널, HP 바 등)
2. 아이템 사용 시스템 (회복 아이템)
3. 지형 효과
4. 무기 삼각 관계 (선택)
5. 영구 사망 (선택)
6. 게임 밸런싱

---

## 추정 작업량 (최소한의 클론 기준)
- **Milestone 1**: 기본 그리드/유닛 표시 - 약 8-12시간
- **Milestone 2**: 유닛 이동 (마우스 기반) - 약 6-10시간
- **Milestone 3**: 전투 시스템 - 약 10-15시간
- **Milestone 4**: AI 및 적 페이즈 - 약 8-12시간
- **Milestone 5**: 게임 진행 (2개 맵) - 약 6-10시간
- **Milestone 6**: 폴리싱 및 추가 기능 - 약 10-15시간

**총 추정 시간**: 약 48-74시간 (최소한의 기능 구현 기준)

**추가 시간** (선택 사항):
- ANSI Art 적용: +5-10시간
- 고급 AI 패턴: +5-8시간
- 복잡한 UI 개선: +8-12시간

---

## 참고 사항

### 기존 엔진 활용
- 현재 슈팅 게임 엔진을 최대한 재사용하되, **실시간 게임에서 턴제 게임으로의 전환**이 핵심
- `Tick` 함수는 여전히 사용하지만, 턴 기반으로 입력을 처리하도록 수정
- `Actor` 클래스는 `Unit`의 베이스로 활용 가능
- `Renderer`는 그리드 렌더링을 지원하도록 확장
- 기존 `Timer` 유틸리티는 애니메이션에 활용 가능
- 기존 `Input` 시스템을 확장하여 마우스 클릭을 그리드 좌표로 변환

### 입력 처리 설계
- **주요 입력**: 마우스 클릭 (유닛 선택, 타일 선택, 메뉴 선택)
- **보조 입력**: 키보드 (메뉴 탐색, 단축키)
- 모든 키보드 액션은 마우스로도 가능하도록 구현
- UI 요소는 클릭 가능한 영역을 명확히 표시

### 맵 데이터 형식
- 텍스트 파일 기반 (`.txt`)
- 파싱하기 쉬운 간단한 포맷
- 예시:
  ```
  WIDTH 10
  HEIGHT 10
  TILES
  PPPPPPPPPP
  PFFFPPPFFP
  PPPPMMPPPP
  ...
  UNITS
  PLAYER 2 8 LORD Iron_Sword
  ENEMY 7 2 KNIGHT Iron_Lance
  VICTORY DEFEAT_ALL_ENEMIES
  ```

### 최소한의 클론 원칙
- **스토리/대화 없음**: 게임플레이에만 집중
- **클래스 체인지 없음**: 유닛은 레벨업만 가능
- **2개 맵만**: 튜토리얼 + 메인 맵
- **핵심 메커니즘**: 이동, 전투, 턴 관리, AI에 집중
