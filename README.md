# Fire Emblem Clone

DOS 콘솔 환경에서 구현한 턴제 전략 게임 - Fire Emblem의 핵심 게임플레이 클론

## 프로젝트 개요

이 프로젝트는 1990년 패미컴에서 출시된 "Fire Emblem: Shadow Dragon and the Blade of Light"의 핵심 게임 메커니즘을 C++ 콘솔 환경에서 재구현한 것입니다.

**네임스페이스**: `FE` (FireEmblem)

> **최근 업데이트**: 프로젝트 정리 및 네임스페이스 변경 완료. 자세한 내용은 [CLEANUP_SUMMARY.md](CLEANUP_SUMMARY.md) 참조.

### 주요 특징
- ✅ 턴제 전략 전투 시스템
- ✅ 그리드 기반 맵
- ✅ 다양한 유닛 클래스 (Lord, Cavalier, Knight, Archer, Mage, Fighter, Mercenary)
- ✅ 전투 시스템 (명중률, 데미지, 크리티컬, 더블 어택)
- ✅ 레벨업 및 경험치 시스템
- ✅ AI 적 유닛
- ✅ 무기 삼각 관계 (Sword > Axe > Lance > Sword)
- ✅ 지형 효과 (방어, 회피 보너스)
- ✅ 2개의 맵 (튜토리얼 + 메인)

## 빠른 시작

### 요구사항
- Windows 10/11
- Visual Studio 2019 이상
- C++17 이상

### 빌드 및 실행
1. `FireEmblemClone.sln`을 Visual Studio에서 열기
2. 새로 추가된 파일들을 프로젝트에 포함 (BUILD_GUIDE.md 참조)
3. 솔루션 빌드 (Ctrl+Shift+B)
4. Game 프로젝트를 시작 프로젝트로 설정
5. 실행 (F5 또는 Ctrl+F5)

자세한 빌드 방법은 [BUILD_GUIDE.md](BUILD_GUIDE.md)를 참조하세요.

## 게임 조작법

### 마우스 (주요 입력)
- **유닛 클릭**: 플레이어 유닛 선택
- **타일 클릭**: 이동 위치 선택
- **적 클릭**: 공격 대상 선택

### 키보드 (보조 입력)
- **A**: Attack (공격)
- **W**: Wait (대기)
- **SPACE**: 플레이어 턴 종료
- **ESC**: 취소
- **임의 키**: 적 페이즈 진행

## 게임 플레이

### 기본 흐름
1. **플레이어 페이즈**
   - 자신의 유닛을 클릭하여 선택
   - 이동 가능 범위가 표시됨
   - 이동할 위치를 클릭
   - 행동 메뉴에서 Attack 또는 Wait 선택
   - Attack 선택 시 공격할 적을 클릭
   - 모든 유닛 행동 완료 후 SPACE로 턴 종료

2. **적 페이즈**
   - 적 유닛들이 자동으로 행동
   - AI가 가장 가까운 플레이어 유닛을 공격
   - 아무 키나 눌러서 다음 플레이어 페이즈로

3. **전투**
   - 공격자 → 방어자 반격 (사거리 내) → 더블 어택 (Spd 차이 4 이상)
   - 명중 판정 및 크리티컬 판정 (난수 기반)
   - 경험치 획득 및 레벨업

### 승리/패배 조건
- **승리**: 모든 적 유닛 제거
- **패배**: 모든 플레이어 유닛 사망

## 게임 시스템

### 유닛 스탯
- **HP**: 체력
- **Str**: 물리 공격력
- **Mag**: 마법 공격력
- **Skl**: 명중률, 크리티컬에 영향
- **Spd**: 회피율, 더블 어택에 영향
- **Lck**: 명중/회피/크리티컬에 영향
- **Def**: 물리 방어력
- **Res**: 마법 방어력
- **Mov**: 이동력

