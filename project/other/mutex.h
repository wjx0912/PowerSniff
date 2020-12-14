#pragma once

class CMyMutex
{

public:
	CMyMutex();
	~CMyMutex();

	void Lock();

	void Unlock();

private:
	HANDLE m_mutex;
};
