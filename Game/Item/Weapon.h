#pragma once

namespace FE
{
	// 무기 타입
	enum class WeaponType
	{
		Sword,		// 검
		Lance,		// 창
		Axe,		// 도끼
		Bow,		// 활
		Magic		// 마법
	};

	// 무기 클래스
	class Weapon
	{
	public:
		Weapon(
			const char* name,
			WeaponType type,
			int might,
			int hit,
			int crit,
			int weight,
			int minRange,
			int maxRange,
			int maxDurability
		);
		~Weapon();

		// 무기 사용 (내구도 감소)
		void Use();

		// Getter
		inline const char* GetName() const { return name; }
		inline WeaponType GetType() const { return type; }
		inline int GetMight() const { return might; }
		inline int GetHit() const { return hit; }
		inline int GetCrit() const { return crit; }
		inline int GetWeight() const { return weight; }
		inline int GetMinRange() const { return minRange; }
		inline int GetMaxRange() const { return maxRange; }
		inline int GetDurability() const { return durability; }
		inline int GetMaxDurability() const { return maxDurability; }

		// 내구도 체크
		inline bool IsBroken() const { return durability <= 0; }

	private:
		const char* name;
		WeaponType type;
		int might;		// 공격력
		int hit;		// 명중률
		int crit;		// 크리티컬률
		int weight;		// 무게
		int minRange;	// 최소 사거리
		int maxRange;	// 최대 사거리
		int durability;		// 현재 내구도
		int maxDurability;	// 최대 내구도
	};

	// 기본 무기 생성 함수들
	Weapon* CreateIronSword();
	Weapon* CreateIronLance();
	Weapon* CreateIronAxe();
	Weapon* CreateIronBow();
	Weapon* CreateFire();
}
