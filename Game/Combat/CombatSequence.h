#pragma once

#include "Unit/Unit.h"
#include "Combat/CombatCalculator.h"

namespace FE
{
	class MapLevel;

	class CombatSequence
	{
	public:
		// 전투 시작
		static void InitiateCombat(Unit* attacker, Unit* defender, MapLevel* level);

		// 단일 공격 실행
		static bool ExecuteAttack(Unit* attacker, Unit* defender);

		// 반격 실행
		static void ExecuteCounterAttack(Unit* attacker, Unit* defender);

		// 더블 어택 실행
		static void ExecuteDoubleAttack(Unit* attacker, Unit* defender);

		// 전투 결과 표시 (콘솔 출력)
		static void ShowCombatResult(Unit* attacker, Unit* defender, int damage, bool isHit, bool isCrit);

	private:
		// 난수 생성 (0~99)
		static int Random100();
	};
}
