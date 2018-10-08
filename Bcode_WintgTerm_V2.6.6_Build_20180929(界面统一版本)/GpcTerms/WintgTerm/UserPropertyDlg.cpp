// UserPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UserListDlg.h"
#include "UserPropertyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserPropertyDlg dialog


CUserPropertyDlg::CUserPropertyDlg(bool insert, CWnd* pParent /*=NULL*/)
	: CDialog(CUserPropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserPropertyDlg)
	m_passwd = _T("");
	m_re_passwd = _T("");
	m_propperty = 0;
	m_user_id = _T("");
	m_user_name = _T("");
	m_insert = insert;
	//}}AFX_DATA_INIT
}


void CUserPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserPropertyDlg)
	DDX_Text(pDX, IDC_PASSWD, m_passwd);
	DDV_MaxChars(pDX, m_passwd, 8);
	DDX_Text(pDX, IDC_RE_PASSWD, m_re_passwd);
	DDV_MaxChars(pDX, m_re_passwd, 8);
	DDX_CBIndex(pDX, IDC_PROPERTY_ID, m_propperty);
	DDX_Text(pDX, IDC_USER_ID, m_user_id);
	DDV_MaxChars(pDX, m_user_id, 5);
	DDX_Text(pDX, IDC_USER_NAME, m_user_name);
	DDV_MaxChars(pDX, m_user_name, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserPropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CUserPropertyDlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserPropertyDlg message handlers

void CUserPropertyDlg::OnOK() 
{
	// TODO: Add extra validation here

	if (UpdateData() != TRUE)
		return;	
	if (m_passwd != m_re_passwd)
	{
		AfxMessageBox("ÃÜÂëÊäÈë´íÎó £¡£¡£¡£¡ ");
		GetDlgItem(IDC_PASSWD)->SetFocus();
	}
	else
		CDialog::OnOK();
}

BOOL CUserPropertyDlg::OnInitDialog() 
{
	
	CComboBox *user_type_list;
	user_type_list=(CComboBox *)GetDlgItem(IDC_PROPERTY_ID);
	for (int i=0; i<USER_TYPE_COUNT; i++)
		user_type_list->AddString(USER_TYPE[i]);
	user_type_list->SetCurSel(0);


	CDialog::OnInitDialog();
    
	if (!m_insert)
		((CEdit*)GetDlgItem(IDC_USER_ID))->EnableWindow(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CUserPropertyDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CString pwd="";
	CWnd *pwnd=GetDlgItem(IDC_CHECK_SHOW_PWD);
	CEdit*pedit=(CEdit*)GetDlgItem(IDC_PASSWD);
	if(pedit&&pwnd)
	{
		CRect rect;
	
		pwnd->GetWindowRect(&rect);
		MapWindowPoints(NULL,&point,1);
		if ( rect.PtInRect(point) )
		{		
			pedit->GetWindowText(pwd);
		}	
		pwnd->SetWindowText(pwd);
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CUserPropertyDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd *pwnd=GetDlgItem(IDC_CHECK_SHOW_PWD);
	if(pwnd)
		pwnd->SetWindowText("ÏÔÊ¾Ã÷Âë");
	
	CDialog::OnLButtonUp(nFlags, point);
}
