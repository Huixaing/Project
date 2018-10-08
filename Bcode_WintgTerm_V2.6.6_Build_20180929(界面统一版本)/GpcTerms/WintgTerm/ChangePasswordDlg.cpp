// ChangePasswordDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "wintg.h"
#include "ChangePasswordDlg.h"
#include "messages.h"
#include "function.h"
#include "memshare.h"
#include "tgmsg.h"      /***自定义消息 ：UM_LOGIN_MSG  **/ 
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NO_RIGHT_TO_CHANGE_PASSWORD      0x01
#define CHANGE_PASSWORD_OK               0x02
#define CHANGE_PASSWORD_ERROR            0x03

/////////////////////////////////////////////////////////////////////////////
// CChangePasswordDlg dialog


CChangePasswordDlg::CChangePasswordDlg(CWnd* pParent /*=NULL*/)
: CDialog(CChangePasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangePasswordDlg)
	m_userid = _T("");
	m_username = _T("");
	m_old_password = _T("");
	m_new_password = _T("");
	m_new_password2 = _T("");
	//}}AFX_DATA_INIT
}


void CChangePasswordDlg::DoDataExchange(CDataExchange* pDX)
{

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangePasswordDlg) 
	DDX_Control(pDX, IDC_NEW_PASSWORD2, m_newCtrl2);
	DDX_Control(pDX, IDC_NEW_PASSWORD, m_newCtrl);
	DDX_Control(pDX, IDC_OLD_PASSWORD, m_oldCtrl);
	DDX_Control(pDX, IDC_USERID, m_useridCtrl);
	DDX_Control(pDX, IDC_USERNAME, m_userNameCtrl);
	DDX_Control(pDX, IDC_PROGRESS1, m_prog);
	
	DDX_Text(pDX, IDC_USERID, m_userid);
	DDV_MaxChars(pDX, m_userid, 10);
	DDX_Text(pDX, IDC_USERNAME, m_username);
	DDV_MaxChars(pDX, m_username, 30);
	DDX_Text(pDX, IDC_OLD_PASSWORD, m_old_password);
	DDV_MaxChars(pDX, m_old_password, 10);
	DDX_Text(pDX, IDC_NEW_PASSWORD, m_new_password);
	DDV_MaxChars(pDX, m_new_password, 10);
	DDX_Text(pDX, IDC_NEW_PASSWORD2, m_new_password2);
	DDV_MaxChars(pDX, m_new_password2, 10);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChangePasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CChangePasswordDlg)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_MESSAGE(UM_UNI_MESSAGE, OnMsgGpcUserManager)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangePasswordDlg message handlers

BOOL CChangePasswordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	InitDlgPosition(this);
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgUserManager,0);

	m_userid=gpcdef.GetCurrentUserid();
	m_username=gpcdef.GetCurrentUsername();
	m_strCtcPwd=gpcdef.GetCurrentUserpwd();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CChangePasswordDlg::OnClose()
{
	OnCancel();
}

void CChangePasswordDlg::OnCancel() 
{
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgUserManager);
	CDialog::OnCancel( );
}

void CChangePasswordDlg::OnOk() 
{
	UpdateData(TRUE);
	do
	{
		if(m_userid=="")
		{   
			m_useridCtrl.SetFocus();
			break;
		}
		if(m_userid!="" && m_old_password=="")
		{
			m_oldCtrl.SetFocus();
			break;
		}

		if(m_strCtcPwd!=m_old_password)
		{
			AfxMessageBox("输入的旧密码不正确，请重新输入！",MB_OK);
			m_old_password="";
			UpdateData(FALSE);
			m_oldCtrl.SetFocus( );                     
			break;
		}

		if(m_new_password=="")
		{   
			m_newCtrl.SetFocus();
			break;
		}
		if(m_new_password!="" && m_new_password2=="")
		{   
			m_newCtrl2.SetFocus();
			break;
		}

		if(m_userid !="" && m_old_password !="" && m_new_password !="" && m_new_password2 !="")
		{ 		
			if(m_new_password2!=m_new_password)
			{    
				AfxMessageBox("新的密码输入不一致，请重新输入！",MB_OK);
				m_new_password=m_new_password2="";
				UpdateData(FALSE);
				m_newCtrl.SetFocus( );                     
				break;
			}
			else
			{
				m_prog.SetRange(0,8);
				m_prog.SetPos(0);

				Send_ChangePassword();
			}
		}

	}while(0);	

	return;
}

void CChangePasswordDlg::Send_ChangePassword()
{   
	CLBTH::MsgUserManager  mMsgUserManager;
	mMsgUserManager.count =1;
	strcpy(mMsgUserManager.data[0].user_code,m_userid);
	strcpy(mMsgUserManager.data[0].user_name,m_username);
	strcpy(mMsgUserManager.data[0].user_pwd,m_new_password);
	mMsgUserManager.data[0].job_id = gpcdef.GetUserIdentify();

	mMsgUserManager.data[0].console_count = 0;
	
	mMsgUserManager.oper = CLBTH::MsgUserManager::USERMGR_CHANGE_PROP;

	CLBTH::Archive ar( CLBTH::Archive::store);
	mMsgUserManager.serialize(ar);
	
	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if(server_id>0)
		SendBMsg(&mMsgUserManager,server_id);
	else
		SendBMsg(&mMsgUserManager,0);
	GpcLogTrainMessage(PMTG.ThreadId,gpcdef.GetConsoleid(),"[PMTG] User change Password to:%s",m_new_password);

	mtimes=0;
	bSuccess=false;
	SetTimer(1979, 500, NULL);
}

void CChangePasswordDlg::SendMsgUserManagerQuery()
{
   	CLBTH::MsgUserManagerQuery mMsgUserManagerQuery;
	
	CLBTH::Archive ar( CLBTH::Archive::store);
	mMsgUserManagerQuery.serialize(ar);
	
	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if(server_id>0)
		SendBMsg(&mMsgUserManagerQuery,server_id);
}

LRESULT CChangePasswordDlg::OnMsgGpcUserManager(WPARAM wParam, LPARAM lParam)
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if(pMsgStruct->msg_id==MSGID_MsgUserManager)
	{
		CLBTH::MsgUserManager *pBuf = (CLBTH::MsgUserManager *)pMsgStruct->pmsg;
	
		ushort rec_count = pBuf->count;
		for (int j=0; j<rec_count; j++)
		{
			if(pBuf->data[j].user_code==m_userid && pBuf->data[j].user_name==m_username && pBuf->data[j].user_pwd==m_new_password)
			{
				bSuccess=true;
				break;
			}
		}
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);

	return TRUE;
}

void CChangePasswordDlg::OnTimer(UINT nIDEvent) 
{
	if(1979 == nIDEvent)
	{
		mtimes++;
		m_prog.SetPos(mtimes);
		if(mtimes>=3)
		{
			SendMsgUserManagerQuery();
			KillTimer(1979);
			SetTimer(1983,500, NULL);
		}
	}
	if(1983 == nIDEvent)
	{
		mtimes++;
		m_prog.SetPos(mtimes);
		if(mtimes>7)
		{
			m_prog.SetPos(0);
			KillTimer(1983);
			if(bSuccess)
			{
				gpcdef.SetCurrentUserpwd(m_new_password);
				gpcdef.SetUserPassword(m_new_password.GetBuffer());
				m_strCtcPwd=gpcdef.GetCurrentUserpwd();
				MessageBox("密码修改成功!","提示", MB_OK|MB_ICONINFORMATION);
			}
			else
			{
				MessageBox("密码修改失败!","警告", MB_OK|MB_ICONERROR);
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}
