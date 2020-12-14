#ifndef __XTRACE_YONGJIAN_QQ80101277_2018_H
#define __XTRACE_YONGJIAN_QQ80101277_2018_H
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include <vector>
#include <string>
#include <memory>
#include <winsock.h>

// 如果想禁用XTRACE输出 请注释 #define ENABLE_XTRACE，这是所有功能总控开关
// 内置缓冲区大小：XTRACE_INTERNAL_BUFFER_SIZE，建议512, 1024, 2048, 4096...，不建议更改
#define ENABLE_XTRACE
#define ENABLE_XTRACE_PROFILE
#define XTRACE_INTERNAL_BUFFER_SIZE		4096			// 内置缓冲区大小
#define XTRACE_OUTPUT_PREFIX_PID					// 输出前缀：进程ID
#define XTRACE_OUTPUT_PREFIX_TIME					// 输出前缀：时间

// ================================================================================
// 从这里开始就不要改了
// ================================================================================
#define XTRACE_VERSION				"2018.10.10"
//#define __AUTHOR_DEBUG						// 作者调试开关，禁止更改！

// 内置颜色
#define COLOR_BLACK				RGB(0, 0, 0)		// 黑：默认颜色
#define COLOR_WHITE				RGB(255, 255, 255)	// 白
#define COLOR_RED				RGB(255, 0, 0)		// 红
#define COLOR_GREEN				RGB(0, 255, 0)		// 绿
#define COLOR_BLUE				RGB(0, 0, 255)		// 蓝
#define COLOR_GRAY				RGB(128, 128, 128)	// 灰色
#define COLOR_PURPLE				RGB(128, 0, 128)	// 紫色
#define COLOR_FUCHSIA				RGB(255, 0, 255)	// 紫红
#define COLOR_YELLOW				RGB(255, 255, 0)	// 黄色
#define COLOR_CYAN				RGB(0, 255, 255)	// 青色

// 更改计数器的类型：必须从1开始
#define XTRACE_MSG_TYPE_OUTPUT_STRING		0			// 输出文本
#define XTRACE_MSG_TYPE_COUNT_RESET		1			// 计数器重置
#define XTRACE_MSG_TYPE_COUNT_INC		2			// 计数器加
#define XTRACE_MSG_TYPE_COUNT_DEC		3			// 计数器减

#define XTRACE_PRINT_BIN_LINE_CHARS		16			// 每行二进制的数量

class __CAtoW
{
public:
	__CAtoW(LPCSTR pStr, UINT codePage = CP_ACP) : m_pStr(pStr) {
		if (pStr) {
			// Resize the vector and assign null WCHAR to each element
			int length = MultiByteToWideChar(codePage, 0, pStr, -1, NULL, 0) + 1;
			m_vWideArray.assign(length, L'\0');

			// Fill our vector with the converted WCHAR array
			MultiByteToWideChar(codePage, 0, pStr, -1, &m_vWideArray[0], length);
		}
	}
	~__CAtoW() {}
	operator LPCWSTR() { return m_pStr ? &m_vWideArray[0] : NULL; }
	//operator LPOLESTR() { return m_pStr ? (LPOLESTR)&m_vWideArray[0] : (LPOLESTR)NULL; }

private:
	__CAtoW(const __CAtoW&);
	__CAtoW& operator= (const __CAtoW&);
	std::vector<wchar_t> m_vWideArray;
	LPCSTR m_pStr;
};

class __CWtoA
{
public:
	__CWtoA(LPCWSTR pWStr, UINT codePage = CP_ACP) : m_pWStr(pWStr) {
		// Usage:
		//   CWtoA ansiString(L"Some Text");
		//   CWtoA utf8String(L"Some Text", CP_UTF8);
		//
		// or
		//   SetWindowTextA( WtoA(L"Some Text") ); The ANSI version of SetWindowText
		// Resize the vector and assign null char to each element
		int length = WideCharToMultiByte(codePage, 0, pWStr, -1, NULL, 0, NULL, NULL) + 1;
		m_vAnsiArray.assign(length, '\0');

		// Fill our vector with the converted char array
		WideCharToMultiByte(codePage, 0, pWStr, -1, &m_vAnsiArray[0], length, NULL, NULL);
	}

	~__CWtoA() {m_pWStr = 0; }
	operator LPCSTR() { return m_pWStr ? &m_vAnsiArray[0] : NULL; }

private:
	__CWtoA(const __CWtoA&);
	__CWtoA& operator= (const __CWtoA&);
	std::vector<char> m_vAnsiArray;
	LPCWSTR m_pWStr;
};

