// DlgBasicParam.cpp : implementation file
//

#include "stdafx.h"
#include "tg.h"
#include "tgdocument.h"
#include "TgEditView.h"
#include "DlgBasicParam.h"
#include "SuperPassDlg.h"
#include "DlgMessageBox.h"
#include "tgmsg.h"
#include "sysshare.h"
#include <baseMsg\msgid.h>
#include <BaseMsg/clbth.h>
#include ".\dlgbasicparam.h"
#include "basic_schd_config.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern basic_schd_config* GetBasicConfig();

/////////////////////////////////////////////////////////////////////////////
// CDlgBasicParam dialog
CDlgBasicParam::CDlgBasicParam(CTgDocument *pDoc, int schdtype, CWnd* pParent)
: CDialog(CDlgBasicParam::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBasicParam)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_nschdtype=schdtype;
	m_pParant=pParent;
}
CDlgBasicParam::~CDlgBasicParam()
{
}
void CDlgBasicParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBasicParam)
	DDX_Control(pDX, IDC_PROGRESS1, m_CtrlPro);
	DDX_Text(pDX, IDC_NAME_BASIC, m_strName);
	DDV_MaxChars(pDX, m_strName, 200);  //实际上最大为200
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_basic_start_day);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_basic_start_time);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgBasicParam, CDialog)
	//{{AFX_MSG_MAP(CDlgBasicParam)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnOK)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK2, OnBnClickedOk2)
END_MESSAGE_MAP()
//收到换图的反馈消息

LRESULT CDlgBasicParam::OnCommMsg(WPARAM wParam,LPARAM lParam)
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	CLBTH::MsgBasicScheduleSet *pMsg= ( CLBTH::MsgBasicScheduleSet *)pMsgStruct->pmsg;
	m_CtrlPro.SetPos(100);

	CString  sectip;
	if(pMsg->nConId==m_nConid && pMsg->nScheduleId==m_nschdtype && pMsg->nSetNameEntity==m_myEntity)
	{
		if(pMsg->nSetNameTime>0)
		{
			sectip="基本图命名成功";
		}
		else
		{
			sectip="基本图命名失败";
		}

		sysprint(10,PMTG.ThreadId,"[基本图]:命名 %s con_id %d schd %d name %s", sectip, pMsg->nConId, pMsg->nScheduleId, pMsg->nScheduleName);
		KillTimer(201);
		AfxMessageBox(sectip);
		CWnd *pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(TRUE);
	}


	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

