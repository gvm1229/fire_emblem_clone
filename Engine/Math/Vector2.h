#pragma once

#include "Common/Common.h"
#include <Windows.h>

namespace FE
{
	class FE_API Vector2
	{
	public:
		Vector2();
		Vector2(int x, int y);
		~Vector2();

		// ���ڿ��� ��ȯ�ؼ� ��ȯ�ϴ� �Լ�.
		const char* ToString();

		// ������ �����ε�.
		Vector2 operator+(const Vector2& other) const;
		Vector2 operator-(const Vector2& other) const;

		bool operator==(const Vector2& other) const;
		bool operator!=(const Vector2& other) const;

		// ����ȯ ������ �����ε�.
		operator COORD() const;

		// ���� �⺻ ��.
		static Vector2 Zero; // (0,0) �� �ʿ��ϸ� �̰� static ���� �������� ��
		static Vector2 One;
		static Vector2 Up;
		static Vector2 Right;

	public:
		// x/y ��ǥ.
		int x = 0;
		int y = 0;

	private:
		// ���� ���� ���ڿ��� ��ȯ�� �� ����� ����.
		char* string = nullptr;
	};
}
