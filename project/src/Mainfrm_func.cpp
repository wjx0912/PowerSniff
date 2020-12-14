////////////////////////////////////////////////////
// Mainfrm.cpp  - definitions for the CMainFrame class

#include "stdafx.h"
#include <time.h>
#include <bitset>
#include "mainfrm.h"
#include "resource.h"
#include "MyStatusBar.h"
#include "../other/PluginLua.h"
#include "../other/PluginTCC.h"
#include "../other/ConfigWrap.h"

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (LOWORD(wParam)) {
	case IDM_FILE_OPEN:
	case IDT_TOOLBAR_OPEN:
		return OnFileOpen();
	case IDM_FILE_SAVE:
	case IDT_TOOLBAR_SAVE:
		return OnFileSave();
	case IDM_FILE_EXIT:
		return OnFileExit();
	case IDM_CAPTURE_START:
	case IDT_TOOLBAR_START:
		return OnCaptureStart();
	case IDM_CAPTURE_STOP:
	case IDT_TOOLBAR_STOP:
		return OnCaptureStop();
	case IDM_CAPTURE_CLEAR:
	case IDT_TOOLBAR_CLEAR:
		return OnCaptureClear();
	case IDM_VIEW_OPTIONS:
	case IDT_TOOLBAR_OPTIONS:
		return OnOptions();
	case IDM_LUA_COMPILE_TEST:
		return OnLuaCompileTest();
	case IDM_TCC_COMPILE_TEST:
		return OnTCCCompileTest();
	case IDM_PLUGIN_REINIT:
		return OnPluginReinit();
	case IDM_HELP_ABOUT:
	case IDT_TOOLBAR_ABOUT:
		return OnHelp();
	case IDW_VIEW_STATUSBAR:
		return OnViewStatusBar();
	case IDW_VIEW_TOOLBAR:
		return OnViewToolBar();
	case ID_OPTIONS_AUTOSCROLL:
		return OnAutoScroll();
	}
	if (wParam >= MENU_PLUGIN_START_ID && wParam <= MENU_PLUGIN_START_ID + 100) {
		CRegistryConfig *pconfig = CRegistryConfig::getInstance();

		if (m_bStartCapture) {
			//MessageBox(L"Need stop capture first.", L"ERROR", MB_ICONERROR);
			//return FALSE;
		}

		// 获取点击的菜单信息
		int nNewSelectPlugin = wParam - MENU_PLUGIN_START_ID;
		TCHAR strNewSelectPlugin[256] = L"";
		m_menu.GetSubMenu(2).GetSubMenu(5).GetMenuString(wParam, strNewSelectPlugin, 512, 0);

		// 先把老的插件取消掉：考虑到后台线程正在用插件，用下面的backup，只有可以不用锁
		if (m_pPlugin) {
			CPluginBase *backup = m_pPlugin;
			m_pPlugin = NULL;

			backup->unload();
			delete backup;
		}
		if (m_nSelectPlugin >= 0) {
			m_menu.GetSubMenu(2).GetSubMenu(5).CheckMenuItem(wParam, FALSE);
			if (m_nSelectPlugin == nNewSelectPlugin) {
				m_nSelectPlugin = -1;
				wcscpy(pconfig->m_strSelectedPlugin, L"");
				pconfig->save();
				return FALSE;
			}
		} else {
			m_nSelectPlugin = nNewSelectPlugin;
		}

		// 选中新的插件
		m_menu.GetSubMenu(2).GetSubMenu(5).CheckMenuRadioItem(MENU_PLUGIN_START_ID
			, MENU_PLUGIN_START_ID + 100, wParam, 0);
		wcscpy(pconfig->m_strSelectedPlugin, strNewSelectPlugin);
		pconfig->save();

		// 初始化新插件lua代码
		__InitPlugin(strNewSelectPlugin);
	}
	return FALSE;
}

