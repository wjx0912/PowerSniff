// 参考文章：
// https://www.boost.org/doc/libs/1_60_0/libs/smart_ptr/make_shared_array.html
#ifndef COMMON2018_H
#define COMMON2018_H
#include "../other/RawSock.h"

// 插件描述符
struct PlugInDescriptor
{
	TCHAR				filefullpath[256];	// 文件全路径
	TCHAR				descriptor[64];		// 文件名（不带路径）
	int				msg_id;			// 插件生成菜单时，对应的消息id, 从MENU_PLUGIN_START_ID开始
};

// 输出到"Plugin Output"窗口的一行文本，如果是多行用vector管理
struct PluginOutputItem
{
	int		r;
	int		g;
	int		b;
	int		fontsize;
	CString		text;
};

// rawsocket 或winpcap抓到的一帧数据
struct PcapDataDescriptor
{
	int				len;			// 数据长度（包含pcap帧头）
	unsigned char			*data1;			// 数据：包含了pcap帧头（抓包的时候malloc，注意需要free的地方）
	TCHAR				plugin_summary[256];	// 脚本使用plugin_summary()函数解析一帧数据得到的结果

	TCHAR				protocol[8];
	int				src_addr;
	TCHAR				src_addr_str[16];
	int				dst_addr;
	TCHAR				dst_addr_str[16];

	int				src_port;
	int				dst_port;

	int				total_size;		// 长度：不含pcap帧头
	int				data_size;		// 长度：纯数据长度

	int				capture_time_sec;
	int				capture_time_usec;
	TCHAR				capture_time_str[32];

	std::vector<PluginOutputItem>	plugin_output;		// 插件输出
};

enum PROTOCOL_TYPE
{
	PROTOCOL_TYPE_ALL		= 0,
	PROTOCOL_TYPE_TCP		= 1,
	PROTOCOL_TYPE_UDP		= 2,
	PROTOCOL_TYPE_ICMP		= 3
};

#define WM_USER_PLUGIN_ADD		(WM_USER + 20001)
#define WM_USER_POOL			(WM_USER + 20002)	// 前处理：处理缓冲池数据的线程
#define WM_USER_DATA			(WM_USER + 20003)	// 后处理：UI线程
#define WM_USER_RUN_LUA			(WM_USER + 20004)
#define WM_USER_RUN_TCC			(WM_USER + 20005)

#define MENU_PLUGIN_START_ID		0x6200			// 自定义菜单的消息id

#define COLOR_MAC			RGB(0xaa, 0xaa, 0xaa)
#define COLOR_IP			RGB(0x00, 0x00, 0xff)
#define COLOR_TCP_UDP			RGB(0xff, 0x00, 0x00)
#define COLOR_OPTION			RGB(0x00, 0x80, 0x00)
#define TCP_UDP_ITALIC			FALSE

inline TCHAR *__get_protocol(void *__data, int len)
{
	unsigned char *data = (unsigned char *)__data;

	switch (data[23]) {					// 23是数据中协议类型的位置
	case 6:
		return _T("tcp");
		break;
	case 17:
		return _T("udp");
		break;
	case 1:
		return _T("icmp");
		break;
	}

	return NULL;
}

inline int __get_data_len(void *__data, int len)
{
	unsigned char *data = (unsigned char *)__data;
	tcp_hdr *tcp_hdr1 = NULL;

	switch (data[23]) {					// 23是数据中协议类型的位置
	case 6:
		tcp_hdr1 = (tcp_hdr *)(data + sizeof(eth_hdr) + sizeof(ip_hdr));
		return len - 14 - 20 - tcp_hdr1->thl * 4;
		break;
	case 17:
		return len - 14 - 20 - 8;
		break;
	case 1:
		return len - 14 - 20 - 4;
		break;
	}
	return 0;
}

// 错误码转字符串
std::wstring LastErrorAsString(DWORD errorMessageID);

// 编辑器最大文本数量，快捷键
#define SCINTILLA_MAX_BUFFER_SIZE		(256 * 1024)
#define SCINTILLA_SHOTKEY_OPEN			0x5000
#define SCINTILLA_SHOTKEY_SAVE			0x5001
#define SCINTILLA_SHOTKEY_COMPILE		0x5002
#define SCINTILLA_SHOTKEY_RUN			0x5003

#endif //MAINFRM_H

