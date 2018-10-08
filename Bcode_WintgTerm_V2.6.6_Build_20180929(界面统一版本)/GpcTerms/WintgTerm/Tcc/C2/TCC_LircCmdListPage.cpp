/*
 * 列控 当前限速命令列表
 */

#include "stdafx.h"
#include "resource.h"
#include <sysshare.h>
#include "Tcc_Dlg.h"
#include <SuperPassDlg.h>
#include <DlgMessageBox.h>

namespace TCC {


// CTCCmdStatusPage 对话框

IMPLEMENT_DYNAMIC(CLircCommandPage, CPropertyPage)
CLircCommandPage::CLircCommandPage()
	: CPropertyPage(CLircCommandPage::IDD)
{
}

CLircCommandPage::~CLircCommandPage()
{
}

CTccDlg* CLircCommandPage::GetTccDlg()
{
	CWnd *pWnd = GetParent();
	ASSERT(pWnd);
	pWnd = pWnd->GetParent();
	ASSERT(pWnd);

	return (CTccDlg*)pWnd;
}

ITccAdviseSink* CLircCommandPage::GetTccAdviseSink()
{
	return GetTccDlg()->GetTccAdviseSink();
}

ITccClientInfoProvider* CLircCommandPage::GetTccClientInfoProvider()
{
	return GetTccDlg()->GetTccClientInfoProvider();
}

stano_t CLircCommandPage::GetCurrentTccID()
{
	return GetTccDlg()->GetCurrentTccID();
}

void CLircCommandPage::Refresh()
{
	stano_t nTccID = GetCurrentTccID();
	if(0 == nTccID)
	{
		MessageBox("请选择车站!");
		return ;
	}

	// 刷新线路列表
	m_StccLineStatusGrid.SelectStation(nTccID);

	// 清空实时命令列表
	OnRequestTccCmdList();
}

void CLircCommandPage::AppendCommand(TCC_COMMAND& cmd, DWORD nStatus)
{
	if(cmd.nTccID != GetCurrentTccID())
		return;

	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();
	CListCtrl *pListCtrl = &m_CmdList;

	CString str;

	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));

	for(int i=0; i<pListCtrl->GetItemCount(); i++)
	{
		TCC_COMMAND *pCmd = (TCC_COMMAND *)pListCtrl->GetItemData(i);
		if(pCmd->nConsoleID == cmd.nConsoleID
			&& pCmd->nCmdID == cmd.nCmdID
			&& pCmd->sys_id == cmd.sys_id)
		{
			*pCmd = cmd;
			break;
		}
	}
	if(i == pListCtrl->GetItemCount())
	{
		// 在屁股后增加
		TCC_COMMAND *pCmd = new TCC_COMMAND;
		ASSERT(pCmd);
		*pCmd = cmd;

		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		ASSERT(0 == COL_CMDID);

		lvi.iItem  = pListCtrl->GetItemCount();
		lvi.iSubItem = 0;
		str.Format("%4d", cmd.nCmdID);
		lvi.pszText = (LPSTR)(LPCSTR)str;
		lvi.lParam = (DWORD)pCmd;
		lvi.iItem = pListCtrl->InsertItem(&lvi);
	}
	else
	{
		lvi.iItem = i;
	}

	lvi.mask = LVIF_TEXT;

	// 开始站
	lvi.iSubItem = COL_BEGINSTA;
	if(cmd.nStaID[0])
		str = pInfoProvider->GetStationName(cmd.nStaID[0]);
	else
		str.Empty();
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// 结束站
	lvi.iSubItem = COL_ENDSTA;
	if(cmd.nStaID[1])
        str = pInfoProvider->GetStationName(cmd.nStaID[1]);
	else
		str.Empty();
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// 上下行
	lvi.iSubItem = COL_ABSLINEID;
	str = pInfoProvider->GetTccLineName(cmd.nTccID, cmd.nLineID);
	if(str.IsEmpty())
		str = gpcdef.GetC3TccLineName(cmd.nLineID);
		
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// 开始公里标
	lvi.iSubItem = COL_START_KM;
	if(cmd.bFullLimit)
		str.Format("全限速");
	else
		str = cmd.nKmMark[0].toString();
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// 结束公里标
	lvi.iSubItem = COL_END_KM;
	if(cmd.bFullLimit)
		str.Format("全限速");
	else
		str = cmd.nKmMark[1].toString();
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// 限速值
	lvi.iSubItem = COL_SPEEDVAL;
	if(cmd.bCancelLimit)
		str.Format("取消限速");
	else
		str.Format("%3dKm/h", cmd.nSpeedMaxVal);
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// 限速开始时间
	lvi.iSubItem = COL_START_TIME;
	CTime ct;
	if(cmd.bStartNow || cmd.nTime[0] <= 0)
		str.Format("立即开始");
	else
	{
		ct = cmd.nTime[0];
		str = ct.Format("%m/%d %H:%M");
	}
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// 限速结束时间
	lvi.iSubItem = COL_END_TIME;
	if(cmd.bEndless || cmd.nTime[1] <= 0)
		str.Format("永久有效");
	else
	{
		ct = cmd.nTime[1];
		str = ct.Format("%m/%d %H:%M");
	}
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// 状态
}

void CLircCommandPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TCC_CMD_LIST, m_CmdList);
}

void CLircCommandPage::InitListCtrl(CListCtrl *pListCtrl)
{
	struct COLINFO { TCHAR* label;	INT width;	INT fmt; }
	colinfo[] =
	{
		{"命令号",		60, LVCFMT_RIGHT },
		{"开始站",      80, LVCFMT_LEFT },
		{"结束站",      80, LVCFMT_LEFT },
		{"上下行",		100, LVCFMT_LEFT },
		{"开始公里标",	120, LVCFMT_LEFT },
		{"结束公里标",	120, LVCFMT_LEFT },
		{"限速值",		60, LVCFMT_LEFT },
		{"开始时间",	80, LVCFMT_LEFT },
		{"结束时间",	80, LVCFMT_LEFT },
	};

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(int j = 0; j < sizeof(colinfo)/sizeof(colinfo[0]); j++)
	{
		lvc.iSubItem = j;
		lvc.pszText = colinfo[j].label;
		lvc.cx = colinfo[j].width;
		lvc.fmt = colinfo[j].fmt;
		pListCtrl->InsertColumn(j, &lvc);
	}
}

BOOL CLircCommandPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();
	if(NULL == pInfoProvider)
	{
		MessageBox("没有安装TccClientInfoProvider接口!");
		return FALSE;
	}
	ITccAdviseSink *pAdviseSink = GetTccAdviseSink();
	if(NULL == pAdviseSink)
	{
		MessageBox("没有安装TccAdviseSink接口!");
		return FALSE;
	}

	InitListCtrl(&m_CmdList);

	CRect rcl;
	GetDlgItem(IDC_TCC_STATUS_LIST)->GetWindowRect(&rcl);
	ScreenToClient(&rcl);
	m_StccStatusGrid.Create(rcl, this, 102);

	GetDlgItem(IDC_TCC_LINE_STATUS_GRID)->GetWindowRect(&rcl);
	ScreenToClient(&rcl);
	m_StccLineStatusGrid.Create(rcl, this, 103);

	//OnRequestTccCmdList();	// 申请状态列表
	// 权限控制
	if(!GetCancelRight())
	{
		CWnd* pWnd = GetDlgItem(IDC_TCC_CANCELCMD_BTN);
		if(pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
	}
		
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BEGIN_MESSAGE_MAP(CLircCommandPage, CPropertyPage)
	ON_BN_CLICKED(IDC_TCC_REFRESH, OnRequestTccCmdList)
	ON_CBN_SELCHANGE(IDC_TCC_LIST, OnRequestTccCmdList)
	ON_BN_CLICKED(IDC_TCC_CANCELCMD_BTN, OnCancelLimitSpeed)
	ON_COMMAND(ID_CANCEL_LIMITSPEED, OnCancelLimitSpeed)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(NM_DBLCLK, IDC_TCC_CMD_LIST, OnNMDBClickCmdList)
END_MESSAGE_MAP()


// CLircCommandPage 消息处理程序
void CLircCommandPage::OnNMDBClickCmdList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	LPNMLISTVIEW pNMLISTVIEW = (NMLISTVIEW*)pNMHDR;
	int nItem = pNMLISTVIEW->iItem;
	if(nItem < 0)
		return ;

	TCC_COMMAND *pcmd = (TCC_COMMAND*)m_CmdList.GetItemData(nItem);
	if(pcmd)
	{
		CString prompt = pcmd->toString( GetTccClientInfoProvider() );
		MessageBox(prompt);
	}

	*pResult = 0;
}

