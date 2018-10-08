// CZHManage.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"

#include "JiaoLuShowDlg.h"
#include ".\jiaolushowdlg.h"


#define MSG_UPDATE_RELATION 10012

#define   IDC_CZH_GRID_ID  55322
// CCZHShowDlg 对话框
extern void GetCZHGridHeader(CString& xml);

IMPLEMENT_DYNAMIC(CJiaoLuShowDlg, CDialog)
CJiaoLuShowDlg::CJiaoLuShowDlg(CString title, TRAIN_INDEX train_index, CTGDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CJiaoLuShowDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	sel_train_index=train_index;
	m_title=title;
}

CJiaoLuShowDlg::~CJiaoLuShowDlg()
{
}

void CJiaoLuShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CZH, m_group);
}


BEGIN_MESSAGE_MAP(CJiaoLuShowDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_CZH, OnShowSelTrainRelation)
	ON_WM_CLOSE()
	ON_MESSAGE(MSG_UPDATE_RELATION, OnUpdateTrainRelation)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedRefresh)
END_MESSAGE_MAP()


// CCZHShowDlg 消息处理程序
int   CJiaoLuShowDlg::OnCreate(LPCREATESTRUCT lpCreat)
{
	if(CDialog::OnCreate(lpCreat))
		return -1;
	CRect rect(0,0,0,0);

	CString xml;
	GetCZHGridHeader(xml);
	m_grid.CreateGrid(xml, this,rect,IDC_CZH_GRID_ID);
	m_grid.SetEditable(FALSE);

	return 0;
}

void  CJiaoLuShowDlg::OnSize(UINT nType,int cx,int cy)
{
	if(m_grid.GetSafeHwnd())
		m_grid.MoveWindow(1,52,cx-1,cy-53);

	CDialog::OnSize(nType,cx,cy);
}

void CJiaoLuShowDlg::OnBnClickedRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	m_group.ResetContent();
	int nSel=-1;
	mListRL.clear();
	m_pDoc->myPDataManage->GetTrainRelation(mListRL, true);
	
	for(int i=0; i<mListRL.size(); i++)
	{
		int index=m_group.AddString(mListRL[i].strName);
		if(mListRL[i].IsInThisGroup(sel_train_index))
		{
			nSel=index;
		}
	}

	if(nSel>=0)
	{
		m_group.SetCurSel(nSel);
	}
	ShowCZHTrains();
}

BOOL  CJiaoLuShowDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	CFont font;
	font.CreateFont(18, 0,0,0, FW_BOLD, 0,0,0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Arial");
	m_grid.SetFont(&font);
	font.DeleteObject();

	OnBnClickedRefresh();

	OnShowSelTrainRelation();

	SetWindowText(m_title);
	m_grid.AutoWidth();
	m_grid.Refresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CJiaoLuShowDlg::OnUpdateTrainRelation(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedRefresh();
	return 0;
}

void CJiaoLuShowDlg::OnShowSelTrainRelation()
{
	ShowCZHTrains();
}

void CJiaoLuShowDlg::ShowCZHTrains()
{
	m_grid.DeleteNonFixedRows();
	m_grid.Refresh();

	int nSel=m_group.GetCurSel();
	if(nSel<0 || nSel>=mListRL.size())
		return;

	TrainGroupInfo info=mListRL[nSel];
	CString xml, tmp;
	xml="<CZH>";
	for(int i=0; i<info.train_count; i++)
	{
		xml+="<RECORD>";
		if(i==0)
			sel_train_index=info.trains[i].train_index;
		tmp.Format("<TRAIN_INDEX>%d</TRAIN_INDEX>", info.trains[i].train_index);
		xml += tmp;

		CTG_TRAIN* ptrain=m_pDoc->GetTrainByIndex(info.trains[i].train_index);
		CString strTrain;
		if(ptrain)
			strTrain=m_pDoc->GetTrainShowId(ptrain);
		tmp.Format("<TRAIN_ID>%s</TRAIN_ID>", strTrain);
		xml += tmp;
		
		CWnd* pWnd=GetDlgItem(IDC_EDIT12);
		if(pWnd)
			pWnd->SetWindowText(strTrain);

		pWnd=GetDlgItem(IDC_EDIT11);
		if(pWnd)
		{
			CString trainindexstr;
			trainindexstr.Format("%d",info.trains[i].train_index);
			pWnd->SetWindowText(trainindexstr);
		}

		tmp.Format("<CON_ST_STATION>%s(%s)</CON_ST_STATION>", 
			m_pDoc->GetStationName(info.trains[i].begin_station),
			m_pDoc->GetSideName(info.trains[i].begin_station, info.trains[i].begin_side));
		xml += tmp;

		tmp.Format("<CON_ED_STATION>%s(%s)</CON_ED_STATION>", 
			m_pDoc->GetStationName(info.trains[i].end_station),
			m_pDoc->GetSideName(info.trains[i].end_station, info.trains[i].end_side));
		xml += tmp;

		if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
		{
			CString day, time;
			if(info.trains[i].stTime>0)
			{
				day.Format("%d", info.trains[i].stTime/86400);
				time = ConvertTimetoString(info.trains[i].stTime%86400);
				tmp.Format("<CON_ST_TIME>%s-%s</CON_ST_TIME>", day, time);
				xml += tmp;
			}
			if(info.trains[i].edTime>0)
			{
				day.Format("%d", info.trains[i].edTime/86400);
				time = ConvertTimetoString(info.trains[i].edTime%86400);
				tmp.Format("<CON_ED_TIME>%s-%s</CON_ED_TIME>", day, time);
				xml += tmp;
			}
		}
		else
		{
			if(info.trains[i].stTime>0)
			{
				CTime tm(info.trains[i].stTime);
				tmp.Format("<CON_ST_TIME>%s</CON_ST_TIME>", tm.Format("%Y-%m-%d %H:%M"));
				xml += tmp;
			}

			if(info.trains[i].edTime>0)
			{
				CTime tm(info.trains[i].edTime);
				tmp.Format("<CON_ED_TIME>%s</CON_ED_TIME>", tm.Format("%Y-%m-%d %H:%M"));
				xml += tmp;
			}
		}

		xml+="</RECORD>";
	}

	xml+="</CZH>";

	m_grid.AddRowDataFromXML(xml, "CZH/RECORD");
	m_grid.Refresh();
}


void CJiaoLuShowDlg::OnClose()
{
	DestroyWindow();
}

void CJiaoLuShowDlg::PostNcDestroy() 
{
	delete this;
}


