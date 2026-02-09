#include "Grid.h"

namespace FEClone
{
	// 생성자: 기본 평지로 초기화
	Grid::Grid(int width, int height)
		: width(width)
		, height(height)
	{
		// 2D 배열 초기화
		tiles.resize(height);
		for (int y = 0; y < height; ++y)
		{
			tiles[y].resize(width);
			for (int x = 0; x < width; ++x)
			{
				// 기본적으로 평지로 생성
				tiles[y][x] = new Tile(x, y, TerrainType::Plain);
			}
		}
	}

	// 소멸자: 모든 타일 메모리 해제
	Grid::~Grid()
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				delete tiles[y][x];
			}
		}
	}

	// 특정 위치의 타일 반환
	Tile* Grid::GetTile(int x, int y) const
	{
		if (!IsValidPosition(x, y))
		{
			return nullptr;
		}
		return tiles[y][x];
	}

	Tile* Grid::GetTile(const Vector2& position) const
	{
		return GetTile(position.x, position.y);
	}

	// 특정 위치의 타일 지형 변경
	void Grid::SetTile(int x, int y, TerrainType type)
	{
		if (!IsValidPosition(x, y))
		{
			return;
		}

		// 기존 타일 삭제 후 새 타일 생성
		delete tiles[y][x];
		tiles[y][x] = new Tile(x, y, type);
	}

	// 위치가 그리드 범위 내인지 확인
	bool Grid::IsValidPosition(int x, int y) const
	{
		return (x >= 0 && x < width && y >= 0 && y < height);
	}

	bool Grid::IsValidPosition(const Vector2& position) const
	{
		return IsValidPosition(position.x, position.y);
	}

	// 특정 위치가 이동 가능한지 확인
	bool Grid::IsWalkable(int x, int y) const
	{
		Tile* tile = GetTile(x, y);
		if (tile == nullptr)
		{
			return false;
		}

		// 타일이 걸을 수 있고, 유닛이 없어야 함
		return tile->IsWalkable() && !tile->HasUnit();
	}

	bool Grid::IsWalkable(const Vector2& position) const
	{
		return IsWalkable(position.x, position.y);
	}

	// NavigationSystem을 위한 bool 맵 생성
	std::vector<std::vector<bool>> Grid::GenerateNavigationMap() const
	{
		std::vector<std::vector<bool>> navMap;
		navMap.resize(height);

		for (int y = 0; y < height; ++y)
		{
			navMap[y].resize(width);
			for (int x = 0; x < width; ++x)
			{
				navMap[y][x] = IsWalkable(x, y);
			}
		}

		return navMap;
	}
}
