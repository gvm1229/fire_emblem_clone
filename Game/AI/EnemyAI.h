#pragma once

#include "Unit/Unit.h"
#include "Map/Grid.h"
#include "System/MovementCalculator.h"
#include "Algorithm/NavigationSystem.h"
#include <vector>
#include <unordered_set>
#include <functional>
#include <cstddef>

namespace FEClone
{
	// 적 AI 클래스 - 플레이어 유닛에 접근하는 AI 로직 처리
	class EnemyAI
	{
	public:
		EnemyAI();
		~EnemyAI() = default;

		// 적 AI 실행: 플레이어 유닛에 인접한 타일로 이동 시도
		// enemy: 이동시킬 적 유닛
		// playerUnits: 플레이어 유닛 목록
		// grid: 맵 그리드
		// movementCalculator: 이동 범위 계산기
		// navigationSystem: 경로 탐색 시스템
		// onLog: 이동 시 로그 메시지 전달 (nullptr 가능)
		// performCombat: 인접 시 공격 실행 (attacker, defender) (nullptr 가능)
		void RunAI(
			Unit* enemy,
			const std::vector<Unit*>& playerUnits,
			Grid* grid,
			MovementCalculator& movementCalculator,
			NavigationSystem& navigationSystem,
			std::function<void(const char*)> onLog = nullptr,
			std::function<void(Unit* attacker, Unit* defender)> performCombat = nullptr
		);

	private:
		// 4방향 인접 오프셋
		static const Vector2 directions[4];
	};
}
