///////////////////////////////////////
// MyDialog.cpp

#include "stdafx.h"
#include "resource.h"
#include "../other/RawSock.h"
#include "../src/PowerSniffApp.h"
#include "AboutDialog.h"
#include "PowerSniffApp.h"
#include "Mainfrm.h"


// Definitions for the CAboutDialog class
CAboutDialog::CAboutDialog() : CDialog(IDD_DIALOG_ABOUT)
{
}

CAboutDialog::~CAboutDialog()
{
}

void CAboutDialog::OnDestroy()
{
}

BOOL CAboutDialog::OnInitDialog()
{
	m_Hyperlink.AttachDlgItem(IDC_HOME_ADDR, *this);
	m_info.AttachDlgItem(IDC_LICENCE_EDIT, *this);
	m_info.SetWindowText(
		L"This free program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; "
		L"without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\r\n\r\n"
		L""
		L"The Program use rawsocket or winpcap only for windows, support lua - 5.3.5 script, tcc - 0.9.27, "
		L"Customize software function and script contact author, mail: 80101277@qq.com, \r\n"
		L"QQ group 466507719.\r\n\r\n"
		L""
		L"Copyright(C) 2019 yongjian of hubei.wuhan"
		);

	HICON icon = LoadIcon(GetSplitApp().m_hInstance, MAKEINTRESOURCE(IDW_MAIN));
	SetIcon(icon, TRUE);			// 设置大图标
	SetIcon(icon, FALSE);			// 设置小图标

	GetDlgItem(IDOK).SetFocus();
	return FALSE;
}

void CAboutDialog::OnOK()
{
	CDialog::OnOK();
}
