#pragma once

#include <winsock2.h>
#include <list>

class Selector
{
private:
	static Selector* m_inst;

	fd_set				m_fdSocketInfos, m_fdReads;
	SOCKET				m_hSocketServer;

	std::list<SOCKET> m_clientSocketList;

	Selector();
	~Selector();

public:
	static Selector* GetInst()
{
	if (!m_inst)
		m_inst = new Selector;
	return m_inst;
}

	  static void DestroyInst()
	  {
		  if (m_inst)
			  delete m_inst;
		  m_inst = nullptr;
	  }

	bool Init(SOCKET _hSocketServer);

	int Select();
	fd_set& GetSocketInfos() { return m_fdSocketInfos; }
	
	std::list<SOCKET>& GetClientSocketList() { return m_clientSocketList; }
};

