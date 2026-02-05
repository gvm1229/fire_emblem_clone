#include "Actor.h"
#include "Util/Util.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"

#include <iostream>
#include <Windows.h> // �ܼ��� �����ϴ� �Լ��� OS ���� �ٸ�

namespace FE
{
	Actor::Actor(
		const char* image,
		const Vector2& position,
		Color color)
		: position(position), color(color)
	{
		// ���ڿ� ����.
		width = static_cast<int>(strlen(image));
		this->image = new char[width + 1];
		strcpy_s(this->image, width + 1, image);
	}

	Actor::~Actor()
	{
		// �޸� ����.
		SafeDeleteArray(image);
	}

	void Actor::BeginPlay()
	{
		// �̺�Ʈ�� ���� �Ŀ��� �÷��� ����.
		hasBeganPlay = true;
	}

	void Actor::Tick(float deltaTime)
	{
	}

	void Actor::Draw()
	{
		//Renderer::Draw(position, color, image);

		// �������� ������ ����.
		Renderer::Get().Submit(image, position, color, sortingOrder);
	}

	void Actor::Destroy()
	{
		// ���� �÷��� ����.
		destroyRequested = true;

		// ���� �̺�Ʈ ȣ��.
		OnDestroy();
	}

	void Actor::OnDestroy()
	{
	}

	void Actor::QuitGame()
	{
		// ���� ���� ��û.
		Engine::Get().QuitEngine();
	}

	bool Actor::TestIntersect(const Actor* const other)
	{
		// AABB (Axis Aligned Bounding Box).
		// x ��ǥ�� ����ϸ��. y�� ũ�Ⱑ 1�̱� ����.

		// �ڱ��ڽ��� x��ǥ ����.
		int xMin = position.x;
		int xMax = position.x + width - 1;

		// �浹�� ���� �ٸ� ������ x��ǥ ����.
		int otherXMin = other->GetPosition().x;
		int otherXMax
			= other->position.x + other->width - 1;

		// �Ȱ�ġ�� ���� Ȯ��.

		// �ٸ� ������ ���� ��ǥ��
		// �� ������ ��ǥ���� �� �����ʿ� �ִ� ���.
		// ��, �� ���� ������ ���ε� �׺��� �� ������ �ٱ��� �ִ� ���
		if (otherXMin > xMax)
		{
			return false;
		}

		// �ٸ� ������ ������ ��ǥ��
		// �� ���� ��ǥ���� �� ���ʿ� �ִ� ���.
		// ��, �� ���� ���� ���ε� �׺��� �� ���� �ٱ��� �ִ� ���
		if (otherXMax < xMin)
		{
			return false;
		}

		// y�� ũ�Ⱑ 1�̱� ������ ��ǥ�� ������ ���θ� Ȯ��.
		return position.y == other->position.y;
	}

	void Actor::ChangeImage(const char* newImage)
	{
		// ���� �޸� ����.
		SafeDeleteArray(image);

		// ���ο� ���ڿ� ����.
		width = static_cast<int>(strlen(newImage));
		image = new char[width + 1];
		strcpy_s(image, width + 1, newImage);
	}

	void Actor::SetPosition(const Vector2& newPosition)
	{
		// �������� ��ĭ �׸��� ��û.
		//Renderer::Draw(position, ' ');

		// �����Ϸ��� ��ġ�� ���� ��ġ�� ������ �ǳʲ�.
		if (position == newPosition)
		{
			return;
		}

		// ���ο� ��ġ ����.
		position = newPosition;
	}
}