//////////////////////////////////////////////
// PowerSniffPanes.cpp

#include "stdafx.h"
#include "PowerSniffApp.h"
#include "Mainfrm.h"
#include "PowerSniffPanes.h"

// =======================================================================================
CContainText::CContainText(LPCTSTR szTabTitle)
{
	SetDockCaption(_T("Text View - Docking container"));
	//SetTabText(_T("Text111"));
	SetTabText(szTabTitle);
	//SetTabIcon(IDI_TEXT);
	SetView(m_ViewText);
	//SetHideSingleTab(TRUE);
}

CContainText::~CContainText()
{
}

void CContainText::TextClear()
{
	m_ViewText.TextClear();
}

void CContainText::TextAppend(COLORREF color, int textsize, TCHAR *text)
{
	m_ViewText.TextAppend(color, textsize, text);
}

void CContainText::TextSetColor(int start, int end, COLORREF color, int textsize, BOOL italic)
{
	m_ViewText.TextSetColor(start, end, color, textsize, italic);
}

void CContainText::SetReadOnly(BOOL bReadOnly)
{
	m_ViewText.SetReadOnly(bReadOnly);
}

void CContainText::SetMainFrm(CMainFrame *p)
{
	m_ViewText.SetMainFrm(p);
}

// =======================================================================================
CDockText::CDockText(LPCTSTR szTabTitle) : m_View(szTabTitle)
{
	SetView(m_View);	// Set the view window to our edit control
	SetBarWidth(4);		// Set the width of the splitter bar
}

CDockText::~CDockText()
{
}

void CDockText::TextClear()
{
	m_View.TextClear();
}

void CDockText::TextAppend(COLORREF color, int textsize, TCHAR *text)
{
	m_View.TextAppend(color, textsize, text);
}

void CDockText::TextSetColor(int start, int end, COLORREF color, int textsize, BOOL italic)
{
	m_View.TextSetColor(start, end, color, textsize, italic);
}

void CDockText::SetReadOnly(BOOL bReadOnly)
{
	m_View.SetReadOnly(bReadOnly);
}

void CDockText::SetHideSingleTab(BOOL HideSingle)
{
	m_View.SetHideSingleTab(HideSingle);
}

void CDockText::SetTabText(LPCTSTR szText)
{
	m_View.SetTabText(1, szText);
}

void CDockText::SetMainFrm(CMainFrame *p)
{
	m_View.SetMainFrm(p);
}
