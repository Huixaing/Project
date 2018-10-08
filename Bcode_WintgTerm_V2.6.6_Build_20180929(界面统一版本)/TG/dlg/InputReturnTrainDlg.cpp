// ReturnTrainSelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "InputReturnTrainDlg.h"
#include ".\InputReturnTrainDlg.h"
#include "JiaoluTrainSelDlg.h"

// CInputReturnTrainDlg 对话框

IMPLEMENT_DYNAMIC(CInputReturnTrainDlg, CDialog)
CInputReturnTrainDlg::CInputReturnTrainDlg(CWnd* pParent, const CStringArray& list_trainno, const CDWordArray& list_trainindex)
	: CDialog(CInputReturnTrainDlg::IDD, pParent)
{
	m_return_trainindex=0;
	m_list_trainno.RemoveAll();
	m_list_trainindex.RemoveAll();

	m_list_trainno.Append(list_trainno);
	m_list_trainindex.Append(list_trainindex);
}

CInputReturnTrainDlg::~CInputReturnTrainDlg()
{
}

void CInputReturnTrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInputReturnTrainDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

// CInputReturnTrainDlg 消息处理程序
BOOL CInputReturnTrainDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CInputReturnTrainDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTrainID="";
	CWnd* pWnd=(CWnd*)GetDlgItem(IDC_EDIT1);
	if(pWnd)
		pWnd->GetWindowText(strTrainID);

	WORD count=0;
	CStringArray m_list_trainno1;
	CDWordArray m_list_trainindex1;
	for(int i=0; i<m_list_trainno.GetCount()&&m_list_trainindex.GetCount(); i++)
	{
		CString str=m_list_trainno.GetAt(i), str2;
		int pos=str.Find('(');
		if(pos>=0)
			str=str.Left(pos);
		else
			str=str2;

		if(str==strTrainID)
		{
			m_list_trainno1.Add(m_list_trainno.GetAt(i));
			m_list_trainindex1.Add(m_list_trainindex.GetAt(i));
		}
	}
	if(m_list_trainno1.GetCount()==0)
	{
		m_return_trainindex=0;
		AfxMessageBox("找不到你输入的合理前序列车!!");
	}
	else if(m_list_trainno1.GetCount()==1)
	{
		m_return_trainindex=m_list_trainindex1.GetAt(0);
		OnOK();
	}
	else
	{
		AfxMessageBox("根据你输入的车次, 找到多个前序列车, 请选择!!");
		CJiaoluTrainSelDlg dlg(this, m_list_trainno1, m_list_trainindex1);
		if(dlg.DoModal()==IDOK)
		{
			m_return_trainindex=dlg.m_return_trainindex;
			OnOK();
		}
		else
		{
			OnCancel();
		}
	}
}

void CInputReturnTrainDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	m_return_trainindex=0;
	OnCancel();
}
