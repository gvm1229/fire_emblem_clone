# FireEmblemClone 프로젝트 구현 상태

## 프로젝트 개요
Fire Emblem의 핵심 게임플레이를 DOS 콘솔 환경에서 구현한 턴제 전략 게임입니다.

**네임스페이스**: `FE` (FireEmblem의 약자)

## 최근 변경사항
- ✅ 네임스페이스 `Wanted` → `FE` 변경 (2024)
- ✅ 사용하지 않는 파일 33개 삭제 (슈팅 게임, Sokoban 관련)
- ✅ 프로젝트 구조 정리 및 문서화

## 구현 완료된 기능

### Phase 1: 엔진 확장 (그리드 시스템)
✅ **Grid 시스템** (`Engine/Map/Grid.h/cpp`)
- 2D 그리드 관리
- 타일 접근 및 설정
- 좌표 유효성 검사

✅ **Tile 시스템** (`Engine/Map/Tile.h/cpp`)
- 지형 타입 (Plain, Forest, Mountain, Castle, Village, Water)
- 지형 효과 (방어 보너스, 회피 보너스, 이동 비용)
- ASCII 문자로 표현

✅ **입력 시스템 확장** (`Engine/Core/Input.h/cpp`)
- 스크린 좌표 → 그리드 좌표 변환
- 그리드 클릭 감지

### Phase 2: 유닛 시스템
✅ **UnitClass 시스템** (`Game/Unit/UnitClass.h/cpp`)
- 유닛 클래스 타입 (Lord, Cavalier, Knight, Archer, Mage, Fighter, Mercenary)
- 유닛 소속 (Player, Enemy, Ally)
- 이동 타입 (Infantry, Cavalry, Flying)
- 클래스별 기본 스탯 및 성장률

✅ **Unit 클래스** (`Game/Unit/Unit.h/cpp`)
- Actor 상속
- 스탯 시스템 (HP, Str, Mag, Skl, Spd, Lck, Def, Res, Mov)
- 레벨 및 경험치 시스템
- 레벨업 (랜덤 스탯 성장)
- 그리드 좌표 관리
- 이동 및 데미지 처리

### Phase 3: 전투 시스템
✅ **Weapon 시스템** (`Game/Item/Weapon.h/cpp`)
- 무기 타입 (Sword, Lance, Axe, Bow, Magic)
- 무기 스탯 (공격력, 명중률, 크리티컬률, 무게, 사거리)
- 무기 내구도 관리
- 기본 무기 생성 함수

✅ **CombatCalculator** (`Game/Combat/CombatCalculator.h/cpp`)
- 명중률 계산
- 데미지 계산
- 크리티컬 확률 계산
- 더블 어택 판정
- 무기 삼각 관계 (Sword > Axe > Lance > Sword)
- 반격 가능 여부 (사거리 체크)

✅ **CombatSequence** (`Game/Combat/CombatSequence.h/cpp`)
- 전투 시작 및 실행
- 공격, 반격, 더블 어택 처리
- 명중 판정 (난수 기반)
- 크리티컬 판정
- 경험치 획득

### Phase 4: 턴 관리 시스템
✅ **TurnManager** (`Game/System/TurnManager.h/cpp`)
- 플레이어 페이즈 / 적 페이즈 관리
- 턴 카운터
- 유닛 행동 플래그 리셋

✅ **PlayerPhaseController** (`Game/System/PlayerPhaseController.h/cpp`)
- 마우스 클릭으로 유닛 선택
- 이동 가능 범위 계산 및 표시
- 마우스 클릭으로 이동 위치 선택
- 행동 메뉴 (Attack, Wait)
- 공격 대상 선택
- 플레이어 페이즈 종료 체크

✅ **EnemyPhaseController** (`Game/System/EnemyPhaseController.h/cpp`)
- 적 페이즈 자동 실행
- 모든 적 유닛 AI 실행

### Phase 5: AI 시스템
✅ **AIController** (`Game/AI/AIController.h/cpp`)
- 가장 가까운 적 찾기
- 공격 가능하면 즉시 공격
- 공격 불가능하면 적에게 접근
- 최적의 이동 위치 계산

### Phase 6: 맵 및 레벨 시스템
✅ **MapLevel** (`Game/Level/MapLevel.h/cpp`)
- Grid 통합
- 맵 파일 로딩 (.txt 파싱)
- 유닛 생성 및 배치
- 승리/패배 조건 체크
- TurnManager 및 PhaseController 통합

✅ **맵 데이터**
- `Assets/Tutorial.txt`: 10x10 튜토리얼 맵 (플레이어 1 vs 적 1)
- `Assets/MainMap.txt`: 20x15 메인 맵 (플레이어 4 + 아군 1 vs 적 6)

### Phase 7: 게임 진행
✅ **Main.cpp**
- 게임 시작
- 튜토리얼 맵 로드
- 조작법 안내

## 주요 게임 메커니즘

### 턴제 전투 흐름
1. **플레이어 페이즈**
   - 마우스로 유닛 클릭하여 선택
   - 이동 가능 범위 표시
   - 마우스로 이동 위치 클릭
   - 행동 메뉴에서 Attack 또는 Wait 선택 (키보드 A/W)
   - 공격 선택 시 대상 클릭
   - 모든 유닛 행동 완료 후 SPACE로 턴 종료

2. **적 페이즈**
   - 모든 적 유닛이 AI로 자동 행동
   - 가장 가까운 플레이어 유닛 공격
   - 아무 키나 눌러서 다음 플레이어 페이즈로