#ifndef ASSERT
#define ASSERT assert
#include <assert.h>
#endif

#pragma warning (disable : 4996)
#pragma warning (disable : 4310)

class DllLoad
{
public:
	DllLoad(LPCWSTR lpszName) {
		m_hModule = NULL;
#ifdef __AUTHOR_DEBUG
		if(NULL == m_hModule)
			m_hModule = ::LoadLibrary(L"C:\\Users\\wyt\\Desktop\\XTrace\\bin\\xTrace.dll");
		if(NULL == m_hModule)
			m_hModule = ::LoadLibrary(L"D:\\git\\XTrace\\bin\\xTrace.dll");
#else
		TCHAR szTemp[MAX_PATH] = L"";
		if(::GetTempPath(MAX_PATH - 1, szTemp)) {
			wcscat_s(szTemp, lpszName);
			m_hModule = ::LoadLibrary(szTemp);
		}
#endif
	}
	~DllLoad() {
		::FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
	operator  HMODULE() { return m_hModule;}
protected:
	HMODULE m_hModule;
};

typedef void (WINAPIV * MAGIC_TRACE_PROC)(int nMsg, WPARAM wParam, LPARAM lParam, WPARAM wParam2, LPARAM lParam2);

inline MAGIC_TRACE_PROC GetMagicTraceProc()
{
	static DllLoad xdl(L"xTRACE.dll");
	static MAGIC_TRACE_PROC fnMagicTraceProc = (MAGIC_TRACE_PROC)::GetProcAddress(HMODULE(xdl), "MagicTraceProc"); 
	return fnMagicTraceProc;
}

typedef void (WINAPIV * MAGIC_TRACE_VERSION)(TCHAR *buffer, int maxlen);

inline std::wstring XTraceVersion()
{
	static DllLoad xdl(L"xTRACE.dll");
	static MAGIC_TRACE_VERSION fnMagicTraceVersion = (MAGIC_TRACE_VERSION)::GetProcAddress(HMODULE(xdl), "MagicTraceVersion");
	TCHAR buffer[64] = L"";
	fnMagicTraceVersion(buffer, 64);
	return buffer;
}

inline void ____xTrace_unicode_raw(COLORREF textCr, LPCWSTR lpszFormat, va_list args)
{
	MAGIC_TRACE_PROC pMTrace = GetMagicTraceProc();
	if (!pMTrace)
		return;

	TCHAR *pszBuffer = (TCHAR *)malloc(XTRACE_INTERNAL_BUFFER_SIZE * sizeof(TCHAR));
	memset(pszBuffer, 0x0, XTRACE_INTERNAL_BUFFER_SIZE * sizeof(TCHAR));
	std::shared_ptr<TCHAR> _free_ptr((TCHAR *)pszBuffer, [](TCHAR *p){free(p);});

	// 显示消息内容
	int nBuf = wvsprintf(pszBuffer, lpszFormat, args);
	if (nBuf) {
		int len = wcslen(pszBuffer);
		int rest = len;
		TCHAR * pTemp = pszBuffer;
		while (rest) {
			int l = rest;
			if (l > 508) {
				l = 508;
				const TCHAR * p2 = CharPrevW(pTemp, pTemp + l);
				l = p2 - pTemp;
			}
			TCHAR szBuf[512] = L"";
			wcsncpy(szBuf, pTemp, l);
			pMTrace(XTRACE_MSG_TYPE_OUTPUT_STRING, textCr, (LPARAM)szBuf, 0, 0);
			pTemp += l;
			rest -= l;
		}
	}
}

inline void ____xTrace_unicode(COLORREF textCr, LPCWSTR lpszFormat, va_list args)
{
	MAGIC_TRACE_PROC pMTrace = GetMagicTraceProc();
	if (!pMTrace)
		return;

	TCHAR *pszBuffer = (TCHAR *)malloc(XTRACE_INTERNAL_BUFFER_SIZE * sizeof(TCHAR));
	memset(pszBuffer, 0x0, XTRACE_INTERNAL_BUFFER_SIZE * sizeof(TCHAR));
	std::shared_ptr<TCHAR> _free_ptr((TCHAR *)pszBuffer, [](TCHAR *p){free(p); });

	// 显示进程id
#ifdef XTRACE_OUTPUT_PREFIX_PID
	wsprintf(pszBuffer, L"[%08d] ", GetCurrentProcessId());
	pMTrace(XTRACE_MSG_TYPE_OUTPUT_STRING, textCr, (LPARAM)pszBuffer, 0, 0);
#endif

	// 显示时间
#ifdef XTRACE_OUTPUT_PREFIX_TIME
	SYSTEMTIME sys;
	GetLocalTime(&sys);

	wsprintf(pszBuffer
		, L"%d-%02d-%02d %02d:%02d:%02d.%04d "
		, sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds
		);
	pMTrace(XTRACE_MSG_TYPE_OUTPUT_STRING, textCr, (LPARAM)pszBuffer, 0, 0);
#endif

	// 显示消息内容
	int nBuf = wvsprintf(pszBuffer, lpszFormat, args);
	if (nBuf) {
		int len = wcslen(pszBuffer);
		int rest = len;
		TCHAR * pTemp = pszBuffer;
		while (rest) {
			int l = rest;
			if (l > 508) {
				l = 508;
				const TCHAR * p2 = CharPrevW(pTemp, pTemp + l);
				l = p2 - pTemp;
			}
			TCHAR szBuf[512] = L"";
			wcsncpy(szBuf, pTemp, l);
			pMTrace(XTRACE_MSG_TYPE_OUTPUT_STRING, textCr, (LPARAM)szBuf, 0, 0);
			pTemp += l;
			rest -= l;
		}
	}
}

// unicode封装
inline void __xTraceRaw(COLORREF textCr, LPCWSTR lpszFormat, ...)
{
	va_list ap;
	va_start(ap, lpszFormat);
	____xTrace_unicode_raw(textCr, lpszFormat, ap);
	va_end(ap);
}

// ANSI封装
inline void __xTraceRaw(COLORREF textCr, LPCSTR lpszFormat, ...)
{
	char *pszBuffer = (char *)malloc(XTRACE_INTERNAL_BUFFER_SIZE * sizeof(char));
	memset(pszBuffer, 0x0, XTRACE_INTERNAL_BUFFER_SIZE * sizeof(char));
	std::shared_ptr<char> _free_ptr((char *)pszBuffer, [](char *p){free(p); });

	va_list ap;
	va_start(ap, lpszFormat);
	vsprintf(pszBuffer, lpszFormat, ap);
	va_end(ap);

	// 转换成unicode输出
	__xTraceRaw(textCr, L"%s", std::wstring(__CAtoW(pszBuffer)).c_str());
}

// unicode封装1
inline void __xTrace(COLORREF textCr, LPCWSTR lpszFormat, ...)
{
	va_list ap;
	va_start(ap, lpszFormat);
	____xTrace_unicode(textCr, lpszFormat, ap);
	va_end(ap);
}

// unicode封装2
inline void __xTrace(LPCWSTR lpszFormat, ...)
{
	va_list ap;
	va_start(ap, lpszFormat);
	____xTrace_unicode(COLOR_BLACK, lpszFormat, ap);
	va_end(ap);
}

// ANSI封装1
inline void __xTrace(COLORREF textCr, LPCSTR lpszFormat, ...)
{
	char *pszBuffer = (char *)malloc(XTRACE_INTERNAL_BUFFER_SIZE * sizeof(char));
	memset(pszBuffer, 0x0, XTRACE_INTERNAL_BUFFER_SIZE * sizeof(char));
	std::shared_ptr<char> _free_ptr((char *)pszBuffer, [](char *p){free(p); });

	va_list ap;
	va_start(ap, lpszFormat);
	vsprintf(pszBuffer, lpszFormat, ap);
	va_end(ap);

	// 转换成unicode输出
	__xTrace(textCr, L"%s", std::wstring(__CAtoW(pszBuffer)).c_str());
}

// ANSI封装2
inline void __xTrace(LPCSTR lpszFormat, ...)
{
	char *pszBuffer = (char *)malloc(XTRACE_INTERNAL_BUFFER_SIZE * sizeof(char));
	memset(pszBuffer, 0x0, XTRACE_INTERNAL_BUFFER_SIZE * sizeof(char));
	std::shared_ptr<char> _free_ptr((char *)pszBuffer, [](char *p){free(p); });

	va_list ap;
	va_start(ap, lpszFormat);
	vsprintf(pszBuffer, lpszFormat, ap);
	va_end(ap);

	// 转换成unicode输出
	__xTrace(COLOR_BLACK, L"%s", std::wstring(__CAtoW(pszBuffer)).c_str());
}

inline void __xCount(int type, int id, int value)
{
	MAGIC_TRACE_PROC pMTrace = GetMagicTraceProc();
	if (!pMTrace && type >= XTRACE_MSG_TYPE_COUNT_RESET && type <= XTRACE_MSG_TYPE_COUNT_DEC)
		return;

	pMTrace(type, id, value, 0, 0);
}

//#include <winsock.h>
//#pragma comment(lib, "wsock32.lib")

//WSADATA wsaData;
//int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);

// 网络版本暂时只支持unicode
// 协议格式参考：CXTraceMonitorDlg::OnNetData()
inline void __xTraceNet(std::wstring ip, int port, COLORREF textCr, LPCWSTR lpszFormat, ...)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		return;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons((u_short)port);
	addr.sin_addr.s_addr = inet_addr(__CWtoA(ip.c_str()));

