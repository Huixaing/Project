// SendStationPlanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SendStationPlanDlg.h"
#include "GridCtrl_src/GridCellCheck.h"

#include "function.h"
#include "tgmsg.h"
#include "messages.h"
#include "memshare.h"
 
extern WriteMessage(PROCESSID usProcessId,ushort usQueueHandle,
		ushort count,char *buffer);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CSendStationPlanDlg dialog


CSendStationPlanDlg::CSendStationPlanDlg(CWintgDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CSendStationPlanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendStationPlanDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
 	m_pDoc = pDoc;
 	m_nFixCols = 0;
	m_nFixRows = 0;
	m_nCols = 7;
 	nUnitNum =0;
    m_pGridCtrl = NULL;
    bAllStationFlag = TRUE;
}


void CSendStationPlanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendStationPlanDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendStationPlanDlg, CDialog)
	//{{AFX_MSG_MAP(CSendStationPlanDlg)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_GRID, OnGrid)
	ON_MESSAGE(UM_STATION_RECV_STATUS_MSG,ChangeStationReceiveStatus)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendStationPlanDlg message handlers
#define ITEMNUM 7

BOOL CSendStationPlanDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWnd *pWnd = GetDlgItem(IDC_START_TIME);
	CRect rc;
	if(!pWnd) return TRUE;
	pWnd->GetWindowRect(rc);
	pWnd->DestroyWindow(); //pWnd was just a placeholder;
	ScreenToClient(rc);
	m_StartDateTime.Create(WS_CHILD|WS_VISIBLE|DTS_UPDOWN,rc,this,IDC_START_TIME);
	m_StartDateTime.ModifyStyle(0,DTS_TIMEFORMAT,SWP_NOSIZE);
  
    pWnd = GetDlgItem(IDC_END_TIME);
 	if(!pWnd) return TRUE;
	pWnd->GetWindowRect(rc);
	pWnd->DestroyWindow(); //pWnd was just a placeholder;
	ScreenToClient(rc);
	m_EndDateTime.Create(WS_CHILD|WS_VISIBLE|DTS_UPDOWN,rc,this,IDC_END_TIME);
	m_EndDateTime.ModifyStyle(0,DTS_TIMEFORMAT,SWP_NOSIZE);
 
	int nValue;
	if(m_pDoc->m_usCurrentTime + 3*3600 > WHOLE_DAY_SECOND)
		 nValue = long_time(OsGetNextDate(PMTG.ThreadId,m_pDoc->m_usCurrentDate),m_pDoc->m_usCurrentTime + 3*3600 - WHOLE_DAY_SECOND);
    else
		 nValue = long_time(m_pDoc->m_usCurrentDate,m_pDoc->m_usCurrentTime + 3*3600);
    CTime ctValue = CTime(nValue);
    m_EndDateTime.SetTime(&ctValue);


	if (m_pGridCtrl == NULL)
	{
		// Create the Gridctrl object
		m_pGridCtrl = new CGridCtrl;
		if (!m_pGridCtrl) return FALSE;

		// Create the Gridctrl window
     	CStatic *pWnd;
     	pWnd = (CStatic *)GetDlgItem(IDC_GRID);
		CRect rect;
		pWnd->GetClientRect(rect);
 
		m_pGridCtrl->Create(rect, this, IDC_GRID);

		// fill it up with stuff
		m_pGridCtrl->SetEditable(FALSE);
		m_pGridCtrl->EnableDragAndDrop(FALSE);
        m_nRows =  m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].nStationNum +1;

		try {
  	    	m_pGridCtrl->SetRowCount(m_nRows);
			m_pGridCtrl->SetColumnCount(m_nCols);
			m_pGridCtrl->SetFixedRowCount(m_nFixRows);
			m_pGridCtrl->SetFixedColumnCount(m_nFixCols);
		}
		catch (CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
			return FALSE;
		}
	}	
	
	TCHAR rgtsz[ITEMNUM][20] = {_T("车站"),_T("上次下达"),_T("开始时间"),_T("结束时间"), _T("是否接收"),_T("是否签收"),_T("签收人")};
 
 
	LV_ITEM			lvitem;
	LV_COLUMN		lvcolumn;
	CRect rect;
 
    DWORD dwTextStyle = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;

    int row,col;

 	// fill rows/cols with text
	for ( row = 0; row <= m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].nStationNum; row++)
		for ( col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
//			if (row < m_nFixRows)
 			if (row == 0)
           {
 				Item.nFormat = DT_RIGHT|DT_WORDBREAK|DT_NOPREFIX;
 				Item.strText.Format(rgtsz[col]);
				m_pGridCtrl->SetItemBkColour(row,col,CLR_GRAY);
 				if(col == 0){
                 	m_pGridCtrl->SetCellType(row,col, RUNTIME_CLASS(CGridCellCheck));
				    
				}
			}
            else{ 
            
				switch(col){
				case 0:
					Item.nFormat = dwTextStyle;
      		 		Item.strText.Format(_T("%s"),
      					  gpcdef.GetStationName(m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan [row-1].nStation));
             		m_pGridCtrl->SetCellType(row,col, RUNTIME_CLASS(CGridCellCheck));
//					m_pGridCtrl->SetItemBkColour(row,col,CLR_INDIANRED);
					m_pGridCtrl->SetItemFgColour (row,col,CLR_BLUE);
					break;
				case 1:
					if(!m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usSendTime)
						break;
			 		Item.nFormat = dwTextStyle;
			 		Item.strText.Format(_T("%s"),
						OsConvertTimetoString(m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usSendTime));
 			        break;
				case 2:
					if(!m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usStartTime)
						break;
			 		Item.nFormat = dwTextStyle;
			 		Item.strText.Format(_T("%s"),
						OsConvertTimetoString(m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usStartTime));
 					break;
				case 3:
					if(!m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usEndTime)
						break;
			 		Item.nFormat = dwTextStyle;
			 		Item.strText.Format(_T("%s"),
						OsConvertTimetoString(m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usEndTime));
 					break;
				case 4:
			 		Item.nFormat = dwTextStyle;
                    if(m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].bReceiveFlag ){
		    	 		Item.strText = "是";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_GREEN);
//                        Item.crFgClr = CLR_GREEN;
					}else{
		    	 		Item.strText = "否";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_INDIANRED);
//						Item.crFgClr = CLR_INDIANRED;
					}
					break;
				case 5:
			 		Item.nFormat = dwTextStyle;
                    if(m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].bSignInFlag ){
		    	 		Item.strText = "是";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_GREEN);
					}else{
		    	 		Item.strText = "否";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_INDIANRED);
					}
 					break;
				case 6:
			 		Item.nFormat = dwTextStyle;
                    Item.strText = m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].strSignName;

				default:
			 		Item.nFormat = dwTextStyle;
					break;
				}			
			}
			m_pGridCtrl->SetItem(&Item);

		}
