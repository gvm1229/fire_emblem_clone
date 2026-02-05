#pragma once

namespace FE
{
	// 유닛 클래스 타입
	enum class UnitClassType
	{
		Lord,		// 로드 (주인공)
		Cavalier,	// 기병
		Knight,		// 기사
		Archer,		// 궁수
		Mage,		// 마법사
		Fighter,	// 전사
		Mercenary	// 용병
	};

	// 유닛 소속
	enum class UnitFaction
	{
		Player,		// 플레이어
		Enemy,		// 적
		Ally		// 아군 NPC
	};

	// 이동 타입
	enum class MovementType
	{
		Infantry,	// 보병
		Cavalry,	// 기병
		Flying		// 비행
	};

	// 유닛 클래스 정보
	struct UnitClassInfo
	{
		UnitClassType classType;
		const char* className;
		MovementType movementType;
		int baseHP;
		int baseStr;
		int baseMag;
		int baseSkl;
		int baseSpd;
		int baseLck;
		int baseDef;
		int baseRes;
		int baseMov;

		// 성장률 (레벨업 시 스탯 증가 확률 %)
		int growthHP;
		int growthStr;
		int growthMag;
		int growthSkl;
		int growthSpd;
		int growthLck;
		int growthDef;
		int growthRes;
	};

	// 유닛 클래스 정보를 반환하는 함수
	const UnitClassInfo& GetUnitClassInfo(UnitClassType classType);

	// 유닛 클래스 표시 문자 반환
	char GetUnitClassCharacter(UnitClassType classType);
}
