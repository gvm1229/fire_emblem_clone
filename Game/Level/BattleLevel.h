#pragma once

#include "Level/Level.h"
#include "Map/Grid.h"
#include "Unit/Unit.h"
#include "System/MovementCalculator.h"
#include "Algorithm/NavigationSystem.h"
#include "AI/EnemyAI.h"
#include "Math/Color.h"
#include <vector>
#include <deque>
#include <string>
#include <utility>

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
		// 적 유닛 선택 (z,x,c,v,b,n,m, comma, period, slash) - 스탯 보기용
		void SelectEnemyByIndex(int index);

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
		void AddLog(const std::vector<std::pair<std::string, Color>>& segments);

		// 종료 조건 표시 (맵과 툴팁 사이)
		void DrawEndConditions();

		// 키보드 툴팁 (하단)
		void DrawKeyboardTooltip();

		// 입력 처리
		void HandleInput();

		// 그리드 위치의 유닛 반환 (플레이어/적)
		Unit* GetUnitAt(const Vector2& gridPos) const;
		Unit* GetEnemyAt(const Vector2& gridPos) const;
		Unit* GetPlayerUnitAt(const Vector2& gridPos) const;

		// 인접 여부 (4방향)
		static bool IsAdjacent(const Vector2& a, const Vector2& b);

		// 지형 타입별 로그 색상 (맵 표시와 동일)
		static Color GetTerrainLogColor(TerrainType type);

		// 전투 실행 (attacker -> defender), 로그 및 사망 처리
		void PerformCombat(Unit* attacker, Unit* defender);

		// 이동 후 공격 대기: 이동 완료 시 공격 실행
		void ProcessPendingAttackAfterMove();

		// 종료 조건 검사 및 게임 오버 처리
		void CheckEndConditions();
		void TriggerGameOver(Faction winner);

	private:
		Grid* grid;                                 // 맵 그리드
		std::vector<Unit*> playerUnits;             // 플레이어 유닛 목록
		std::vector<Unit*> enemyUnits;              // 적 유닛 목록

		Unit* selectedUnit;                         // 현재 선택된 플레이어 유닛 (이동/공격용)
		Unit* selectedEnemy;                        // 현재 선택된 적 유닛 (스탯 보기용)
		std::vector<Vector2> reachableTiles;        // 이동 가능한 타일 목록

		// 이동 후 공격 (플레이어가 적을 클릭해 이동한 뒤 자동 공격)
		Unit* unitPendingAttack = nullptr;
		Unit* attackTarget = nullptr;

		MovementCalculator movementCalculator;      // 이동 범위 계산기
		NavigationSystem navigationSystem;          // 경로 탐색 시스템
		EnemyAI enemyAI;                            // 적 AI

		// 턴 관리
		bool isPlayerTurn;
		int turnCount;

		// 게임 종료 (true면 맵 정리 후 승자 표시)
		bool gameOver = false;
		Faction gameOverWinner = Faction::Player;

		// UI 버퍼 (렌더링 시 포인터 저장용)
		char uiBuffers[16][64];                     // 16개의 UI 텍스트 버퍼 (terrain info 추가)

		// 이벤트 로그 (최근 20개). 각 항목 = (문자열, 색상) 세그먼트 목록
		static const int kMaxEventLogEntries = 20;
		using LogSegment = std::pair<std::string, Color>;
		std::deque<std::vector<LogSegment>> eventLog;

		// 래핑된 로그 줄 (Draw 완료까지 수명 유지)
		std::vector<std::vector<LogSegment>> wrappedLogLines;
	};
}
