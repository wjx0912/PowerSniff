///////////////////////////////////////
// MyDialog.h

#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H
#include "../control/Hyperlink.h"


// Declaration of the CAboutDialog class
class CAboutDialog : public CDialog
{
public:
	CAboutDialog();
	virtual ~CAboutDialog();

protected:
	virtual void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnOK();

private:
	CHyperlink	m_Hyperlink;
	CEdit		m_info;
};

#endif //MYDIALOG_H
