// DlgFindTrainResult.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "DlgFindTrainResult.h"
#include ".\dlgfindtrainresult.h"


// CDlgFindTrainResult 对话框

IMPLEMENT_DYNAMIC(CDlgFindTrainResult, CDialog)
CDlgFindTrainResult::CDlgFindTrainResult(int tg_type, int TrainCnt, CString TrainId, long PostionList[], long PTrainIdList[],long PTimeList[], CWnd* pParent)
	: CDialog(CDlgFindTrainResult::IDD, pParent)
{
	m_tg_type = tg_type;
	m_nTrainCnt = TrainCnt;
	m_strTrainId = TrainId;

	for (int i=0; i<TrainCnt;i++)
	{
		m_nlistPosition[i] = PostionList[i];
		m_nlistTrainId[i] = PTrainIdList[i];
		m_nlistDepartTime[i] = PTimeList[i];
	}

	pMainFrme = (CTGFrame*)pParent;
}

CDlgFindTrainResult::~CDlgFindTrainResult()
{
}

void CDlgFindTrainResult::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_RESULT, m_listResult);
}


BEGIN_MESSAGE_MAP(CDlgFindTrainResult, CDialog)
	ON_BN_CLICKED(IDC_BNT_CLOSE, OnBnClickedBntClose)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESULT, OnLvnItemchangedListResult)
END_MESSAGE_MAP()


// CDlgFindTrainResult 消息处理程序

void CDlgFindTrainResult::OnBnClickedBntClose()
{
	CDialog::OnOK();
}

void CDlgFindTrainResult::OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (pNMLV->uNewState==(LVIS_SELECTED|LVIS_FOCUSED))
	{
		int index = pNMLV->iItem;
		pMainFrme->SendMessage(MSG_SET_TRAIN_FOCUS, m_nlistPosition[index], m_nlistTrainId[index]);
	}

	*pResult = 0;
}

BOOL CDlgFindTrainResult::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwStyle = m_listResult.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_listResult.SetExtendedStyle(dwStyle);
	m_listResult.InsertColumn(0, "序号",LVCFMT_CENTER,50);
	m_listResult.InsertColumn(1, "车次号",LVCFMT_CENTER,100);
	m_listResult.InsertColumn(2, "始发时间",LVCFMT_CENTER,150);

	m_listResult.DeleteAllItems();
	int row;
	CString str;
	CTime tm;
	for (int i =0;i<m_nTrainCnt;i++)
	{
		int count = m_listResult.GetItemCount();
		str.Format("%d",i);
		row = m_listResult.InsertItem(m_listResult.GetItemCount(), str);
		m_listResult.SetItemText(row,1,m_strTrainId);
		
		if(m_tg_type == TG_TYPE::BASIC_TG)
		{
			long tm1=m_nlistDepartTime[i];
			str.Format("%2d时%2d分%2d秒", tm1/3600, (tm1/60)%60, tm1%60);
		}
		else
		{
			tm = CTime(m_nlistDepartTime[i]);
			str.Format("%4d年%2d月%2d日 %2d时%2d分", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(),tm.GetMinute());
		}
		
		m_listResult.SetItemText(row,2,str);
	}

	m_listResult.Invalidate(FALSE);

	return TRUE; 
}

