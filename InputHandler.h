//Class to handle input

#pragma once

#ifndef _INPUT_HANDLER_H_
#define _INPUT_HANDLER_H_

//#include <xnamath.h>

namespace ee
{
	enum MouseButton {MOUSE_LEFT = 0, MOUSE_MIDDLE = 1, MOUSE_RIGHT = 2};

	class InputHandler
	{
	public:
		InputHandler();
		InputHandler(const InputHandler&);
		~InputHandler();

		bool Init();

		bool IsKeyDown(unsigned int key) const;
		bool IsKeyUp(unsigned int key) const;

		bool IsMouseButtonDown(MouseButton button) const;
		bool IsMouseButtonUp(MouseButton button) const;

		int MouseX() const;
		int MouseY() const;

		//XMFLOAT2 GetMousePoint() const;



	private:
		friend class Window; //Allows the Window class only to update key states with the message function
		bool m_keys[256];
		bool m_mouseButtons[3];
		int m_mouseX, m_mouseY; //Last known mouse position

		void KeyDown(unsigned int key);
		void KeyUp(unsigned int key);

		void MouseDown(MouseButton button, int x, int y);
		void MouseUp(MouseButton button, int x, int y);
		void MouseMove(int x, int y);

	};
}

#endif