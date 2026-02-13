# 디버그 노트 (Debug Notes)

이 문서는 게임 개발 중 발생한 버그의 원인과 해결 방법을 정리한 것입니다.  
게임 개발 초보자도 이해할 수 있도록 단계별로 설명합니다.

---

## 이벤트 로그 텍스트가 깨져 보이는 현상 (해결됨)

### 1. 현상

이벤트 로그 패널에 표시되는 문자가 깨지거나, 의미 없는 글자(쓰레기 값)가 보이는 문제가 있었습니다.

---

### 2. 게임이 화면을 그리는 순서 (배경 지식)

게임은 매 프레임마다 다음 순서로 동작합니다.

1. **업데이트 (Tick)**  
   캐릭터 이동, 입력 처리 등 로직을 처리합니다.
2. **그리기 요청 (Draw)**  
   레벨·UI 등이 “이런 글자/그림을 이 좌표에 그려줘”라고 **요청**만 합니다.  
   이때 `Renderer::Submit(글자, 위치, 색, ...)` 를 호출합니다.
3. **실제 그리기 (Renderer::Draw)**  
   모아둔 요청을 한꺼번에 처리해서, 그제서야 콘솔 화면에 출력합니다.

즉, **Submit 시점**과 **실제로 화면에 그려지는 시점**이 한 프레임 안에서 **시간적으로 떨어져 있습니다.**

---

### 3. Submit은 “글자 내용”이 아니라 “위치(주소)”만 기억한다

`Renderer::Submit(const char* text, ...)` 에서 `text` 는 **포인터**입니다.

- **포인터** = 메모리 속에 있는 “실제 문자열 데이터”의 **위치(주소)** 를 가리키는 값입니다.
- 렌더러는 문자열을 **복사해서 저장하지 않고**, “나중에 이 주소에 가서 읽을 거다”라고 **주소만** 큐(render queue)에 넣어 둡니다.
- 따라서 **나중에 그 주소에 있는 데이터가 그대로 유효해야** 올바른 글이 그려집니다.

---

### 4. 문제의 코드에서 무슨 일이 일어났는지

이벤트 로그를 줄 단위로 나누어(래핑) 그릴 때, 대략 다음과 같이 작성되어 있었습니다.

```cpp
while (pos < msg.length())
{
    std::string line = msg.substr(pos, len);   // ← "임시" 문자열 생성
    Renderer::Get().Submit(line.c_str(), ...); // ← 이 줄의 "주소"를 큐에 등록
    // ← 여기서 반복이 끝나면 line은 소멸합니다!
}
```

- `msg.substr(pos, len)` 은 **그 순간만 쓰고 버릴** 새 문자열을 만듭니다.  
  이런 것을 **임시 객체(temporary)** 라고 부릅니다.
- `line.c_str()` 은 “지금 이 순간 `line` 이 차지하고 있는 메모리 주소”를 넘겨 줍니다.
- **반복이 한 번 끝날 때마다** `line` 변수는 **소멸**하고, 그 메모리는 다시 시스템에게 돌아갑니다.  
  즉, 방금 Submit에 넘겼던 **그 주소에 있던 문자열은 이미 없어진 상태**가 됩니다.

이렇게 “이미 없어진 메모리를 가리키는 포인터”를 **댕글링 포인터(dangling pointer)** 라고 합니다.

---

### 5. 왜 “나중에” 문제가 되는가?

실제 그리기는 **모든 Draw(그리기 요청)가 끝난 뒤**에 일어납니다.

- `mainLevel->Draw()` 가 실행되면서 `DrawLogPanel()` 이 호출되고,  
  위의 while 문에서 여러 번 `Submit(line.c_str(), ...)` 가 호출됩니다.
- 그 직후 while 문이 끝나면서 **모든 `line` 이 소멸**합니다.
- 그 다음에 `renderer->Draw()` 가 호출되면, 렌더러는 큐에 쌓인 **주소들**을 보고 “그 주소에 가서 글자를 읽자” 하고 접근합니다.
- 하지만 그 시점에는 **그 주소에 있던 문자열은 이미 없어졌기 때문에**, 다른 값이 들어 있거나 쓰레기 메모리를 읽게 되고, **화면에는 깨진 글자**가 나옵니다.

정리하면:

- **Submit할 때** 넘긴 주소는 “그 순간만” 유효한 임시 문자열을 가리키고 있었고,
- **실제로 그릴 때**는 그 메모리가 이미 파괴된 뒤라서,  
  → **댕글링 포인터**를 읽는 것이고, 그래서 로그 텍스트가 깨져 보였습니다.

---

