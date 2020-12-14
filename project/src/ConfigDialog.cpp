///////////////////////////////////////
// MyDialog.cpp

#include "stdafx.h"
#include <bitset>
#include "ConfigDialog.h"
#include "resource.h"
#include "../other/RawSock.h"
#include "../other/ConfigWrap.h"
#include "../src/PowerSniffApp.h"


// Definitions for the CConfigDialog class
CConfigDialog::CConfigDialog() : CDialog(IDD_DIALOG_OPTIONS)
{
	m_nUsedAdapterMask = 0xffffffff;
}

CConfigDialog::~CConfigDialog()
{
}

void CConfigDialog::OnDestroy()
{
	// End the application
	//::PostQuitMessage(0);
}

INT_PTR CConfigDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//	switch (uMsg)
//	{
		//Additional messages to be handled go here
//	}

	// Pass unhandled messages on to parent DialogProc
	return DialogProcDefault(uMsg, wParam, lParam);
}

BOOL CConfigDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (LOWORD(wParam))
    {
	case IDC_BUTTON_OK1:
		OnOK();
		return TRUE;
	case IDC_BUTTON_CANCEL1:
		OnCancel();
		return TRUE;

	case IDC_RADIO_IP_ALL:
		m_ipaddr.EnableWindow(FALSE);
		return TRUE;
	case IDC_RADIO_IP_SOURCE:
	case IDC_RADIO_IP_DEST:
	case IDC_RADIO_IP_BOTH:
		m_ipaddr.EnableWindow(TRUE);
		return TRUE;

	case IDC_RADIO_PORT_ALL:
		m_filter_port.EnableWindow(FALSE);
		return TRUE;
	case IDC_RADIO_PORT_SOURCE:
	case IDC_RADIO_PORT_DEST:
	case IDC_RADIO_PORT_BOTH:
		m_filter_port.EnableWindow(TRUE);
		return TRUE;
    }

	return FALSE;
}

