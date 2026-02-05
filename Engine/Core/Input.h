#pragma once

#include "Common/Common.h"
#include "Math/Vector2.h"

namespace FE
{
	class FE_API Input
	{
		friend class Engine;

		// 데이터.
		// Down/Up/Key
		// Tip: struct / class 기능적 차이는 없음
		// Tip: struct 는 데이터에 쓰임, 함수를 가질 순 있으나 데이터.
		struct KeyState
		{
			// 현재 키가 눌렸는지.
			bool isKeyDown = false;

			// 이전에 키가 눌렸는지.
			bool wasKeyDown = false;
		};

	private:
		Input();
		~Input();

	public:
		// 입력 확인 함수.
		// 이전에 입력이 안됐는데, 현재 입력이 됐으면 1번 호출.
		bool GetKeyDown(int keyCode);

		// 이전에 입력이 됐는데, 현재 입력이 취소됐으면 1번 호출.
		bool GetKeyUp(int keyCode);

		// 현재 눌려있으면 반복 호출.
		bool GetKey(int keyCode);

		// 마우스 버튼 클릭 여부 확인.
		// buttonCode: 0 -> 왼쪽 버튼, 1 -> 오른쪽 버튼.
		bool GetMouseButtonDown(int buttonCode);

		// 마우스 버튼 클릭 해제 여부 확인.
		bool GetMouseButtonUp(int buttonCode);

		// 마우스 눌림 상태 확인.
		bool GetMouseButton(int buttonCode);

		// 전역적으로 접근하는 함수.
		// 포인터면 주소를 줘야 해서 권한을 많이 줘야 하지만,
		// 레퍼런스면 값처럼 쓸 수 있어서 권한이 적음
		static Input& Get();

		// 마우스 커서 위치 확인 함수.
		inline Vector2 MousePosition() const { return mousePosition; }

		// ??? ??? ??? ??? ?? (gridOffsetX, gridOffsetY: ???? ???? ???? ??)
		Vector2 ScreenToGrid(int gridOffsetX, int gridOffsetY) const;

		// ?? ??? ??? ?????? ??
		bool IsGridClicked(int gridX, int gridY, int gridOffsetX, int gridOffsetY) const;

	private:
		// 입력 처리 함수.
		void ProcessInput();

		// 기존 입력 저장 함수.
		void SavePreviousInputStates();

	private:
		// 키 상태 저장용 배열.
		// (255가지면 문제 없이 모든 키보드와 마우스 입력 처리 가능)
		KeyState keyStates[255] = { };

		// 마우스 커서 위치.
		Vector2 mousePosition;

		// 전역적으로 접근하도록 만들기 위한 전역 변수.
		static Input* instance;
	};
}