### 6. 해결 방법 (수명을 늘려 주기)

핵심 아이디어는 하나입니다.

- **“실제로 그릴 때까지”** 그 문자열이 **메모리에 살아 있어야** 합니다.
- 그래서 “임시 변수”에 넣지 말고, **오래 사는 저장소**에 문자열을 넣어 두고, 그 저장소에 있는 문자열의 주소를 Submit에 넘깁니다.

구체적인 수정:

1. **BattleLevel** 에 **멤버 변수**를 하나 추가했습니다.  
   `std::vector<std::string> wrappedLogLines;`  
   → 이 벡터는 `Draw()` 한 번이 끝날 때까지 살아 있으므로, 여기에 넣은 문자열은 `renderer->Draw()` 가 실행되는 순간까지 **유효**합니다.

2. **DrawLogPanel()** 안에서:
   - 매번 그리기 전에 `wrappedLogLines.clear()` 로 이전 프레임 내용을 비우고,
   - 줄 단위로 나눈 문자열을 **임시 변수에 넣지 말고** `wrappedLogLines.push_back(msg.substr(pos, len))` 처럼 **벡터에 추가**합니다.
   - 모든 줄을 벡터에 다 넣은 뒤에,  
     `Renderer::Get().Submit(wrappedLogLines[i].c_str(), ...)` 처럼 **벡터에 들어 있는 문자열의 주소**를 Submit에 넘깁니다.

이렇게 하면:

- Submit에 넘기는 주소는 **벡터가 들고 있는 문자열**을 가리키고,
- 그 벡터는 `mainLevel->Draw()` 가 끝나고 `renderer->Draw()` 가 실행될 때까지 **그대로 유지**되므로,
- 렌더러가 나중에 그 주소를 읽을 때도 **아직 유효한 문자열**이 있어서, 로그 텍스트가 정상적으로 그려집니다.

---

### 7. 초보자를 위한 한 줄 요약

- **원인:** “잠깐만 쓰고 버리는 문자열”의 주소를 Submit에 넘겼기 때문에, 실제로 그릴 때는 그 메모리가 이미 없어져서 **댕글링 포인터**가 되었고, 그래서 글이 깨져 보였습니다.
- **해결:** 줄 단위 문자열을 **프레임이 끝날 때까지 살아 있는 벡터(`wrappedLogLines`)** 에 넣어 두고, 그 벡터에 있는 문자열의 주소만 Submit에 넘기도록 바꿨습니다.

---

### 8. 수정된 파일

- **Game/Level/BattleLevel.h**  
  - 멤버 변수 `wrappedLogLines` 추가.
- **Game/Level/BattleLevel.cpp**  
  - `DrawLogPanel()` 에서 래핑된 줄을 `wrappedLogLines` 에 채운 뒤, 그 벡터의 `.c_str()` 만 Submit에 전달하도록 변경.

---

## 유닛 번호(맵 표시)가 깨져 보이는 현상 (해결됨)

### 1. 현상

맵 위 유닛을 2x2 문자 블록으로 그릴 때, **우하단 칸에 표시하는 유닛 번호**(1~9, 0)가 깨지거나 쓰레기 문자가 보이는 문제가 있었습니다.  
(char와 int 변환 오류로 오해하기 쉬우나, 원인은 아래와 같습니다.)

---

### 2. 원인: 로그 패널과 같은 “댕글링 포인터”

`Renderer::Submit(const char* text, ...)` 는 **포인터만** 저장하고, 실제 그리기는 나중에 `Renderer::Draw()` 에서 일괄 수행합니다.

유닛 번호를 그릴 때 다음과 같이 **함수 안의 지역 변수** 버퍼를 넘기고 있었습니다.

```cpp
void Unit::Draw()
{
    // ...
    char numberStr[2] = { '0', '\0' };   // ← 지역 변수 (Draw() 안에서만 유효)
    if (useNumberInCorner)
        numberStr[0] = (unitIndex == 9) ? '0' : static_cast<char>('1' + unitIndex);
    // ...
    Renderer::Get().Submit(numberStr, Vector2(...), color, 10);  // ← 주소만 저장
}  // ← 함수가 끝나면 numberStr 소멸 → 그 주소는 댕글링 포인터
```

- `numberStr` 은 **Draw()가 끝날 때** 소멸합니다.
- Submit 시점에는 유효한 문자열이 있지만, **실제로 그리는 시점**에는 그 메모리가 이미 없어져 있어, 렌더러가 읽을 때 쓰레기 값이 나와 번호가 깨져 보였습니다.

