#pragma once

#include "Common/Common.h"
#include "Math/Vector2.h"
#include <queue>
#include <unordered_map>
#include <deque>
#include <vector>

namespace FEClone
{
	// A* 길찾기 알고리즘을 처리하는 클래스
	// AlgorithmPractice의 NavigationSystem을 참고하여 구현
	class FECLONE_API NavigationSystem
	{
	private:
		// A* 노드 구조체
		struct Node
		{
			Node() = default;
			Node(const Vector2& position, const Vector2& previousPosition)
				: position(position), previousPosition(previousPosition)
			{
			}

			// fCost가 작을수록 우선순위가 높음 (min-heap)
			bool operator<(const Node& other) const
			{
				return fCost > other.fCost;
			}

			bool operator==(const Node& other) const
			{
				return position == other.position;
			}

			Vector2 position;           // 현재 노드 위치
			Vector2 previousPosition;   // 이전 노드 위치 (경로 재구성용)
			float gCost = 0.0f;         // 시작점부터 현재 노드까지의 비용
			float hCost = 0.0f;         // 현재 노드에서 목적지까지의 추정 비용 (휴리스틱)
			float fCost = 0.0f;         // 총 비용 (gCost + hCost)
		};

		// 4방향 이동 (상, 우, 하, 좌)
		Vector2 directions[4] =
		{
			Vector2(0, -1),  // 위
			Vector2(1, 0),   // 오른쪽
			Vector2(0, 1),   // 아래
			Vector2(-1, 0)   // 왼쪽
		};

	public:
		NavigationSystem() = default;
		~NavigationSystem() = default;

		// A* 경로 탐색 함수
		// start: 시작 위치
		// destination: 목적지 위치
		// map: 2D bool 배열 (true = 이동 가능, false = 장애물)
		// outPath: 결과 경로 (시작점 제외, 목적지 포함)
		void FindPath(
			const Vector2& start, 
			const Vector2& destination, 
			const std::vector<std::vector<bool>>& map, 
			std::deque<Vector2>* outPath
		);

	private:
		// 목적지에서 역추적하여 경로 생성
		void ConstructPath(Node goalNode, const Vector2& start, std::deque<Vector2>* outPath);

		// 경로를 못 찾았을 경우 대체 목적지 설정 (가장 가까운 도달 가능 지점)
		void SetAlternativeDestination(const Vector2& start, const Vector2& destination, std::deque<Vector2>* outPath);

		// 휴리스틱 비용 계산 (유클리드 거리)
		float CalculateHeuristic(const Vector2& position, const Vector2& destination);

		// 위치가 유효한지 확인 (맵 범위 내 + 이동 가능)
		bool IsValidPosition(const Vector2& position, const std::vector<std::vector<bool>>& map);

	private:
		// 열린 리스트 (탐색할 노드들, 우선순위 큐)
		std::priority_queue<Node, std::vector<Node>> openList;

		// 닫힌 리스트 (이미 방문한 노드들)
		std::unordered_map<Vector2, Node> closedList;
	};
}