BOOL CMainFrame::OnDropFiles(LPCTSTR path)
{
	OnCaptureClear();

	CPcapRead pcapread;
	int ret = pcapread.OpenFile(path);
	if (0 == ret) {
	} else if (-1 == ret) {
		MessageBox(L"Can't open file!", L"ERROR", MB_ICONERROR);
		return FALSE;
	} else {
		MessageBox(L"Invalid pcap format!", L"ERROR", MB_ICONERROR);
		return FALSE;
	}

	unsigned char *buffer = (unsigned char *)malloc(0x40000);
	if (buffer) {
		int len;
		while (pcapread.ReadPcapData(buffer, &len)) {
			if (len <= 0)
				continue;

			// 一帧数据
			PcapDataDescriptor des;
			des.len = len;
			des.data1 = (unsigned char *)malloc(len);	// 内存分配！！！
			memcpy(des.data1, buffer, len);
			memset(des.plugin_summary, 0x0, sizeof(des.plugin_summary));

			// 丢到datapool里面
			m_mutex_datapool.Lock();
			pcapdata_datapool.push_back(des);
			m_mutex_datapool.Unlock();
		}
	}
	free(buffer);
	::PostThreadMessage(m_nDataPoolThreadID, WM_USER_POOL, 0, 0);

	return TRUE;
}

BOOL CMainFrame::OnFileOpen()
{
	m_listview.SetFocus();

	CFileDialog dlg(TRUE, L"Open File", NULL, OFN_FILEMUSTEXIST, NULL);
	if (dlg.DoModal(*this) == IDOK) {
		CString str = dlg.GetPathName();
		OnDropFiles(str.c_str());
	}

	return TRUE;
}

BOOL CMainFrame::OnFileSave()
{
	if (pcapdata_ui.size() <= 0)
		return TRUE;
	m_listview.SetFocus();

	// 保存对话框
	CString Filters(L"Wireshark/tcpdump File (*.pcap)\0*.pcap\0\0");
	CFileDialog dlg(FALSE, L"Save File", NULL, OFN_FILEMUSTEXIST, Filters);
	if (dlg.DoModal(*this) == IDOK) {
		CString str = dlg.GetPathName();
		CPcapWrite pcapwrite;
		if (0 == pcapwrite.OpenFile(str)) {
			// 保存pcap头
			pcapwrite.WritePcapHead();
			//m_mutex_ui.Lock();
			for (auto it = pcapdata_ui.begin(); it != pcapdata_ui.end(); it++) {
				pcapwrite.WritePcapData(it->data1, it->len);	// 保存一帧数据
			}
			//m_mutex_ui.Unlock();
			pcapwrite.Sync();
		} else {
			MessageBox(L"Can't write file!", L"ERROR", MB_ICONERROR);
		}
	}

	return TRUE;
}

BOOL CMainFrame::OnFileExit()
{
	// Issue a close request to the frame
	PostMessage(WM_CLOSE);

	return TRUE;
}

BOOL CMainFrame::OnCaptureStart()
{
	CRegistryConfig *pconfig = CRegistryConfig::getInstance();
	m_listview.SetFocus();
	GetToolBar().DisableButton(IDT_TOOLBAR_START);
	GetToolBar().EnableButton(IDT_TOOLBAR_STOP);
	m_menu.GetSubMenu(1).CheckMenuRadioItem(IDM_CAPTURE_START, IDM_CAPTURE_STOP, IDM_CAPTURE_START, 0);

	try {
		m_bStartCapture = TRUE;
		std::bitset<32> config_mask(pconfig->m_nUsedAdapterMask);
		for (int i = 0; i < CRawSocket::adapter_number; i++) {
			if (config_mask[i]) {
				m_rawsocket[i]->Start();
			} else {
				m_rawsocket[i]->Stop();
			}
		}
	} catch (...) {
	}

	return TRUE;
}

BOOL CMainFrame::OnCaptureStop()
{
	m_listview.SetFocus();
	GetToolBar().EnableButton(IDT_TOOLBAR_START);
	GetToolBar().DisableButton(IDT_TOOLBAR_STOP);
	m_menu.GetSubMenu(1).CheckMenuRadioItem(IDM_CAPTURE_START, IDM_CAPTURE_STOP, IDM_CAPTURE_STOP, 0);

	try {
		m_bStartCapture = FALSE;
		for (int i = 0; i < CRawSocket::adapter_number; i++) {
			m_rawsocket[i]->Stop();
		}
	} catch (...) {
	}

	return TRUE;
}

