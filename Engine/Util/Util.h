#pragma once

#include "Math/Vector2.h"
#include "Math/Color.h"
#include <time.h>

using namespace FE;

// ����(Helper) ��� ����.
namespace Util
{
	// �ܼ� Ŀ�� ��ġ �̵�(����)�ϴ� �Լ�.
	inline void SetConsolePosition(const Vector2& position)
	{
		SetConsoleCursorPosition(
			GetStdHandle(STD_OUTPUT_HANDLE),
			static_cast<COORD>(position)
		);
	}

	// �ܼ� �ؽ�Ʈ ���� �Լ�.
	inline void SetConsoleTextColor(Color color)
	{
		SetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE),
			static_cast<unsigned short>(color)
		);
	}

	// Ŀ�� ���.
	inline void TurnOffCursor()
	{
		// Ŀ�� ���.
		CONSOLE_CURSOR_INFO info = {};
		GetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);

		info.bVisible = false;
		SetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);
	}

	// Ŀ�� �ѱ�.
	inline void TurnOnCursor()
	{
		// Ŀ�� ���.
		CONSOLE_CURSOR_INFO info = {};
		GetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);

		info.bVisible = true;
		SetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);
	}

	inline void SetRandomSeed()
	{
		// �ð� ���� ���� ���ڰ����� ����.
		srand(static_cast<unsigned int>(time(nullptr)));
	}

	// ���� ���� �Լ�.
	inline int Random(int min, int max)
	{
		int diff = (max - min) + 1;
		return ((diff * rand()) / (RAND_MAX + 1)) + min;
	}

	// �ε��Ҽ��� ���� �Լ�.
	inline float RandomRange(float min, float max)
	{
		// 0~1������ float ��� ���� ��.
		float random
			= static_cast<float>(rand())
			/ static_cast<float>(RAND_MAX);

		float diff = (max - min);
		return (random * diff) + min;
	}

	// � ���� �� �� ���̷� ������ �� ����ϴ� �Լ�.
	template<typename T>
	T Clamp(T value, T min, T max)
	{
		if (value < min)
		{
			value = min;
		}
		else if (value > max)
		{
			value = max;
		}

		return value;
	}
}

// �޸� ���� �Լ�.
template<typename T>
void SafeDelete(T*& t)
{
	if (t)
	{
		delete t;
		t = nullptr;
	}
}

template<typename T>
void SafeDeleteArray(T*& t)
{
	if (t)
	{
		delete[] t;
		t = nullptr;
	}
}