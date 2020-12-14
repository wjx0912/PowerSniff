//////////////////////////////////////////////
// PowerSniffPanes.h

#ifndef PowerSniffPANES_H
#define PowerSniffPANES_H

#include "Views.h"
#include "Mainfrm.h"

#define DEFAULT_RICHEDIT_TEXT_SIZE		10

class CContainText : public CDockContainer
{
public:
	CContainText(LPCTSTR szTabTitle);
	~CContainText();
	void TextClear();
	void TextAppend(COLORREF color, int textsize, TCHAR *text);
	void TextSetColor(int start, int end, COLORREF color, int textsize, BOOL italic);
	void SetReadOnly(BOOL bReadOnly);
	void SetMainFrm(CMainFrame *p);

private:
	CViewText m_ViewText;
};

class CDockText : public CDocker
{
public:
	CDockText(LPCTSTR szTabTitle);
	virtual ~CDockText();
	void TextClear();
	void TextAppend(COLORREF color, int textsize, TCHAR *text);
	void TextSetColor(int start, int end, COLORREF color, int textsize, BOOL italic = FALSE);
	void SetReadOnly(BOOL bReadOnly);

	void SetHideSingleTab(BOOL HideSingle);
	void SetTabText(LPCTSTR szText);
	void SetMainFrm(CMainFrame *p);

private:
	CContainText m_View;
};

#endif
