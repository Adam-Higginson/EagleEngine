//Class to handle input

#pragma once

#ifndef _INPUT_HANDLER_H_
#define _INPUT_HANDLER_H_

namespace ee
{
	class InputHandler
	{
	public:
		InputHandler();
		InputHandler(const InputHandler&);
		~InputHandler();

		bool Init();

		void keyDown(unsigned int key);
		void keyUp(unsigned int key);

		bool isKeyDown(unsigned int key);
		bool isKeyUp(unsigned int key);

	private:
		bool m_keys[256];
	};
}

#endif