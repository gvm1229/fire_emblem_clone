#pragma once

#include "Common/Common.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

namespace FEClone
{
	// 지형 타입 열거형
	enum class TerrainType
	{
		Plain,      // 평지: 이동 비용 1, 회피 +0
		Forest,     // 숲: 이동 비용 2, 회피 +20
		Mountain,   // 산: 이동 비용 3, 회피 +30
		Castle,     // 성: 이동 비용 1, 회피 +20, 방어 +2, 체력 회복
		Village,    // 마을: 이동 비용 1, 회피 +10
		Water,      // 물: 일반 유닛 통과 불가 (특정 클래스만 가능)
		Wall        // 벽: 통과 불가
	};

	// 타일 클래스 - 각 타일의 지형 정보를 담음
	class Tile
	{
	public:
		// 생성자
		Tile(int x, int y, TerrainType type);
		~Tile() = default;

		// 위치 반환
		Vector2 GetPosition() const { return position; }

		// 지형 타입 반환
		TerrainType GetTerrainType() const { return terrainType; }

		// 이동 비용 반환 (A* 알고리즘에서 사용)
		int GetMovementCost() const { return movementCost; }

		// 회피 보너스 반환
		int GetAvoidBonus() const { return avoidBonus; }

		// 방어 보너스 반환
		int GetDefenseBonus() const { return defenseBonus; }

		// 체력 회복 여부 반환
		bool IsHealingTile() const { return isHealing; }

		// 통과 가능 여부 반환 (일반 유닛 기준)
		bool IsWalkable() const { return isWalkable; }

		// 현재 타일에 유닛이 있는지 여부
		bool HasUnit() const { return hasUnit; }
		void SetHasUnit(bool value) { hasUnit = value; }

	// 렌더링용 문자열 반환
	const char* GetDisplayString() const { return displayStr; }

	// 렌더링용 색상 반환
	Color GetDisplayColor() const { return displayColor; }

private:
	Vector2 position;              // 타일 위치 (그리드 좌표)
	TerrainType terrainType;       // 지형 타입
	int movementCost;              // 이동 비용 (1~3)
	int avoidBonus;                // 회피 보너스 (0~30)
	int defenseBonus;              // 방어 보너스 (0~2)
	bool isHealing;                // 체력 회복 여부
	bool isWalkable;               // 통과 가능 여부
	bool hasUnit;                  // 유닛 존재 여부

	// 렌더링 정보 (멀티라인 ASCII 대비)
	char displayStr[8];            // UTF-8 문자열 (최대 4바이트 + null)
	Color displayColor;            // 색상
	};
}
