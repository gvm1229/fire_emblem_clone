#pragma once

#include "Common/Common.h"

namespace FEClone
{
	// 유닛의 기본 스탯 구조체
	struct UnitStats
	{
		int maxHP;          // 최대 체력
		int currentHP;      // 현재 체력
		int strength;       // 힘 (물리 공격력)
		int magic;          // 마력 (마법 공격력)
		int skill;          // 기술 (명중률, 치명타율 영향)
		int speed;          // 속도 (회피율, 더블 어택 판정)
		int luck;           // 행운 (회피율, 치명타 회피)
		int defense;        // 수비 (물리 방어)
		int resistance;     // 마방 (마법 방어)
		int movement;       // 이동력

		// 생성자
		UnitStats()
			: maxHP(20)
			, currentHP(20)
			, strength(5)
			, magic(0)
			, skill(5)
			, speed(5)
			, luck(0)
			, defense(3)
			, resistance(0)
			, movement(5)
		{
		}
	};
}
