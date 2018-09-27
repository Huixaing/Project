#include "StdAfx.h"
#include "tgdocument.h"
#include ".\dlgbasicdeletename.h"
#include "basic_schd_config.h"
#include <baseMsg\msgid.h>
#include <BaseMsg/clbth.h>
#include <baseMsg\msg_schd.h>
#include "sysshare.h"
#include "tgmsg.h"

extern basic_schd_config* GetBasicConfig();
extern BOOL ReqBasicName_ToServer(int con_id, bool bload);

CDlgBasicDeleteName::CDlgBasicDeleteName(int conid, CTgDocument *pDoc, CWnd* pParent)
: CDialog(CDlgBasicDeleteName::IDD, pParent)
{
	m_nConid=conid;
	m_pDoc=pDoc;
	m_pParant=pParent;
}

CDlgBasicDeleteName::~CDlgBasicDeleteName(void)
{
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgBasicScheduleSet);
}

BOOL CDlgBasicDeleteName::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgBasicScheduleSet,0);
	UpdateBakcupBasicName();
	InitDlgPosition(this);
	return TRUE;  
}

void CDlgBasicDeleteName::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CDlgBasicDeleteName)
	DDX_Control(pDX, IDC_LIST_BASICNAME, m_combo_basicname);
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CDlgBasicDeleteName, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
END_MESSAGE_MAP()

void CDlgBasicDeleteName::OnBnClickedBtnDelete()
{
	int nSel = m_combo_basicname.GetCurSel();
	if(nSel>=0)
	{
		CLBTH::MsgBasicScheduleSet schd_name;
		schd_name.nConId =m_nConid;
		schd_name.nScheduleId=m_combo_basicname.GetItemData(nSel);

		// 基本图名称 = 0 表示 删除该
		memset(schd_name.nScheduleName, 0, MAX_SCHEDULE_NAME_LEN);

		schd_name.nStartTime=0;
		schd_name.nEndTime=0;

		// 预计投入时间
		schd_name.nEstimateStartTime=0;

		// 命名时间
		CTime tm=CTime::GetCurrentTime();
		schd_name.nSetNameTime=tm.GetTime();

		// 命名实体
		schd_name.nSetNameEntity=gpcdef.GetThisEntity();

		long eid=GetSpecialEntityID(SERVER_TYPE_BASIC_TG,schd_name.nConId);
		SendTGBMsg(schd_name.nConId,&schd_name,eid);

		sysprint(10,PMTG.ThreadId,"[删除归档基本图]:%d向%d请求删除归档基本图 nConId %d nScheduleId %d nScheduleName:%s.", 
			schd_name.nSetNameEntity,
			eid, schd_name.nConId, schd_name.nScheduleId, 
			schd_name.nScheduleName);
		
		m_combo_basicname.ResetContent();
		sysprint(10,PMTG.ThreadId,"[删除归档基本图]:清空下拉选择框中归档基本图名称,等待服务器返回更新归档基本图名字.");

		SetTimer(22222,200, NULL);
	}
}

//void CDlgBasicDeleteName::OnOK()
//{
//	// TODO: 在此添加专用代码和/或调用基类
//
//	CDialog::OnOK();
//}

void CDlgBasicDeleteName::OnBnClickedBtnClose()
{
	OnCancel();
}

void CDlgBasicDeleteName::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 22222)
	{
		KillTimer(22222);
		ReqBasicName_ToServer(m_nConid, true);
	}
		
	CDialog::OnTimer(nIDEvent);
}

LRESULT CDlgBasicDeleteName::OnCommMsg(WPARAM wParam,LPARAM lParam)
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if(pMsgStruct->msg_id == MSGID_MsgBasicScheduleSet)
	{
		CLBTH::MsgBasicScheduleSet *pMsg= ( CLBTH::MsgBasicScheduleSet *)pMsgStruct->pmsg;
		if(m_nConid == pMsg->nConId)
		{
			// 2017-09-11 这段代码和 tg_logic_main中重复，但是不能删除，因为tg_logic_main中后保存，这里先刷新，会总是少一条基本图名字
			pMsg->nScheduleName[MAX_SCHEDULE_NAME_LEN-1]=0;
	        GetBasicConfig()->SetBasicSchdName(pMsg->nConId, pMsg->nScheduleId, pMsg->nScheduleName, pMsg->nStartTime, pMsg->nEndTime, pMsg->nEstimateStartTime);
			
			sysprint(10,PMTG.ThreadId,"[删除归档基本图]:收到服务器返回基本图名字,更新对话框归档基本图名字.");
			UpdateBakcupBasicName();
		}
	}
	
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

void CDlgBasicDeleteName::UpdateBakcupBasicName()
{
	m_combo_basicname.ResetContent();
	SECTION_BASIC_NAME* pBasic = GetBasicConfig()->GetBasicSchdNameStruct(m_nConid);
	if(pBasic)
	{
		for(int i=0; i<pBasic->num; i++)
		{
			CString text = pBasic->basic_backup_schd[i].name;
			int     type = pBasic->basic_backup_schd[i].type;
			int index=m_combo_basicname.AddString(text);
			m_combo_basicname.SetItemData(index, type);
		}
	}
}