BOOL CMainFrame::OnCaptureClear()
{
	m_listview.SetFocus();
	GetToolBar().DisableButton(IDT_TOOLBAR_SAVE);
	GetToolBar().DisableButton(IDT_TOOLBAR_CLEAR);
	m_nSelectListctrlItem = -1;

	// 暂停抓包
	BOOL oldState = m_bStartCapture;
	m_bStartCapture = FALSE;

#if 1
	m_pDockTextBottomLeftProtocolData->TextClear();
	m_pDockTextBottomRightPluginOutput->TextClear();

	//m_mutex_ui.Lock();
	for (auto it = pcapdata_ui.begin(); it != pcapdata_ui.end(); it++) {
		if (it->data1)
			free(it->data1);
	}
	pcapdata_ui.clear();
	//m_mutex_ui.Unlock();
#endif

	// 恢复抓包
	m_bStartCapture = oldState;
	m_bRefreshListview = TRUE;
	return TRUE;
}

BOOL CMainFrame::OnOptions()
{
	CRegistryConfig *pconfig = CRegistryConfig::getInstance();
	m_listview.SetFocus();

	if (MB_OKCANCEL == m_ConfigDialog.DoModal(GetHwnd())) {
		if (m_ConfigDialog.m_nUsedAdapterMask != pconfig->m_nUsedAdapterMask) {
			std::bitset<32> dialog_mask(m_ConfigDialog.m_nUsedAdapterMask);
			std::bitset<32> config_mask(pconfig->m_nUsedAdapterMask);
			for (int i = 0; i < CRawSocket::adapter_number; i++) {
				if (NULL == m_rawsocket[i])
					continue;

				if (dialog_mask[i] == config_mask[i])
					continue;

				if (m_bStartCapture) {
					try {
						if (dialog_mask[i])
							m_rawsocket[i]->Start();
						else
							m_rawsocket[i]->Stop();
					} catch (...) {
					}
				}
			}

			pconfig->m_nUsedAdapterMask = m_ConfigDialog.m_nUsedAdapterMask;
			pconfig->save();
		}
	}

	return TRUE;
}

BOOL CMainFrame::OnAutoScroll()
{
	CRegistryConfig *pconfig = CRegistryConfig::getInstance();

	if (pconfig->m_bAutoScroll) {
		pconfig->m_bAutoScroll = FALSE;		// 取消
		m_menu.GetSubMenu(2).CheckMenuItem(2, MF_BYPOSITION | MF_UNCHECKED);
	} else {
		pconfig->m_bAutoScroll = TRUE;		// 选中
		m_menu.GetSubMenu(2).CheckMenuItem(2, MF_BYPOSITION | MF_CHECKED);
	}

	pconfig->save();
	return TRUE;
}

BOOL CMainFrame::OnHelp()
{
	m_AboutDialog.DoModal(GetHwnd());
	return TRUE;
}

BOOL CMainFrame::OnLuaCompileTest()
{
	if (m_LuaCompileDialog.GetHwnd() == NULL)
		m_LuaCompileDialog.DoModeless(GetHwnd());
	else
		m_LuaCompileDialog.ShowWindow(TRUE);
	return TRUE;
}

BOOL CMainFrame::OnTCCCompileTest()
{
	if (m_TCCCompileDialog.GetHwnd() == NULL)
		m_TCCCompileDialog.DoModeless(GetHwnd());
	else
		m_TCCCompileDialog.ShowWindow(TRUE);
	return TRUE;
}

BOOL CMainFrame::OnPluginReinit()
{
	if (!m_pPlugin) {
		MessageBox(L"Need stop capture first.", L"ERROR", MB_ICONERROR);
	} else {
		TCHAR path[256] = L"";
		wcscpy(path, m_pPlugin->getPluginPath());

		__InitPlugin(path);
	}
	return TRUE;
}

