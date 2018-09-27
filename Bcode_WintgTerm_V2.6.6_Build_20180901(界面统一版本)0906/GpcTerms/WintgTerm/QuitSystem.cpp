// QuitSystem.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "QuitSystem.h"


// CQuitSystem 对话框

IMPLEMENT_DYNAMIC(CQuitSystem, CDialog)
CQuitSystem::CQuitSystem(CWnd* pParent /*=NULL*/)
	: CDialog(CQuitSystem::IDD, pParent)
{
}

CQuitSystem::~CQuitSystem()
{
}

void CQuitSystem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuitSystem)
	DDX_Control(pDX, IDC_STATIC_EXIT_BMP, m_logoExit);
	//}}AFX_DATA_MAP
}

BOOL CQuitSystem::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect rect;
	CWnd* pWnd= GetDlgItem(IDC_STATIC_EXIT_BMP);
	pWnd->GetWindowRect(rect);
	
	m_logoExit.SetPicSize(rect,  IDB_BITMAP_EXIT, true);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CQuitSystem, CDialog)
END_MESSAGE_MAP()


// CQuitSystem 消息处理程序
