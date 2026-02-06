#include "Renderer.h"
#include "ScreenBuffer.h"
#include "Util/Util.h"

namespace FE
{
	Renderer::Frame::Frame(int bufferCount)
	{
		// �迭 ���� �� �ʱ�ȭ.
		charInfoArray = new CHAR_INFO[bufferCount];
		memset(charInfoArray, 0, sizeof(CHAR_INFO) * bufferCount);

		sortingOrderArray = new int[bufferCount];
		memset(sortingOrderArray, 0, sizeof(int) * bufferCount);

		//ZeroMemory(charInfoArray, 0, sizeof(CHAR_INFO) * bufferCount);
	}

	Renderer::Frame::~Frame()
	{
		SafeDeleteArray(charInfoArray);
		SafeDeleteArray(sortingOrderArray);
	}

	void Renderer::Frame::Clear(const Vector2& screenSize)
	{
		// 2���� �迭�� �ٷ�� 1���� �迭�� ��ȸ�ϸ鼭
		// �� ����(' ')�� ����.
		const int width = screenSize.x;
		const int height = screenSize.y;

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				// �迭 �ε��� ���ϱ�.
				const int index = (y * width) + x;

				// ���� �� �� �Ӽ� ����.
				CHAR_INFO& info = charInfoArray[index];
				info.Char.AsciiChar = ' ';
				info.Attributes = 0;

				// �׸��� �켱���� �ʱ�ȭ.
				sortingOrderArray[index] = -1;
			}
		}
	}

	// -------------- Frame -------------- //

	// ���� ���� �ʱ�ȭ.
	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer(const Vector2& screenSize)
		: screenSize(screenSize)
	{
		instance = this;

		// ������ ��ü ����.
		const int bufferCount = screenSize.x * screenSize.y;
		frame = new Frame(bufferCount);

		// ������ �ʱ�ȭ.
		frame->Clear(screenSize);

		// ���� ���� ��ü ���� �� �ʱ�ȭ.
		screenBuffers[0] = new ScreenBuffer(screenSize);
		screenBuffers[0]->Clear();

		screenBuffers[1] = new ScreenBuffer(screenSize);
		screenBuffers[1]->Clear();

		// Ȱ��ȭ ���� ����.
		Present();
	}

	Renderer::~Renderer()
	{
		SafeDelete(frame);
		for (ScreenBuffer*& buffer : screenBuffers)
		{
			SafeDelete(buffer);
		}
	}

	void Renderer::Draw()
	{
		// ȭ�� �����.
		Clear();

		// ��������: ������ ��� ���Ͱ� �������� submit �� �Ϸ�
		// ���� ť ��ȸ�ϸ鼭 ������ ä��� (2���� �迭���� ���)
		for (const RenderCommand& command : renderQueue)
		{
			// ȭ�鿡 �׸� �ؽ�Ʈ�� ������ �ǳʶ�.
			if (!command.text)
			{
				continue;
			}

			// ���� ���� ȭ�� ������� Ȯ��.
			if (command.position.y < 0
				|| command.position.y >= screenSize.y)
			{
				continue;
			}

			// ȭ�鿡 �׸� ���ڿ� ����.
			const int length = static_cast<int>(strlen(command.text));

			// �ȱ׷��� �Ǹ� �ǳʶ�.
			if (length <= 0)
			{
				continue;
			}

			// x��ǥ �������� ȭ�鿡�� ������� Ȯ��.
			// ��ġ�� ���� ����: "abcde"
			// length ���� 1 ���� ������ index ��� ����
			const int startX = command.position.x;
			const int endX = command.position.x + length - 1;

			if (endX < 0 || startX >= screenSize.x)
			{
				continue;
			}

			// ���� ��ġ.
			const int visibleStart = startX < 0 ? 0 : startX;
			// ȭ�� ������� üũ�ϰ� ���� ����
			const int visibleEnd
				= endX >= screenSize.x ? screenSize.x - 1 : endX;

			// ���ڿ� ����.
			for (int x = visibleStart; x <= visibleEnd; ++x)
			{
				// ���ڿ� ���� ���� �ε���.
				const int sourceIndex = x - startX;

				// ������ (2���� ���� �迭) �ε���.
				const int index
					= (command.position.y * screenSize.x) + x;

				// �׸��� �켱���� ��.
				if (frame->sortingOrderArray[index]
					> command.sortingOrder)
				{
					continue;
				}

				// ������ ���.
				frame->charInfoArray[index].Char.AsciiChar
					= command.text[sourceIndex];
				frame->charInfoArray[index].Attributes
					= (WORD)command.color;

				// �켱���� ������Ʈ.
				frame->sortingOrderArray[index]
					= command.sortingOrder;
			}
		}

		// �׸���.
		GetCurrentBuffer()->Draw(frame->charInfoArray);

		// ���� ��ȯ.
		Present();

		// ���� ť ����.
		renderQueue.clear();
	}

	Renderer& Renderer::Get()
	{
		if (!instance)
		{
			MessageBoxA(
				nullptr,
				"Renderer::Get() - instance is null",
				"Error",
				MB_OK
			);

			__debugbreak();
		}

		return *instance;
	}

	void Renderer::Clear()
	{
		// ȭ�� �����.
		// 1. ������(2���� �迭 ������) �����.
		frame->Clear(screenSize);

		// 2. �ܼ� ���� �����.
		GetCurrentBuffer()->Clear();
	}

	void Renderer::Submit(
		const char* text,
		const Vector2& position,
		Color color,
		int sortingOrder)
	{
		// ���� ������ ���� �� ť�� �߰�.
		RenderCommand command = {};
		command.text = text;
		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;

		renderQueue.emplace_back(command);
	}

	void Renderer::PresentImmediately()
	{
		Draw();
		GetCurrentBuffer()->Draw(frame->charInfoArray);
		Present();
	}

	void Renderer::Present()
	{
		// ���� ��ȯ.
		SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());

		// �ε��� ��ü.
		currentBufferIndex = 1 - currentBufferIndex;
	}

	ScreenBuffer* Renderer::GetCurrentBuffer()
	{
		return screenBuffers[currentBufferIndex];
	}
}