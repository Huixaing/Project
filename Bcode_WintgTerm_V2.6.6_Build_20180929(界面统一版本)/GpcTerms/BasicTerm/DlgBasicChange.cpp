// DlgBasicChange.cpp : implementation file
//

#include "stdafx.h"
#include "tg.h"
#include "tgdocument.h"
#include "DlgBasicChange.h"
#include "SuperPassDlg.h"
#include "DlgMessageBox.h"
#include "tgmsg.h"
#include "sysshare.h"
#include <baseMsg\msgid.h>
#include <BaseMsg/clbth.h>
#include ".\dlgbasicchange.h"
#include "basic_schd_config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CUR_BASIC_PREV 0
#define CUR_BASIC_BACKUP 1
#define CUR_BASIC 2


/////////////////////////////////////////////////////////////////////////////
// CDlgBasicChange dialog
extern CWintg  theWintgApp;
extern BOOL ReqBasicName_ToServer(int con_id, bool bload);
extern basic_schd_config* GetBasicConfig();

CDlgBasicChange::CDlgBasicChange(CTgDocument *pDoc, CTG_ScheduleInfo::SCHEDULE_TYPE schdtype, CWnd* pParent)
: CDialog(CDlgBasicChange::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBasicChange)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_nSrcSchdType=schdtype;
	m_bSucceed=FALSE;
}
CDlgBasicChange::~CDlgBasicChange()
{
}
void CDlgBasicChange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBasicChange)
	DDX_Control(pDX, IDC_PROGRESS, m_CtrlPro);

	DDX_Control(pDX, IDC_COMBO_CURR, m_combo_cur);
	DDX_Control(pDX, IDC_COMBO_CUR_NAME, m_combo_cur_name);
	DDX_Control(pDX, IDC_COMBO_OBJ, m_combo_obj);
	DDX_Control(pDX, IDC_COMBO_OBJ_NAME, m_combo_obj_name);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgBasicChange, CDialog)
	//{{AFX_MSG_MAP(CDlgBasicChange)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnOK)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJ, OnSelObjSchdType)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJ_NAME, OnSelObjSchdName)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()
//收到换图的反馈消息
LRESULT CDlgBasicChange::OnCommMsg(WPARAM wParam,LPARAM lParam)
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	
	CString  sectip;
	if(gpcdef.IsNewBasicDump())
	{
		CLBTH::MsgConvertBasicSchdNew *pMsg= ( CLBTH::MsgConvertBasicSchdNew *)pMsgStruct->pmsg;
		if(pMsg->flag==MsgConvertBasicSchdNew::ECHO_OK)
		{
			sectip="换图操作成功";
			m_bSucceed=TRUE;
			ReqBasicName_ToServer(pMsg->conid, true);
		}
		else
		{
			sectip="换图操作失败";
			m_bSucceed=FALSE;
		}
	}
	else
	{
		CLBTH::MsgConvertBasicSchd *pMsg= ( CLBTH::MsgConvertBasicSchd *)pMsgStruct->pmsg;
		if(pMsg->flag==MsgConvertBasicSchd::ECHO_OK)
		{
			sectip="换图 操作成功";
			m_bSucceed=TRUE;
		}
		else
		{
			sectip="换图 操作失败";
			m_bSucceed=FALSE;
		}
	}

	sysprint(10,PMTG.ThreadId,"[基本图]:%s", sectip);

	m_CtrlPro.SetPos(100);
	KillTimer(201);
	AfxMessageBox(sectip);
	CWnd *pWnd = GetDlgItem(IDOK);
	pWnd->EnableWindow(TRUE);
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

