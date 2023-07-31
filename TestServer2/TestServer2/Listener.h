#pragma once

#include <WinSock2.h>

// ������ ���ο� Ŭ���̾�Ʈ ���� ��û�� �޾��ִ� ����
class Listener
{
private:
	static Listener* m_inst;


	SOCKADDR_IN		m_servAddr;
	SOCKET			m_hSocketServer;

	Listener();
	~Listener();

public:
	static Listener* GetInst()
	{
		if (!m_inst)
			m_inst = new Listener;
		return m_inst;
	}

	static void DestroyInst()
	{
		if (m_inst)
			delete m_inst;
		m_inst = nullptr;
	}

	bool Init(const char* _serverIP, int _serverPort);
	SOCKET AcceptClient();

	void Disconnect();

	void Listen();

	SOCKET GetSocketServer() const { return m_hSocketServer; }

private:
	bool Connect(const char* _serverIP, int _serverPort, int _backlog = 3);
};

