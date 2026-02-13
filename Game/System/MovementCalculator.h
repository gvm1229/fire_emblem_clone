#pragma once

#include "Common/Common.h"
#include "Math/Vector2.h"
#include "Map/Grid.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>

namespace FEClone
{
	// 이동 가능 범위를 계산하는 클래스
	// Dijkstra 알고리즘을 사용하여 지형 비용을 고려한 이동 범위 계산
	class MovementCalculator
	{
	public:
		MovementCalculator() = default;
		~MovementCalculator() = default;

		// 이동 가능 범위 계산
		// start: 시작 위치
		// maxMovement: 최대 이동력
		// grid: 그리드
		// outReachableTiles: 결과 (이동 가능한 타일 목록)
		void CalculateReachableTiles(
			const Vector2& start,
			int maxMovement,
			const Grid* grid,
			std::vector<Vector2>* outReachableTiles
		);

		// 특정 위치까지의 실제 이동 비용 반환 (마지막 계산 기준)
		int GetMovementCostTo(const Vector2& position) const;

	private:
		// Dijkstra 노드 구조체
		struct Node
		{
			Vector2 position;
			int cost;

			// 우선순위 큐용 (비용이 작을수록 우선)
			bool operator>(const Node& other) const
			{
				return cost > other.cost;
			}
		};

		// 4방향 이동
		Vector2 directions[4] =
		{
			Vector2(0, -1),  // 위
			Vector2(1, 0),   // 오른쪽
			Vector2(0, 1),   // 아래
			Vector2(-1, 0)   // 왼쪽
		};

		// 마지막 계산 결과 (위치 -> 이동 비용)
		std::unordered_map<Vector2, int> costMap;
	};
}