BOOL CDlgBasicChange::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nConid = m_pDoc->GetCurrentSectionNo();

	m_nObjSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL;
	if(gpcdef.IsNewBasicDump())
	{
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgConvertBasicSchdNew,0);
		m_combo_cur.AddString("预览基本图");
		m_combo_cur.SetItemData(CUR_BASIC_PREV, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV);
		m_combo_cur.AddString("归档基本图");
		m_combo_cur.SetItemData(CUR_BASIC_BACKUP, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP);
		m_combo_cur.AddString("常用基本图");
		m_combo_cur.SetItemData(CUR_BASIC, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);

		if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC==m_nSrcSchdType)
		{
			m_combo_cur.SetCurSel(CUR_BASIC);
			m_combo_obj.AddString("预览基本图");
			m_combo_obj.SetItemData(0, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV);
			m_combo_obj.AddString("归档基本图");
			m_combo_obj.SetItemData(1, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP);
		}
		else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==m_nSrcSchdType)
		{
			m_combo_cur.SetCurSel(CUR_BASIC_PREV);
			m_combo_obj.AddString("常用基本图");
			m_combo_obj.SetItemData(0, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);
			m_combo_obj.AddString("归档基本图");
			m_combo_obj.SetItemData(1, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP);	
		}
		else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP==m_nSrcSchdType)
		{
			m_combo_cur.SetCurSel(CUR_BASIC_BACKUP);
			m_combo_obj.AddString("预览基本图");
			m_combo_obj.SetItemData(0, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV);
			m_combo_obj.AddString("常用基本图");
			m_combo_obj.SetItemData(1, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);

			if(gpcdef.GetUserIdentify() == GPC_DISPATCHER)
			{
				m_combo_obj.SetCurSel(1);
				OnSelObjSchdType();
				m_combo_obj.EnableWindow(FALSE);
			}
			else
			{
				m_combo_obj.EnableWindow(TRUE);
			}
		}
		else
		{
			m_combo_obj.EnableWindow(FALSE);
			m_combo_obj_name.EnableWindow(FALSE);
			CWnd *pWnd = GetDlgItem(IDOK);
			pWnd->EnableWindow(FALSE);
		}

		m_combo_cur.EnableWindow(FALSE);
		CString name=GetBasicConfig()->GetBasicNameByType(m_nConid, m_nSrcSchdType);
		if(!name.IsEmpty())
		{
			m_combo_cur_name.AddString(name);
			m_combo_cur_name.SetCurSel(0);
		}
		m_combo_cur_name.EnableWindow(FALSE);
	}
	else
	{
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgConvertBasicSchd,0);
		m_combo_cur.AddString("预览基本图");
		m_combo_cur.SetItemData(0, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV);
		m_combo_cur.SetCurSel(0);
		m_combo_cur.EnableWindow(FALSE);
		CString name=GetBasicConfig()->GetBasicNameByType(m_nConid, m_nSrcSchdType);
		if(!name.IsEmpty())
		{
			m_combo_cur_name.AddString(name);
			m_combo_cur_name.SetCurSel(0);
		}
		m_combo_cur_name.EnableWindow(FALSE);

		
		m_nSrcSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV;
		m_nObjSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;
		m_combo_obj.AddString("常用基本图");
		m_combo_obj.SetItemData(0, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);
		m_combo_obj.SetCurSel(0);
		name=GetBasicConfig()->GetBasicNameByType(m_nConid, m_nSrcSchdType);
		if(!name.IsEmpty())
		{
			m_combo_obj_name.AddString(name);
			m_combo_obj_name.SetCurSel(0);
		}
		m_combo_obj.EnableWindow(FALSE);
		m_combo_obj_name.EnableWindow(FALSE);
	}

	CWnd* pWnd = GetDlgItem(IDC_STATIC_CON_NAME);
	if(pWnd)
	{
		CString str = "换图台: ";
		pWnd->SetWindowText(str+gpcdef.GetSectionName(m_nConid));
	}
	// TODO: Add extra initialization here
	InitDlgPosition(this);
	nChangeStatusPos = 0;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBasicChange::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnBnClickedCancel();
}

