#pragma once

#include "Unit/Unit.h"
#include "Map/Tile.h"

namespace FE
{
	// 전투 계산 결과
	struct CombatForecast
	{
		int attackerDamage;
		int defenderDamage;
		int attackerHitRate;
		int defenderHitRate;
		int attackerCritRate;
		int defenderCritRate;
		bool attackerDoubles;
		bool defenderDoubles;
		bool defenderCanCounter;
	};

	class CombatCalculator
	{
	public:
		// 전투 예측 계산
		static CombatForecast CalculateCombatForecast(
			Unit* attacker,
			Unit* defender,
			Tile* attackerTile,
			Tile* defenderTile
		);

		// 명중률 계산
		static int CalculateHitRate(Unit* attacker, Unit* defender, Tile* defenderTile);

		// 회피율 계산
		static int CalculateAvoid(Unit* unit, Tile* tile);

		// 데미지 계산
		static int CalculateDamage(Unit* attacker, Unit* defender);

		// 크리티컬 확률 계산
		static int CalculateCritical(Unit* attacker, Unit* defender);

		// 더블 어택 가능 여부
		static bool CanDoubleAttack(Unit* attacker, Unit* defender);

		// 무기 삼각 관계 보너스
		static int WeaponTriangleBonus(WeaponType attackerWeapon, WeaponType defenderWeapon);

		// 반격 가능 여부 (사거리 체크)
		static bool CanCounter(Unit* attacker, Unit* defender);

		// 거리 계산
		static int CalculateDistance(int x1, int y1, int x2, int y2);
	};
}
