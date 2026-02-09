#include "NavigationSystem.h"
#include <cmath>
#include <cfloat>

namespace Wanted
{
	// A* 경로 탐색 구현
	void NavigationSystem::FindPath(
		const Vector2& start,
		const Vector2& destination,
		const std::vector<std::vector<bool>>& map,
		std::deque<Vector2>* outPath)
	{
		// 리스트 초기화
		while (!openList.empty())
		{
			openList.pop();
		}
		closedList.clear();
		outPath->clear();

		// 시작 노드를 OpenList에 추가
		openList.push(Node(start, start));

		while (!openList.empty())
		{
			// 가장 낮은 fCost를 가진 노드를 가져옴
			Node currentNode = openList.top();
			openList.pop();

			// 이미 ClosedList에 있는 경우 무시
			if (closedList.find(currentNode.position) != closedList.end())
			{
				continue;
			}

			// 현재 노드를 ClosedList에 추가
			closedList[currentNode.position] = currentNode;

			// 목적지에 도착했으면 경로 생성 후 종료
			if (currentNode.position == destination)
			{
				ConstructPath(currentNode, start, outPath);
				break;
			}

			// 4방향 탐색
			for (int i = 0; i < 4; ++i)
			{
				Vector2 nextPosition = currentNode.position + directions[i];

				// 다음 위치가 유효한지 확인
				if (!IsValidPosition(nextPosition, map))
				{
					continue;
				}

				// 이미 ClosedList에 있는 경우 무시
				if (closedList.find(nextPosition) != closedList.end())
				{
					continue;
				}

				// 새 노드 생성 및 비용 계산
				Node newNode(nextPosition, currentNode.position);
				newNode.gCost = currentNode.gCost + 1.0f; // 기본 이동 비용 1
				newNode.hCost = CalculateHeuristic(nextPosition, destination);
				newNode.fCost = newNode.gCost + newNode.hCost;

				// OpenList에 추가
				openList.push(newNode);
			}
		}

		// 경로를 찾지 못한 경우 대체 목적지 설정
		if (outPath->empty())
		{
			SetAlternativeDestination(start, destination, outPath);
		}
	}

	// 목적지에서 시작점까지 역추적하여 경로 생성
	void NavigationSystem::ConstructPath(Node goalNode, const Vector2& start, std::deque<Vector2>* outPath)
	{
		Vector2 currentPosition = goalNode.position;

		// 역추적하여 경로 생성
		while (currentPosition != start)
		{
			outPath->push_front(currentPosition);
			currentPosition = closedList[currentPosition].previousPosition;
		}
	}

	// 경로를 못 찾았을 경우 가장 가까운 도달 가능한 지점으로 설정
	void NavigationSystem::SetAlternativeDestination(
		const Vector2& start,
		const Vector2& destination,
		std::deque<Vector2>* outPath)
	{
		float minGCost = FLT_MAX;
		float minHCost = FLT_MAX;
		Vector2 alternativeDestination;

		// ClosedList에서 목적지에 가장 가까운 노드 찾기
		for (auto& nodePair : closedList)
		{
			if (nodePair.second.position == start)
			{
				continue;
			}

			float hCost = nodePair.second.hCost;
			float gCost = nodePair.second.gCost;

			// hCost가 작을수록, 같으면 gCost가 작을수록 우선
			if (hCost < minHCost || (hCost == minHCost && gCost < minGCost))
			{
				minHCost = hCost;
				minGCost = gCost;
				alternativeDestination = nodePair.second.position;
			}
		}

		// 대체 목적지로 경로 생성
		if (closedList.find(alternativeDestination) != closedList.end())
		{
			ConstructPath(closedList[alternativeDestination], start, outPath);
		}
	}

	// 휴리스틱 비용 계산 (유클리드 거리)
	float NavigationSystem::CalculateHeuristic(const Vector2& position, const Vector2& destination)
	{
		float dx = static_cast<float>(destination.x - position.x);
		float dy = static_cast<float>(destination.y - position.y);
		return sqrtf(dx * dx + dy * dy);
	}

	// 위치가 유효한지 확인 (맵 범위 내 + 이동 가능)
	bool NavigationSystem::IsValidPosition(const Vector2& position, const std::vector<std::vector<bool>>& map)
	{
		// 맵 범위 확인
		int height = static_cast<int>(map.size());
		if (height == 0)
		{
			return false;
		}

		int width = static_cast<int>(map[0].size());

		if (position.x < 0 || position.x >= width ||
			position.y < 0 || position.y >= height)
		{
			return false;
		}

		// 이동 가능 여부 확인
		return map[position.y][position.x];
	}
}
