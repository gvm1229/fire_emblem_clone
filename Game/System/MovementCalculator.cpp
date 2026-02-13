#include "MovementCalculator.h"
#include <algorithm>

namespace FEClone
{
	// Dijkstra 알고리즘으로 이동 가능 범위 계산
	void MovementCalculator::CalculateReachableTiles(
		const Vector2& start,
		int maxMovement,
		const Grid* grid,
		std::vector<Vector2>* outReachableTiles)
	{
		// 초기화
		outReachableTiles->clear();
		costMap.clear();

		// 우선순위 큐 (비용이 작을수록 우선)
		std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;

		// 방문한 타일
		std::unordered_set<Vector2> visited;

		// 시작 노드 추가
		openList.push({ start, 0 });
		costMap[start] = 0;

		while (!openList.empty())
		{
			Node current = openList.top();
			openList.pop();

			// 이미 방문한 타일이면 건너뛰기
			if (visited.find(current.position) != visited.end())
			{
				continue;
			}

			// 방문 처리
			visited.insert(current.position);

			// 시작 위치가 아니면 결과에 추가
			if (current.position != start)
			{
				outReachableTiles->push_back(current.position);
			}

			// 4방향 탐색
			for (int i = 0; i < 4; ++i)
			{
				Vector2 nextPos = current.position + directions[i];

				// 유효한 위치인지 확인
				if (!grid->IsValidPosition(nextPos))
				{
					continue;
				}

				// 이미 방문했으면 건너뛰기
				if (visited.find(nextPos) != visited.end())
				{
					continue;
				}

				// 타일 정보 가져오기
				Tile* tile = grid->GetTile(nextPos);
				if (tile == nullptr || !tile->IsWalkable())
				{
					continue;
				}

				// 유닛이 있으면 건너뛰기 (자기 자신 제외)
				if (tile->HasUnit())
				{
					continue;
				}

				// 다음 타일까지의 이동 비용 계산
				int newCost = current.cost + tile->GetMovementCost();

				// 최대 이동력을 초과하면 건너뛰기
				if (newCost > maxMovement)
				{
					continue;
				}

				// 이미 더 적은 비용으로 도달 가능하면 건너뛰기
				auto it = costMap.find(nextPos);
				if (it != costMap.end() && it->second <= newCost)
				{
					continue;
				}

				// 비용 업데이트 및 오픈리스트에 추가
				costMap[nextPos] = newCost;
				openList.push({ nextPos, newCost });
			}
		}
	}

	// 특정 위치까지의 이동 비용 반환
	int MovementCalculator::GetMovementCostTo(const Vector2& position) const
	{
		auto it = costMap.find(position);
		if (it != costMap.end())
		{
			return it->second;
		}
		return -1; // 도달 불가능
	}
}
