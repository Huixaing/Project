// TrainPartRunDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainPartRunDlg.h"
#include ".\TrainPartRunDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrainPartRunDlg dialog

CTrainPartRunDlg::CTrainPartRunDlg(CTGDoc *pDoc, CWnd* pParent, int station, int station_next)
	: CDialog(CTrainPartRunDlg::IDD, pParent)
{
	m_sStaionid = _T("");
	
	m_pDoc = pDoc;
	m_station = station;
	m_station_next = station_next;

	m_start_station=0;
	m_end_station=0;
}

void CTrainPartRunDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, m_sStaionid);
	DDV_MaxChars(pDX, m_sStaionid,  9);
	DDX_Control(pDX, IDC_COMBO1, m_next_comboBox);
	DDX_Control(pDX, IDC_COMBO2, m_cur_comboBox);
}

BEGIN_MESSAGE_MAP(CTrainPartRunDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelHelpStartStation)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelHelpDirectStation)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainPartRunDlg message handlers

BOOL CTrainPartRunDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here IDC_EDIT2
	int iSel;
	CString str;
	if(m_station!=0)
	{
		str=m_pDoc->GetStationName(m_station);
		iSel=m_next_comboBox.AddString(str);
		m_next_comboBox.SetItemData(iSel, m_station);
		iSel=m_cur_comboBox.AddString(str);
		m_cur_comboBox.SetItemData(iSel, m_station);
	}
	if(m_station_next!=0)
	{
		str=m_pDoc->GetStationName(m_station_next);
		iSel=m_next_comboBox.AddString(str);
		m_next_comboBox.SetItemData(iSel, m_station_next);
		iSel=m_cur_comboBox.AddString(str);
		m_cur_comboBox.SetItemData(iSel, m_station_next);
	}
	m_cur_comboBox.SetCurSel(-1);
	m_next_comboBox.SetCurSel(-1);
	
	CWnd*pWnd = GetDlgItem(IDC_EDIT3);
	if(NULL != pWnd)
		pWnd->ShowWindow(SW_SHOW);

	pWnd = GetDlgItem(IDC_STATIC2);
	if(NULL != pWnd)
		pWnd->ShowWindow(SW_SHOW);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTrainPartRunDlg::OnOK() 
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

	int nSel=m_cur_comboBox.GetCurSel();
	if(nSel!=-1)
		m_start_station=m_cur_comboBox.GetItemData(nSel);
	else
		m_start_station=0;
	
	nSel=m_next_comboBox.GetCurSel();
	if(nSel!=-1)
		m_end_station=m_next_comboBox.GetItemData(nSel);
	else
		m_end_station=0;

	if(m_start_station==0)
	{
		AfxMessageBox("请选择救援列车始发站");	
	    return;
	}

	if(m_end_station==0)
	{
		AfxMessageBox("请选择救援列车终到区间方向站");	
	    return;
	}

	if(m_end_station==m_start_station)
	{
		AfxMessageBox("救援列车始发站和终到区间前方站不能是同一站");	
	    return;
	}

	CDialog::OnOK();
}

void CTrainPartRunDlg::OnEnChangeEdit1()
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
	
void CTrainPartRunDlg::OnSelHelpStartStation()
{
	// TODO: 在此添加控件通知处理程序代码
	int nSel=m_cur_comboBox.GetCurSel();
	if(nSel==-1)
		return;
		
	int start_station=m_cur_comboBox.GetItemData(nSel);
	if(start_station==m_station)
	{
		for(int n=0; n<m_next_comboBox.GetCount(); n++)
		{
			int next_station=m_next_comboBox.GetItemData(n);
			if(next_station==m_station_next)
			{
				m_next_comboBox.SetCurSel(n);
				break;
			}
		}
	}
	if(start_station==m_station_next)
	{
		for(int n=0; n<m_next_comboBox.GetCount(); n++)
		{
			int next_station=m_next_comboBox.GetItemData(n);
			if(next_station==m_station)
			{
				m_next_comboBox.SetCurSel(n);
				break;
			}
		}
	}
}

void CTrainPartRunDlg::OnSelHelpDirectStation()
{
	// TODO: 在此添加控件通知处理程序代码
	int nSel=m_next_comboBox.GetCurSel();
	if(nSel==-1)
		return;
		
	int start_station=m_next_comboBox.GetItemData(nSel);
	if(start_station==m_station)
	{
		for(int n=0; n<m_cur_comboBox.GetCount(); n++)
		{
			int next_station=m_cur_comboBox.GetItemData(n);
			if(next_station==m_station_next)
			{
				m_cur_comboBox.SetCurSel(n);
				break;
			}
		}
	}
	if(start_station==m_station_next)
	{
		for(int n=0; n<m_cur_comboBox.GetCount(); n++)
		{
			int next_station=m_cur_comboBox.GetItemData(n);
			if(next_station==m_station)
			{
				m_cur_comboBox.SetCurSel(n);
				break;
			}
		}
	}
}
