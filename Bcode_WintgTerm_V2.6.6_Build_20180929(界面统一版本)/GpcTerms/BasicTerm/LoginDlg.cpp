// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include <io.h>
#include "LoginDlg.h"
#include "messages.h"
#include "function.h"
#include "memshare.h"
#include "tgmsg.h"      /***自定义消息 ：UM_LOGIN_MSG  **/ 
#include <BaseMsg/clbth.h>
#include <BaseMsg/gpc_msgid.h>
#include "dynCertificate.h"
#include "icommtask.h"
#include "td/td_frame.h"
#include ".\logindlg.h"
#import  <msxml.dll> named_guids
using namespace MSXML;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const COLORREF g_crGray=RGB(192,192,192);
const COLORREF g_crRed=RGB(255,0,255);
extern void SetCurrentLoginInfo(const char *userid,const char *password,int shiftid); // defined in pubfun.cpp

typedef int ( *pFPIFea_IP)(const char* ipaddrpt, BYTE *psTZ,  int* IpLength,int nTF);
typedef int ( *pFPIFeature_FDR)(int nPortNo, unsigned char *psTZ, int* lpLength);
typedef int ( *pRemoteVerifyFp)(char* addr, const int port, char* userName,unsigned char* fingerInfo);
typedef int (__stdcall *pGiOtp_Auth)(const char* sHost,const int iPort, const char* sAppID, const char* sUserID, const char* sAuthCode,char** sMsg); 

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog
const int timeid = 86418;
CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginDlg)
	m_consoleid = _T("");
	m_userid = _T("");
	m_username = _T("");
	m_password = _T("");
	m_date = _T("");
	m_banbie = _T("");
	m_username3 = _T("");
	m_checkgenban = FALSE;
	//}}AFX_DATA_INIT
	m_nAuthType=0;
	m_fixToken="";
	m_nPreSchdReqNum=0;
	m_bCtrlLoginBanci=false;
	m_nMaxPreNum = 2;
	m_hGiOtpAPIDLL=NULL;
	m_hFpClientDLL=NULL;
	m_hJHDLLCUDLL=NULL;
	m_isLocalGetData = 0 ;
	m_UsbPort = -1 ;
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Control(pDX, IDC_USERNAME32, m_username3Ctrl);
	DDX_Control(pDX, IDC_BANBIE, m_BanbieCtrl);
	DDX_Control(pDX, IDC_DATE, m_DateCtrl);
	DDX_Control(pDX, IDC_PASSWORD, m_PasswordCtrl);
	DDX_Control(pDX, IDC_USERNAME, m_UsernameCtrl);
	DDX_Control(pDX, IDC_USERID, m_UseridCtrl);
	DDX_Text(pDX, IDC_CONSOLEID, m_consoleid);
	DDX_Text(pDX, IDC_USERID, m_userid);
	DDV_MaxChars(pDX, m_userid, 10);
	DDX_Text(pDX, IDC_USERNAME, m_username);
	DDX_Text(pDX, IDC_PASSWORD, m_password);
	DDV_MaxChars(pDX, m_password, 10);
	DDX_Text(pDX, IDC_DATE, m_date);
	DDX_Text(pDX, IDC_BANBIE, m_banbie);	
	DDX_Text(pDX, IDC_USERNAME32, m_username3);	
	DDV_MaxChars(pDX, m_username3, 10);
	DDX_Check(pDX, IDC_CHECK_GENBAN2, m_checkgenban);
	DDX_Control(pDX, IDC_CHECK_AUTOREFRESH, m_checkbtn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_PREVIOUS, OnPrevious)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_CHECK_GENBAN2, OnCheckgenban)
	ON_BN_CLICKED(IDOK, OnButtonEnter)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP	
	ON_BN_CLICKED(IDC_CHECK_AUTOREFRESH, OnBnClickedCheckLogin)
	ON_MESSAGE(UM_UNI_MESSAGE, OnMsgGpcAccessReply)
	ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers
void LoginWindowSetAutoReloginEnabled(bool flag);
void CLoginDlg::OnButtonEnter() 
{   	
	int theStatus = 1;
	UpdateData(TRUE);

	if(m_password.GetLength()>0)
	{
		// 超级密码
		if((m_userid=="casco") && (m_password=="casco" || m_password=="CASCO"))
		{
			m_username="CASCO";
			GpcLogTrainMessage(0,0,"[LOGIN]:使用 CASCO");
			LoginOK();
			return;
		}


		if(m_nAuthType == 0) // CTC服务器确认密码
		{
			SendLoginInfo(m_userid,m_strCtcPwd);
			GpcLogTrainMessage(0,0,"[LOGIN]:确认CTC服务器认证 %s %s", m_userid, m_strCtcPwd);
		}
		else if(m_nAuthType == 1) // 按照内部验证不成功,信达环宇验证
		{
			if(m_strCtcPwd == m_password) // 服务器返回密码==人工输入密码
			{
				GpcLogTrainMessage(0,0,"[LOGIN]:使用 CTC密码 验证通过,进入系统");
				LoginOK();	//验证通过，进入系统
			}
			else
			{
				bool m_bcfvsr_status=CertificateUser(m_userid.GetBuffer(),m_password.GetBuffer());  // 发送给身份认证服务器
				if(m_bcfvsr_status)
				{
					GpcLogTrainMessage(0,0,"[LOGIN]:使用身份认证服务器验证通过,进入系统");
					LoginOK();	//验证通过，进入系统
				}
			}
		}
		else if(m_nAuthType == 2) // 完全按照内部验证
		{
			SendLoginInfo(m_userid,m_password);
			GpcLogTrainMessage(0,0,"[LOGIN]:完全由CTC服务器认证 %s %s", m_userid, m_password);
		}
		else if(m_nAuthType == 3) // 完全信达环宇验证
		{
			bool m_bcfvsr_status=CertificateUser(m_userid.GetBuffer(),m_password.GetBuffer());  // 发送给身份认证服务器
			if(m_bcfvsr_status)
			{
				SendLoginInfo(m_userid,m_strCtcPwd);
				GpcLogTrainMessage(0,0,"[LOGIN]:完全由信达环宇服务器认证 %s %s %s", m_userid, m_strCtcPwd, m_password);
			}
		}
		else
		{
			CString text;
			text.Format("请注意, 本终端用户在gpc.def中[USER]中登陆认证项配置错误, 合法值0,1,2,3");
			AfxMessageBox(text);
			GpcLogTrainMessage(0,0,"%s", text);
		}
	}
	else
	{
		if(m_bIgnoreLoginTm)
		{
			CString text="请注意,你忽略班次重叠登陆,你确实要这么做吗?\n如果不是请不要选择\"忽略班别(6点-8点和18点-20点)重叠\"";
			if(MessageBox(text,"提示",MB_YESNO|MB_ICONQUESTION)==IDNO)
			{
				return;
			}
		}

		m_userid.TrimLeft();
		if(m_userid.GetLength() == 0)
		{    
			AfxMessageBox("请输入代码！",MB_OK);
			m_UseridCtrl.SetFocus( );                     
		}
		else
		{
			SendGpcAccessInfo(CLBTH::MsgGpcAccessInfo::WANT_PWD,m_userid);

			if(!strcmp(m_userid,"casco"))
			{
				m_PasswordCtrl.EnableWindow(TRUE); 
                m_PasswordCtrl.SetFocus();
			}
			// 如下代码测试使用
			/*CLBTH::MsgGpcAccessReply reply;
			reply.status=CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_OK;
			strcpy(reply.pwd, "12345");
			strcpy(reply.user_name, "乔永涛");

			UNI_MESSAGE_STRUCT  lParam;
			lParam.source_id=10001;
			lParam.msg_id=0;
			lParam.pmsg=&reply;
			OnMsgGpcAccessReply(0, (LPARAM)&lParam);*/
		}
	}
}

