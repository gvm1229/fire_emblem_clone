#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

class Wall : public Actor
{
	RTTI_DECLARATIONS(Wall, Actor)

public:
	// 위치 지정이 가능하도록 생성자 선언.
	Wall(const Vector2& position);
};
