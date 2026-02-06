#define NOMINMAX
#include "CombatCalculator.h"
#include "Item/Weapon.h"
#include <algorithm>

namespace FE
{
	// 전투 예측 계산
	CombatForecast CombatCalculator::CalculateCombatForecast(
		Unit* attacker,
		Unit* defender,
		Tile* attackerTile,
		Tile* defenderTile
	)
	{
		CombatForecast forecast;

		// 데미지 계산
		forecast.attackerDamage = CalculateDamage(attacker, defender);
		forecast.defenderDamage = CalculateDamage(defender, attacker);

		// 명중률 계산
		forecast.attackerHitRate = CalculateHitRate(attacker, defender, defenderTile);
		forecast.defenderHitRate = CalculateHitRate(defender, attacker, attackerTile);

		// 크리티컬 확률 계산
		forecast.attackerCritRate = CalculateCritical(attacker, defender);
		forecast.defenderCritRate = CalculateCritical(defender, attacker);

		// 더블 어택 여부
		forecast.attackerDoubles = CanDoubleAttack(attacker, defender);
		forecast.defenderDoubles = CanDoubleAttack(defender, attacker);

		// 반격 가능 여부
		forecast.defenderCanCounter = CanCounter(attacker, defender);

		return forecast;
	}

	// 명중률 계산
	int CombatCalculator::CalculateHitRate(Unit* attacker, Unit* defender, Tile* defenderTile)
	{
		Weapon* weapon = attacker->GetEquippedWeapon();
		if (weapon == nullptr)
		{
			return 0;
		}

		// 기본 명중률 = 무기 명중 + (Skl * 2) + (Lck / 2)
		int baseHit = weapon->GetHit() + (attacker->GetSkl() * 2) + (attacker->GetLck() / 2);

		// 회피율 계산
		int avoid = CalculateAvoid(defender, defenderTile);

		// 최종 명중률 = 기본 명중률 - 회피율
		int hitRate = baseHit - avoid;

		// 무기 삼각 관계 보너스
		if (defender->GetEquippedWeapon() != nullptr)
		{
			hitRate += WeaponTriangleBonus(weapon->GetType(), defender->GetEquippedWeapon()->GetType()) * 15;
		}

		// 명중률은 0~100 사이
		hitRate = std::max(0, std::min(100, hitRate));

		return hitRate;
	}

	// 회피율 계산
	int CombatCalculator::CalculateAvoid(Unit* unit, Tile* tile)
	{
		// 회피율 = (Spd * 2) + Lck + 지형 보너스
		int avoid = (unit->GetSpd() * 2) + unit->GetLck();

		if (tile != nullptr)
		{
			avoid += tile->GetAvoidBonus();
		}

		return avoid;
	}

	// 데미지 계산
	int CombatCalculator::CalculateDamage(Unit* attacker, Unit* defender)
	{
		Weapon* weapon = attacker->GetEquippedWeapon();
		if (weapon == nullptr)
		{
			return 0;
		}

		int attackPower = 0;
		int defense = 0;

		// 물리 또는 마법 공격 판단
		if (weapon->GetType() == WeaponType::Magic)
		{
			attackPower = attacker->GetMag() + weapon->GetMight();
			defense = defender->GetRes();
		}
		else
		{
			attackPower = attacker->GetStr() + weapon->GetMight();
			defense = defender->GetDef();
		}

		// 데미지 = 공격력 - 방어력 (최소 0)
		int damage = attackPower - defense;
		damage = std::max(0, damage);

		return damage;
	}

	// 크리티컬 확률 계산
	int CombatCalculator::CalculateCritical(Unit* attacker, Unit* defender)
	{
		Weapon* weapon = attacker->GetEquippedWeapon();
		if (weapon == nullptr)
		{
			return 0;
		}

		// 크리티컬률 = 무기 크리티컬 + (Skl / 2)
		int critRate = weapon->GetCrit() + (attacker->GetSkl() / 2);

		// 상대의 Lck으로 감소
		critRate -= defender->GetLck();

		// 크리티컬률은 0~100 사이
		critRate = std::max(0, std::min(100, critRate));

		return critRate;
	}

	// 더블 어택 가능 여부
	bool CombatCalculator::CanDoubleAttack(Unit* attacker, Unit* defender)
	{
		// Spd가 4 이상 차이나면 더블 어택 가능
		return (attacker->GetSpd() - defender->GetSpd()) >= 4;
	}

	// 무기 삼각 관계 보너스
	int CombatCalculator::WeaponTriangleBonus(WeaponType attackerWeapon, WeaponType defenderWeapon)
	{
		// Sword > Axe > Lance > Sword
		if (attackerWeapon == WeaponType::Sword && defenderWeapon == WeaponType::Axe)
			return 1;  // 유리
		if (attackerWeapon == WeaponType::Axe && defenderWeapon == WeaponType::Lance)
			return 1;
		if (attackerWeapon == WeaponType::Lance && defenderWeapon == WeaponType::Sword)
			return 1;

		if (attackerWeapon == WeaponType::Sword && defenderWeapon == WeaponType::Lance)
			return -1; // 불리
		if (attackerWeapon == WeaponType::Axe && defenderWeapon == WeaponType::Sword)
			return -1;
		if (attackerWeapon == WeaponType::Lance && defenderWeapon == WeaponType::Axe)
			return -1;

		return 0; // 중립
	}

	// 반격 가능 여부 (사거리 체크)
	bool CombatCalculator::CanCounter(Unit* attacker, Unit* defender)
	{
		Weapon* defenderWeapon = defender->GetEquippedWeapon();
		if (defenderWeapon == nullptr)
		{
			return false;
		}

		// 공격자와 방어자 사이의 거리 계산
		int distance = CalculateDistance(
			attacker->GetGridX(), attacker->GetGridY(),
			defender->GetGridX(), defender->GetGridY()
		);

		// 방어자의 무기 사거리 내에 있는지 확인
		return distance >= defenderWeapon->GetMinRange() && distance <= defenderWeapon->GetMaxRange();
	}

	// 거리 계산 (맨해튼 거리)
	int CombatCalculator::CalculateDistance(int x1, int y1, int x2, int y2)
	{
		int dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);
		int dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);
		return dx + dy;
	}
}
