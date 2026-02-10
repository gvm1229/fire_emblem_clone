#pragma once

#include "Level/Level.h"
#include "Map/Grid.h"
#include "Unit/Unit.h"
#include "System/MovementCalculator.h"
#include "Algorithm/NavigationSystem.h"
#include "AI/EnemyAI.h"
#include <vector>
#include <deque>
#include <string>

namespace FEClone
{
	// 전투 레벨 - Fire Emblem 전투 맵
	class BattleLevel : public Level
	{
		RTTI_DECLARATIONS(BattleLevel, Level)

	public:
		BattleLevel();
		virtual ~BattleLevel() override;

		// Level 오버라이드
		virtual void BeginPlay() override;
		virtual void Tick(float deltaTime) override;
		virtual void Draw() override;

		// 맵 로딩
		bool LoadMap(const char* mapFilePath);

		// 유닛 추가
		void AddUnit(Unit* unit, const Vector2& gridPosition, Faction faction, int unitIndex = -1);

		// 플레이어 유닛 선택 (숫자 키)
		void SelectUnitByIndex(int index);

		// 마우스 클릭 처리
		void OnMouseClick(const Vector2& mousePos);

	private:
		// 그리드 렌더링
		void DrawGrid();

		// 이동 가능 범위 하이라이트
		void DrawMovementRange();

	// 스탯 UI 패널
	void DrawStatsPanel();

	// 이벤트 로그 패널 (유닛 정보 오른쪽)
	void DrawLogPanel();

	// 이벤트 로그 추가 (최근 20개 유지)
	void AddLog(const char* message);

	// 키보드 툴팁 (하단)
	void DrawKeyboardTooltip();

	// 입력 처리
	void HandleInput();

private:
	Grid* grid;                                 // 맵 그리드
	std::vector<Unit*> playerUnits;             // 플레이어 유닛 목록
	std::vector<Unit*> enemyUnits;              // 적 유닛 목록

	Unit* selectedUnit;                         // 현재 선택된 유닛
	std::vector<Vector2> reachableTiles;        // 이동 가능한 타일 목록

	MovementCalculator movementCalculator;      // 이동 범위 계산기
	NavigationSystem navigationSystem;          // 경로 탐색 시스템
	EnemyAI enemyAI;                            // 적 AI

	// 턴 관리
	bool isPlayerTurn;
	int turnCount;

	// UI 버퍼 (렌더링 시 포인터 저장용)
	char uiBuffers[16][64];                     // 16개의 UI 텍스트 버퍼 (terrain info 추가)

	// 이벤트 로그 (최근 20개)
	static const int kMaxEventLogEntries = 20;
	std::deque<std::string> eventLog;

	// 래핑된 로그 줄 (Submit에 넘기는 포인터가 이 수명 동안 유효해야 함)
	std::vector<std::string> wrappedLogLines;
	};
}
