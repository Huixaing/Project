// DlgTDMSBasicTip.cpp : implementation file
//

#include "stdafx.h"
#include "DlgTDMSBasicTip.h"
#include ".\dlgtdmsbasictip.h"
#include "tg_function_config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CDlgTDMSBasicTip::CDlgTDMSBasicTip(CWnd* pParent)
	: CDialog(CDlgTDMSBasicTip::IDD, pParent)
{
	m_bSelTDMSTrainGenBasic=false;
}

CDlgTDMSBasicTip::~CDlgTDMSBasicTip()
{
}

void CDlgTDMSBasicTip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_check);
}

BEGIN_MESSAGE_MAP(CDlgTDMSBasicTip, CDialog)
	ON_BN_CLICKED(IDC_CHECK1, OnSelTDMSStateChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTDMSBasicTip message handlers
BOOL CDlgTDMSBasicTip::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(GetTGFunctionConfig()->IsEnableSelGenBasicTrain())
	{
		if(m_bSelTDMSTrainGenBasic)
			m_check.SetCheck(1);
		else
			m_check.SetCheck(0);
	}
	else
	{
		m_check.ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTDMSBasicTip::OnSelTDMSStateChange()
{
	// TODO: 在此添加控件通知处理程序代码
	if(GetTGFunctionConfig()->IsEnableSelGenBasicTrain())
	{
		if(m_check.GetCheck())
			m_bSelTDMSTrainGenBasic=true;
		else
			m_bSelTDMSTrainGenBasic=false;
	}
	else
	{
		m_bSelTDMSTrainGenBasic=false;
	}
}
