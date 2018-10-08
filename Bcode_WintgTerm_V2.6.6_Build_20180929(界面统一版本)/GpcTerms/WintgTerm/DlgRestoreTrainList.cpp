// DlgRestoreTrainList.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgRestoreTrainList.h"
#include "tg_filebase.h"
#include "function.h"
#define IDC_SBT_MARKLIST  5201
#define IDC_SBT_MARKLIST2  5202

#include "tgpspdata.h"

static PropertyInfoPara g_train_list_def[]=
{
	{0,ID_PD_TRAIN_SHIFT_INDEX,CTC_DATA_INT,"GTID","列车索引"},
	{0,ID_PD_TRAIN_ID,CTC_DATA_STR,"TNO","车次号"},
	{1,ID_PD_TRAIN_OPER,CTC_DATA_STR,"oper_id","操作"},
	{0,ID_PD_TRAIN_FIRST_STA_NAME,CTC_DATA_STR,"LSTST","首站"},
	{0,ID_PD_TRAIN_IN_SECTION_TIME,CTC_DATA_DATETIME,"LSTTM","本台开始"},
	{0,ID_PD_TRAIN_ROUTE_NAME,CTC_DATA_STR,"LEDTM","径路"},
};

int count_of_train_list_def()
{
	return sizeof(g_train_list_def)/sizeof(g_train_list_def[0]);
}

BOOL CTrainInfoGrid::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	return CXMLDataGrid::CreateGrid(parent,rect,ctrlid,g_train_list_def,count_of_train_list_def());
}

int CTrainInfoGrid::InsertTrain(const char *id, CString& operstr, const CTG_SingleTrain *ptrain)
{
	int row=CXMLDataGrid::Insert1EmptyRow(id);
	for(int i=1;i<m_nFieldNum;i++)
	{
		if(m_FiledInfo[i].id==ID_PD_TRAIN_OPER)
		{
			GetCell(row,i)->SetText(operstr);
			if(ptrain->m_nTipToDdy!=0)// 不为0表示有提示标志,设置浅红色背景
			{
				GetCell(row,0)->SetBackClr(RGB(255,100,164));
			}
		}
		else
		{
			const char *strvalue=TGPSP_GetItemValue(ptrain,m_FiledInfo[i].id,0);
			int   intvalue=TGPSP_GetItemIntValue(ptrain,m_FiledInfo[i].id,0);
			if(!strvalue)
			{
				if(intvalue!=-1)
					strvalue=GetCTCValueDispString(m_FiledInfo[i].data_type,intvalue,0);
			}
			if(strvalue)		
			{
				GetCell(row,i)->SetText(strvalue);
			}
		}
	}
	return row;
}

// CDlgRestoreTrainList 对话框

IMPLEMENT_DYNAMIC(CDlgRestoreTrainList, CDialog)
CDlgRestoreTrainList::CDlgRestoreTrainList(CTG_TrainScheduleBase *psourcebase,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRestoreTrainList::IDD, pParent)
{
	m_bAllSelected=FALSE;
	m_pSourceDataBase=psourcebase;
	CTG_ScheduleInfo schd;
	m_pSourceDataBase->GetScheduleInfo(schd);
	m_trainbase.SetScheduleInfo(schd);
}

CDlgRestoreTrainList::~CDlgRestoreTrainList()
{
}

void CDlgRestoreTrainList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRestoreTrainList, CDialog)
	ON_BN_CLICKED(IDC_BTN_QUERY, OnBnClickedBtnQuery)
	ON_BN_CLICKED(IDC_CHK_ALL, OnBnClickedChkAll)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgRestoreTrainList 消息处理程序

