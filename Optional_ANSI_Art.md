# ANSI Art를 활용한 DOS 게임 비주얼 개선

## ANSI Art란?
ANSI Art는 ANSI 이스케이프 시퀀스를 사용하여 텍스트 터미널에서 색상, 커서 위치, 텍스트 스타일 등을 제어하는 기법입니다. DOS 및 콘솔 환경에서 ASCII 문자만으로는 표현하기 어려운 복잡한 그래픽을 구현할 수 있습니다.

## Windows 콘솔에서 ANSI 지원

### Windows 10/11 ANSI 이스케이프 시퀀스 활성화
Windows 10 버전 1511 이후부터 Windows 콘솔이 ANSI 이스케이프 시퀀스를 지원합니다. 활성화 방법:

```cpp
// Engine/Render/Renderer.cpp 또는 Engine/Engine/Engine.cpp에 추가

#include <windows.h>

void EnableVirtualTerminalProcessing()
{
    // stdout 핸들 가져오기
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return;
    }

    // 현재 콘솔 모드 가져오기
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return;
    }

    // ENABLE_VIRTUAL_TERMINAL_PROCESSING 플래그 추가
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        // ANSI 지원 실패 시 로그 출력
        printf("Warning: Failed to enable ANSI escape sequences.\n");
    }
}
```

엔진 초기화 시점에 호출:
```cpp
// Engine/Engine/Engine.cpp의 Initialize() 함수에서
void Engine::Initialize()
{
    EnableVirtualTerminalProcessing();
    // ... 기존 초기화 코드
}
```

## ANSI 이스케이프 시퀀스 기본

### 기본 문법
```
ESC[<파라미터>m
```
- `ESC`는 ASCII 코드 27 (0x1B)
- C++에서는 `\x1B` 또는 `\033`으로 표현

### 주요 ANSI 코드

#### 1. 색상 코드

**전경색 (텍스트 색상):**
```
30: 검은색
31: 빨간색
32: 초록색
33: 노란색
34: 파란색
35: 자홍색 (Magenta)
36: 청록색 (Cyan)
37: 흰색

90-97: 밝은 색상 (Bright/Bold)
```

**배경색:**
```
40: 검은색 배경
41: 빨간색 배경
42: 초록색 배경
43: 노란색 배경
44: 파란색 배경
45: 자홍색 배경
46: 청록색 배경
47: 흰색 배경

100-107: 밝은 배경색
```

**예시:**
```cpp
// 빨간색 텍스트
std::cout << "\x1B[31mRed Text\x1B[0m" << std::endl;

// 노란색 배경에 파란색 텍스트
std::cout << "\x1B[34;43mBlue on Yellow\x1B[0m" << std::endl;

// 밝은 녹색 텍스트
std::cout << "\x1B[92mBright Green\x1B[0m" << std::endl;
```

#### 2. 텍스트 스타일
```
0:  리셋 (모든 속성 초기화)
1:  굵게 (Bold)
2:  흐리게 (Dim)
3:  기울임 (Italic) - 지원 제한적
4:  밑줄 (Underline)
5:  깜빡임 (Blink) - 지원 제한적
7:  반전 (Reverse/Inverse) - 전경색과 배경색 반전
```

**예시:**
```cpp
// 굵은 빨간색
std::cout << "\x1B[1;31mBold Red\x1B[0m" << std::endl;

// 밑줄 친 파란색
std::cout << "\x1B[4;34mUnderlined Blue\x1B[0m" << std::endl;
```

#### 3. 커서 제어
```
\x1B[<y>;<x>H   : 커서를 (x, y) 위치로 이동 (1-based)
\x1B[<n>A       : 커서를 위로 n칸 이동
\x1B[<n>B       : 커서를 아래로 n칸 이동
\x1B[<n>C       : 커서를 오른쪽으로 n칸 이동
\x1B[<n>D       : 커서를 왼쪽으로 n칸 이동
\x1B[H          : 커서를 홈(0,0)으로 이동
\x1B[2J         : 화면 전체 지우기
\x1B[K          : 커서부터 줄 끝까지 지우기
```

**예시:**
```cpp
// 커서를 (10, 5) 위치로 이동
std::cout << "\x1B[5;10H" << "Hello at (10,5)";

// 화면 전체 지우기
std::cout << "\x1B[2J\x1B[H";
```

