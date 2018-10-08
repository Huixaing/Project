// AddNewTrainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AddNewTrainDlg.h"
#include "tgmsg.h" //add by cf
#include "function.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include "tgdocument.h"
#include "TgEditView.h"
#include "sysshare.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddNewTrainDlg dialog

int station_id=0;
CAddNewTrainDlg::CAddNewTrainDlg(CTgDocument *pDoc,int schd_type,CWnd* pParent /*=NULL*/)
	: CDialog(CAddNewTrainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddNewTrainDlg)
	m_strTrainId = _T("");
	m_strReceiveTime = _T("");
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_nTrainCount = 0;
  	m_pView = (CTgEditView*)pParent;
	passway_count = 0;
	Flag_moredata = FALSE;
	this->Schd_type = schd_type;
}

void CAddNewTrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddNewTrainDlg)
	DDX_Control(pDX, IDC_COMBO_ROUTE, m_ctrlRoute);
	DDX_Control(pDX, IDC_COMBO_STATION, m_ctrlStation);
	DDX_Control(pDX, IDC_COMBO_RUNTYPE, m_ctrlRunType);
	DDX_Control(pDX, IDC_LIST_TRAIN, m_ctrlList);
	DDX_Text(pDX, IDC_TRAIN_ID, m_strTrainId);
	DDV_MaxChars(pDX, m_strTrainId, 9);
	DDX_Text(pDX, IDC_RECEIVE_TIME, m_strReceiveTime);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddNewTrainDlg, CDialog)
	//{{AFX_MSG_MAP(CAddNewTrainDlg)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_CBN_SELCHANGE(IDC_COMBO_ROUTE, OnSelchangeComboRoute)
	ON_CBN_KILLFOCUS(IDC_COMBO_ROUTE, OnKillfocusComboRoute)
	ON_CBN_SELCHANGE(IDC_COMBO_RUNTYPE, OnSelchangeRunType)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_STATION, OnSelchangeComboStation)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddNewTrainDlg message handlers

