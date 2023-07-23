#pragma once


const int BodyLen = 256;
const int UserNameLen = 32;

enum class ePacketType
{
	None,
	Chat,
	Exit
};