#### 4. 256색 및 RGB 색상 (고급)
```
\x1B[38;5;<n>m    : 전경색을 256색 팔레트의 n번 색상으로
\x1B[48;5;<n>m    : 배경색을 256색 팔레트의 n번 색상으로

\x1B[38;2;<r>;<g>;<b>m : 전경색을 RGB로 (r, g, b는 0-255)
\x1B[48;2;<r>;<g>;<b>m : 배경색을 RGB로
```

**예시:**
```cpp
// 256색 팔레트 - 색상 82 (밝은 초록)
std::cout << "\x1B[38;5;82mColor 82\x1B[0m" << std::endl;

// RGB 색상 - 주황색 (255, 165, 0)
std::cout << "\x1B[38;2;255;165;0mOrange\x1B[0m" << std::endl;
```

---

## FireEmblemClone에 ANSI Art 적용

### 1. ANSI 유틸리티 클래스 생성

`Engine/Render/ANSIUtil.h`:
```cpp
#ifndef ANSI_UTIL_H
#define ANSI_UTIL_H

#include <string>
#include <sstream>

// ANSI 이스케이프 시퀀스 유틸리티 클래스
class ANSIUtil
{
public:
    // 색상 enum
    enum class ForegroundColor
    {
        Black = 30,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37,
        BrightBlack = 90,
        BrightRed = 91,
        BrightGreen = 92,
        BrightYellow = 93,
        BrightBlue = 94,
        BrightMagenta = 95,
        BrightCyan = 96,
        BrightWhite = 97
    };

    enum class BackgroundColor
    {
        Black = 40,
        Red = 41,
        Green = 42,
        Yellow = 43,
        Blue = 44,
        Magenta = 45,
        Cyan = 46,
        White = 47,
        BrightBlack = 100,
        BrightRed = 101,
        BrightGreen = 102,
        BrightYellow = 103,
        BrightBlue = 104,
        BrightMagenta = 105,
        BrightCyan = 106,
        BrightWhite = 107
    };

    // 전경색 설정
    static std::string SetForeground(ForegroundColor color)
    {
        std::stringstream ss;
        ss << "\x1B[" << static_cast<int>(color) << "m";
        return ss.str();
    }

    // 배경색 설정
    static std::string SetBackground(BackgroundColor color)
    {
        std::stringstream ss;
        ss << "\x1B[" << static_cast<int>(color) << "m";
        return ss.str();
    }

    // 전경색 + 배경색 동시 설정
    static std::string SetColors(ForegroundColor fg, BackgroundColor bg)
    {
        std::stringstream ss;
        ss << "\x1B[" << static_cast<int>(fg) << ";" << static_cast<int>(bg) << "m";
        return ss.str();
    }

    // RGB 색상 (TrueColor 지원 터미널용)
    static std::string SetForegroundRGB(int r, int g, int b)
    {
        std::stringstream ss;
        ss << "\x1B[38;2;" << r << ";" << g << ";" << b << "m";
        return ss.str();
    }

    static std::string SetBackgroundRGB(int r, int g, int b)
    {
        std::stringstream ss;
        ss << "\x1B[48;2;" << r << ";" << g << ";" << b << "m";
        return ss.str();
    }

    // 스타일 설정
    static std::string Bold() { return "\x1B[1m"; }
    static std::string Dim() { return "\x1B[2m"; }
    static std::string Underline() { return "\x1B[4m"; }
    static std::string Reverse() { return "\x1B[7m"; }

    // 리셋
    static std::string Reset() { return "\x1B[0m"; }

    // 커서 제어
    static std::string CursorTo(int x, int y)
    {
        std::stringstream ss;
        ss << "\x1B[" << y << ";" << x << "H";
        return ss.str();
    }

    static std::string ClearScreen() { return "\x1B[2J\x1B[H"; }
    static std::string ClearLine() { return "\x1B[K"; }

    // 커서 숨김/표시
    static std::string HideCursor() { return "\x1B[?25l"; }
    static std::string ShowCursor() { return "\x1B[?25h"; }
};

#endif // ANSI_UTIL_H
```

### 2. 게임에 ANSI 적용 예시

