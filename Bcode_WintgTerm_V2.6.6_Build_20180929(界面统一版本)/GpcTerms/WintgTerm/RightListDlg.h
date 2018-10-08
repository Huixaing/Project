#pragma once
// CRightListDlg.h : header file
//
#include <vector>
#include "resource.h"
#include "tgmsg.h"

/////////////////////////////////////////////////////////////////////////////
// CRightListDlg dialog
struct RIGHT_DEFINE 
{
	int  con_id;
	char con_name[50];
	int  entity_id;
	char entity_name[32];
    int role_right;
	char role_param[2048]; 
	RIGHT_DEFINE()
	{
		con_id=0;
		entity_id=0;
		role_right=0;
		memset(con_name,0,sizeof(con_name));
		memset(entity_name,0,sizeof(entity_name));
		memset(role_param,0,sizeof(role_param));
	}
};

class CRightListDlg : public CDialog
{
	// Construction
public:
	CRightListDlg(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CRightListDlg)
	enum { IDD = IDD_RIGHT_LIST };

	CListCtrl m_right_list;
	CButton m_btn_add;
	CButton m_btn_update;
	CButton m_btn_delete;
	CButton m_btn_close;

	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRightListDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRightListDlg)
	afx_msg LRESULT OnRightListData(WPARAM wParam,LPARAM lParam);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	std::vector<RIGHT_DEFINE> mVectRightList;

private:
	int m_huge_count;
	
public:
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedUpdateRight();
	afx_msg void OnBnClickedDeleteRight();
	afx_msg void OnBnClickedAddRight();

	void ShowRightList();
	void SendMsgRightListQuery();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