// 刷新限速命令列表
void CLircCommandPage::OnRequestTccCmdList()
{
	stano_t nTccID = GetCurrentTccID();
	if(0 == nTccID)
	{
		MessageBox("请选择车站!");
		return ;
	}

	// 清空命令列表
	for(int i=0; i<m_CmdList.GetItemCount(); i++)
	{
		TCC_COMMAND *pcmd = (TCC_COMMAND*)m_CmdList.GetItemData(i);
		if(pcmd)
			delete pcmd;
	}
	m_CmdList.DeleteAllItems();

	// 清空列控状态
	m_StccStatusGrid.ClearStatus();

	// 清空线路状态
	m_StccLineStatusGrid.ClearStatus();

	// 申请命令列表
	GetTccAdviseSink()->OnRequestTccCmdList(nTccID, GetSafeHwnd());
}

bool CLircCommandPage::GetExecuteRight()
{
	TCC::ITccClientInfoProvider* pIProvider = GetTccClientInfoProvider();
	if(pIProvider == NULL)
		return false;
	
	if(pIProvider->IsCTCGPC())
	{
		if(pIProvider->IsTCCExecute())
			return true;
	}
	return false;
}

bool CLircCommandPage::GetCancelRight()
{
	TCC::ITccClientInfoProvider* pIProvider = GetTccClientInfoProvider();
	if(pIProvider == NULL)
		return false;
	
	if(!pIProvider->IsTCCReadOnly())
		return true;

	return false;
}

// 取消限速，先通过调度命令检验、确认后分解到服务器
void CLircCommandPage::OnCancelLimitSpeed()
{
	// 权限控制
	if(!GetCancelRight())
		return;
	
	INT nIndex = m_CmdList.GetCurSel();
	if(CB_ERR == nIndex)
		return ;

	TCC_COMMAND *pcmd = (TCC_COMMAND*)m_CmdList.GetItemData(nIndex);
	if(pcmd)
	{
		STOCK_COMMAND *pStockCmd = GetTccDlg()->GetConfirmCommand(pcmd->nConsoleID, pcmd->sys_id, pcmd->nCmdID);
		//if(NULL == pStockCmd)
		//	pStockCmd = GetTccDlg()->GetLocalCommand(pcmd->nConsoleID, pcmd->sys_id, pcmd->nCmdID);
		GetTccAdviseSink()->OnCancelTccCommand(*pcmd, pStockCmd, GetSafeHwnd());
	}
}

void CLircCommandPage::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// 权限控制
	if(!GetCancelRight())
		return;

	INT nIndex = m_CmdList.GetCurSel();
	if(CB_ERR == nIndex)
		return;
    
	CMenu menu;
	menu.CreatePopupMenu();
	menu.InsertMenu(
		0, 
		MF_BYPOSITION|MF_STRING, 
		ID_CANCEL_LIMITSPEED, 
		"生成取消限速调度命令");

	menu.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		point.x, point.y, 
		this);
}

CStccStatusGrid::CStccStatusGrid()
{
}

CStccStatusGrid::~CStccStatusGrid()
{
}

IMPLEMENT_DYNAMIC(CStccStatusGrid, CGridCtrl)

BEGIN_MESSAGE_MAP(CStccStatusGrid, CGridCtrl)
END_MESSAGE_MAP()

BOOL CStccStatusGrid::Create(const RECT& rect, CWnd* parent, UINT nID, DWORD dwStyle)
{
	if(!CGridCtrl::Create(rect, parent, nID, dwStyle))
		return FALSE;


	CGridCtrl& grid = *this;

	CRect rcClient;
	grid.GetClientRect(&rcClient);

	try{
		//grid.SetRowCount(1);                //行
		grid.SetColumnCount(10);             //列
		//grid.SetFixedRowCount(1);        //固定行
	//	grid.SetFixedColumnCount(1);     //固定列		    
		grid.SetFixedTextColor(RGB(0,0,0));      //文本颜色
		grid.SetGridLineColor(RGB(166,202,240));       //线的颜色
	//    grid.SetFixedBkColor(RGB(255,242,240));
		grid.SetEditable(FALSE);
		grid.SetColumnWidth(0, 100);
		grid.SetColumnWidth(1, 50);
		grid.SetColumnWidth(2, 100);
		grid.SetColumnWidth(3, 50);
		grid.SetColumnWidth(4, 100);
		grid.SetColumnWidth(5, 50);
		grid.SetColumnWidth(6, 100);
		grid.SetColumnWidth(7, 50);
		grid.SetColumnWidth(8, 100);
		grid.SetColumnWidth(9, 50);
	}
   	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	
	GV_ITEM item;
	item.mask = GVIF_TEXT|GVIF_FORMAT;
	item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

	item.row = grid.InsertRow("列控通信状态");
	item.row = grid.InsertRow("列控A");
	item.row = grid.InsertRow("列控B");;
	item.row = grid.InsertRow("联锁通道A");
	item.row = grid.InsertRow("联锁通道B");
	item.row = grid.InsertRow("CTC通道A");
	item.row = grid.InsertRow("CTC通道B");

	item.col = 2;
	
	for(; item.col < 10; )
	{
		item.row = 0;
		item.strText.Format("LEU%d状态", item.col/2);
		grid.SetItem(&item);
		
		item.row++;
		for(; item.row < 5; item.row++)
		{
			item.strText.Format("LEU%d应答器%d", item.col/2, item.row);
			grid.SetItem(&item);
		}

		item.col += 2;
	}

	for(int row=0; row<grid.GetRowCount(); row++)
	{
		for(int col=0; col<grid.GetColumnCount(); )
		{
			SetItemBkColour(row ,col , RGB(255,242,240));
			col+=2;
		}
	}
	
	grid.SetSingleRowSelection();
    ExpandColumnsToFit();

	return TRUE;
}
void CStccStatusGrid::ClearStatus()
{
	for(int row=0; row<GetRowCount(); row++)
	{
		for(int col=1; col<GetColumnCount();)
		{
			SetItemText(row, col, NULL);
			col += 2;
		}
	}

	for(int col=1; col<GetColumnCount();)
	{
		RedrawColumn(col);
		col += 2;
	}

}

