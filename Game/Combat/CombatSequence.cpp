#include "CombatSequence.h"
#include "Level/MapLevel.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace FE
{
	// 전투 시작
	void CombatSequence::InitiateCombat(Unit* attacker, Unit* defender, MapLevel* level)
	{
		printf("\n=== COMBAT START ===\n");
		printf("%s attacks %s!\n", attacker->GetUnitName(), defender->GetUnitName());

		// 전투 예측 계산
		Tile* attackerTile = level->GetGrid()->GetTile(attacker->GetGridX(), attacker->GetGridY());
		Tile* defenderTile = level->GetGrid()->GetTile(defender->GetGridX(), defender->GetGridY());

		CombatForecast forecast = CombatCalculator::CalculateCombatForecast(
			attacker, defender, attackerTile, defenderTile
		);

		// 1. 공격자의 공격
		bool attackerHit = ExecuteAttack(attacker, defender);

		// 방어자가 사망했는지 확인
		if (!defender->IsAlive())
		{
			printf("%s has been defeated!\n", defender->GetUnitName());
			// 공격자가 경험치 획득
			attacker->GainExperience(30); // 적 처치 시 30 EXP
			printf("=== COMBAT END ===\n\n");
			return;
		}

		// 2. 방어자의 반격
		if (forecast.defenderCanCounter)
		{
			ExecuteCounterAttack(defender, attacker);

			// 공격자가 사망했는지 확인
			if (!attacker->IsAlive())
			{
				printf("%s has been defeated!\n", attacker->GetUnitName());
				printf("=== COMBAT END ===\n\n");
				return;
			}
		}
		else
		{
			printf("%s cannot counter-attack!\n", defender->GetUnitName());
		}

		// 3. 공격자의 더블 어택
		if (forecast.attackerDoubles && attackerHit)
		{
			ExecuteDoubleAttack(attacker, defender);

			if (!defender->IsAlive())
			{
				printf("%s has been defeated!\n", defender->GetUnitName());
				attacker->GainExperience(30);
				printf("=== COMBAT END ===\n\n");
				return;
			}
		}

		// 4. 방어자의 더블 어택
		if (forecast.defenderDoubles && forecast.defenderCanCounter)
		{
			ExecuteDoubleAttack(defender, attacker);

			if (!attacker->IsAlive())
			{
				printf("%s has been defeated!\n", attacker->GetUnitName());
				printf("=== COMBAT END ===\n\n");
				return;
			}
		}

		// 전투 종료 (둘 다 생존)
		attacker->GainExperience(10); // 전투 참여 시 10 EXP
		printf("=== COMBAT END ===\n\n");
	}

	// 단일 공격 실행
	bool CombatSequence::ExecuteAttack(Unit* attacker, Unit* defender)
	{
		Weapon* weapon = attacker->GetEquippedWeapon();
		if (weapon == nullptr)
		{
			printf("%s has no weapon!\n", attacker->GetUnitName());
			return false;
		}

		// 명중률 계산
		int hitRate = CombatCalculator::CalculateHitRate(attacker, defender, nullptr);
		int critRate = CombatCalculator::CalculateCritical(attacker, defender);

		// 난수 생성하여 명중 판정
		int hitRoll = Random100();
		bool isHit = hitRoll < hitRate;

		if (!isHit)
		{
			printf("%s's attack missed!\n", attacker->GetUnitName());
			ShowCombatResult(attacker, defender, 0, false, false);
			return false;
		}

		// 크리티컬 판정
		int critRoll = Random100();
		bool isCrit = critRoll < critRate;

		// 데미지 계산
		int damage = CombatCalculator::CalculateDamage(attacker, defender);
		if (isCrit)
		{
			damage *= 3; // 크리티컬은 데미지 3배
		}

		// 데미지 적용
		defender->TakeDamage(damage);

		// 무기 내구도 감소
		weapon->Use();

		// 결과 표시
		ShowCombatResult(attacker, defender, damage, isHit, isCrit);

		return true;
	}

	// 반격 실행
	void CombatSequence::ExecuteCounterAttack(Unit* attacker, Unit* defender)
	{
		printf("%s counter-attacks!\n", attacker->GetUnitName());
		ExecuteAttack(attacker, defender);
	}

	// 더블 어택 실행
	void CombatSequence::ExecuteDoubleAttack(Unit* attacker, Unit* defender)
	{
		printf("%s attacks again! (Double Attack)\n", attacker->GetUnitName());
		ExecuteAttack(attacker, defender);
	}

	// 전투 결과 표시
	void CombatSequence::ShowCombatResult(Unit* attacker, Unit* defender, int damage, bool isHit, bool isCrit)
	{
		if (isHit)
		{
			if (isCrit)
			{
				printf("  CRITICAL! %s deals %d damage to %s! (HP: %d/%d)\n",
					attacker->GetUnitName(), damage, defender->GetUnitName(),
					defender->GetCurrentHP(), defender->GetMaxHP());
			}
			else
			{
				printf("  %s deals %d damage to %s! (HP: %d/%d)\n",
					attacker->GetUnitName(), damage, defender->GetUnitName(),
					defender->GetCurrentHP(), defender->GetMaxHP());
			}
		}
		else
		{
			printf("  Miss!\n");
		}
	}

	// 난수 생성 (0~99)
	int CombatSequence::Random100()
	{
		static bool seeded = false;
		if (!seeded)
		{
			srand(static_cast<unsigned int>(time(nullptr)));
			seeded = true;
		}
		return rand() % 100;
	}
}
