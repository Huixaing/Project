// UserPropertyDlg.cpp : implementation file
//
#include "stdafx.h"
#include "DlgQuickModifyMultiTrains.h"
#include ".\dlgquickmodifymultitrains.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgQuickModifyMultiTrains dialog

#define MODIFY_LIST_COLUMN_COUNT  4
#define MODIFY_TIME_PERCENT_COUNT  9

_TCHAR *_gMultiModifyTrain[MODIFY_LIST_COLUMN_COUNT] =
{
	_T("序号"),_T("车次号"),_T("选中"),_T("列车索引号")
};

int _gTimePercent[MODIFY_TIME_PERCENT_COUNT] = {10, 20, 30, 40, 50, 100, 150, 200, 300};

CDlgQuickModifyMultiTrains::CDlgQuickModifyMultiTrains(CTGDoc *pdoc, CWnd* pParent /*=NULL*/)
: CDialog(CDlgQuickModifyMultiTrains::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgQuickModifyMultiTrains)
	m_pDoc = pdoc;
	m_pGridCtrl = NULL;
	m_sel_all=false;
	m_begin_tm=CTime(0);
	m_end_tm=CTime(0);
	start_sta_idx=0;
	end_sta_idx=0;
	//}}AFX_DATA_INIT
}


void CDlgQuickModifyMultiTrains::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQuickModifyMultiTrains)
	DDX_Control(pDX, IDC_START_STATION, m_start_station_list);
	DDX_Control(pDX, IDC_END_STATION, m_end_station_list);

	DDX_Control(pDX, IDC_BUFFER_PERCENT, m_buffer_list);
	DDX_Control(pDX, IDC_EDIT_MOVE_MINUTE, m_move_min);

	DDX_Control(pDX, IDC_RADIO_MOVE, m_choose_move);
	DDX_Control(pDX, IDC_RADIO_SLOW, m_choose_slow);

	DDX_Control(pDX, IDC_START_TIME, m_start_time);
	DDX_Control(pDX, IDC_END_TIME, m_end_time);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQuickModifyMultiTrains, CDialog)
	//{{AFX_MSG_MAP(CDlgQuickModifyMultiTrains)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDC_FIND_TRAINS, OnBnClickedFindTrains)
	ON_BN_CLICKED(IDC_MODIFY_TRAINS, OnBnClickedModifyTrains)
	ON_BN_CLICKED(IDC_MODIFY_CHK_ALL, OnBnClickedModifyChkAll)
	ON_CBN_SELCHANGE(IDC_START_STATION, OnCbnSelchangeStartStation)
	ON_BN_CLICKED(ID_MODIFY_TRAINS_CLOSE, OnBnClickedModifyTrainsClose)
	
	ON_BN_CLICKED(IDC_RADIO_MOVE, OnBnChooseMove)
	ON_BN_CLICKED(IDC_RADIO_SLOW, OnBnChooseSlow)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQuickModifyMultiTrains message handlers

BOOL CDlgQuickModifyMultiTrains::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int i=0;
	for(;i < m_pDoc->m_nDispDistanceNum;i++)
	{
		CString sta_name;
		sta_name.Format("%s(%d)",m_pDoc->m_sDispDistance[i].station_name, i);
		int idx = m_start_station_list.InsertString(i, sta_name);
		m_start_station_list.SetItemData(idx ,i);
	}

	m_buffer_list.EnableWindow(FALSE);
	m_move_min.EnableWindow(FALSE);

	m_choose_move.EnableWindow(FALSE);
	m_choose_slow.EnableWindow(FALSE);

	(CButton*)GetDlgItem(IDC_MODIFY_TRAINS)->EnableWindow(FALSE);

	m_start_time.SetFormat("HH:mm:ss");
	m_end_time.SetFormat("HH:mm:ss");

	CTime cur_tm = CTime::GetCurrentTime();
	m_start_time.SetTime(&cur_tm);
	CTime end_tm = cur_tm + 1*60*60;
	m_end_time.SetTime(&end_tm);

	initialGridCtrl();

	int idx=0;
	for (int m=0; m<MODIFY_TIME_PERCENT_COUNT; m++)
	{
		CString text;
		text.Format("%d%%",_gTimePercent[m]);
		idx = m_buffer_list.InsertString(-1,text);
		m_buffer_list.SetItemData(idx,_gTimePercent[m]/10);
	}

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgQuickModifyMultiTrains::OnBnClickedClose()
{
	delete m_pGridCtrl;
	CDialog::OnCancel();
}