void CLircCommandPage::Tcc_Report(DWORD nTccID, TCC_STATUS& status)
{
	if(nTccID != GetCurrentTccID())
		return ;
	m_StccStatusGrid.SetStatus(status);

	// 设置线路状态
	lineid_t nTccLineID[8];
	dir_t    dir[8];
	for(int i=0; i<status.init_section_num; i++)
	{
		nTccLineID[i] = status.init_line_no[i];
		dir[i] = status.init_direction[i];
	}
	m_StccLineStatusGrid.SetStatus(GetCurrentTccID(), nTccLineID, dir, status.init_section_num);
}

void CStccStatusGrid::SetStatus(TCC_STATUS& status)
{
	CString str;
	int row = 0;

	COLORREF GREEN = RGB(0, 255, 0);
	COLORREF RED = RGB(255, 0, 0);

	COLORREF clr = RED;
	// 列控通信状态	(0=断;1=单机;2=双机)
	switch(status.comm_active)
	{
	case 0:
	default:
		str = "断";	
		clr = RED;
		break;
	case 1:
		str = "单机";
		clr = GREEN;
		break;
	case 2:
		str = "双机";
		clr = GREEN;
		break;
	}
	SetItemFgColour(row, 1, clr);
	SetItemText(row++, 1, str);

	// 列控主备状态
	for(int i=0; i<2; i++, row++)
	{
		switch(status.stcc_master[i])
		{
		case 0:
			str = "备机";
			clr = 0;
			break;
		case 1:
			str = "主机";
			clr = GREEN;
			break;
		default:
			str.Format("未知 %d", status.stcc_master[i]);
			clr = 0;
			break;
		}
		SetItemFgColour(row, 1, clr);
		SetItemText(row, 1, str);

	}

	// 联锁通道状态
	for(i=0; i<2; i++,row++)
	{
		switch(status.ilock_channel[i])
		{
		case 0:
			str = "断";
			clr = RED;
			break;
		case 1:
			str = "通";
			clr = GREEN;
			break;
		default:
			str.Format("未知 %d", status.ilock_channel[i]);
			clr = 0;
			break;
		}
		SetItemFgColour(row, 1, clr);
		SetItemText(row, 1, str);
	}

	// CTC通道状态
	for(i=0; i<2; i++,row++)
	{
		switch(status.ctc_channel[i])
		{
		case 0:
			str = "断";
			clr = RED;
			break;
		case 1:
			str = "通";
			clr = GREEN;
			break;
		default:
			str.Format("未知 %d", status.ctc_channel[i]);
			clr = 0;
			break;
		}
		SetItemFgColour(row, 1, clr);
		SetItemText(row, 1, str);
	}

	row = 0;
	// LEU通道状态及应答器
	for(i=0; i<4; i++)
	{
//LEU通道状态		
		switch(status.leu_channel[i])
		{
		case 0:
			str = "断";
			clr = RED;
			break;
		case 1:
			str = "通";
			clr = GREEN;
			break;
		default:
			str.Format("未知 %d", status.leu_channel[i]);
			clr = 0;
			break;
		}
		SetItemFgColour(row, i*2+3, clr);
		SetItemText(row++, i*2+3, str);

// 应答器状态
		for(int j=0; j<4; j++)
		{
			switch(status.leu_ack[i*4+j])
			{
			case 0:
				str = "断";
				clr = RED;
				break;
			case 1:
				str = "通";
				clr = GREEN;
				break;
			default:
				str.Format("未知 %d", status.leu_ack[i*4+j]);
				clr = 0;
				break;
			}
			SetItemFgColour(row,  i*2+3, clr);
			SetItemText(row++,  i*2+3, str);
		}

		row = 0;
	}	


	Refresh();
}


