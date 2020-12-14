#include "stdafx.h"
#include <string>
#include "ConfigWrap.h"
#include "common.h"
#include "../src/Mainfrm.h"
#include "../src/PowerSniffApp.h"

CRegistryConfig::CRegistryConfig()
{
}

CRegistryConfig::~CRegistryConfig()
{
}

CRegistryConfig *CRegistryConfig::getInstance()
{
	static CRegistryConfig object;
	return &object;
}

BOOL CRegistryConfig::load()
{
	// ====================================
	// 加载：工具栏和状态栏：是否显示，样式，位置
	CRegKey reg1;
	CString key1 = CString(L"Software\\") + CString(REGIST_KEY) + CString(L"\\Theme Settings");
	if (ERROR_SUCCESS != reg1.Open(HKEY_CURRENT_USER, key1)) {
		reg1.Create(HKEY_CURRENT_USER, key1);
		if (ERROR_SUCCESS != reg1.Open(HKEY_CURRENT_USER, key1)) {
			return FALSE;
		}
	}
	int nBands;
	reg1.QueryDWORDValue(L"NumBands", (DWORD&)nBands);
	for (int i = 0; i < nBands; ++i) {
		TCHAR szSubKey[32];
		UINT valuen;

		wsprintf(szSubKey, L"Band ID %d", i + 1);
		reg1.QueryDWORDValue(szSubKey, (DWORD&)valuen);
		m_vBandIDs.push_back(valuen);

		wsprintf(szSubKey, L"Band Style %d", i + 1);
		reg1.QueryDWORDValue(szSubKey, (DWORD&)valuen);
		m_vBandStyles.push_back(valuen);

		wsprintf(szSubKey, L"Band Size %d", i + 1);
		reg1.QueryDWORDValue(szSubKey, (DWORD&)valuen);
		m_vBandSizes.push_back(valuen);
	}

	// ====================================
	CRegKey reg2;
	BOOL create2 = FALSE;
	CString key2 = CString(L"Software\\") + CString(REGIST_KEY) + CString(L"\\Program Settings");
	if (ERROR_SUCCESS != reg2.Open(HKEY_CURRENT_USER, key2)) {
		reg2.Create(HKEY_CURRENT_USER, key2);
		if (ERROR_SUCCESS != reg2.Open(HKEY_CURRENT_USER, key2)) {
			return FALSE;
		} else {
			create2 = TRUE;
		}
	}
	if (create2) {
		m_bDebug = FALSE;
		m_bAutoScroll = TRUE;
		memset(m_strSelectedPlugin, 0x0, sizeof(m_strSelectedPlugin));
		m_nFilterProtocol = PROTOCOL_TYPE_ALL;
		m_nFilterIPType = 0;
		m_nFilterIP = 0;
		m_nFilterPortType = 0;
		m_nFilterPort = 0;
		m_nUsedAdapterMask = 0xffffffff;
		m_bValidDataCheck = TRUE;
		m_nWndHeightOrigData = 275;
		m_nWndWidthPluginData = 680;
	} else {
		ULONG len;
		reg2.QueryDWORDValue(L"debug", (DWORD&)m_bDebug);
		reg2.QueryDWORDValue(L"Auto Scroll", (DWORD&)m_bAutoScroll);
		reg2.QueryStringValue(L"Selected Plugin", m_strSelectedPlugin, &len);
		reg2.QueryDWORDValue(L"Filter Protocol", (DWORD&)m_nFilterProtocol);
		reg2.QueryDWORDValue(L"Filter IP Type", (DWORD&)m_nFilterIPType);
		reg2.QueryDWORDValue(L"Filter IP", (DWORD&)m_nFilterIP);
		reg2.QueryDWORDValue(L"Filter Port Type", (DWORD&)m_nFilterPortType);
		reg2.QueryDWORDValue(L"Filter Port", (DWORD&)m_nFilterPort);
		reg2.QueryDWORDValue(L"Filter Network Adapter", (DWORD&)m_nUsedAdapterMask);
		reg2.QueryDWORDValue(L"Valid Data Length Check", (DWORD&)m_bValidDataCheck);
		reg2.QueryDWORDValue(L"Wnd Height Orig Data", (DWORD&)m_nWndHeightOrigData);
		reg2.QueryDWORDValue(L"Wnd Width Plugin Data", (DWORD&)m_nWndWidthPluginData);
	}

	return TRUE;
}