void CDlgQuickModifyMultiTrains::OnBnClickedFindTrains()
{
	m_sel_all=false;
	CButton* pButton = (CButton*)GetDlgItem(IDC_MODIFY_CHK_ALL);
	pButton->SetWindowText("全选");

	CTime cur_tm;
	m_start_time.GetTime(m_begin_tm);
	m_end_time.GetTime(m_end_tm);
	
	if (m_begin_tm>m_end_tm)
		m_end_tm += CTimeSpan(1,0,0,0);
	
	ULONG train_index_list[2048], index_cnt=0;
	memset(train_index_list, 0, sizeof(train_index_list));

	index_cnt = m_pDoc->FindTrainByTime(m_begin_tm.GetTime(), m_end_tm.GetTime(), train_index_list);

	int count=1;
	long train_index=0;
	CTG_TRAIN* train = NULL;
	ASSERT_VALID(m_pGridCtrl);
	m_pGridCtrl->DeleteNonFixedRows();

	int start_index=0, end_index=0;
	
	start_index = m_start_station_list.GetCurSel();
	end_index = m_end_station_list.GetCurSel();
	if(start_index==-1 || end_index==-1)
	{
		AfxMessageBox("请选择车站");
		return;
	}
	start_sta_idx = m_start_station_list.GetItemData(start_index);
	end_sta_idx = m_end_station_list.GetItemData(end_index);

	m_pDoc->sysprint(0,0,"[批量调整] 查找列车 起始车站 %s(%d) 结束车站 %s(%d) 开始时间 %d 结束时间 %d", 
		m_pDoc->m_sDispDistance[start_sta_idx].station_name, start_index,
		m_pDoc->m_sDispDistance[end_sta_idx].station_name, end_index,
		m_begin_tm.GetTime(), m_end_tm.GetTime());
	
	for (int i=index_cnt-1; i>=0; i--)
	{
		train_index = train_index_list[i];
		train = m_pDoc->myPDataManage->GetTrain(train_index);

		if(train == NULL)
			continue;

		// 按车站筛选列车
		if (!m_pDoc->IfTrainInStationScope(train, start_sta_idx, end_sta_idx, m_begin_tm.GetTime(), m_end_tm.GetTime()))
		{
			for(int y=i;y<index_cnt-1;y++)
			{	
				train_index_list[y]=train_index_list[y+1];
			}
			train_index_list[index_cnt]=0;
			index_cnt--;
			continue;
		}
	}

	m_pGridCtrl->SetRedraw(FALSE);
	m_pGridCtrl->SetRowCount(index_cnt + 1);

	for(int i=0; i< index_cnt; i++)
	{
		train_index = train_index_list[i];
		train = m_pDoc->myPDataManage->GetTrain(train_index);

		if(train == NULL)
			continue;

		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = count;  //行
		Item.col = 0;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%d",count);
		m_pGridCtrl->SetItem(&Item);            


		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = count;  //行
		Item.col = 1;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%s",  m_pDoc->GetTrainShowId(train));
		m_pGridCtrl->SetCellType(count,1,RUNTIME_CLASS(CUnEditAbleGridCell));
		m_pGridCtrl->SetItem(&Item);            

		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = count;  //行
		Item.col = 2;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("选中");
		m_pGridCtrl->SetCellType(count,2,RUNTIME_CLASS(CGridCellCheck));
		m_pGridCtrl->SetItem(&Item);            

		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = count;  //行
		Item.col = 3;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%d",train_index);
		m_pGridCtrl->SetItem(&Item);            

		m_pGridCtrl->SetItemData(count,0,(DWORD)train_index);
		count ++;
	}

	m_pGridCtrl->SetRedraw(TRUE);

	if (index_cnt>0)
	{
		m_choose_move.EnableWindow(TRUE);
		m_choose_slow.EnableWindow(TRUE);

		(CButton*)GetDlgItem(IDC_MODIFY_TRAINS)->EnableWindow(TRUE);
	}
}