CStccLineStatusGrid::CStccLineStatusGrid()
	: m_nCurrentTccID(0)
{
}

CStccLineStatusGrid::~CStccLineStatusGrid()
{
}

ITccClientInfoProvider* CStccLineStatusGrid::GetTccClientInfoProvider()
{
	HWND hWnd = GetTccDialogWndHandle();
	ASSERT(hWnd);
	CTccDlg *pTccDlg = (CTccDlg*)CWnd::FromHandle(hWnd);
	ASSERT_VALID(pTccDlg);

	return pTccDlg->GetTccClientInfoProvider();
}

ITccAdviseSink* CStccLineStatusGrid::GetTccAdviseSink()
{
	HWND hWnd = GetTccDialogWndHandle();
	ASSERT(hWnd);
	CTccDlg *pTccDlg = (CTccDlg*)CWnd::FromHandle(hWnd);
	ASSERT_VALID(pTccDlg);

	return pTccDlg->GetTccAdviseSink();
}
stano_t CStccLineStatusGrid::GetCurrentTccID()
{
	HWND hWnd = GetTccDialogWndHandle();
	ASSERT(hWnd);
	CTccDlg *pTccDlg = (CTccDlg*)CWnd::FromHandle(hWnd);
	ASSERT_VALID(pTccDlg);

	return pTccDlg->GetCurrentTccID();
}


IMPLEMENT_DYNAMIC(CStccLineStatusGrid, CGridCtrl)

BEGIN_MESSAGE_MAP(CStccLineStatusGrid, CGridCtrl)
	//{{AFX_MSG_MAP(CShuntNoteGou)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_INIT_STCC_LINE, OnInitStccLine)
	ON_COMMAND(ID_INIT_STCC_LINE2, OnInitStccLine2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CStccLineStatusGrid::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	if(!CGridCtrl::Create(rect, pParentWnd, nID, dwStyle))
		return FALSE;

	struct COLINFO { TCHAR* label;	INT width;	INT fmt; }
	colinfo[] =
	{
		{"线路", 120, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"区间", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"相邻站", 120, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"状态", 120,   DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"处理", 120, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
	};

	CRect rcClient;
	GetClientRect(&rcClient);

	int nColNum = sizeof(colinfo)/sizeof(colinfo[0]);
	try{
		SetRowCount(1);                //行
		SetColumnCount(nColNum);             //列
		SetFixedRowCount(1);        //固定行
		SetFixedColumnCount(3);     //固定列		    
		SetFixedTextColor(RGB(0,0,0));      //文本颜色
		SetGridLineColor(RGB(166,202,240));       //线的颜色
	    SetFixedBkColor(RGB(255,242,240));

		for(int i=0; i<GetColumnCount(); i++)
		{
			SetColumnWidth(i, colinfo[i].width);
		}
	}
   	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	GV_ITEM item;
	item.mask = GVIF_TEXT|GVIF_FORMAT;
	item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

	item.row = 0;  //行
	for(item.col = 0; item.col < GetColumnCount(); item.col++)
	{
		item.strText.Format(colinfo[item.col].label);
		item.nFormat = colinfo[item.col].fmt;
		SetItem(&item);
	}
	
	SetSingleRowSelection();
//    ExpandColumnsToFit();

	return TRUE;
}

void CStccLineStatusGrid::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// 权限控制
	TCC::ITccClientInfoProvider* pIProvider = GetTccClientInfoProvider();
	if(pIProvider == NULL)
		return;
	
	if(pIProvider->IsCTCGPC())
	{
		if(!pIProvider->IsTCCExecute())
			return;
	}
	else
	{
		return;
	}
			
	CCellID cellFocus = GetFocusCell();
	if(cellFocus.row <1 || cellFocus.row >= GetRowCount())
		return;

	int nInitComplete = (int)GetItemData(cellFocus.row, 3);
	
	CString str = GetItemText(cellFocus.row, 1);

	if(str == "侧线限速")
	{
		CMenu menu;
		menu.CreatePopupMenu();

		if(nInitComplete == 2)
		{
			// 不能多次初始化列控中心
			if(!gpcdef.IsReInitTCC(2))
			{
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING|MF_GRAYED, 
					ID_INIT_STCC_LINE2, 
					"侧线初始化");
			}
			else
			{
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING, 
					ID_INIT_STCC_LINE2, 
					"侧线初始化");
			}
		}
		else
		{
			menu.AppendMenu(
					MF_BYPOSITION|MF_STRING, 
					ID_INIT_STCC_LINE2, 
					"侧线初始化");
		}

		menu.TrackPopupMenu(
			TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
			point.x, point.y, 
			this);
	}
	else
	{
		CMenu menu;
		menu.CreatePopupMenu();
		if(nInitComplete == 2)
		{
			// 不能多次初始化列控中心
			if(!gpcdef.IsReInitTCC(2))
			{
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING|MF_GRAYED, 
					ID_INIT_STCC_LINE, 
					"限速初始化");
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING|MF_GRAYED, 
					ID_INIT_STCC_LINE2, 
					"全线限速初始化");
			}
			else
			{
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING, 
					ID_INIT_STCC_LINE, 
					"限速初始化");
				menu.AppendMenu(
					MF_BYPOSITION|MF_STRING, 
					ID_INIT_STCC_LINE2, 
					"全线限速初始化");
			}
		}
		else
		{
			menu.AppendMenu(
				MF_BYPOSITION|MF_STRING, 
				ID_INIT_STCC_LINE, 
				"限速初始化");
			menu.AppendMenu(
				MF_BYPOSITION|MF_STRING, 
				ID_INIT_STCC_LINE2, 
				"全线限速初始化");
		}
		

		menu.TrackPopupMenu(
			TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
			point.x, point.y, 
			this);
	}
}


