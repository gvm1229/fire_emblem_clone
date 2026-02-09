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
}

	// 소멸자
	Unit::~Unit()
	{
	}

	// BeginPlay: 액터 생성 시 호출
	void Unit::BeginPlay()
	{
		Actor::BeginPlay();

		// 초기 렌더링 위치를 그리드 위치로 설정
		SetPosition(gridPosition);
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
	Actor::Draw();

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
			// 이동 완료
			state = UnitState::Idle;
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
			SetPosition(gridPosition);

			// 경로가 끝나면 이동 완료
			if (movementPath.empty())
			{
				state = UnitState::Idle;
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
		// 턴 종료 상태면 회색
		if (state == UnitState::Done)
		{
			return Color::White; // 회색 대신 White 사용
		}

		// 선택 상태면 노란색
		if (state == UnitState::Selected)
		{
			return Color::Yellow;
		}

		// 진영에 따른 색상
		switch (faction)
		{
		case Faction::Player:
			return Color::Blue;
		case Faction::Enemy:
			return Color::Red;
		case Faction::Ally:
			return Color::Green;
		case Faction::Neutral:
			return Color::White;
		default:
			return Color::White;
		}
	}
}
