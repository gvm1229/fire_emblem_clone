#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

class Box : public Actor
{
	// RTTI
	RTTI_DECLARATIONS(Box, Actor)

public:
	Box(const Vector2& newPosition);
};