	TCHAR *pszBuffer = (TCHAR *)malloc(XTRACE_INTERNAL_BUFFER_SIZE * sizeof(TCHAR));
	memset(pszBuffer, 0x0, XTRACE_INTERNAL_BUFFER_SIZE * sizeof(TCHAR));
	std::shared_ptr<TCHAR> _free_ptr((TCHAR *)pszBuffer, [](TCHAR *p){free(p); });
	
#if 1
	va_list ap;
	va_start(ap, lpszFormat);
	int nBuf = wvsprintf(pszBuffer + 4, lpszFormat, ap);
	va_end(ap);

	char *buffer = (char *)pszBuffer;
	buffer[0] = (char)0x5a;
	buffer[1] = (char)0xa5;
	buffer[2] = (char)0xff;
	buffer[3] = (char)XTRACE_MSG_TYPE_OUTPUT_STRING;
	memcpy(buffer + 4, &textCr, sizeof(COLORREF));
	int len = 8 + nBuf * 2 + 2;
#endif

	sendto(sockfd, buffer, len, 0, (struct sockaddr*)&addr, sizeof(addr));
	closesocket(sockfd);
}

// 协议格式参考：CXTraceMonitorDlg::OnNetData()
inline void __xCountNet(std::wstring ip, int port, int type, int id, int value)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		return;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons((u_short)port);
	addr.sin_addr.s_addr = inet_addr(__CWtoA(ip.c_str()));

