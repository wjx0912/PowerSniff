///////////////////////////////////////////////////////
// Mainfrm.h
//  Declaration of the CMainFrame class

#ifndef MAINFRM_H
#define MAINFRM_H

#include <vector>
#include <list>
#include "PowerSniffPanes.h"
#include "MyStatusBar.h"
#include "ConfigDialog.h"
#include "AboutDialog.h"
#include "CompileDialogLua.h"
#include "CompileDialogTCC.h"
#include "../other/common.h"
#include "../other/RawSock.h"
#include "../other/PcapRW.h"
#include "../other/LuaSource.h"
#include "../other/PluginBase.h"
#include "../other/mutex.h"

class CMainFrame : public CDockFrame
{
	friend class CAboutDialog;
public:
	CMainFrame(void);
	virtual ~CMainFrame();
	virtual void OnDestroy();
	virtual BOOL LoadRegistrySettings(LPCTSTR szKeyName);
	virtual BOOL SaveRegistrySettings();

	void __SetReBarColors(COLORREF clrBkGnd1, COLORREF clrBkGnd2, COLORREF clrBand1, COLORREF clrBand2);
	void __SetCustomToolbarText(LPCTSTR text);		// 更新状态栏的字符串
	void SetCurrentItemSummary(TCHAR *textcontent);		// 更新listview的Plugin Summary一项
	void __InitPlugin(LPCTSTR filename);			// filename不包含path
	virtual int  OnCreate(CREATESTRUCT& rcs);
	void SetupToolBar();
	void __PluginScanner();
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnInitialUpdate();

public:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL OnDropFiles(LPCTSTR path);
	BOOL OnFileOpen();
	BOOL OnFileSave();
	BOOL OnFileExit();
	BOOL OnCaptureStart();
	BOOL OnCaptureStop();
	BOOL OnCaptureClear();
	BOOL OnOptions();
	BOOL OnAutoScroll();
	BOOL OnHelp();
	BOOL OnLuaCompileTest();
	BOOL OnTCCCompileTest();
	BOOL OnPluginReinit();

	void OnListItemChanged(void);
	void OnListItemDoubleClick(void);
	void __OnListItemClick(void);
	void OnChangeCursor(int start, int end);	// 鼠标在数据区位置变化时，显示到解析窗口
	void __OnGetdispinfoList(NMHDR *pNMHDRt);	// virtual list自画

private:
	void __DataPoolThread();
	void ProcessDataPool(BOOL bDrop);		// 数据的第一遍处理：在datapool对应的线程中（用插件处理）
	void ProcessDataOnUI(const struct PcapDataDescriptor &data1);		// 数据的第二遍处理：UI
	void OnTimer(UINT_PTR nIDEvent);

public:
	// 菜单，listview，3个docktext
	CMenu						m_menu;
	CViewList					m_listview;
	BOOL						m_bRefreshListview;	// 不要一来数据就刷新，用定时器限制一下
	CDockText					*m_pDockTextBottomLeftProtocolData;
	CDockText					*m_pDockTextBottomRightHexShower;
	CDockText					*m_pDockTextBottomRightPluginOutput;

private:
	CRawSocket					*m_rawsocket[32];	// 最多32个网卡，每个网卡一个对象
	BOOL						m_bStartCapture;	// 是否开始了抓包

	std::vector<struct PlugInDescriptor>		m_listPlugin;		// 所有lua和c插件的列表
	int						m_nSelectPlugin;	// 用户在菜单选中的插件序号，从0开始
public:
	CPluginBase					*m_pPlugin;		// 当前使用了哪个插件

private:
	int						m_nSelectListctrlItem;	// listview选中项
	TCHAR						*m_buffer;		// 临时缓冲

private:
	CMyMutex					m_mutex_datapool;
	std::vector<struct PcapDataDescriptor>		pcapdata_datapool;	// 数据来了之后，丢到缓冲池，然后马上返回，
										// 后台线程用插件处理缓冲池里面的包，然后丢到pcapdata_list
										// 通常情况下，缓冲池里面的包不会太多
	std::thread					*m_pDataPoolThread;	// 处理缓冲池数据的线程
	int						m_nDataPoolThreadID;	// 处理缓冲池数据的线程ID
	//CMyMutex					m_mutex_ui;		// 全部在ui线程处理，所以不需要锁
	std::list<struct PcapDataDescriptor>		pcapdata_ui;		// 数量不大于MAX_PACKET_NUMBER

private:
	CConfigDialog					m_ConfigDialog;
	CAboutDialog					m_AboutDialog;
	CCompileDialogLua				m_LuaCompileDialog;
	CCompileDialogTCC				m_TCCCompileDialog;
};

#define TIMER_REFRESH_STATUSBAR		1
#define TIMER_REFRESH_LISTCTRL		2

#endif

