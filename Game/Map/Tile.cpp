#include "Tile.h"

namespace FEClone
{
	// 생성자: 지형 타입에 따라 속성 초기화
	Tile::Tile(int x, int y, TerrainType type)
		: position(x, y)
		, terrainType(type)
		, movementCost(1)
		, avoidBonus(0)
		, defenseBonus(0)
	, isHealing(false)
	, isWalkable(true)
	, hasUnit(false)
	, displayStr(".")
	, displayColor(Color::White)
	{
		// 지형 타입에 따른 속성 설정
		switch (terrainType)
		{
	case TerrainType::Plain:
		movementCost = 1;
		avoidBonus = 0;
		defenseBonus = 0;
		isWalkable = true;
		strcpy_s(displayStr, sizeof(displayStr), ".");  // 평지 - 점
		displayColor = Color::White;
		break;

	case TerrainType::Forest:
		movementCost = 2;
		avoidBonus = 20;
		defenseBonus = 1;
		isWalkable = true;
		strcpy_s(displayStr, sizeof(displayStr), "♣");  // 숲 - 클로버 (UTF-8)
		displayColor = Color::Green;
		break;

	case TerrainType::Mountain:
		movementCost = 3;
		avoidBonus = 30;
		defenseBonus = 2;
		isWalkable = true;
		strcpy_s(displayStr, sizeof(displayStr), "▲");  // 산 - 삼각형 (UTF-8)
		displayColor = Color::White;
		break;

	case TerrainType::Castle:
		movementCost = 1;
		avoidBonus = 20;
		defenseBonus = 2;
		isHealing = true;
		isWalkable = true;
		strcpy_s(displayStr, sizeof(displayStr), "♦");  // 성 - 다이아몬드 (UTF-8)
		displayColor = FEClone::Color::Cyan;
		break;

	case TerrainType::Village:
		movementCost = 1;
		avoidBonus = 10;
		defenseBonus = 0;
		isWalkable = true;
		strcpy_s(displayStr, sizeof(displayStr), "◘");  // 마을 - 역사각형 (UTF-8)
		displayColor = FEClone::Color::Yellow;
		break;

	case TerrainType::Water:
		movementCost = 999; // 거의 통과 불가
		avoidBonus = 0;
		defenseBonus = 0;
		isWalkable = false;
		strcpy_s(displayStr, sizeof(displayStr), "≈");  // 물 - 물결 (UTF-8)
		displayColor = Color::Blue;
		break;

	case TerrainType::Wall:
		movementCost = 999;
		avoidBonus = 0;
		defenseBonus = 0;
		isWalkable = false;
		strcpy_s(displayStr, sizeof(displayStr), "█");  // 벽 - 실선 블록 (UTF-8)
		displayColor = Color::White;
		break;
		}
	}
}