//  m_pGridCtrl->SetRowHeight(1,30);
	m_pGridCtrl->SetColumnWidth(0,120);
    m_pGridCtrl->ExpandToFit(TRUE);

 	//set checked
	CGridCellCheck * pCell;
   	for ( row = 0; row <= m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].nStationNum; row++){
	    pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,0);
        pCell->SetCheck (TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSendStationPlanDlg::OnOK() 
{
    int  nSelectedNum = 0;
	int  nSelectedList[MAX_STATION_PER_SECTION];
	long usDateValue,usTimeValue;

	CTime ctStartTimeValue,ctEndTimeValue;
	m_StartDateTime.GetTime(ctStartTimeValue);
	m_EndDateTime.GetTime(ctEndTimeValue);
    CString str;


	bool m_bChecked;
    CGridCellCheck *pCell;
	for (int row = 1; row <= m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].nStationNum; row++){
	    pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,0);
        m_bChecked = pCell->GetCheck ();
		if(m_bChecked){
		  nSelectedList[nSelectedNum++] = m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].nStation ;
		  m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usSendDate = m_pDoc->m_usCurrentDate;
		  m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usSendTime = m_pDoc->m_usCurrentTime;
          usTimeValue = minute_time(ctStartTimeValue.GetTime (),&usDateValue);
		  m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usStartDate = usDateValue;
		  m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usStartTime = usTimeValue;
          usTimeValue = minute_time(ctEndTimeValue.GetTime(),&usDateValue);
		  m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usEndDate = usDateValue;
		  m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usEndTime = usTimeValue;
		  m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].bSignInFlag  = 0;
		  m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].bReceiveFlag = 0;
		  m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].strSignName[0] = 0x00;

          //update data		  
  		  str = OsConvertTimetoString(m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usSendTime);
          m_pGridCtrl->SetItemText (row,1,str);
		  str = OsConvertTimetoString(m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usStartTime);
          m_pGridCtrl->SetItemText (row,2,str);
		  str = OsConvertTimetoString(m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[row-1].usEndTime);
          m_pGridCtrl->SetItemText (row,3,str);
		  str = "否";
		  m_pGridCtrl->SetItemFgColour (row,4,CLR_INDIANRED);
          m_pGridCtrl->SetItemText (row,4,str);
		  str = "否";
		  m_pGridCtrl->SetItemFgColour (row,5,CLR_INDIANRED);
          m_pGridCtrl->SetItemText (row,5,str);
		  str = "";
          m_pGridCtrl->SetItemText (row,6,str);
		}
	}
    m_pGridCtrl->UpdateData ();
    Invalidate(FALSE);

    ushort usCommQueueHandle;
	while(OsGetQueueHandle(PMTG.ThreadId,&usCommQueueHandle,
		"QUEUES.GPC.COMM"))
	{
		OsSleep(1000L,PMTG.ThreadId);
		sysprint(0,PMTG.ThreadId,"[PMTG]:get comm queue handle failed");
	}
    
	//compare start time  to end time
    usTimeValue = minute_time(ctStartTimeValue.GetTime (),&usDateValue);
    long usEndTimeValue,usEndDateValue;
    usEndTimeValue = minute_time(ctEndTimeValue.GetTime (),&usEndDateValue);
    if(usEndTimeValue < usTimeValue){
		usEndDateValue = OsGetNextDate(PMTG.ThreadId,usEndDateValue);
	}

	int nLinkCount = 0;
	char buff[128], *ptr;
 	ushort count;

	//MESSAGE BEGIN
   	ptr = buff;
	*ptr++ = PMTG_MESSAGE;
	ptr += 2;
 
    *(ushort*)ptr = NettoHostUShort(SEND_STPC_MANUAL_PLAN);
    ptr += 2;
	*(ushort*)ptr = NettoHostUShort(gpcdef.GetUserGpcId());
	ptr += 2;
	*ptr = (byte)m_pDoc->m_nCurrentSectionNo;
	ptr +=1;

    *(time_t*)ptr = NettoHostUInteger(ctStartTimeValue.GetTime());
	ptr += 4;
 //   *(time_t*)ptr = NettoHostUInteger(ctEndTimeValue.GetTime ());
    *(time_t*)ptr = NettoHostUInteger(long_time(usEndDateValue,usEndTimeValue));
	ptr += 4;

	//debug
