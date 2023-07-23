#pragma once

#include "Settings.h"

class Chat
{
private:
	char m_inputBuffer[BodyLen];
	int m_inputBufferLoc = 0;

public:
	Chat();
	~Chat();

	const char* GetInputBuffer() const { return m_inputBuffer; }

	ePacketType Input(bool& _isTyped);

	void ClearInputBuffer() { m_inputBuffer[0] = 0; }
};

