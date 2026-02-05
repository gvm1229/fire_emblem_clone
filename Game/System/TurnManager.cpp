#include "TurnManager.h"
#include "Level/MapLevel.h"
#include <stdio.h>

namespace FE
{
	TurnManager::TurnManager(MapLevel* level)
		: level(level)
		, currentPhase(GamePhase::PlayerPhase)
		, turnCount(1)
	{
	}

	TurnManager::~TurnManager()
	{
	}

	void TurnManager::Update(float deltaTime)
	{
		// 턴 관리는 주로 외부에서 호출되므로 여기서는 특별한 처리 없음
	}

	// 플레이어 페이즈 시작
	void TurnManager::StartPlayerPhase()
	{
		currentPhase = GamePhase::PlayerPhase;
		ResetAllUnitActions();
		printf("\n--- Turn %d: Player Phase ---\n", turnCount);
	}

	// 플레이어 페이즈 종료
	void TurnManager::EndPlayerPhase()
	{
		printf("Player Phase End\n");
		StartEnemyPhase();
	}

	// 적 페이즈 시작
	void TurnManager::StartEnemyPhase()
	{
		currentPhase = GamePhase::EnemyPhase;
		ResetAllUnitActions();
		printf("\n--- Turn %d: Enemy Phase ---\n", turnCount);
	}

	// 적 페이즈 종료
	void TurnManager::EndEnemyPhase()
	{
		printf("Enemy Phase End\n");
		turnCount++;
		StartPlayerPhase();
	}

	// 모든 유닛의 행동 완료 플래그 리셋
	void TurnManager::ResetAllUnitActions()
	{
		std::vector<Unit*> allUnits = level->GetAllUnits();
		for (Unit* unit : allUnits)
		{
			unit->SetActionDone(false);
		}
	}
}
