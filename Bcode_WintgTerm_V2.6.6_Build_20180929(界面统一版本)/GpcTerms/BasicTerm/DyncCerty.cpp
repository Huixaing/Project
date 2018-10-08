// DyncCerty.cpp : implementation file
//

#include "stdafx.h"
#include "DyncCerty.h"
#include "DynCertificate.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDyncCerty dialog


CDyncCerty::CDyncCerty(CWnd* pParent /*=NULL*/)
	: CDialog(CDyncCerty::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDyncCerty)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDyncCerty::~CDyncCerty()
{

}

void CDyncCerty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDyncCerty)
	DDX_Control(pDX, IDC_DC_PWD, m_pwd);
	DDX_Control(pDX, IDC_DC_USER, m_userid);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDyncCerty, CDialog)
	//{{AFX_MSG_MAP(CDyncCerty)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDyncCerty message handlers

void CDyncCerty::OnCancel() 
{
	isOk=false;
	CDialog::OnCancel();
}
void CDyncCerty::OnOk() 
{
	m_userid.GetWindowText(m_strUser);
	m_pwd.GetWindowText(m_strPwd);
	
	CDynCertificate dynCert;
	if(!dynCert.DynInit())
	{
		AfxMessageBox("动态口令认证服务初始化失败");
		exit(0);
	}
	int rc = dynCert.DynVerify((char*)(LPCTSTR)m_strUser,(char*)(LPCTSTR)m_strPwd);
	dynCert.DynUninit();
	isOk=false;
	if(rc==AUTH_SUCCESS||rc == AUTH_ERR_NET){
		if(rc == AUTH_ERR_NET){
			AfxMessageBox("连接口令认证服务器出错，现在采用本地认证,请速联系管理员！！");
		}
		isOk=true;
	}else if(rc == AUTH_USER_NOTEXIST){
		AfxMessageBox("认证服务器上不存在本用户，请重试。");
	}
	else
	{
		AfxMessageBox("口令认证失败，请联系管理员");
	}
	if(!isOk){
		m_userid.SetWindowText("");
		m_pwd.SetWindowText("");
		m_userid.SetFocus();
		return;
	}else{
		/*	KillTimer(13);*/
		CDialog::OnOK();
	}
	
}

BOOL CDyncCerty::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	// TODO: Add your specialized code here and/or call the base class
	CWnd* pWndFocus = GetFocus();
	
	if ((pMsg->message == WM_KEYDOWN) &&
		((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE)))
	{
		// Special case: if control with focus is an edit control with
		// ES_WANTRETURN style, let it handle the Return key.
		
		TCHAR szClass[10];
		if ((pMsg->wParam == VK_RETURN) &&((pWndFocus = GetFocus()) != NULL) &&IsChild(pWndFocus))
		{
			CWnd *pWnd;
			if(pWndFocus == GetDlgItem(IDC_DC_USER))
			{
				pWnd = GetDlgItem(IDC_DC_PWD); 
				GotoDlgCtrl(pWnd); 
			} else if(pWndFocus == GetDlgItem(IDC_DC_PWD)){
				pWnd = GetDlgItem(IDOK); 
				GotoDlgCtrl(pWnd); 
				OnOk();
			}
		}
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
	
}
