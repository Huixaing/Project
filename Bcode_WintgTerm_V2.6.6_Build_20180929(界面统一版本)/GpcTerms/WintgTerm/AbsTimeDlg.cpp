// AbsTimeDlg.cpp : implementation file
//

#include "stdafx.h"

#include "bureau.h"
#include "AbsTimeDlg.h"
#include "function.h"
#include "tgmsg.h"
#include "messages.h"
#include "memshare.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include "db_msgproc.h"
#include "tg_basicpara.h"
#include ".\abstimedlg.h"
#include "AbsTimeEditDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PRIO_DEF ("prio_def.def")

#define TRAIN_TYPE  ("TRAIN_TYPE")
#define ABS_TYPE    ("ABS_TYPE")
#define PRIO_DATA  ("RUN_PRIORITY")

/////////////////////////////////////////////////////////////////////////////
// AbsTimeDlg dialog


CAbsTimeDlg::CAbsTimeDlg(int con_id,CWnd* pParent /*=NULL*/) 
: CDialog(CAbsTimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AbsTimeDlg)
	//}}AFX_DATA_INIT
	m_nSectionID=con_id;
}

CAbsTimeDlg::~CAbsTimeDlg()
{
}


void CAbsTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsTimeDlg)
	DDX_Control(pDX, IDC_CMB_STATION, m_cmbStation);
	DDX_Control(pDX, IDC_LIST5, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbsTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CAbsTimeDlg)
	ON_CBN_SELCHANGE(IDC_CMB_STATION, OnSelchangeCmbStation)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_RESTORE_ORIGDATA, OnBnClickedRestoreOrigdata)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST5, OnModifyAbsTime)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsTimeDlg message handlers
BOOL CAbsTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(dwStyle);

	m_list.InsertColumn(0, "序号",LVCFMT_CENTER,40);
	m_list.InsertColumn(1, "区间名称",LVCFMT_CENTER,200);
	m_list.InsertColumn(2, "区间编号",LVCFMT_CENTER,80);
	
	m_list.InsertColumn(3, "方向",LVCFMT_CENTER,60);
	m_list.InsertColumn(4, "区间时分类型",LVCFMT_LEFT,120);
	m_list.InsertColumn(5, "停车附加时分",LVCFMT_CENTER,90);
	m_list.InsertColumn(6, "起车附加时分",LVCFMT_CENTER,90);
	m_list.InsertColumn(7, "区间运行时分",LVCFMT_CENTER,90);

	nIndex = 0;
	m_nStationCount=256;
	memset(m_nCurrentStationList, 0, sizeof(m_nCurrentStationList));
	if(gpcdef.GetStationList(m_nSectionID,m_nStationCount,m_nCurrentStationList)<=0)
	{
		MessageBox("配置错误,无法获取车站列表", "错误", MB_ICONHAND);
		return FALSE;
	}


	for(int i=0;i < m_nStationCount;i++)
	{
		char* name = gpcdef.GetStationName(m_nCurrentStationList[i]);
		if (NULL == name)
		{
			CString stmp;
			stmp.Format("配置错误,无法获取%d车站名称", m_nCurrentStationList[i]);
			MessageBox(stmp, "错误", MB_ICONHAND);
			continue;
		}
		m_cmbStation.InsertString(i,name);
	}
	
	m_cmbStation.SetCurSel(-1);
	m_nCurStation = -1;

	InitDlgPosition(this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

CString CAbsTimeDlg::GetTrainTypeStr(int nType)
{
	CString ct;
	const char* p=GetTGBasicParam()->GetAbsTimeTypeNameById(nType);
	if(p==NULL)
	{
		p = GetTGBasicParam()->GetTrainTypeNameById(nType);
		if (p==NULL)
			ct.Format("类型%d", nType);
		else
			ct=p;
	}
	else
		ct=p;
	
	return ct;
}

ushort CAbsTimeDlg::ConvertStrToInt(CString str)
{
	int n = str.Find(":");
	if(n == -1)
		return (ushort) atoi(str)*60;
	else
	{
		CString left = str.Left(n);
		CString right = str.Right(str.GetLength() - n -1);
		ushort temp = (ushort) atoi(left)*60 + atoi(right);
		return temp;
	}
}

void CAbsTimeDlg::OnSelchangeCmbStation() 
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_cmbStation.GetCurSel();
	if(nCurSel >= m_nStationCount)
		m_nCurStation = -1;
	else
		m_nCurStation = m_nCurrentStationList[nCurSel];
	ShowRecord();
}