void CStccLineStatusGrid::OnInitStccLine()
{
	CCellID cellFocus = GetFocusCell();
	if(cellFocus.row <1 || cellFocus.row >= GetRowCount())
		return;
	INT row = cellFocus.row;

	if(0 == GetItemData(row, 3))
	{
		MessageBox("不能初始化!");
		return ;
	}

	extern std::vector<CString> AvailablePWDList;
	CSuperPassDlg  dlg, dlg1;
	dlg.m_strPrompt = "请输入第一重密码";
	dlg1.m_strPrompt = "请输入第二重密码";

	if(AvailablePWDList.size()==0)
	{
		stano_t nTccID = GetCurrentTccID();
		lineid_t nTccLineID = (lineid_t)GetItemData(row, 0);
		dir_t dir = (dir_t)GetItemData(row, 1);

		GetTccAdviseSink()->OnSendInitCommand(nTccID, nTccLineID, dir, GetSafeHwnd());
	}
	else
	{
		if (dlg.DoModal() == IDOK)
		{

			CString str = AvailablePWDList[0];
							
			if(dlg.m_super_pass.Compare(str) != 0)
			{
				CString str;
				str.Format("%s","密码错误!");
				CDlgMessageBox dlg(str);
				dlg.DoModal();
				return;
			}
			else
			{
				if (dlg1.DoModal() == IDOK)
				{
					CString str = AvailablePWDList[1];
							
					if(dlg1.m_super_pass.Compare(str) != 0)
					{
						CString str;
						str.Format("%s","密码错误!");
						CDlgMessageBox dlg(str);
						dlg.DoModal();
						return;
					}
				}
				else
					return;
			}
			
			stano_t nTccID = GetCurrentTccID();
			lineid_t nTccLineID = (lineid_t)GetItemData(row, 0);
			dir_t dir = (dir_t)GetItemData(row, 1);

			GetTccAdviseSink()->OnSendInitCommand(nTccID, nTccLineID, dir, GetSafeHwnd());

		}
	}

}