#if 1
	char buffer[32];
	buffer[0] = (char)0x5a;
	buffer[1] = (char)0xa5;
	buffer[2] = (char)0xff;
	buffer[3] = (char)type;
	memcpy(buffer + 4, &id, sizeof(COLORREF));
	memcpy(buffer + 8, &value, sizeof(COLORREF));
#endif

	sendto(sockfd, buffer, 12, 0, (struct sockaddr*)&addr, sizeof(addr));
	closesocket(sockfd);
}

static inline void __printline(COLORREF textCr, int addr, const void *buffer, int len)
{
	TCHAR outstr[256] = L"";
	TCHAR temp[8] = L"";
	wsprintf(outstr, L",0X%04X: ", addr);

#if 1
	for (int i = 0; i < len; i++) {
		wsprintf(temp, L"%02X ", ((const unsigned char *)buffer)[i]);
		wcscat(outstr, temp);
		if (7 == i)
			wcscat(outstr, L" ");
	}
	if (len < 8)
		wcscat(outstr, L" ");
	for (int i = len; i < XTRACE_PRINT_BIN_LINE_CHARS; i++)
		wcscat(outstr, L"   ");
#endif

#if 1
	wcscat(outstr, L" | ");
	for (int i = 0; i < len; i++) {
		char tempAnsi[8];
		if (L'%' == ((const unsigned char *)buffer)[i]) {
			strcpy(tempAnsi, "%%");
		} else if (isprint(((const unsigned char *)buffer)[i])) {
			sprintf(tempAnsi, "%c", ((const unsigned char *)buffer)[i]);
		} else {
			sprintf(tempAnsi, "%c", L'.');
		}
		wcscat(outstr, __CAtoW(tempAnsi));
		if (7 == i)
			wcscat(outstr, L" ");
	}
	if (len < 8)
		wcscat(outstr, L" ");
	for (int i = len; i < XTRACE_PRINT_BIN_LINE_CHARS; i++)
		wcscat(outstr, L" ");
#endif

	wcscat(outstr, L"\n");
	__xTrace(textCr, outstr);
}