BOOL CLoginDlg::OnInitDialog() 
{   
	CDialog::OnInitDialog();
	CoInitialize(NULL);
	CTime t=CTime::GetCurrentTime(); 
	m_nShiftID=Get_CurrentShiftid();

	CString DispatchStationName;          
	DispatchStationName.Format ("%s",gpcdef.GetCurUserConsoleName());
	m_consoleid=DispatchStationName; 
	UpdateData(FALSE);   

	m_UseridCtrl.EnableWindow(TRUE);
	m_UsernameCtrl.EnableWindow(FALSE);
	m_PasswordCtrl.EnableWindow (FALSE);

	m_UseridCtrl.SetFocus();

	ConvertShiftidToString(Get_CurrentShiftid());
	UpdateData(FALSE);

	//判断当前是否有权限登陆，调度员权限b只能在交班三小时以内登陆
	//如果超出了三小时，只能登陆当前班，左右两边移动按钮变灰
	//在三小时以内，左右两边只能一个按钮起作用
	CTime curt = CTime::GetCurrentTime();

	time_t tbegin,tend;

	if(curt.GetHour()>=6 && curt.GetHour()<18)//第二班
	{
		CTime shiftt = CTime(curt.GetYear(),curt.GetMonth(),curt.GetDay(),6,0,0);
		tbegin = shiftt.GetTime();
		tend = tbegin + 3*3600;

	}
	else if(curt.GetHour()>=18)//第一班和当前日期的18点比
	{
		CTime shiftt = CTime(curt.GetYear(),curt.GetMonth(),curt.GetDay(),18,0,0);
		tbegin = shiftt.GetTime();
		tend = tbegin + 3*3600;
	}
	else//第一班，和前一天的18点比，肯定已经超时了
	{
		tbegin = 0;
		tend = 0;
	}

	m_bIgnoreLoginTm=false;
	m_checkbtn.SetCheck(FALSE);
	m_checkbtn.EnableWindow(FALSE);
	m_checkbtn.ShowWindow(SW_HIDE);
		
	//	InitDlgPosition(this);
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgGpcAccessReply,0);
	
	SetLoginWindowOpenFlag(true); // 禁止自动重发第二阶段登录

	// 加载各种认证dll
	CString szPrompt;
	m_hGiOtpAPIDLL = LoadLibrary( "GiAgentX.dll" );
	if (m_hGiOtpAPIDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:信达环宇服务器认证模块GiAgentX.dll 不存在");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
	}

	m_hFpClientDLL = LoadLibrary( "FpClient.dll" );
	if (m_hFpClientDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:信达环宇指纹验证模块FpClient.dll 不存在");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
	}

	m_hJHDLLCUDLL = LoadLibrary( "jhdllcu.dll" );
	if (m_hJHDLLCUDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:信达环宇指纹采集模块jhdllcu.dll 不存在");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
	}
	m_nAuthType = gpcdef.GetLoginType();
	nFlagEnableVer=0;
	LoadCertifyFile();
	LoadCertifyFile3();

	GpcLogTrainMessage(0,0,"认证版本:%d(0:动态口令牌 1:认证服务器 2:指纹认证)" , nFlagEnableVer);
	
	return TRUE;   
}

HBRUSH CLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(m_DateCtrl.m_hWnd==pWnd->m_hWnd || m_BanbieCtrl.m_hWnd==pWnd->m_hWnd )
	{     
		pDC->SetBkColor(bk_color);
		return (HBRUSH) m_Brush;
	}  

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}


void CLoginDlg::OnPrevious() 
{   
	UpdateData(TRUE);
	m_nShiftID=Get_PreShiftid(m_nShiftID);
	ConvertShiftidToString(m_nShiftID);

	m_nPreSchdReqNum++;
	if(m_nPreSchdReqNum>=m_nMaxPreNum && m_bCtrlLoginBanci)
	{
		CWnd* pWnd = GetDlgItem(IDC_PREVIOUS);
		if(pWnd != NULL)
			pWnd->EnableWindow(FALSE);
	}
}

void CLoginDlg::OnNext() 
{   
	UpdateData(TRUE);
	m_nShiftID=Get_NextShiftid(m_nShiftID);
	ConvertShiftidToString(m_nShiftID);

	m_nPreSchdReqNum--;
	if(m_nPreSchdReqNum<m_nMaxPreNum && m_bCtrlLoginBanci)
	{
		CWnd* pWnd = GetDlgItem(IDC_PREVIOUS);
		if(pWnd != NULL)
			pWnd->EnableWindow(TRUE);
	}
}

