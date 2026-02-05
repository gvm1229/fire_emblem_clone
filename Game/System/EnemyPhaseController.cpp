#include "EnemyPhaseController.h"
#include "Level/MapLevel.h"
#include "System/TurnManager.h"
#include "AI/AIController.h"
#include "Core/Input.h"
#include <stdio.h>

namespace FE
{
	EnemyPhaseController::EnemyPhaseController(MapLevel* level, TurnManager* turnManager)
		: level(level)
		, turnManager(turnManager)
		, enemyPhaseExecuted(false)
	{
	}

	EnemyPhaseController::~EnemyPhaseController()
	{
	}

	void EnemyPhaseController::Update(float deltaTime)
	{
		// 적 페이즈가 아니면 리턴
		if (turnManager->GetCurrentPhase() != GamePhase::EnemyPhase)
		{
			enemyPhaseExecuted = false;
			return;
		}

		// 적 페이즈가 아직 실행되지 않았으면 실행
		if (!enemyPhaseExecuted)
		{
			ExecuteEnemyPhase();
			enemyPhaseExecuted = true;
		}

		// 아무 키나 눌러서 적 페이즈 종료
		printf("\nPress any key to continue...\n");

		// 아무 키나 눌렀는지 확인
		for (int key = 0; key < 255; ++key)
		{
			if (Input::Get().GetKeyDown(key))
			{
				turnManager->EndEnemyPhase();
				break;
			}
		}
	}

	// 적 페이즈 실행
	void EnemyPhaseController::ExecuteEnemyPhase()
	{
		printf("\n=== Executing Enemy Phase ===\n");

		// 모든 적 유닛에 대해 AI 실행
		std::vector<Unit*> enemyUnits = level->GetEnemyUnits();

		for (Unit* enemy : enemyUnits)
		{
			if (!enemy->IsActionDone())
			{
				AIController::DecideAndExecuteAction(enemy, level);
			}
		}

		printf("\n=== Enemy Phase Complete ===\n");
	}
}
