// InsertSimpleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InsertSimpleDlg.h"
#include ".\insertsimpledlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInsertSimpleDlg dialog


CInsertSimpleDlg::CInsertSimpleDlg(LPCTSTR title,CWnd* pParent,int flag)
	: CDialog(CInsertSimpleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInsertSimpleDlg)
	m_sStaionid = _T("");
	
	//}}AFX_DATA_INIT
	this->title = title;
	m_flag = flag;
}


void CInsertSimpleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertSimpleDlg)
	DDX_Text(pDX, IDC_EDIT1, m_sStaionid);
	DDV_MaxChars(pDX, m_sStaionid,  9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInsertSimpleDlg, CDialog)
	//{{AFX_MSG_MAP(CInsertSimpleDlg)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertSimpleDlg message handlers

BOOL CInsertSimpleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(title);
    if(m_flag == 5)//INSERT_TRAIN_RETURN)	
	{
		CWnd* pWnd = GetDlgItem(IDC_EDIT3);
		if(NULL != pWnd)
			pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_STATIC2);
		if(NULL != pWnd)
			pWnd->ShowWindow(SW_SHOW);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CInsertSimpleDlg::OnOK() 
{
	UpdateData(TRUE);

	if( m_sStaionid.Find(" ")>=0)
	{
		MessageBox("车次中有空格,请新输入!");
		return  ;
	}
	UpdateData(FALSE);
	if(m_sStaionid.GetLength( ) == 0){
		AfxMessageBox("请输入车次号");	
	    return;
	}

	CWnd* pWnd = GetDlgItem(IDC_EDIT3);
	if(NULL != pWnd)
		pWnd->GetWindowText(m_sRetTrainId);

	if(m_sRetTrainId.GetLength( ) == 0){
		AfxMessageBox("请输入返回车次号");	
	    return;
	}
	CDialog::OnOK();
}

void CInsertSimpleDlg::OnEnChangeEdit1()
{
	CString m_strTrainId;
	CWnd* pWnd = GetDlgItem(IDC_EDIT1);
	if(NULL != pWnd)
		pWnd->GetWindowText(m_strTrainId);

	if(m_strTrainId.IsEmpty())
		return;

	CString tempTrainId;
	CString temp;

	int len = m_strTrainId.GetLength();
	for(int i= len-1;i>=0;i--)
	{
		char lastchar = m_strTrainId.GetAt(i);
		if(lastchar < '0' || lastchar > '9')
			break;
	}
	tempTrainId = m_strTrainId.Right(len - i - 1);
	int TrainIdNum = atoi(tempTrainId);
	tempTrainId = m_strTrainId.Left(len - tempTrainId.GetLength());
	if((TrainIdNum%2) == 0)
	{
		if(TrainIdNum > 0)
			temp.Format("%d", TrainIdNum-1);
		else
			temp.Format("%d", TrainIdNum+1);
		tempTrainId += temp;
	}
	else
	{
		temp.Format("%d", TrainIdNum+1);
		tempTrainId += temp;
	}

	pWnd = GetDlgItem(IDC_EDIT3);
	if(NULL != pWnd)
		pWnd->SetWindowText(tempTrainId);
}

		