즉, **char vs int 타입 문제가 아니라**, “잠깐만 유효한 지역 버퍼의 주소를 Submit에 넘겨서, 나중에 그릴 때는 이미 없어진 메모리를 가리키는 **댕글링 포인터**” 문제입니다.

---

### 3. 해결 방법

Submit에 넘기는 문자열이 **실제로 그릴 때까지 유효하도록**, **유닛 객체의 멤버 변수**에 번호용 버퍼를 두고 그 주소를 넘깁니다.

- **Unit.h**  
  - `char unitNumberDisplay[2];` 멤버 추가. (유닛이 살아 있는 동안 유효.)
- **Unit.cpp Draw()**  
  - `numberStr` 대신 `unitNumberDisplay` 에 번호 문자를 채운 뒤,  
  - `Renderer::Get().Submit(unitNumberDisplay, ...)` 로 전달.

이렇게 하면 Submit이 저장하는 포인터가 **Unit 객체의 멤버**를 가리키므로, `Draw()` 가 끝난 뒤에도 그 주소는 유효하고, 렌더러가 그릴 때 올바른 문자가 표시됩니다.

---

### 4. 수정된 파일

- **Game/Unit/Unit.h**  
  - 멤버 변수 `unitNumberDisplay[2]` 추가.
- **Game/Unit/Unit.cpp**  
  - `Draw()` 에서 지역 버퍼 `numberStr` 제거, `unitNumberDisplay` 에 쓰고 그 포인터를 Submit에 전달.

---

## 게임 종료 시 크래시 (해결됨)

### 1. 현상

전투로 아군 또는 적군이 전멸(또는 로드 전투 불능)이 되어 게임 종료 조건이 만족될 때, `TriggerGameOver()`가 호출된 직후 **프로그램이 크래시**하는 현상이 있었습니다.

---

### 2. 원인: 게임 종료 처리 후 이미 널로 만든 포인터 사용

`TriggerGameOver()` 안에서는 다음을 수행합니다.

- 모든 플레이어/적 유닛에 대해 타일에서 유닛 제거, `Destroy()` 호출
- `playerUnits`, `enemyUnits` 벡터 비우기
- **`selectedUnit`, `unitPendingAttack`, `attackTarget` 등을 `nullptr`로 설정**

그런데 `TriggerGameOver()`는 **전투 중**에 호출됩니다. 예를 들어:

- **이동 후 공격**: `ProcessPendingAttackAfterMove()` → `PerformCombat()` → (적 전멸 등으로) `CheckEndConditions()` → `TriggerGameOver()`  
  이때 `TriggerGameOver()`가 `unitPendingAttack`과 `attackTarget`을 `nullptr`로 만듭니다.
- **함수 반환 순서**: `TriggerGameOver()` 반환 → `CheckEndConditions()` 반환 → `PerformCombat()` 반환 → **`ProcessPendingAttackAfterMove()`로 복귀**
- `ProcessPendingAttackAfterMove()`에서는 그다음에 **`unitPendingAttack->EndTurn()`** 을 호출합니다.  
  이미 `unitPendingAttack`은 `nullptr`이므로 **null 포인터 역참조**로 크래시가 발생합니다.

비슷하게, **인접 적을 클릭해 공격**할 때 `OnMouseClick()` 안에서 `PerformCombat()` 호출 후 게임 종료가 되면, `selectedUnit`이 `TriggerGameOver()`에서 `nullptr`로 바뀐 뒤 같은 프레임에 `selectedUnit->EndTurn()`을 호출하게 되어 크래시가 날 수 있습니다.

즉, **게임 종료 시 포인터를 먼저 널로 만들었는데, 그 포인터를 아직 사용하는 코드 경로가 남아 있었던 것**이 원인입니다.

---

### 3. 해결 방법

`PerformCombat()`(및 그 안에서 호출되는 `CheckEndConditions()` / `TriggerGameOver()`)를 호출한 **직후**, 해당 포인터를 쓰기 전에 **게임 종료 여부를 확인**하고, 종료되었으면 더 이상 진행하지 않습니다.

- **ProcessPendingAttackAfterMove()**  
  `PerformCombat(unitPendingAttack, attackTarget)` 호출 직후  
  **`if (gameOver) return;`**  
  을 넣어, 게임 종료 시 `unitPendingAttack->EndTurn()` 등에 진입하지 않도록 합니다.
- **OnMouseClick()** (인접 적 클릭 시 즉시 공격하는 분기)  
  `PerformCombat(selectedUnit, clickedEnemy)` 호출 직후  
  **`if (gameOver) return;`**  
  을 넣어, 게임 종료 시 `selectedUnit->EndTurn()` 등에 진입하지 않도록 합니다.

