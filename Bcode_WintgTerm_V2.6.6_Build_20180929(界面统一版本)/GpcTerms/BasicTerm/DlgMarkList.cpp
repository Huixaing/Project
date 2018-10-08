// DlgTrainList.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgMarkList.h"
#include "tg_schedule_base.h"
#include "tg_work_schdule_holder.h"
#define IDC_SBT_TRAINLIST  5211
// CDlgMarkList 对话框

IMPLEMENT_DYNAMIC(CDlgMarkList, CDialog)
CDlgMarkList::CDlgMarkList(const char *caption,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMarkList::IDD, pParent)
{
	m_pHolder=NULL;
	m_nScheduleType=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT; 
	m_nCurrentSel=-1;
	m_pData=NULL;
	m_szCaption=caption;
}

CDlgMarkList::~CDlgMarkList()
{
}

void CDlgMarkList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMarkList, CDialog)
	ON_NOTIFY(GVN_SELCHANGING,IDC_SBT_TRAINLIST,OnGridSelectChange)
END_MESSAGE_MAP()

BOOL CDlgMarkList::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(m_szCaption);
	CRect rect;
	GetDlgItem(IDC_GRID_FRAME)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_grid.m_bRowSelectable=TRUE;
    m_grid.CreateGrid(this,rect,IDC_SBT_TRAINLIST);
	m_grid.SetEditable(TRUE);

	CTG_TrainScheduleBase *pdata;
	if(m_nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		m_pData=m_pHolder->GetShiftScheduleData();
		CTG_ScheduleInfo info;
		m_pData->GetScheduleInfo(info);
		TIME start,end;
		info.GetShiftManagerRange(start,end); 
		CTG_ObjectPtrGroup group;
		m_pData->FindMarks(group,start,end);
		for(int i=0;i<group.GetCount();i++)
		{
			CTG_SingleMark *pmark=(CTG_SingleMark *)group.GetPtr(i); 
			int row=m_grid.InsertMark(pmark->GetMarkID(),pmark);  
			m_grid.GetCell(row,1)->SetData(pmark->GetBlockIndex()); 
		}	
	}
	return TRUE;
}


void CDlgMarkList::OnOK()
{
	if(!m_pData)
		return;
	if(m_nCurrentSel>0)
	{
		TRAIN_INDEX train_index=m_grid.GetCell(m_nCurrentSel,1)->GetData(); 
		CTG_SingleMark *pmark=m_pData->GetSingleMark(train_index);
		if(!pmark)
			return;
		CString txtstr;
		GetDlgItem(IDC_ED_NOTE)->GetWindowText(txtstr);
		//pmark->SetImportInfo(txtstr);
		//pmark->SetSyncStatusX(SYCFLAG_NULL,TRUE); 
		GetDlgItem(IDC_ED_NOTE)->EnableWindow(FALSE); 
	}
}
// CDlgMarkList 消息处理程序

void CDlgMarkList::OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult) 
 {
	 /*
	 NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	 if(pv->iRow>=1)
	 {
		 if(m_nCurrentSel!=pv->iRow)
		 {
			GetDlgItem(IDC_ED_NOTE)->EnableWindow(TRUE); 
			m_nCurrentSel=pv->iRow;
			TRAIN_INDEX train_index=m_grid.GetCell(m_nCurrentSel,1)->GetData(); 
			CTG_SingleTrain *ptrain=m_pData->GetSingleTrain(train_index);
			CString txtstr;
			if(ptrain)
			{
				txtstr=ptrain->GetImportInfo(); 
			}
			GetDlgItem(IDC_ED_NOTE)->SetWindowText(txtstr);
		 }
	 }*/
 }
