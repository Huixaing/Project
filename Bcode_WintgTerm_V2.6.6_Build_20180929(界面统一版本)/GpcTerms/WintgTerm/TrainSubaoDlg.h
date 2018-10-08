#pragma once
#include "tgmsg.h"                  //消息和数据结构定义
#include "resource.h"
#include "messages.h"
#include "function.h"
#include "memshare.h"
#include <baseMsg/gpc_msgid.h>
#include ".\ext_src\CJFlatComboBox.h"  //office 风格的COMBOX
#include <BaseMsg/clbth.h>
#include "utility/locoinput.h"
class CTgDocument;
class CSuBaoDlg : public CDialog
{
public:
	CSuBaoDlg(CWnd* pParent, ULONG train_index, CTgDocument* pDoc, const char* trainid, long space);
	
// Construction
public:
	CEdit m_driver_name;
	CEdit m_driver_leader_name;
	CEdit m_train_length;
	CEdit m_train_weight;
	CEdit m_train_car_num;
	CEdit m_train_report;
	CListCtrl	m_List;
	
	CComboBox m_tpye;
	CComboBox m_enginetype;
	CComboBox m_spacelevel;
	CEdit m_train_engineid;
	CString m_strtrain_engineid;
	
// Dialog Data
	//{{AFX_DATA(CSuBaoDlg)
	enum { IDD = IDD_DLG_TRAIN_SUBAO };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSuBaoDlg)
	public:
	virtual void OnOK();
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSuBaoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
    afx_msg void OnDelete();
    afx_msg void OnChange();
	//}}AFX_MSG
   
	void InitIdentify();
	void ClearEdit();

	BOOL SendSubaoMsg(ushort oper);
	BOOL ShowSubao();
	BOOL QuerySubao();
	bool GetSubaoInfo();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam);
	void  RecvSubao(CLBTH::MsgSubaoFormat *pBuf);
	void fillLeftContent(int curSel);

	DECLARE_MESSAGE_MAP()
private:
	CLBTH::MsgSubaoFormat m_suBao;
	
	WORD m_nSectionID;
	DWORD m_nTrainIndex;
	BOOL m_bReadOnly;
	CTgDocument* m_pDoc;
	int schd_type;
	int m_sel;
	int m_prefNum;
	CString m_nTrainId;
	long m_space;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
};