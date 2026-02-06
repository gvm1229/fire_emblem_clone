#pragma once

#include "Common/Common.h"
#include "Tile.h"

namespace FE
{
	class FE_API Grid
	{
	public:
		Grid();
		Grid(int width, int height);
		~Grid();

		// 타일 가져오기/설정하기
		Tile* GetTile(int x, int y) const;
		void SetTile(int x, int y, TerrainType type);

		// 좌표 유효성 검사
		bool IsValidPosition(int x, int y) const;

		// Grid 크기 getter
		inline int GetWidth() const { return width; }
		inline int GetHeight() const { return height; }

		// Grid 초기화 (모든 타일을 평지로)
		void Initialize(int width, int height);

		// Grid 클리어
		void Clear();

	private:
		int width;
		int height;
		Tile** tiles; // 2D 배열
	};
}