void CDlgQuickModifyMultiTrains::OnBnClickedModifyTrains()
{
	int begin_tm=m_begin_tm.GetTime();
	int end_tm=m_end_tm.GetTime();
	if (m_choose_slow.GetCheck())
	{
		int selIndex=-1;
		selIndex=m_buffer_list.GetCurSel();
		int buff_percent = m_buffer_list.GetItemData(selIndex);

		if (-1==selIndex)
		{
			AfxMessageBox("请选择缓行参数!");
			return;
		}

		m_pDoc->sysprint(0,0,"[批量调整] 调整列车 起始车站 %s(%d) 结束车站 %s(%d) 开始时间 %d 结束时间 %d 缓行参数 %d", 
			m_pDoc->m_sDispDistance[start_sta_idx].station_name, start_sta_idx,
			m_pDoc->m_sDispDistance[end_sta_idx].station_name, end_sta_idx,
			begin_tm, end_tm, buff_percent);

		CString text,tmp;
		BOOL m_bChecked=FALSE;
		CGridCellCheck *pCell;
		for(int i=1;i<= (m_pGridCtrl->GetRowCount()-1);i++)
		{
			pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,2);
			if(pCell)
			{
				m_bChecked = pCell->GetCheck();
			}
			else
			{
				m_bChecked=FALSE;
			}

			if(m_bChecked)
			{
				DWORD key = (DWORD)m_pGridCtrl->GetItemData(i,0);
				tmp.Format("%s|",m_pGridCtrl->GetItemText(i,1));
				text+=tmp;

				mSelTrainKey.push_back(key);
			}
		}

		if(mSelTrainKey.size() ==0)
		{
			AfxMessageBox("请选中列车!");
			return;
		}
		else
		{
			CString afxmsg;
			afxmsg = "确认修改这些列车("+text+")的区间运缓时间?";
			if(AfxMessageBox(afxmsg, MB_YESNO) == IDYES)
			{
				m_pDoc->sysprint(0,0,"[批量调整] 调整列车列表 %s",text);

				CString xml="<root>", str;
				str.Format("<percent begin_sta_index=\"%d\" end_sta_idx=\"%d\" begin_tm=\"%d\" end_tm=\"%d\">",
					start_sta_idx, end_sta_idx, begin_tm, end_tm);
				xml+=str;
				str.Format("%d</percent>", buff_percent);
				xml+=str;

				for (int i=0; i<mSelTrainKey.size(); i++)
				{
					str.Format("<TRAIN_INDEX>%u</TRAIN_INDEX>", mSelTrainKey[i]);
					xml+=str;
				}
				xml+="</root>";
				m_pDoc->m_ptgctrl->FIRE_MULTI_TRAIN_OPER(xml, 0x00000004);


				CDialog::OnOK();
			}
		}
	}
	else if (m_choose_move.GetCheck())
	{
		CString text;
		m_move_min.GetWindowText(text);

		if (text.IsEmpty())
		{
			AfxMessageBox("请输入平移分钟数!");
			return;
		}

		int moveMin=0;
		moveMin=atoi(text);

		m_pDoc->sysprint(0,0,"[批量调整] 调整列车 起始车站 %s(%d) 结束车站 %s(%d) 开始时间 %d 结束时间 %d 平移分钟 %d", 
			m_pDoc->m_sDispDistance[start_sta_idx].station_name, start_sta_idx,
			m_pDoc->m_sDispDistance[end_sta_idx].station_name, end_sta_idx,
			begin_tm, end_tm, moveMin);

		CString tmp;
		BOOL m_bChecked=FALSE;
		CGridCellCheck *pCell;
		for(int i=1;i<= (m_pGridCtrl->GetRowCount()-1);i++)
		{
			pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,2);
			if(pCell)
			{
				m_bChecked = pCell->GetCheck();
			}
			else
			{
				m_bChecked=FALSE;
			}

			if(m_bChecked)
			{
				DWORD key = (DWORD)m_pGridCtrl->GetItemData(i,0);
				tmp.Format("%s|",m_pGridCtrl->GetItemText(i,1));
				text+=tmp;

				mSelTrainKey.push_back(key);
			}
		}

		if(mSelTrainKey.size() ==0)
		{
			AfxMessageBox("请选中列车!");
			return;
		}
		else
		{
			CString afxmsg;
			afxmsg = "确认平移这些列车("+text+")的时间?";
			if(AfxMessageBox(afxmsg, MB_YESNO) == IDYES)
			{
				m_pDoc->sysprint(0,0,"[批量调整] 调整列车列表 %s",text);
				CString xml="<root>", str;
				str.Format("<movetm begin_sta_index=\"%d\" end_sta_idx=\"%d\" begin_tm=\"%u\" end_tm=\"%u\">",
					start_sta_idx, end_sta_idx, begin_tm, end_tm);
				xml+=str;

				str.Format("%d</movetm>", moveMin);
				xml+=str;
				for (int i=0; i<mSelTrainKey.size(); i++)
				{
					str.Format("<TRAIN_INDEX>%u</TRAIN_INDEX>", mSelTrainKey[i]);
					xml+=str;
				}
				xml+="</root>";
				m_pDoc->m_ptgctrl->FIRE_MULTI_TRAIN_OPER(xml, 0x00000003);
				CDialog::OnOK();
			}
		}
	}
	else
	{
		AfxMessageBox("请选择操作方式!");
		return;
	}
}

