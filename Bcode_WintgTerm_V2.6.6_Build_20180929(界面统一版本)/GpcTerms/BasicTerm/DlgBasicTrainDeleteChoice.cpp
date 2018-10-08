// DlgBasicTrainDeleteChoice.cpp : implementation file
//

#include "stdafx.h"
#include "DlgBasicTrainDeleteChoice.h"
#include ".\dlgbasictraindeletechoice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBasicTrainDeleteChoice dialog
CDlgBasicTrainDeleteChoice::CDlgBasicTrainDeleteChoice(int nChoice, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBasicTrainDeleteChoice::IDD, pParent)
{
	m_nChoice = nChoice;
	if(m_nChoice!=1 && m_nChoice!=2 && m_nChoice!=3)
		m_nChoice=1;
}

CDlgBasicTrainDeleteChoice::~CDlgBasicTrainDeleteChoice()
{
}

void CDlgBasicTrainDeleteChoice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO1, btn_all);
	DDX_Control(pDX, IDC_RADIO2, btn_pess);
	DDX_Control(pDX, IDC_RADIO3, btn_freg);
}


BEGIN_MESSAGE_MAP(CDlgBasicTrainDeleteChoice, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBasicTrainDeleteChoice message handlers
BOOL CDlgBasicTrainDeleteChoice::OnInitDialog() 
{
	CDialog::OnInitDialog();
  	SetButton();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBasicTrainDeleteChoice::SetButton()
{
	if(m_nChoice==2)
	{
		btn_all.SetCheck(FALSE);
		btn_pess.SetCheck(TRUE);
		btn_freg.SetCheck(FALSE);
	}
	else if(m_nChoice==3)
	{
		btn_all.SetCheck(FALSE);
		btn_pess.SetCheck(FALSE);
		btn_freg.SetCheck(TRUE);
	}
	else
	{
		m_nChoice=1;
		btn_all.SetCheck(TRUE);
		btn_pess.SetCheck(FALSE);
		btn_freg.SetCheck(FALSE);
	}
}

void CDlgBasicTrainDeleteChoice::OnClose()
{
	m_nChoice=0;
	OnCancel();
}

void CDlgBasicTrainDeleteChoice::OnOK() 
{
	// TODO: Add extra validation here
	if(m_nChoice!=1 && m_nChoice!=2 && m_nChoice!=3)
	{
		MessageBox("请选择删除基本图车次类型", "提示");
		return;
	}
  
	CDialog::OnOK();
}

void CDlgBasicTrainDeleteChoice::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nChoice=1;
	SetButton();
}

void CDlgBasicTrainDeleteChoice::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nChoice=2;
	SetButton();
}

void CDlgBasicTrainDeleteChoice::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nChoice=3;
	SetButton();
}
