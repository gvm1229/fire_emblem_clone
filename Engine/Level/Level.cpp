#include "Level.h"
#include "Actor/Actor.h"

namespace FE
{
	Level::Level()
	{
	}

	Level::~Level()
	{
		// �޸� ����.
		for (Actor*& actor : actors)
		{
			// ���� ��ü �޸� ����.
			if (actor)
			{
				delete actor;
				actor = nullptr;
			}
		}

		// �迭 �ʱ�ȭ.
		actors.clear();
	}

	void Level::BeginPlay()
	{
		// ���Ϳ� �̺�Ʈ �긮��.
		for (Actor* actor : actors)
		{
			// �̹� BeginPlay ȣ��� ���ʹ� �ǳʶٱ�.
			if (actor->HasBeganPlay())
			{
				continue;
			}

			actor->BeginPlay();
		}
	}

	void Level::Tick(float deltaTime)
	{
		// ���Ϳ� �̺�Ʈ �긮��.
		for (Actor* actor : actors)
		{
			actor->Tick(deltaTime);
		}
	}

	void Level::Draw()
	{
		//// ���Ϳ� �̺�Ʈ �긮��.
		//for (Actor* actor : actors)
		//{
		//	// ���� ��ġ�� �ٸ� ���Ͱ� �ִ��� Ȯ���ϱ� ����.
		//	Actor* search = nullptr;
		//	for (Actor* otherActor : actors)
		//	{
		//		// ���� ���ʹ� �� ����.
		//		if (actor == otherActor)
		//		{
		//			continue;
		//		}

		//		// ��ġ ��.
		//		if (actor->GetPosition() == otherActor->GetPosition())
		//		{
		//			// ���� ���� ��.
		//			if (actor->GetSortingOrder()
		//				< otherActor->GetSortingOrder())
		//			{
		//				search = otherActor;
		//				break;
		//			}
		//		}
		//	}

		//	// ���� ��ġ�� �켱���� ���� �ٸ� ���Ͱ� ������ �ȱ׸�.
		//	if (search)
		//	{
		//		continue;
		//	}

		//	// �׸���.
		//	actor->Draw();
		//}

		// ���� ��ȸ�ϸ鼭 Draw �Լ� ȣ��.
		for (Actor* const actor : actors)
		{
			if (!actor->IsActive())
			{
				continue;
			}

			actor->Draw();
		}
	}

	void Level::AddNewActor(Actor* newActor)
	{
		// ���߿� �߰��� ���� �ӽ� �迭�� ����.
		addRequestedActors.emplace_back(newActor);

		// ���ʽ� ����.
		newActor->SetOwner(this);
	}

	void Level::ProcessAddAndDestroyActors()
	{
		// ���� ó��.
		for (int ix = 0; ix < static_cast<int>(actors.size()); )
		{
			// ���� ��û�� ���Ͱ� �ִ��� Ȯ��.
			if (actors[ix]->DestroyRequested())
			{
				// ���� ó��.
				delete actors[ix];
				actors.erase(actors.begin() + ix);
				continue;
			}

			++ix;
		}

		// �߰� ó��.
		if (addRequestedActors.size() == 0)
		{
			return;
		}

		for (Actor* const actor : addRequestedActors)
		{
			actors.emplace_back(actor);
		}

		// ó���� �������� �迭 �ʱ�ȭ.
		addRequestedActors.clear();
	}
}