#pragma once

#include "Common/Common.h"

namespace FE
{
	// 지형 타입 enum
	enum class TerrainType
	{
		Plain,		// 평지
		Forest,		// 숲
		Mountain,	// 산
		Castle,		// 성
		Village,	// 마을
		Water		// 물
	};

	class FE_API Tile
	{
	public:
		Tile();
		Tile(TerrainType type);
		~Tile();

		// 지형 효과 관련 함수
		int GetDefenseBonus() const;
		int GetAvoidBonus() const;
		int GetMoveCost() const;

		// 지형 타입 getter/setter
		inline TerrainType GetTerrainType() const { return terrainType; }
		inline void SetTerrainType(TerrainType type) { terrainType = type; }

		// 타일을 ASCII 문자로 표현
		char GetDisplayCharacter() const;

	private:
		TerrainType terrainType;
	};
}
