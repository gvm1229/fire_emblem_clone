#include "Unit.h"
#include "Render/Renderer.h"

namespace FEClone
{
RTTI_DEFINITIONS(Unit)

// 생성자
Unit::Unit(UnitClass unitClass)
	: Actor()
	, stats()
	, faction(Faction::Player)
	, unitClass(unitClass)
	, state(UnitState::Idle)
	, gridPosition(0, 0)
	, moveSpeed(5.0f)
	, moveTimer(0.0f)
	, displayStr("")
	, unitIndex(-1)
{
	strcpy_s(displayStr, sizeof(displayStr), GetUnitClassString(unitClass));

	// Lord 유닛은 스탯을 50% 강화
	if (unitClass == UnitClass::Lord)
	{
		stats.maxHP = static_cast<int>(stats.maxHP * 1.5f);
		stats.currentHP = stats.maxHP;
		stats.strength = static_cast<int>(stats.strength * 1.5f);
		stats.magic = (stats.magic == 0) ? 3 : static_cast<int>(stats.magic * 1.5f);
		stats.skill = static_cast<int>(stats.skill * 1.5f);
		stats.speed = static_cast<int>(stats.speed * 1.5f);
		stats.luck = (stats.luck == 0) ? 3 : static_cast<int>(stats.luck * 1.5f);
		stats.defense = static_cast<int>(stats.defense * 1.5f);
		stats.resistance = (stats.resistance == 0) ? 3 : static_cast<int>(stats.resistance * 1.5f);
		stats.movement = static_cast<int>(stats.movement * 1.5f);
	}
}

	// 소멸자
	Unit::~Unit()
	{
	}

	// BeginPlay: 액터 생성 시 호출
	void Unit::BeginPlay()
	{
		Actor::BeginPlay();

		// Actor의 position은 사용하지 않음 (gridPosition만 사용)
		// 렌더링은 gridPosition + Vector2(1, 1)로 처리
	}

	// Tick: 매 프레임 업데이트
	void Unit::Tick(float deltaTime)
	{
		Actor::Tick(deltaTime);

		// 이동 중이면 이동 처리
		if (state == UnitState::Moving)
		{
			UpdateMovement(deltaTime);
		}
	}

// Draw: 유닛 렌더링
void Unit::Draw()
{
	// Actor::Draw()는 호출하지 않음 (position 대신 gridPosition 사용)

	// 그리드 위치 + 1 (UI 공간 확보)
	Vector2 renderPos = gridPosition + Vector2(1, 1);

	// 유닛 문자열 출력
	Renderer::Get().Submit(displayStr, renderPos, GetDisplayColor(), 5);
}

	// 이동 경로 설정
	void Unit::SetPath(const std::deque<Vector2>& newPath)
	{
		movementPath = newPath;
		if (!movementPath.empty())
		{
			state = UnitState::Moving;
			moveTimer = 0.0f;
		}
	}

	// 이동 업데이트
	void Unit::UpdateMovement(float deltaTime)
	{
		if (movementPath.empty())
		{
			// 이동 완료: 턴 종료
			state = UnitState::Done;
			return;
		}

		moveTimer += deltaTime;

		// 다음 타일로 이동할 시간이 되면
		float timePerTile = 1.0f / moveSpeed;
		if (moveTimer >= timePerTile)
		{
			moveTimer = 0.0f;

			// 다음 위치로 이동
			gridPosition = movementPath.front();
			movementPath.pop_front();

			// 경로가 끝나면 턴 종료
			if (movementPath.empty())
			{
				state = UnitState::Done;
			}
		}
	}

	// 표시 문자열 설정
	void Unit::SetDisplayString(const char* str)
	{
		strcpy_s(displayStr, sizeof(displayStr), str);
	}

	// 클래스 변경
	void Unit::SetUnitClass(UnitClass newClass)
	{
		unitClass = newClass;
		strcpy_s(displayStr, sizeof(displayStr), GetUnitClassString(unitClass));
	}

	// 렌더링 색상 반환 (진영 + 상태에 따라)
	Color Unit::GetDisplayColor() const
	{
		// 턴 종료 상태면 마젠타타
		if (state == UnitState::Done)
		{
			return Color::Purple;
		}

		// Lord 유닛은 기본 마젠타색
		if (unitClass == UnitClass::Lord)
		{
			return Color::Yellow;
		}

		// 선택 상태면 하늘색
		if (state == UnitState::Selected)
		{
			return Color::Cyan;
		}

		// 진영에 따른 색상
		switch (faction)
		{
		case Faction::Player: // 플레이어 유닛은 파란색
			return Color::Blue;
		case Faction::Enemy: // 적 유닛은 빨간색
			return Color::Red;
		case Faction::Ally: // 동맹 유닛은 초록색
			return Color::Green;
		default: // 기타 유닛은 흰색
			return Color::White;
		}
	}
}
