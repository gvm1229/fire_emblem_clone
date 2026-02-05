#include "Grid.h"
#include <stdio.h>

namespace FE
{
	Grid::Grid() : width(0), height(0), tiles(nullptr)
	{
	}

	Grid::Grid(int width, int height) : width(0), height(0), tiles(nullptr)
	{
		Initialize(width, height);
	}

	Grid::~Grid()
	{
		Clear();
	}

	// 특정 좌표의 타일 반환
	Tile* Grid::GetTile(int x, int y) const
	{
		if (!IsValidPosition(x, y))
		{
			printf("[Grid::GetTile] Invalid position: (%d, %d)\n", x, y);
			return nullptr;
		}

		return &tiles[y][x];
	}

	// 특정 좌표의 타일 타입 설정
	void Grid::SetTile(int x, int y, TerrainType type)
	{
		if (!IsValidPosition(x, y))
		{
			printf("[Grid::SetTile] Invalid position: (%d, %d)\n", x, y);
			return;
		}

		tiles[y][x].SetTerrainType(type);
	}

	// 좌표가 그리드 범위 내에 있는지 확인
	bool Grid::IsValidPosition(int x, int y) const
	{
		return x >= 0 && x < width && y >= 0 && y < height;
	}

	// Grid 초기화
	void Grid::Initialize(int w, int h)
	{
		// 기존 데이터 정리
		Clear();

		width = w;
		height = h;

		// 2D 배열 동적 할당
		tiles = new Tile*[height];
		for (int y = 0; y < height; ++y)
		{
			tiles[y] = new Tile[width];
			// 모든 타일을 평지로 초기화
			for (int x = 0; x < width; ++x)
			{
				tiles[y][x] = Tile(TerrainType::Plain);
			}
		}
	}

	// Grid 메모리 해제
	void Grid::Clear()
	{
		if (tiles != nullptr)
		{
			for (int y = 0; y < height; ++y)
			{
				delete[] tiles[y];
			}
			delete[] tiles;
			tiles = nullptr;
		}

		width = 0;
		height = 0;
	}
}
