#pragma once

#include "Common/Common.h"
#include "Tile.h"
#include <vector>

namespace Wanted
{
	// 그리드 클래스 - 맵의 타일들을 관리
	class Grid
	{
	public:
		// 생성자: 지정된 크기의 그리드 생성
		Grid(int width, int height);
		~Grid();

		// 크기 반환
		int GetWidth() const { return width; }
		int GetHeight() const { return height; }

		// 특정 위치의 타일 반환
		Tile* GetTile(int x, int y) const;
		Tile* GetTile(const Vector2& position) const;

		// 특정 위치의 타일 설정 (지형 타입 변경)
		void SetTile(int x, int y, TerrainType type);

		// 위치가 그리드 범위 내인지 확인
		bool IsValidPosition(int x, int y) const;
		bool IsValidPosition(const Vector2& position) const;

		// 특정 위치가 이동 가능한지 확인 (타일 + 유닛 여부)
		bool IsWalkable(int x, int y) const;
		bool IsWalkable(const Vector2& position) const;

		// Navigation을 위한 bool 맵 생성 (true = 이동 가능, false = 장애물)
		std::vector<std::vector<bool>> GenerateNavigationMap() const;

	private:
		int width;                      // 그리드 너비
		int height;                     // 그리드 높이
		std::vector<std::vector<Tile*>> tiles;  // 2D 타일 배열
	};
}
