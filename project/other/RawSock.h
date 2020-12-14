#ifndef RAW_SOCK_H
#define RAW_SOCK_H

#include <cstring>
#include <cmath>
#include <conio.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <functional>
#include <thread>

// pcap文件格式：https://www.cnblogs.com/2017Crown/p/7162303.html
#define PCAP_HEAD_SIZE		16		// 手动构造：pcap文件格式中每帧的头
#define IPV4_ETH_HEAD_SIZE	14		// 手动构造：mac头14字节

// ===============================================================
//Mac头部，总长度14字节
typedef struct _eth_hdr
{
	unsigned char	dstmac[6];	//目标mac地址
	unsigned char	srcmac[6];	//源mac地址
	unsigned short	eth_type;	//以太网类型
}eth_hdr;

//IP头部，总长度20字节
typedef struct _ip_hdr
{
	unsigned char	version;	//版本
	unsigned char	tos;		//服务类型
	unsigned short	tot_len;	//总长度
	unsigned short	id;		//标志
	unsigned short	frag_off;	//分片偏移
	unsigned char	ttl;		//生存时间
	unsigned char	protocol;	//协议
	unsigned short	chk_sum;	//检验和
	struct in_addr	srcaddr;	//源IP地址
	struct in_addr	dstaddr;	//目的IP地址
}ip_hdr;

//TCP头部，总长度20字节
typedef struct _tcp_hdr
{
	unsigned short	src_port;	//源端口号
	unsigned short	dst_port;	//目的端口号
	unsigned int	seq_no;		//序列号
	unsigned int	ack_no;		//确认号

	unsigned char	reserved_1:4;	//保留6位中的4位首部长度
	unsigned char	thl:4;		//tcp头部长度
	unsigned char	flag:6;		//6位标志
	unsigned char	reseverd_2:2;	//保留6位中的2位

	unsigned short	wnd_size;	//16位窗口大小
	unsigned short	chk_sum;	//16位TCP检验和
	unsigned short	urgt_p;		//16为紧急指针
}tcp_hdr;

//UDP头部，总长度8字节
typedef struct _udp_hdr
{
	unsigned short	src_port;	//远端口号
	unsigned short	dst_port;	//目的端口号
	unsigned short	uhl;		//udp头部长度
	unsigned short	chk_sum;	//16位udp检验和
}udp_hdr;

//ICMP头部，总长度4字节
typedef struct _icmp_hdr
{
	unsigned char	icmp_type;	//类型
	unsigned char	code;		//代码
	unsigned short	chk_sum;	//16位检验和
}icmp_hdr; 

// ===============================================================
#define PROT_ICMP		1	// Internet Control Message [RFC792] 
#define PROT_TCP		6	// Transmission Control     [RFC793]
#define PROT_UDP		17	// User Datagram            [RFC768][JBP]

// ===============================================================
#define MAX_PACKET_NUMBER	(10000 * 1000)		// 最大包数量，过了就循环缓冲
#define MAX_PACKET_SIZE		0x40000
#define MAX_ADAPTER_NUMBER	128

// ===============================================================

#define SIO_RCVALL		(IOC_IN | IOC_VENDOR | 1)

#define ULONG_BYTE4(u)		((u & 0xFF000000) >> 24)
#define ULONG_BYTE3(u)		((u & 0xFF0000) >> 16)
#define ULONG_BYTE2(u)		((u & 0xFF00) >> 8)
#define ULONG_BYTE1(u)		(u & 0xFF)

#define BYTE_L(u)		(u & 0xF)
#define BYTE_H(u)		(u >> 4)

class CMainFrame;

// 有几个网卡，就有几个CRawSocket对象
class CRawSocket
{
public:
	// 静态接口
	static int EnumAdapters();
	static TCHAR adapter_ip[MAX_ADAPTER_NUMBER][32];		// 网卡IP地址
	static TCHAR adapter_des[MAX_ADAPTER_NUMBER][1024];		// 网卡描述字符串
	static int adapter_number;					// 网卡个数

public:
	// 每个网卡对象的实现
	CRawSocket()
	{
		m_nAdapterID = -1;
		m_bEnabled = FALSE;

		s = INVALID_SOCKET;
		sa = { 0 };
		buffer = (BYTE *)malloc(MAX_PACKET_SIZE);
		ZeroMemory(buffer, MAX_PACKET_SIZE);

		m_pThread = NULL;
		m_pFunc = nullptr;
	};
	~CRawSocket()
	{
		::closesocket(s);
		s = INVALID_SOCKET;
		free(buffer);
		buffer = NULL;
	};

	// Init和UnInit只在初始化和卸载的时候调用一次；Start和Stop可以随时调用
	void Init(int adapter_id, CMainFrame *pFrame, std::function<int(const char *buffer, int len)> pFunc);
	void UnInit();
	void Start(void);
	void Stop(void);
	void* GetSniffData(int *len);				// 等待一帧数据（阻塞）

public:
	int			m_nAdapterID;			// 网卡ID
	BOOL			m_bEnabled;			// 是否使能抓包

private:
	void __CaptureThread();

private:
	SOCKET			s;				// socket句柄
	SOCKADDR_IN		sa;				// 地址
	BYTE			*buffer;			// 一帧数据（避免频繁分配内存）

	std::thread		*m_pThread;
	std::function<int(const char *buffer, int len)>		m_pFunc;
};
#endif