3. **전투 시스템**
   - 공격자 공격 → 방어자 반격 (사거리 내) → 더블 어택 (Spd 차이 4 이상)
   - 명중 판정 (난수 기반)
   - 크리티컬 판정 (데미지 3배)
   - 경험치 획득 (전투 10 EXP, 처치 30 EXP)
   - 레벨업 (경험치 100 도달 시)

### 스탯 시스템
- **HP**: 체력
- **Str**: 물리 공격력
- **Mag**: 마법 공격력
- **Skl**: 명중률 및 크리티컬에 영향
- **Spd**: 회피율 및 더블 어택에 영향
- **Lck**: 명중/회피/크리티컬에 영향
- **Def**: 물리 방어력
- **Res**: 마법 방어력
- **Mov**: 이동력

### 전투 계산 공식
- **명중률** = 무기 명중 + (Skl × 2) + (Lck / 2) - 적 회피율
- **회피율** = (Spd × 2) + Lck + 지형 보너스
- **데미지** = (Str or Mag + 무기 공격력) - 적 방어력
- **크리티컬률** = 무기 크리티컬 + (Skl / 2) - 적 Lck
- **더블 어택** = Spd 차이 4 이상

### 무기 삼각 관계
- Sword > Axe (명중률 +15)
- Axe > Lance (명중률 +15)
- Lance > Sword (명중률 +15)

## 입력 시스템

### 마우스 (주요)
- 유닛 선택
- 이동 위치 선택
- 공격 대상 선택

### 키보드 (보조)
- **A**: Attack 선택
- **W**: Wait 선택
- **SPACE**: 플레이어 턴 종료
- **ESC**: 취소
- **임의 키**: 적 페이즈 종료

## 파일 구조

### Engine (엔진 확장)
```
Engine/
├─ Map/
│  ├─ Grid.h/cpp           # 그리드 시스템
│  └─ Tile.h/cpp           # 타일 및 지형
├─ Core/
│  └─ Input.h/cpp          # 입력 시스템 (마우스→그리드 변환 추가)
```

### Game (게임 로직)
```
Game/
├─ Unit/
│  ├─ UnitClass.h/cpp      # 유닛 클래스 정의
│  └─ Unit.h/cpp           # 유닛 클래스
├─ Item/
│  └─ Weapon.h/cpp         # 무기 시스템
├─ Combat/
│  ├─ CombatCalculator.h/cpp  # 전투 계산
│  └─ CombatSequence.h/cpp    # 전투 실행
├─ AI/
│  └─ AIController.h/cpp   # AI 로직
├─ System/
│  ├─ TurnManager.h/cpp    # 턴 관리
│  ├─ PlayerPhaseController.h/cpp  # 플레이어 입력 처리
│  └─ EnemyPhaseController.h/cpp   # 적 AI 실행
├─ Level/
│  └─ MapLevel.h/cpp       # 맵 레벨 (통합)
└─ Main.cpp                # 게임 시작점
```

### Assets (맵 데이터)
```
Assets/
├─ Tutorial.txt            # 튜토리얼 맵 (10x10)
└─ MainMap.txt             # 메인 맵 (20x15)
```

## 빌드 방법

### 필요한 작업
1. **Engine.vcxproj**에 새 파일 추가:
   - `Engine/Map/Grid.cpp`
   - `Engine/Map/Tile.cpp`
   - `Engine/Core/Input.cpp` (수정됨)

2. **Game.vcxproj**에 새 파일 추가:
   - `Game/Unit/UnitClass.cpp`
   - `Game/Unit/Unit.cpp`
   - `Game/Item/Weapon.cpp`
   - `Game/Combat/CombatCalculator.cpp`
   - `Game/Combat/CombatSequence.cpp`
   - `Game/AI/AIController.cpp`
   - `Game/System/TurnManager.cpp`
   - `Game/System/PlayerPhaseController.cpp`
   - `Game/System/EnemyPhaseController.cpp`
   - `Game/Level/MapLevel.cpp`
   - `Game/Main.cpp` (수정됨)

3. Visual Studio에서 솔루션 빌드
4. Assets 폴더가 실행 파일과 같은 위치에 있는지 확인

## 알려진 제한사항

### 구현되지 않은 기능
- ❌ ANSI Art (색상 없이 ASCII만)
- ❌ Pathfinding (이동은 직선으로만)
- ❌ 지형 이동 비용 (모든 유닛이 동일하게 이동)
- ❌ 복잡한 AI 패턴 (기본 공격형만)
- ❌ 인벤토리 시스템 (유닛당 1개 무기만)
- ❌ 아이템 사용
- ❌ 거래 시스템
- ❌ 맵 간 유닛 유지 (각 맵이 독립적)

### 간소화된 부분
- 이동 가능 범위: 맨해튼 거리로 계산 (지형 비용 무시)
- UI: 콘솔 텍스트만 (그래픽 없음)
- 맵 전환: 수동으로 Main.cpp 수정 필요

## 플레이 가이드

### 튜토리얼 맵
1. 플레이어 Lord (L)가 (2, 7)에 시작
2. 적 Fighter (F)가 (7, 2)에 시작
3. Lord를 클릭하여 선택
4. 이동할 위치를 클릭
5. A를 눌러 Attack 선택
6. 적을 클릭하여 공격
7. 적을 처치하면 승리

### 메인 맵
1. 플레이어 유닛 4명 + 아군 1명
2. 적 유닛 6명
3. 모든 적을 처치하면 승리
4. 모든 플레이어 유닛이 사망하면 패배

## 향후 개선 사항 (선택)
- ANSI Art 적용 (색상 추가)
- Pathfinding (BFS/Dijkstra)
- 복잡한 AI 패턴
- 인벤토리 및 아이템 시스템
- 지형 효과 완전 구현
- 맵 간 전환 시스템
- 세이브/로드 기능
