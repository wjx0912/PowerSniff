//////////////////////////////////////////////////////////////
// Views.cpp -  Definitions for the CViewSimple, CViewList,
//              CViewTree and CViewText classes

#include "stdafx.h"
#include "PowerSniffApp.h"
#include "Views.h"
#include "resource.h"
#include "../other/ConfigWrap.h"


///////////////////////////////////////////////
// CViewText functions


CViewText::CViewText()
{
	m_mainframe = NULL;
}

CViewText::~CViewText()
{
}

void CViewText::PreCreate(CREATESTRUCT &cs)
{
	cs.style = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_CHILD |
		WS_TABSTOP | WS_HSCROLL | WS_VISIBLE | WS_VSCROLL;

	cs.dwExStyle = WS_EX_CLIENTEDGE;
	cs.lpszClass = RICHEDIT_CLASS; // RichEdit ver 2.0
}

void CViewText::SetFontDefaults()
{
	//Set font
	if (m_Font.GetHandle() == 0)
		m_Font.CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN, L"Courier New");
	SetFont(m_Font, FALSE);

	// Required for Dev-C++
#ifndef IMF_AUTOFONT
#define IMF_AUTOFONT			0x0002
#endif

	// Prevent Unicode characters from changing the font
	LRESULT lres = SendMessage(EM_GETLANGOPTIONS, 0, 0);
	lres &= ~IMF_AUTOFONT;
	SendMessage(EM_SETLANGOPTIONS, 0, lres);
}

void CViewText::OnInitialUpdate(void)
{
	//increase the text limit of the rich edit window
	LimitText(-1);

	//Determine which messages will be passed to the parent
	//DWORD dwMask = ENM_KEYEVENTS;
	//SetEventMask(dwMask);

	SetFontDefaults();
	//SetReadOnly(TRUE);
	SetTextMode(TM_RICHTEXT | TM_MULTILEVELUNDO | TM_SINGLECODEPAGE);
}

void CViewText::TextClear()
{
	//TRACE(L"TextClear\n"));
	SetWindowText(L"");
	//Clear();
	//this->SetSel(0, GetWindowTextLength());
	//this->ReplaceSel(L""));
}

void CViewText::TextAppend(COLORREF color, int textsize, TCHAR *text)
{
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR | CFM_SIZE;
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	cf.crTextColor = color;
	cf.yHeight = 20 * textsize;
	SendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

	SetSel(-1, -1);
	ReplaceSel(text);
}

void CViewText::TextSetColor(int start, int end, COLORREF color, int textsize, BOOL italic)
{
	this->SetSel(start, end);

	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	if (italic) {
		cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_ITALIC;
		cf.dwEffects |= CFE_ITALIC;
	} else {
		cf.dwMask = CFM_COLOR | CFM_SIZE;
		cf.dwEffects &= ~CFE_ITALIC;
	}
	//cf.dwEffects |= CFE_ITALIC;
	//cf.dwEffects |= CFE_BOLD;
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	cf.crTextColor = color;
	cf.yHeight = 20 * textsize;
	SendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

#define RICHEDIT_POS_CHANGE_MSG		(WM_USER + 1000)
LRESULT CViewText::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_KEYDOWN:
		{
			if (m_mainframe)
			{
				PostMessage(RICHEDIT_POS_CHANGE_MSG);
			}

			break;
		}
		case WM_LBUTTONDOWN:
		{
			if (m_mainframe)
			{
				PostMessage(RICHEDIT_POS_CHANGE_MSG);
			}

			break;
		}
		case RICHEDIT_POS_CHANGE_MSG:
		{
			long start, end;
			GetSel(start, end);
			m_mainframe->OnChangeCursor(start, end);
			break;
		}
	}

	return WndProcDefault(uMsg, wParam, lParam);
}

void CViewText::SetMainFrm(CMainFrame *p)
{
	m_mainframe = p;
}

///////////////////////////////////////////////
// CViewList functions
CViewList::CViewList()
{
	m_mainframe = NULL;
	m_nSelItem = 0;
}

CViewList::~CViewList()
{
	if (IsWindow())
		DeleteAllItems();
}

void CViewList::PreCreate(CREATESTRUCT &cs)
{
	cs.style &= ~LVS_TYPEMASK;
	cs.style |= LVS_REPORT | LVS_SINGLESEL | LVS_OWNERDATA;
	//cs.style |= LVS_SHOWSELALWAYS | LVS_OWNERDRAWFIXED;
}