CString CLoginDlg::GetNextDay()
{ 
	int  CurYear,CurMonth,CurDay;
	CString NextDay;

	CTime t=CTime::GetCurrentTime();
	CurYear=t.GetYear();
	CurMonth=t.GetMonth();
	CurDay=t.GetDay();
	ASSERT(CurDay>=1 && CurDay<=31);
	switch(CurDay)
	{
	case 28:
		if( (CurYear %4==0 && CurYear %100!=0)||(CurYear %400==0) )
			NextDay.Format ("%4d年%d月%d日",CurYear,CurMonth,29);
		else
			NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth+1,  1);
		break;

	case 29:
		if(CurMonth==2)
			NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth+1,1);
		else
			NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth,30);
		break;

	case 30:
		if(CurMonth==1 || CurMonth==3 || CurMonth==5 || CurMonth==7 || CurMonth==8
			|| CurMonth==10 ||CurMonth==12)
			NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth,31);
		else
			NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth+1,1);
		break;

	case 31:
		if(CurMonth==12)
			NextDay.Format ("%4d年 %d月%d日",CurYear+1,1,1);
		else
			NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth+1,1);
		break;

	default:
		NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth,CurDay+1);
		break;

	}

	return NextDay;
}

int CLoginDlg::GetConsoleid()
{
	CString DispatchStationName;  // 调度台名称
	int consoleid;

	DispatchStationName.Format ("%s",gpcdef.GetCurUserName ());
	ASSERT( DispatchStationName.Remove('g')==1 );
	ASSERT( DispatchStationName.Remove('p')==1 );
	ASSERT( DispatchStationName.Remove('c')==1) ;

	consoleid=atoi(DispatchStationName);
	ASSERT(consoleid>=0);
	return consoleid;
}

void CLoginDlg::Set_Ddy_Name(unsigned long shift_id, short ddt_id,const char *ddyname)
{
	CLBTH::MsgSetDDyName  msg;
	msg.conid=ddt_id;
	msg.shift_id=shift_id;
	strncpy(msg.user_name,ddyname,sizeof(msg.user_name)-1);

	long  server_id=GetSpecialEntityID(SERVER_TYPE_DB,ddt_id);
	GpcLogTrainMessage(0,0,"[LOGIN]:send entity:  %d  conid:  %d shift_id:  %d 登陆调度员名字: %s",  
		server_id,  msg.conid,  msg.shift_id, msg.user_name);
	if(!SendTGBMsg(ddt_id,&msg,server_id))
		SendBMsg(&msg,0);
}


void CLoginDlg::ConvertShiftidToString(unsigned long id)
{
	CString date,banbie;

	int CurYear  = id/1000000;
	int CurMonth = id/10000-CurYear*100;
	int CurDay   = id/100-(id/10000)*100;
	date.Format("%4d年%2d月%2d日",CurYear,CurMonth,CurDay);
	int CurBanbie=id-(id/10)*10;
	banbie=(CurBanbie==1) ? "第一班" : "第二班";

	m_date=date;
	m_banbie=banbie;

	if(CurBanbie==1)
		bk_color=g_crGray;
	else
		bk_color=g_crRed;
	m_Brush.DeleteObject();
	m_Brush.CreateSolidBrush(bk_color); //color initiate;

	UpdateData(FALSE);
	Invalidate(FALSE);
}

void CLoginDlg::OnCheckgenban() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_checkgenban)
	{
		m_username3Ctrl.EnableWindow(TRUE);
		m_username3Ctrl.SetFocus();
	}
	else
	{
		m_username3Ctrl.EnableWindow(FALSE);
	}
}


