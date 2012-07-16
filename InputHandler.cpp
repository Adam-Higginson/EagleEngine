#include "InputHandler.h"

ee::InputHandler::InputHandler()
{
}

ee::InputHandler::InputHandler(const InputHandler &other)
{
}

ee::InputHandler::~InputHandler()
{
}

bool ee::InputHandler::Init()
{
	for (int i = 0; i < 256; i++)
		m_keys[i] = false;

	return true;
}

void ee::InputHandler::keyDown(unsigned int key)
{
	m_keys[key] = true;
}

void ee::InputHandler::keyUp(unsigned int key)
{
	m_keys[key] = false;
}

bool ee::InputHandler::isKeyDown(unsigned int key)
{
	return m_keys[key];
}

bool ee::InputHandler::isKeyUp(unsigned int key)
{
	return !m_keys[key];
}