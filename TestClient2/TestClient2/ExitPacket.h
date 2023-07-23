#pragma once
#include "Packet.h"
class ExitPacket :
    public Packet
{
public:
    ExitPacket();
    ~ExitPacket();

    char* MakePacket() override;
};

