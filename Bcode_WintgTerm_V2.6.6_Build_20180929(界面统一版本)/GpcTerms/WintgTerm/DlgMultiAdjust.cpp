// DlgRestoreTrainList.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "tg_schedule_base.h"
#include "function.h"
#include "DlgMultiAdjust.h"
#define IDC_SBT_MARKLIST  5201

// CDlgMultiAdjust 对话框

IMPLEMENT_DYNAMIC(CDlgMultiAdjust, CDialog)
CDlgMultiAdjust::CDlgMultiAdjust(CTG_TrainScheduleBase *psourcebase,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMultiAdjust::IDD, pParent)
{
	m_bAllSelected=FALSE;
	m_pSourceDataBase=psourcebase;
	CTG_ScheduleInfo schd;
	m_pSourceDataBase->GetScheduleInfo(schd);
}

CDlgMultiAdjust::~CDlgMultiAdjust()
{
}

void CDlgMultiAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMultiAdjust, CDialog)
	ON_BN_CLICKED(IDC_BTN_QUERY, OnBnClickedBtnQuery)
	ON_BN_CLICKED(IDC_CHK_ALL, OnBnClickedChkAll)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgMultiAdjust 消息处理程序

void CDlgMultiAdjust::OnBnClickedBtnQuery()
{
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_STARTTIME);
    CString time_str;
    pComboBox->GetWindowText (time_str);//GetCurSel( );
	int nStartTime = atoi(time_str);
	pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_ENDTIME);
    pComboBox->GetWindowText (time_str);//GetCurSel( );
	int nEndTime = atoi(time_str);
 

    time_t  sttm=ConvertToLongTime(nStartTime,0,0,time(NULL),12,12); 
	time_t  endtm=ConvertToLongTime(nEndTime,0,0,time(NULL),12,12); 

	
	CTG_TrainPtrGroup group;

	m_pSourceDataBase->FindTrains(group,NULL_STATION,sttm,endtm,FALSE,FALSE);

	for(int i=0;i<group.GetTrainCount();i++)
	{
		CString cap;
		int row;
		CTG_SingleTrain *ptrain=group.GetTrainPtr(i);
		if(!ptrain)
			continue;
		if(ptrain->IsDeletedObject() || ptrain->GetTrainRecordCount()<=0)
			continue;
		CString operstr;
		cap.Format("%d", ptrain->GetTrainIndex()); 
		row=m_grid.InsertTrain(cap,operstr,ptrain); 
		m_grid.GetCell(row,1)->SetData(ptrain->GetTrainIndex()); 
		if(ptrain->GetPriority()<4 || ptrain->GetSpaceRank()>0 || ptrain->GetMilitaryRank()>0)  // 默认旅客列车和行包车不调整
			m_grid.SetRowSelected(row,FALSE); 
		else
			m_grid.SetRowSelected(row,TRUE); 
	}
	m_grid.AutoSizeColumns();
	m_grid.Refresh();
}

void CDlgMultiAdjust::OnBnClickedChkAll()
{
	if(m_bAllSelected)
		m_bAllSelected=FALSE;
	else
		m_bAllSelected=TRUE;	
}

int  CDlgMultiAdjust::GetSelectedTrains(TRAIN_INDEX  buffer[],int buffersize)
{
	int rc=0;
	for(int row=1;row<m_grid.GetRowCount();row++)
	{
		if(!m_grid.IsRowSelected(row))
			continue;
		TRAIN_INDEX trainindex=m_grid.GetCell(row,1)->GetData();
		if(rc<buffersize)
			buffer[rc++]=trainindex;
	}
	return rc;
}

void CDlgMultiAdjust::OnBnClickedOk()
{
	OnOK();
}

BOOL CDlgMultiAdjust::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CRect rect;
	GetDlgItem(IDC_GRID_FRAME)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_grid.m_bRowSelectable=TRUE;
    m_grid.CreateGrid(this,rect,IDC_SBT_MARKLIST);
	m_grid.SetEditable(TRUE);

	
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_STARTTIME);
	CString text;
	CTime ctm=CTime::GetCurrentTime();
	int nCurrentHour = ctm.GetHour();
	int i;
	for(  i=0;i < 24;i++)
	{
		text.Format("%02d",i);
		pComboBox->AddString(text);
	}
	pComboBox->SetCurSel(nCurrentHour-1);
	pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_ENDTIME);
	for(i= 0;i < 24;i++)
	{
		text.Format("%02d",i);
		pComboBox->AddString(text);
	}

	pComboBox->SetCurSel((nCurrentHour+3)%24);

    return TRUE;
}