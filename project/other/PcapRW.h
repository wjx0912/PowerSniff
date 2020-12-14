#ifndef PCAP_RW_20150623_H
#define PCAP_RW_20150623_H
#pragma warning (disable : 4819)
#pragma warning (disable : 4091)
#include "wxx_file.h"

typedef struct pcap_hdr_s {
	unsigned int	magic_number;		/* magic number */
	unsigned short	version_major;		/* major version number */
	unsigned short	version_minor;		/* minor version number */
	int		thiszone;		/* GMT to local correction */
	unsigned int	sigfigs;		/* accuracy of timestamps */
	unsigned int	snaplen;		/* max length of captured packets, in octets */
	unsigned int	network;		/* data link type */
} pcap_hdr;

typedef struct pcaprec_hdr_s {
	unsigned int	ts_sec;			/* timestamp seconds */
	unsigned int	ts_usec;		/* timestamp microseconds */
	unsigned int	incl_len;		/* number of octets of packet saved in file */
	unsigned int	orig_len;		/* actual length of packet */
} pcaprec_hdr;

// ²Î¿¼ÁË: https://github.com/DiabloHorn/csniff
class CPcapWrite
{
public:
	CPcapWrite();
	int OpenFile(LPCTSTR fullpath);
	void WritePcapHead();
	void WritePcapData(unsigned char *data, int len);
	void Sync();

private:
	TCHAR		filename[512];
	CFile		m_file;
};

class CPcapRead
{
public:
	CPcapRead();
	int OpenFile(LPCTSTR fullpath);
	BOOL ReadPcapData(unsigned char *data, int *len);

private:
	TCHAR		filename[512];
	CFile		m_file;
};

#endif  	// MYSTATUSBAR_H
