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

	// Draw: 유닛 렌더링 (2x2 멀티라인). Lord 제외하고 우하단에 유닛 번호 표시
	void Unit::Draw()
	{
		// Actor::Draw()는 호출하지 않음 (position 대신 gridPosition 사용)

		int baseX = gridPosition.x * 2 + 1;
		int baseY = gridPosition.y * 2 + 1;

		Color color = GetDisplayColor();
		// Render Priority: 10 (Unit - highest priority)

		// Lord는 4칸 모두 클래스 문자; 그 외는 우하단만 번호
		bool useNumberInCorner = (unitClass != UnitClass::Lord && unitIndex >= 0);
		unitNumberDisplay[0] = '0';
		unitNumberDisplay[1] = '\0';
		if (useNumberInCorner)
		{
			unitNumberDisplay[0] = (unitIndex == 9) ? '0' : static_cast<char>('1' + unitIndex);
		}

		// 상단
		Renderer::Get().Submit(displayStr, Vector2(baseX, baseY), color, 10);
		Renderer::Get().Submit(displayStr, Vector2(baseX + 1, baseY), color, 10);
		// 하단 좌, 우하단
		Renderer::Get().Submit(displayStr, Vector2(baseX, baseY + 1), color, 10);
		if (useNumberInCorner)
			Renderer::Get().Submit(unitNumberDisplay, Vector2(baseX + 1, baseY + 1), color, 10);
		else
			Renderer::Get().Submit(displayStr, Vector2(baseX + 1, baseY + 1), color, 10);
	}

	void Unit::TakeDamage(int damage)
	{
		if (damage <= 0)
			return;
		stats.currentHP -= damage;
		if (stats.currentHP < 0)
			stats.currentHP = 0;
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
		// 턴 종료 상태: 적은 적 턴일 때만 보라색, 플레이어 턴에는 빨간색 유지
		if (state == UnitState::Done)
		{
			if (faction == Faction::Enemy)
			{
				if (displayAsEnemyTurn)
					return Color::Purple;
				return Color::Red;
			}
			return Color::Purple;  // 플레이어/동맹 Done은 항상 보라
		}

		// 선택 상태면 하늘색
		if (state == UnitState::Selected)
		{
			return Color::Cyan;
		}

		// Lord 유닛은 기본 노란색
		if (unitClass == UnitClass::Lord)
		{
			return Color::Yellow;
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
