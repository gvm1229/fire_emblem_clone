#pragma once

#include "Common/Common.h"
#include <Windows.h>

namespace FE
{
	// �ֿܼ� �ؽ�Ʈ ���� ���� ������ �� �����
	// ���� ������.
	enum class FE_API Color : unsigned short
	{
		Black = 0,
		Blue = FOREGROUND_BLUE,
		Green = FOREGROUND_GREEN,
		Red = FOREGROUND_RED,
		White = Blue | Green | Red,
	};
}