BOOL CRegistryConfig::save()
{
	// ====================================
	// 保存：工具栏和状态栏：是否显示，样式，位置
	CRegKey reg1;
	CString key1 = CString(L"Software\\") + CString(REGIST_KEY) + CString(L"\\Theme Settings");
	if (ERROR_SUCCESS != reg1.Open(HKEY_CURRENT_USER, key1)) {
		reg1.Create(HKEY_CURRENT_USER, key1);
		if (ERROR_SUCCESS != reg1.Open(HKEY_CURRENT_USER, key1)) {
			return FALSE;
		}
	}

	REBARBANDINFO rbbi;
	ZeroMemory(&rbbi, sizeof(REBARBANDINFO));
	rbbi.fMask = RBBIM_ID | RBBIM_STYLE | RBBIM_SIZE;

	CMainFrame *pFrame = GetSplitApp().GetMainFrame();
	int nBands = pFrame->GetReBar().GetBandCount();
	reg1.SetDWORDValue(L"NumBands", (DWORD)nBands);
	for (int i = 0; i < nBands; ++i) {
		TCHAR szSubKey[32];
		pFrame->GetReBar().GetBandInfo(i, rbbi);
		UINT nID = rbbi.wID;
		UINT nStyle = rbbi.fStyle;
		UINT nSize = rbbi.cx;

		wsprintf(szSubKey, L"Band ID %d", i + 1);
		reg1.SetDWORDValue(szSubKey, (DWORD)nID);

		wsprintf(szSubKey, L"Band Style %d", i + 1);
		reg1.SetDWORDValue(szSubKey, (DWORD)nStyle);

		wsprintf(szSubKey, L"Band Size %d", i + 1);
		reg1.SetDWORDValue(szSubKey, (DWORD)nSize);
	}

	// ====================================
	CRegKey reg2;
	CString key2 = CString(L"Software\\") + CString(REGIST_KEY) + CString(L"\\Program Settings");
	if (ERROR_SUCCESS != reg2.Open(HKEY_CURRENT_USER, key2)) {
		reg2.Create(HKEY_CURRENT_USER, key2);
		if (ERROR_SUCCESS != reg2.Open(HKEY_CURRENT_USER, key2)) {
			return FALSE;
		} else {
		}
	}

	reg2.SetDWORDValue(L"debug", (DWORD&)m_bDebug);
	reg2.SetDWORDValue(L"Auto Scroll", (DWORD&)m_bAutoScroll);
	reg2.SetStringValue(L"Selected Plugin", m_strSelectedPlugin);
	reg2.SetDWORDValue(L"Filter Protocol", (DWORD)m_nFilterProtocol);
	reg2.SetDWORDValue(L"Filter IP Type", (DWORD)m_nFilterIPType);
	reg2.SetDWORDValue(L"Filter IP", (DWORD)m_nFilterIP);
	reg2.SetDWORDValue(L"Filter Port Type", (DWORD)m_nFilterPortType);
	reg2.SetDWORDValue(L"Filter Port", (DWORD)m_nFilterPort);
	reg2.SetDWORDValue(L"Filter Network Adapter", (DWORD)m_nUsedAdapterMask);
	reg2.SetDWORDValue(L"Valid Data Length Check", (DWORD)m_bValidDataCheck);

	m_nWndHeightOrigData = pFrame->m_pDockTextBottomRightHexShower->GetDockSize();
	m_nWndWidthPluginData = pFrame->m_pDockTextBottomLeftProtocolData->GetDockSize();
	reg2.SetDWORDValue(L"Wnd Height Orig Data", (DWORD)m_nWndHeightOrigData);
	reg2.SetDWORDValue(L"Wnd Width Plugin Data", (DWORD)m_nWndWidthPluginData);

	return FALSE;
}