#### 예시 1: 유닛 색상 구분
`Game/Unit/Unit.cpp`:
```cpp
#include "Engine/Render/ANSIUtil.h"

std::string Unit::GetDisplayString() const
{
    std::string result;

    // 소속에 따라 색상 구분
    switch (Faction)
    {
    case UnitFaction::Player:
        // 플레이어 유닛은 파란색
        result += ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::BrightBlue);
        break;
    case UnitFaction::Enemy:
        // 적 유닛은 빨간색
        result += ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::BrightRed);
        break;
    case UnitFaction::Ally:
        // 아군 NPC는 초록색
        result += ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::BrightGreen);
        break;
    }

    // 유닛 클래스에 따른 문자
    result += GetClassCharacter(); // 예: 'L' (Lord), 'C' (Cavalier)

    // 색상 리셋
    result += ANSIUtil::Reset();

    return result;
}
```

#### 예시 2: 지형 색상 표현
`Engine/Map/Tile.cpp`:
```cpp
std::string Tile::GetDisplayString() const
{
    std::string result;

    switch (TerrainType)
    {
    case TerrainType::Plain:
        // 평지: 밝은 녹색 배경
        result += ANSIUtil::SetBackground(ANSIUtil::BackgroundColor::Green);
        result += " "; // 빈 공간
        break;
    case TerrainType::Forest:
        // 숲: 어두운 녹색에 나무 문자
        result += ANSIUtil::SetColors(ANSIUtil::ForegroundColor::Green, 
                                       ANSIUtil::BackgroundColor::Black);
        result += "♣"; // 또는 'T' (Tree)
        break;
    case TerrainType::Mountain:
        // 산: 회색/갈색
        result += ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::White);
        result += "^"; // 산 모양
        break;
    case TerrainType::Castle:
        // 성: 노란색
        result += ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::BrightYellow);
        result += "♔"; // 또는 '#'
        break;
    case TerrainType::Water:
        // 물: 파란색
        result += ANSIUtil::SetColors(ANSIUtil::ForegroundColor::BrightBlue, 
                                       ANSIUtil::BackgroundColor::Blue);
        result += "≈"; // 물결
        break;
    }

    result += ANSIUtil::Reset();
    return result;
}
```

#### 예시 3: UI 하이라이트 (선택된 유닛, 이동 범위)
`Game/UI/RangeDisplay.cpp`:
```cpp
void RangeDisplay::DrawMoveRange(const std::vector<Vector2>& moveRange)
{
    for (const Vector2& pos : moveRange)
    {
        // 이동 가능 타일을 밝은 파란색 배경으로 하이라이트
        std::string highlight = ANSIUtil::SetBackground(ANSIUtil::BackgroundColor::BrightBlue);
        
        // 타일 위치로 커서 이동 후 출력
        std::cout << ANSIUtil::CursorTo(pos.X, pos.Y);
        std::cout << highlight << " " << ANSIUtil::Reset();
    }
}

void RangeDisplay::DrawAttackRange(const std::vector<Vector2>& attackRange)
{
    for (const Vector2& pos : attackRange)
    {
        // 공격 가능 타일을 빨간색 배경으로 하이라이트
        std::string highlight = ANSIUtil::SetBackground(ANSIUtil::BackgroundColor::Red);
        
        std::cout << ANSIUtil::CursorTo(pos.X, pos.Y);
        std::cout << highlight << " " << ANSIUtil::Reset();
    }
}
```

#### 예시 4: HP 바 표시
`Game/UI/UnitInfoPanel.cpp`:
```cpp
void UnitInfoPanel::DrawHealthBar(const Unit* unit)
{
    int hpPercentage = (unit->CurrentHP * 100) / unit->MaxHP;
    int barLength = 10; // HP 바 길이
    int filledBars = (hpPercentage * barLength) / 100;

    std::cout << "HP: ";

    // HP 비율에 따라 색상 변경
    for (int i = 0; i < barLength; ++i)
    {
        if (i < filledBars)
        {
            // HP가 높으면 녹색, 중간이면 노란색, 낮으면 빨간색
            if (hpPercentage > 60)
                std::cout << ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::BrightGreen);
            else if (hpPercentage > 30)
                std::cout << ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::BrightYellow);
            else
                std::cout << ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::BrightRed);

            std::cout << "█"; // 채워진 바
        }
        else
        {
            std::cout << ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::White);
            std::cout << "░"; // 빈 바
        }
    }

    std::cout << ANSIUtil::Reset();
    std::cout << " " << unit->CurrentHP << "/" << unit->MaxHP << std::endl;
}
```

---

## 고급 ANSI Art 기법

