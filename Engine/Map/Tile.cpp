#include "Tile.h"

namespace FE
{
	Tile::Tile() : terrainType(TerrainType::Plain)
	{
	}

	Tile::Tile(TerrainType type) : terrainType(type)
	{
	}

	Tile::~Tile()
	{
	}

	// 지형별 방어 보너스 반환
	int Tile::GetDefenseBonus() const
	{
		switch (terrainType)
		{
		case TerrainType::Plain:
			return 0;
		case TerrainType::Forest:
			return 1;
		case TerrainType::Mountain:
			return 2;
		case TerrainType::Castle:
			return 3;
		case TerrainType::Village:
			return 1;
		case TerrainType::Water:
			return 0;
		default:
			return 0;
		}
	}

	// 지형별 회피 보너스 반환
	int Tile::GetAvoidBonus() const
	{
		switch (terrainType)
		{
		case TerrainType::Plain:
			return 0;
		case TerrainType::Forest:
			return 20;
		case TerrainType::Mountain:
			return 30;
		case TerrainType::Castle:
			return 20;
		case TerrainType::Village:
			return 10;
		case TerrainType::Water:
			return 0;
		default:
			return 0;
		}
	}

	// 지형별 이동 비용 반환 (1 = 일반, 2 = 어려움)
	int Tile::GetMoveCost() const
	{
		switch (terrainType)
		{
		case TerrainType::Plain:
			return 1;
		case TerrainType::Forest:
			return 2;
		case TerrainType::Mountain:
			return 3;
		case TerrainType::Castle:
			return 1;
		case TerrainType::Village:
			return 1;
		case TerrainType::Water:
			return 99; // 물은 대부분 유닛이 통과 불가
		default:
			return 1;
		}
	}

	// 타일을 ASCII 문자로 표현
	char Tile::GetDisplayCharacter() const
	{
		switch (terrainType)
		{
		case TerrainType::Plain:
			return '.';  // 평지
		case TerrainType::Forest:
			return 'T';  // 나무 (Tree)
		case TerrainType::Mountain:
			return '^';  // 산
		case TerrainType::Castle:
			return '#';  // 성
		case TerrainType::Village:
			return 'V';  // 마을
		case TerrainType::Water:
			return '~';  // 물결
		default:
			return ' ';
		}
	}
}
