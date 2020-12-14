///////////////////////////////////////
// MyDialog.h

#ifndef TCC_COMPILE_20150709_H
#define TCC_COMPILE_20150709_H
#include "../scintilla/Scintilla.h"
#include "../scintilla/SciLexer.h"

// Declaration of the CCompileDialogTCC class
class CCompileDialogTCC : public CDialog
{
public:
	CCompileDialogTCC();
	virtual ~CCompileDialogTCC();

protected:
	virtual void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();
	sptr_t SendEditor(unsigned int iMessage, uptr_t wParam = 0, sptr_t lParam = 0);

private:
	void OnOpen1();
	void OnSave1();
	void OnSaveAs1();
	void OnCompile1();
	void OnRun();

private:
	HWND			m_hwnd_edit;
	SciFnDirect		m_fnDirect;
	sptr_t			m_ptrDirect;

private:
	CResizer		m_Resizer;
	CString			m_current_filename;

public:
	CEdit			m_result;
};

#endif
