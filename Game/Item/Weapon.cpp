#include "Weapon.h"
#include <stdio.h>

namespace FE
{
	Weapon::Weapon(
		const char* name,
		WeaponType type,
		int might,
		int hit,
		int crit,
		int weight,
		int minRange,
		int maxRange,
		int maxDurability
	)
		: name(name)
		, type(type)
		, might(might)
		, hit(hit)
		, crit(crit)
		, weight(weight)
		, minRange(minRange)
		, maxRange(maxRange)
		, durability(maxDurability)
		, maxDurability(maxDurability)
	{
	}

	Weapon::~Weapon()
	{
	}

	// 무기 사용 시 내구도 감소
	void Weapon::Use()
	{
		if (durability > 0)
		{
			durability--;
		}

		if (IsBroken())
		{
			printf("[Weapon::Use] %s is broken!\n", name);
		}
	}

	// 기본 무기 생성 함수들
	Weapon* CreateIronSword()
	{
		return new Weapon("Iron Sword", WeaponType::Sword, 5, 90, 0, 5, 1, 1, 46);
	}

	Weapon* CreateIronLance()
	{
		return new Weapon("Iron Lance", WeaponType::Lance, 7, 80, 0, 8, 1, 1, 45);
	}

	Weapon* CreateIronAxe()
	{
		return new Weapon("Iron Axe", WeaponType::Axe, 8, 75, 0, 10, 1, 1, 45);
	}

	Weapon* CreateIronBow()
	{
		return new Weapon("Iron Bow", WeaponType::Bow, 6, 80, 0, 5, 2, 2, 45);
	}

	Weapon* CreateFire()
	{
		return new Weapon("Fire", WeaponType::Magic, 5, 90, 0, 4, 1, 2, 40);
	}
}
