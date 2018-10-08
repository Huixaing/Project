
// SelectScheduleTrain.cpp : implementation file
//

#include "stdafx.h"
#include "oscalls.h"
#include "sysshare.h"
#include "resource.h"
#include "SelectScheduleMark.h"
#include "function.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include "tg_work_schdule_holder.h"
#include "tg_basicpara.h"
#include "gpcdef.h"
#include "tg_function_config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define IDC_SBT_MARKLIST  5201

CSelectScheduleMark::CSelectScheduleMark(CWorkScheduleHolder *pHdr,CWnd* pParent/*=NULL*/)
	: CDialog(CSelectScheduleMark::IDD, pParent)
{
	
	m_pHolder = pHdr;
	m_nCount = 0;
	m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT;
	m_bEnableChoiceSchedule=FALSE;
	intArray=NULL;
	m_check=FALSE;
}


void CSelectScheduleMark::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectScheduleMark)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectScheduleMark, CDialog)
	//{{AFX_MSG_MAP(CSelectScheduleMark)
	ON_BN_CLICKED(IDC_SBT_ACK, OnSbtAck)
	ON_BN_CLICKED(IDC_SBT_CANCEL, OnSbtCancel)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SBT_GET, OnSbtGet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectScheduleMark message handlers

BOOL CSelectScheduleMark::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CTG_ScheduleInfo schd;
	m_pHolder->GetWorkScheduleInfo(schd); 

	int i,j;
 
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
	
	if(schd.GetShiftNo()  == 2)
	{
		j=6;
	}
	else
	{
		j=18;
	}
	for(  i=0;i < 24;i++)
	{
		text.Format("%02d",(j+i)%24);
		pComboBox->AddString(text);
	}
	pComboBox->SetCurSel((nCurrentHour-j+24)%24);
	m_nStartTime = nCurrentHour;

	pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_ENDTIME);
	for(i= 0;i < 24;i++)
	{
		text.Format("%02d",(i+j)%24);
		pComboBox->AddString(text);
	}

	pComboBox->SetCurSel(((nCurrentHour+3)-j+24)%24);

	pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_STATIONLIST);

	int nStationIndexFlag[32];
	memset(nStationIndexFlag,0x00,sizeof(nStationIndexFlag));
	int nCount = 0;
	//modi by cf after 9-4

	pComboBox->AddString("全部区段");
	pComboBox->SetItemData(nCount,0);
	nCount += 1;
	
	int nStationNum,nStationList[512];
	nStationNum = sizeof(nStationList)/sizeof(nStationList[0]);
	if(GetTGFunctionConfig()->GetGPCDEFx()->GetStationList(schd.m_nSectionID,nStationNum,nStationList)>0)
	for(i=0;i<nStationNum;i++)
	{
		//if(nStationIndexFlag[(m_pDoc->m_nCurrentStationList[i] - 1)/32] & (1 << (m_pDoc->m_nCurrentStationList[i] - 1)%32))
		//	continue;
		//nStationIndexFlag[(m_pDoc->m_nCurrentStationList[i] - 1)/32] |= (1 << (m_pDoc->m_nCurrentStationList[i] - 1)%32);

		pComboBox->AddString(GetTGFunctionConfig()->GetGPCDEFx()->GetStationName(nStationList[i]));
		pComboBox->SetItemData(nCount,(DWORD)nStationList[i]);
		nCount += 1;
	}
	
	//end of cf 
	
	pComboBox->SetCurSel(0);

	
	if(m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		((CButton *)GetDlgItem(IDC_RADIO_SHIFT_SCHEDULE))->SetCheck(TRUE); 
	else
		((CButton *)GetDlgItem(IDC_RADIO_BASIC_SCHEDULE))->SetCheck(TRUE); 
	m_nCurrentSel = -1;
	
	
    if(m_bEnableChoiceSchedule)
	{
		GetDlgItem(IDC_RADIO_SHIFT_SCHEDULE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_BASIC_SCHEDULE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_RADIO_SHIFT_SCHEDULE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_BASIC_SCHEDULE)->EnableWindow(FALSE);
	}
	InitDlgPosition(this);
 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectScheduleMark::OnSbtAck() 
{
	sysprint(0,PMTG.ThreadId,"调度员生成图定符号,确定生成, BEGIN");
	for(int i=0;i<m_nCount;i++)
	{
		if(m_grid.IsRowSelected(info[i].row_index))
			info[i].checked =TRUE;
		else
			info[i].checked =FALSE;
	}
	if(((CButton *)GetDlgItem(IDC_CHECK_FORCE))->GetCheck()>0)
		m_check=TRUE;
	else
		m_check=FALSE;
	CDialog::OnOK( );	
}

void CSelectScheduleMark::OnSbtCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel( );
}