BOOL CAddNewTrainDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
    
	CWnd *pWndFocus = GetDlgItem(IDC_BUTTON_ADD);
	pWndFocus->SetFocus ();

	CButton* pButton = (CButton *)GetDlgItem(IDC_GIVEOUT);
	pButton->SetCheck(FALSE);
	pButton->EnableWindow(FALSE);

	pButton = (CButton *)GetDlgItem(IDC_FETCH);
	pButton->SetCheck(FALSE);
	pButton->EnableWindow(FALSE);
	
	pButton = (CButton *)GetDlgItem(IDC_START);
	pButton->SetCheck(FALSE);
	pButton->EnableWindow(FALSE);

	pButton = (CButton *)GetDlgItem(IDC_TERMINAL);
	pButton->SetCheck(FALSE);
	pButton->EnableWindow(FALSE);
	
	InitDlgPosition(this);
	LoadPasswayInfo();

	// 过滤passway多余站名
	std::vector<int> station_list;
	for(int i=0;i < passway_count;i++)
	{		
		int stationno = Info[i].Begin_sta;
		int k=0;
		for(k=0; k<station_list.size(); k++)
		{
			if(station_list[k] == stationno)
				break;
		}
		if(k>=station_list.size() && stationno>0)
		{
			station_list.push_back(stationno);
		}
	}

	for(int i=0; i<station_list.size(); i++)
	{
		m_ctrlStation.AddString(gpcdef.GetStationName(station_list[i]));
		m_ctrlStation.SetItemData(i,(DWORD)station_list[i]);
	} 
    
	CStringArray name_list;
	CWordArray run_type_list;
	int count=GetTGBasicParam()->GetAllAbsTimeTypeName(name_list, run_type_list);
	
	int nItem=m_ctrlRunType.AddString("默认区间时分");
	m_ctrlRunType.SetItemData(nItem, 255);
	for(int i=0; i<count; i++)
	{
		nItem=m_ctrlRunType.AddString(name_list.GetAt(i));
		m_ctrlRunType.SetItemData(nItem, run_type_list.GetAt(i));
	}
	if(count<=0)
	{
		CWnd* pWnd=GetDlgItem(IDC_ABS_TIME_STATIC);
		if(pWnd)
			pWnd->ShowWindow(SW_HIDE);
		m_ctrlRunType.ShowWindow(SW_HIDE);
		m_ctrlRunType.EnableWindow(FALSE);
	}
	m_ctrlRunType.SetCurSel(0);
	m_ctrlStation.SetCurSel(0);
	m_ctrlRoute.SetCurSel(0);

	OnSelchangeComboRoute();
	OnSelchangeComboStation();
	OnSelchangeRunType();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddNewTrainDlg::OnButtonAdd() 
{
	UpdateData(TRUE);

    BYTE btTrainType = 1;

    BOOL bReportArrival;
	bReportArrival = TRUE;

	BOOL giveout_terminal;
	CButton *pButton2 = (CButton*)GetDlgItem(IDC_GIVEOUT);
	giveout_terminal = pButton2->GetCheck();

	BOOL fetch_begin;
	CButton *pButton3 = (CButton *)GetDlgItem(IDC_FETCH);
	fetch_begin = pButton3->GetCheck();

	m_strTrainId = ChangeAlpha(m_strTrainId);
    //车次中不允许有空格
	if(m_strTrainId.Find(" ")>=0)
	{
		MessageBox("车次中有空格,请新输入!");
		return;
	}

	if(m_strTrainId == ""){
	   AfxMessageBox("请输入车次号!");
	   return;
	}
	if(m_strReceiveTime  == ""){
	   AfxMessageBox("请输入接入时间!");
	   return;
	}
	//////////////////////////////////////////////////////////////////////////
	//zgj changed at shanghai 
	bool isValid=true;
	do{
		if(!isdigit(m_strReceiveTime.GetAt(0)))
			isValid=false;
		//格式为：hh:mm,或者hh.mm
		if(!(m_strReceiveTime.FindOneOf(_T("."))==-1||m_strReceiveTime.FindOneOf(_T(":"))==-1))
			isValid=false;
		int hour=atoi((LPCSTR)m_strReceiveTime);
		int minute=atoi(m_strReceiveTime.Right(m_strReceiveTime.GetLength()-m_strReceiveTime.FindOneOf(_T("."))-1));
		if(hour<0||hour>24||minute<0||minute>59)
			isValid=false;
		//还可能是形如：hhmm的形式
		if(!isValid)
		{
			if(m_strReceiveTime.GetLength() == 4)
			{
				int hour=atoi((LPCSTR)(m_strReceiveTime.Left(2)));
				int minute=atoi(m_strReceiveTime.Right(2));
				if(hour<0||hour>24||minute<0||minute>59)
					isValid=false;
				else
				{	
					//添加上标示符
					CString hh= m_strReceiveTime.Left(2);
					CString mm= m_strReceiveTime.Right(2);
					m_strReceiveTime = hh+":"+mm;

					isValid=true;
				}

			}

		}
	}while(0);
	if(!isValid){
		MessageBox("对不起，时间输入不对!");
		CWnd* pWnd = GetDlgItem(IDC_RECEIVE_TIME); 
		GotoDlgCtrl(pWnd); 
		return;
	}
	/////////////////////////////////////////////////////////////////
	for(int i = 0;i < m_nTrainCount;i ++){
		if(!strcmp(m_nReportTrainInfo[i].strTrainId,(LPCTSTR)m_strTrainId)){
			AfxMessageBox("此车已在列表中!");
			return;
		}
	}

	int index;
	long usDate;
	index = m_ctrlStation.GetCurSel ();
	m_nReportTrainInfo[m_nTrainCount].nStation = (ushort)m_ctrlStation.GetItemData(index);
	memset(m_nReportTrainInfo[m_nTrainCount].strTrainId,0,sizeof(m_nReportTrainInfo[m_nTrainCount].strTrainId));
	strncpy(m_nReportTrainInfo[m_nTrainCount].strTrainId,(LPCTSTR)m_strTrainId,9);

 	index = m_ctrlRoute.GetCurSel ();
	m_nReportTrainInfo[m_nTrainCount].nRoute = (ushort)m_ctrlRoute.GetItemData(index);
  	m_nReportTrainInfo[m_nTrainCount].usReceiveTime = OsConvertStringtoTime((char *)(LPCTSTR)m_strReceiveTime);
	if(m_nReportTrainInfo[m_nTrainCount].usReceiveTime==-1){
		MessageBox("对不起，时间输入不对!");
		CWnd* pWnd = GetDlgItem(IDC_RECEIVE_TIME); 
		GotoDlgCtrl(pWnd); 
		return;
	}
    if(m_pDoc->GetCurrentTGTime() >= 18*3600 && m_pDoc->GetCurrentTGTime() < 24*3600){
       if(m_nReportTrainInfo[m_nTrainCount].usReceiveTime >= 18*3600 && 
		  m_nReportTrainInfo[m_nTrainCount].usReceiveTime < 24*3600)
		  usDate = m_pDoc->GetCurrentTGDate();
	   else
		  usDate = OsGetNextDate(PMTG.ThreadId,m_pDoc->GetCurrentTGDate());
	}else{
		  usDate = m_pDoc->GetCurrentTGDate();
	}
    
	// 判断是否重复加车
	if(m_pView!=NULL)
	{
		if(!m_pView->IsAddTrain(m_strTrainId))
		{
			return;
		}
	}

	m_nReportTrainInfo[m_nTrainCount].usReceiveDate = usDate ;
  	m_nReportTrainInfo[m_nTrainCount].btTrainType = btTrainType;
  	m_nReportTrainInfo[m_nTrainCount].nFlag = bReportArrival;
	m_nReportTrainInfo[m_nTrainCount].giveout = giveout_terminal;
	m_nReportTrainInfo[m_nTrainCount].fetch= fetch_begin;
	m_nReportTrainInfo[m_nTrainCount].nRunType=m_nRunType;

    CString str,strRoute;
	m_ctrlRoute.GetWindowText (strRoute);

	if(giveout_terminal)
	{
		if(fetch_begin)
		{
			str.Format ("%s  %s %s %s %s",m_nReportTrainInfo[m_nTrainCount].strTrainId,
				m_strReceiveTime,"接入","交出",strRoute);
		}
		else
		{
			str.Format ("%s  %s %s %s %s",m_nReportTrainInfo[m_nTrainCount].strTrainId,
				m_strReceiveTime,"始发","交出",strRoute);

		}

	}
	else
	{
		if(fetch_begin)
		{
			str.Format ("%s  %s %s %s %s",m_nReportTrainInfo[m_nTrainCount].strTrainId,
				m_strReceiveTime,"接入","终到",strRoute);

		}
		else
		{
			str.Format ("%s  %s %s %s %s",m_nReportTrainInfo[m_nTrainCount].strTrainId,
				m_strReceiveTime,"始发","终到",strRoute);

		}
	}

    m_ctrlList.AddString(str);
   	m_nTrainCount ++;
}

