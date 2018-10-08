// DlgDividePortLateTime.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "sysshare.h"
#include "DlgDividePortLateTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDividePortLateTime dialog
CDlgDividePortLateTime::CDlgDividePortLateTime(CTG_SingleTrain *ptrain,CWnd* pParent)
	: CDialog(CDlgDividePortLateTime::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDividePortLateTime)
	m_strTime = _T("");
	m_strTimeEx = _T("");
	//}}AFX_DATA_INIT
	m_ptrain=ptrain;
	m_sborder=0;
	m_eborder=0;
}


void CDlgDividePortLateTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDividePortLateTime)
	DDX_Text(pDX, IDC_EDIT_TIME, m_strTime);
	DDX_Text(pDX, IDC_EDIT_TIME_EX, m_strTimeEx);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDividePortLateTime, CDialog)
	//{{AFX_MSG_MAP(CDlgDividePortLateTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDividePortLateTime message handlers

BOOL CDlgDividePortLateTime::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString strTitle;
	strTitle.Format("%s分界口/区段站晚点时分(from %d to %d)", m_ptrain->GetTrainID(),
		m_ptrain->GetTrainInfo()->m_data.from_conid, m_ptrain->GetTrainInfo()->m_data.to_conid);
	SetWindowText(strTitle);

	m_sborder=m_ptrain->GetTrainInfo()->m_data.sborder_actual_time;
	m_eborder=m_ptrain->GetTrainInfo()->m_data.eborder_actual_time;
	
	//first border station
	if(m_ptrain->GetTrainInfo()->m_data.sborder_sta>0)
	{
		strTitle = gpcdef.GetStationName(m_ptrain->GetTrainInfo()->m_data.sborder_sta);
	}
	else
	{
		strTitle="";
	}
	CWnd *pWnd = GetDlgItem(IDC_STATIC_FIRST);
	if(NULL != pWnd)
	{
		if(strTitle.IsEmpty())
			strTitle += "分界口/区段站晚点:";
		else
			strTitle += " 正晚点:";
		pWnd->SetWindowText(strTitle);
	}
	int nLateTime = m_ptrain->GetTrainInfo()->m_data.sborder_actual_time - m_ptrain->GetTrainInfo()->m_data.sborder_plan_time;

	if(nLateTime < 0)
	{
		if(abs(nLateTime/60) > 60)
			m_strTime.Format("-%d.%02d",abs(nLateTime/3600),abs((nLateTime/60)%60));
		else
			m_strTime.Format("-%d",abs(nLateTime/60));
	}
	else
	{
		if(abs(nLateTime/60) > 60)
			m_strTime.Format("%d.%02d",nLateTime/3600,abs((nLateTime/60)%60));
		else
			m_strTime.Format("%d",nLateTime/60);
	}
	

	//second border station
    if(m_ptrain->GetTrainInfo()->m_data.eborder_sta > 0)
	{
		strTitle = gpcdef.GetStationName(m_ptrain->GetTrainInfo()->m_data.eborder_sta);
		
		CWnd *pWnd = GetDlgItem(IDC_STATIC_SECOND);
		if(NULL != pWnd)
		{
			strTitle += "分界口晚点:";
			pWnd->SetWindowText(strTitle);
		}

		int nLateTime = m_ptrain->GetTrainInfo()->m_data.eborder_actual_time - m_ptrain->GetTrainInfo()->m_data.eborder_plan_time;

		if(nLateTime < 0)
		{
			if(abs(nLateTime/60) > 60)
			{
				m_strTimeEx.Format("-%d.%02d",abs(nLateTime/3600),abs((nLateTime/60)%60));
			}
			else
			{
				m_strTimeEx.Format("-%d",abs(nLateTime/60));
			}
		}
		else
		{
			if(abs(nLateTime/60) > 60)
			{
				m_strTimeEx.Format("%d.%02d",nLateTime/3600,abs((nLateTime/60)%60));
			}
			else
			{
				m_strTimeEx.Format("%d",nLateTime/60);
			}
		}
	
	}
	else
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_STATIC_SECOND);
		if(pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_EDIT_TIME_EX);
		if(pWnd)
			pWnd->EnableWindow(FALSE);
	}


		
	//if(!m_pDoc->IsCanEditTrain())
	//{
	//	CWnd* pWnd = GetDlgItem(IDOK);
	//	if(pWnd)
	//		pWnd->EnableWindow(FALSE);
	//}
	
	CRect rect, rect1;
	pWnd = this->GetParent();
	if(NULL != pWnd)
	{
		pWnd->GetWindowRect(&rect);
		this->GetWindowRect(&rect1);
		int x = rect.CenterPoint().x-rect1.Width()/2;
		int y = rect.CenterPoint().y-rect1.Height()/2;
		this->SetWindowPos(pWnd, x,  y, rect1.Width(), rect1.Height(), SWP_SHOWWINDOW);
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDividePortLateTime::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();

	if(NULL != m_ptrain)
	{
		char theStatus = 1;
		int uValue = 0;
		WORD uFirstValue,uSecondValue;
		char *ptr;
		char string[12];
		bool flag = TRUE;

		char *theString = (char*)(LPCTSTR)m_strTime;

		do 
		{
			if(theString == NULL || strlen(theString) == 0)
				break;
			if(strlen(theString) > 6)
				break;
			strcpy(string,theString);
			ptr = string;
			if(strchr(string,'-'))
			{
				flag = FALSE;
				ptr ++;
			} 

			if(!(strchr(ptr, '.') || strchr(ptr, ':')))
			{  
				uValue += atoi(ptr) * 60;
				theStatus = 0;
				break;
			}
			else
				ptr = strtok(ptr,":.");

			if(sscanf(ptr,"%hu",&uFirstValue) != 1)
				break;
			if(uFirstValue >= 72)
			{
				AfxMessageBox("时间太大,请重新输入!");
				return;
			}
			uValue = uFirstValue * 3600;
			if((ptr = strtok(NULL,":.")) != NULL)
			{
				if(sscanf(ptr,"%hu",&uSecondValue) != 1)
					break;
				if(uSecondValue > 60)
					break;
				uValue += uSecondValue * 60;
			}

			theStatus = 0;
		}while(0);

		if(theStatus == 1)
		{
			AfxMessageBox("时间格式有误,请重新输入!");
			return;
		}
		if(!flag)
			uValue = -uValue;

		m_ptrain->GetTrainInfo()->m_data.sborder_actual_time = 
			m_ptrain->GetTrainInfo()->m_data.sborder_plan_time + uValue;

		//second
		if(m_ptrain->GetTrainInfo()->m_data.eborder_sta != 0)
		{
			char *theString = (char*)(LPCTSTR)m_strTimeEx;
			theStatus = 1;
			uValue = 0;
			flag = TRUE;

			do 
			{
				if(theString == NULL || strlen(theString) == 0)
					break;
				if(strlen(theString) > 6)
					break;
				strcpy(string,theString);
				ptr = string;
				if(strchr(string,'-')){
					flag = FALSE;
					ptr ++;
				} 

				if(!(strchr(ptr, '.') || strchr(ptr, ':')))
				{  
					uValue += atoi(ptr) * 60;
					theStatus = 0;
					break;
				}
				else
					ptr = strtok(ptr,":.");

				if(sscanf(ptr,"%hu",&uFirstValue) != 1)
					break;
				if(uFirstValue >= 72)
				{
					AfxMessageBox("第二个时间太大,请重新输入!");
					return;
				}
				uValue = uFirstValue * 3600;
				if((ptr = strtok(NULL,":.")) != NULL)
				{
					if(sscanf(ptr,"%hu",&uSecondValue) != 1)
						break;
					if(uSecondValue > 60)
						break;
					uValue += uSecondValue * 60;
				}

				theStatus = 0;
			}while(0);

			if(theStatus == 1)
			{
				AfxMessageBox("第二个时间格式有误,请重新输入!");
				return;
			}
			if(!flag)
				uValue = -uValue;

			m_ptrain->GetTrainInfo()->m_data.eborder_actual_time = 
				m_ptrain->GetTrainInfo()->m_data.eborder_plan_time + uValue;
		}


		if((m_sborder != m_ptrain->GetTrainInfo()->m_data.sborder_actual_time) 
			||(m_sborder != m_ptrain->GetTrainInfo()->m_data.sborder_actual_time))
		{
			GpcLogTrainMessage(0,0,"修改 %s 分界口/区段站早晚点信息 sborder:%d->%d eborder:%d->%d", 
				m_ptrain->GetTrainID(),
				m_sborder, m_ptrain->GetTrainInfo()->m_data.sborder_actual_time,
				m_eborder, m_ptrain->GetTrainInfo()->m_data.eborder_actual_time);
		}
	}

	CDialog::OnOK();
}
