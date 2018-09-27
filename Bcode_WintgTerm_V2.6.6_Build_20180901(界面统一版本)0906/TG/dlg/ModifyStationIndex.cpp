// ModifyStationIndex.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "ModifyStationIndex.h"
#include ".\modifystationindex.h"


// CModifyStationIndex 对话框

IMPLEMENT_DYNAMIC(CModifyStationIndex, CDialog)
CModifyStationIndex::CModifyStationIndex(CTGDoc *pDoc, CTG_TRAIN* train, CWnd* pParent)
	: CDialog(CModifyStationIndex::IDD, pParent)
{
	m_pDoc = pDoc;
	mTrain = *train;
}

CModifyStationIndex::~CModifyStationIndex()
{
}

void CModifyStationIndex::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_inStationBox);
	DDX_Control(pDX, IDC_COMBO4, m_outStationBox);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CModifyStationIndex, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedModify)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnNMClickList1)
END_MESSAGE_MAP()


// CModifyStationIndex 消息处理程序
BOOL CModifyStationIndex::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_nRecIndex=-1;
	m_inStationBox.ResetContent();
	m_outStationBox.ResetContent();

	CString title;
	title.Format("编辑列车 %s",m_pDoc->GetTrainShowId(mTrain));

	SetWindowText(title);
	
	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(dwStyle);

	m_list.InsertColumn(0, "序号",LVCFMT_LEFT,40);
	m_list.InsertColumn(1, "车站",LVCFMT_LEFT,120);
	m_list.InsertColumn(2, "入口站序",LVCFMT_CENTER,90);
	m_list.InsertColumn(3, "出口站序",LVCFMT_CENTER,90);

	ShowTrain();
	
	if(!m_pDoc->IsCanEditTrain())
	{
		m_inStationBox.EnableWindow(FALSE);
		m_outStationBox.EnableWindow(FALSE);
		CWnd* pWnd = GetDlgItem(IDC_BUTTON1);
		if(pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDOK);
		if(pWnd)
			pWnd->EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModifyStationIndex::ShowTrain()
{
	m_list.DeleteAllItems();
	
	for(int i=0; i<mTrain.GetRecordNum(); i++)
	{		
		TgTRAIN_RECORD* rec = mTrain.GetRecordByRecIndex(i);
		if(NULL == rec)
			continue;

		CString text;
		int row = m_list.GetItemCount();
		// 序号
		text.Format("%d",i);
		row = m_list.InsertItem(row, text);

		// 车站
		text = m_pDoc->GetStationName(rec->station);
		m_list.SetItemText(row, 1, text);
		m_list.SetItemData(row, rec->station);

		// 入口站序
		text.Format("%d",rec->in_station_index);
		m_list.SetItemText(row, 2, text);

		// 出口站序
		text.Format("%d",rec->out_station_index);
		m_list.SetItemText(row, 3, text);
	}
}

void CModifyStationIndex::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	for(int row=0; row<m_list.GetItemCount(); row++)
	{
		if(m_list.GetItemState(row,LVIS_SELECTED) == LVIS_SELECTED)
		{
			m_nRecIndex=row;

			m_inStationBox.ResetContent();
			m_outStationBox.ResetContent();

			DWORD_PTR nStationNo = m_list.GetItemData(row);

			int nInSel=-1, nOutSel=-1;
			for(int idx=0; idx<m_pDoc->m_nDispDistanceNum; idx++)
			{
				if(nStationNo == m_pDoc->m_sDispDistance[idx].station_number)
				{
					int nIndex;
					CString tmp;
					tmp.Format("%d", idx);
					
					nIndex = m_inStationBox.AddString(tmp);
					m_inStationBox.SetItemData(nIndex, idx);
					CString text = m_list.GetItemText(row, 2);
					if(text == tmp)
					{
						nInSel=nIndex;
					}

					nIndex = m_outStationBox.AddString(tmp);
					m_outStationBox.SetItemData(nIndex, idx);
					text = m_list.GetItemText(row, 3);
					if(text == tmp)
					{
						nOutSel=nIndex;
					}
				}
			}

			if(nInSel != -1)
			{
				m_inStationBox.SetCurSel(nInSel);
			}
			if(nOutSel != -1)
			{
				m_outStationBox.SetCurSel(nOutSel);
			}
		}
	}

	*pResult = 0;
}

void CModifyStationIndex::OnBnClickedModify()
{
	if(m_nRecIndex < 0)
	{
		AfxMessageBox("请选择车站");
		return;
	}
	
	int nInSel  = m_inStationBox.GetCurSel();
	int nOutSel = m_outStationBox.GetCurSel();
	if(nInSel == LB_ERR)
	{
		AfxMessageBox("请选择入口站序");
		return;
	}
	if(nOutSel == LB_ERR)
	{
		AfxMessageBox("请选择出口站序");
		return;
	}
	if(m_nRecIndex >= 0 && m_nRecIndex < mTrain.myRecord.size())
	{
		mTrain.myRecord[m_nRecIndex].in_station_index  = m_inStationBox.GetItemData(nInSel);
		mTrain.myRecord[m_nRecIndex].out_station_index = m_outStationBox.GetItemData(nOutSel);
		ShowTrain();
	}
}

void CModifyStationIndex::OnBnClickedOk()
{
	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_train_index);
	if(NULL == pTrain)
	{
		MessageBox("请注意: 该车可能已经删除, 修改站序失败", "错误");
		return;
	}

	CButton* pWnd = (CButton*)GetDlgItem(IDC_CHECK_DELETE);
	if(pWnd)
	{
		if(BST_CHECKED == pWnd->GetCheck())
		{
			CString text;
			text.Format("你确实要删除列车 %s 吗",m_pDoc->GetTrainShowId(mTrain));
			if(IDYES != MessageBox(text, "提示", MB_YESNO|MB_ICONQUESTION))
				return;
			
			m_pDoc->sysprint(4,0,"[TG] CModifyStationIndex: 删除列车 %s",m_pDoc->GetTrainShowId(mTrain));
			m_pDoc->SendDeleteTrainXml(m_train_index);
			this->DestroyWindow();
			return;
		}
	}
	m_pDoc->sysprint(4,0,"[TG] CModifyStationIndex:修改站序 %s",m_pDoc->GetTrainShowId(mTrain));
	m_pDoc->SendUpdateTrain(mTrain);
	this->DestroyWindow();
	//OnOK();
}

void CModifyStationIndex::PostNcDestroy()
{
	delete this;
	CDialog::PostNcDestroy();
}

void CModifyStationIndex::OnBnClickedCancel()
{
	this->DestroyWindow();
	//OnCancel();
}


