#pragma once

const int UserNameLen = 32;
const int BodyLen = 256;

enum class ePacketType
{
	None,
	Chat,
	Exit
};