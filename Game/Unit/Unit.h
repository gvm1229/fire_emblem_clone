#pragma once

#include "Actor/Actor.h"
#include "UnitStats.h"
#include "UnitClass.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include <deque>

namespace FEClone
{
	// 유닛 진영
	enum class Faction
	{
		Player,     // 아군
		Enemy,      // 적군
		Ally,       // 동맹군 (AI가 조종하지만 플레이어 편)
	};

	// 유닛 상태
	enum class UnitState
	{
		Idle,       // 대기 중
		Selected,   // 선택됨
		Moving,     // 이동 중
		Acting,     // 행동 중 (공격, 아이템 사용 등)
		Done        // 턴 종료 (회색 처리)
	};

	// 유닛 클래스 - Fire Emblem의 캐릭터 유닛
	class Unit : public Actor
	{
		RTTI_DECLARATIONS(Unit, Actor)

	public:
		Unit(UnitClass unitClass = UnitClass::Soldier);
		virtual ~Unit() override;

		// Actor 오버라이드
		virtual void BeginPlay() override;
		virtual void Tick(float deltaTime) override;
		virtual void Draw() override;

		// 스탯 관련
		UnitStats& GetStats() { return stats; }
		const UnitStats& GetStats() const { return stats; }

		// 진영 관련
		Faction GetFaction() const { return faction; }
		void SetFaction(Faction newFaction) { faction = newFaction; }

		// 클래스 관련
		UnitClass GetUnitClass() const { return unitClass; }
		void SetUnitClass(UnitClass newClass);


		// 상태 관련
		UnitState GetState() const { return state; }
		void SetState(UnitState newState) { state = newState; }

		// 그리드 위치 관련 (렌더링 위치와 별개)
		Vector2 GetGridPosition() const { return gridPosition; }
		void SetGridPosition(const Vector2& pos) { gridPosition = pos; }

		// 이동 관련
		void SetPath(const std::deque<Vector2>& newPath);
		bool IsMoving() const { return state == UnitState::Moving; }
		void UpdateMovement(float deltaTime);

		// 턴 종료
		void EndTurn() { state = UnitState::Done; }
		void ResetTurn() { state = UnitState::Idle; }
		bool HasActedThisTurn() const { return state == UnitState::Done; }

		// 렌더링 정보
		const char* GetDisplayString() const { return displayStr; }
		void SetDisplayString(const char* str);
		Color GetDisplayColor() const;

		// 숫자 키 인덱스 (플레이어 유닛만 사용: 0~9)
		int GetUnitIndex() const { return unitIndex; }
		void SetUnitIndex(int index) { unitIndex = index; }

	private:
		UnitStats stats;                // 유닛 스탯
		Faction faction;                // 진영
		UnitClass unitClass;            // 병과
		UnitState state;                // 현재 상태
		Vector2 gridPosition;           // 그리드 상의 위치

		// 이동 관련
		std::deque<Vector2> movementPath;  // 이동 경로
		float moveSpeed;                   // 이동 속도 (타일/초)
		float moveTimer;                   // 이동 타이머

		// 렌더링 정보 (멀티라인 ASCII 대비)
		char displayStr[8];                // UTF-8 문자열 (최대 4바이트 + null)
		int unitIndex;                     // 숫자 키 인덱스 (-1이면 미할당)
	};
}
