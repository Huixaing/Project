#if !defined(AFX_USERLISTDLG_H__E55DBA43_5427_4B2F_A4BC_1B53DB0BD972__INCLUDED_)
#define AFX_USERLISTDLG_H__E55DBA43_5427_4B2F_A4BC_1B53DB0BD972__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserListDlg.h : header file
//
#include <vector>
#include "resource.h"
#include "tgmsg.h"

#define		USER_TYPE_COUNT	18

struct USER_DEFINE 
{
	char user_id[20]; 
	char user_name[20];
	char user_pass[20];
	BYTE user_property;
	//bool valid;
	BYTE nconsole_count; 
	BYTE ncon_list[20];
};

const char USER_TYPE[USER_TYPE_COUNT][16]={"计划员", "调度员","操作员","调度长","背投","培训","服务","电务维修","绘图","机调员","货调台","统计","AFFAIR_SERVER","SUPER_DISPATCH","OTHER_DISPATCH","系统维护","系统调试","综合维修调度"};

/////////////////////////////////////////////////////////////////////////////
// CUserListDlg dialog
class CUserListDlg : public CDialog
{
// Construction
public:
	CUserListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserListDlg)
	enum { IDD = IDD_USER_LIST };
	CComboBox	m_user_type_list;
	CButton	m_btn_modify;
	CButton	m_btn_del;
	CButton	m_btn_add;
	CListCtrl	m_user_list;
	int		m_cur_user_type;
	//}}AFX_DATA
	std::vector<USER_DEFINE> user;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserListDlg)
	afx_msg LRESULT OnUserListData(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDelUser();
	afx_msg void OnAddUser();
	afx_msg void OnModifyUser();
	afx_msg void OnSelchangeComboUserType();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListUser(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int  GetUser(char *user_code);
	void SendMsgUserManagerQuery();
	void ShowUserList();

protected:
	virtual void PostNcDestroy();
	afx_msg void OnClose();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERLISTDLG_H__E55DBA43_5427_4B2F_A4BC_1B53DB0BD972__INCLUDED_)