LRESULT CLoginDlg::OnMsgGpcAccessReply(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if (MSGID_MsgGpcAccessReply == pMsgStruct->msg_id)
	{
		CLBTH::MsgGpcAccessReply *pServerReply= ( CLBTH::MsgGpcAccessReply *)pMsgStruct->pmsg;

		switch(pServerReply->status)
		{
		case CLBTH::MsgGpcAccessReply::GPC_LOGIN_CHECK_OK:
			{
				GpcLogTrainMessage(0,0,"[LOGIN]:CTC服务器认证通过, 进入系统");
				LoginOK();
			}
			break;
		case CLBTH::MsgGpcAccessReply::GPC_LOGIN_CHECK_FAIL:
			AfxMessageBox("用户名密码校验错误,请重新输入!");
			break;  
		case CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_OK:
			{
				if(nFlagEnableVer == 2) // 信达环宇指纹服务验证
				{
					m_username=pServerReply->user_name;
					m_strCtcPwd = pServerReply->pwd;
					UpdateData(FALSE);
					m_UseridCtrl.EnableWindow(FALSE);
					GpcLogTrainMessage(0,0,"[LOGIN]:CTC服务器返回 %s %s", m_username, m_strCtcPwd);

					SetTimer(timeid,100,NULL);
				}
				else
				{
					m_strCtcPwd = pServerReply->pwd;
					m_username=pServerReply->user_name;
					UpdateData(FALSE);
					m_PasswordCtrl.EnableWindow(TRUE); 
					m_UseridCtrl.EnableWindow(FALSE);
					m_PasswordCtrl.SetFocus();
					GpcLogTrainMessage(0,0,"[LOGIN]:CTC服务器返回 %s %s", m_username, m_strCtcPwd);
				}
			}
			break;
		case CLBTH::MsgGpcAccessReply::GPC_ACCESS_SERVER_READY:
			m_username=pServerReply->user_name;
			UpdateData(FALSE);
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_NOT_EXIST:
			AfxMessageBox("此用户不存在,请重新输入!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_INVALID_GPC_ID:
			AfxMessageBox("用户代码不正确,请重新输入!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_JOBID_NOT_EXIST:
			AfxMessageBox("用户工作权限不存在,请重新输入!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_CONID_NOT_EXIST:
			AfxMessageBox("用户对应的调度台不存在,请重新输入!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_GPCID_ALREADY_ONLINE:
			AfxMessageBox("用户已登陆，请联系管理员!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_CONID_DISMATCH:
			AfxMessageBox("用户不能操作本台,请重新输入!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_JOBID_DISMATCH:
			AfxMessageBox("用户代码和权限不匹配,请重新输入!");
			break;
		case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_UNKNOWN_MESSAGE:
			AfxMessageBox("未知消息,请重新输入!");
			break;		 
		default:
			break;
		}
	}
	return TRUE;
}

void CLoginDlg::OnDestroy() 
{
	KillTimer(timeid);
	if (m_hGiOtpAPIDLL)
		FreeLibrary(m_hGiOtpAPIDLL);
	if (m_hFpClientDLL)
		FreeLibrary(m_hFpClientDLL);
	if (m_hJHDLLCUDLL)
		FreeLibrary(m_hJHDLLCUDLL);
	m_vectsvr.clear();
	m_vectsvr2.clear();
	m_vectsvr3.clear();
	::CoUninitialize();
	CDialog::OnDestroy();
	SetLoginWindowOpenFlag(false); // 允许自动重发第二阶段登录
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgGpcAccessReply);
}

void CLoginDlg::SendLoginInfo(const char *username,const char *passwd) //  get userid from gpcdef
{
	CLBTH::MsgGpcAccessReply mMsgGpcAccessInfo;
	int m_nCurrentSectionList[64];
	int m_nSectionNum  = sizeof(m_nCurrentSectionList) / sizeof(m_nCurrentSectionList[0]);
	gpcdef.GetSectionList(m_nSectionNum,m_nCurrentSectionList);
	int m_nCurrentSectionNo = m_nCurrentSectionList[0];

	strcpy(mMsgGpcAccessInfo.user_code,username);
	strcpy(mMsgGpcAccessInfo.user_name,""); // gpcdef.GetCurrentUserid());
	strcpy(mMsgGpcAccessInfo.pwd,passwd);
	mMsgGpcAccessInfo.job_id = gpcdef.GetUserIdentify(); ///工作类型：计划员，助调
	mMsgGpcAccessInfo.gpc_id = gpcdef.GetConsoleid(); //唯一的,由username转换而来   
	mMsgGpcAccessInfo.con_id = m_nCurrentSectionNo; 
	strcpy(mMsgGpcAccessInfo.os_identify,"W");
	strcpy(mMsgGpcAccessInfo.sys_version,"2.0");
	mMsgGpcAccessInfo.status=CLBTH::MsgGpcAccessReply::GPC_LOGIN_CHECK;
	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if(server_id>0)
		SendBMsg(&mMsgGpcAccessInfo,server_id);
	else
		SendBMsg(&mMsgGpcAccessInfo,0);
}

void CLoginDlg::SendGpcAccessInfo(ushort uAccessStep,const char *userid) //  get userid from gpcdef
{
	CLBTH::MsgGpcAccessInfo mMsgGpcAccessInfo;
	int m_nCurrentSectionList[64];
	int m_nSectionNum  = sizeof(m_nCurrentSectionList) / sizeof(m_nCurrentSectionList[0]);
	gpcdef.GetSectionList(m_nSectionNum,m_nCurrentSectionList);
	int m_nCurrentSectionNo = m_nCurrentSectionList[0];
	strcpy(mMsgGpcAccessInfo.user_code,userid); // gpcdef.GetCurrentUserid());
	mMsgGpcAccessInfo.job_id = gpcdef.GetUserIdentify(); ///工作类型：计划员，助调
	mMsgGpcAccessInfo.gpc_id = gpcdef.GetConsoleid(); //唯一的,由username转换而来   
	mMsgGpcAccessInfo.con_id = m_nCurrentSectionNo; 

	mMsgGpcAccessInfo.step  = ( CLBTH::MsgGpcAccessInfo::ACCESS_STEP)uAccessStep;
	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if(server_id>0)
		SendBMsg(&mMsgGpcAccessInfo,server_id);
	else
		SendBMsg(&mMsgGpcAccessInfo,0); 

}

void CLoginDlg::LoginOK()
{
	//save current username and password
	m_userid.TrimLeft();
	gpcdef.SetCurrentUserid(m_userid);        //userid	
	//加上跟班调度名字
	if(!m_username3.IsEmpty())
	{
		m_username += "/";
		m_username += m_username3;
	}
	gpcdef.SetCurrentUsername(m_username);
	gpcdef.SetCurrentUserpwd(m_strCtcPwd);

	int shiftid=m_nShiftID;
	gpcdef.SetLoginShiftid(m_nShiftID);        //login shiftid,e.g. 2002060701 
	unsigned long m_usCurrentDate = (shiftid/1000000 - 1990) * 380 +
		( shiftid/10000-(shiftid/1000000)*100 - 1) * 31 + 
		shiftid/100 - (shiftid/10000)*100  - 1 ;			 
	gpcdef.SetLoginDate(m_usCurrentDate);     //login date    

	// 仅发送B权限调度员登陆姓名
	if((gpcdef.GetUserIdentify() == GPC_DISPATCHER) &&   (shiftid ==Get_CurrentShiftid()))
	{
		int m_nCurrentSectionList[64];
		int m_nSectionNum  = sizeof(m_nCurrentSectionList) / sizeof(m_nCurrentSectionList[0]);
		gpcdef.GetSectionList(m_nSectionNum,m_nCurrentSectionList);
		int m_nCurrentSectionNo = m_nCurrentSectionList[0];
		Set_Ddy_Name(shiftid,m_nCurrentSectionNo,m_username);
	}

	sysprint(0,PMTG.ThreadId,"[LOGIN]:用户成功登陆主机! ");               				
	LoginWindowSetAutoReloginEnabled(true);

	////// 通知调度命令管理程序
	CString token,content;
	if(GenXMLMessage_InformDCMDLogin(token,content,m_username))
	{
		long nEntityOfDCMD=GetSpecialEntityID(GPCTERM_TYPE_DCMD,0);
		if(nEntityOfDCMD>0)
		{

			TGSendStringMessage(token,content,nEntityOfDCMD);
		}
	}
	CDialog::OnOK();
}

void CLoginDlg::LoadCertifyFile()
{
	CString szPrompt;

	m_vectsvr.clear();
	m_fixToken="";

	CString strCertifyFile = gpcdef.GetDataPathName();
	strCertifyFile += "\\UserCertificate.xml";
	//文件不可读时，按原有认证流程走
	if( _access(strCertifyFile, 4) == -1)
		return;

	MSXML::IXMLDOMNodePtr pNode, pChild;
	MSXML::IXMLDOMDocumentPtr pDOMDoc;
	HRESULT hr = pDOMDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if( FAILED(hr) )
	{
		szPrompt.Format( "[LOGIN]:MSXML 6.0没有安装" );
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		AfxMessageBox(szPrompt);
		return;
	}

	if( VARIANT_TRUE != pDOMDoc->load( (LPCSTR)strCertifyFile) )
	{	//文件格式不正确时的报警
		MSXML::IXMLDOMParseErrorPtr pError = pDOMDoc->GetparseError();
		if(S_OK != pError->GeterrorCode())
		{
			CString str; 
			str.Format(
				"Validation failed on %s"
				"\nLine: %d"
				"\n=====================", 
				strCertifyFile, pError->Getline());

			_bstr_t strRes =  _bstr_t(str)
				+ _bstr_t("\nReason: ") + pError->Getreason()
				+ _bstr_t("\nText: ") + pError->GetsrcText();

			szPrompt = (const char*)strRes;
			GpcLogTrainMessage(0,0,"[LOGIN]:%s", szPrompt);
			AfxMessageBox( szPrompt );
			return;
		}
	}

	// 信达环宇新认证标识
	pNode = pDOMDoc->selectSingleNode( "USER/TOKEN" );
	if(NULL != pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		m_fixToken = v.bstrVal;
		m_fixToken.Trim();
		m_fixToken = m_fixToken.MakeLower();
		GpcLogTrainMessage(0,0,"[LOGIN]:信达环宇新认证标识: %s", CString(v.bstrVal));
	}
		
	// 新模式认证 认证服务器
	MSXML::IXMLDOMNodeListPtr pNodeServerList = pDOMDoc->selectNodes("USER/SERVERLIST/SERVER");
	for(int i=0; i<pNodeServerList->Getlength(); i++)
	{
		MSXML::IXMLDOMNodePtr pServerNode=pNodeServerList->Getitem(i);
		if(pServerNode==NULL)
			continue;

		CertiSvrInfo svr;
		_variant_t v;

		pNode = pServerNode->selectSingleNode("IP");
		if(NULL != pNode)
		{
			v = pNode->GetnodeTypedValue();
			CString ip = v.bstrVal;
			if(ip.GetLength() >= 7 && ip.GetLength() <= 15)
			{
				strncpy(svr.strIp, ip, sizeof(svr.strIp));
				svr.strIp[15] = 0;
			}
		}

		pNode = pServerNode->selectSingleNode("SVRPORT");
		if(NULL != pNode)
		{
			v = pNode->GetnodeTypedValue();
			svr.port = strtoul(CString(v.bstrVal), NULL, 0);
		}
		if(CString(svr.strIp) != "" && svr.port >0)	
		{
			m_vectsvr.push_back(svr);
			GpcLogTrainMessage(0,0,"[LOGIN]:认证服务器 IP: %s, port: %d", svr.strIp, svr.port);
		}
		else
		{
			AfxMessageBox("认证服务器配置错误");
			GpcLogTrainMessage(0,0,"[LOGIN]:认证服务器配置错误 IP : %s, port:  %d", svr.strIp, svr.port);
		}
	}

	if(m_vectsvr.size()>0)
		nFlagEnableVer=1;
	return;
}

bool CLoginDlg::CertificateUser(char *username, char *pwd) 
{
	SetWindowText("正在与认证服务器通信校验用户信息...");
	
	int nFlagCertifyModle = 1;

	// 认证版本 1:动态口令牌 2:认证服务器 3:指纹认证
	if(nFlagEnableVer==1)
	{
		//  如果完全按照认证服务器,则不配置m_fixToken, 否则配置m_fixToken来区
		//  别:动态口令牌和认证服务器两种混合兼容,具体见下面注释
		if(m_fixToken.IsEmpty())
			nFlagCertifyModle = 2;
		else
		{
			CString strUserCode = username; // 用户代码
			if(strUserCode.GetLength() >= m_fixToken.GetLength())
			{
				// 动态口令牌和认证服务器两种混合兼容时,根据用户输
				// 入,比如m_fixToken=nc如果输入nc1213,则按照认证服务器认证;如果输入1213,
				// 按照动态口令牌认证。所以判断strUserName是否为m_fixToken
				// 前缀
				CString token = strUserCode.Left(m_fixToken.GetLength());
				if(token == m_fixToken)
					nFlagCertifyModle = 2;
			}
		}
	}
	else if(nFlagEnableVer==2)
	{
			nFlagCertifyModle = 3;
	}
	
	CString szSysID = gpcdef.GetCurUserName();
	GpcLogTrainMessage(0,0,"[LOGIN]:SysID %s UserName %s", szSysID, username);
	
	if(nFlagCertifyModle == 1)		// 配置用户如何验证
	{
		return CertificateUserVersion1(username, pwd);
	}
	if(nFlagCertifyModle == 2)
	{
		return CertificateUserVersion2(username, pwd, szSysID.GetBuffer());
	}
	if (nFlagCertifyModle == 3)
	{
		return CertificateUserVersion3(username);
	}

	GpcLogTrainMessage(0,0,"认证模式:%d(1:动态口令牌 2:认证服务器 3:指纹认证)" , nFlagCertifyModle);
	
	return false;
}

bool CLoginDlg::CertificateUserVersion1(char *username, char *pwd)
{
	nFlag = 0;
	CDynCertificate dynCert;
	CString cap;
	GetWindowText(cap);
	SetWindowText("正在与认证服务器通信校验用户信息...");
	if(!dynCert.DynInit())
	{
		sysprint(0,PMTG.ThreadId,"[LOGIN]:动态口令认证服务初始化失败");
		AfxMessageBox("动态口令认证服务初始化失败");
		SetWindowText(cap);
		return false;
	}

	int rc = dynCert.DynVerify(username, pwd);

	dynCert.DynUninit();
	SetWindowText(cap);

	if(rc == AUTH_USER_NOTEXIST)
	{
		sysprint(0,PMTG.ThreadId,"[LOGIN]:用户在认证服务器不存在，请联系管理员");
		AfxMessageBox("用户在认证服务器不存在，请联系管理员");				
	}    
	else if(rc == AUTH_PWD_MISMATCH)
	{
		AfxMessageBox("密码错误,请重新输入 ");
		sysprint(0,PMTG.ThreadId,"[LOGIN]:用户密码错误! ");
		m_password="";
		UpdateData(FALSE);
		m_PasswordCtrl.SetFocus();
	}
	else if(rc == AUTH_ERR_NET)
	{
		sysprint(0,PMTG.ThreadId,"[LOGIN]:连接口令认证服务器出错! ");
		AfxMessageBox("连接口令认证服务器出错 ");
	}
	else if(rc == AUTH_ERR_OTHER)
	{
		sysprint(0,PMTG.ThreadId,"[LOGIN]:口令认证出错! ");
		AfxMessageBox("口令认证出错，请联系管理员");
	}

	//查看返回错误是否是连接问题
	if(rc == AUTH_ERR_NET || rc == AUTH_ERR_OTHER)
	{
		rc=AUTH_ERR_NET;
	}

	return (rc==AUTH_SUCCESS);
}

bool CLoginDlg::CertificateUserVersion2(char *username, char *pwd, char* szSysID)
{
	CString szPrompt;

	if (m_hGiOtpAPIDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:信达环宇服务器认证模块GiAgentX.dll 不存在");
		return false;
	}

	//DLL API接口函数
	pGiOtp_Auth GiOtp_Auth=(pGiOtp_Auth)GetProcAddress(m_hGiOtpAPIDLL,"GiOtp_Auth");
	if (GiOtp_Auth == NULL)
	{
		szPrompt.Format("[LOGIN]:信达环宇GiAgentX.dll中函数GiOtp_Auth不存在");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		AfxMessageBox( szPrompt );
		return false;
	}

	//逐个服务器进行验证，当有一个服务器验证通过时跳出循环
	CString szSvrIPAddr("127.0.0.1");	//口令牌服务器IP地址
	int nSvrPort = 12751;				//口令牌服务端口号
	bool bPass = false;
	for(int idx=0; idx<m_vectsvr.size(); idx++)
	{
		nSvrPort = m_vectsvr[idx].port;
		szSvrIPAddr = m_vectsvr[idx].strIp;
	
		//开始校验
		char* pTemp = NULL;
		int nVal = GiOtp_Auth(szSvrIPAddr, nSvrPort, szSysID, username, pwd,&pTemp);
		if (nVal == 0)
		{ 
			szPrompt.Format(
				"[LOGIN]- 新认证服务器, 认证服务器%s(port%d) 认证 资源:%s 用户:%s 通过",
				szSvrIPAddr, nSvrPort, szSysID, username);
			GpcLogTrainMessage(0,0,"%s", szPrompt);
			
			bPass = true;// 认证通过
			break; 
		}
		else
		{	
			//认证不通过，记录失败原因
			struct FailReason {int nCode; LPCSTR szReason; };
			static FailReason reason[] = {
				{-1100, "连接认证服务器失败"},
				{-1101, "数据传输失败"},
				{-1102, "DAC校验失败，数据可能被篡改"},
				{-9999, "系统异常"},
				{-1001, "应用标识 XXX不存在"},
				{-1002, "操作员 XXX 不存在"},
				{-1003, "操作员XXX非正常使用状态"},
				{-1004, "操作员XXX未分配令牌"},
				{-1008, "本次认证口令与上次相同"},
				{-1099, "应答码错误"},
				{-1021, "令牌同步处理失败"},
				{-1020, "无此令牌"},
				{-1009, "计算错误"},
			};
			for(int iReason=0; iReason<sizeof(reason)/sizeof(reason[0]); iReason++)
			{
				if(reason[iReason].nCode == nVal)
				{
					szPrompt.Format(
						"[LOGIN]- 新认证服务器%s(port%d) 认证 资源:%s 用户:%s 不通过,\n"
						"错误码:%d, 原因:%s",
						szSvrIPAddr, nSvrPort, szSysID, username, 
						nVal, reason[iReason].szReason);
					GpcLogTrainMessage(0,0,"%s", szPrompt);
					AfxMessageBox(szPrompt);
					break;
				}
			}
		}
	}

	return bPass;
}

void CLoginDlg::LoadCertifyFile3()
{
	CString szPrompt;
	m_vectsvr2.clear();
	m_vectsvr3.clear();
	
	CString strCertifyFile = gpcdef.GetDataPathName();
	strCertifyFile += "\\UserFingerCertificateNo.xml";
	//文件不可读时，按原有认证流程走
	if( _access(strCertifyFile, 4) == -1)
		return;

	MSXML::IXMLDOMNodePtr pNode, pChild, pServerNode;
	MSXML::IXMLDOMDocumentPtr pDOMDoc;
	HRESULT hr = pDOMDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if( FAILED(hr) )
	{
		szPrompt.Format( "[LOGIN]:MSXML 6.0没有安装" );
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		AfxMessageBox(szPrompt);
		return;
	}

	if( VARIANT_TRUE != pDOMDoc->load( (LPCSTR)strCertifyFile) )
	{	//文件格式不正确时的报警
		MSXML::IXMLDOMParseErrorPtr pError = pDOMDoc->GetparseError();
		if(S_OK != pError->GeterrorCode())
		{
			CString str; 
			str.Format(
				"Validation failed on %s"
				"\nLine: %d"
				"\n=====================", 
				strCertifyFile, pError->Getline());

			_bstr_t strRes =  _bstr_t(str)
				+ _bstr_t("\nReason: ") + pError->Getreason()
				+ _bstr_t("\nText: ") + pError->GetsrcText();

			szPrompt = (const char*)strRes;
			GpcLogTrainMessage(0,0,"[LOGIN]:%s", szPrompt);
			AfxMessageBox( szPrompt );
			return;
		}
	}

	MSXML::IXMLDOMNodePtr UsbNode = pDOMDoc->selectSingleNode("USER/USBPORT");
	if(UsbNode)
	{
		m_isLocalGetData = 1 ;
		_variant_t Val = UsbNode ->GetnodeTypedValue();
		m_UsbPort =atoi(CString(Val.bstrVal));

		if(m_UsbPort < -1 || m_UsbPort >9)
		{
			AfxMessageBox("指纹采集设备配置错误");
			GpcLogTrainMessage(0,0,"[LOGIN]:指纹采集设备配置错误 USBID: %d", m_UsbPort);
		}
	}
	else
	{
	//指纹采集设备
		MSXML::IXMLDOMNodeListPtr pNodeFPEquipmentList = pDOMDoc->selectNodes("USER/EQUIPMENT_LIST/EQUIPMENT");
		for(int i=0; i<pNodeFPEquipmentList->Getlength(); i++)
		{
			int nGpcNo = 0;
			pServerNode=pNodeFPEquipmentList->Getitem(i);
			if(pServerNode==NULL)
				continue;

			CertiSvrInfo svr2;
			_variant_t v2;

			svr2.nTf=1;
			pNode = pServerNode->selectSingleNode("@GPCNO");
			if(NULL != pNode)
			{
				v2 = pNode->GetnodeTypedValue();
				nGpcNo=atoi(CString(v2.bstrVal));

				if(nGpcNo != gpcdef.GetUserGpcId())
					continue;
			}

			if (gpcdef.GetUserGpcId() == nGpcNo)
			{
				pNode = pServerNode->selectSingleNode("TF");
				if(NULL != pNode)
				{
					v2 = pNode->GetnodeTypedValue();
					svr2.nTf=atoi(CString(v2.bstrVal));
				}

				pNode = pServerNode->selectSingleNode("IP");
				if(NULL != pNode)
				{
					v2 = pNode->GetnodeTypedValue();
					CString ip = v2.bstrVal;
					if(ip.GetLength() >= 7 && ip.GetLength() <= 15)
					{
						strncpy(svr2.strIp, ip, sizeof(svr2.strIp));
						svr2.strIp[15] = 0;
					}
				}

				pNode = pServerNode->selectSingleNode("SVRPORT");
				if(NULL != pNode)
				{
					v2 = pNode->GetnodeTypedValue();
					svr2.port = strtoul(CString(v2.bstrVal), NULL, 0);
				}
			}

			if(CString(svr2.strIp) != "" && svr2.port >0 && svr2.nTf == 1 && nGpcNo > 0)
			{
				m_vectsvr2.push_back(svr2);
				GpcLogTrainMessage(0,0,"[LOGIN]:指纹采集设备 IP: %s , port: %d, ntf: %d, GpcNo: %d", svr2.strIp, svr2.port, svr2.nTf, nGpcNo);
			}
			else
			{
				AfxMessageBox("指纹采集设备配置错误");
				GpcLogTrainMessage(0,0,"[LOGIN]:指纹采集设备配置错误 IP: %s ,port: %d, ntf: %d, GpcNo: %d ", svr2.strIp, svr2.port, svr2.nTf, nGpcNo);
				return;
			}
		//指纹比对服务器
		}
	}
	if (m_vectsvr2.size() == 0)
	{
		GpcLogTrainMessage(0,0,"[LOGIN]:当前用户没有配置指纹采集设备");
		return;
	}
	MSXML::IXMLDOMNodeListPtr pNodeFPServerList = pDOMDoc->selectNodes("USER/SERVER_LIST/SERVER");
	for(int i=0; i<pNodeFPServerList->Getlength(); i++)                      
	{
		pServerNode=pNodeFPServerList->Getitem(i);
		if(pServerNode==NULL)
			continue;

		CertiSvrInfo svr3;
		_variant_t v3;

		pNode = pServerNode->selectSingleNode("IP");
		if(NULL != pNode)
		{
			v3 = pNode->GetnodeTypedValue();
			CString ip = v3.bstrVal;
			if(ip.GetLength() >= 7 && ip.GetLength() <= 15)
			{
				strncpy(svr3.strIp, ip, sizeof(svr3.strIp));
				svr3.strIp[15] = 0;
			}
		}

		pNode = pServerNode->selectSingleNode("SVRPORT");
		if(NULL != pNode)
		{
			v3 = pNode->GetnodeTypedValue();
			svr3.port = strtoul(CString(v3.bstrVal), NULL, 0);
		}
		if(CString(svr3.strIp) != "" && svr3.port > 0)
		{
			m_vectsvr3.push_back(svr3);
			GpcLogTrainMessage(0,0,"[LOGIN]:指纹比对服务器 IP: %s , port: %d", svr3.strIp, svr3.port);
		}
		else
		{
			AfxMessageBox("指纹比对服务器配置错误");
			GpcLogTrainMessage(0,0,"[LOGIN]:指纹比对服务器配置错误 IP: %s , port: %d ", svr3.strIp, svr3.port);
			return;
		}
	}
	
	if (m_vectsvr3.size()>0 )
	{
		if(m_vectsvr2.size()>0 || m_UsbPort >= 0)
			nFlagEnableVer=2;
	}
	return;
}
bool CLoginDlg::CertificateUserVersion3(char *username)//信达环宇指纹验证 hjz
{
	CString szPrompt;
	if (m_hFpClientDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:信达环宇指纹验证模块FpClient.dll 不存在");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		AfxMessageBox( szPrompt );
		return false;
	}

	//DLL API接口函数
	pRemoteVerifyFp RemoteVerifyFp=(pRemoteVerifyFp)GetProcAddress(m_hFpClientDLL,"remoteVerifyFp");
	if (RemoteVerifyFp == NULL)
	{
		szPrompt.Format("[LOGIN]:信达环宇FpClient.dll中函数RemoteVerifyFp不存在");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		AfxMessageBox( szPrompt );
		return false;
	}

	//1.指纹采集
	BYTE szFingerInfo[512];
	int  nFingerInfoLen=sizeof(szFingerInfo)/sizeof(szFingerInfo[0]);
	memset(szFingerInfo, 0, nFingerInfoLen);
	bool bCollectionOK = FingerCollection(szFingerInfo, &nFingerInfoLen);

	//2.发送至服务器进行比对
	if(bCollectionOK)
	{
		SetWindowText("正在验证用户指纹......");
		int szPort = 0;
		for(int idx=0; idx<m_vectsvr3.size(); idx++)
		{
			CString szSvrIPAddr = m_vectsvr3[idx].strIp;
			szPort = m_vectsvr3[idx].port;

			//开始校验
			int nVal = RemoteVerifyFp(szSvrIPAddr.GetBuffer(), szPort, username, szFingerInfo);
			if (nVal == 0)
			{ 
				szPrompt.Format("[LOGIN]- 指纹认证服务器%s:%d 认证用户:%s 通过",szSvrIPAddr,szPort, username);
				return true;// 认证通过
			}
			else
			{	
				//认证不通过，记录失败原因
				struct FailReason {int nCode; LPCSTR szReason; };
				static FailReason reason[] = {
					{-1, "连接超时"},
					{1, "比对失败,两枚指纹不相同"},
					{2, "数据格式不正确"},
					{3, "网络连接失败"},
				};
				for(int iReason=0; iReason<sizeof(reason)/sizeof(reason[0]); iReason++)
				{
					if(reason[iReason].nCode == nVal)
					{
						szPrompt.Format(
							"[LOGIN]- 认证服务器%s:%d 认证用户:%s 不通过,错误码:%d, 原因:%s",
							szSvrIPAddr, szPort, username, nVal, reason[iReason].szReason);
						GpcLogTrainMessage(0,0,"%s", szPrompt);
						break;
					}
				}
			}
		}
	}
	
	szPrompt.Format("你的指纹验证失败, 无权限登陆本终端, 请确认并重新输入用户代号");
	GpcLogTrainMessage(0,0,"%s username %", szPrompt, username);

	MessageBox(szPrompt, "错误", MB_ICONHAND);

	return false;
}

bool CLoginDlg::FingerCollection(BYTE* szFingerInfo, int* nFingerInfoLen)
{
	CString szPrompt;
	if (m_hJHDLLCUDLL == NULL)
	{
		szPrompt.Format("[LOGIN]:信达环宇指纹采集模块jhdllcu.dll 不存在");
		GpcLogTrainMessage(0,0,"%s", szPrompt);
		MessageBox("无法进行指纹采集, 请维护人员确认dll目录下是否存在jhdllcu.dll", "警告", MB_ICONWARNING);
		return false;
	}
	pFPIFea_IP FPIFea_IP=(pFPIFea_IP)GetProcAddress(m_hJHDLLCUDLL,"FPIFea_IP");
	if (FPIFea_IP == NULL)
	{
		szPrompt.Format("[LOGIN]:信达环宇jhdllcu.dll中函数FPIFea_IP不存在");
		GpcLogTrainMessage(0,0,"%s", szPrompt);

		MessageBox("无法进行指纹采集, 请维护人员确认dll目录下jhdllcu.dll是否为正确版本", "警告", MB_ICONWARNING);
		return false;
	}
	pFPIFeature_FDR FPIFeature_FDR=(pFPIFeature_FDR)GetProcAddress(m_hJHDLLCUDLL,"FPIFeature_FDR");

	if(FPIFeature_FDR == NULL)
	{
		szPrompt.Format("[LOGIN]:信达环宇jhdllcu.dll中函数FPIFeature_FDR不存在");
		GpcLogTrainMessage(0,0,"%s", szPrompt);

		MessageBox("无法进行指纹采集, 请维护人员确认dll目录下jhdllcu.dll是否为正确版本", "警告", MB_ICONWARNING);
		return false;
	
	}

	if(m_isLocalGetData == 1)
	{
		while(1)
		{
			int nVal = FPIFeature_FDR(m_UsbPort, szFingerInfo, nFingerInfoLen); 
			if (nVal != 0)
			{	
				struct FailReason {int nCode; LPCSTR szReason; };
				static FailReason reason[] = {
					{-1, "建立连接失败"},
					{-2, "返回数据冗余检验出错"},
					{-3, "发送数据失败"},
					{-4, "接受数据失败"},
					{-5, "指纹采集失败"},
				};
				for(int iReason=0; iReason<sizeof(reason)/sizeof(reason[0]); iReason++)
				{
					if(reason[iReason].nCode == nVal)
					{
						szPrompt.Format(
							"[LOGIN]- 指纹采集usb设备采集指纹失败, 错误码:%d, 原因:%s",
							nVal, reason[iReason].szReason);

						GpcLogTrainMessage(0, 0,"%s", szPrompt);
						break;
					}
				}		
			}
			else
			{
				return true; // usb采集成功,返回
			}
			if(MessageBox("指纹采集失败, 你是否重新采集", "警告", MB_ICONWARNING|MB_OKCANCEL) == IDCANCEL)
				break;
		}
		return false;
	}
	else
	{
		while(1)
		{
			CString szEquipIPAddr;
			for(int idx=0; idx<m_vectsvr2.size(); idx++)
			{
				SetWindowText("请进行指纹采集");

				szEquipIPAddr.Format("%s:%d", m_vectsvr2[idx].strIp, m_vectsvr2[idx].port);
				int nVal = FPIFea_IP(szEquipIPAddr, szFingerInfo, nFingerInfoLen, m_vectsvr2[idx].nTf);
				if (nVal != 0)
				{	
					struct FailReason {int nCode; LPCSTR szReason; };
					static FailReason reason[] = {
						{-1, "建立连接失败"},
						{-2, "返回数据冗余检验出错"},
						{-3, "发送数据失败"},
						{-4, "接受数据失败"},
						{-5, "指纹采集失败"},
					};
					for(int iReason=0; iReason<sizeof(reason)/sizeof(reason[0]); iReason++)
					{
						if(reason[iReason].nCode == nVal)
						{
							szPrompt.Format(
								"[LOGIN]- 指纹采集设备%s采集指纹失败, 错误码:%d, 原因:%s",
								szEquipIPAddr, nVal, reason[iReason].szReason);

								GpcLogTrainMessage(0, 0,"%s", szPrompt);
							break;
						}
					}
				}
				else
				{
					return true; // 采集成功,返回
				}
			}

			if(MessageBox("指纹采集失败, 你是否重新采集", "警告", MB_ICONWARNING|MB_OKCANCEL) == IDCANCEL)
				break;
		}
		return false;
	}
}

void CLoginDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent==timeid)
	{
		KillTimer(timeid);
		bool m_bcfvsr_status=CertificateUser(m_userid.GetBuffer(), "");  // 发送给身份认证服务器
		if(m_bcfvsr_status)
		{			
			SendLoginInfo(m_userid,m_strCtcPwd);
			GpcLogTrainMessage(0,0,"[LOGIN]:完全由信达环宇指纹服务器认证 %s %s", m_userid, m_strCtcPwd);
		}
		else
		{
			m_username="";
			m_strCtcPwd="";
			UpdateData(FALSE);
			m_UseridCtrl.EnableWindow(TRUE);
			m_PasswordCtrl.EnableWindow(FALSE);
			m_UseridCtrl.SetWindowText("");
			m_UseridCtrl.SetFocus( );
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CLoginDlg::OnBnClickedCheckLogin()
{
	if(m_checkbtn.GetCheck()==TRUE)
	{
		m_bIgnoreLoginTm=true;
	}
	else
	{
		m_bIgnoreLoginTm=false;
	}
}
