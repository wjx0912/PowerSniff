////////////////////////////////////////////////////
// Mainfrm.cpp  - definitions for the CMainFrame class

#include "stdafx.h"
#include <time.h>
#include "mainfrm.h"
#include "resource.h"
#include "MyStatusBar.h"
#include "../other/PluginLua.h"
#include "../other/PluginTCC.h"
#include "../other/ConfigWrap.h"

// 判断：是否过滤协议族，ip地址，端口，有效数据返回TRUE，需要丢弃返回FALSE；
// 顺便解析协议，ip，端口和数据大小到data对象中
// 这个函数在抓包线程里面执行，必须快速返回
static BOOL __simple_parse(struct PcapDataDescriptor &data)
{
	unsigned char *buffer = data.data1;
	CRegistryConfig *pconfig = CRegistryConfig::getInstance();

	// 每帧pcap数据的头；23是数据中协议类型的位置
	data.total_size = data.len - PCAP_HEAD_SIZE;		// 长度：不含pcap帧头
	void *_data111 = (void *)(data.data1 + sizeof(struct pcaprec_hdr_s));
	int _len111 = data.len - sizeof(struct pcaprec_hdr_s);
	data.data_size = __get_data_len(_data111, _len111);	// 长度：纯数据长度

#if 1
	// 有效数据长度大于0
	if (pconfig->m_bValidDataCheck) {
		if (data.data_size <= 0)
			return FALSE;
	}
#endif

	// 解析src和dst的ip地址
	ip_hdr *ip1 = (ip_hdr *)(buffer + sizeof(struct pcaprec_hdr_s) + sizeof(eth_hdr));
	data.src_addr = (int)htonl(ip1->srcaddr.S_un.S_addr);
	data.dst_addr = (int)htonl(ip1->dstaddr.S_un.S_addr);

#if 1
	unsigned char *src_addr_p = (unsigned char *)&data.src_addr;
	swprintf(data.src_addr_str, L"%d.%d.%d.%d", src_addr_p[3], src_addr_p[2], src_addr_p[1], src_addr_p[0]);
	unsigned char *dst_addr_p = (unsigned char *)&data.dst_addr;
	swprintf(data.dst_addr_str, L"%d.%d.%d.%d", dst_addr_p[3], dst_addr_p[2], dst_addr_p[1], dst_addr_p[0]);

	// 过滤ip地址
	if (0 == pconfig->m_nFilterIPType) {				// 0是all
	} else if (1 == pconfig->m_nFilterIPType) {			// 1是source
		if (pconfig->m_nFilterIP == data.src_addr) {
		} else {
			return FALSE;
		}
	} else if (2 == pconfig->m_nFilterIPType) {			// 2是dest
		if (pconfig->m_nFilterIP == data.dst_addr) {
		} else {
			return FALSE;
		}
	} else if (3 == pconfig->m_nFilterIPType) {			// 3是both
		if (pconfig->m_nFilterIP == data.src_addr || pconfig->m_nFilterIP == data.dst_addr) {
		} else {
			return FALSE;
		}
	}
#endif

	// lua脚本处理：如果返回delete则删除这个包，给lua函数的参数：index, protocol, time, totallen, datalen, data
	int pos = sizeof(struct pcaprec_hdr_s) + 23;
	if (6 == (int)buffer[pos]) {
		wcscpy(data.protocol, L"TCP");

		// 过滤协议族
		if (PROTOCOL_TYPE_ALL != pconfig->m_nFilterProtocol && PROTOCOL_TYPE_TCP != pconfig->m_nFilterProtocol)
			return FALSE;

		// 解析端口
		tcp_hdr *tcp1 = (tcp_hdr *)(buffer + sizeof(struct pcaprec_hdr_s) + sizeof(eth_hdr) + sizeof(ip_hdr));
		data.src_port = (int)ntohs(tcp1->src_port);
		data.dst_port = (int)ntohs(tcp1->dst_port);
	} else if (17 == (int)buffer[pos]) {
		wcscpy(data.protocol, L"UDP");

		// 过滤协议族
		if (PROTOCOL_TYPE_ALL != pconfig->m_nFilterProtocol && PROTOCOL_TYPE_UDP != pconfig->m_nFilterProtocol)
			return FALSE;

		// 解析端口
		udp_hdr *udp1 = (udp_hdr *)(buffer + sizeof(struct pcaprec_hdr_s) + sizeof(eth_hdr) + sizeof(ip_hdr));
		data.src_port = (int)ntohs(udp1->src_port);
		data.dst_port = (int)ntohs(udp1->dst_port);
	} else if (1 == (int)buffer[pos]) {
		wcscpy(data.protocol, L"ICMP");

		// 过滤协议族
		if (PROTOCOL_TYPE_ALL != pconfig->m_nFilterProtocol && PROTOCOL_TYPE_ICMP != pconfig->m_nFilterProtocol)
			return FALSE;

		// 解析端口
		data.src_port = 0;
		data.dst_port = 0;

		// 过滤端口
		if (pconfig->m_nFilterPort) {
			return FALSE;
		}
	} else {
		return FALSE;
	}

#if 1
	// 过滤端口（只需要过滤tcp，udp；ICMP是没有端口的）
	if (6 == (int)buffer[pos] || 17 == (int)buffer[pos]) {		// only TCP/UDP
		if (0 == pconfig->m_nFilterPortType) {			// 0是all
		} else if (1 == pconfig->m_nFilterPortType) {		// 1是source
			if (pconfig->m_nFilterPort == data.src_port) {
			} else {
				return FALSE;
			}
		} else if (2 == pconfig->m_nFilterPortType) {		// 2是dest
			if (pconfig->m_nFilterPort == data.dst_port) {
			} else {
				return FALSE;
			}
		} else if (3 == pconfig->m_nFilterPortType) {		// 3是both
			if (pconfig->m_nFilterPort == data.src_port || pconfig->m_nFilterPort == data.dst_port) {
			} else {
				return FALSE;
			}
		}
	}
#endif

	// 解析时间：实时抓包和加载pcap文件都需要解析时间
	pcaprec_hdr_s *_pcap_hdr = (pcaprec_hdr_s *)buffer;
	data.capture_time_sec = _pcap_hdr->ts_sec;
	data.capture_time_usec = _pcap_hdr->ts_usec;
	struct tm tm1;
	time_t capture_time_sec = data.capture_time_sec;
	localtime_s(&tm1, (const time_t *)&capture_time_sec);
	swprintf(data.capture_time_str, L"%d-%02d-%02d %02d:%02d:%02d:%03d"
		, tm1.tm_year + 1900
		, tm1.tm_mon + 1
		, tm1.tm_mday
		, tm1.tm_hour
		, tm1.tm_min
		, tm1.tm_sec
		, data.capture_time_usec / 1000
	);
	return TRUE;
}

