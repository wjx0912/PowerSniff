#include "stdafx.h"
#include "MyStatusBar.h"


void CMyStatusBar::OnAttach() 
{
	// Create the StatusBar parts
	int iPaneWidths[] = { 10, 20 };
	CreateParts(2, iPaneWidths);
	SetStatusParts();

	// Create the Hyperlink
	m_Hyperlink.Create(*this);
	m_StaticText.Create(*this);

	//m_StaticText.SetWindowPos(NULL, CRect(3, 3, 100, 20), SWP_SHOWWINDOW);
	// Set a Part as owner drawn. The part's drawing is performed in CMainFrame::OnDrawItem.
	//m_StaticText.SetWindowText(_T("aaaaaaaaaa"));
	//SetPartText(0, _T("aaaaaaaaaa"));
}

void CMyStatusBar::SetCustomText(LPCTSTR text)
{
	m_StaticText.SetWindowText(text);
}

void CMyStatusBar::PreCreate(CREATESTRUCT &cs)
{
	cs.style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_BOTTOM;
	
	// Suppress the gripper unless XPThemes are enabled
	if (IsXPThemed())
		cs.style |= SBARS_SIZEGRIP;
}

void CMyStatusBar::SetStatusParts()
{
	CRect rcClient = GetClientRect();
	int width = MAX(600, rcClient.right);
	SetPartWidth(0, width - 105);
	SetPartWidth(1, 105);
}

LRESULT CMyStatusBar::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_SIZE:
		{
			// Reposition the StatusBar parts
			SetStatusParts();

			CRect rcPart0 = GetPartRect(0);
			m_StaticText.SetWindowPos(NULL, rcPart0, SWP_SHOWWINDOW);
			CRect rcPart1 = GetPartRect(1);
			m_Hyperlink.SetWindowPos(NULL, rcPart1, SWP_SHOWWINDOW);
			break;
		}
	}

	return CStatusBar::WndProc(uMsg, wParam, lParam);
}
