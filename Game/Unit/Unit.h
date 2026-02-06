#pragma once

#include "Actor/Actor.h"
#include "UnitClass.h"

namespace FE
{
	// 전방 선언
	class Weapon;
	class Item;

	class Unit : public Actor
	{
		RTTI_DECLARATIONS(Unit, Actor)

	public:
		Unit(
			UnitClassType classType,
			UnitFaction faction,
			int gridX,
			int gridY,
			const char* unitName = "Unit"
		);
		virtual ~Unit();

		// Actor 오버라이드
		virtual void BeginPlay() override;
		virtual void Tick(float deltaTime) override;
		virtual void Draw() override;

		// 유닛 이동
		void MoveTo(int x, int y);
		bool CanMoveTo(int x, int y) const;

		// 데미지 처리
		void TakeDamage(int damage);
		bool IsAlive() const;

		// 경험치 및 레벨업
		void GainExperience(int exp);
		bool CheckLevelUp();
		void LevelUp();

		// 행동 완료 플래그
		inline bool IsActionDone() const { return isActionDone; }
		inline void SetActionDone(bool done) { isActionDone = done; }

		// Getter
		inline int GetGridX() const { return gridX; }
		inline int GetGridY() const { return gridY; }
		inline UnitClassType GetClassType() const { return classType; }
		inline UnitFaction GetFaction() const { return faction; }
		inline const char* GetUnitName() const { return unitName; }

		inline int GetCurrentHP() const { return currentHP; }
		inline int GetMaxHP() const { return maxHP; }
		inline int GetStr() const { return str; }
		inline int GetMag() const { return mag; }
		inline int GetSkl() const { return skl; }
		inline int GetSpd() const { return spd; }
		inline int GetLck() const { return lck; }
		inline int GetDef() const { return def; }
		inline int GetRes() const { return res; }
		inline int GetMov() const { return mov; }

		inline int GetLevel() const { return level; }
		inline int GetExperience() const { return experience; }

		// 무기 관련 (간단히 포인터로만 보관)
		inline Weapon* GetEquippedWeapon() const { return equippedWeapon; }
		inline void SetEquippedWeapon(Weapon* weapon) { equippedWeapon = weapon; }

	protected:
		// 그리드 좌표
		int gridX;
		int gridY;

		// 유닛 클래스 및 소속
		UnitClassType classType;
		UnitFaction faction;
		const char* unitName;

		// 스탯
		int maxHP;
		int currentHP;
		int str;	// Strength (물리 공격력)
		int mag;	// Magic (마법 공격력)
		int skl;	// Skill (명중률, 크리티컬)
		int spd;	// Speed (회피율, 더블 어택)
		int lck;	// Luck
		int def;	// Defense (물리 방어력)
		int res;	// Resistance (마법 방어력)
		int mov;	// Movement (이동력)

		// 레벨 및 경험치
		int level;
		int experience;

		// 행동 완료 여부
		bool isActionDone;

		// 장비한 무기 (간단히 하나만)
		Weapon* equippedWeapon;
	};
}