void CMainFrame::__DataPoolThread()
{
	MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);		// 强制系统创建消息队列
	while (GetMessage(&msg, NULL, 0, 0)) {
		switch (msg.message) {
		case WM_USER_POOL:
			ProcessDataPool(FALSE);
			break;
		default:
			DispatchMessage(&msg);
			break;
		}
	}
}

// 循环处理缓冲池数据，直到没有
void CMainFrame::ProcessDataPool(BOOL bDrop)
{
	while (pcapdata_datapool.size() > 0) {
		m_mutex_datapool.Lock();
		struct PcapDataDescriptor data = pcapdata_datapool.front();
		pcapdata_datapool.erase(pcapdata_datapool.begin());
		m_mutex_datapool.Unlock();

		if (bDrop) {
			free(data.data1);
			continue;
		} else {
			// 协议，ip，端口是否被用户过滤
			if (!__simple_parse(data)) {
				free(data.data1);
				continue;
			}

			// 执行插件里面的handle_data
			if (m_pPlugin) {
				void *data111 = (void *)(data.data1 + sizeof(struct pcaprec_hdr_s));
				int len111 = data.len - sizeof(struct pcaprec_hdr_s);

				try {
					m_pPlugin->lock();
					wcscpy(m_pPlugin->m_plugin_summary, L"");
					m_pPlugin->m_delete = FALSE;
					m_pPlugin->handle_data(__get_protocol(data111, len111), data111, len111, __get_data_len(data111, len111));
					wcscpy(data.plugin_summary, m_pPlugin->m_plugin_summary);
					wcscpy(m_pPlugin->m_plugin_summary, L"");
					m_pPlugin->unlock();
				} catch (...) {
				}

				if (m_pPlugin->m_delete) {
					free(data.data1);
					continue;
				}
			}

			std::unique_ptr<PcapDataDescriptor> myParams(new PcapDataDescriptor());
			*myParams = data;

			if (!::IsWindow(*this)) {
				free(data.data1);
				this->PostMessage(WM_QUIT);
			} else {
				this->PostMessage(WM_USER_DATA, reinterpret_cast<WPARAM>(myParams.release()), 0);
			}
		}
	}
}

