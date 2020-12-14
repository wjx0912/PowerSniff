///////////////////////////////////////
// MyDialog.h

#ifndef MYDIALOG_H
#define MYDIALOG_H

// Declaration of the CConfigDialog class
class CConfigDialog : public CDialog
{
public:
	CConfigDialog();
	virtual ~CConfigDialog();

protected:
	virtual void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();
	int AddItem(LPCTSTR szText);
	BOOL SetSubItem(int nItem, int nSubItem, LPCTSTR szText);

public:
	int		m_nUsedAdapterMask;	// OnOK后返回的结果

private:
	// 驱动类型
	CButton		m_radio_driver_rawsocket;
	CButton		m_radio_driver_winpcap;

	// 协议族
	CButton		m_radio_all;
	CButton		m_radio_tcp;
	CButton		m_radio_udp;
	CButton		m_radio_icmp;

	// ip地址
	CButton		m_radio_ip_all;
	CButton		m_radio_ip_source;
	CButton		m_radio_ip_dest;
	CButton		m_radio_ip_both;
	CIPAddress	m_ipaddr;

	// 端口
	CButton		m_radio_port_all;
	CButton		m_radio_port_source;
	CButton		m_radio_port_dest;
	CButton		m_radio_port_both;
	CEdit		m_filter_port;

	// 检查有效数据长度是否大于0
	CButton		m_check_valid_data_length;

	// 适配器列表
	CListView	m_adapterlist;
};

#endif //MYDIALOG_H
