#pragma once

#include "Common/Common.h"
#include <Windows.h>

namespace FEClone
{
	// 콘솔에 텍스트 색상 등을 지정할 때 사용할
	// 색상 열거형.
	enum class FECLONE_API Color : unsigned short
	{
		Black = 0,
		Blue = FOREGROUND_BLUE,
		Green = FOREGROUND_GREEN,
		Red = FOREGROUND_RED,
		Purple = Red | Blue,
		Yellow = Red | Green,
		Cyan = Blue | Green,
		White = Blue | Green | Red,
		bgGray = BACKGROUND_INTENSITY,
		bgGray_BlueText = Blue | BACKGROUND_INTENSITY,
		bgGray_GreenText = Green | BACKGROUND_INTENSITY,
		bgGray_PurpleText = Purple | BACKGROUND_INTENSITY,
		bgGray_YellowText = Yellow | BACKGROUND_INTENSITY,
		bgGray_CyanText = Cyan | BACKGROUND_INTENSITY,
		bgGray_WhiteText = White | BACKGROUND_INTENSITY,
	};
}