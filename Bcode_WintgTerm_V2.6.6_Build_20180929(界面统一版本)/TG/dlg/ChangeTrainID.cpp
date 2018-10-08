// ChangeTrainID.cpp : implementation file
//

#include "stdafx.h"
#include "IF.h"

#include "resource.h"

#include "DlgMessageBox.h"
#include ".\ChangeTrainID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeTrainID dialog
CChangeTrainID::CChangeTrainID(CString strOldTrainId,CString title, CWnd* pParent /*=NULL*/)
	: CDialog(CChangeTrainID::IDD, pParent)
{
	m_strNewTrainId = _T("");
	m_pDoc = NULL;
	m_oldTrainId = strOldTrainId;
	m_nFlag = 0;
	m_nSelRecIdx = -1;
	m_train_index = 0;
	m_title = title;
}

void CChangeTrainID::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeTrainID)
	DDX_Text(pDX, IDC_CHANGE_NEWID, m_strNewTrainId);
	DDV_MaxChars(pDX, m_strNewTrainId, 9);
	DDX_Text(pDX, IDC_CHANGE_OLDID, m_oldTrainId);
	DDV_MaxChars(pDX, m_oldTrainId, 9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeTrainID, CDialog)
	//{{AFX_MSG_MAP(CChangeTrainID)
	ON_BN_CLICKED(IDC_CHANGE_ACK, OnChangeAck)
	ON_BN_CLICKED(IDC_CHANGE_CANCEL, OnChangeCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeTrainID message handlers

void CChangeTrainID::OnChangeAck() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString strText = m_strNewTrainId;
    //车次中不允许有空格

	if(strText.Find(" ")>=0)
	{
		strText = "车次中有空格,请新输入!";
        CDlgMessageBox dlg(strText);
		dlg.DoModal();
		return;
	}
	if(strText.Find("O")>0)
	{
		strText = "车次中有字母o,确定吗?";
        CDlgMessageBox dlg(strText);
		if(dlg.DoModal() != IDOK)
		  return;
	}

	if(m_strNewTrainId.GetLength( ) > 0)
		SendTrainUpdate();
}

void CChangeTrainID::SendTrainUpdate()
{
	if(3 == m_nFlag)
	{
		CDialog::OnOK();
		return;
	}

	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("请注意: 该车可能已经删除, 修改车次失败", "错误");
		return;
	}
	
	CTG_TRAIN tmptrain=*pTrain;
	// 修改车次
	if(1 == m_nFlag)
	{
		m_pDoc->sysprint(4,0,"修改车次 %s->%s", m_oldTrainId, m_strNewTrainId);
		tmptrain.myTrainInfo.train_name=this->m_strNewTrainId;
		for(int i = 0; i < tmptrain.GetRecordNum(); i++)
		{
			tmptrain.myRecord[i].arrive_train_id = this->m_strNewTrainId; 
			tmptrain.myRecord[i].depart_train_id = this->m_strNewTrainId; 
		}

		m_pDoc->SendUpdateTrain(tmptrain, SCHD_UPDATE, CHANGE_TRAINID, 0, 0, this->m_strNewTrainId);
	}
	else if(2 == m_nFlag)
	{
		bool bSelAbs = false;
		if(m_nSelRectIdx < tmptrain.m_nPointCount)
		{
			if(tmptrain.m_pPerPointInfo[m_nSelRectIdx].start_depart_flag == ABS_START_POSITION ||
			   tmptrain.m_pPerPointInfo[m_nSelRectIdx].start_depart_flag == ABS_ARRIVAL_POSITION ||
			   tmptrain.m_pPerPointInfo[m_nSelRectIdx].start_depart_flag == ABS_DEPART_POSITION)
			{
				bSelAbs = true;
			}
		}

		CString newTrainId = this->m_strNewTrainId;
		CString oldTrainId = m_oldTrainId;

		int idx = m_nSelRecIdx;
		if(bSelAbs)
			idx = m_nSelRecIdx+1;

		for(;idx < tmptrain.GetRecordNum(); idx++)
		{
			if(idx > m_nSelRecIdx)
			{
				tmptrain.myRecord[idx].arrive_train_id = this->m_strNewTrainId;
			}
			tmptrain.myRecord[idx].depart_train_id = this->m_strNewTrainId;
			m_pDoc->sysprint(4,0,"变换折返车次old trainid %s from %d to %d is %s", oldTrainId, m_nSelRecIdx, tmptrain.GetRecordNum(), newTrainId);
		}

		if(m_nSelRecIdx < tmptrain.GetRecordNum())
		    m_pDoc->SendUpdateTrain(tmptrain, SCHD_UPDATE, ZHEFAN_TRAINID, tmptrain.myRecord[m_nSelRecIdx].station, m_nSelRecIdx, this->m_strNewTrainId);
	}

	CDialog::OnOK();
}

void CChangeTrainID::OnChangeCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

BOOL CChangeTrainID::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(!m_pDoc->IsCanEditTrain())
	{
		CWnd* pwnd = GetDlgItem(IDOK);
		if(pwnd)
			pwnd->EnableWindow(FALSE);
	}
	if(!m_title.IsEmpty())
	   SetWindowText(m_title);

	SetCenterPosition();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeTrainID::SetCenterPosition()
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

void CChangeTrainID::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::PostNcDestroy();
}
BOOL CChangeTrainID::PreTranslateMessage(MSG* lpMsg)
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