BOOL CConfigDialog::OnInitDialog()
{
	CRegistryConfig *pconfig = CRegistryConfig::getInstance();

	// Remove taskbar item
	SetWindowLong(GWL_EXSTYLE, GetWindowLong(GWL_EXSTYLE) & ~WS_EX_APPWINDOW);
	m_radio_driver_rawsocket.AttachDlgItem(IDC_RADIO_CAPTURE_METHOD_RAWSOCKET, *this);
	m_radio_driver_winpcap.AttachDlgItem(IDC_RADIO_CAPTURE_METHOD_WINPCAP, *this);
	m_radio_driver_rawsocket.SetCheck(TRUE);
	m_radio_driver_winpcap.SetCheck(FALSE);
	m_radio_driver_winpcap.EnableWindow(FALSE);

	// Attach CWnd objects to the dialog items
	m_radio_all.AttachDlgItem(IDC_RADIO_PROTOCOL_ALL, *this);
	m_radio_tcp.AttachDlgItem(IDC_RADIO_PROTOCOL_TCP, *this);
	m_radio_udp.AttachDlgItem(IDC_RADIO_PROTOCOL_UDP, *this);
	m_radio_icmp.AttachDlgItem(IDC_RADIO_PROTOCOL_ICMP, *this);
	switch (pconfig->m_nFilterProtocol) {
		case PROTOCOL_TYPE_TCP:
			m_radio_all.SetCheck(FALSE);
			m_radio_tcp.SetCheck(TRUE);
			m_radio_udp.SetCheck(FALSE);
			m_radio_icmp.SetCheck(FALSE);
			break;
		case PROTOCOL_TYPE_UDP:
			m_radio_all.SetCheck(FALSE);
			m_radio_tcp.SetCheck(FALSE);
			m_radio_udp.SetCheck(TRUE);
			m_radio_icmp.SetCheck(FALSE);
			break;
		case PROTOCOL_TYPE_ICMP:
			m_radio_all.SetCheck(FALSE);
			m_radio_tcp.SetCheck(FALSE);
			m_radio_udp.SetCheck(FALSE);
			m_radio_icmp.SetCheck(TRUE);
			break;
		default:
			pconfig->m_nFilterProtocol = 0;
			m_radio_all.SetCheck(TRUE);
			m_radio_tcp.SetCheck(FALSE);
			m_radio_udp.SetCheck(FALSE);
			m_radio_icmp.SetCheck(FALSE);
			break;
	}

	// ip地址
	m_radio_ip_all.AttachDlgItem(IDC_RADIO_IP_ALL, *this);
	m_radio_ip_source.AttachDlgItem(IDC_RADIO_IP_SOURCE, *this);
	m_radio_ip_dest.AttachDlgItem(IDC_RADIO_IP_DEST, *this);
	m_radio_ip_both.AttachDlgItem(IDC_RADIO_IP_BOTH, *this);
	m_ipaddr.AttachDlgItem(IDC_FILTER_IP_ADDR, *this);
	if (1 == pconfig->m_nFilterIPType) {
		m_radio_ip_all.SetCheck(FALSE);
		m_radio_ip_source.SetCheck(TRUE);
		m_radio_ip_dest.SetCheck(FALSE);
		m_radio_ip_both.SetCheck(FALSE);
		m_ipaddr.EnableWindow(TRUE);
	} else if (2 == pconfig->m_nFilterIPType) {
		m_radio_ip_all.SetCheck(FALSE);
		m_radio_ip_source.SetCheck(FALSE);
		m_radio_ip_dest.SetCheck(TRUE);
		m_radio_ip_both.SetCheck(FALSE);
		m_ipaddr.EnableWindow(TRUE);
	} else if (3 == pconfig->m_nFilterIPType) {
		m_radio_ip_all.SetCheck(FALSE);
		m_radio_ip_source.SetCheck(FALSE);
		m_radio_ip_dest.SetCheck(FALSE);
		m_radio_ip_both.SetCheck(TRUE);
		m_ipaddr.EnableWindow(TRUE);
	} else {
		m_radio_ip_all.SetCheck(TRUE);
		m_radio_ip_source.SetCheck(FALSE);
		m_radio_ip_dest.SetCheck(FALSE);
		m_radio_ip_both.SetCheck(FALSE);
		m_ipaddr.EnableWindow(FALSE);
	}
	m_ipaddr.SetAddress(pconfig->m_nFilterIP);

	// 端口
	m_radio_port_all.AttachDlgItem(IDC_RADIO_PORT_ALL, *this);
	m_radio_port_source.AttachDlgItem(IDC_RADIO_PORT_SOURCE, *this);
	m_radio_port_dest.AttachDlgItem(IDC_RADIO_PORT_DEST, *this);
	m_radio_port_both.AttachDlgItem(IDC_RADIO_PORT_BOTH, *this);
	m_filter_port.AttachDlgItem(IDC_FILTER_PORT, *this);
	m_filter_port.SetLimitText(5);
	if (1 == pconfig->m_nFilterPortType) {
		m_radio_port_all.SetCheck(FALSE);
		m_radio_port_source.SetCheck(TRUE);
		m_radio_port_dest.SetCheck(FALSE);
		m_radio_port_both.SetCheck(FALSE);
		m_filter_port.EnableWindow(TRUE);
	} else if (2 == pconfig->m_nFilterPortType) {
		m_radio_port_all.SetCheck(FALSE);
		m_radio_port_source.SetCheck(FALSE);
		m_radio_port_dest.SetCheck(TRUE);
		m_radio_port_both.SetCheck(FALSE);
		m_filter_port.EnableWindow(TRUE);
	} else if (3 == pconfig->m_nFilterPortType) {
		m_radio_port_all.SetCheck(FALSE);
		m_radio_port_source.SetCheck(FALSE);
		m_radio_port_dest.SetCheck(FALSE);
		m_radio_port_both.SetCheck(TRUE);
		m_filter_port.EnableWindow(TRUE);
	} else {
		m_radio_port_all.SetCheck(TRUE);
		m_radio_port_source.SetCheck(FALSE);
		m_radio_port_dest.SetCheck(FALSE);
		m_radio_port_both.SetCheck(FALSE);
		m_filter_port.EnableWindow(FALSE);
	}
	TCHAR temp1[32] = L"";
	wsprintf(temp1, L"%d", pconfig->m_nFilterPort);
	m_filter_port.SetWindowText(temp1);

	// 有效数据长度大于0
	m_check_valid_data_length.AttachDlgItem(IDC_CHECK_VALID_DATA_LENGTH, *this);
	m_check_valid_data_length.SetCheck(pconfig->m_bValidDataCheck);

	// 适配器列表
	m_adapterlist.AttachDlgItem(IDC_LISTVIEW_ADAPTER_LIST, *this);
	DWORD dwStyle = (DWORD)m_adapterlist.GetWindowLongPtr(GWL_STYLE);
	m_adapterlist.SetWindowLongPtr(GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);

	DWORD dwStyleEx = m_adapterlist.GetExtendedStyle();
	dwStyleEx |= LVS_EX_FULLROWSELECT;		// 选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyleEx |= LVS_EX_CHECKBOXES;			// item前生成checkbox控件
	m_adapterlist.SetExtendedStyle(dwStyleEx);

	TCHAR szString[3][32] = { L"Nr", L"ipaddr", L"descriptor" };
	int   columnWidth[3] = { 38, 102, 280 };

	// initialise the columns head
	LV_COLUMN lvColumn;
	ZeroMemory(&lvColumn, sizeof(LV_COLUMN));
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for (int i = 0; i < 3; ++i) {
		lvColumn.cx = columnWidth[i];
		lvColumn.pszText = szString[i];
		m_adapterlist.InsertColumn(i, lvColumn);
	}

	// initialise the columns data
	std::bitset<32> config_mask(pconfig->m_nUsedAdapterMask);
	for (int i = 0; i < CRawSocket::adapter_number; i++) {
		TCHAR temp[32] = L"";
		wsprintf(temp, L"%d", i);
		AddItem(temp);

		SetSubItem(i, 1, CRawSocket::adapter_ip[i]);
		SetSubItem(i, 2, CRawSocket::adapter_des[i]);

		if (config_mask[i]) {
			m_adapterlist.SetCheckState(i, TRUE);
		} else {
			m_adapterlist.SetCheckState(i, FALSE);
		}
	}

	HICON icon = LoadIcon(GetSplitApp().m_hInstance, MAKEINTRESOURCE(IDW_MAIN));
	SetIcon(icon, TRUE);			// 设置大图标
	SetIcon(icon, FALSE);			// 设置小图标

	GetDlgItem(IDC_BUTTON_CANCEL1).SetFocus();
	return FALSE;
}

