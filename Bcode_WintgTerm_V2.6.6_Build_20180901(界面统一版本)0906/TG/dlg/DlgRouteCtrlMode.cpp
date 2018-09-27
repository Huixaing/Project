// DlgRouteCtrlMode.cpp : implementation file
//

#include "stdafx.h"
#include "DlgRouteCtrlMode.h"
#include ".\dlgroutectrlmode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRouteCtrlMode dialog


CDlgRouteCtrlMode::CDlgRouteCtrlMode(CTGDoc *pDoc,int nStNo,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRouteCtrlMode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRouteCtrlMode)
		// NOTE: the ClassWizard will add member initialization here
	nStationNo = nStNo;
	m_pDoc = pDoc;
	//}}AFX_DATA_INIT
}


void CDlgRouteCtrlMode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRouteCtrlMode)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRouteCtrlMode, CDialog)
	//{{AFX_MSG_MAP(CDlgRouteCtrlMode)
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRouteCtrlMode message handlers

BOOL CDlgRouteCtrlMode::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	BOOL bFound = FALSE;
	
	for (int i = 0; i < m_pDoc->m_nDispDistanceNum; i++)
	{
		int nStation = m_pDoc->m_sDispDistance[i].station_number;
		if(nStationNo == nStation)
		{
			bFound = TRUE;
			break;
		}
	}
	if(bFound == FALSE) return FALSE;
	
	CWnd *pWnd = GetDlgItem(IDC_STATION);
	pWnd->SetWindowText(m_pDoc->GetStationName(nStationNo));
	
	CString strStatus;
	if(m_pDoc->m_sDispDistance[i].usLircStatus != 0)
	{
       //strStatus ="站死";
       strStatus ="与车站断开";
	}
	else
	{
      if(m_pDoc->m_sDispDistance[i].nRouteCtrlMode == 0)    //按图排路
		  strStatus ="按图排路";
	   else if(m_pDoc->m_sDispDistance[i].nRouteCtrlMode == 1) //存储控制
		  strStatus ="存储控制";
	   else if(m_pDoc->m_sDispDistance[i].nRouteCtrlMode == 2) //人工排路
			 strStatus ="人工排路";
	   else
             strStatus ="未知状态";
	 }
	
	 pWnd = GetDlgItem(IDC_STATUS);
	 pWnd->SetWindowText(strStatus);
	 

	 strStatus.Empty();
	 pWnd = GetDlgItem(IDC_MODE_STATUS);
	 if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 0)       //分散自律 
		 //nOldColor = pDC->SetTextColor(CLR_LAWNGREEN);
		 strStatus = "分散自律控制";
	 else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 1)  //中心控制 
		 //nOldColor = pDC->SetTextColor(CLR_YELLOW);
		 strStatus = "中心优先控制";
	 else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 2)  //车站控制
		 //nOldColor = pDC->SetTextColor(CLR_BLUE);
		 strStatus = "车站优先控制";
	 else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 3)  //非常控制
		 //nOldColor = pDC->SetTextColor(CLR_DARKGRAY);
		 strStatus = "非常站控";
	 else
		 //nOldColor = pDC->SetTextColor(CLR_DARKGRAY);
		 strStatus = "与车站断开";
	 pWnd->SetWindowText(strStatus);
	 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CDlgRouteCtrlMode::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/*
	// TODO:  在此更改 DC 的任何属性
    DISPDISTANCE m_pDoc->m_sDispDistance[128];
	BOOL bFound = FALSE;
	m_sDispDistance = m_pDoc->m_sDispDistance;
	for (int i = 0; i < m_pDoc->m_nDispDistanceNum; i++)
	{
		int nStation = m_pDoc->m_sDispDistance[i].station_number;
		if(nStationNo == nStation)
		{
			bFound = TRUE;
			break;
		}
	}
	if(bFound == FALSE) return hbr;

	COLORREF col;
    if(GetDlgItem(IDC_MODE_STATUS)->m_hWnd==pWnd->m_hWnd )
	{
       if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 0)       //分散自律 
		  col = CLR_LAWNGREEN;
	   else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 1)  //中心控制 
		  col =  CLR_YELLOW;
	   else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 2)  //车站控制
		  col = CLR_BLUE;
	   else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == 3)  //非常控制
		  col = CLR_GRAY;
	   else
		  col = CLR_RED ;

		pDC->SetBkColor(col);


	}
	*/
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