/*	CString strEndTime;
    ULONG nTime = (ULONG)long_time(usEndDateValue,usEndTimeValue);
	strEndTime.Format("time_t:%u,str:",nTime);
	strEndTime += OsConvertTimetoString(usEndTimeValue);
//  *(time_t*)ptr = NettoHostUInteger(long_time(m_pDoc->m_usCurrentDate ,m_pDoc->m_usCurrentTime));
//	ptr += 4;
*/ 	
    *(ushort*)ptr = NettoHostUShort((ushort)m_pDoc->m_usCurrentTime);
	ptr += 2;

	*(ushort*)ptr = NettoHostUShort(nSelectedNum);
	ptr += 2;
    for(int i=0;i<nSelectedNum;i++){
	    *(ushort*)ptr = NettoHostUShort(nSelectedList[i]);
        ptr += 2;
	}
 	count = ptr -buff;
	*(ushort *)(buff + 1) = NettoHostUShort(count - 1);
	WriteMessage(PMTG.ThreadId,	usCommQueueHandle,count,buff);	

//	CDialog::OnOK();
}

LRESULT CSendStationPlanDlg::ChangeStationReceiveStatus(WPARAM wParam,LPARAM lParam)//(int nStationNo, int nFlag)
{
	int nStationNo,nFlag;
	nStationNo = (int)wParam;
	nFlag = (int)lParam;

	for ( int i = 0; i <= m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].nStationNum; i++){
		if(nStationNo == m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan [i].nStation){
			if(nFlag){
			   m_pGridCtrl->SetItemText (i+1,5,"是");
			   m_pGridCtrl->SetItemFgColour (i+1,5,CLR_GREEN);
			   CString str;
			   str.Format("%s",m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].m_StationPlan[i].strSignName);
			   m_pGridCtrl->SetItemText (i+1,6,str);           
			}else{
			   m_pGridCtrl->SetItemText(i+1,4,"是");
			   m_pGridCtrl->SetItemFgColour (i+1,4,CLR_GREEN);
			}
		}
	}
	m_pGridCtrl->UpdateData ();
    Invalidate(FALSE);
    return true;
}

void CSendStationPlanDlg::OnTimer(UINT nIDEvent) 
{
 	
	CDialog::OnTimer(nIDEvent);
}

void CSendStationPlanDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CSendStationPlanDlg::OnGrid() 
{
	bool m_bChecked;
    CGridCellCheck *pCell;
 	pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(0,0);
	m_bChecked = pCell->GetCheck ();
	if(bAllStationFlag == m_bChecked)
		return;
	else{
		for (int row = 1; row <= m_pDoc->m_StationPlanInfo[m_pDoc->m_nCurrentSectionNo].nStationNum; row++){
 			pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row,0);
			if(m_bChecked){
				pCell->SetCheck(TRUE);
			}else{
				pCell->SetCheck(FALSE);		
			}

		}	
        bAllStationFlag = m_bChecked;

	}
}

void CSendStationPlanDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if(m_pGridCtrl)
	    delete m_pGridCtrl;
	
}

void CSendStationPlanDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::PostNcDestroy();
	delete this;
}
