#include "Unit.h"
#include "Render/Renderer.h"
#include <stdio.h>
#include <stdlib.h>

namespace FE
{
	RTTI_DEFINITIONS(Unit)

	Unit::Unit(
		UnitClassType classType,
		UnitFaction faction,
		int gridX,
		int gridY,
		const char* unitName
	)
		: Actor("", Vector2::Zero, Color::White)
		, classType(classType)
		, faction(faction)
		, gridX(gridX)
		, gridY(gridY)
		, unitName(unitName)
		, level(1)
		, experience(0)
		, isActionDone(false)
		, equippedWeapon(nullptr)
	{
		// 클래스 정보 가져오기
		const UnitClassInfo& info = GetUnitClassInfo(classType);

		// 기본 스탯 초기화
		maxHP = info.baseHP;
		currentHP = maxHP;
		str = info.baseStr;
		mag = info.baseMag;
		skl = info.baseSkl;
		spd = info.baseSpd;
		lck = info.baseLck;
		def = info.baseDef;
		res = info.baseRes;
		mov = info.baseMov;

		// 이미지 설정 (클래스 문자)
		char classChar = GetUnitClassCharacter(classType);
		char* img = new char[2];
		img[0] = classChar;
		img[1] = '\0';
		ChangeImage(img);
		delete[] img;

		// sortingOrder 설정 (유닛은 타일보다 위에 그려져야 함)
		sortingOrder = 10;
	}

	Unit::~Unit()
	{
	}

	void Unit::BeginPlay()
	{
		Actor::BeginPlay();
	}

	void Unit::Tick(float deltaTime)
	{
		Actor::Tick(deltaTime);

		// 화면 위치 업데이트 (그리드 좌표를 화면 좌표로 변환)
		// 그리드는 (1, 1)부터 시작한다고 가정 (0, 0)은 UI용
		SetPosition(Vector2(gridX + 1, gridY + 1));
	}

	void Unit::Draw()
	{
		Actor::Draw();
	}

	// 유닛을 특정 그리드 좌표로 이동
	void Unit::MoveTo(int x, int y)
	{
		if (!CanMoveTo(x, y))
		{
			printf("[Unit::MoveTo] %s cannot move to (%d, %d)\n", unitName, x, y);
			return;
		}

		gridX = x;
		gridY = y;
	}

	// 이동 가능 여부 확인 (현재는 간단히 true 반환, 나중에 범위 체크 추가)
	bool Unit::CanMoveTo(int x, int y) const
	{
		// TODO: 이동 범위, 장애물 체크 등
		return true;
	}

	// 데미지를 받음
	void Unit::TakeDamage(int damage)
	{
		currentHP -= damage;
		if (currentHP < 0)
		{
			currentHP = 0;
		}

		printf("[Unit::TakeDamage] %s took %d damage. HP: %d/%d\n",
			unitName, damage, currentHP, maxHP);

		if (!IsAlive())
		{
			printf("[Unit::TakeDamage] %s has been defeated!\n", unitName);
			// 사망 처리
			Destroy();
		}
	}

	// 생존 여부
	bool Unit::IsAlive() const
	{
		return currentHP > 0;
	}

	// 경험치 획득
	void Unit::GainExperience(int exp)
	{
		experience += exp;
		printf("[Unit::GainExperience] %s gained %d EXP. Total: %d/100\n",
			unitName, exp, experience);

		// 레벨업 체크
		while (CheckLevelUp())
		{
			LevelUp();
		}
	}

	// 레벨업 가능 여부 체크
	bool Unit::CheckLevelUp()
	{
		return experience >= 100;
	}

	// 레벨업 처리
	void Unit::LevelUp()
	{
		experience -= 100;
		level++;

		printf("[Unit::LevelUp] %s leveled up to Level %d!\n", unitName, level);

		// 클래스 정보 가져오기
		const UnitClassInfo& info = GetUnitClassInfo(classType);

		// 성장률에 따라 랜덤으로 스탯 증가
		int hpGain = (rand() % 100 < info.growthHP) ? 1 : 0;
		int strGain = (rand() % 100 < info.growthStr) ? 1 : 0;
		int magGain = (rand() % 100 < info.growthMag) ? 1 : 0;
		int sklGain = (rand() % 100 < info.growthSkl) ? 1 : 0;
		int spdGain = (rand() % 100 < info.growthSpd) ? 1 : 0;
		int lckGain = (rand() % 100 < info.growthLck) ? 1 : 0;
		int defGain = (rand() % 100 < info.growthDef) ? 1 : 0;
		int resGain = (rand() % 100 < info.growthRes) ? 1 : 0;

		maxHP += hpGain;
		currentHP += hpGain; // 레벨업 시 HP도 회복
		str += strGain;
		mag += magGain;
		skl += sklGain;
		spd += spdGain;
		lck += lckGain;
		def += defGain;
		res += resGain;

		printf("  HP +%d, Str +%d, Mag +%d, Skl +%d, Spd +%d, Lck +%d, Def +%d, Res +%d\n",
			hpGain, strGain, magGain, sklGain, spdGain, lckGain, defGain, resGain);
	}
}
