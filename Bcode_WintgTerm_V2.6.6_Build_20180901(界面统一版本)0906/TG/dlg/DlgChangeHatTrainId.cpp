// DlgChangeHatTrainId.cpp : implementation file
//

#include "stdafx.h"
#include "tgdoc.h"
#include "DlgChangeHatTrainId.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeHatTrainId dialog


CDlgChangeHatTrainId::CDlgChangeHatTrainId(CString trainid,CString hatid, CWnd* pParent)
	: CDialog(CDlgChangeHatTrainId::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChangeHatTrainId)
	m_strTrainid = _T("");
	m_strHatid = _T("");
	//}}AFX_DATA_INIT

	m_strTrainid = trainid;
	m_strHatid = hatid;
}


void CDlgChangeHatTrainId::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChangeHatTrainId)
	DDX_Text(pDX, IDC_CHANGE_TRAINID, m_strTrainid);
	DDV_MaxChars(pDX, m_strTrainid, 9);
	DDX_Text(pDX, IDC_CHANGE_HATID, m_strHatid);
	DDV_MaxChars(pDX, m_strHatid, 9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChangeHatTrainId, CDialog)
	//{{AFX_MSG_MAP(CDlgChangeHatTrainId)
	ON_BN_CLICKED(IDC_CHANGE_ACK, OnChangeAck)
	ON_BN_CLICKED(IDC_CHANGE_CANCEL, OnChangeCancel)
	ON_BN_CLICKED(IDC_CHANGE_CANCEL_ACK,  OnChangeCancelACK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeHatTrainId message handlers

void CDlgChangeHatTrainId::OnChangeAck() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SendTrainUpdate();
	CDialog::OnOK();
}

void CDlgChangeHatTrainId::OnChangeCancel() 
{
	CDialog::OnCancel();
}


BOOL CDlgChangeHatTrainId::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetCenterPosition();
		// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChangeHatTrainId::SetCenterPosition()
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

void CDlgChangeHatTrainId::SendTrainUpdate()
{
	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("请注意: 该车可能已经删除, 修改戴帽车次失败", "错误");
		return;
	}

	CTG_TRAIN tmpTrain=*pTrain;
	if(this->m_strHatid != m_strTrainid)
	{
		m_pDoc->sysprint(4,0,"修改戴帽车次号:%s --> %s",m_strTrainid, this->m_strHatid);
		for(int idx = 0; idx < tmpTrain.GetRecordNum(); idx++)
		{
			if(tmpTrain.myRecord[idx].original_arrive_train_id.IsEmpty())
			    tmpTrain.myRecord[idx].original_arrive_train_id = tmpTrain.myRecord[idx].arrive_train_id;
			if(tmpTrain.myRecord[idx].original_depart_train_id.IsEmpty())
			    tmpTrain.myRecord[idx].original_depart_train_id = tmpTrain.myRecord[idx].depart_train_id;
			tmpTrain.myRecord[idx].arrive_train_id = this->m_strHatid;
			tmpTrain.myRecord[idx].depart_train_id = this->m_strHatid;
		}
		m_pDoc->SendUpdateTrain(tmpTrain, SCHD_UPDATE, HAT_TRAINID, 0, 0, this->m_strHatid);
	}
}

BOOL CDlgChangeHatTrainId::PreTranslateMessage(MSG* lpMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//add for pressing return button to certain change train id
    //兰州要求更改车次号后，点击确定按钮直接确定
	if ((lpMsg->message == WM_KEYDOWN) &&(lpMsg->wParam == VK_RETURN))
	{
		OnChangeAck();
		return TRUE;
	}

	return CDialog::PreTranslateMessage(lpMsg);
}

void CDlgChangeHatTrainId::OnChangeCancelACK()
{
	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("请注意: 该车可能已经删除, 取消戴帽车次失败", "错误");
		return;
	}

	CString text;
	text.Format("确认取消 %s 次列车的戴帽车次?", m_strTrainid);
	if (IDYES == MessageBox(text,"确定",MB_YESNO))
	{
		CTG_TRAIN tmpTrain=*pTrain;
		if(this->m_strHatid != m_strTrainid)
		{
			m_pDoc->sysprint(4,0,"取消戴帽车次,恢复车次号:%s",m_strTrainid);
			for(int idx = 0; idx < tmpTrain.GetRecordNum(); idx++)
			{
				tmpTrain.myRecord[idx].original_arrive_train_id = "";
				tmpTrain.myRecord[idx].original_depart_train_id = "";
				tmpTrain.myRecord[idx].arrive_train_id = m_strTrainid;
				tmpTrain.myRecord[idx].depart_train_id = m_strTrainid;
			}
			m_pDoc->SendUpdateTrain(tmpTrain, SCHD_UPDATE, HAT_TRAINID, 0, 0, m_strTrainid);
		}
	}

	CDialog::OnOK();
}
