//////////////////////////////////////////////////
// PowerSniffApp.h
//  Declaration of the CApp class

#ifndef PowerSniffAPP_H
#define PowerSniffAPP_H

#include "Mainfrm.h"

class CPowerSniffApp : public CWinApp
{
public:
	CPowerSniffApp(HINSTANCE hInstance);
	virtual ~CPowerSniffApp();
	virtual BOOL InitInstance();
	CMainFrame* GetMainFrame() { return &m_Frame; }

public:
	CMainFrame	m_Frame;
	HINSTANCE	m_hInstance;
};

inline CPowerSniffApp& GetSplitApp()
{
	return static_cast<CPowerSniffApp&>(GetApp());
}

#endif