void CStccLineStatusGrid::OnInitStccLine2()
{
	CCellID cellFocus = GetFocusCell();
	if(cellFocus.row <1 || cellFocus.row >= GetRowCount())
		return;
	INT row = cellFocus.row;
	
	if(0 == GetItemData(row, 3))
	{
		MessageBox("不能全线初始化!");
		return ;
	}

	extern std::vector<CString> AvailablePWDList;
	CSuperPassDlg  dlg, dlg1;
	dlg.m_strPrompt = "请输入第一重密码";
	dlg1.m_strPrompt = "请输入第二重密码";

	if(AvailablePWDList.size()==0)
	{
		stano_t nTccID = GetCurrentTccID();
		lineid_t nTccLineID = (lineid_t)GetItemData(row, 0);

		GetTccAdviseSink()->OnSendInitCommand(nTccID, nTccLineID, DIR_DUAL, GetSafeHwnd());
	}

	else
	{
		if (dlg.DoModal() == IDOK)
		{

			CString str = AvailablePWDList[0];
							
			if(dlg.m_super_pass.Compare(str) != 0)
			{
				CString str;
				str.Format("%s","密码错误!");
				CDlgMessageBox dlg(str);
				dlg.DoModal();
				return;
			}
			else
			{
				if (dlg1.DoModal() == IDOK)
				{
					CString str = AvailablePWDList[1];
							
					if(dlg1.m_super_pass.Compare(str) != 0)
					{
						CString str;
						str.Format("%s","密码错误!");
						CDlgMessageBox dlg(str);
						dlg.DoModal();
						return;
					}
				}
				else
					return;
			}
			stano_t nTccID = GetCurrentTccID();
			lineid_t nTccLineID = (lineid_t)GetItemData(row, 0);

			GetTccAdviseSink()->OnSendInitCommand(nTccID, nTccLineID, DIR_DUAL, GetSafeHwnd());
		}
	}
}

void CStccLineStatusGrid::SelectStation(stano_t nStaID)
{
	DeleteNonFixedRows();

	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

	GV_ITEM item;
	item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
	item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

	std::vector<lineid_t> lines;
	pInfoProvider->GetTccLineList(nStaID, lines);

	stano_t nLinkStaID = 0;
	for(int i=0; i<lines.size(); i++)
	{
		lineid_t nTccLineID = lines[i];

		TCC::dir_t dir = pInfoProvider->GetTccLineAbsNum(nStaID, nTccLineID);
		
		if(TCC::DIR_UP == dir || TCC::DIR_DUAL == dir)
		{
			item.col = 0;
			item.row = InsertRow(pInfoProvider->GetTccLineName(nStaID, nTccLineID));
			SetItemData(item.row, item.col, nTccLineID);

			item.col ++;
			item.strText = "上行方向";
			item.lParam = DIR_UP;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);

			nLinkStaID = pInfoProvider->GetLinkedStation(nStaID, nTccLineID, DIR_UP);
			if(nLinkStaID)
				item.strText = pInfoProvider->GetStationName(nLinkStaID);
			else
				item.strText.Empty();
			item.col ++;
			item.lParam = nLinkStaID;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);

			item.col ++;
			item.strText = "未知";
			SetItemBkColour(item.row, item.col, RGB(255, 0, 0));
			item.lParam = 0;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);
			SetItemState(item.row, item.col, GVIS_READONLY);

			item.col ++;
			SetItemState(item.row, item.col, GVIS_READONLY);
		}

		if(TCC::DIR_DOWN == dir || TCC::DIR_DUAL == dir)
		{
			item.col = 0;
			item.row = InsertRow(pInfoProvider->GetTccLineName(nStaID, nTccLineID));
			SetItemData(item.row, item.col, nTccLineID);

			item.col ++;
			item.strText = "下行方向";
			item.lParam = DIR_DOWN;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);

			nLinkStaID = pInfoProvider->GetLinkedStation(nStaID, nTccLineID, DIR_DOWN);
			if(nLinkStaID)
				item.strText = pInfoProvider->GetStationName(nLinkStaID);
			else
				item.strText.Empty();
			item.col ++;
			item.lParam = nLinkStaID;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);

			item.col ++;
			item.strText = "未知";
			SetItemBkColour(item.row, item.col, RGB(255, 0, 0));
			item.lParam = 0;
			item.nFormat = GetItemFormat(0, item.col);
			SetItem(&item);
			SetItemState(item.row, item.col, GVIS_READONLY);

			item.col ++;
			SetItemState(item.row, item.col, GVIS_READONLY);
		}
	}

	lines.clear();
	pInfoProvider->GetTccBlockLineList(nStaID, lines);

	for(int i=0; i<lines.size(); i++)
	{
		lineid_t nTccLineID = lines[i];
		
		item.col = 0;
		item.row = InsertRow(pInfoProvider->GetTccBlockLineName(nStaID, nTccLineID));
		SetItemData(item.row, item.col, nTccLineID);

		item.col ++;
		item.strText = "侧线限速";
		item.lParam = DIR_UP;
		item.nFormat = GetItemFormat(0, item.col);
		SetItem(&item);

		item.col ++;
		item.strText.Empty();
		item.lParam = 0;
		item.nFormat = GetItemFormat(0, item.col);
		SetItem(&item);

		item.col ++;
		item.strText = "未知";
		SetItemBkColour(item.row, item.col, RGB(255, 0, 0));
		item.lParam = 0;
		item.nFormat = GetItemFormat(0, item.col);
		SetItem(&item);
		SetItemState(item.row, item.col, GVIS_READONLY);

		item.col ++;
		SetItemState(item.row, item.col, GVIS_READONLY);

	}
	Refresh();
}