void CDlgBasicChange::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	if(gpcdef.IsNewBasicDump())
		UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgConvertBasicSchdNew);
	else
		UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgConvertBasicSchd);
	OnCancel();
}
/////////////////////////////////////////////////////////////////////////////
// CDlgBasicChange message handlers
void CDlgBasicChange::OnOK() 
{
	int nSel = m_combo_obj.GetCurSel();
	if(nSel<0)
	{
		MessageBox("请选择目标基本图","提示", MB_OK|MB_ICONWARNING);
		return;
	}
	
	m_nObjSchdType = (CTG_ScheduleInfo::SCHEDULE_TYPE)m_combo_obj.GetItemData(nSel);
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP==m_nObjSchdType)
	{
		int nSubSel = m_combo_obj_name.GetCurSel();
		if(nSubSel<0)
		{
			MessageBox("请选择归档基本图","提示", MB_OK|MB_ICONWARNING);
			return;
		}
		int nNewBasicType = GetBasicConfig()->GetNewBackupBasicType(m_nConid);
		if(m_nObjBackupSchdSubType != nNewBasicType)
		{
			CString text;
			m_combo_obj_name.GetWindowText(text);
			text = "你确实要覆盖 归档基本图\"" + text + "\"吗?";
			if(MessageBox(text,"提示", MB_YESNO|MB_ICONQUESTION)==IDNO)
				return;
		}
	}

	if(gpcdef.IsNewBasicDump())
	{
		if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV == m_nSrcSchdType)
		{
			CString str;
			m_combo_cur_name.GetWindowText(str);
			if(str.IsEmpty())
			{
				if(IDYES==MessageBox("你还没有给预览基本图命名, 你需要命名吗?", "提示", MB_YESNO|MB_ICONQUESTION))
				{
					return;
				}
			}
			else
			{
				CString tmp="请注意, 当前预览基本图命名字为:\'";
				tmp += str;
				tmp += "\', 是否使用该名字?";
				if(IDNO==MessageBox(tmp, "提示", MB_YESNO|MB_ICONQUESTION))
				{
					return;
				}
			}
		}
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

	if(gpcdef.IsNewBasicDump())
	{
		MsgConvertBasicSchdNew dump_schd;
		dump_schd.conid =m_nConid;
		if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP == m_nSrcSchdType)
			dump_schd.from_schd_type=GetBasicConfig()->GetCurSelBackupBasicType(m_nConid);
		else
		    dump_schd.from_schd_type=m_nSrcSchdType;

		if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP == m_nObjSchdType)
			dump_schd.to_schd_type=m_nObjBackupSchdSubType;
		else
		    dump_schd.to_schd_type=m_nObjSchdType;
		
		dump_schd.flag =  MsgConvertBasicSchdNew::CONVERT;
		long eid=GetSpecialEntityID(SERVER_TYPE_BASIC_TG,dump_schd.conid);
		SendTGBMsg(dump_schd.conid,&dump_schd,eid);

		sysprint(10,PMTG.ThreadId,"[基本图]:换图 conid %d flag %d from_schd_type %d to_schd_type %d eid %d", 
			dump_schd.conid, dump_schd.flag, 
			dump_schd.from_schd_type, dump_schd.to_schd_type, eid);
	}
	else
	{
		MsgConvertBasicSchd change_schd;
		change_schd.conid =	m_nConid;
		change_schd.flag =  MsgConvertBasicSchd::CONVERT; 
		long eid=GetSpecialEntityID(SERVER_TYPE_BASIC_TG,change_schd.conid);
		SendTGBMsg(change_schd.conid,&change_schd,eid);
		sysprint(10,PMTG.ThreadId,"[基本图]:换图 conid %d flag %d eid %d", 
			change_schd.conid, change_schd.flag, eid);
	}

	CWnd *pWnd = GetDlgItem(IDOK);
	pWnd->EnableWindow(FALSE);

	m_CtrlPro.SetRange(0,100);

	if(!SetTimer(201,80,NULL) )
	{
		sysprint(10,PMTG.ThreadId,"[PMTG]: create  change basic graph  timers failed");
	}
}

void CDlgBasicChange::OnTimer(UINT nIDEvent) 
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

void CDlgBasicChange::OnSelObjSchdType()
{
	m_combo_obj_name.ResetContent();
	int nSel = m_combo_obj.GetCurSel();
	if(nSel>=0)
	{
		int nObjSchdType = m_combo_obj.GetItemData(nSel);
		CString name=GetBasicConfig()->GetBasicNameByType(m_nConid, nObjSchdType);
		if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC==nObjSchdType)
		{
			if(!name.IsEmpty())
			{
				m_combo_obj_name.AddString(name);
				m_combo_obj_name.SetCurSel(0);
			}
			m_combo_obj_name.EnableWindow(FALSE);
		}
		else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==nObjSchdType)
		{
			if(!name.IsEmpty())
			{
				m_combo_obj_name.AddString(name);
				m_combo_obj_name.SetCurSel(0);
			}
			m_combo_obj_name.EnableWindow(FALSE);
		}
		else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP==nObjSchdType)
		{
			m_combo_obj_name.EnableWindow(TRUE);

			int nNewBasicType = GetBasicConfig()->GetNewBackupBasicType(m_nConid);
			if(nNewBasicType>0)
			{
				int rt=m_combo_obj_name.AddString("新建归档图");
				m_combo_obj_name.SetItemData(rt, nNewBasicType);
			}

			const SECTION_BASIC_NAME* pBasicSchd=GetBasicConfig()->GetBasicSchdNameStruct(m_nConid);
			if(pBasicSchd)
			{
				for(int i=0; i<pBasicSchd->num; i++)
				{
					name=pBasicSchd->basic_backup_schd[i].name;
					if(!name.IsEmpty())
					{
						int rt=m_combo_obj_name.AddString(name);
						m_combo_obj_name.SetItemData(rt, pBasicSchd->basic_backup_schd[i].type);
					}
				}
			}
		}
	}
}

void CDlgBasicChange::OnSelObjSchdName()
{
	m_nObjBackupSchdSubType=-1;
	int nSel = m_combo_obj.GetCurSel();
	if(nSel<0)
	{
		MessageBox("请选择目标基本图","提示", MB_OK|MB_ICONWARNING);
		return;
	}

	int nObjSchdType = m_combo_obj.GetItemData(nSel);
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP==nObjSchdType)
	{
		int nSelName = m_combo_obj_name.GetCurSel();
		if(nSelName>=0)
		{
			m_nObjBackupSchdSubType=m_combo_obj_name.GetItemData(nSelName);
		}
	}
}

