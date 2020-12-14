#include "stdafx.h"
#include "PcapRW.h"

// ==========================================================================
CPcapWrite::CPcapWrite()
{
	// 空函数
}

int CPcapWrite::OpenFile(LPCTSTR fullpath)
{
	try {
		m_file.Open(fullpath, CREATE_ALWAYS);
	} catch (...) {
		return -1;
	}

	return 0;
}

void CPcapWrite::WritePcapHead()
{
	pcap_hdr head;
	head.magic_number = 0xa1b2c3d4;
	head.network = 1;	//ethernet
	head.sigfigs = 0;
	head.snaplen = 65535;
	head.thiszone = 0;
	head.version_major = 2;
	head.version_minor = 4;
	m_file.Write(&head, sizeof(head));
	//XTRACE("write head: \n");
	//printbin((void *)&head, sizeof(struct pcap_hdr_s));
}

void CPcapWrite::WritePcapData(unsigned char *data, int len)
{
	// 16字节pcap帧头加数据
	m_file.Write(data, len);
	//XTRACE("write data: \n");
	//printbin(data, len);
}

void CPcapWrite::Sync()
{
	m_file.Flush();
}

// ==========================================================================
CPcapRead::CPcapRead()
{
	// 空函数
}

int CPcapRead::OpenFile(LPCTSTR fullpath)
{
	try {
		m_file.Open(fullpath, OPEN_EXISTING);
	} catch (...) {
		return -1;
	}

	pcap_hdr head;
	if (sizeof(head) != m_file.Read(&head, sizeof(head)))
		return -2;

	if (0xa1b2c3d4 != head.magic_number || 1 != head.network)
		return -3;

	return 0;
}

// 输出：16字节pcap帧头加数据
BOOL CPcapRead::ReadPcapData(unsigned char *data, int *len)
{
	pcaprec_hdr_s  *pcaprec_head = (pcaprec_hdr_s  *)data;
	if (sizeof(struct pcaprec_hdr_s) != m_file.Read(pcaprec_head, sizeof(struct pcaprec_hdr_s)))
		return FALSE;

	//if (pcaprec_head->incl_len != pcaprec_head->orig_len)
	//	assert(FALSE);

	if (pcaprec_head->incl_len <= 0 || pcaprec_head->incl_len >= 1536)
		return FALSE;

	if (pcaprec_head->incl_len != m_file.Read(data + sizeof(struct pcaprec_hdr_s), pcaprec_head->incl_len))
		return FALSE;

	*len = sizeof(struct pcaprec_hdr_s) + pcaprec_head->incl_len;
	return TRUE;
}
