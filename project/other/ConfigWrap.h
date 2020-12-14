#pragma once
#include <vector>

#define REGIST_KEY		L"PowerSniff"

class CRegistryConfig
{
private:
	CRegistryConfig();
	virtual ~CRegistryConfig();

public:
	static CRegistryConfig *getInstance();
	BOOL load();
	BOOL save();			// 退出，更改配置时调用

public:
	// Theme Settings：vector容量为2，表示工具栏和状态栏
	std::vector<UINT>	m_vBandIDs;
	std::vector<UINT>	m_vBandStyles;
	std::vector<UINT>	m_vBandSizes;

	// Program Settings：其它设置
	BOOL			m_bDebug;			// 是否输出调试到xtrace（仅软件作者使用）
	BOOL			m_bAutoScroll;			// 是否自动滚动listview到最底部
	TCHAR			m_strSelectedPlugin[256];	// 当前选中了插件的文件名，非全路径
								// （插件只放在plugin文件夹下）

	int			m_nFilterProtocol;		// 过滤协议：TCP, UDP, ICMP
	int			m_nFilterIPType;		// 过滤IP类型
	int			m_nFilterIP;			// 过滤IP
	int			m_nFilterPortType;		// 过滤端口类型
	int			m_nFilterPort;			// 过滤端口
	int			m_nUsedAdapterMask;		// 过滤网卡
	BOOL			m_bValidDataCheck;		// 有效数据长度大于0

	int			m_nWndHeightOrigData;		// 左下的数据窗口的高度
	int			m_nWndWidthPluginData;		// 右下的解析（或插件）窗口的宽度
};
