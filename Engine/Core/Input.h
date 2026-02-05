#pragma once

#include "Common/Common.h"
#include "Math/Vector2.h"

namespace FE
{
	class FE_API Input
	{
		friend class Engine;

		// ������.
		// Down/Up/Key
		// Tip: struct / class ����� ���̴� ����
		// Tip: struct �� �����Ϳ� ����, �Լ��� ���� �� ������ ������.
		struct KeyState
		{
			// ���� Ű�� ���ȴ���.
			bool isKeyDown = false;

			// ������ Ű�� ���ȴ���.
			bool wasKeyDown = false;
		};

	private:
		Input();
		~Input();

	public:
		// �Է� Ȯ�� �Լ�.
		// ������ �Է��� �ȵƴµ�, ���� �Է��� ������ 1�� ȣ��.
		bool GetKeyDown(int keyCode);

		// ������ �Է��� �ƴµ�, ���� �Է��� ��ҵ����� 1�� ȣ��.
		bool GetKeyUp(int keyCode);

		// ���� ���������� �ݺ� ȣ��.
		bool GetKey(int keyCode);

		// ���콺 ��ư Ŭ�� ���� Ȯ��.
		// buttonCode: 0 -> ���� ��ư, 1 -> ������ ��ư.
		bool GetMouseButtonDown(int buttonCode);

		// ���콺 ��ư Ŭ�� ���� ���� Ȯ��.
		bool GetMouseButtonUp(int buttonCode);

		// ���콺 ���� ���� Ȯ��.
		bool GetMouseButton(int buttonCode);

		// ���������� �����ϴ� �Լ�.
		// �����͸� �ּҸ� ��� �ؼ� ������ ���� ��� ������,
		// ���۷����� ��ó�� �� �� �־ ������ ����
		static Input& Get();

		// ���콺 Ŀ�� ��ġ Ȯ�� �Լ�.
		inline Vector2 MousePosition() const { return mousePosition; }

		// ??? ??? ??? ??? ?? (gridOffsetX, gridOffsetY: ???? ???? ???? ??)
		Vector2 ScreenToGrid(int gridOffsetX, int gridOffsetY) const;

		// ?? ??? ??? ?????? ??
		bool IsGridClicked(int gridX, int gridY, int gridOffsetX, int gridOffsetY) const;

	private:
		// �Է� ó�� �Լ�.
		void ProcessInput();

		// ���� �Է� ���� �Լ�.
		void SavePreviousInputStates();

	private:
		// Ű ���� ����� �迭.
		// (255������ ���� ���� ��� Ű����� ���콺 �Է� ó�� ����)
		KeyState keyStates[255] = { };

		// ���콺 Ŀ�� ��ġ.
		Vector2 mousePosition;

		// ���������� �����ϵ��� ����� ���� ���� ����.
		static Input* instance;
	};
}
