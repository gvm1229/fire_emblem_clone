#include "AIController.h"
#include "Level/MapLevel.h"
#include "Combat/CombatSequence.h"
#include "Combat/CombatCalculator.h"
#include <stdio.h>

namespace FE
{
	// AI 행동 결정 및 실행
	void AIController::DecideAndExecuteAction(Unit* unit, MapLevel* level)
	{
		printf("\n[AI] %s is thinking...\n", unit->GetUnitName());

		// 1. 가장 가까운 적 찾기
		Unit* target = FindNearestTarget(unit, level);

		if (target == nullptr)
		{
			printf("[AI] %s: No targets found. Waiting.\n", unit->GetUnitName());
			unit->SetActionDone(true);
			return;
		}

		printf("[AI] %s: Target found - %s\n", unit->GetUnitName(), target->GetUnitName());

		// 2. 공격 가능한지 확인
		Weapon* weapon = unit->GetEquippedWeapon();
		if (weapon != nullptr)
		{
			int distance = CombatCalculator::CalculateDistance(
				unit->GetGridX(), unit->GetGridY(),
				target->GetGridX(), target->GetGridY()
			);

			// 공격 가능하면 바로 공격
			if (distance >= weapon->GetMinRange() && distance <= weapon->GetMaxRange())
			{
				printf("[AI] %s: Attacking %s!\n", unit->GetUnitName(), target->GetUnitName());
				CombatSequence::InitiateCombat(unit, target, level);
				unit->SetActionDone(true);
				return;
			}
		}

		// 3. 공격 불가능하면 적에게 가까이 이동
		Vector2 bestMove = FindBestMovePosition(unit, target, level);

		if (bestMove.x != unit->GetGridX() || bestMove.y != unit->GetGridY())
		{
			printf("[AI] %s: Moving from (%d, %d) to (%d, %d)\n",
				unit->GetUnitName(),
				unit->GetGridX(), unit->GetGridY(),
				bestMove.x, bestMove.y);

			unit->MoveTo(bestMove.x, bestMove.y);
		}
		else
		{
			printf("[AI] %s: Cannot move closer. Waiting.\n", unit->GetUnitName());
		}

		// 4. 이동 후 다시 공격 가능한지 확인
		if (weapon != nullptr)
		{
			int distance = CombatCalculator::CalculateDistance(
				unit->GetGridX(), unit->GetGridY(),
				target->GetGridX(), target->GetGridY()
			);

			if (distance >= weapon->GetMinRange() && distance <= weapon->GetMaxRange())
			{
				printf("[AI] %s: Attacking %s after movement!\n", unit->GetUnitName(), target->GetUnitName());
				CombatSequence::InitiateCombat(unit, target, level);
			}
		}

		unit->SetActionDone(true);
	}

	// 가장 가까운 적 찾기
	Unit* AIController::FindNearestTarget(Unit* unit, MapLevel* level)
	{
		Unit* nearestTarget = nullptr;
		int minDistance = 999999;

		// 적 찾기 (AI 유닛이 Enemy 소속이면 Player를 찾고, Player 소속이면 Enemy를 찾음)
		std::vector<Unit*> targets;
		if (unit->GetFaction() == UnitFaction::Enemy)
		{
			targets = level->GetPlayerUnits();
		}
		else
		{
			targets = level->GetEnemyUnits();
		}

		for (Unit* target : targets)
		{
			int distance = CombatCalculator::CalculateDistance(
				unit->GetGridX(), unit->GetGridY(),
				target->GetGridX(), target->GetGridY()
			);

			if (distance < minDistance)
			{
				minDistance = distance;
				nearestTarget = target;
			}
		}

		return nearestTarget;
	}

	// 최적의 이동 위치 찾기 (적에게 가까워지기)
	Vector2 AIController::FindBestMovePosition(Unit* unit, Unit* target, MapLevel* level)
	{
		Vector2 currentPos(unit->GetGridX(), unit->GetGridY());
		Vector2 bestPos = currentPos;
		int minDistance = CombatCalculator::CalculateDistance(
			unit->GetGridX(), unit->GetGridY(),
			target->GetGridX(), target->GetGridY()
		);

		int mov = unit->GetMov();

		// 이동 가능한 범위 내에서 적에게 가장 가까운 위치 찾기
		for (int dy = -mov; dy <= mov; ++dy)
		{
			for (int dx = -mov; dx <= mov; ++dx)
			{
				int manhattanDist = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
				if (manhattanDist > mov)
				{
					continue;
				}

				int newX = unit->GetGridX() + dx;
				int newY = unit->GetGridY() + dy;

				// 이동 가능한지 확인
				if (!CanMoveToPosition(unit, newX, newY, level))
				{
					continue;
				}

				// 이 위치에서 적까지의 거리 계산
				int distance = CombatCalculator::CalculateDistance(newX, newY, target->GetGridX(), target->GetGridY());

				if (distance < minDistance)
				{
					minDistance = distance;
					bestPos = Vector2(newX, newY);
				}
			}
		}

		return bestPos;
	}

	// 이동 가능한 위치인지 확인
	bool AIController::CanMoveToPosition(Unit* unit, int x, int y, MapLevel* level)
	{
		// 그리드 범위 내인지 확인
		if (!level->GetGrid()->IsValidPosition(x, y))
		{
			return false;
		}

		// 다른 유닛이 있는지 확인
		Unit* unitAtPos = level->GetUnitAtPosition(x, y);
		if (unitAtPos != nullptr && unitAtPos != unit)
		{
			return false;
		}

		// TODO: 지형 이동 비용 체크 (현재는 생략)

		return true;
	}
}