void CMainFrame::OnListItemChanged(void)
{
	int nItem = (int)m_listview.SendMessage(LVM_GETNEXTITEM, (WPARAM)-1, (LPARAM)MAKELPARAM(LVNI_FOCUSED, 0));
	if (nItem < 0 || nItem == m_nSelectListctrlItem)
		return;
	else
		m_nSelectListctrlItem = nItem;

	if (nItem >= (int)pcapdata_ui.size())
		return;

	// 清空
	m_pDockTextBottomLeftProtocolData->TextClear();

	// 获取当前选中行的对象数据
	//m_mutex_ui.Lock();
	auto it = pcapdata_ui.begin();
	advance(it, m_nSelectListctrlItem);
	PcapDataDescriptor data = *it;
	unsigned char *source = data.data1;
	data.data1 = (unsigned char *)malloc(data.len);
	memcpy(data.data1, source, data.len);
	//m_mutex_ui.Unlock();
	std::shared_ptr<void> _free_ptr((void *)data.data1, [](void *p) {
		free(p);
	});

	// 解析
	unsigned char *p = data.data1 + sizeof(struct pcaprec_hdr_s);
	int len = data.len - sizeof(struct pcaprec_hdr_s);

	int div, mod, i, j;
	div = len / 16;
	mod = len % 16;
	TCHAR temp[32] = L"";
	wcscpy(m_buffer, L"");
	for (i = 0; i < div; i++) {
		// 地址
		swprintf(temp, L"%08X  ", (int)(i * 16));
		wcscat(m_buffer, temp);

		// 显示16进制数据
		for (j = 0; j < 16; j++) {
			if (8 == j)
				wcscat(m_buffer, L" ");

			swprintf(temp, L"%02X ", p[i * 16 + j]);
			wcscat(m_buffer, temp);
		}
		wcscat(m_buffer, L"  ");

		// 显示可打印字符
		for (j = 0; j < 16; j++) {
			if (8 == j)
				wcscat(m_buffer, L" ");

			if (isprint(p[i * 16 + j]))
				swprintf(temp, L"%c", p[i * 16 + j]);
			else
				swprintf(temp, L"%c", _T('.'));
			wcscat(m_buffer, temp);
		}
		wcscat(m_buffer, L" \n");
	}

	// 地址
	swprintf(temp, L"%08X  ", (int)(i * 16));
	wcscat(m_buffer, temp);

	// 剩余的16进制数据
	for (j = 0; j < mod; j++) {
		if (8 == j)
			wcscat(m_buffer, L" ");

		swprintf(temp, L"%02X ", p[i * 16 + j]);
		wcscat(m_buffer, temp);
	}

	for (j = mod; j < 16; j++) {
		if (8 == j)
			wcscat(m_buffer, L" ");

		wcscat(m_buffer, L"   ");
	}
	wcscat(m_buffer, L"  ");

	// 剩余的可打印字符
	for (j = 0; j < mod; j++) {
		if (8 == j)
			wcscat(m_buffer, L" ");

		if (isprint(p[i * 16 + j]))
			swprintf(temp, L"%c", p[i * 16 + j]);
		else
			swprintf(temp, L"%c", _T('.'));
		wcscat(m_buffer, temp);
	}

	// 从这里开始，设置文本颜色
	m_pDockTextBottomLeftProtocolData->TextAppend(RGB(0, 0, 0), DEFAULT_RICHEDIT_TEXT_SIZE, m_buffer);

	switch (p[23]) {
	case 6:
	{
		// mac头: 14字节
		m_pDockTextBottomLeftProtocolData->TextSetColor(10, 53, COLOR_MAC, DEFAULT_RICHEDIT_TEXT_SIZE);
		m_pDockTextBottomLeftProtocolData->TextSetColor(61, 76, COLOR_MAC, DEFAULT_RICHEDIT_TEXT_SIZE);		// ascii

		// ip头：20字节
		m_pDockTextBottomLeftProtocolData->TextSetColor(53, 59, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);
		m_pDockTextBottomLeftProtocolData->TextSetColor(76, 78, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);		// ascii
		m_pDockTextBottomLeftProtocolData->TextSetColor(90, 139, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);
		m_pDockTextBottomLeftProtocolData->TextSetColor(141, 158, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);	// ascii
		m_pDockTextBottomLeftProtocolData->TextSetColor(170, 176, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);
		m_pDockTextBottomLeftProtocolData->TextSetColor(221, 223, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);	// ascii

		// tcp头：20字节
		m_pDockTextBottomLeftProtocolData->TextSetColor(176, 219, COLOR_TCP_UDP, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);
		m_pDockTextBottomLeftProtocolData->TextSetColor(223, 238, COLOR_TCP_UDP, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
		m_pDockTextBottomLeftProtocolData->TextSetColor(250, 268, COLOR_TCP_UDP, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);
		m_pDockTextBottomLeftProtocolData->TextSetColor(301, 307, COLOR_TCP_UDP, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii

		// tcp 可选项: 最多40字节！！！浅蓝色
		tcp_hdr *tcp_hdr1 = (tcp_hdr *)(p + sizeof(eth_hdr) + sizeof(ip_hdr));
		int option_len = tcp_hdr1->thl * 4 - 20;
		if (option_len > 0) {
			if (option_len / 4 >= 1) {
				m_pDockTextBottomLeftProtocolData->TextSetColor(268, 281, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// 13字符
				m_pDockTextBottomLeftProtocolData->TextSetColor(307, 312, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
			}
			if (option_len / 4 >= 2) {
				m_pDockTextBottomLeftProtocolData->TextSetColor(281, 293, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);
				m_pDockTextBottomLeftProtocolData->TextSetColor(312, 316, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
			}
			if (option_len / 4 >= 3) {
				m_pDockTextBottomLeftProtocolData->TextSetColor(293, 299, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// 有换行
				m_pDockTextBottomLeftProtocolData->TextSetColor(316, 318, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
				m_pDockTextBottomLeftProtocolData->TextSetColor(330, 336, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);
				m_pDockTextBottomLeftProtocolData->TextSetColor(381, 383, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
			}
			if (option_len / 4 >= 4) {
				m_pDockTextBottomLeftProtocolData->TextSetColor(336, 348, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);
				m_pDockTextBottomLeftProtocolData->TextSetColor(383, 387, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
			}
			if (option_len / 4 >= 5) {
				m_pDockTextBottomLeftProtocolData->TextSetColor(348, 361, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// 13字符
				m_pDockTextBottomLeftProtocolData->TextSetColor(387, 392, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
			}
			if (option_len / 4 >= 6) {
				m_pDockTextBottomLeftProtocolData->TextSetColor(361, 373, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);
				m_pDockTextBottomLeftProtocolData->TextSetColor(392, 396, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
			}
			if (option_len / 4 >= 7) {
				m_pDockTextBottomLeftProtocolData->TextSetColor(373, 379, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// 有换行
				m_pDockTextBottomLeftProtocolData->TextSetColor(396, 398, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
				m_pDockTextBottomLeftProtocolData->TextSetColor(410, 416, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);
				m_pDockTextBottomLeftProtocolData->TextSetColor(461, 463, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
			}
			if (option_len / 4 >= 8) {
				m_pDockTextBottomLeftProtocolData->TextSetColor(416, 428, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);
				m_pDockTextBottomLeftProtocolData->TextSetColor(463, 467, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
			}
			if (option_len / 4 >= 9) {
				m_pDockTextBottomLeftProtocolData->TextSetColor(428, 441, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// 13字符
				m_pDockTextBottomLeftProtocolData->TextSetColor(467, 472, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
			}
			if (option_len / 4 >= 10) {
				m_pDockTextBottomLeftProtocolData->TextSetColor(441, 453, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);
				m_pDockTextBottomLeftProtocolData->TextSetColor(472, 476, COLOR_OPTION, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
			}
		}
		break;
	}
	case 17:
	{
		// mac头: 14字节
		m_pDockTextBottomLeftProtocolData->TextSetColor(10, 53, COLOR_MAC, DEFAULT_RICHEDIT_TEXT_SIZE);
		m_pDockTextBottomLeftProtocolData->TextSetColor(61, 76, COLOR_MAC, DEFAULT_RICHEDIT_TEXT_SIZE);		// ascii

		// ip头：20字节
		m_pDockTextBottomLeftProtocolData->TextSetColor(53, 59, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);
		m_pDockTextBottomLeftProtocolData->TextSetColor(76, 78, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);		// ascii
		m_pDockTextBottomLeftProtocolData->TextSetColor(90, 139, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);
		m_pDockTextBottomLeftProtocolData->TextSetColor(141, 158, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);	// ascii
		m_pDockTextBottomLeftProtocolData->TextSetColor(170, 176, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);
		m_pDockTextBottomLeftProtocolData->TextSetColor(221, 223, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);	// ascii

		// udp头：8字节
		m_pDockTextBottomLeftProtocolData->TextSetColor(176, 201, COLOR_TCP_UDP, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);
		m_pDockTextBottomLeftProtocolData->TextSetColor(223, 232, COLOR_TCP_UDP, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
		break;
	}
	case 1:
	{
		// mac头: 14字节
		m_pDockTextBottomLeftProtocolData->TextSetColor(10, 53, COLOR_MAC, DEFAULT_RICHEDIT_TEXT_SIZE);		// hex
		m_pDockTextBottomLeftProtocolData->TextSetColor(61, 76, COLOR_MAC, DEFAULT_RICHEDIT_TEXT_SIZE);		// ascii

		// ip头：20字节
		m_pDockTextBottomLeftProtocolData->TextSetColor(53, 59, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);		// hex
		m_pDockTextBottomLeftProtocolData->TextSetColor(76, 78, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);		// ascii
		m_pDockTextBottomLeftProtocolData->TextSetColor(90, 139, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);		// hex
		m_pDockTextBottomLeftProtocolData->TextSetColor(141, 158, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);	// ascii
		m_pDockTextBottomLeftProtocolData->TextSetColor(170, 176, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);	// hex
		m_pDockTextBottomLeftProtocolData->TextSetColor(221, 223, COLOR_IP, DEFAULT_RICHEDIT_TEXT_SIZE);	// ascii

		// icmp头：4字节
		m_pDockTextBottomLeftProtocolData->TextSetColor(176, 188, COLOR_TCP_UDP, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// hex
		m_pDockTextBottomLeftProtocolData->TextSetColor(223, 227, COLOR_TCP_UDP, DEFAULT_RICHEDIT_TEXT_SIZE, TCP_UDP_ITALIC);	// ascii
		break;
	}
	default:
	{
		break;
	}
	}

	__OnListItemClick();
}

void CMainFrame::OnListItemDoubleClick(void)
{
	if (m_nSelectListctrlItem < 0)
		return;

	// 获取当前选中行的对象数据
	//m_mutex_ui.Lock();
	auto it = pcapdata_ui.begin();
	advance(it, m_nSelectListctrlItem);
	PcapDataDescriptor data = *it;
	unsigned char *source = data.data1;
	data.data1 = (unsigned char *)malloc(data.len);
	memcpy(data.data1, source, data.len);
	//m_mutex_ui.Unlock();
	std::shared_ptr<void> _free_ptr((void *)data.data1, [](void *p) {
		free(p);
	});

	// 执行插件里面的handle_double
	void *data111 = (void *)(data.data1 + sizeof(struct pcaprec_hdr_s));
	int len111 = data.len - sizeof(struct pcaprec_hdr_s);
	if (m_pPlugin) {
		m_pPlugin->lock();
		wcscpy(m_pPlugin->m_plugin_summary, L"");
		m_pPlugin->handle_double(__get_protocol(data111, len111), data111, len111, __get_data_len(data111, len111));
		wcscpy(m_pPlugin->m_plugin_summary, L"");
		m_pPlugin->unlock();
	}
}

void CMainFrame::__OnListItemClick(void)
{
	if (m_nSelectListctrlItem < 0)
		return;

	// 获取当前选中行的对象数据
	//m_mutex_ui.Lock();
	auto it = pcapdata_ui.begin();
	advance(it, m_nSelectListctrlItem);
	PcapDataDescriptor data = *it;
	unsigned char *source = data.data1;
	data.data1 = (unsigned char *)malloc(data.len);
	memcpy(data.data1, source, data.len);
	//m_mutex_ui.Unlock();
	std::shared_ptr<void> _free_ptr((void *)data.data1, [](void *p) {
		free(p);
	});

	// 执行插件里面的handle_click
	void *data111 = (void *)(data.data1 + sizeof(struct pcaprec_hdr_s));
	int len111 = data.len - sizeof(struct pcaprec_hdr_s);
	if (m_pPlugin) {
		m_pPlugin->lock();
		wcscpy(m_pPlugin->m_plugin_summary, L"");
		m_pPlugin->handle_click(__get_protocol(data111, len111), data111, len111, __get_data_len(data111, len111));
		wcscpy(m_pPlugin->m_plugin_summary, L"");
		m_pPlugin->unlock();
	}
}

void CMainFrame::OnChangeCursor(int start, int end)
{
	UNREFERENCED_PARAMETER(end);

	if (m_nSelectListctrlItem < 0)
		return;

	// 获取当前选中行的对象数据
	//m_mutex_ui.Lock();
	auto it = pcapdata_ui.begin();
	advance(it, m_nSelectListctrlItem);
	PcapDataDescriptor data = *it;
	unsigned char *source = data.data1;
	data.data1 = (unsigned char *)malloc(data.len);
	memcpy(data.data1, source, data.len);
	//m_mutex_ui.Unlock();
	std::shared_ptr<void> _free_ptr((void *)data.data1, [](void *p) {
		free(p);
	});

	// 计算当前鼠标指向数据的哪个位置
	int line = start / 80;
	int offset_char = start % 80;
	int offset = -1;
	if (offset_char >= 10 && offset_char <= 33) {
		offset = (offset_char - 10) / 3;

	} else if (offset_char >= 35 && offset_char <= 58) {
		offset = 8 + (offset_char - 35) / 3;
	}

	// 位置找到了，显示她，值存储到value
	int value = 0;
	if (offset >= 0) {
		if ((line * 16 + offset) < (data.len - 16))		// 16 是 sizeof struct pcaprec_hdr_s
		{
			if ((line * 16 + offset + 4) < (data.len - 16)) {
				memcpy(&value, data.data1 + 16 + line * 16 + offset, 4);
			} else {
				memcpy(&value, data.data1 + 16 + line * 16 + offset, data.len - 16 - line * 16 - offset);
			}

			// 显示
			m_pDockTextBottomRightHexShower->TextClear();
			TCHAR buffer[2048];
			swprintf(buffer,
				L"bytes : %02X %02X %02X %02X\n"
				L"hex32 : 0x%08X\n"
				L"int8  : %-12d\nuint8 : %-12d\n"
				L"int16 : %-12d\nuint16: %-12d\n"
				L"int32 : %-12d\nuint32: %-12u\n"
				, value & 0xff, (value >> 8) & 0xff, (value >> 16) & 0xff, (value >> 24) & 0xff
				, value
				, (char)value
				, (unsigned char)value
				, (short)value
				, (unsigned short)value
				, (int)value
				, (unsigned int)value
			);
			m_pDockTextBottomRightHexShower->TextAppend(RGB(0x0, 0x0, 0x0), DEFAULT_RICHEDIT_TEXT_SIZE, buffer);

			// GB2312
			int value_backup = value;
			unsigned char *p = (unsigned char *)&value_backup;
			if (p[0] >= 32 && p[0] <= 126) {
				swprintf(buffer, L"ascii : %c\n", p[0]);		// ASCII
				m_pDockTextBottomRightHexShower->TextAppend(RGB(0x0, 0x0, 0x0), DEFAULT_RICHEDIT_TEXT_SIZE, buffer);
			} else {
				m_pDockTextBottomRightHexShower->TextAppend(RGB(0x0, 0x0, 0x0), DEFAULT_RICHEDIT_TEXT_SIZE, L"ascii : \n");
			}
			if (p[0] >= 0xA1 && p[0] <= 0xF7 && p[1] >= 0xA1 && p[1] <= 0xFE) {
				p[2] = 0x0;
				p[3] = 0x0;
				m_pDockTextBottomRightHexShower->TextAppend(RGB(0x0, 0x0, 0x0), DEFAULT_RICHEDIT_TEXT_SIZE, L"gb2312: ");
				::MultiByteToWideChar(936, 0, (LPCSTR)p, -1, buffer, 2);
				m_pDockTextBottomRightHexShower->TextAppend(RGB(0x0, 0x0, 0x0), DEFAULT_RICHEDIT_TEXT_SIZE, buffer);
				m_pDockTextBottomRightHexShower->TextAppend(RGB(0x0, 0x0, 0x0), DEFAULT_RICHEDIT_TEXT_SIZE, L"\n");
				//swprintf(buffer, L"gb2312:%s\n", (TCHAR *)p);
			} else {
				m_pDockTextBottomRightHexShower->TextAppend(RGB(0x0, 0x0, 0x0), DEFAULT_RICHEDIT_TEXT_SIZE, L"gb2312: \n");
			}

			// 时间
			time_t t1 = value;
			struct tm *tm1 = localtime((const time_t *)&t1);
			if (tm1) {
				swprintf(buffer, L"time_t: %d-%d-%d %d:%d:%d"
					, tm1->tm_year + 1900
					, tm1->tm_mon + 1
					, tm1->tm_mday
					, tm1->tm_hour
					, tm1->tm_min
					, tm1->tm_sec
				);
				m_pDockTextBottomRightHexShower->TextAppend(RGB(0x0, 0x0, 0x0), DEFAULT_RICHEDIT_TEXT_SIZE, buffer);
			} else {
				m_pDockTextBottomRightHexShower->TextAppend(RGB(0x0, 0x0, 0x0), DEFAULT_RICHEDIT_TEXT_SIZE, L"time_t: ");
			}
		}
	}
}

// 参考：https://www.codeproject.com/Articles/7891/Using-virtual-lists
void CMainFrame::__OnGetdispinfoList(NMHDR *pNMHDR)
{
	LV_DISPINFO *pDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM *pItem = &(pDispInfo)->item;

	int itemid = pItem->iItem;		// 需要显示的行
	if (itemid >= (int)pcapdata_ui.size())
		return;

	// 获取当前选中行的对象数据
	//m_mutex_ui.Lock();
	auto it = pcapdata_ui.begin();
	advance(it, itemid);
	PcapDataDescriptor data = *it;
	data.data1 = NULL;
	//m_mutex_ui.Unlock();

	// 处理列
	TCHAR buffer[32] = L"";
	if (0 == pItem->iSubItem) {		// Index
		swprintf(buffer, L"%d", itemid + 1);
		wcsncpy(pItem->pszText, buffer, pItem->cchTextMax);
	} else if (1 == pItem->iSubItem) {	// Protocol
		wcsncpy(pItem->pszText, data.protocol, pItem->cchTextMax);
	} else if (2 == pItem->iSubItem) {	// Source Address
		wcsncpy(pItem->pszText, data.src_addr_str, pItem->cchTextMax);
	} else if (3 == pItem->iSubItem) {	// Dest Address
		wcsncpy(pItem->pszText, data.dst_addr_str, pItem->cchTextMax);
	} else if (4 == pItem->iSubItem) {	// Source Port
		swprintf(buffer, L"%d", data.src_port);
		wcsncpy(pItem->pszText, buffer, pItem->cchTextMax);
	} else if (5 == pItem->iSubItem) {	// Dest Port
		swprintf(buffer, L"%d", data.dst_port);
		wcsncpy(pItem->pszText, buffer, pItem->cchTextMax);
	} else if (6 == pItem->iSubItem) {	// Total Size
		swprintf(buffer, L"%d", data.total_size);
		wcsncpy(pItem->pszText, buffer, pItem->cchTextMax);
	} else if (7 == pItem->iSubItem) {	// Data Size
		swprintf(buffer, L"%d", data.data_size);
		wcsncpy(pItem->pszText, buffer, pItem->cchTextMax);
	} else if (8 == pItem->iSubItem) {	// Capture Time
		wcsncpy(pItem->pszText, data.capture_time_str, pItem->cchTextMax);
	} else if (9 == pItem->iSubItem) {	// Plugin Summary
		wcsncpy(pItem->pszText, data.plugin_summary, pItem->cchTextMax);
	}
}
