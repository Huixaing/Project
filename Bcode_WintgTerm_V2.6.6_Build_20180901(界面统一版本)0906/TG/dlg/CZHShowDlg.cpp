// CZHManage.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"

#include "CZHShowDlg.h"
#include ".\czhshowdlg.h"

#define MSG_UPDATE_CZH 10012

#define   IDC_CZH_GRID_ID  55322
// CCZHShowDlg 对话框
extern void GetCZHGridHeader(CString& xml);

IMPLEMENT_DYNAMIC(CCZHShowDlg, CDialog)
CCZHShowDlg::CCZHShowDlg(CString title, CTG_TRAIN* train, CTGDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CCZHShowDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	m_train=train;
	m_title=title;
}

CCZHShowDlg::~CCZHShowDlg()
{
}

void CCZHShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CZH, m_group);
}


BEGIN_MESSAGE_MAP(CCZHShowDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_CZH, OnSelCZHChange)
	ON_WM_CLOSE()
	ON_MESSAGE(MSG_UPDATE_CZH, OnUpdateSelCZHTrain)
END_MESSAGE_MAP()


// CCZHShowDlg 消息处理程序
int   CCZHShowDlg::OnCreate(LPCREATESTRUCT lpCreat)
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

void  CCZHShowDlg::OnSize(UINT nType,int cx,int cy)
{
	if(m_grid.GetSafeHwnd())
		m_grid.MoveWindow(1,52,cx-1,cy-53);

	CDialog::OnSize(nType,cx,cy);
}

BOOL  CCZHShowDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	CFont font;
	font.CreateFont(18, 0,0,0, FW_BOLD, 0,0,0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Arial");
	m_grid.SetFont(&font);
	font.DeleteObject();

	CStringArray array;
	m_pDoc->myChezuhao.GetTrainCZHText(array);

	int nSel=-1;
	for(int i=0; i<array.GetCount(); i++)
	{
		int index=m_group.AddString(array.GetAt(i));
		
		if(m_train->myTrainInfo.shift_index_string==array.GetAt(i))
			nSel=index;
	}

	if(nSel>=0)
	{
		m_group.SetCurSel(nSel);
	}

	OnSelCZHChange();

	this->SetWindowText(m_title);

	m_grid.AutoWidth();
	m_grid.Refresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CCZHShowDlg::OnUpdateSelCZHTrain(WPARAM wParam, LPARAM lParam)
{
	// 判断车组列表是否需要刷新
	CStringArray array;
	m_pDoc->myChezuhao.GetTrainCZHText(array);
	bool bCzhChged=false;
	if(array.GetCount()!=m_group.GetCount())
	{
		bCzhChged=true;
	}
	else
	{
		for(int i=0; i<array.GetCount(); i++)
		{
			CString czh=array.GetAt(i);

			int n=0;
			for(n=0; n<m_group.GetCount();n++)
			{
				CString text;
				m_group.GetLBText(i,text);
				if(text==czh)
				{
					break;
				}
			}
			if(n>=m_group.GetCount())
			{
				bCzhChged=true;
				break;
			}
		}
	}

	if(bCzhChged)
	{
		CString czh;
		int nSel=m_group.GetCurSel();
		if(nSel>=0)
		{
			m_group.GetLBText(nSel, czh);
		}

		m_group.ResetContent();

		nSel=-1;
		for(int i=0; i<array.GetCount(); i++)
		{
			int index=m_group.AddString(array.GetAt(i));
			
			if(czh==array.GetAt(i))
				nSel=index;
		}

		if(nSel>=0)
		{
			m_group.SetCurSel(nSel);
		}
	}

	OnSelCZHChange();
	return 0;
}

void CCZHShowDlg::OnSelCZHChange()
{
	int nSel=m_group.GetCurSel();
	if(nSel>=0)
	{
		CString czh;
		m_group.GetLBText(nSel, czh);

		TRAIN_CHEZUHAO* pTrainsCZH=m_pDoc->myChezuhao.GetCzhByCheZuHao(czh);
		ShowCZHTrains(pTrainsCZH);
	}
}

void CCZHShowDlg::ShowCZHTrains(TRAIN_CHEZUHAO* pTrainsCZH)
{
	m_grid.DeleteNonFixedRows();
	m_grid.Refresh();

	if(pTrainsCZH==NULL)
		return;

	CString xml, tmp;
	xml="<CZH>";
	std::map<ULONG, TRAIN_CZH_INFO>::iterator it = pTrainsCZH->trains.begin();
	for(;it!=pTrainsCZH->trains.end(); it++)
	{
		xml+="<RECORD>";
		
		tmp.Format("<TRAIN_INDEX>%d</TRAIN_INDEX>", it->second.train_index);
		xml += tmp;

		tmp.Format("<TRAIN_ID>%s</TRAIN_ID>", it->second.trainid);
		xml += tmp;

		tmp.Format("<CON_ST_STATION>%s</CON_ST_STATION>", m_pDoc->GetStationName(it->second.begin_station));
		xml += tmp;

		tmp.Format("<CON_ED_STATION>%s</CON_ED_STATION>", m_pDoc->GetStationName(it->second.end_station));
		xml += tmp;

		if(m_pDoc->GetTgType() == TG_TYPE::BASIC_TG)
		{
			CString day, time;
			if(it->second.stTime>0)
			{
				day.Format("%d", it->second.stTime/86400);
				time = ConvertTimetoString(it->second.stTime%86400);
				tmp.Format("<CON_ST_TIME>%s-%s</CON_ST_TIME>", day, time);
				xml += tmp;
			}
			if(it->second.edTime>0)
			{
				day.Format("%d", it->second.edTime/86400);
				time = ConvertTimetoString(it->second.edTime%86400);
				tmp.Format("<CON_ED_TIME>%s-%s</CON_ED_TIME>", day, time);
				xml += tmp;
			}
		}
		else
		{
			if(it->second.stTime>0)
			{
				CTime tm(it->second.stTime);
				tmp.Format("<CON_ST_TIME>%s</CON_ST_TIME>", tm.Format("%Y-%m-%d %H:%M"));
				xml += tmp;
			}

			if(it->second.edTime>0)
			{
				CTime tm(it->second.edTime);
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


void CCZHShowDlg::OnClose()
{
	DestroyWindow();
}

void CCZHShowDlg::PostNcDestroy() 
{
	delete this;
}
