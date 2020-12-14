#include "stdafx.h"
#include "mutex.h"

CMyMutex::CMyMutex()
{
	m_mutex = ::CreateMutex(NULL, FALSE, NULL);
}

CMyMutex::~CMyMutex()
{
	::CloseHandle(m_mutex);
}

void CMyMutex::Lock()
{
	WaitForSingleObject(m_mutex, INFINITE);
}

void CMyMutex::Unlock()
{
	::ReleaseMutex(m_mutex);
}