void CStccLineStatusGrid::ClearStatus()
{
	for(int row=GetFixedRowCount(); row<GetRowCount(); row++)
	{
		SetItemText(row, 3, "未知");
		SetItemData(row, 3, 0);
		SetItemBkColour(row, 3, RGB(255,255,0));
		SetItemText(row, 4, NULL);
	}
	RedrawColumn(3);
	RedrawColumn(4);
}

void CStccLineStatusGrid::SetStatus(stano_t nStaID, lineid_t nTccLineID[], dir_t dir[], INT num)
{
	if(nStaID != GetCurrentTccID())
		return;

	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

	GV_ITEM item;
	item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
	item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

	item.col = 3;
	for(int row=GetFixedRowCount(); row<GetRowCount(); row++)
	{
		item.row = row;
		item.strText = "正常";
		SetItemBkColour(item.row, item.col, RGB(0, 255, 0));
		item.lParam = 2;
		SetItem(&item);
	}

	for(int i=0; i<num; i++)
	{
		stano_t nLinkStaID = pInfoProvider->GetLinkedStation(nStaID, nTccLineID[i], dir[i]);
		for(int row=GetFixedRowCount(); row<GetRowCount(); row++)
		{
			item.row = row;
			if(GetItemData(row, 0) != nTccLineID[i]
				|| GetItemData(row, 1) != dir[i]
				|| GetItemData(row, 2) != nLinkStaID)
			{
				continue;
			}
			item.strText = "限速未初始化";
			SetItemBkColour(item.row, item.col, RGB(255, 0, 0));
			item.lParam = 1;
			SetItem(&item);
		}
	}
	RedrawColumn(item.col);
}

void CLircCommandPage::SetTccLineInitialCommandOperResult(
		stano_t nTccID, 
		lineid_t nTccLineID, 
		dir_t dir,
		DWORD nStatus,
		CString strComment)
{
	if(nTccID != GetCurrentTccID())
		return;

	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

	//侧线限速
	BOOL bSidings = pInfoProvider->IsTccBlockLine(nTccID, nTccLineID);
	if(bSidings)
	{
		GV_ITEM item;
		item.mask = GVIF_TEXT|GVIF_FORMAT;
		item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

		for(int row=m_StccLineStatusGrid.GetFixedRowCount(); row<m_StccLineStatusGrid.GetRowCount(); row++)
		{
			if(m_StccLineStatusGrid.GetItemData(row, 0)== nTccLineID)
			{
				item.row = row;
				item.col = 4;
				item.strText = strComment;
				item.lParam = 0;
				m_StccLineStatusGrid.SetItem(&item);
				
				m_StccLineStatusGrid.RedrawCell(row, item.col);
			}
		}
	}
	else
	//
	{
		if(dir == DIR_DUAL)
		{
			SetTccLineInitialCommandOperResult(nTccID, nTccLineID, DIR_UP, nStatus, strComment);
			SetTccLineInitialCommandOperResult(nTccID, nTccLineID, DIR_DOWN, nStatus, strComment);
		}
		else
		{
			stano_t nLinkStaID = pInfoProvider->GetLinkedStation(nTccID, nTccLineID, dir);

			GV_ITEM item;
			item.mask = GVIF_TEXT|GVIF_FORMAT;
			item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

			for(int row=m_StccLineStatusGrid.GetFixedRowCount(); row<m_StccLineStatusGrid.GetRowCount(); row++)
			{
				if(m_StccLineStatusGrid.GetItemData(row, 0) != nTccLineID
					|| (m_StccLineStatusGrid.GetItemData(row, 1) != dir)
					|| m_StccLineStatusGrid.GetItemData(row, 2) != nLinkStaID)
					continue;

				item.row = row;
				item.col = 4;
				item.strText = strComment;
				item.lParam = 0;
				m_StccLineStatusGrid.SetItem(&item);
				
				m_StccLineStatusGrid.RedrawCell(row, item.col);
			}
		}
	}

}


};