#define __PACKET_LIST_SHRINK_SIZE1	(MAX_PACKET_NUMBER * 10 / 100)		// 每次收缩数据包数量：10%
#define __PACKET_LIST_SHRINK_SIZE2	(__PACKET_LIST_SHRINK_SIZE1 <= 100000? __PACKET_LIST_SHRINK_SIZE1: 100000)

// 处理WM_USER_DATA消息：来了一个数据包（插件已经处理完毕），加到队列，并加到listview控件
void CMainFrame::ProcessDataOnUI(const struct PcapDataDescriptor &data1)
{
	// 数据丢到池中
	//m_mutex_ui.Lock();
	pcapdata_ui.push_back(data1);
	if (pcapdata_ui.size() >= MAX_PACKET_NUMBER) {
		// 限制队列大小：到达最大数量时，每次删除若干条
		for (int i = 0; i < __PACKET_LIST_SHRINK_SIZE2; i++) {
			struct PcapDataDescriptor data = pcapdata_ui.front();
			free(data.data1);
			pcapdata_ui.erase(pcapdata_ui.begin());
		}
	}
	//m_mutex_ui.Unlock();

	m_bRefreshListview = TRUE;		// 显示新数据，并自动滚动到最后面一行

	;					// 更新状态栏：放到ui线程的OnTimer里面做
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_REFRESH_STATUSBAR == nIDEvent) {
		// 更新状态栏：放到ui线程的OnTimer里面做
		TCHAR temp1[128] = L"";
		if (m_nSelectListctrlItem >= 0)
			swprintf(temp1, L"Total packet: %d, Select packet index: %d", pcapdata_ui.size(), m_nSelectListctrlItem + 1);
		else
			swprintf(temp1, L"Total packet: %d", pcapdata_ui.size());

		CRegistryConfig *pconfig = CRegistryConfig::getInstance();
		if (pconfig->m_bDebug) {
			XCOUNT_RESET(0, (int)pcapdata_ui.size());
			XCOUNT_RESET(10, (int)pcapdata_datapool.size());
		}

		__SetCustomToolbarText(temp1);

		// 更新工具栏
		GetToolBar().EnableButton(IDT_TOOLBAR_SAVE);
		if (m_listview.GetItemCount() > 0) {
			if (m_bStartCapture) {
				GetToolBar().EnableButton(IDT_TOOLBAR_STOP);
			}
			GetToolBar().EnableButton(IDT_TOOLBAR_CLEAR);
		}

	}

	if (TIMER_REFRESH_LISTCTRL == nIDEvent && m_bRefreshListview) {
		m_bRefreshListview = FALSE;

		//m_listview.LockWindowUpdate();
		m_listview.SetItemCountEx(pcapdata_ui.size(), LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL);
		m_listview.ScrollToBottom();
		//m_listview.UnLockWindowUpdate();
	}
}