void CViewList::OnInitialUpdate()
{
	// Set the report style
	DWORD dwStyleEx = GetExtendedStyle();
	dwStyleEx |= LVS_EX_DOUBLEBUFFER;
	//dwStyleEx |= LVS_EX_GRIDLINES;
	//dwStyleEx |= LVS_EX_CHECKBOXES;
	dwStyleEx |= LVS_EX_FULLROWSELECT;
	SetExtendedStyle(dwStyleEx);

	__SetColumns();
}

void CViewList::__SetColumns()
{
	//empty the list
	DeleteAllItems();

	TCHAR szString[10][32] = { L"Index", L"Protocol", L"Source Address", L"Dest Address"
		, L"Source Port", L"Dest Port", L"Total Size", L"Data Size"
		, L"Capture Time", L"Plugin Summary" };
	//int   columnWidth[10] = { 50, 65, 110, 110, 80, 90, 70, 70, 160, 380 };
	int   columnWidth[10] = { 60, 70, 120, 120, 90, 100, 80, 80, 160, 220 };

	//initialise the columns
	LV_COLUMN lvColumn;
	ZeroMemory(&lvColumn, sizeof(LV_COLUMN));
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for (int i = 0; i < 10; ++i) {
		lvColumn.cx = columnWidth[i];
		lvColumn.pszText = szString[i];
		InsertColumn(i, lvColumn);
	}
}

BOOL CViewList::SetSubItem(int nItem, int nSubItem, LPCTSTR szText)
{
	LVITEM lvi1;
	ZeroMemory(&lvi1, sizeof(LVITEM));
	lvi1.mask = LVIF_TEXT;
	lvi1.iItem = nItem;
	lvi1.iSubItem = nSubItem;
	lvi1.pszText = (LPTSTR)szText;
	return (BOOL)SendMessage(LVM_SETITEM, 0, (LPARAM)&lvi1);
}

int CViewList::AddLine(const struct PcapDataDescriptor &data)
{
	int id = GetItemCount();
	TCHAR _id[32] = L"";
	swprintf(_id, L"%d", id + 1);

	// 加入新的一列
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.pszText = _id;
	lvi.iItem = GetItemCount();
	InsertItem(lvi);

	// 该列其它显示
	SetSubItem(id, 1, data.protocol);
	SetSubItem(id, 2, data.src_addr_str);
	SetSubItem(id, 3, data.dst_addr_str);

	TCHAR src_port[32];
	TCHAR dst_port[32];
	TCHAR total_size[32];
	TCHAR data_size[32];
	swprintf(src_port, L"%d", data.src_port);
	swprintf(dst_port, L"%d", data.dst_port);
	swprintf(total_size, L"%d", data.total_size);
	swprintf(data_size, L"%d", data.data_size);
	SetSubItem(id, 4, src_port);
	SetSubItem(id, 5, dst_port);
	SetSubItem(id, 6, total_size);
	SetSubItem(id, 7, data_size);

	SetSubItem(id, 8, data.capture_time_str);
	SetSubItem(id, 9, data.plugin_summary);
	return 0;
}

void CViewList::ScrollToBottom()
{
	CRegistryConfig *pconfig = CRegistryConfig::getInstance();
	if (pconfig->m_bAutoScroll) {
		EnsureVisible(GetItemCount() - 1, FALSE);
	}
}

void CViewList::SetMainFrm(CMainFrame *p)
{
	m_mainframe = p;
}

// 参考：https://sourceforge.net/p/win32-framework/discussion/600648/thread/3d61a6ef/
LRESULT CViewList::OnNotifyReflect(WPARAM wParam, LPARAM lParam)
{
	LPNMHDR  pNMHDR = (LPNMHDR)lParam;
	if (!m_mainframe)
		return -1;

	switch (pNMHDR->code) {
#if 1
	case NM_KILLFOCUS:
		if (GetSelectedCount() > 0) {
			m_nSelItem = GetSelectionMark();
			SetItemState(m_nSelItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
		}
		break;
	case NM_SETFOCUS:
		if (GetSelectedCount() > 0) {
			SetItemState(m_nSelItem, FALSE, LVIF_STATE);
		}
		break;
#endif
	case LVN_GETDISPINFO:
		m_mainframe->__OnGetdispinfoList(pNMHDR);
		break;
	case LVN_ITEMCHANGED:
	case NM_CLICK:
	case NM_DBLCLK:
		// 必须用SendMessage，否则pNMHDR会被释放，然后得不到正确的信息
		m_mainframe->SendMessage(WM_NOTIFY, wParam, lParam);
		break;
	}

	return 0;
}