int CConfigDialog::AddItem(LPCTSTR szText)
{
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.pszText = (LPTSTR)szText;
	lvi.iItem = m_adapterlist.GetItemCount();

	return m_adapterlist.InsertItem(lvi);
}

BOOL CConfigDialog::SetSubItem(int nItem, int nSubItem, LPCTSTR szText)
{
	LVITEM lvi1;
	ZeroMemory(&lvi1, sizeof(LVITEM));
	lvi1.mask = LVIF_TEXT;
	lvi1.iItem = nItem;
	lvi1.iSubItem = nSubItem;
	lvi1.pszText = (LPTSTR)szText;
	return (BOOL)m_adapterlist.SendMessage(LVM_SETITEM, 0, (LPARAM)&lvi1);
}

// mask在外部保存，其它的配置在OnOK()里面保存
void CConfigDialog::OnOK()
{
	CRegistryConfig *pconfig = CRegistryConfig::getInstance();

	// 协议：all, tcp, udp, icmp
	if (m_radio_tcp.GetCheck())
		pconfig->m_nFilterProtocol = PROTOCOL_TYPE_TCP;
	else if(m_radio_udp.GetCheck())
		pconfig->m_nFilterProtocol = PROTOCOL_TYPE_UDP;
	else if (m_radio_icmp.GetCheck())
		pconfig->m_nFilterProtocol = PROTOCOL_TYPE_ICMP;
	else
		pconfig->m_nFilterProtocol = PROTOCOL_TYPE_ALL;

	// 过滤ip
	DWORD t1;
	m_ipaddr.GetAddress(t1);
	pconfig->m_nFilterIP = t1;
	if (m_radio_ip_source.GetCheck()) {
		pconfig->m_nFilterIPType = 1;
	} else if (m_radio_ip_dest.GetCheck()) {
		pconfig->m_nFilterIPType = 2;
	} else if (m_radio_ip_both.GetCheck()) {
		pconfig->m_nFilterIPType = 3;
	} else {
		pconfig->m_nFilterIPType = 0;
	}

	// 过滤端口
	pconfig->m_nFilterPort = _wtoi(m_filter_port.GetWindowText());
	if (m_radio_port_source.GetCheck()) {
		pconfig->m_nFilterPortType = 1;
	} else if (m_radio_port_dest.GetCheck()) {
		pconfig->m_nFilterPortType = 2;
	} else if (m_radio_port_both.GetCheck()) {
		pconfig->m_nFilterPortType = 3;
	} else {
		pconfig->m_nFilterPortType = 0;
	}

	// 过滤适配器：结果存到m_nUsedAdapterMask而不是pconfig：只有这个配置在外部保存
	m_nUsedAdapterMask = 0;
	for (int i = 0; i < CRawSocket::adapter_number; i++) {
		if (m_adapterlist.GetCheckState(i))
			m_nUsedAdapterMask |= (1 << i);
	}

	// 有效数据长度大于0
	pconfig->m_bValidDataCheck = m_check_valid_data_length.GetCheck();

	// 退出
	pconfig->save();
	CDialog::OnOK();
}

void CConfigDialog::OnCancel()
{
	CDialog::OnCancel();
}
