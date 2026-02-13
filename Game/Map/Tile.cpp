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
		, displayColor(Color::White)
	{
		// displayStr 배열 초기화 (2x2 멀티라인 ASCII 아트)
		strcpy_s(displayStr[0], sizeof(displayStr[0]), ".");
		strcpy_s(displayStr[1], sizeof(displayStr[1]), ".");
		strcpy_s(displayStr[2], sizeof(displayStr[2]), ".");
		strcpy_s(displayStr[3], sizeof(displayStr[3]), ".");

		// 지형 타입에 따른 속성 설정
		switch (terrainType)
		{
	case TerrainType::Plain:
		movementCost = 1;
		avoidBonus = 0;
		defenseBonus = 0;
		isWalkable = true;
		// 2x2 평지: 점들
		strcpy_s(displayStr[0], sizeof(displayStr[0]), ".");
		strcpy_s(displayStr[1], sizeof(displayStr[1]), ".");
		strcpy_s(displayStr[2], sizeof(displayStr[2]), ".");
		strcpy_s(displayStr[3], sizeof(displayStr[3]), ".");
		displayColor = Color::bgGray_WhiteText;
		break;

	case TerrainType::Forest:
		movementCost = 2;
		avoidBonus = 20;
		defenseBonus = 1;
		isWalkable = true;
		// 2x2 숲: 나무 모양
		strcpy_s(displayStr[0], sizeof(displayStr[0]), "♠");
		strcpy_s(displayStr[1], sizeof(displayStr[1]), "♠");
		strcpy_s(displayStr[2], sizeof(displayStr[2]), "♠");
		strcpy_s(displayStr[3], sizeof(displayStr[3]), "♠");
		displayColor = Color::bgGray_GreenText;
		break;

	case TerrainType::Mountain:
		movementCost = 3;
		avoidBonus = 30;
		defenseBonus = 2;
		isWalkable = true;
		// 2x2 산: 삼각형
		strcpy_s(displayStr[0], sizeof(displayStr[0]), "/");
		strcpy_s(displayStr[1], sizeof(displayStr[1]), "\\");
		strcpy_s(displayStr[2], sizeof(displayStr[2]), "▓");
		strcpy_s(displayStr[3], sizeof(displayStr[3]), "▓");
		displayColor = Color::bgGray_WhiteText;
		break;

	case TerrainType::Castle:
		movementCost = 1;
		avoidBonus = 20;
		defenseBonus = 2;
		isHealing = true;
		isWalkable = true;
		// 2x2 성: 성곽
		strcpy_s(displayStr[0], sizeof(displayStr[0]), "╔");
		strcpy_s(displayStr[1], sizeof(displayStr[1]), "╗");
		strcpy_s(displayStr[2], sizeof(displayStr[2]), "╚");
		strcpy_s(displayStr[3], sizeof(displayStr[3]), "╝");
		displayColor = Color::bgGray_CyanText;
		break;

	case TerrainType::Village:
		movementCost = 1;
		avoidBonus = 10;
		defenseBonus = 0;
		isWalkable = true;
		// 2x2 마을: 집 모양
		strcpy_s(displayStr[0], sizeof(displayStr[0]), "/");
		strcpy_s(displayStr[1], sizeof(displayStr[1]), "\\");
		strcpy_s(displayStr[2], sizeof(displayStr[2]), "▄");
		strcpy_s(displayStr[3], sizeof(displayStr[3]), "▄");
		displayColor = Color::bgGray_YellowText;
		break;

	case TerrainType::Water:
		movementCost = 999; // 거의 통과 불가
		avoidBonus = 0;
		defenseBonus = 0;
		isWalkable = false;
		// 2x2 물: 물결
		strcpy_s(displayStr[0], sizeof(displayStr[0]), "≈");
		strcpy_s(displayStr[1], sizeof(displayStr[1]), "≈");
		strcpy_s(displayStr[2], sizeof(displayStr[2]), "≈");
		strcpy_s(displayStr[3], sizeof(displayStr[3]), "≈");
		displayColor = Color::bgGray_BlueText;
		break;

	case TerrainType::Wall:
		movementCost = 999;
		avoidBonus = 0;
		defenseBonus = 0;
		isWalkable = false;
		// 2x2 벽: 실선 블록
		strcpy_s(displayStr[0], sizeof(displayStr[0]), "█");
		strcpy_s(displayStr[1], sizeof(displayStr[1]), "█");
		strcpy_s(displayStr[2], sizeof(displayStr[2]), "█");
		strcpy_s(displayStr[3], sizeof(displayStr[3]), "█");
		displayColor = Color::bgGray_WhiteText;
		break;
		}
	}

	// 지형 타입을 표시용 문자열로 변환
	const char* GetTerrainTypeName(TerrainType type)
	{
		switch (type)
		{
		case TerrainType::Plain:    return "Plain";
		case TerrainType::Forest:   return "Forest";
		case TerrainType::Mountain: return "Mountain";
		case TerrainType::Castle:   return "Castle";
		case TerrainType::Village:  return "Village";
		case TerrainType::Water:    return "Water";
		case TerrainType::Wall:     return "Wall";
		default:                    return "Unknown";
		}
	}
}