### 전투 계산
- **명중률** = 무기 명중 + (Skl × 2) + (Lck / 2) - 적 회피율
- **데미지** = (Str or Mag + 무기 위력) - 적 방어력
- **크리티컬** = 무기 크리티컬 + (Skl / 2) - 적 Lck (성공 시 데미지 3배)
- **더블 어택** = 자신의 Spd가 적보다 4 이상 높으면 2회 공격

### 무기 삼각 관계
- Sword > Axe (명중률 +15)
- Axe > Lance (명중률 +15)
- Lance > Sword (명중률 +15)

### 지형 효과
- **평지 (.)**: 기본 지형
- **숲 (T)**: 회피 +20, 방어 +1
- **산 (^)**: 회피 +30, 방어 +2
- **성 (#)**: 회피 +20, 방어 +3
- **마을 (V)**: 회피 +10, 방어 +1
- **물 (~)**: 대부분 유닛 통과 불가

## 맵

### 튜토리얼 맵 (Tutorial.txt)
- 크기: 10x10
- 플레이어: Lord 1명
- 적: Fighter 1명
- 목적: 기본 조작 및 전투 학습

### 메인 맵 (MainMap.txt)
- 크기: 20x15
- 플레이어: Lord, Cavalier, Archer, Mage (4명)
- 아군: Knight 1명
- 적: Fighter, Archer, Cavalier, Mage, Knight, Mercenary (6명)
- 목적: 본격적인 전략 전투

## 프로젝트 구조

```
FireEmblemClone/
├─ Engine/              # 게임 엔진
│  ├─ Map/             # Grid 및 Tile 시스템
│  ├─ Core/            # 입력 처리
│  ├─ Render/          # 렌더링
│  └─ ...
├─ Game/               # 게임 로직
│  ├─ Unit/            # 유닛 시스템
│  ├─ Item/            # 무기 시스템
│  ├─ Combat/          # 전투 계산 및 실행
│  ├─ AI/              # AI 로직
│  ├─ System/          # 턴 관리 및 컨트롤러
│  ├─ Level/           # 맵 레벨
│  └─ Main.cpp         # 진입점
└─ Assets/             # 맵 데이터
   ├─ Tutorial.txt
   └─ MainMap.txt
```

## 문서

- [BUILD_GUIDE.md](BUILD_GUIDE.md) - 빌드 및 트러블슈팅 가이드
- [Context.md](Context.md) - 구현 상태 및 기술 문서
- [CLEANUP_SUMMARY.md](CLEANUP_SUMMARY.md) - 코드 정리 요약 (네임스페이스 변경 등)
- [TODO.md](TODO.md) - 구현 계획 (체크리스트)
- [Original_FireEmblem.md](Original_FireEmblem.md) - 원작 게임 분석
- [Optional_ANSI_Art.md](Optional_ANSI_Art.md) - ANSI Art 적용 가이드
- [LegacyContext.md](LegacyContext.md) - 이전 슈팅 게임 엔진 문서

## 알려진 제한사항

이 프로젝트는 **최소한의 클론**을 목표로 하므로 다음 기능들은 구현되지 않았습니다:

- ❌ ANSI Art (색상 없이 ASCII만)
- ❌ 스토리 및 대화 시스템
- ❌ 클래스 체인지
- ❌ 복잡한 AI 패턴 (기본 공격형만)
- ❌ 인벤토리 시스템 (유닛당 무기 1개만)
- ❌ 아이템 사용 및 거래
- ❌ Pathfinding (이동은 맨해튼 거리 기반)
- ❌ 세이브/로드 기능

## 기여

이 프로젝트는 학습 목적으로 만들어졌습니다. 개선 사항이나 버그를 발견하시면 이슈를 등록해 주세요.

## 라이선스

교육용 프로젝트입니다.

## 감사의 말

- 원작 "Fire Emblem: Shadow Dragon and the Blade of Light" (Intelligent Systems, Nintendo)

## 스크린샷 예시

```
튜토리얼 맵:
..........
..........
..T.....T.
..........
....##....
....##....
..........
..T.....T.
..........
..........

L = Lord (플레이어)
F = Fighter (적)
. = 평지
T = 숲
# = 성
```
