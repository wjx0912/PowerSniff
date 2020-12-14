#ifndef MYSTATUSBAR_H
#define MYSTATUSBAR_H

#include "../control/Hyperlink.h"
#include "../control/TransparentStatic.h"

class CMyStatusBar : public CStatusBar
{
public:
	CMyStatusBar() {}
	virtual ~CMyStatusBar() {}
	void SetCustomText(LPCTSTR text);

public:

protected:
	virtual void OnAttach();
	virtual void PreCreate(CREATESTRUCT &cs);
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void SetStatusParts();

private:
	CHyperlink m_Hyperlink;
	CTransparentStatic    m_StaticText;
};

#endif  	// MYSTATUSBAR_H
