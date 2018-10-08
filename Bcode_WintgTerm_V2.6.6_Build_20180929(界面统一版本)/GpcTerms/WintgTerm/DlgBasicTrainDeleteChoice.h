
#pragma once

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgBasicTrainDeleteChoice dialog

class CDlgBasicTrainDeleteChoice : public CDialog
{
// Construction
public:
	CDlgBasicTrainDeleteChoice(int nChoice, CWnd* pParent);   // standard constructor
	~CDlgBasicTrainDeleteChoice();

	enum { IDD = IDD_DLG_BASIC_DELETE_CHOICE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

protected:
	CButton btn_all, btn_pess, btn_freg;	
	void SetButton();
	
public:
	afx_msg void OnClose();


	int m_nChoice; // 1:all 2:pessager 3:freg
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
};

