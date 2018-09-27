// TransportInfo.cpp : implementation file
//

#include "stdafx.h"
#include "TransportInfo.h"
#include ".\transportinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransportInfo dialog


CTransportInfo::CTransportInfo(CWnd* pParent)
	: CDialog(CTransportInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransportInfo)
	//}}AFX_DATA_INIT
	m_bEdit = false;
	m_train_index = 0;
	m_rec_index = -1;
	m_pDoc = NULL;
}


void CTransportInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransportInfo)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransportInfo, CDialog)
	//{{AFX_MSG_MAP(CTransportInfo)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransportInfo message handlers

BOOL CTransportInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();

    CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_STATION);	
	if(pEdit)
	{
		if(m_opmsSetStationText == 0)
		    pEdit->SetWindowText(m_strStation);
		else
			pEdit->SetWindowText("");
		if(!m_bEdit)
			pEdit->EnableWindow(FALSE);
	}

    pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ABS);	
	if(pEdit)
	{
		pEdit->SetWindowText(m_strAbs);
		if(!m_bEdit)
			pEdit->EnableWindow(FALSE);
	}

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_WARNING);	
	if(pEdit)
	{
		pEdit->SetWindowText(m_strOpmsStationText);
		pEdit->SetReadOnly();
	}
   
	if(!m_bEdit)
	{
		CWnd *pWnd = (CWnd*)GetDlgItem(IDOK);	
		if(pWnd)
		   pWnd->EnableWindow(FALSE);
	}

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TICKET);	
	if(pEdit)
	{
		pEdit->SetWindowText(m_strTicket);
		pEdit->EnableWindow(FALSE);
	}

	SetCenterPosition();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransportInfo::SetCenterPosition()
{
	CRect rect, rect1;
	CWnd* pWnd = this->GetParent();
	if(NULL != pWnd)
	{
		pWnd->GetWindowRect(&rect);
		this->GetWindowRect(&rect1);
		int x = rect.CenterPoint().x-rect1.Width()/2;
		int y = rect.CenterPoint().y-rect1.Height()/2;
		this->SetWindowPos(pWnd, x,  y, rect1.Width(), rect1.Height(), SWP_SHOWWINDOW);
	}
}

void CTransportInfo::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_STATION);	
	if(pEdit)
	{
		CString tmpStationText;
		pEdit->GetWindowText(tmpStationText);
		if(tmpStationText.GetLength() > 250)
		{
			AfxMessageBox("请注意: 你输入的车站信息超过了125个字!,请缩减内容");
			return;
		}
		
		if(!tmpStationText.IsEmpty())
		    m_opmsSetStationText=0;
		else
			m_opmsSetStationText=1;
		m_strStation = tmpStationText;
	}
    pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ABS);	
	if(pEdit)
	{
		pEdit->GetWindowText(m_strAbs);
		if(m_strAbs.GetLength() > 250)
		{
			AfxMessageBox("请注意: 你输入的区间信息超过了125个字!,请缩减内容");
			return;
		}
	}
	SendTrainUpdate();
}

void CTransportInfo::SendTrainUpdate()
{
	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("请注意: 该车可能已经删除, 修改运输信息失败", "错误");
		return;
	}
	CTG_TRAIN train=*pTrain;   // 不能直接该内存数据
	TgTRAIN_RECORD *rec = train.GetRecordByRecIndex(m_rec_index);
	if(NULL != rec)
	{
		rec->szOpmsStationText   = this->m_strOpmsStationText;
		rec->opmsGiveStationText = this->m_opmsSetStationText;
		rec->szStationText       = this->m_strStation;
		rec->szAbsText           = this->m_strAbs;

		m_pDoc->sysprint(4,0,"[TG] CTGForm::OnTransportInfo 设置运输信息");
		m_pDoc->SendUpdateTrain(train);
	}
	CDialog::OnOK();
}

void CTransportInfo::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

BOOL CTransportInfo::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ((pMsg->message == WM_KEYDOWN) &&(pMsg->wParam == VK_RETURN))
	{
		OnBnClickedOk();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
