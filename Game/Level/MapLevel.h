#pragma once

#include "Level/Level.h"
#include "Map/Grid.h"
#include "Unit/Unit.h"
#include <vector>

namespace FE
{
	// 전방 선언
	class TurnManager;
	class PlayerPhaseController;
	class EnemyPhaseController;

	class MapLevel : public Level
	{
		RTTI_DECLARATIONS(MapLevel, Level)

	public:
		MapLevel(const char* mapFilePath);
		virtual ~MapLevel();

		// Level 오버라이드
		virtual void BeginPlay() override;
		virtual void Tick(float deltaTime) override;
		virtual void Draw() override;

		// 맵 로딩
		void LoadMap(const char* filePath);

		// 유닛 생성
		Unit* SpawnUnit(UnitFaction faction, int gridX, int gridY, UnitClassType classType, WeaponType weaponType);

		// 승리/패배 조건 체크
		bool CheckVictoryCondition();
		bool CheckDefeatCondition();

		// Grid 접근
		inline Grid* GetGrid() const { return grid; }

		// 특정 그리드 위치의 유닛 찾기
		Unit* GetUnitAtPosition(int gridX, int gridY) const;

		// 모든 유닛 가져오기
		std::vector<Unit*> GetAllUnits() const;
		std::vector<Unit*> GetPlayerUnits() const;
		std::vector<Unit*> GetEnemyUnits() const;

	private:
		// Grid
		Grid* grid;

		// 맵 파일 경로
		const char* mapFilePath;

		// 턴 관리자
		TurnManager* turnManager;
		PlayerPhaseController* playerPhaseController;
		EnemyPhaseController* enemyPhaseController;

		// 승리/패배 조건
		bool isGameOver;
		bool isVictory;

		// 다음 맵 이름 (승리 시)
		const char* nextMapName;
	};
}
