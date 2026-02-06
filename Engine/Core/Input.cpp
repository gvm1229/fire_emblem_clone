#include "Input.h"
#include "Engine/Engine.h"
#include "Util/Util.h"
#include <Windows.h>
#include <iostream>
#include <cassert>

// Ctrl + HomeŰ�� ���� ���� ���� �̵� ����.
// Rider�� ��� �ڵ� �߰� ����.
namespace FE
{
	// ���� ���� �ʱ�ȭ.
	Input* Input::instance = nullptr;

	Input::Input()
	{
		// ���� ������ �ڱ� �ڽ� �Ҵ�
		// ��ü�� �ʱ�ȭ�Ǹ� �ڱ� �ڽ��� �ּҸ� ����.
		instance = this;
	}
	
	Input::~Input()
	{
	}

	bool Input::GetKeyDown(int keyCode)
	{
		return keyStates[keyCode].isKeyDown
			&& !keyStates[keyCode].wasKeyDown;
	}

	bool Input::GetKeyUp(int keyCode)
	{
		return !keyStates[keyCode].isKeyDown
			&& keyStates[keyCode].wasKeyDown;
	}

	bool Input::GetKey(int keyCode)
	{
		return keyStates[keyCode].isKeyDown;
	}

	bool Input::GetMouseButtonDown(int buttonCode)
	{
		// buttonCode�� 0�̰ų� 1���� Ȯ��.
		// �� �˻縦 ��� ���ϸ� debugbreak�� ������.
		assert(buttonCode == 0 || buttonCode == 1);

		int keyCode = 0;
		if (buttonCode == 0)
		{
			keyCode = VK_LBUTTON;
		}
		else if (buttonCode == 1)
		{
			keyCode = VK_RBUTTON;
		}

		return keyStates[keyCode].isKeyDown
			&& !keyStates[keyCode].wasKeyDown;
	}

	bool Input::GetMouseButtonUp(int buttonCode)
	{
		// buttonCode�� 0�̰ų� 1���� Ȯ��.
		// �� �˻縦 ��� ���ϸ� debugbreak�� ������.
		assert(buttonCode == 0 || buttonCode == 1);

		int keyCode = 0;
		if (buttonCode == 0)
		{
			keyCode = VK_LBUTTON;
		}
		else if (buttonCode == 1)
		{
			keyCode = VK_RBUTTON;
		}

		return !keyStates[keyCode].isKeyDown
			&& keyStates[keyCode].wasKeyDown;
	}

	bool Input::GetMouseButton(int buttonCode)
	{
		// buttonCode�� 0�̰ų� 1���� Ȯ��.
		// �� �˻縦 ��� ���ϸ� debugbreak�� ������.
		assert(buttonCode == 0 || buttonCode == 1);

		int keyCode = 0;
		if (buttonCode == 0)
		{
			keyCode = VK_LBUTTON;
		}
		else if (buttonCode == 1)
		{
			keyCode = VK_RBUTTON;
		}

		return keyStates[keyCode].isKeyDown;
	}

	Input& Input::Get()
	{
		// �̱��� (Singleton)
		// ��ü�� ��Ȯ�� �ϳ��� ����, �� ���Ƽ���, �� ����� �ȵ�
		// �� �Լ��� ������ ������Ʈ���� ������.
		// ���� ������ �̹� �ʱ�ȭ �Ϸ� ����.
		if (!instance)
		{
			//return *nullptr;
			std::cout << "Error: Input::Get(). instance is null\n";

			// ����� ��忡���� ������.
			// �ڵ����� �ߴ��� �ɸ�.
			__debugbreak();
		}

		// Lazy-Pattern.
		// ����Ƽ�� C++�� ����.
		//static Input instance;
		return *instance;
	}

