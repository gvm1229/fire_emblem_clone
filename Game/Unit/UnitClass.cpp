#include "UnitClass.h"

namespace FE
{
	// 유닛 클래스별 기본 스탯 정의
	static const UnitClassInfo unitClassData[] =
	{
		// Lord
		{
			UnitClassType::Lord, "Lord", MovementType::Infantry,
			18, 5, 0, 5, 7, 6, 5, 0, 5,  // 기본 스탯
			80, 40, 0, 50, 50, 50, 30, 0  // 성장률
		},
		// Cavalier
		{
			UnitClassType::Cavalier, "Cavalier", MovementType::Cavalry,
			20, 7, 0, 6, 6, 3, 6, 0, 7,  // 기본 스탯
			80, 50, 0, 40, 40, 30, 35, 0  // 성장률
		},
		// Knight
		{
			UnitClassType::Knight, "Knight", MovementType::Infantry,
			22, 8, 0, 5, 4, 2, 9, 0, 4,  // 기본 스탯
			90, 60, 0, 35, 30, 25, 50, 0  // 성장률
		},
		// Archer
		{
			UnitClassType::Archer, "Archer", MovementType::Infantry,
			16, 5, 0, 7, 6, 4, 4, 0, 5,  // 기본 스탯
			70, 40, 0, 60, 50, 40, 25, 0  // 성장률
		},
		// Mage
		{
			UnitClassType::Mage, "Mage", MovementType::Infantry,
			16, 0, 7, 6, 7, 5, 3, 7, 5,  // 기본 스탯
			70, 0, 60, 50, 50, 45, 20, 60  // 성장률
		},
		// Fighter
		{
			UnitClassType::Fighter, "Fighter", MovementType::Infantry,
			20, 7, 0, 6, 7, 4, 5, 0, 5,  // 기본 스탯
			85, 55, 0, 50, 50, 35, 30, 0  // 성장률
		},
		// Mercenary
		{
			UnitClassType::Mercenary, "Mercenary", MovementType::Infantry,
			18, 6, 0, 7, 8, 5, 5, 0, 5,  // 기본 스탯
			75, 45, 0, 55, 55, 40, 30, 0  // 성장률
		}
	};

	const UnitClassInfo& GetUnitClassInfo(UnitClassType classType)
	{
		return unitClassData[static_cast<int>(classType)];
	}

	char GetUnitClassCharacter(UnitClassType classType)
	{
		switch (classType)
		{
		case UnitClassType::Lord:
			return 'L';
		case UnitClassType::Cavalier:
			return 'C';
		case UnitClassType::Knight:
			return 'K';
		case UnitClassType::Archer:
			return 'A';
		case UnitClassType::Mage:
			return 'M';
		case UnitClassType::Fighter:
			return 'F';
		case UnitClassType::Mercenary:
			return 'R'; // meRcenary
		default:
			return '?';
		}
	}
}
