#include "PlayerPhaseController.h"
#include "Level/MapLevel.h"
#include "System/TurnManager.h"
#include "Combat/CombatSequence.h"
#include "Combat/CombatCalculator.h"
#include "Core/Input.h"
#include <stdio.h>
#include <algorithm>

namespace FE
{
	PlayerPhaseController::PlayerPhaseController(MapLevel* level, TurnManager* turnManager)
		: level(level)
		, turnManager(turnManager)
		, inputState(PlayerInputState::Idle)
		, selectedUnit(nullptr)
		, selectedMovePosition(Vector2::Zero)
		, selectedAction(0)
		, gridOffsetX(1)
		, gridOffsetY(1)
	{
	}

	PlayerPhaseController::~PlayerPhaseController()
	{
	}

	void PlayerPhaseController::Update(float deltaTime)
	{
		// 플레이어 페이즈가 아니면 리턴
		if (turnManager->GetCurrentPhase() != GamePhase::PlayerPhase)
		{
			return;
		}

		// 입력 처리
		HandleInput();

		// 모든 플레이어 유닛이 행동을 완료했는지 체크
		if (IsPlayerPhaseDone())
		{
			printf("\nAll player units have acted. Press SPACE to end turn.\n");

			// 스페이스 키로 턴 종료
			if (Input::Get().GetKeyDown(VK_SPACE))
			{
				turnManager->EndPlayerPhase();
			}
		}
	}

	void PlayerPhaseController::HandleInput()
	{
		switch (inputState)
		{
		case PlayerInputState::Idle:
			HandleIdleState();
			break;
		case PlayerInputState::UnitSelected:
			HandleUnitSelectedState();
			break;
		case PlayerInputState::MovementSelected:
			HandleMovementSelectedState();
			break;
		case PlayerInputState::ActionMenuOpen:
			HandleActionMenuState();
			break;
		case PlayerInputState::TargetSelection:
			HandleTargetSelectionState();
			break;
		}
	}

	// Idle 상태: 유닛 선택 대기
	void PlayerPhaseController::HandleIdleState()
	{
		// 마우스 클릭 시 유닛 선택
		if (Input::Get().GetMouseButtonDown(0))
		{
			Vector2 gridPos = Input::Get().ScreenToGrid(gridOffsetX, gridOffsetY);

			// 클릭한 위치의 유닛 찾기
			Unit* clickedUnit = level->GetUnitAtPosition(gridPos.x, gridPos.y);

			if (clickedUnit != nullptr && clickedUnit->GetFaction() == UnitFaction::Player && !clickedUnit->IsActionDone())
			{
				selectedUnit = clickedUnit;
				inputState = PlayerInputState::UnitSelected;
				printf("Selected: %s at (%d, %d)\n", selectedUnit->GetUnitName(), selectedUnit->GetGridX(), selectedUnit->GetGridY());

				// 이동 가능 범위 계산
				moveRange = CalculateMoveRange(selectedUnit);
				printf("Move range calculated: %d tiles\n", static_cast<int>(moveRange.size()));
			}
		}
	}

	// 유닛 선택 상태: 이동 위치 선택 대기
	void PlayerPhaseController::HandleUnitSelectedState()
	{
		// ESC 키로 취소
		if (Input::Get().GetKeyDown(VK_ESCAPE))
		{
			selectedUnit = nullptr;
			inputState = PlayerInputState::Idle;
			moveRange.clear();
			printf("Selection cancelled.\n");
			return;
		}

		// 마우스 클릭 시 이동 위치 선택
		if (Input::Get().GetMouseButtonDown(0))
		{
			Vector2 gridPos = Input::Get().ScreenToGrid(gridOffsetX, gridOffsetY);

			// 이동 가능 범위 내인지 확인
			bool inRange = false;
			for (const Vector2& pos : moveRange)
			{
				if (pos.x == gridPos.x && pos.y == gridPos.y)
				{
					inRange = true;
					break;
				}
			}

			if (inRange)
			{
				// 해당 위치에 다른 유닛이 있는지 확인
				Unit* unitAtPos = level->GetUnitAtPosition(gridPos.x, gridPos.y);
				if (unitAtPos == nullptr || unitAtPos == selectedUnit)
				{
					selectedMovePosition = gridPos;
					selectedUnit->MoveTo(gridPos.x, gridPos.y);
					printf("Moved %s to (%d, %d)\n", selectedUnit->GetUnitName(), gridPos.x, gridPos.y);

					inputState = PlayerInputState::ActionMenuOpen;
					selectedAction = 0; // 기본값: Attack
				}
				else
				{
					printf("Tile occupied by another unit.\n");
				}
			}
			else
			{
				printf("Out of move range.\n");
			}
		}
	}

	// MovementSelected 상태: 이동 확정 후 처리 (현재 미사용)
	void PlayerPhaseController::HandleMovementSelectedState()
	{
		// 현재 UnitSelected에서 직접 ActionMenuOpen으로 전환하므로 이 상태는 사용되지 않음
		// 향후 이동 확정 전 확인 단계가 필요하면 여기에 구현
		inputState = PlayerInputState::ActionMenuOpen;
	}