	void Input::ProcessInput()
	{
		// �Է� �ڵ� ������ & ���콺 �Է� Ȱ��ȭ ����.
		static HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
		static bool initialized = false;

		if (!initialized)
		{
			// ���콺 �̺�Ʈ Ȱ��ȭ.
			DWORD mode = ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
			mode &= ~ENABLE_QUICK_EDIT_MODE;
			BOOL result = SetConsoleMode(inputHandle, mode);

			// ���� ������ ��� ����ó��.
			if (result == FALSE)
			{
				// ���� �޽��� ���.
				MessageBoxA(
					nullptr,
					"Input::ProcessInput() - Failed to set console mode",
					"Error",
					MB_OK
				);
				__debugbreak();
			}

			initialized = true;
		}

		// Ű����/���콺 �Է��� �б� ���� ����.
		const int recordCount = 256;

		// ������ �Է� �̺�Ʈ�� �����ϱ� ���� �迭.
		INPUT_RECORD records[recordCount] = {};

		// �̹� �����ӿ� ó���� �Է� �̺�Ʈ ��.
		DWORD eventReadCount = 0;

		// ���� �Է� �̺�Ʈ�� �ִ��� Ȯ��.
		if (PeekConsoleInput(inputHandle, records, recordCount, &eventReadCount)
			&& eventReadCount > 0)
		{
			// �̺�Ʈ�� ������, �̺�Ʈ�� �о ó��.
			if (ReadConsoleInput(inputHandle, records, recordCount, &eventReadCount))
			{
				// ���� �̺�Ʈ ����ŭ ����.
				for (int ix = 0; ix < static_cast<int>(eventReadCount); ++ix)
				{
					INPUT_RECORD& record = records[ix];

					// �̺�Ʈ Ÿ�� ���� ó��.
					switch (record.EventType)
					{
						// Ű �̺�Ʈ�� ���.
					case KEY_EVENT:
					{
						// Ű ���� ó��.
						if (record.Event.KeyEvent.bKeyDown)
						{
							keyStates[record.Event.KeyEvent.wVirtualKeyCode].isKeyDown = true;
						}

						// Ű�� �ȴ����� ��.
						else
						{
							keyStates[record.Event.KeyEvent.wVirtualKeyCode].isKeyDown = false;
						}
					}
					break;

					// ���콺 �̺�Ʈ ó��.
					case MOUSE_EVENT:
					{
						// ���콺 x ��ġ ����.
						mousePosition.x = record.Event.MouseEvent.dwMousePosition.X;

						// x��ǥ ���� (���콺 Ŀ���� ȭ�� �ȿ� �����ǵ���).
						mousePosition.x
							= Util::Clamp<int>(mousePosition.x, 0, Engine::Get().GetWidth() - 1);

						// y��ǥ ����.
						mousePosition.y
							= Util::Clamp<int>(mousePosition.y, 0, Engine::Get().GetHeight() - 1);

						// ���콺 Ŭ�� ���� ���� ����.
						keyStates[VK_LBUTTON].isKeyDown
							= (record.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;

						keyStates[VK_RBUTTON].isKeyDown
							= (record.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0;
					}
					break;
					}
				}
			}
		}

		// Ű ������ �Է� �б�.
		// !!! �ü���� �����ϴ� ����� ����� �� �ۿ� ����.
		//for (int ix = 0; ix < 255; ++ix)
		//{
		//	keyStates[ix].isKeyDown
		//		= (GetAsyncKeyState(ix) & 0x8000) > 0 ? true : false;
		//}
	}
	
	void Input::SavePreviousInputStates()
	{
		// ���� Engine::Run() �� �־���
		// ���� �Է� ���� ���� �Է� ������ ����.
		for (int ix = 0; ix < 255; ++ix)
		{
			keyStates[ix].wasKeyDown
				= keyStates[ix].isKeyDown;
		}
	}

	// ??? ??? ??? ??? ??
	Vector2 Input::ScreenToGrid(int gridOffsetX, int gridOffsetY) const
	{
		int gridX = mousePosition.x - gridOffsetX;
		int gridY = mousePosition.y - gridOffsetY;
		return Vector2(gridX, gridY);
	}

	// ?? ??? ??? ?????? ??
	bool Input::IsGridClicked(int gridX, int gridY, int gridOffsetX, int gridOffsetY) const
	{
		Vector2 gridPos = ScreenToGrid(gridOffsetX, gridOffsetY);
		return gridPos.x == gridX && gridPos.y == gridY;
	}
}
