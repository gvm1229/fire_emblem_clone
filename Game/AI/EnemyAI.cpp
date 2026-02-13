#include "EnemyAI.h"
#include "Map/Tile.h"
#include <cstdio>

namespace FEClone
{
	// 4방향 인접 오프셋
	const Vector2 EnemyAI::directions[4] = {
		Vector2(0, -1),  // 위
		Vector2(1, 0),   // 오른쪽
		Vector2(0, 1),   // 아래
		Vector2(-1, 0)   // 왼쪽
	};

	// 생성자
	EnemyAI::EnemyAI()
	{
	}

	// 적 AI 실행: 인접 시 공격, 아니면 플레이어 쪽으로 이동
	void EnemyAI::RunAI(
		Unit* enemy,
		const std::vector<Unit*>& playerUnits,
		Grid* grid,
		MovementCalculator& movementCalculator,
		NavigationSystem& navigationSystem,
		std::function<void(const char*)> onLog,
		std::function<void(Unit* enemy, size_t pathCount, TerrainType terrainType)> onEnemyMoved,
		std::function<void(Unit* attacker, Unit* defender)> performCombat)
	{
		const Vector2 enemyPos = enemy->GetGridPosition();

		// 플레이어 유닛에 인접해 있으면 공격 후 턴 종료
		for (Unit* playerUnit : playerUnits)
		{
			if (!playerUnit->IsAlive()) continue;
			const Vector2 playerPos = playerUnit->GetGridPosition();
			int dx = enemyPos.x - playerPos.x;
			int dy = enemyPos.y - playerPos.y;
			if ((dx == 0 && (dy == 1 || dy == -1)) || (dy == 0 && (dx == 1 || dx == -1)))
			{
				if (performCombat)
					performCombat(enemy, playerUnit);
				enemy->EndTurn();
				return;
			}
		}

		// 플레이어 유닛에 인접한 타일 수집 (걸을 수 있는 타일만)
		std::unordered_set<Vector2> targetTiles;
		for (Unit* playerUnit : playerUnits)
		{
			const Vector2 playerPos = playerUnit->GetGridPosition();
			for (int i = 0; i < 4; ++i)
			{
				Vector2 adj = playerPos + directions[i];
				if (grid->IsValidPosition(adj) && grid->IsWalkable(adj))
				{
					targetTiles.insert(adj);
				}
			}
		}

		if (targetTiles.empty())
		{
			enemy->EndTurn();
			return;
		}

		// 적의 이동 가능 범위 계산
		std::vector<Vector2> enemyReachable;
		movementCalculator.CalculateReachableTiles(
			enemyPos,
			enemy->GetStats().movement,
			grid,
			&enemyReachable
		);

		// 이동 가능한 타일 중 플레이어 인접 타일에 가장 가까운 목적지 선택
		Vector2 bestDestination(-1, -1);
		int bestDistance = 999999;

		for (const Vector2& reachable : enemyReachable)
		{
			// 목표 타일까지의 최소 거리
			int minDistToTarget = 999999;
			for (const Vector2& target : targetTiles)
			{
				int dx = reachable.x - target.x;
				int dy = reachable.y - target.y;
				int dist = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
				if (dist < minDistToTarget)
				{
					minDistToTarget = dist;
				}
			}

			// 이미 인접 타일이면 거리 0
			if (targetTiles.count(reachable) != 0)
			{
				minDistToTarget = 0;
			}

			if (minDistToTarget < bestDistance)
			{
				bestDistance = minDistToTarget;
				bestDestination = reachable;
			}
		}

		if (bestDestination.x < 0)
		{
			// 도달 가능한 목표 없음
			enemy->EndTurn();
			return;
		}

		// 경로 탐색 및 이동
		std::deque<Vector2> path;
		std::vector<std::vector<bool>> navMap = grid->GenerateNavigationMap();
		navigationSystem.FindPath(enemyPos, bestDestination, navMap, &path);

		if (!path.empty())
		{
			// 이전 위치 타일 업데이트
			Tile* oldTile = grid->GetTile(enemyPos);
			if (oldTile != nullptr)
			{
				oldTile->SetHasUnit(false);
			}

			enemy->SetPath(path);

			// 목적지 타일 업데이트
			Tile* newTile = grid->GetTile(bestDestination);
			if (newTile != nullptr)
			{
				newTile->SetHasUnit(true);
			}

			// 로그: Enemy Unit #N moved X tiles to Terrain (onEnemyMoved) 또는 단순 메시지 (onLog)
			if (onEnemyMoved)
			{
				TerrainType terrainType = newTile ? newTile->GetTerrainType() : TerrainType::Plain;
				onEnemyMoved(enemy, path.size(), terrainType);
			}
			else if (onLog)
			{
				const char* terrainName = GetTerrainTypeName(newTile ? newTile->GetTerrainType() : TerrainType::Plain);
				char buf[80];
				sprintf_s(buf, sizeof(buf), "Enemy moved %zu tiles to %s.", path.size(), terrainName);
				onLog(buf);
			}
		}
		else
		{
			enemy->EndTurn();
		}
	}
}
