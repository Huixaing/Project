// ProhibitMeetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "ProhibitMeetDlg.h"
#include ".\prohibitmeetdlg.h"


// CProhibitMeetDlg 对话框
IMPLEMENT_DYNAMIC(CProhibitMeetDlg, CDialog)
CProhibitMeetDlg::CProhibitMeetDlg(CWnd* pParent)
	: CDialog(CProhibitMeetDlg::IDD, pParent)
{
	m_train_index = 0;
	m_pDoc = NULL;
	m_pTrain = NULL;
}

CProhibitMeetDlg::~CProhibitMeetDlg()
{
}

void CProhibitMeetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_COMBO1, mBeginSta);
	DDX_Control(pDX, IDC_COMBO3, mEndSta);
	DDX_Control(pDX, IDC_COMBO2, mNoMeet);
}


BEGIN_MESSAGE_MAP(CProhibitMeetDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedBtnUpdate)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedBtnDel)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnSelListRow)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CProhibitMeetDlg 消息处理程序
BOOL CProhibitMeetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//D 0: 客 1 货 2
	BYTE nNotMeetInfoCount=m_pDoc->m_ptgctrl->mConfigTg.getNotMeetInfoCount();
	for(BYTE i=0; i<nNotMeetInfoCount; i++)
	{
		NotMeetInfo info;
		if(!m_pDoc->m_ptgctrl->mConfigTg.getNotMeetInfoByNo(i, info))
		{
			break;
		}
		int idx=mNoMeet.AddString(info.sName);
		mNoMeet.SetItemData(idx, info.nNoMeetFlag);
	}

	DWORD style = m_list.GetExtendedStyle();
	style |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(style);

	m_list.InsertColumn(0, "序号", LVCFMT_LEFT,40);
	m_list.InsertColumn(1, "开始站", LVCFMT_LEFT,90);
	m_list.InsertColumn(2, "结束站", LVCFMT_LEFT,90);
	m_list.InsertColumn(3, "禁会描述", LVCFMT_LEFT,120);

	if(!m_pDoc->IsCanEditTrain())
	{
		CWnd* pWnd = GetDlgItem(IDOK);
		if(pWnd)
			pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BUTTON1);
		if(pWnd)
			pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BUTTON2);
		if(pWnd)
			pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BUTTON3);
		if(pWnd)
			pWnd->EnableWindow(FALSE);
	}

	m_pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL != m_pTrain)
	{
		m_train = *m_pTrain;
		m_pTrain = &m_train;
	
		int row1=0,row2=0;
		for(int idx = 0; idx < m_pTrain->myRecord.size(); idx++)
		{
			CString staName = m_pDoc->GetStationName(m_pTrain->myRecord[idx].station);
			if(idx != 0)
			{
				mEndSta.AddString(staName);
				mEndSta.SetItemData(row1++,idx);
			}
			if(idx != m_pTrain->myRecord.size()-1)
			{
				mBeginSta.AddString(staName);
				mBeginSta.SetItemData(row2++,idx);
			}
		}
		ShowMeet();
	}
	SetCenterPosition();
	return TRUE;
}

void CProhibitMeetDlg::SetCenterPosition()
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

void CProhibitMeetDlg::ShowMeet()
{
	m_list.DeleteAllItems();

	if(NULL == m_pTrain)
		return;

	int row = 0, bsta = 0, esta = 0;
	WORD nNotMeetFlag = 0;
	for(int idx = 0; idx < m_pTrain->myRecord.size(); idx++)
	{
		if(nNotMeetFlag != (m_pTrain->myRecord[idx].no_meet_reason & 0xffff))
		{
			if(nNotMeetFlag != 0)
			{
				esta = m_pTrain->myRecord[idx].station;
				
				// 添加信息
				CString itemText;
				itemText.Format("%d", row+1);

				m_list.InsertItem(row, itemText);

				// bSta
				itemText = m_pDoc->GetStationName(bsta);
				m_list.SetItemText(row, 1, itemText);
				
				// esta
				itemText = m_pDoc->GetStationName(esta);
				m_list.SetItemText(row, 2, itemText);
  
				// text
				itemText = m_pDoc->m_ptgctrl->mConfigTg.getNotMeetName(nNotMeetFlag);
				m_list.SetItemData(row, nNotMeetFlag);
				m_list.SetItemText(row++, 3, itemText);
			}
			bsta = m_pTrain->myRecord[idx].station;
			nNotMeetFlag = (m_pTrain->myRecord[idx].no_meet_reason & 0xffff);
		}
	}
}