void CSelectScheduleMark::OnDestroy() 
{
	CDialog::OnDestroy();

	if(intArray)
		delete[] intArray;
	
}

void CSelectScheduleMark::OnSbtGet() 
{
    //get selected tarin  conditions
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_STATIONLIST);
	int nSel = pComboBox->GetCurSel( );
	if(nSel == CB_ERR)
		return;
	int nStationNo = pComboBox->GetItemData(nSel);

	m_grid.DeleteNonFixedRows(); 
	
    m_nCount = 0;
	
	pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_STARTTIME);
    CString time_str;
    pComboBox->GetWindowText (time_str);//GetCurSel( );
	int nStartTime = atoi(time_str);
	m_nStartTime = nStartTime;
//	int nStartTime = pComboBox->GetCurSel( );
	pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_ENDTIME);
    pComboBox->GetWindowText (time_str);//GetCurSel( );
	int nEndTime = atoi(time_str);
 
 
	if(((CButton *)GetDlgItem(IDC_RADIO_SHIFT_SCHEDULE))->GetCheck()==1)
        m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT;
	else
		m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;

	if(m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC) // 基本图没有日期
	{
	
	}
	else // 班计划有日期，计算小时
	{
		CTime curtm=CTime::GetCurrentTime();
		CTime todaytm=CTime(curtm.GetYear(),curtm.GetMonth(),curtm.GetDay(),0,0,0);
		long  todayhour=todaytm.GetTime()/3600;
		long  curhour=curtm.GetTime()/3600;
		nStartTime+=todayhour;
		nEndTime+=todayhour;
		if(nStartTime>curhour+12) nStartTime-=24;
		else if(nStartTime<curhour-6) nStartTime+=24;
		if(nStartTime>curhour+12 || nStartTime<curhour-6)
		{
			MessageBox("时间范围不合理,起始时间应该在当前时间附近!","提示");
			return;
		}
		if(nEndTime<nStartTime)
		    nEndTime+=24;
        if(nEndTime<nStartTime || nEndTime>nStartTime+12)
		{
			MessageBox("时间范围不能超过12小时!","提示");
			return;
		}

		/*
		CTime stt,ett;
		stt=nStartTime*3600;
		ett=nEndTime*3600;
		CString timarange=stt.Format("选择的时间范围是: %Y-%m-%d %H:%M:%S")+"--";
		timarange+=ett.Format("%Y-%m-%d %H:%M:%S");
		MessageBox(timarange,"提示");
		*/
	}
	
	CTG_TrainScheduleBase *pwork_schd=m_pHolder->GetWorkScheduleData();
	CTG_TrainScheduleBase *pbasic_schd=m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC? m_pHolder->GetBasicScheduleData():m_pHolder->GetShiftScheduleData();
	CTG_ObjectPtrGroup group;
	pbasic_schd->FindMarks(group, nStartTime*3600,nEndTime*3600);
	int i;
	for(i=0;i<group.GetCount();i++) 
   	{
		CTG_SingleMark *pmark=(CTG_SingleMark *)group.GetPtr(i);
		if(!pmark)  // not possi
			break;
		info[i].mark_index =pmark->GetBlockIndex();
		info[i].mark_type =pmark->GetMarkType(); 
		int row=m_grid.InsertMark(pmark->GetMarkID(),pmark);
		if(pwork_schd->GetSingleMarkByShiftString(pmark->GetShiftIndexString()))
		{
			m_grid.GetCell(row,0)->SetBackClr(ROW_COLOR_LEVEL_1); 
			info[i].exist=1; 
			info[i].checked =FALSE;
			m_grid.SetRowSelected(row,FALSE);
		}
		else
		{
			m_grid.GetCell(row,0)->SetBackClr(ROW_COLOR_LEVEL_2); 
			info[i].exist=0; 
			info[i].checked =TRUE;
			m_grid.SetRowSelected(row,FALSE);
		}
		info[i].row_index=row; 
	}
	m_grid.AutoSizeColumns(); 
	m_grid.Refresh(); 
	m_nCount=i;

  //  sysprint(0,PMTG.ThreadId,"调度员生成图定计划/读取列表, 车种:%d,运行类型:%d,方向:%d",nTrainClass,nStartType,nDirFlag);
}

