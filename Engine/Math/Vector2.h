#pragma once

#include "Common/Common.h"
#include <Windows.h>
#include <functional>

namespace FEClone
{
	class FECLONE_API Vector2
	{
	public:
		Vector2();
		Vector2(int x, int y);
		~Vector2();

		// 문자열로 변환해서 반환하는 함수.
		const char* ToString();

		// 연산자 오버로딩.
		Vector2 operator+(const Vector2& other) const;
		Vector2 operator-(const Vector2& other) const;

		bool operator==(const Vector2& other) const;
		bool operator!=(const Vector2& other) const;

		// 형변환 연산자 오버로딩.
		operator COORD() const;

		// 벡터 기본 값.
		static Vector2 Zero; // (0,0) 이 필요하면 이걸 static 으로 가져오면 됨
		static Vector2 One;
		static Vector2 Up;
		static Vector2 Right;

	public:
		// x/y 좌표.
		int x = 0;
		int y = 0;

	private:
		// 벡터 값을 문자열로 변환할 때 사용할 변수.
		char* string = nullptr;
	};
}

// std::unordered_map, std::unordered_set을 위한 hash 함수 특수화
namespace std
{
	template<>
	struct hash<FEClone::Vector2>
	{
		size_t operator()(const FEClone::Vector2& v) const noexcept
		{
			// x와 y를 결합하여 hash 생성
			return hash<int>()(v.x) ^ (hash<int>()(v.y) << 1);
		}
	};
}