### 1. Box Drawing Characters (박스 그리기 문자)
ASCII 확장 문자를 사용하여 테두리와 메뉴를 그릴 수 있습니다:

```
┌─┬─┐  ╔═╦═╗  ╭─┬─╮
│ │ │  ║ ║ ║  │ │ │
├─┼─┤  ╠═╬═╣  ├─┼─┤
│ │ │  ║ ║ ║  │ │ │
└─┴─┘  ╚═╩═╝  ╰─┴─╯
```

**예시:**
```cpp
void DrawMenuBox(int x, int y, int width, int height)
{
    std::cout << ANSIUtil::CursorTo(x, y) << "┌";
    for (int i = 0; i < width - 2; ++i) std::cout << "─";
    std::cout << "┐";

    for (int row = 1; row < height - 1; ++row)
    {
        std::cout << ANSIUtil::CursorTo(x, y + row) << "│";
        std::cout << ANSIUtil::CursorTo(x + width - 1, y + row) << "│";
    }

    std::cout << ANSIUtil::CursorTo(x, y + height - 1) << "└";
    for (int i = 0; i < width - 2; ++i) std::cout << "─";
    std::cout << "┘";
}
```

### 2. 블록 문자 활용
```
█ ▓ ▒ ░  (음영 블록)
▀ ▄ ▌ ▐  (반 블록)
■ □ ▪ ▫  (사각형)
● ○ ◆ ◇  (원형/마름모)
```

### 3. 유니코드 심볼
게임 요소를 더 직관적으로 표현:
```
유닛: ♔ ♕ ♖ ♗ ♘ ♙ (체스 말)
무기: ⚔ ⚡ ☠ ✦
지형: ♣ ♠ ≈ ▲ ☼
```

**주의**: 유니코드 문자는 콘솔 폰트에 따라 표시가 안 될 수 있습니다. ASCII로 대체 문자를 준비해두는 것이 좋습니다.

---

## 렌더링 시스템 통합

### ScreenBuffer와 ANSI 통합
기존 `ScreenBuffer` 클래스를 확장하여 ANSI 코드를 저장:

`Engine/Render/ScreenBuffer.h`:
```cpp
class ScreenBuffer
{
private:
    struct Cell
    {
        char Character;
        std::string ANSICode; // ANSI 색상/스타일 코드
    };

    Cell** Buffer;
    int Width;
    int Height;

public:
    // 기존 함수들...

    // ANSI 지원 함수 추가
    void SetCell(int x, int y, char character, const std::string& ansiCode);
    void Render(); // ANSI 코드와 함께 출력
};
```

`Engine/Render/ScreenBuffer.cpp`:
```cpp
void ScreenBuffer::SetCell(int x, int y, char character, const std::string& ansiCode)
{
    if (x < 0 || x >= Width || y < 0 || y >= Height)
        return;

    Buffer[y][x].Character = character;
    Buffer[y][x].ANSICode = ansiCode;
}

void ScreenBuffer::Render()
{
    std::cout << ANSIUtil::ClearScreen();
    std::cout << ANSIUtil::HideCursor();

    for (int y = 0; y < Height; ++y)
    {
        for (int x = 0; x < Width; ++x)
        {
            std::cout << Buffer[y][x].ANSICode;
            std::cout << Buffer[y][x].Character;
            std::cout << ANSIUtil::Reset();
        }
        std::cout << std::endl;
    }

    std::cout << ANSIUtil::ShowCursor();
}
```

---

## 성능 고려사항

### 1. ANSI 코드 캐싱
자주 사용하는 ANSI 문자열을 미리 생성하여 재사용:
```cpp
static const std::string PlayerUnitColor = ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::BrightBlue);
static const std::string EnemyUnitColor = ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::BrightRed);
```

### 2. 최소한의 출력
매 프레임마다 전체 화면을 다시 그리지 말고, 변경된 부분만 업데이트:
```cpp
void Renderer::DrawOnlyChanges(ScreenBuffer* oldBuffer, ScreenBuffer* newBuffer)
{
    for (int y = 0; y < Height; ++y)
    {
        for (int x = 0; x < Width; ++x)
        {
            if (oldBuffer->GetCell(x, y) != newBuffer->GetCell(x, y))
            {
                std::cout << ANSIUtil::CursorTo(x + 1, y + 1); // 1-based
                std::cout << newBuffer->GetANSICode(x, y);
                std::cout << newBuffer->GetCharacter(x, y);
            }
        }
    }
}
```

