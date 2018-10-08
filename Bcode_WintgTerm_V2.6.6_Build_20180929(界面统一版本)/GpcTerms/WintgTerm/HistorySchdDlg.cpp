// HistorySchdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HistorySchdDlg.h"
#include "messages.h"
#include "function.h"
#include "memshare.h"
#include "tgmsg.h"      /***自定义消息 ：UM_LOGIN_MSG  **/ 
#include "DlgInputDate.h"
#include <BaseMsg/clbth.h>
#include <BaseMsg/gpc_msgid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
const COLORREF g_crGray=RGB(192,192,192);
const COLORREF g_crRed=RGB(255,0,255);

/////////////////////////////////////////////////////////////////////////////
// CHistorySchdDlg dialog


CHistorySchdDlg::CHistorySchdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHistorySchdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHistorySchdDlg)
 	m_date = _T("");
	m_banbie = _T("");
	m_bUnChange = FALSE;
	m_inputFlag = FALSE;
	m_nPreSchdReqNum=0;
	m_bCtrlBanci=false;
	//}}AFX_DATA_INIT
}


void CHistorySchdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistorySchdDlg)
	DDX_Control(pDX, IDC_BANBIE, m_BanbieCtrl);
	DDX_Control(pDX, IDC_DATE, m_DateCtrl);
 	DDX_Text(pDX, IDC_DATE, m_date);
	DDX_Text(pDX, IDC_BANBIE, m_banbie);
	DDX_Check(pDX, IDC_CHECK_INPUTDIR, m_inputFlag);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHistorySchdDlg, CDialog)
	//{{AFX_MSG_MAP(CHistorySchdDlg)
    ON_MESSAGE(UM_LOGIN_MSG, OnLoginMsg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_PREVIOUS, OnPrevious)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_CHECK_INPUTDIR, OnCheckInputdir)
	//}}AFX_MSG_MAP
	//afx_msg void OnAbsTimeMsg(WPARAM wParam, LPARAM lParam);
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistorySchdDlg message handlers

void CHistorySchdDlg::OnOK() 
{   	
    CButton *pButton;
	pButton = (CButton *)GetDlgItem(IDC_RADIO_ORIGINAL);
    if(pButton->GetCheck ())
	   m_bUnChange = TRUE;	
	else
	   m_bUnChange = FALSE;	

	int shiftid=id;
	gpcdef.SetHistoryShiftid(shiftid);        //login shiftid,e.g. 2002060701 
	unsigned long m_usCurrentDate = (shiftid%1000000 - 1990) * 380 +
					 ( shiftid%10000-(shiftid%1000000)*100 - 1) * 31 + 
					 shiftid%100 - (shiftid%10000)*100  - 1 ;
	CDialog::OnOK(); 
}
 
LRESULT CHistorySchdDlg::OnLoginMsg(WPARAM wParam, LPARAM lParam)
{
   return TRUE;
}

BOOL CHistorySchdDlg::OnInitDialog() 
{   
    CButton *pButton;
	pButton = (CButton *)GetDlgItem(IDC_RADIO_MODIFY);
    pButton->SetCheck ( TRUE);

	id = Get_CurrentShiftid();
    ConvertShiftidToString(Get_CurrentShiftid());
    UpdateData(FALSE);

    //if(gpcdef.GetUserIdentify() != GPC_TEST)
	//{
		pButton = (CButton *)GetDlgItem(IDC_RADIO_MODIFY);
		pButton->ShowWindow(FALSE);

		pButton = (CButton *)GetDlgItem(IDC_RADIO_ORIGINAL);
		pButton->ShowWindow(FALSE);
	//}
	//else
	/*
	{
		pButton = (CButton *)GetDlgItem(IDC_RADIO_MODIFY);
		pButton->ShowWindow(TRUE);
		
		pButton = (CButton *)GetDlgItem(IDC_RADIO_ORIGINAL);
		pButton->ShowWindow(TRUE);
	}
    ///////////// 增加区段选择 ////////////////////
	*/
	// 不允许输入登录班次
	if(m_bCtrlBanci)
	{
		CWnd* pWnd = GetDlgItem(IDC_CHECK_INPUTDIR);
		if(pWnd != NULL)
			pWnd->EnableWindow(FALSE);
	}
	InitDlgPosition(this);
	return TRUE;   
}

HBRUSH CHistorySchdDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	 if(  m_DateCtrl.m_hWnd==pWnd->m_hWnd  || m_BanbieCtrl.m_hWnd==pWnd->m_hWnd )
	 {     
	     pDC->SetBkColor(bk_color);
	     return (HBRUSH) m_Brush;
     }  
 
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

 
void CHistorySchdDlg::OnPrevious() 
{   
  	UpdateData(TRUE);
    id=Get_PreShiftid(id);
    ConvertShiftidToString(id);

	m_nPreSchdReqNum++;
	if(m_nPreSchdReqNum>=m_nPreSchdMaxNum && m_bCtrlBanci)
	{
		CWnd* pWnd = GetDlgItem(IDC_PREVIOUS);
		if(pWnd != NULL)
			pWnd->EnableWindow(FALSE);
	}
}
 
