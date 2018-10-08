// DlgMessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgTimeMessageBox.h"
#include ".\dlgtimemessagebox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTimeMessageBox dialog
BYTE CDlgTimeMessageBox::WindowPosition[15][3];
CDlgTimeMessageBox::CDlgTimeMessageBox(CString sMessage, int nTimeSecond, int x, int y, CWnd* pParent)
	: CDialog(CDlgTimeMessageBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTimeMessageBox)
	m_text = _T("");
	//}}AFX_DATA_INIT
	m_text = sMessage;
	m_nTimeSecond=nTimeSecond;
	mx = x;
	my = y;
	WindowPosition[mx][my]=1;
}


void CDlgTimeMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTimeMessageBox)
	DDX_Text(pDX, IDC_STATIC_MESSAGE, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTimeMessageBox, CDialog)
	//{{AFX_MSG_MAP(CDlgTimeMessageBox)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTimeMessageBox message handlers

BOOL CDlgTimeMessageBox::OnInitDialog() 
{
	CDialog::OnInitDialog();

    UpdateData(FALSE);
    SetTimer(1840, 1000, NULL);
	CString title;
	title.Format("本窗口 %d 秒后自动关闭", m_nTimeSecond);
	this->SetWindowText(title);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTimeMessageBox::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent==1840)
	{
		m_nTimeSecond--;
		if(m_nTimeSecond<=0)
		{
			OnClose();
			return;
		}
		else
		{
			CString title;
			title.Format("本窗口 %d 秒后自动关闭", m_nTimeSecond);
			this->SetWindowText(title);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CDlgTimeMessageBox::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	KillTimer(1840);
	WindowPosition[mx][my]=0;
	m_nTimeSecond=0;
	DestroyWindow();
}

void CDlgTimeMessageBox::PostNcDestroy() 
{
	delete this;
}
