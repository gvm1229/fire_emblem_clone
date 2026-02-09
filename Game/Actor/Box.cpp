#include "Box.h"

Box::Box(const Vector2& newPosition)
	: super("B", newPosition, Color::Blue)
{
	// 그리기 우선순위 설정
	sortingOrder = 5;
}