BOOL CDlgBasicParam::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(gpcdef.IsEnableBasicDumpTime())
	{
		CWnd* pWnd=GetDlgItem(IDC_STATIC_TIME);
		if(pWnd)
			pWnd->ShowWindow(SW_SHOW);

		m_basic_start_day.ShowWindow(SW_SHOW);
		m_basic_start_time.ShowWindow(SW_SHOW);
	}

	// 命名实体
	GPCENTITY entityInfo;
	if (false == gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entityInfo))
	{
		AfxMessageBox("配置文件错误：找不到本用户的ENTITY!");
		(CWnd*)GetDlgItem(IDC_NAME_BASIC)->EnableWindow(FALSE);
		m_basic_start_day.EnableWindow(FALSE);
		m_basic_start_time.EnableWindow(FALSE);
		return 0;
	}
	m_myEntity=entityInfo.nEntityId;
	m_nConid = m_pDoc->GetCurrentSectionNo();
	m_basic_start_day.SetFormat("yyyy-MM-dd");
	m_basic_start_time.SetFormat("HH:mm:ss");
		
	CTime time(2011, 3, 20, 18, 0, 0);
	m_basic_start_time.SetTime(&time);

	CString strName="基本图名称:";
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC==m_nschdtype)
	{
		strName="常用基本图名称";
		m_nschdtype=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==m_nschdtype)
	{
		strName="预览基本图名称";
		m_nschdtype=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV;
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP==m_nschdtype)
	{
		strName="归档基本图名称";
		m_nschdtype=GetBasicConfig()->GetCurSelBackupBasicType(m_nConid);
	}

	CWnd* pWnd = GetDlgItem(IDC_STATIC_NAME);
	if(pWnd)
	{
		pWnd->SetWindowText(gpcdef.GetSectionName(m_nConid)+strName);
	}
	// TODO: Add extra initialization here
	InitDlgPosition(this);
	nChangeStatusPos = 0;
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgBasicScheduleSet,0);

	pWnd = (CWnd*)GetDlgItem(IDC_NAME_BASIC);
	if(pWnd)
	{
		pWnd->SetFocus();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBasicParam::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnBnClickedOk2();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgBasicParam message handlers
void CDlgBasicParam::OnOK() 
{
	CString strBasicName;
	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_NAME_BASIC);
	if(pWnd)
	{
		pWnd->GetWindowText(strBasicName);
	}

	if(strBasicName.IsEmpty())
	{
		AfxMessageBox("请输入基本图名字！！");
		return;
	}
	m_CtrlPro.SetPos(0);
	CSuperPassDlg pdlg;
	if (pdlg.DoModal() == IDOK)
	{
		if (!pdlg.PasswordIsOk())
		{
			CString str;
			str.Format("%s","密码错误!");
			CDlgMessageBox dlg(str);
			dlg.DoModal();
			return;
		}
	}
	else
		return;

	MsgBasicScheduleSet schd_name;
	schd_name.nConId =m_nConid;
	schd_name.nScheduleId=m_nschdtype;

	// 基本图名称
	memset(schd_name.nScheduleName, 0, MAX_SCHEDULE_NAME_LEN);
	strncpy(schd_name.nScheduleName, strBasicName, MAX_SCHEDULE_NAME_LEN-1);
	schd_name.nScheduleName[MAX_SCHEDULE_NAME_LEN-1]=0;

	schd_name.nStartTime=0;
	schd_name.nEndTime=0;

	// 预计投入时间
	schd_name.nEstimateStartTime=0;
	if(gpcdef.IsEnableBasicDumpTime())
	{
		CTime day, time;
		m_basic_start_day.GetTime(day);
		m_basic_start_time.GetTime(time);

		CTime day_time(day.GetYear(), day.GetMonth(), day.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
		schd_name.nEstimateStartTime=day_time.GetTime();
		sysprint(10,PMTG.ThreadId,"[基本图]: 预计投用时间:%s", day_time.Format("%Y-%m-%d %H:%M:%S"));
	}

	// 命名时间
	CTime tm=CTime::GetCurrentTime();
	schd_name.nSetNameTime=tm.GetTime();

	// 命名实体
	schd_name.nSetNameEntity=m_myEntity;

	long eid=GetSpecialEntityID(SERVER_TYPE_BASIC_TG,schd_name.nConId);
	SendTGBMsg(schd_name.nConId,&schd_name,eid);

	sysprint(10,PMTG.ThreadId,"[基本图]:命名 svr_eid %d conid %d schdid %d sttm %d edtm %d name %s(gpc_eid:%d update:%s)", 
			eid, schd_name.nConId, schd_name.nScheduleId, 
			schd_name.nStartTime, schd_name.nEndTime, 
			schd_name.nScheduleName,
			schd_name.nSetNameEntity, 
			tm.Format("%Y-%m-%d %H:%M:%S"));
	

	pWnd = GetDlgItem(IDOK);
	pWnd->EnableWindow(FALSE);

	m_CtrlPro.SetRange(0,100);

	SetTimer(201,80,NULL);
	return;
}

void CDlgBasicParam::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent != 201)
		return;
	nChangeStatusPos +=2;
	if(nChangeStatusPos >=100)
		nChangeStatusPos =0;
	m_CtrlPro.SetPos(nChangeStatusPos);
	m_CtrlPro.Invalidate();

	CDialog::OnTimer(nIDEvent);
}

void CDlgBasicParam::OnBnClickedOk2()
{
	// TODO: 在此添加控件通知处理程序代码
	((CTgEditView*)m_pParant)->OnUpdateTitle();
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgBasicScheduleSet);
	OnCancel();
}
