#include "Chat.h"
#include <iostream>
#include <conio.h>

#include "Settings.h"

Chat::Chat()
{
}

Chat::~Chat()
{
}

ePacketType Chat::Input(bool& _isTyped)
{
	if (_kbhit())
	{
		char ch = _getch();
		_isTyped = true;

		if (ch == '\r' || ch == '\n') {
			if (m_inputBufferLoc <= 0) return ePacketType::None;
			m_inputBuffer[m_inputBufferLoc] = 0;
			m_inputBufferLoc = 0;
			return ePacketType::Chat;
		}
		else if (ch == '\b')
		{
			if (m_inputBufferLoc <= 0) return ePacketType::None;
			m_inputBuffer[--m_inputBufferLoc] = 0;
		}
		else if (ch == 27)
		{
			return ePacketType::Exit;
		}
		else
		{
			m_inputBuffer[m_inputBufferLoc++] = ch;
			m_inputBuffer[m_inputBufferLoc] = 0;
		}
	}
	return ePacketType::None;
}
