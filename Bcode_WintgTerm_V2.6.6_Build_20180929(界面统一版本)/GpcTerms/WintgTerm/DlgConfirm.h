#pragma once
#include "resource.h"

class CDlgConfirm : public CDialog
{
public:
	CDlgConfirm(const CString& tipText,const CStringArray& stringArray, CWnd* pParent = NULL);
	~CDlgConfirm();

	enum { IDD = IDD_DIALOG_CONFIRM };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

	CListBox m_listBox;
public:
	CString m_tipText;
	CStringArray m_stringArray;
};
