// FindTrainDlg.cpp : implementation file
//

#include "stdafx.h"

#include "FindTrainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindTrainDlg dialog


CFindTrainDlg::CFindTrainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindTrainDlg::IDD, pParent)
	, train_index(0)
{
	//{{AFX_DATA_INIT(CFindTrainDlg)
	train_id = _T("");
	train_index = 0;
	m_train_id = _T("");
	m_train_index = 0;
	//}}AFX_DATA_INIT
}


void CFindTrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindTrainDlg)
	DDX_Text(pDX, IDC_TRAIN_ID, train_id);
	DDV_MaxChars(pDX, train_id, 9);
	DDX_Control(pDX, IDC_CHECK1, m_findByTrainIndex);
	DDX_Text(pDX, IDC_TRAIN_INDEX, train_index);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindTrainDlg, CDialog)
	//{{AFX_MSG_MAP(CFindTrainDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindTrainDlg message handlers

void CFindTrainDlg::OnOK() 
{
	UpdateData(TRUE);
	train_id = ChangeAlpha(train_id);

	int nFindByTrainIndex = m_findByTrainIndex.GetCheck();
	if(nFindByTrainIndex==1)
	{
		if(train_index == 0)
		{
			AfxMessageBox("请输入要查找的列车索引!");
			return;
		}
		m_train_id="";
		m_train_index=train_index;
	}
	else
	{
		if(train_id.GetLength() == 0)
		{
			AfxMessageBox("请输入车次号!");
			return;
		}
		m_train_id=train_id;
		m_train_index=0;
	}

	CDialog::OnOK();
}

BOOL CFindTrainDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
