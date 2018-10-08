#pragma once
#include "afxwin.h"
#include "resource.h"

class CTgDocument;
class CDlgBasicDeleteName :
	public CDialog
{
public:
	CDlgBasicDeleteName(int conid, CTgDocument *pDoc, CWnd* pParent);
	~CDlgBasicDeleteName(void);

	// Dialog Data
	//{{AFX_DATA(CDlgBasicDeleteName)
	enum { IDD = IDD_DLG_BASIC_DELETENAME };
	//}}AFX_DATA

private:
	int m_nConid;
	CTgDocument *m_pDoc;
	CWnd* m_pParant;
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	CComboBox m_combo_basicname;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnDelete();
protected:
	LRESULT OnCommMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	void UpdateBakcupBasicName();
	afx_msg void OnBnClickedBtnClose();
};