static inline void __printbin(COLORREF textCr, const void *buffer, int len)
{
	int i;
	for (i = 0; i < len; i += XTRACE_PRINT_BIN_LINE_CHARS) {
		int leave = len - i;
		if (leave >= XTRACE_PRINT_BIN_LINE_CHARS)
			__printline(textCr, i, (const char *)buffer + i, XTRACE_PRINT_BIN_LINE_CHARS);
		else
			__printline(textCr, i, (const char *)buffer + i, leave);
	}
}

static inline void __printbin(const void *buffer, int len)
{
	__printbin(RGB(0, 0, 0), buffer, len);
}

static inline void __printline_raw(COLORREF textCr, int addr, const void *buffer, int len)
{
	TCHAR outstr[256] = L"";
	TCHAR temp[8] = L"";
	wsprintf(outstr, L"0X%04X: ", addr);

#if 1
	for (int i = 0; i < len; i++) {
		wsprintf(temp, L"%02X ", ((const unsigned char *)buffer)[i]);
		wcscat(outstr, temp);
		if (7 == i)
			wcscat(outstr, L" ");
	}

	if (len < 8)
		wcscat(outstr, L" ");
	for (int i = len; i < XTRACE_PRINT_BIN_LINE_CHARS; i++)
		wcscat(outstr, L"   ");
#endif

#if 1
	wcscat(outstr, L" | ");
	for (int i = 0; i < len; i++) {
		char tempAnsi[8];
		if (L'%' == ((const unsigned char *)buffer)[i]) {
			strcpy(tempAnsi, "%%");
		} else if (isprint(((const unsigned char *)buffer)[i])) {
			sprintf(tempAnsi, "%c", ((const unsigned char *)buffer)[i]);
		} else {
			sprintf(tempAnsi, "%c", L'.');
		}
		wcscat(outstr, __CAtoW(tempAnsi));
		if (7 == i)
			wcscat(outstr, L" ");
	}
	if (len < 8)
		wcscat(outstr, L" ");
	for (int i = len; i < XTRACE_PRINT_BIN_LINE_CHARS; i++)
		wcscat(outstr, L" ");
#endif

	wcscat(outstr, L"\n");
	__xTraceRaw(textCr, outstr);
}

static inline void __printbin_raw(COLORREF textCr, const void *buffer, int len)
{
	int i;
	for (i = 0; i < len; i += XTRACE_PRINT_BIN_LINE_CHARS) {
		int leave = len - i;
		if (leave >= XTRACE_PRINT_BIN_LINE_CHARS)
			__printline_raw(textCr, i, (const char *)buffer + i, XTRACE_PRINT_BIN_LINE_CHARS);
		else
			__printline_raw(textCr, i, (const char *)buffer + i, leave);
	}
}

static inline void __printbin_raw(const void *buffer, int len)
{
	__printbin_raw(RGB(0, 0, 0), buffer, len);
}

// =======================================
static inline void SleepUs(ULONG ulMicroSeconds)
{
	LARGE_INTEGER  timeStop;
	LARGE_INTEGER  timeStart;
	LARGE_INTEGER  Freq;
	LONGLONG  ulTimeToWait;

	if (!QueryPerformanceFrequency(&Freq))
		return;

	QueryPerformanceCounter(&timeStart);

	ulTimeToWait = Freq.QuadPart * ulMicroSeconds / 1000 / 1000;
	timeStop = timeStart;

	while ((timeStop.QuadPart - timeStart.QuadPart) < ulTimeToWait) {
		QueryPerformanceCounter(&timeStop);
	}
}

struct _xprofile
{
	_xprofile() {
		clear();
	}
	void clear() {
		count		= 0;
		total_us	= 0.0;
		current_us	= -1.0;
		min_us		= -1.0;
		max_us		= -1.0;
	}
	int		count;
	double		total_us;
	double		current_us;
	double		min_us;
	double		max_us;
	LONGLONG	t1;		// 起始时间
	LONGLONG	t2;		// 结束时间
};
static std::vector<_xprofile>	g_profile;
static LONGLONG			g_persecond = 0;
static inline void _xprofile_init(int vector_size)
{
	g_profile.clear();
	g_profile.reserve(vector_size);
	for (int i = 0; i < vector_size; i++) {
		g_profile.push_back(_xprofile());
		g_profile[i].clear();
	}
	QueryPerformanceFrequency((LARGE_INTEGER *)&g_persecond);
}