void CDlgQuickModifyMultiTrains::OnCbnSelchangeStartStation()
{
	m_end_station_list.ResetContent();

	int idx = m_start_station_list.GetCurSel();
	int sta_idx = m_start_station_list.GetItemData(idx);
	
	int i=0;
	for(;i < m_pDoc->m_nDispDistanceNum;i++)
	{
		if (i== sta_idx)
			continue;

		if (m_pDoc->m_sDispDistance[i].sub_section == m_pDoc->m_sDispDistance[sta_idx].sub_section)
		{
			CString sta_name;
			sta_name.Format("%s(%d)",m_pDoc->m_sDispDistance[i].station_name, i);
			int end_idx = m_end_station_list.AddString(sta_name);
			m_end_station_list.SetItemData(end_idx, i);
		}
	}

	UpdateData(false);

}

void CDlgQuickModifyMultiTrains::initialGridCtrl()
{
	if(m_pGridCtrl == NULL)
		m_pGridCtrl= new CGridCtrl;

	int col;

	CRect rect2;
	CRect rect;
	CWnd *pWnd;
	pWnd= GetDlgItem(IDC_LIST);
	pWnd->GetClientRect(rect);
	pWnd->MapWindowPoints(this,rect);

	m_pGridCtrl->Create(rect,this,IDC_LIST);
	try
	{
		m_pGridCtrl->SetFixedRowCount(1);
		m_pGridCtrl->SetFixedColumnCount(1);
		m_pGridCtrl->SetColumnCount(MODIFY_LIST_COLUMN_COUNT);           
		m_pGridCtrl->SetRowCount(1);
		m_pGridCtrl->SetFixedTextColor(CLR_BLUE);      //文本颜色
		m_pGridCtrl->SetGridLineColor(RGB(166,202,240));       //线的颜色
		m_pGridCtrl->SetFixedBkColor(GetSysColor(COLOR_3DFACE));
		m_pGridCtrl->SetFixedColumnSelection(TRUE);
		m_pGridCtrl->SetFixedRowSelection(TRUE);
	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return ;
	}

	for (col = 0; col < MODIFY_LIST_COLUMN_COUNT; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		if(col == 0)
		{
			Item.nState = GVIS_READONLY|GVIS_FIXEDROW|GVIS_FIXEDCOL;
		}
		else
		{
			Item.nState = GVIS_READONLY|GVIS_FIXEDROW;
		}
		Item.row = 0;
		Item.col = col;
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format(_gMultiModifyTrain[col]);
		m_pGridCtrl->SetItem(&Item);
	}
}

void CDlgQuickModifyMultiTrains::OnBnClickedModifyChkAll()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_MODIFY_CHK_ALL);

	if (!m_sel_all)
	{
		m_sel_all=true;
		pButton->SetWindowText("全不选");
	}
	else
	{
		m_sel_all=false;
		pButton->SetWindowText("全选");
	}

	CGridCellCheck *pCell;
	for(int i= 1; i<= (m_pGridCtrl->GetRowCount()-1);i++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,2);
		if(pCell)
		{
			if (!m_sel_all)
				pCell->SetCheck(FALSE);
			else
				pCell->SetCheck(TRUE);
		}
	}
}

void CDlgQuickModifyMultiTrains::OnBnClickedModifyTrainsClose()
{
	CDialog::OnCancel();
}

void CDlgQuickModifyMultiTrains::OnBnChooseMove()
{
	m_move_min.EnableWindow(TRUE);
	m_buffer_list.EnableWindow(FALSE);
}

void CDlgQuickModifyMultiTrains::OnBnChooseSlow()
{
	m_move_min.EnableWindow(FALSE);
	m_buffer_list.EnableWindow(TRUE);
}