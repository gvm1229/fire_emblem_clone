#pragma once

namespace FE
{
	// 전방 선언
	class MapLevel;

	// 현재 페이즈
	enum class GamePhase
	{
		PlayerPhase,
		EnemyPhase
	};

	class TurnManager
	{
	public:
		TurnManager(MapLevel* level);
		~TurnManager();

		// 턴 업데이트
		void Update(float deltaTime);

		// 페이즈 전환
		void StartPlayerPhase();
		void EndPlayerPhase();
		void StartEnemyPhase();
		void EndEnemyPhase();

		// 모든 유닛의 행동 완료 플래그 리셋
		void ResetAllUnitActions();

		// Getter
		inline GamePhase GetCurrentPhase() const { return currentPhase; }
		inline int GetTurnCount() const { return turnCount; }

	private:
		MapLevel* level;
		GamePhase currentPhase;
		int turnCount;
	};
}
