#include "InputHandler.h"

namespace ee
{
	ee::InputHandler::InputHandler()
	{
	}

	ee::InputHandler::InputHandler(const InputHandler &other)
	{
	}

	ee::InputHandler::~InputHandler()
	{
	}

	bool InputHandler::Init()
	{
		for (int i = 0; i < 256; i++)
			m_keys[i] = false;

		for (int i = 0; i < 3; i++)
			m_mouseButtons[i] = false;

		m_mouseX = 0;
		m_mouseY = 0;

		return true;
	}

	void InputHandler::KeyDown(unsigned int key)
	{
		m_keys[key] = true;
	}

	void InputHandler::KeyUp(unsigned int key)
	{
		m_keys[key] = false;
	}

	bool InputHandler::IsKeyDown(unsigned int key) const
	{
		return m_keys[key];
	}

	bool InputHandler::IsKeyUp(unsigned int key) const
	{
		return !m_keys[key];
	}

	void InputHandler::MouseDown(MouseButton button, int x, int y)
	{
		m_mouseButtons[button] = true;
		m_mouseX = x;
		m_mouseY = y;
	}

	void InputHandler::MouseUp(MouseButton button, int x, int y)
	{
		m_mouseButtons[button] = false;
		m_mouseX = x;
		m_mouseY = y;
	}

	void InputHandler::MouseMove(int x, int y)
	{
		m_mouseX = x;
		m_mouseY = y;
	}

	bool InputHandler::IsMouseButtonDown(MouseButton button) const
	{
		return m_mouseButtons[button];
	}

	bool InputHandler::IsMouseButtonUp(MouseButton button) const
	{
		return !m_mouseButtons[button];
	}

	int InputHandler::MouseX() const
	{
		return m_mouseX;
	}

	int InputHandler::MouseY() const
	{
		return m_mouseY;
	}

	/*XMFLOAT2 InputHandler::GetMousePoint() const
	{
		XMFLOAT2 point;
		point.x = (float)m_mouseX;
		point.y = (float)m_mouseY;
	}*/
}