#pragma once

#include "Math/Vector2.h"
#include "Math/Color.h"
#include <Windows.h>
#include <vector>

namespace FE
{
	// �ܼ� ���۸� �����ϴ� Ŭ����.
	class ScreenBuffer;

	// ���� ���۸��� �����ϴ� ������ Ŭ����.
	class FE_API Renderer
	{
		// ������ ����ü - 2���� ���� �迭�� �׸��� �� ����ü.
		struct Frame
		{
			Frame(int bufferCount);
			~Frame();

			// ����� �Լ�.
			void Clear(const Vector2& screenSize);

			// ���� ���� ������ ������ ���� Ÿ��.
			CHAR_INFO* charInfoArray = nullptr;

			// �׸��� �켱���� �迭.
			int* sortingOrderArray = nullptr;
		};

		// �������� ������.
		struct RenderCommand
		{
			// ȭ�鿡 ������ ���ڿ� ��.
			const char* text = nullptr;

			// ��ǥ.
			Vector2 position;

			// ����.
			Color color = Color::White;

			// �׸��� �켱����.
			int sortingOrder = 0;
		};

	public:
		Renderer(const Vector2& screenSize);
		~Renderer();

		// �׸��� �Լ�.
		void Draw();

		// �׸��µ� �ʿ��� �����͸� ����(����)�ϴ� �Լ�.
		void Submit(
			const char* text,
			const Vector2& position,
			Color color = Color::White,
			int sortingOrder = 0
		);

		// ��� ȭ�鿡 ǥ���� �� ���.
		void PresentImmediately();

		// �̱��� ���� �Լ�.
		static Renderer& Get();

	private:

		// ȭ�� ����� �Լ�.
		void Clear();

		// ���� ���۸��� Ȱ���� Ȱ��ȭ ���۸� ��ȯ�ϴ� �Լ�.
		void Present();

		// ���� ����� ���۸� ��ȯ�ϴ� �Լ�(Getter).
		ScreenBuffer* GetCurrentBuffer();

	private:
		// ȭ�� ũ��.
		Vector2 screenSize;

		// ������ ������ ��ü.
		Frame* frame = nullptr;

		// ���� ���� �迭.
		ScreenBuffer* screenBuffers[2] = {};

		// ���� Ȱ��ȭ�� ���� �ε���.
		int currentBufferIndex = 0;

		// ���� ť (���� ��� �׸��� ����� ��Ƶδ� �迭).
		std::vector<RenderCommand> renderQueue;

		// �̱��� ������ ���� ���� ����.
		static Renderer* instance;
	};
}