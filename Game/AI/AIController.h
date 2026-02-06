#pragma once

#include "Unit/Unit.h"
#include "Math/Vector2.h"

namespace FE
{
	// 전방 선언
	class MapLevel;

	class AIController
	{
	public:
		// AI 행동 결정 및 실행
		static void DecideAndExecuteAction(Unit* unit, MapLevel* level);

		// 가장 가까운 적 찾기
		static Unit* FindNearestTarget(Unit* unit, MapLevel* level);

		// 최적의 이동 위치 찾기 (적에게 가까워지기)
		static Vector2 FindBestMovePosition(Unit* unit, Unit* target, MapLevel* level);

		// 이동 가능한 위치 계산 (간단 버전)
		static bool CanMoveToPosition(Unit* unit, int x, int y, MapLevel* level);
	};
}