void CAbsTimeDlg::ShowRecord()
{
	m_list.DeleteAllItems();

	map<long, AbsTime>& tmpAbsTime = GetTGBasicParam()->m_aAbsTimes;
	map<long, AbsTime>::iterator iter = tmpAbsTime.begin();
	
	int iItem = 0;
	for (; iter != tmpAbsTime.end(); iter++)
	{   
		for(int i=0; i<iter->second.trainAbsTime.size(); i++)
		{
			TrainAbsTime *ptr=&iter->second.trainAbsTime[i];
			if(NULL == ptr)
				continue;

			if((m_nCurStation!=-1) && (ptr->station1!=m_nCurStation) &&  (ptr->station2!=m_nCurStation))
				continue;
			if(ptr->station1 == 65535 && ptr->station2 == 65535)
				continue;
		
			iItem++;
			CString text;
			// 序号
			text.Format("%d",  m_list.GetItemCount()); 
			int nRow=m_list.InsertItem(iItem, text);

			// 区间名称
			text.Format("%s",  GetTGBasicParam()->GetAbsName(ptr->abs_number)); 
			m_list.SetItemText(nRow,1, text);

			// 区间编号
			text.Format("%d",  ptr->abs_number); 
			m_list.SetItemText(nRow,2, text);

			// 方向
			if(ptr->direction == 0)
				text="上行";
			else if(ptr->direction == 1)
				text="下行";
			else
				text="单线";
			m_list.SetItemText(nRow,3, text);
			
			// 列车类型
			text.Format("%s", GetTrainTypeStr(ptr->type)); 
			m_list.SetItemText(nRow,4, text);

			// 停车附加时分
			text.Format("%d:%d", ptr->stop_time/60,ptr->stop_time%60);
			m_list.SetItemText(nRow,5, text);

			// 启车附加时分
			text.Format("%d:%d", ptr->start_time/60,ptr->start_time%60);
			m_list.SetItemText(nRow,6, text);

			// 区间运行时分
			text.Format("%d:%d", ptr->run_time/60, ptr->run_time%60);
			m_list.SetItemText(nRow,7, text);

			m_list.SetItemData(nRow, ptr->direction+ptr->type*100);
		}
	}
}

void CAbsTimeDlg::OnOK() 
{
	CDialog::OnOK();
}

void CAbsTimeDlg::OnModifyAbsTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i=0; i<m_list.GetItemCount(); i++)
	{
		if(m_list.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			CString text;
			int start_tm=0, stop_tm=0, run_tm=0;
			
			text = m_list.GetItemText(i,2);
			int abs_no=atoi(text);
			long key = m_list.GetItemData(i); // ptr->direction+ptr->type*100
			int direction=key%100;
			int type=key/100;

			// 停车附加时分
			text = m_list.GetItemText(i,5);
			stop_tm=ConvertStrToInt(text);
			
			// 启车附加时分
			text = m_list.GetItemText(i,6);
			start_tm=ConvertStrToInt(text);
			
			// 区间运行时分
			text = m_list.GetItemText(i,7);
			run_tm=ConvertStrToInt(text);
			
			CAbsTimeEditDlg dlg(this);
			dlg.m_start_tm = start_tm;
			dlg.m_stop_tm =stop_tm;
			dlg.m_run_tm = run_tm;

			if(dlg.DoModal() == IDOK)
			{
				if(dlg.m_start_tm != start_tm || dlg.m_stop_tm !=stop_tm
					|| dlg.m_run_tm != run_tm)
				{
					TrainAbsTime tm;
					
					tm.abs_number= abs_no;
					tm.type = type;
					tm.direction = direction;
					
					tm.stop_time = dlg.m_stop_tm;
					tm.start_time = dlg.m_start_tm;
					tm.run_time = dlg.m_run_tm;            
						
					if(GetTGBasicParam()->SetAbsTimeData(tm))
					{
						GpcLogTrainMessage(0,0,"修改区间运行时分: abs_number %d direction %d type %d sta:%d-%d",tm.abs_number,
							tm.direction, tm.type, tm.station1,tm.station2);
						GetTGBasicParam()->SaveUserAbsTimeData();
						ShowRecord();
					}
				}
			}
			break;
		}
	}

	*pResult = 0;
}

void CAbsTimeDlg::OnBnClickedRestoreOrigdata()
{
	for(int i=0; i<m_list.GetItemCount(); i++)
	{
		CString text = m_list.GetItemText(i,2);
		int abs_no=atoi(text);
		long key = m_list.GetItemData(i); // ptr->direction+ptr->type*100
		int direction=key%100;
		int type=key/100;

		TrainAbsTime *ptr=GetTGBasicParam()->GetAbsTimePtr(abs_no, type, direction);		
		if(ptr)
		{
			ptr->start_time=ptr->orig_start_time;
			ptr->stop_time=ptr->orig_stop_time;
			ptr->run_time=ptr->orig_run_time;
			GpcLogTrainMessage(0,0,"恢复区间运行时分: %d-%d",ptr->station1,ptr->station2);
		}
	}
	GetTGBasicParam()->SaveUserAbsTimeData();
	ShowRecord();
}
