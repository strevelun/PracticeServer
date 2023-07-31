#include "Selector.h"
#include "Listener.h"

#include <cstdio>

Selector* Selector::m_inst = nullptr;

Selector::Selector()
{
	FD_ZERO(&m_fdSocketInfos);
	FD_ZERO(&m_fdReads);
	m_hSocketServer = INVALID_SOCKET;

}

Selector::~Selector()
{
}

bool Selector::Init(SOCKET _hSocketServer)
{
	m_hSocketServer = _hSocketServer;
	FD_ZERO(&m_fdSocketInfos);
	FD_SET(m_hSocketServer, &m_fdSocketInfos);

	return true;
}

int Selector::Select()
{
	SOCKET clientSocket;

	m_fdReads = m_fdSocketInfos;
	int	iRet = select(0, &m_fdReads, 0, 0, 0);
	if (iRet == SOCKET_ERROR) return SOCKET_ERROR;

	for (int i = 0; i < m_fdSocketInfos.fd_count; i++)
	{
		clientSocket = m_fdSocketInfos.fd_array[i];
		if (FD_ISSET(clientSocket, &m_fdReads))
		{
			if (clientSocket == m_hSocketServer)
			{
				SOCKET acceptedClientSocket = Listener::GetInst()->AcceptClient();
				printf("%d ¿¬°áµÊ\n", (int)acceptedClientSocket);
				FD_SET(acceptedClientSocket, &m_fdSocketInfos);
			}
			else
			{
				m_clientSocketList.push_back(clientSocket);
			}
		}
	}
}