static inline void _xprofile_start(int i)
{
	QueryPerformanceCounter((LARGE_INTEGER *)&g_profile[i].t1);
}

static inline void _xprofile_end(int i)
{
	QueryPerformanceCounter((LARGE_INTEGER *)&g_profile[i].t2);
	g_profile[i].count++;
	g_profile[i].current_us = static_cast<double>((g_profile[i].t2 - g_profile[i].t1) * 1000000.0 / g_persecond);
	g_profile[i].total_us += g_profile[i].current_us;
	if (g_profile[i].min_us < 0) {
		g_profile[i].min_us = g_profile[i].current_us;
	} else {
		if (g_profile[i].current_us < g_profile[i].min_us)
			g_profile[i].min_us = g_profile[i].current_us;
	}
	if (g_profile[i].max_us < 0) {
		g_profile[i].max_us = g_profile[i].current_us;
	} else {
		if (g_profile[i].current_us > g_profile[i].max_us)
			g_profile[i].max_us = g_profile[i].current_us;
	}
}

static inline void _xprofile_print(int i)
{
	TCHAR buffer[256] = L"";

	swprintf(buffer
		, L"profile-%2d, count: %8d, total: "
		, i
		, g_profile[i].count
		);
	__xTraceRaw(COLOR_BLACK, buffer);

	swprintf(buffer
		, L"%12.2lf"
		, g_profile[i].total_us
		);
	__xTraceRaw(COLOR_RED, buffer);

	swprintf(buffer
		, L"us, current: %8.2lfus, average: %8.2lfus, min: %8.2lfus, max: %8.2lfus\n"
		, g_profile[i].current_us
		, g_profile[i].total_us / g_profile[i].count
		, g_profile[i].min_us
		, g_profile[i].max_us
		);
	__xTraceRaw(COLOR_BLACK, buffer);
}

static inline void _xprofile_printall()
{
	for (int i = 0; i < (int)g_profile.size(); i++) {
		if (g_profile[i].count > 0)
			_xprofile_print(i);
	}
}

// =======================================
#ifdef ENABLE_XTRACE
#define XTRACE					__xTrace
#define XTRACE_RAW				__xTraceRaw
#define XTRACE_NET				__xTraceNet
#define XTRACE_LINE()				XTRACE("%s(%d)\n", __FUNCTION__, __LINE__)
#define XCOUNT_RESET(id, value)			__xCount(XTRACE_MSG_TYPE_COUNT_RESET, id, value)
#define XCOUNT_INC(id, value)			__xCount(XTRACE_MSG_TYPE_COUNT_INC, id, value)
#define XCOUNT_DEC(id, value)			__xCount(XTRACE_MSG_TYPE_COUNT_DEC, id, value)
#define XCOUNT_NET_RESET(ip, port, id, value)	__xCountNet(ip, port, XTRACE_MSG_TYPE_COUNT_RESET, id, value)
#define XCOUNT_NET_INC(ip, port, id, value)	__xCountNet(ip, port, XTRACE_MSG_TYPE_COUNT_INC, id, value)
#define XCOUNT_NET_DEC(ip, port, id, value)	__xCountNet(ip, port, XTRACE_MSG_TYPE_COUNT_DEC, id, value)
#define printbin				__printbin
#define printbin_raw				__printbin_raw
#else
#define XTRACE
#define XTRACE_RAW
#define XTRACE_NET
#define XTRACE_LINE()
#define XCOUNT_RESET(id, value)
#define XCOUNT_INC(id, value)
#define XCOUNT_DEC(id, value)
#define XCOUNT_NET_RESET(ip, port, id, value)
#define XCOUNT_NET_INC(ip, port, id, value)
#define XCOUNT_NET_DEC(ip, port, id, value)
#define printbin
#define printbin_raw
#endif

#if (defined ENABLE_XTRACE) && (defined ENABLE_XTRACE_PROFILE)
#define XPROFILE_INIT		_xprofile_init
#define XPROFILE_START		_xprofile_start
#define XPROFILE_END		_xprofile_end
#define XPROFILE_PRINT		_xprofile_print
#define XPROFILE_PRINTALL	_xprofile_printall	
#else
#define XPROFILE_INIT
#define XPROFILE_START
#define XPROFILE_END
#define XPROFILE_PRINT
#define XPROFILE_PRINTALL()
#endif

// =======================================
#endif