void CProhibitMeetDlg::OnBnClickedBtnAdd()
{
	SetValue();
}

void CProhibitMeetDlg::OnBnClickedBtnUpdate()
{
	if(mBIdx == -1 || mEIdx == -1 || nNotMeetFlag == 0)
	{
		AfxMessageBox("请选中要修改的禁会");
		return;
	}
	mBIdx = mBeginSta.GetCurSel();
	mEIdx = mEndSta.GetCurSel();
	ResetValue();

	SetValue();
	ShowMeet();
}

void CProhibitMeetDlg::OnBnClickedBtnDel()
{
	// TODO: 在此添加控件通知处理程序代码
	if(mBIdx == -1 || mEIdx == -1 || nNotMeetFlag == 0)
	{
		AfxMessageBox("请选中要删除的禁会");
		return;
	}

	ResetValue();
}

void CProhibitMeetDlg::SetValue()
{
	if(NULL == m_pTrain)
		return;

	int nBSel = mBeginSta.GetCurSel();
	int nESel = mEndSta.GetCurSel();
	int nMSel = mNoMeet.GetCurSel();
	if(nBSel == -1 || nESel == -1 || nMSel == -1)
		return;

	DWORD_PTR IdxB = mBeginSta.GetItemData(nBSel);
	DWORD_PTR IdxE = mEndSta.GetItemData(nESel);
	if(IdxB >= IdxE)
	{
		AfxMessageBox("开始站必须在结束站之前, 两者不能相等");
		return;
	}
	
	DWORD_PTR IdxM = mNoMeet.GetItemData(nMSel);
	for(DWORD_PTR i = IdxB; i < IdxE; i++)
		m_pTrain->myRecord[i].no_meet_reason = IdxM;
	ShowMeet();
}

//	int mBIdx, mEIdx;
void CProhibitMeetDlg::ResetValue()
{
	if(NULL == m_pTrain)
		return;

	for(int i = mBIdx; i < mEIdx; i++)
		m_pTrain->myRecord[i].no_meet_reason = 0;

	ShowMeet();
}
void CProhibitMeetDlg::OnSelListRow(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	mBIdx = -1, mEIdx = -1;
	nNotMeetFlag = 0;
	for(int idx = 0; idx < m_list.GetItemCount(); idx++)
	{
		if(m_list.GetItemState(idx, LVIS_SELECTED) == LVIS_SELECTED)
		{
			CString tmp;
			CString bStaName;

			// bSta
			bStaName = m_list.GetItemText(idx, 1);
			for(int i = 0; i < mBeginSta.GetCount(); i++)
			{
				mBeginSta.GetLBText(i,tmp);
				if(tmp == bStaName)
				{
					mBIdx = mBeginSta.GetItemData(i);
					mBeginSta.SetCurSel(i);
					break;
				}
			}

			// eSta
			bStaName = m_list.GetItemText(idx, 2);
			for(int i = 0; i < mEndSta.GetCount(); i++)
			{
				mEndSta.GetLBText(i,tmp);
				if(tmp == bStaName)
				{
					mEIdx = mEndSta.GetItemData(i);
					mEndSta.SetCurSel(i);
					break;
				}
			}

			// flag
			nNotMeetFlag = m_list.GetItemData(idx);
			int iSel=-1;
			for(int i=0; i<mNoMeet.GetCount(); i++)
			{
				WORD nFlag=mNoMeet.GetItemData(i);
				if(nFlag==nNotMeetFlag)
				{
					iSel=i;
					break;
				}
			}
			mNoMeet.SetCurSel(iSel);
			
			break;
		}
	}
}

void CProhibitMeetDlg::SendTrainUpdate()
{
	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("请注意: 该车可能已经删除, 修改禁会信息失败", "错误");
		return;
	}

	m_pDoc->sysprint(4,0,"[TG] CTGForm::OnCannotMeet,设置列车禁会");
	if(NULL!=m_pTrain)
	{
		m_pDoc->SendUpdateTrain(m_train);
	}
}

void CProhibitMeetDlg::OnBnClickedOk()
{
	SendTrainUpdate();
	this->DestroyWindow();
}

void CProhibitMeetDlg::PostNcDestroy()
{
	delete this;
	CDialog::PostNcDestroy();
}

void CProhibitMeetDlg::OnBnClickedCancel()
{
	this->DestroyWindow();
}
