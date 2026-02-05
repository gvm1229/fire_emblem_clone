#include "Vector2.h"
#include <iostream>

namespace Wanted
{
	Vector2 Vector2::Zero(0, 0);
	Vector2 Vector2::One(1, 1);
	Vector2 Vector2::Up(0, 1);
	Vector2 Vector2::Right(1, 0);

	Vector2::Vector2()
	{
	}

	Vector2::Vector2(int x, int y)
		: x(x), y(y) // new 로 할당하는 게 아니라면 이걸 사용해야 함, 기본 할당을 무시하게 됨
	{
		//this->x = x; // 같은 걸 사용하면, 변수 기본 할당 (0) 이후에 한번 더 할당이 이루어짐
	}
	
	Vector2::~Vector2()
	{
		if (string)
		{
			delete[] string;
			string = nullptr;
		}
	}
	
	const char* Vector2::ToString()
	{
		// 기존 문자열이 있다면 제거.
		if (string)
		{
			delete[] string;
			string = nullptr;
		}

		string = new char[128]; // 내부 임시 변수
		memset(string, 0, sizeof(char) * 128);
		sprintf_s(string, 128, "(%d, %d)", x, y);

		return string;
	}

	Vector2 Vector2::operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 Vector2::operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}

	bool Vector2::operator==(const Vector2& other) const
	{
		return (x == other.x) && (y == other.y);
	}

	bool Vector2::operator!=(const Vector2& other) const
	{
		//return (x != other.x) || (y != other.y);
		return !(*this == other);
	}

	Vector2::operator COORD() const
	{
		COORD coord = {};
		coord.X = static_cast<short>(x);
		coord.Y = static_cast<short>(y);

		return coord;
	}
}