	// 행동 메뉴 상태: Attack 또는 Wait 선택
	void PlayerPhaseController::HandleActionMenuState()
	{
		printf("\nAction Menu: [A]ttack  [W]ait\n");

		// A 키: Attack
		if (Input::Get().GetKeyDown('A'))
		{
			selectedAction = 0;
			attackRange = CalculateAttackRange(selectedUnit);

			if (attackRange.empty())
			{
				printf("No targets in range. Unit waits.\n");
				selectedUnit->SetActionDone(true);
				selectedUnit = nullptr;
				inputState = PlayerInputState::Idle;
			}
			else
			{
				printf("Select target to attack.\n");
				inputState = PlayerInputState::TargetSelection;
			}
		}

		// W 키: Wait
		if (Input::Get().GetKeyDown('W'))
		{
			printf("%s waits.\n", selectedUnit->GetUnitName());
			selectedUnit->SetActionDone(true);
			selectedUnit = nullptr;
			inputState = PlayerInputState::Idle;
		}

		// ESC 키로 취소 (이동 전으로 돌아가기는 복잡하므로 현재는 그냥 대기)
		if (Input::Get().GetKeyDown(VK_ESCAPE))
		{
			printf("%s waits.\n", selectedUnit->GetUnitName());
			selectedUnit->SetActionDone(true);
			selectedUnit = nullptr;
			inputState = PlayerInputState::Idle;
		}
	}

	// 공격 대상 선택 상태
	void PlayerPhaseController::HandleTargetSelectionState()
	{
		// ESC 키로 취소
		if (Input::Get().GetKeyDown(VK_ESCAPE))
		{
			printf("Attack cancelled. Unit waits.\n");
			selectedUnit->SetActionDone(true);
			selectedUnit = nullptr;
			inputState = PlayerInputState::Idle;
			attackRange.clear();
			return;
		}

		// 마우스 클릭 시 대상 선택
		if (Input::Get().GetMouseButtonDown(0))
		{
			Vector2 gridPos = Input::Get().ScreenToGrid(gridOffsetX, gridOffsetY);

			// 공격 가능 범위 내인지 확인
			bool inRange = false;
			for (const Vector2& pos : attackRange)
			{
				if (pos.x == gridPos.x && pos.y == gridPos.y)
				{
					inRange = true;
					break;
				}
			}

			if (inRange)
			{
				Unit* target = level->GetUnitAtPosition(gridPos.x, gridPos.y);

				if (target != nullptr && target->GetFaction() == UnitFaction::Enemy)
				{
					printf("Attacking %s!\n", target->GetUnitName());

					// 전투 실행
					CombatSequence::InitiateCombat(selectedUnit, target, level);

					// 유닛 행동 완료
					selectedUnit->SetActionDone(true);
					selectedUnit = nullptr;
					inputState = PlayerInputState::Idle;
					attackRange.clear();
				}
				else
				{
					printf("No valid target at this position.\n");
				}
			}
			else
			{
				printf("Target out of range.\n");
			}
		}
	}

	// 이동 가능 범위 계산 (간단 버전: 주변 Mov 칸)
	std::vector<Vector2> PlayerPhaseController::CalculateMoveRange(Unit* unit)
	{
		std::vector<Vector2> range;
		int mov = unit->GetMov();
		int unitX = unit->GetGridX();
		int unitY = unit->GetGridY();

		// 맨해튼 거리 기준으로 이동 범위 계산
		for (int y = -mov; y <= mov; ++y)
		{
			for (int x = -mov; x <= mov; ++x)
			{
				int distance = (x < 0 ? -x : x) + (y < 0 ? -y : y);
				if (distance <= mov)
				{
					int targetX = unitX + x;
					int targetY = unitY + y;

					// 그리드 범위 내인지 확인
					if (level->GetGrid()->IsValidPosition(targetX, targetY))
					{
						range.push_back(Vector2(targetX, targetY));
					}
				}
			}
		}

		return range;
	}

	// 공격 가능 범위 계산
	std::vector<Vector2> PlayerPhaseController::CalculateAttackRange(Unit* unit)
	{
		std::vector<Vector2> range;
		Weapon* weapon = unit->GetEquippedWeapon();

		if (weapon == nullptr)
		{
			return range;
		}

		int minRange = weapon->GetMinRange();
		int maxRange = weapon->GetMaxRange();
		int unitX = unit->GetGridX();
		int unitY = unit->GetGridY();

		// 적 유닛들의 위치 확인
		std::vector<Unit*> enemies = level->GetEnemyUnits();

		for (Unit* enemy : enemies)
		{
			int distance = CombatCalculator::CalculateDistance(
				unitX, unitY, enemy->GetGridX(), enemy->GetGridY()
			);

			if (distance >= minRange && distance <= maxRange)
			{
				range.push_back(Vector2(enemy->GetGridX(), enemy->GetGridY()));
			}
		}

		return range;
	}

	// 플레이어 페이즈 종료 여부 체크
	bool PlayerPhaseController::IsPlayerPhaseDone()
	{
		std::vector<Unit*> playerUnits = level->GetPlayerUnits();

		for (Unit* unit : playerUnits)
		{
			if (!unit->IsActionDone())
			{
				return false;
			}
		}

		return true;
	}
}
