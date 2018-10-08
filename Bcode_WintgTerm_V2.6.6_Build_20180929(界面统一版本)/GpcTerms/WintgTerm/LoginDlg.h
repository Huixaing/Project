#if !defined(AFX_LOGINDLG_H__47202805_4C66_4F3E_B3FF_AE3F4ACEFCC8__INCLUDED_)
#define AFX_LOGINDLG_H__47202805_4C66_4F3E_B3FF_AE3F4ACEFCC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoginDlg.h : header file
//
#include "resource.h"
#include "icommtask.h"   

#include <vector>

using namespace std;
//#include "tgmsg.h"
/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

class CLoginDlg : public CDialog
{
// Construction
public:
	BOOL SendAuthority();
	unsigned long m_nShiftID;
	void ConvertShiftidToString(unsigned long id);
	void Set_Ddy_Name(unsigned long shift_id,short ddt_id,const char * ddyname);
	unsigned long nGetNextDay();
	unsigned long nGetToday();
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor
	unsigned long  g_banbie;      //save current banbie ,e.g, 2002060701 
      int nFlag;
// Dialog Data
	//{{AFX_DATA(CLoginDlg)
	enum { IDD = IDD_LOGIN2 };
	CEdit	m_username3Ctrl;
	CEdit	m_BanbieCtrl;
	CEdit	m_DateCtrl;
	CEdit	m_PasswordCtrl;
	CEdit	m_UsernameCtrl;
	CEdit	m_UseridCtrl;
	CString	m_consoleid;
	CString	m_userid;
	CString	m_username;
	CString	m_password;
	CString	m_date;
	CString	m_banbie;
	CString	m_username3;
	BOOL	m_checkgenban;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int GetConsoleid();
	CString GetNextDay();
	COLORREF bk_color;
	CString password;
	CBrush m_Brush;

	// Generated message map functions
	//{{AFX_MSG(CLoginDlg)
	afx_msg void OnButtonEnter();
	afx_msg LRESULT OnLoginMsg(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPrevious();
	afx_msg void OnNext();
	afx_msg void OnCheckgenban();
	afx_msg LRESULT  OnMsgGpcAccessReply(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void LoadCertifyFile();
	void LoadCertifyFile3();
	bool CertificateUser(char *username, char *pwd);
	bool CertificateUserVersion1(char *username, char *pwd);
	bool CertificateUserVersion2(char *username, char *pwd, char *szSysID);
	
	//信达环宇指纹识别
	bool CertificateUserVersion3(char *username);
	bool FingerCollection(BYTE* szFingerInfo, int* nFingerInfoLen);
	
	BOOL  SelectEnable;
	int   m_nAuthType;
	CString m_strCtcPwd;

	int m_isLocalGetData ;//1:usb,0:net
	int m_UsbPort ;//usb端口号默认为-1
	void  SendGpcAccessInfo(ushort uAccessStep,const char *userid);
	void  SendLoginInfo(const char *username,const char *passwd); // CTC服务器
	void  LoginOK(); // 登陆成功
	
private:
	struct CertiSvrInfo
	{
		int   nTf;
		DWORD port;
		char  strIp[16];
		CertiSvrInfo()
		{
			nTf=1;
			port = 0;
			memset(strIp, 0, sizeof(strIp));
		}
	};
	CString m_fixToken; // 信达环宇 前缀
	std::vector<CertiSvrInfo> m_vectsvr;
	std::vector<CertiSvrInfo> m_vectsvr2;
	std::vector<CertiSvrInfo> m_vectsvr3;
	int nFlagEnableVer; // 认证版本 0:动态口令牌 1:认证服务器 2:指纹认证
	int  m_nPreSchdReqNum; // 向前登录班次次数
public:
	int  m_nMaxPreNum; // 允许最大向前登录次数
	bool m_bCtrlLoginBanci; // 控制班次登录

private:
		HINSTANCE m_hGiOtpAPIDLL;
		HINSTANCE m_hFpClientDLL;
		HINSTANCE m_hJHDLLCUDLL;
public:
	afx_msg void OnTimer(UINT nIDEvent);

public:
	afx_msg void OnBnClickedCheckLogin();
	CButton m_checkbtn;
    bool m_bIgnoreLoginTm; // 忽略班别重叠----存在实际在下一班,但要登到上一班的情况,比如7,19点
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINDLG_H__47202805_4C66_4F3E_B3FF_AE3F4ACEFCC8__INCLUDED_)
