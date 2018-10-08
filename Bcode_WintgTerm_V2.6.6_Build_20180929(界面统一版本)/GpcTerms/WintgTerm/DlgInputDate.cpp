// DlgInputDate.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgInputDate.h"
#include "function.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInputDate dialog


CDlgInputDate::CDlgInputDate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputDate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInputDate)
	m_inputDate = 0;
	m_inputBanbie = 0;
	//}}AFX_DATA_INIT
}


void CDlgInputDate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInputDate)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInputDate, CDialog)
	//{{AFX_MSG_MAP(CDlgInputDate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInputDate message handlers

BOOL CDlgInputDate::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComboBox* pBox=(CComboBox*)GetDlgItem(IDC_COMBO1);
	if(pBox)
	{
		int nRt=0;
		
		nRt=pBox->AddString("第一班");
		pBox->SetItemData(nRt, 1);

		nRt=pBox->AddString("第二班");
		pBox->SetItemData(nRt, 2);

		pBox->SetCurSel(0);
	}
	// TODO: Add extra initialization here
	m_inputBanbie = 1;
	m_inputDate = CTime::GetCurrentTime();

	//////////////////////////////////////////////////////////////////////////
	CTime t = CTime::GetCurrentTime();
	CTime t_last = CTime(t.GetTime()-24*3600);
 
	long m_usCurrentTime = (t.GetHour( ) * 60 + t.GetMinute( )) * 60 + t.GetSecond( );
	long m_usCurrentDate = (t.GetYear( ) - 1990) * 380 + (t.GetMonth( ) - 1) * 31
		+ t.GetDay( ) - 1;
	CEdit *pEdit;
	CString strTime;
	pEdit = (CEdit*)GetDlgItem(IDC_DATE);
    strTime = OsConvertDatetoString(m_usCurrentDate);
    pEdit->SetWindowText(strTime);
	//////////////////////////////////////////////////////////////////////////
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInputDate::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	////////////////
	CComboBox* pBox=(CComboBox*)GetDlgItem(IDC_COMBO1);
	if(pBox)
	{
		int nRt=pBox->GetCurSel();
		if(nRt<0)
		{
			AfxMessageBox("请选择班别！！");
			return;
		}

		m_inputBanbie = pBox->GetItemData(nRt);
	}

	CEdit *pEdit;
	CString strDate;
	pEdit = (CEdit*)GetDlgItem(IDC_DATE);
	pEdit->GetWindowText(strDate);
	
	long usStartDate;
	if(strDate.IsEmpty())
	{
		AfxMessageBox("请输入正确的日期");
		return;
		
	}
    usStartDate = OsConvertStringtoDate((char*)(LPCTSTR)strDate);
	if(usStartDate <= 0){
		AfxMessageBox("请输入正确的日期");
        return;
	}
 
	m_inputDate = CTime(long_time(usStartDate,0));
	///////////////
	CDialog::OnOK();
}