이렇게 하면 `TriggerGameOver()`에서 포인터를 널로 만든 뒤에도, 그 포인터를 사용하는 코드가 실행되지 않아 크래시가 사라집니다.

---

### 4. 추가로 적용한 안전 처리

- **Draw()**  
  - `gameOver`일 때는 적 유닛 루프(색상 갱신)를 돌지 않도록 `if (!gameOver)` 로 감쌌습니다.  
  - 게임 종료 시에는 유닛 액터를 그리지 않도록 `if (!gameOver)` 일 때만 `Level::Draw()`를 호출하여, 파괴 예정인 유닛을 그리지 않습니다.
- 게임 종료 후에도 **엔진 루프는 계속 돌아가며**, 입력·AI·턴 로직만 `Tick()` 앞단의 `if (gameOver) return;` 으로 건너뜁니다.  
  화면에는 정리된 맵과 승리 문구만 남겨 두어, 플레이어에게 결과를 보여 줍니다.

---

### 5. 수정된 파일

- **Game/Level/BattleLevel.cpp**  
  - `ProcessPendingAttackAfterMove()`: `PerformCombat()` 호출 직후 `if (gameOver) return;` 추가.  
  - `OnMouseClick()` (인접 적 공격 분기): `PerformCombat()` 호출 직후 `if (gameOver) return;` 추가.  
  - `Draw()`: `gameOver`일 때 적 유닛 루프 생략, `gameOver`일 때 `Level::Draw()` 생략.
## 프로젝트 진행 중 겪었던 어려움 (요약)

1. **콘솔 UTF-8/유니코드 출력**  
   CP437·wchar_t 시도 시 문자가 깨짐. 해결: 문자열은 UTF-8 `char*`로 유지하고, Renderer에서 `MultiByteToWideChar`로 UTF-16 변환 후 `WriteConsoleOutputW` 사용. 콘솔 코드 페이지 및 가상 터미널 설정 필요.

2. **유닛 스탯 패널 일부 미표시**  
   `DrawStatsPanel`에서 지역 변수 버퍼 포인터를 `Renderer::Submit`에 넘기면, 프레임 종료 후 포인터가 무효화되어 깨진 문자열 출력. 해결: 클래스 멤버 `uiBuffers[12][64]`에 동적 텍스트 저장 후 포인터 전달.

3. **마우스 클릭 시 유닛 미이동**  
   - `Input::ProcessInput`에서 `mousePosition.y`를 설정하지 않아 Y 좌표가 0으로 고정됨.  
   - 이동 직후 `OnMouseClick`에서 `EndTurn()` 호출로 상태가 Done으로 바뀌어 시각적 이동이 끝나기 전에 턴 종료.  
   해결: 마우스 Y 갱신 추가, 이동 완료 시에만 `UpdateMovement`에서 state를 Done으로 변경.

4. **턴이 입력 없이 자동 진행**  
   `HandleInput` 안에 조건 없는 블록에서 `isPlayerTurn`을 매 프레임 토글하고 있음. 해결: 해당 블록 제거, SPACE 키 입력 시에만 턴 전환.

5. **유닛이 화면에서 움직이지 않음**  
   유닛 렌더링이 `Actor::position` 기준으로만 되어 있고, 이동 시 `gridPosition`만 바뀌어 화면 위치가 갱신되지 않음. 해결: `Unit::Draw`는 `gridPosition`을 2x2 화면 좌표로 변환해 그리며, `AddUnit`/`BeginPlay`에서 `SetPosition` 호출 제거.

6. **네임스페이스/API 리팩터링**  
   `Wanted` → `FEClone`, `WANTED_API` → `FECLONE_API` 전환 시 엔진·게임 전반 참조 수정. Game 쪽 클래스에 DLL API 매크로가 붙어 링크 불일치(C4273) 발생 → Game 프로젝트 클래스에서는 매크로 제거.

7. **2x2 타일 표시 문자열 초기화**  
   `Tile::displayStr`을 `char[4][8]`로 바꾼 뒤 생성자 초기화 리스트에 `displayStr(".")`를 두면 C2075(brace-enclosed initializer list 필요) 발생. 해결: 초기화 리스트에서 제거하고, 생성자 본문에서 `strcpy_s`로 네 개의 행 각각 초기화.

8. **좌표·UI 레이아웃**  
   2x2 멀티라인 적용 후 그리드 좌표와 화면 좌표 불일치. 마우스 클릭은 `(x-1)/2`, `(y-1)/2`로 그리드로 변환. 스탯 패널·툴팁 위치를 `grid*2` 기준으로 재계산해 겹침 방지.
