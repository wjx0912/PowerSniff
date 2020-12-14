#include "stdafx.h"
#include <iostream>
#include <IPHlpApi.h>
#include <IPIfCons.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include "RawSock.h"
#include "PcapRW.h"
#include "../src/Mainfrm.h"

using namespace std;

int CRawSocket::adapter_number = -1;
TCHAR CRawSocket::adapter_ip[MAX_ADAPTER_NUMBER][32] = {0 };
TCHAR CRawSocket::adapter_des[MAX_ADAPTER_NUMBER][1024] = {0 };

int CRawSocket::EnumAdapters()
{
	if(adapter_number > 0)
		return 0;

	if(adapter_number == 0)
		return -1;

	adapter_number = 0;
	WSADATA	WSAData;
	if(0 != ::WSAStartup(MAKEWORD(2, 2), &WSAData)) {
		MessageBox (NULL, L"WSAStartup() failed.", L"ERROR", MB_ICONERROR);
		return -2;
	}

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	IP_ADAPTER_INFO *pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));

	if (ERROR_BUFFER_OVERFLOW == GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) {
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen);
	}

	if (NO_ERROR != GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) {
		free(pAdapterInfo);
		return -3;
	}

	for (IP_ADAPTER_INFO *pAdapter = pAdapterInfo; NULL != pAdapter && adapter_number < MAX_ADAPTER_NUMBER; ) {
		if (0 != strcmp("0.0.0.0", pAdapter->IpAddressList.IpAddress.String)) {
			wcscpy(adapter_ip[adapter_number], __CAtoW(pAdapter->IpAddressList.IpAddress.String));
			wcscpy(adapter_des[adapter_number], __CAtoW(pAdapter->Description));
			adapter_number++;
		}
		pAdapter = pAdapter->Next;
	}

	free(pAdapterInfo);

#if 1
	// 补丁
	if (0 == adapter_number) {
		wcscpy(adapter_ip[adapter_number], L"0.0.0.0");
		wcscpy(adapter_des[adapter_number], L"capture all networks");
		adapter_number++;
	}
#endif

	return 0;
}

std::wstring LastErrorAsString(DWORD errorMessageID)
{
	if (0 == errorMessageID)
		return std::wstring();

	LPTSTR messageBuffer = nullptr;
	size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
		,NULL ,errorMessageID ,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&messageBuffer, 0, NULL);

	std::wstring message(messageBuffer, size);
	LocalFree(messageBuffer);

	return message;
}

void CRawSocket::Init(int adapter_id, CMainFrame *pFrame, std::function<int(const char *buffer, int len)> pFunc)
{
	UnInit();

	m_nAdapterID = adapter_id;
	m_pFunc = pFunc;

	// 初始化socket
	s = ::socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (INVALID_SOCKET == (int)s) {
		std::wstring strError = L"Create socket failed:\r\n" + LastErrorAsString(::GetLastError());
		MessageBox(pFrame->GetHwnd(), strError.c_str(), L"ERROR", MB_ICONERROR);
		return;
	}

	ZeroMemory(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(__CWtoA(CRawSocket::adapter_ip[m_nAdapterID]));
	if (-1 == ::bind(s, (SOCKADDR *)&sa, sizeof(SOCKADDR))) {
		MessageBox(NULL, L"bind() failed.", L"ERROR", MB_ICONERROR);
		return;
	}

#if 1
	// 超时设置: windows
	int timeout = 100;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
#else
	// 超时设置: linux
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 100 * 1000;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));
#endif

	// 创建捕获线程
	m_pThread = new std::thread(&CRawSocket::__CaptureThread, this);
	return;
}

void CRawSocket::UnInit()
{
	if (m_nAdapterID < 0)
		return;

	m_nAdapterID = -1;
	m_bEnabled = FALSE;

	s = INVALID_SOCKET;
	sa = { 0 };
	ZeroMemory(buffer, MAX_PACKET_SIZE);

	::closesocket(s);
	s = INVALID_SOCKET;

	// 停止线程
	if (m_pThread) {
		m_pThread->join();
		delete m_pThread;
		m_pThread = NULL;
		m_pFunc = nullptr;
	}
}

// 接收处理raw socket数据
// windows的raw socket限制了获取mac头，所以只能填0
void CRawSocket::__CaptureThread()
{
	while (TRUE) {
		int _count = ::recv(s, (char*)buffer + PCAP_HEAD_SIZE + IPV4_ETH_HEAD_SIZE
			, MAX_PACKET_SIZE - PCAP_HEAD_SIZE - IPV4_ETH_HEAD_SIZE, 0);

		if (SOCKET_ERROR == _count) {
			// https://docs.microsoft.com/zh-cn/windows/desktop/api/winsock/nf-winsock-recv
			int err = ::WSAGetLastError();
			if (WSAETIMEDOUT == err)
				continue;
			else
				break;
		} else if (0 == _count) {
			break;	// socket正常关闭
		} else {
			if (m_bEnabled && m_pFunc) {
				// 回调函数在CMainFrame::OnInitialUpdate()中，是lambda函数
				int count = _count + PCAP_HEAD_SIZE + IPV4_ETH_HEAD_SIZE;
				memset(buffer + PCAP_HEAD_SIZE, 0x0, IPV4_ETH_HEAD_SIZE);	// Mac头全部填0

				// pcap文件格式：每帧前面是16字节的头
				pcaprec_hdr *pcaprec = (pcaprec_hdr *)buffer;
				SYSTEMTIME wtm;
				::GetLocalTime(&wtm);
				time_t clock;
				struct tm tm;
				tm.tm_year = wtm.wYear - 1900;
				tm.tm_mon = wtm.wMonth - 1;
				tm.tm_mday = wtm.wDay;
				tm.tm_hour = wtm.wHour;
				tm.tm_min = wtm.wMinute;
				tm.tm_sec = wtm.wSecond;
				tm.tm_isdst = -1;
				clock = mktime(&tm);
				pcaprec->orig_len = _count + IPV4_ETH_HEAD_SIZE;
				pcaprec->incl_len = _count + IPV4_ETH_HEAD_SIZE;
				pcaprec->ts_sec = (unsigned int)clock;
				pcaprec->ts_usec = wtm.wMilliseconds * 1000;

				m_pFunc((const char *)buffer, count);
			}
		}
	}
}

void CRawSocket::Start(void)
{
	m_bEnabled = TRUE;

	u_long tmp = 1;
	::ioctlsocket(s, SIO_RCVALL, (u_long*)&tmp);
}

void CRawSocket::Stop(void)
{
	m_bEnabled = FALSE;

	u_long tmp = 0;
	::ioctlsocket(s, SIO_RCVALL, (u_long*)&tmp);
}