void CHistorySchdDlg::OnNext() 
{   
  	UpdateData(TRUE);
	id=Get_NextShiftid(id);
    ConvertShiftidToString(id);

	m_nPreSchdReqNum--;
	if(m_nPreSchdReqNum<m_nPreSchdMaxNum && m_bCtrlBanci)
	{
		CWnd* pWnd = GetDlgItem(IDC_PREVIOUS);
		if(pWnd != NULL)
			pWnd->EnableWindow(TRUE);
	}
}

 
CString CHistorySchdDlg::GetNextDay()
{ 
	int  CurYear,CurMonth,CurDay;
	CString NextDay;
	
	CTime t=CTime::GetCurrentTime();
    CurYear=t.GetYear();
	CurMonth=t.GetMonth();
	CurDay=t.GetDay();
    ASSERT(CurDay>=1 && CurDay<=31);
	switch(CurDay)
	{
	case 28:
		if( (CurYear %4==0 && CurYear %100!=0)||(CurYear %400==0) )
			NextDay.Format ("%4d年%d月%d日",CurYear,CurMonth,29);
		else
			NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth+1,  1);
		break;
		
	case 29:
		if(CurMonth==2)
            NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth+1,1);
		else
            NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth,30);
		break;
		
    case 30:
		if(CurMonth==1 || CurMonth==3 || CurMonth==5 || CurMonth==7 || CurMonth==8
			|| CurMonth==10 ||CurMonth==12)
			NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth,31);
		else
			NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth+1,1);
		break;
		
	case 31:
		if(CurMonth==12)
			NextDay.Format ("%4d年 %d月%d日",CurYear+1,1,1);
		else
			NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth+1,1);
		break;
		
	default:
		NextDay.Format ("%4d年 %d月%d日",CurYear,CurMonth,CurDay+1);
		break;
		
	}
	
	return NextDay;
}
 
int CHistorySchdDlg::GetConsoleid()
{

	CString DispatchStationName;  // 调度台名称
	int consoleid;

    DispatchStationName.Format ("%s",gpcdef.GetCurUserName ());
	ASSERT( DispatchStationName.Remove('g')==1 );
    ASSERT( DispatchStationName.Remove('p')==1 );
    ASSERT( DispatchStationName.Remove('c')==1) ;

	consoleid=atoi(DispatchStationName);
	ASSERT(consoleid>=0);
	return consoleid;
}

unsigned long CHistorySchdDlg::nGetToday()
{
	int  iYear,iMonth,iDay;
	CString  sYear,sMonth,sDay,Today;
	unsigned long result;
    
	iYear=t.GetYear();
	iMonth=t.GetMonth();
	iDay=t.GetDay();
    
    sYear.Format ("%4d",iYear);
	
	if(iMonth>9)
		sMonth.Format("%2d",iMonth);
	else
		sMonth.Format("0%d",iMonth);
	
    if(iDay>9)
		sDay.Format("%2d",iDay);
	else
		sDay.Format("0%d",iDay);
	
    Today=sYear+sMonth+sDay;
	result=atol(Today);
	
    return result;
}

unsigned long CHistorySchdDlg::nGetNextDay()
{
	int  CurYear,CurMonth,CurDay;
	CString  NextDay;
    unsigned long result;
    CurYear=t.GetYear();
	CurMonth=t.GetMonth();
	CurDay=t.GetDay();
    ASSERT(CurDay>=1 && CurDay<=31);
	switch(CurDay)
	{
	case 28:
		    if( (CurYear %4==0 && CurYear %100!=0)||(CurYear %400==0) ) //闰年
				NextDay.Format ("%4d%2d%2d",CurYear,CurMonth,29);
			else
				NextDay.Format ("%4d%2d%2d",CurYear,CurMonth+1,  1);
	         break;

	case 29:
		    if(CurMonth==2)
            NextDay.Format ("%4d%2d%2d",CurYear,CurMonth+1,1);
			else
            NextDay.Format ("%4d%2d%2d",CurYear,CurMonth,30);
			break;

    case 30:
		    if(CurMonth==1 || CurMonth==3 || CurMonth==5 || CurMonth==7 || CurMonth==8
				|| CurMonth==10 ||CurMonth==12)
			  NextDay.Format ("%4d%2d%2d",CurYear,CurMonth,31);
			else
			  NextDay.Format ("%4d%2d%2d",CurYear,CurMonth+1,1);
			break;

	case 31:
		    if(CurMonth==12)
				 NextDay.Format ("%4d%2d%2d",CurYear+1,1,1);
			else
				 NextDay.Format ("%4d%2d%2d",CurYear,CurMonth+1,1);
			break;

	default:
		     NextDay.Format ("%4d%2d%2d",CurYear,CurMonth,CurDay+1);
           break;
		   
	}

	NextDay.Replace(" ","0");  
    result=atol(NextDay);
    return result;
}
void CHistorySchdDlg::ConvertShiftidToString(unsigned long id)
{
	CString date,banbie;
	
	int CurYear  = id/1000000;
	int CurMonth = id/10000-CurYear*100;
	int CurDay   = id/100-(id/10000)*100;
    date.Format("%4d年%2d月%2d日",CurYear,CurMonth,CurDay);
	int CurBanbie=id-(id/10)*10;
    banbie=(CurBanbie==1) ? "第一班" : "第二班";
  
	m_date=date;
	m_banbie=banbie;

    if(CurBanbie==1)
	 bk_color=g_crGray;
	else
     bk_color=g_crRed;
    m_Brush.DeleteObject();
	m_Brush.CreateSolidBrush(bk_color); //color initiate;

	UpdateData(FALSE);
    Invalidate(FALSE);
}

void CHistorySchdDlg::OnCheckInputdir() 
{
	CDlgInputDate dlg(this);
	if(dlg.DoModal() == IDOK)
	{
		CTime t;
		int banbie;
		t = dlg.m_inputDate;
		banbie = dlg.m_inputBanbie;

		CString strShiftid;
		strShiftid = t.Format("%Y%m%d");
		if(banbie == 1)
			strShiftid += "01";
		else
			strShiftid += "02";

		id = atol((char*)(LPCTSTR)strShiftid);
		
		ConvertShiftidToString(id);
	}
	else
	{
	}
	m_inputFlag = FALSE;
	UpdateData(FALSE);
}