BOOL CAddNewTrainDlg::PreTranslateMessage(MSG* pMsg) 
{
	CWnd* pWndFocus = GetFocus();

	if ((pMsg->message == WM_KEYDOWN) &&
		((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE)))
	{
		// Special case: if control with focus is an edit control with
		// ES_WANTRETURN style, let it handle the Return key.

		TCHAR szClass[10];
		if ((pMsg->wParam == VK_RETURN) &&
			((pWndFocus = GetFocus()) != NULL) &&
			IsChild(pWndFocus)  &&
			GetClassName(pWndFocus->m_hWnd, szClass, 10) &&
			(_tcsicmp(szClass, _T("EDIT")) == 0)
		){
			CWnd *pWnd;
			if(pWndFocus == GetDlgItem(IDC_TRAIN_ID)) //车次
			{
				pWnd = GetDlgItem(IDC_RECEIVE_TIME); 
				GotoDlgCtrl(pWnd); 
			} else if(pWndFocus == GetDlgItem(IDC_RECEIVE_TIME)){ //开始时间点
  					pWnd = GetDlgItem(IDC_TRAIN_ID); 
					GotoDlgCtrl(pWnd); 
                    OnButtonAdd();
					pWndFocus->SetWindowText("");
					pWnd->SetWindowText("");
			}
		}
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CAddNewTrainDlg::OnButtonDel() 
{
	int index;
//	long usDate;
	index = m_ctrlList.GetCurSel ();
	if(m_nTrainCount==0 || index < 0 )return;
	m_ctrlList.DeleteString (index);
    for (int i=index;i<m_nTrainCount-1;i++){
	m_nReportTrainInfo[i].nStation = m_nReportTrainInfo[i+1].nStation;
	strcpy(m_nReportTrainInfo[i].strTrainId, m_nReportTrainInfo[i+1].strTrainId);
	m_nReportTrainInfo[i].nRoute = m_nReportTrainInfo[i+1].nRoute ;
	m_nReportTrainInfo[i].usReceiveTime = m_nReportTrainInfo[i+1].usReceiveTime;
	m_nReportTrainInfo[i].usReceiveDate = m_nReportTrainInfo[i+1].usReceiveDate;
	m_nReportTrainInfo[i].btTrainType = m_nReportTrainInfo[i+1].btTrainType;
	m_nReportTrainInfo[i].nFlag = m_nReportTrainInfo[i+1].nFlag;
	m_nReportTrainInfo[i].giveout = m_nReportTrainInfo[i+1].giveout;
	m_nReportTrainInfo[i].fetch = m_nReportTrainInfo[i+1].fetch;
	}
	m_nTrainCount --;
	if(index -1 >=0)
	    m_ctrlList.SetCurSel(index-1);	
	else
	    m_ctrlList.SetCurSel(0);	
}

/*******************************************************************/   
// this function is added by cf in order to send message LOAD_PASSWAY_INFO to CAD 
/* modify by cf                                                    */
/*******************************************************************/   
void CAddNewTrainDlg::LoadPasswayInfo()
{
	char buff[128], *ptr;
 	ptr = buff;
	
    int db_count=sizeof(Info)/sizeof(Info[0]);
	if(GetTGBasicParam()->GetPasswayInfo(m_pDoc->GetCurrentSectionNo(),Info,&db_count))
	{
		passway_count=db_count;
		return;
	}
}
/*******************************************************************/ 
/* above add by cf                                                 */ 
/******************************************************************/  


/*******************************************************************/   
//this function is added by cf in order to answer for UM_PASSWAY_INFO message
/* modify by cf                                                       */
/*******************************************************************/   
int add_train_select_id=0;

void CAddNewTrainDlg::OnSelchangeComboRoute() 
{
	// TODO: Add your control notification handler code here
	int index;
	index = m_ctrlRoute.GetCurSel();	
	add_train_select_id=index;

	CString str="";
	m_ctrlRoute.GetWindowText(str);
	int start=str.FindOneOf("(");
	int end=str.FindOneOf("-");
	CButton *pButton;

//	if(index != 0)
	{
		pButton = (CButton *)GetDlgItem(IDC_GIVEOUT);
		pButton->SetCheck(TRUE);
		pButton->EnableWindow();
		
		pButton = (CButton *)GetDlgItem(IDC_FETCH);
		pButton->SetCheck(TRUE);
		pButton->EnableWindow();
		
		pButton = (CButton *)GetDlgItem(IDC_START);
		pButton->SetCheck(FALSE);
		pButton->EnableWindow();
		
		pButton = (CButton *)GetDlgItem(IDC_TERMINAL);
		pButton->SetCheck(FALSE);
		pButton->EnableWindow();

		int inflag,outflag;
		int RouteNo=m_ctrlRoute.GetItemData(index);
		if(GetTGBasicParam()->GetPasswayFlag(m_pDoc->GetCurrentSectionNo(),RouteNo,&inflag,&outflag))
		{
			if(inflag==SCHD_START) // 只能始发
			{
				((CButton *)GetDlgItem(IDC_START))->SetCheck(1);
				((CButton *)GetDlgItem(IDC_FETCH))->SetCheck(0);
				((CButton *)GetDlgItem(IDC_START))->EnableWindow(FALSE);
				((CButton *)GetDlgItem(IDC_FETCH))->EnableWindow(FALSE);
			}
			if(outflag==SCHD_TERMINAL) // 只能终到
			{
				((CButton *)GetDlgItem(IDC_TERMINAL))->SetCheck(1);
				((CButton *)GetDlgItem(IDC_GIVEOUT))->SetCheck(0);
				((CButton *)GetDlgItem(IDC_TERMINAL))->EnableWindow(FALSE);
				((CButton *)GetDlgItem(IDC_GIVEOUT))->EnableWindow(FALSE);
			}
		}
	}

#ifdef DRAW_STOP_SIDE_SPECIAL
	if(m_pDoc->m_nCurrentSectionNo == 5){
		pButton = (CButton *)GetDlgItem(IDC_START);
		pButton->SetCheck(TRUE);
		pButton->EnableWindow();

		pButton = (CButton *)GetDlgItem(IDC_FETCH);
		pButton->SetCheck(FALSE);
		pButton->EnableWindow();
		
	}
	if(m_pDoc->m_nCurrentSectionNo == 6){
		pButton = (CButton *)GetDlgItem(IDC_GIVEOUT);
		pButton->SetCheck(FALSE);
		pButton->EnableWindow();
		
		pButton = (CButton *)GetDlgItem(IDC_TERMINAL);
		pButton->SetCheck(TRUE);
		pButton->EnableWindow();
		
	}
#endif
}

void CAddNewTrainDlg::OnSelchangeRunType()
{
	int nSel=m_ctrlRunType.GetCurSel();
	if(nSel==-1)
	{
		m_nRunType=0;
		return;
	}
	m_nRunType=m_ctrlRunType.GetItemData(nSel);
}

void CAddNewTrainDlg::OnKillfocusComboRoute() 
{	
}

void CAddNewTrainDlg::OnSelchangeComboStation()
{
	m_ctrlRoute.SetEditSel(0,-1);
    int iRouteCount;
    int iStationNo,i;
	iRouteCount = m_ctrlRoute.GetCount();
	
	int iCurSel = m_ctrlStation.GetCurSel();
	station_id=iCurSel;
	for(i=0;i<iRouteCount;i++)
	{
		m_ctrlRoute.DeleteString(0);
	}
	iRouteCount = m_ctrlRoute.GetCount();
	
	iStationNo = (ushort)m_ctrlStation.GetItemData(iCurSel);
	
    for(i=0;i<passway_count;i++)
	{
		if(iStationNo == Info[i].Begin_sta /*||iStationNo == info[i].End_sta*/)
		{
			m_ctrlRoute.AddString(Info[i].Pass_way_name); 
			m_ctrlRoute.SetItemData(iRouteCount++,Info[i].Pass_way_id);
		}
	}

	// 该车站没有配置径路则不显示径路
	iRouteCount = m_ctrlRoute.GetCount();
	if(iRouteCount > 0)
	m_ctrlRoute.SetCurSel(0);
	UpdateData(FALSE);
 }