void CDlgRestoreTrainList::OnBnClickedBtnQuery()
{
	for (int i=m_grid.GetRowCount()-1;i>0;i--)
	{
		m_grid.DeleteRow(i);
	}
	CTG_ScheduleInfo info;
	m_trainbase.DeleteAllData(info,SYCFLAG_NULL);

	CString trainno;
	GetDlgItem(IDC_ED_TRAINNO)->GetWindowText(trainno);

	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_STARTTIME);
    CString time_str;
    pComboBox->GetWindowText (time_str);//GetCurSel( );
	int nStartTime = atoi(time_str);
	pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_ENDTIME);
    pComboBox->GetWindowText (time_str);//GetCurSel( );
	int nEndTime = atoi(time_str);
 

    time_t  sttm=ConvertToLongTime(nStartTime,0,0,time(NULL),12,12); 
	time_t  endtm=ConvertToLongTime(nEndTime,0,0,time(NULL),12,12); 

	if(m_bAllSelected)
		TG_LoadFromFiles(&m_trainbase,0,0,NULL);
	else
        TG_LoadFromFiles(&m_trainbase,sttm,endtm,trainno);

	CTG_TrainPtrGroup group;

	m_trainbase.FindAllTrains(group);

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
		CTG_SingleTrain *prev=m_pSourceDataBase->GetSingleTrain(ptrain->GetTrainIndex());  // 
	    if(prev)
			operstr="修改";
		else
			operstr="增加";
		cap.Format("%d", ptrain->GetTrainIndex()); 
		row=m_grid.InsertTrain(cap,operstr,ptrain); 
		m_grid.GetCell(row,1)->SetData(ptrain->GetTrainIndex()); 
		if(prev)
			m_grid.SetRowSelected(row,FALSE); 
		else
			m_grid.SetRowSelected(row,TRUE); 
	}
	m_grid.AutoSizeColumns();
	m_grid.Refresh();

	CTG_ObjectPtrGroup markgroup;
	m_trainbase.FindMarks(markgroup);

	for(int i=0;i<markgroup.GetCount();i++)
	{
		CString cap;
		int row;
		CTG_SingleMark *pmark=(CTG_SingleMark *)markgroup.GetPtr(i);
		if(!pmark)
			continue;
		if(pmark->IsDeletedObject())
			continue;
		CString operstr;
		CTG_SingleMark *prev=m_pSourceDataBase->GetSingleMark(pmark->GetBlockIndex());  // 
	    if(prev)
			operstr="修改";
		else
			operstr="增加";
		cap.Format("%d", pmark->GetBlockIndex()); 
		row=m_mark_grid.InsertMark(cap,pmark); 
		m_mark_grid.GetCell(row,1)->SetData(pmark->GetBlockIndex()); 
		if(prev)
			m_mark_grid.SetRowSelected(row,FALSE); 
		else
			m_mark_grid.SetRowSelected(row,TRUE); 
	}
	m_mark_grid.AutoSizeColumns();
	m_mark_grid.Refresh();

}

void CDlgRestoreTrainList::OnBnClickedChkAll()
{
	if(m_bAllSelected)
		m_bAllSelected=FALSE;
	else
		m_bAllSelected=TRUE;	
	if(m_bAllSelected)
	{
		GetDlgItem(IDC_ED_TRAINNO)->SetWindowText("");
		GetDlgItem(IDC_ED_TRAINNO)->EnableWindow(FALSE);
		GetDlgItem(IDC_SBT_STARTTIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SBT_ENDTIME)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_ED_TRAINNO)->EnableWindow(TRUE);
		GetDlgItem(IDC_SBT_STARTTIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_SBT_ENDTIME)->EnableWindow(TRUE);
	}
}

void CDlgRestoreTrainList::OnBnClickedOk()
{
	CTG_ScheduleInfo schd;
	m_pSourceDataBase->GetScheduleInfo(schd);
	for(int row=1;row<m_grid.GetRowCount();row++)
	{
		if(!m_grid.IsRowSelected(row))
			continue;
		TRAIN_INDEX trainindex=m_grid.GetCell(row,1)->GetData();
		CTG_SingleTrain *ptrain=m_trainbase.GetSingleTrain(trainindex);
		if(ptrain)
		{

			CTG_SingleTrain newtrain;
			newtrain=*ptrain;
			CTG_SingleTrain *prev=m_pSourceDataBase->GetSingleTrain(newtrain.GetTrainIndex());  // 
			if(prev)
				m_pSourceDataBase->UpdateTrain(schd,newtrain,SYCFLAG_NULL);
			else
				m_pSourceDataBase->AppendTrain(schd,newtrain,SYCFLAG_NULL);
		}
	}

	for(int row=1;row<m_mark_grid.GetRowCount();row++)
	{
		if(!m_mark_grid.IsRowSelected(row))
			continue;
		TRAIN_INDEX trainindex=m_mark_grid.GetCell(row,1)->GetData();
		CTG_SingleMark *pmark=m_trainbase.GetSingleMark(trainindex);
		if(pmark)
		{

			CTG_SingleMark newmark;
			newmark=*pmark;
			CTG_SingleMark *prev=m_pSourceDataBase->GetSingleMark(newmark.GetBlockIndex());  // 
			if(prev)
				m_pSourceDataBase->UpdateMark(schd,newmark,SYCFLAG_NULL);
			else
				m_pSourceDataBase->AppendMark(schd,newmark,SYCFLAG_NULL);
		}
	}
	OnOK();
}

BOOL CDlgRestoreTrainList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CRect rect;
	GetDlgItem(IDC_GRID_FRAME)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_grid.m_bRowSelectable=TRUE;
    m_grid.CreateGrid(this,rect,IDC_SBT_MARKLIST);
	m_grid.SetEditable(TRUE);

	GetDlgItem(IDC_GRID_FRAME2)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_mark_grid.m_bRowSelectable=TRUE;
    m_mark_grid.CreateGrid(this,rect,IDC_SBT_MARKLIST2);
	m_mark_grid.SetEditable(TRUE);

	
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

	CTG_ScheduleInfo schd;
	m_trainbase.GetScheduleInfo(schd);
	if(IS_BASIC_SCHD(schd.m_nType))  // 2010.2.26 对于基本图总是恢复所有
	{
		OnBnClickedChkAll();
		GetDlgItem(IDC_CHK_ALL)->EnableWindow(FALSE);
	}
	return TRUE;
}