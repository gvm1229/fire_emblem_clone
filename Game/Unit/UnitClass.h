#pragma once

namespace FEClone
{
	// 유닛 클래스 (병과)
	enum class UnitClass
	{
		Lord,           // 로드 (주인공) - 'L'
		Cavalier,       // 기병 - 'C'
		Knight,         // 기사 - 'K'
		Archer,         // 궁수 - 'A'
		Mage,           // 마법사 - 'M'
		Fighter,        // 전사 - 'F'
		Soldier,        // 병사 - 'S'
		Mercenary,      // 용병 - 'R'
		Thief,          // 도적 - 'T'
		Cleric          // 성직자 - 'P'
	};

	// 유닛 클래스에 따른 표시 문자열 반환 (UTF-8)
	inline const char* GetUnitClassString(UnitClass unitClass)
	{
		switch (unitClass)
		{
		case UnitClass::Lord:       return "L";
		case UnitClass::Cavalier:   return "C";
		case UnitClass::Knight:     return "K";
		case UnitClass::Archer:     return "A";
		case UnitClass::Mage:       return "M";
		case UnitClass::Fighter:    return "F";
		case UnitClass::Soldier:    return "S";
		case UnitClass::Mercenary:  return "R";
		case UnitClass::Thief:      return "T";
		case UnitClass::Cleric:     return "P";
		default:                    return "?";
		}
	}

	// 유닛 클래스에 따른 전체 이름 반환 (스탯 표시용)
	inline const char* GetFullUnitClassString(UnitClass unitClass)
	{
		switch (unitClass)
		{
		case UnitClass::Lord:       return "Lord";
		case UnitClass::Cavalier:   return "Cavalier";
		case UnitClass::Knight:     return "Knight";
		case UnitClass::Archer:     return "Archer";
		case UnitClass::Mage:       return "Mage";
		case UnitClass::Fighter:    return "Fighter";
		case UnitClass::Soldier:    return "Soldier";
		case UnitClass::Mercenary:  return "Mercenary";
		case UnitClass::Thief:      return "Thief";
		case UnitClass::Cleric:     return "Cleric";
		default:                    return "Unknown";
		}
	}
}