### 3. 버퍼링
std::stringstream을 사용하여 출력을 버퍼링한 후 한 번에 출력:
```cpp
void Renderer::DrawBuffered()
{
    std::stringstream ss;
    ss << ANSIUtil::ClearScreen();

    for (int y = 0; y < Height; ++y)
    {
        for (int x = 0; x < Width; ++x)
        {
            ss << Buffer[y][x].ANSICode;
            ss << Buffer[y][x].Character;
        }
        ss << "\n";
    }

    ss << ANSIUtil::Reset();
    std::cout << ss.str(); // 한 번에 출력
}
```

---

## 폴백(Fallback) 전략

ANSI 지원이 안 되는 환경을 위한 대비:

```cpp
class Renderer
{
private:
    bool IsANSISupported;

public:
    void Initialize()
    {
        IsANSISupported = EnableVirtualTerminalProcessing();
        
        if (!IsANSISupported)
        {
            printf("ANSI not supported. Using basic ASCII mode.\n");
        }
    }

    std::string GetColoredString(const std::string& text, ANSIUtil::ForegroundColor color)
    {
        if (IsANSISupported)
        {
            return ANSIUtil::SetForeground(color) + text + ANSIUtil::Reset();
        }
        else
        {
            // ANSI 미지원 시 색상 없이 텍스트만 반환
            return text;
        }
    }
};
```

---

## 테스트 및 디버깅

### ANSI 테스트 프로그램
```cpp
void TestANSI()
{
    EnableVirtualTerminalProcessing();

    std::cout << ANSIUtil::ClearScreen();
    std::cout << ANSIUtil::Bold() << "ANSI Test Program" << ANSIUtil::Reset() << std::endl;
    std::cout << std::endl;

    // 색상 테스트
    std::cout << "Foreground Colors:" << std::endl;
    std::cout << ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::Red) << "Red" << ANSIUtil::Reset() << std::endl;
    std::cout << ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::Green) << "Green" << ANSIUtil::Reset() << std::endl;
    std::cout << ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::Blue) << "Blue" << ANSIUtil::Reset() << std::endl;

    // 박스 그리기 테스트
    std::cout << std::endl << "Box Drawing:" << std::endl;
    std::cout << "┌─────────┐" << std::endl;
    std::cout << "│  Menu   │" << std::endl;
    std::cout << "└─────────┘" << std::endl;

    // HP 바 테스트
    std::cout << std::endl << "HP Bar:" << std::endl;
    std::cout << ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::BrightGreen);
    std::cout << "████████";
    std::cout << ANSIUtil::SetForeground(ANSIUtil::ForegroundColor::White);
    std::cout << "░░" << ANSIUtil::Reset() << " 80/100" << std::endl;
}
```

---

## 추천 구현 단계

1. **1단계**: `EnableVirtualTerminalProcessing()` 함수를 엔진 초기화 시 호출
2. **2단계**: `ANSIUtil` 유틸리티 클래스 구현
3. **3단계**: 간단한 ANSI 테스트 프로그램으로 색상 출력 확인
4. **4단계**: 유닛 색상 구분 적용 (플레이어/적)
5. **5단계**: 지형 색상 적용
6. **6단계**: UI 하이라이트 (이동 범위, 공격 범위)
7. **7단계**: 고급 UI (HP 바, 메뉴 박스 등)
8. **8단계**: 최적화 및 폴백 처리

---

## 참고 자료
- [ANSI Escape Codes - Wikipedia](https://en.wikipedia.org/wiki/ANSI_escape_code)
- [Windows Console Virtual Terminal Sequences](https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences)
- [Box Drawing Characters](https://en.wikipedia.org/wiki/Box-drawing_character)
- [Unicode Block Elements](https://en.wikipedia.org/wiki/Block_Elements)

---

## 주의사항
- ANSI 코드는 Windows 10 버전 1511 이상에서만 지원됩니다.
- 일부 터미널 에뮬레이터는 TrueColor(RGB) 색상을 지원하지 않을 수 있습니다.
- 유니코드 문자는 콘솔 폰트 설정에 따라 깨질 수 있으니 대체 ASCII 문자를 준비하세요.
- 색상 사용 시 색맹 사용자를 고려하여 색상만으로 정보를 구분하지 말고 문자나 기호도 함께 사용하세요.
