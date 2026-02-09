# Fire Emblem Clone

Original_FireEmblem.md를 기반으로 한 간단한 Fire Emblem 클론 게임입니다.

## 프로젝트 구조

```
FireEmblemClone/
├── Engine/              # 게임 엔진 (DLL)
│   ├── Actor/          # 액터 베이스 클래스
│   ├── Algorithm/      # A* 길찾기 알고리즘
│   ├── Common/         # 공통 헤더, RTTI
│   ├── Core/           # 입력 처리
│   ├── Engine/         # 엔진 메인 루프
│   ├── Level/          # 레벨 베이스 클래스
│   ├── Math/           # Vector2, Color 등
│   └── Render/         # 렌더러, 스크린 버퍼
│
├── Game/               # 게임 로직
│   ├── Level/          # BattleLevel (전투 맵)
│   ├── Map/            # Grid, Tile (지형 시스템)
│   ├── Unit/           # Unit, UnitStats (유닛 시스템)
│   └── System/         # MovementCalculator (이동 범위 계산)
│
├── Assets/             # 게임 리소스
│   └── BattleMap.txt   # 맵 데이터
│
└── Config/             # 설정 파일
    └── Setting.txt     # 해상도, 프레임레이트 설정
```

## 빌드 방법

1. Visual Studio 2022로 `FireEmblemClone.sln` 열기
2. 솔루션 구성을 `Debug` 또는 `Release`로 설정
3. 플랫폼을 `x64`로 설정
4. `빌드 > 솔루션 빌드` (Ctrl+Shift+B)

## 실행

빌드 후 `Bin\x64\Debug\Game\Game.exe` 실행

### ⚠️ 중요: 콘솔 폰트 설정

지형이 깨진 문자(翽, » 등)로 보인다면:
1. **콘솔 창 타이틀 바 우클릭** → **속성**
2. **글꼴** 탭 → **래스터 글꼴** 선택
3. **확인**

**왜 필요한가요?**
- 게임은 코드 페이지 437 (IBM PC ASCII)의 박스 드로잉 문자 사용
- 기본 폰트(MS 고딕, 굴림)는 이 문자들을 제대로 표시 못함
- 래스터 글꼴 또는 Consolas가 올바르게 표시

자세한 내용: `CONSOLE_SETUP.md` 참고

## 게임 조작법

### 기본 조작
- **숫자 키 0~9**: 플레이어 유닛 선택
- **마우스 클릭**: 선택된 유닛을 클릭한 위치로 이동
- **ESC**: 유닛 선택 해제
- **Enter**: 턴 종료 (플레이어 턴 ↔ 적 턴)

### 게임 흐름
1. 숫자 키로 유닛 선택
2. 노란색 `*`로 표시된 이동 가능 범위 확인
3. 마우스로 이동할 위치 클릭
4. 유닛이 자동으로 경로를 따라 이동
5. 모든 유닛 이동 완료 후 Enter로 턴 종료

## 구현된 기능

### 핵심 시스템
- ✅ **Grid & Tile 시스템**: 지형 타입별 이동 비용, 회피/방어 보너스
- ✅ **Unit 시스템**: 스탯(HP, STR, MAG, SKL, SPD, LCK, DEF, RES, MOV), 진영(Player/Enemy), 상태 관리
- ✅ **A* 경로 탐색**: AlgorithmPractice 참고, 장애물 회피 경로 계산
- ✅ **이동 범위 계산**: Dijkstra 알고리즘, 지형 비용 고려
- ✅ **턴 시스템**: 플레이어 턴 ↔ 적 턴
- ✅ **스탯 UI**: 선택된 유닛의 정보를 화면 오른쪽에 표시

### 지형 타입
- `.` Plain (평지): 이동 비용 1
- `T` Forest (숲): 이동 비용 2, 회피 +20
- `^` Mountain (산): 이동 비용 3, 회피 +30
- `#` Castle (성): 이동 비용 1, 회피 +20, 방어 +2, 체력 회복
- `H` Village (마을): 이동 비용 1, 회피 +10
- `~` Water (물): 통과 불가
- `X` Wall (벽): 통과 불가

## 미구현 기능

- ⏳ **전투 시스템**: 공격, 반격, 데미지 계산
- ⏳ **AI 시스템**: 적 유닛 자동 행동
- ⏳ **무기 시스템**: 무기 타입, 내구도, 무기 삼각관계
- ⏳ **경험치/레벨업**: 스탯 성장
- ⏳ **입력 모니터**: 디버깅용 입력 상태 표시

## 맵 파일 형식 (BattleMap.txt)

```
15 15
...............
...T.....T.....
..............#
.T.............
(... 15줄)
```

- 첫 줄: 너비 높이
- 이후: 각 타일의 지형 문자

## 설정 파일 (Config/Setting.txt)

```
framerate = 60
width = 70
height = 30
```

- `framerate`: 프레임레이트 (FPS)
- `width`: 콘솔 너비 (문자 단위)
- `height`: 콘솔 높이 (문자 단위)

## 개발 노트

- **Namespace**: `Wanted` (향후 `FE`로 변경 예정)
- **1칸 ASCII**: 현재는 1칸 ASCII 사용, 향후 멀티라인 ASCII 확장 가능하도록 구조 설계
- **최대 유닛 수**: 플레이어 유닛 최대 10개 (숫자 키 0~9)
- **부분 이동**: 이동력 6인 유닛을 2칸만 이동시킬 수 있음 (경로상 어디든 클릭 가능)

## 참고 자료

- `Original_FireEmblem.md`: Fire Emblem: Shadow Dragon and the Blade of Light 원작 메커니즘
