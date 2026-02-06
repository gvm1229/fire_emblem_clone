#pragma once

#include "Unit/Unit.h"
#include "Math/Vector2.h"
#include <vector>

namespace FE
{
	// 전방 선언
	class MapLevel;
	class TurnManager;

	// 플레이어 입력 상태
	enum class PlayerInputState
	{
		Idle,					// 대기 중
		UnitSelected,			// 유닛 선택됨
		MovementSelected,		// 이동 위치 선택됨
		ActionMenuOpen,			// 행동 메뉴 열림
		TargetSelection			// 공격 대상 선택 중
	};

	class PlayerPhaseController
	{
	public:
		PlayerPhaseController(MapLevel* level, TurnManager* turnManager);
		~PlayerPhaseController();

		// 업데이트
		void Update(float deltaTime);

		// 입력 처리
		void HandleInput();

		// 상태별 처리
		void HandleIdleState();
		void HandleUnitSelectedState();
		void HandleMovementSelectedState();
		void HandleActionMenuState();
		void HandleTargetSelectionState();

		// 이동 가능 범위 계산 (간단 버전)
		std::vector<Vector2> CalculateMoveRange(Unit* unit);

		// 공격 가능 범위 계산
		std::vector<Vector2> CalculateAttackRange(Unit* unit);

		// 턴 종료 체크
		bool IsPlayerPhaseDone();

	private:
		MapLevel* level;
		TurnManager* turnManager;

		PlayerInputState inputState;

		// 선택된 유닛
		Unit* selectedUnit;

		// 이동 위치
		Vector2 selectedMovePosition;

		// 이동 가능 범위
		std::vector<Vector2> moveRange;

		// 공격 가능 범위
		std::vector<Vector2> attackRange;

		// 선택된 행동 (0: Attack, 1: Wait)
		int selectedAction;

		// 그리드 오프셋 (화면에서 그리드가 시작하는 위치)
		int gridOffsetX;
		int gridOffsetY;
	};
}
