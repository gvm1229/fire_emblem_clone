#pragma once

namespace FE
{
	// 전방 선언
	class MapLevel;
	class TurnManager;

	class EnemyPhaseController
	{
	public:
		EnemyPhaseController(MapLevel* level, TurnManager* turnManager);
		~EnemyPhaseController();

		// 업데이트
		void Update(float deltaTime);

		// 적 페이즈 실행
		void ExecuteEnemyPhase();

	private:
		MapLevel* level;
		TurnManager* turnManager;

		bool enemyPhaseExecuted;
	};
}
