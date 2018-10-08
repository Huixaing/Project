/*
 * 本地限速命令管理
 */

#include "stdafx.h"
#include "resource.h"
#include <sysshare.h>
#include "Tcc_Dlg.h"
#include "TccCommandSendDlg.h"
#include "TccCommandStatusChangeDlg.h"
#include "PasswordInputDlg.h"

namespace TCC_C3 {



void InitListCtrl(CGridCtrl& grid)
{
	struct COLINFO { TCHAR* label;	INT width;	INT fmt; }
	colinfo[] =
	{
		{"命令号", 60, DT_RIGHT | DT_WORDBREAK|DT_NOPREFIX },
		{"调度命令号", 80, DT_RIGHT | DT_WORDBREAK|DT_NOPREFIX },
		{"开始站", 0, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"结束站", 0, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"上下行", 100, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"开始公里标", 80, DT_CENTER | DT_WORDBREAK|DT_NOPREFIX },
		{"结束公里标", 80, DT_CENTER | DT_WORDBREAK|DT_NOPREFIX},
		{"限速值", 60, DT_RIGHT | DT_WORDBREAK|DT_NOPREFIX },
		{"状态", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"本站状态", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"生成时间", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"本站执行时间", 100, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"开始时间", 0, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"结束时间", 0, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
	};

	CRect rcClient;
	grid.GetClientRect(&rcClient);

	try{
		grid.SetRowCount(1);                //行
		grid.SetColumnCount(sizeof(colinfo)/sizeof(colinfo[0]));             //列
		grid.SetFixedRowCount(1);        //固定行
		grid.SetFixedColumnCount(3);     //固定列		    
		grid.SetFixedTextColor(RGB(0,0,0));      //文本颜色
		grid.SetGridLineColor(RGB(166,202,240));       //线的颜色
	    grid.SetFixedBkColor(RGB(255,242,240));

		for(int i=0; i<grid.GetColumnCount(); i++)
		{
			grid.SetColumnWidth(i, colinfo[i].width);
		}
	}
   	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return ;
	}

	GV_ITEM item;
	item.mask = GVIF_TEXT|GVIF_FORMAT;
	item.nFormat = DT_CENTER| DT_WORDBREAK|DT_NOPREFIX;

	item.row = 0;  //行
	for(item.col = 0; item.col < grid.GetColumnCount(); item.col++)
	{
		item.strText.Format(colinfo[item.col].label);
		item.nFormat = colinfo[item.col].fmt;
		grid.SetItem(&item);
	}
	grid.SetHeaderSort(TRUE);
	grid.SetSingleRowSelection();
}

void CConfirmCommandPage::AppendCommand(STOCK_COMMAND& cmd)
{
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

	CGridCtrl& grid = m_CmdList;

	GV_ITEM item;
	item.row = item.col = 0;

	item.mask = GVIF_TEXT|GVIF_FORMAT;//|GVIF_PARAM;
	item.nFormat = DT_CENTER|DT_WORDBREAK|DT_NOPREFIX;

	CString str;
	str.Format("%4d", cmd.nCmdID);

	int nPos = 0;
	for(int i=1; i<grid.GetRowCount(); i++)
	{
		STOCK_COMMAND *pCmd = (STOCK_COMMAND *)grid.GetItemData(i, 0);
		if(pCmd->nConsoleID == cmd.nConsoleID
			&& pCmd->nCmdID == cmd.nCmdID
			&& pCmd->sys_id == cmd.sys_id
			&& pCmd->bCancelLimit == cmd.bCancelLimit)
		{
			*pCmd = cmd;
			break;
		}
	}
	if(i == grid.GetRowCount())
	{
		// 在屁股后增加
		STOCK_COMMAND *pCmd = new STOCK_COMMAND;
		ASSERT(pCmd);
		*pCmd = cmd;
		item.row = grid.InsertRow(str, -1);
		grid.SetItemFormat(item.row, item.col, grid.GetItemFormat(0, item.col));
		grid.SetItemData(item.row, 0, (DWORD)pCmd);
	}
	else
	{
		item.row = i;
		grid.SetItemText(item.row, 0, str);
	}
	grid.EnsureVisible(item.row, 0);

	// 调度命令号
	item.col ++;
	item.strText = cmd.dcmd_id;
	//item.lParam = ;
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 开始站
	item.col ++;
	item.strText = pInfoProvider->GetStationName(cmd.nStaID[0]);
	//item.lParam = cmd.nStaID[0];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 结束站
	item.col ++;
	item.strText = pInfoProvider->GetStationName(cmd.nStaID[1]);
	//item.lParam = cmd.nStaID[1];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	item.mask |= GVIF_STATE;
	item.nState = GVIS_READONLY;

	// 上下行
	item.col ++;
	if(cmd.nLineID)
		item.strText = pInfoProvider->GetLineName(cmd.nLineID);
	else
		item.strText = pInfoProvider->GetTccLineName(cmd.tcc[0].nID, cmd.tcc[0].nTccLineID);
	//item.lParam = cmd.nLineID;
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 开始公里标
	item.col ++;
	if(cmd.bFullLimit)
		str.Format("全限速");
	else
		str = cmd.nKmMark[0].toString(pInfoProvider, cmd.nLineID);
	item.strText = str;
	//item.lParam = (DWORD)cmd.nKmMark[0];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 结束公里标
	item.col ++;
	if(cmd.bFullLimit)
		str.Format("全限速");
	else
		str = cmd.nKmMark[1].toString(pInfoProvider, cmd.nLineID);
	item.strText = str;
	//item.lParam = (DWORD)cmd.nKmMark[1];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 限速值
	item.col ++;
	if(cmd.bCancelLimit)
	{
		grid.SetItemFgColour(item.row, item.col, RGB(255, 0, 0) );
		str.Format("取消限速");
	}
	else
	{
		grid.SetItemFgColour(item.row, item.col);
		str.Format("%3dKm/h", cmd.nSpeedMaxVal);
	}
	item.strText = str;
	//item.lParam = cmd.nSpeedMaxVal;
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 状态
	INT nConfirmNum = 0;
	INT nExecNum = 0;
	INT nCancelNum = 0;
	INT nUnknownNum = 0;
	for(i=0; i<cmd.tcc.size(); i++)
	{
		switch(cmd.tcc[i].nStatus)
		{
		case CONFIRM:
			nConfirmNum ++;
			break;
		case EXECUTING:
			nExecNum ++;
			break;
		case CANCELED:
			nCancelNum ++;
			break;
		default:
			nUnknownNum ++;
			break;
		}
	}

	// 设置状态 和 颜色
	// 确认为绿，执行或取消为灰， 未知为红
	// 全部确认	命令号（绿）、命令状态（绿）、本站状态（绿）
	// 全部执行 命令号（灰）、命令状态（灰）、本站状态（灰）
	// 全部取消 同 全部执行
	// 部分执行 命令号（兰）、命令状态（蓝）、本站状态（绿或灰）
	// 部分取消 同 部分执行
	item.col ++;

	COLORREF clrBk = grid.GetGridBkColor();
	COLORREF clrFg = grid.GetGridColor();
	str.Empty();
	if(nConfirmNum == cmd.tcc.size())
	{
		grid.SetItemFgColour(item.row, 0, RGB(0, 255, 0));
		grid.SetItemFgColour(item.row, item.col, RGB(0, 255, 0));
		str.Format("全部确认");
	}
	else if(nExecNum == cmd.tcc.size())
	{
		grid.SetItemFgColour(item.row, 0, RGB(127, 127, 127));
		grid.SetItemFgColour(item.row, item.col, RGB(127, 127, 127));//RGB(0, 0, 255));
		str.Format("全部执行");
	}
	else if(nCancelNum == cmd.tcc.size())
	{
		//if(grid.GetItemText(item.row, 2).IsEmpty())
		//	grid.SetItemBkColour(item.row, 2, RGB(255, 0, 0));
		//else
		//	grid.SetItemFgColour(item.row, 2, RGB(255, 0, 0));
		grid.SetItemFgColour(item.row, 0, RGB(127, 127, 127));
		grid.SetItemFgColour(item.row, item.col, RGB(127, 127, 127));//RGB(0, 0, 255));
		str.Format("全部取消");
	}
	else
	{
		if(nUnknownNum)
		{
			grid.SetItemFgColour(item.row, 0, RGB(255, 0, 0));
			grid.SetItemFgColour(item.row, item.col, RGB(255, 0, 0));//RGB(0, 0, 255));
			str += "存在未知状态";
		}
		else
		{
			if(nExecNum)
			{
				grid.SetItemFgColour(item.row, 0, RGB(0, 0, 255));
				grid.SetItemFgColour(item.row, item.col, RGB(0, 0, 255));//RGB(0, 0, 255));
				str += "部分执行";
			}
			if(nCancelNum)
			{
				//if(grid.GetItemText(item.row, 2).IsEmpty())
				//	grid.SetItemBkColour(item.row, 2, RGB(255, 0, 0));
				//else
				//	grid.SetItemFgColour(item.row, 2, RGB(255, 0, 0));
				grid.SetItemFgColour(item.row, 0, RGB(0, 0, 255));
				grid.SetItemFgColour(item.row, item.col, RGB(0, 0, 255));//RGB(0, 0, 255));
				str += "部分取消";
			}
			//if(nConfirmNum)
			//{
			//	grid.SetItemFgColour(item.row, 0, RGB(0, 255, 0));
			//	str = "全部确认" + str;
			//}
		}
	}
	item.strText = str;
	//item.lParam = 0;
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 本站状态
	item.col ++;
	for (i=0; i<cmd.tcc.size(); i++)
	{
		if (cmd.tcc[i].nID != GetCurrentTccID())
			continue;

		switch(cmd.tcc[i].nStatus)
		{
		case CONFIRM:
			item.strText = "已确认";
			grid.SetItemFgColour(item.row, item.col, RGB(0, 255, 0));
			break;
		case EXECUTING:
			item.strText = "已执行";
			grid.SetItemFgColour(item.row, item.col, RGB(127, 127, 127));
			break;
		case CANCELED:
			item.strText = "已取消";
			grid.SetItemFgColour(item.row, item.col, RGB(127, 127, 127));
			break;
		default:
			item.strText.Format("未知状态 %d", cmd.tcc[i].nStatus);
			grid.SetItemFgColour(item.row, item.col, RGB(255, 0, 0));
			break;
		}
	}
	item.lParam = 0;
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	CTime ct;
	// 生成时间
	item.col ++;
	if(cmd.nCreateTime <= 0)
		str.Format("");
	else
	{
		ct = cmd.nCreateTime;
		str= ct.Format("%m-%d %H:%M");
	}
	item.strText = str;
	//item.lParam = ;
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 本站执行时间
	item.col ++;
	for (i=0; i<cmd.tcc.size(); i++)
	{
		if (cmd.tcc[i].nID != GetCurrentTccID())
			continue;

		if(cmd.tcc[i].nTime <= 0)
			str.Format("");
		else
		{
			ct = cmd.tcc[i].nTime;
			str= ct.Format("%m-%d %H:%M");
		}
		item.strText = str;
		//item.lParam = ;
		item.nFormat = grid.GetItemFormat(0, item.col);
		grid.SetItem(&item);
	}

	// 限速开始时间
	item.col ++;
	if(cmd.bStartNow || cmd.nTime[0] <= 0)
	{
		str = "立即开始";
	}
	else
	{
		ct = cmd.nTime[0];
		str = ct.Format("%m-%d %H:%M");
	}
	item.strText = str;
	//item.lParam = cmd.nTime[0];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 限速结束时间
	item.col ++;
	if(cmd.bEndless || cmd.nTime[1] <= 0)
		str.Format("永久有效");
	else
	{
		ct = cmd.nTime[1];
		str= ct.Format("%m-%d %H:%M");
	}
	item.strText = str;
	//item.lParam = cmd.nTime[1];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	grid.RedrawRow(item.row);
}

// CConfirmCommandPage 对话框

IMPLEMENT_DYNAMIC(CConfirmCommandPage, CPropertyPage)
CConfirmCommandPage::CConfirmCommandPage()
	: CPropertyPage(CConfirmCommandPage::IDD)
	, m_pCurCmd(NULL)
	, m_bTimeBegin(1), m_bTimeEnd(1)
{
}

CConfirmCommandPage::~CConfirmCommandPage()
{
}

CTccDlg* CConfirmCommandPage::GetTccDlg()
{
	CWnd *pWnd = GetParent();
	ASSERT(pWnd);
	pWnd = pWnd->GetParent();
	ASSERT(pWnd);

	return (CTccDlg*)pWnd;
}

ITccAdviseSink* CConfirmCommandPage::GetTccAdviseSink()
{
	return GetTccDlg()->GetTccAdviseSink();
}

ITccClientInfoProvider* CConfirmCommandPage::GetTccClientInfoProvider()
{
	return GetTccDlg()->GetTccClientInfoProvider();
}

stano_t CConfirmCommandPage::GetCurrentTccID()
{
	/*stano_t nTccID = 0;
	int nIndex = m_TccList.GetCurSel();
	if(CB_ERR == nIndex)
		return 0;
	nTccID = m_TccList.GetItemData(nIndex);
	return nTccID;*/

	return GetTccDlg()->GetCurrentTccID();
}

void CConfirmCommandPage::Refresh()
{
	OnBtnClickedTccRefresh();
}

STOCK_COMMAND* CConfirmCommandPage::GetCommand(DWORD nConsoleID, sysid_t& sysid, WORD nCmdID)
{
	STOCK_COMMAND *p = NULL;
	for(int row=m_CmdList.GetFixedRowCount(); row<m_CmdList.GetRowCount(); row++)
	{
		STOCK_COMMAND *pcmd = (STOCK_COMMAND*)m_CmdList.GetItemData(row, 0);
		if(NULL == pcmd)
			continue;
		if(pcmd->sys_id == sysid)
		{
			p = pcmd;
			break;
		}
		else if(pcmd->nCmdID == nCmdID && !pcmd->bCancelLimit)
		{
			WORD status = pcmd->GetTccStatus(GetCurrentTccID());
			if(EXECUTING == status)
			{
				p = pcmd;
				break;
			}
			if(CANCELED != status)
				p = pcmd;
		}
	}
	return p;
}

void CConfirmCommandPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_TCC_CMD_NOTE, m_CmdString);
	DDX_Control(pDX, IDC_TCC_CMD_STATUS, m_OperPrompt);

	//DDX_Control(pDX, IDC_TCC_LIST, m_TccList);

	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN, m_tmBegin);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND, m_tmEnd);

	DDX_Radio(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO, m_bTimeBegin);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO, m_BeginRadio);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO2, m_BeginRadio2);
	DDX_Radio(pDX, IDC_TCC_CMD_TIMEEND_RADIO, m_bTimeEnd);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND_RADIO, m_EndRadio);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND_RADIO2, m_EndRadio2);
}

void CConfirmCommandPage::SetCommandOperResult(TCC_COMMAND& cmd, DWORD nStatus, LPCSTR strComment)
{
	m_OperPrompt.SetWindowText(strComment);
}

void CConfirmCommandPage::ShowCmd(STOCK_COMMAND *pcmd)
{
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

	if(pcmd)
		m_CmdString = pcmd->toString(pInfoProvider);
	else
		m_CmdString.Empty();
	
	UpdateData(FALSE);
}


#define GRIDCTRL 101
BOOL CConfirmCommandPage::OnInitDialog()
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

	CRect rcl;
	GetDlgItem(IDC_TCC_CMD_LIST)->GetWindowRect(&rcl);
	ScreenToClient(rcl);
	m_CmdList.Create(rcl, this, GRIDCTRL);
	InitListCtrl(m_CmdList);

	// 初始化开始站、结束站
	CString str;
	int nIndex;
	std::vector<stano_t> staList;
	pInfoProvider->GetStationList(staList);
	for(int i=0; i<staList.size(); i++)
	{
		str.Format("%s (%d)", pInfoProvider->GetStationName(staList[i]), staList[i]);
		//nIndex = m_TccList.AddString(str);
		//ASSERT(CB_ERR != nIndex);
		//m_TccList.SetItemData(nIndex, staList[i]);
	}

	//查询时间范围
	m_tmBegin.SetFormat("yyy'-'MM'-'dd HH':'mm");
	m_tmEnd.SetFormat("yyy'-'MM'-'dd HH':'mm");

	CTime ct;
	ct = CTime::GetCurrentTime();
	m_tmEnd.SetTime(ct.GetTime());

	ct -= CTimeSpan(2, 0, 0, 0);
	m_tmBegin.SetTime(ct.GetTime());

	m_tmBegin.EnableWindow(m_bTimeBegin == 0);
	m_tmEnd.EnableWindow(m_bTimeEnd == 0);

		// 权限限制
	if(!GetExecuteRight())
	{
		CWnd* pWnd = GetDlgItem(IDC_TCC_SENDCMD_BTN);
		if(pWnd)
			pWnd->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BEGIN_MESSAGE_MAP(CConfirmCommandPage, CPropertyPage)
	ON_NOTIFY(NM_CLICK, GRIDCTRL, OnNMClickTccCmdList)
	ON_NOTIFY(NM_RCLICK, GRIDCTRL, OnNMClickTccCmdList)
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_TCC_SENDCMD_BTN, OnSendTccCommand)
	ON_BN_CLICKED(IDC_TCC_REFRESH, OnBtnClickedTccRefresh)
	ON_COMMAND(ID_SEND_LIMITSPEED, OnSendTccCommand)
	ON_COMMAND(ID_DEL_PARTIAL_STOCKCOMMAND, OnDelPartialStockCommand)
	ON_COMMAND(ID_DEL_STOCKCOMMAND, OnDelStockCommand)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEBEGIN_RADIO2, OnBtnClickedTccCmdTimebeginRadio2)
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEBEGIN_RADIO,  OnBtnClickedTccCmdTimebeginRadio)
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEEND_RADIO2,   OnBtnClickedTccCmdTimeendRadio2)
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEEND_RADIO,    OnBtnClickedTccCmdTimeendRadio)
	ON_COMMAND(ID_TCCCOMMAND_STATUS_CHANGE, OnTccCommandStatusChange)
END_MESSAGE_MAP()

// CConfirmCommandPage 消息处理程序
//

void CConfirmCommandPage::OnNMClickTccCmdList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW *)pNMHDR;
	int nItem = pItem->iRow;
	if(nItem < 1)
		return ;

	// 设置当前选择的命令
	STOCK_COMMAND *pCmd = (STOCK_COMMAND *)m_CmdList.GetItemData(nItem, 0);
	if(m_pCurCmd != pCmd)
	{
		m_pCurCmd = pCmd;
		ShowCmd(m_pCurCmd);
	}

	*pResult = 0;
}


void CConfirmCommandPage::OnBtnClickedTccRefresh()
{
	UpdateData(TRUE);

	stano_t nTccID = GetCurrentTccID();
	if(0 == nTccID)
	{
		MessageBox("请选择车站!");
		return ;
	}

	m_pCurCmd = NULL;
	ShowCmd(m_pCurCmd);

	for(int row=m_CmdList.GetFixedRowCount(); row<m_CmdList.GetRowCount(); row++)
	{
		STOCK_COMMAND *pcmd = (STOCK_COMMAND *)m_CmdList.GetItemData(row, 0);
		if(pcmd)
			delete pcmd;
		m_CmdList.SetItemData(row, 0, 0);
	}
	m_CmdList.DeleteNonFixedRows();
	m_CmdList.Refresh();

	CTime ct;
	m_tmBegin.GetTime(ct);
	time_t tmBegin = ct.GetTime();

	m_tmEnd.GetTime(ct);
	time_t tmEnd = ct.GetTime();

	if(m_bTimeBegin)
		tmBegin = 0;
	if(m_bTimeEnd)
		tmEnd = 0;

	GetTccAdviseSink()->OnRequestStockCommandList(0, nTccID, dcmd_id_t(), tmBegin, tmEnd, GetSafeHwnd(), 0);
	//GetTccAdviseSink()->OnRequestStockCommandList(nTccID, GetSafeHwnd());
}

void CConfirmCommandPage::OnSendTccCommand()
{
	// 权限限制
	if(!GetExecuteRight())
		return;

	if(NULL == m_pCurCmd)
		return;

	stano_t nTccID = GetCurrentTccID();
	TCC_COMMAND cmd;
	if(m_pCurCmd->GetTccCommand(&cmd, nTccID))
	{
		WORD nStatus = m_pCurCmd->GetTccStatus(nTccID);
		switch(nStatus)
		{
		case CONFIRM:
			{
				CTccCommandSendDlg dlg(cmd, CTccCommandSendDlg::TCC_COMMAND_CONFIRM, m_pCurCmd, GetTccClientInfoProvider(), GetTccAdviseSink(), this);
				dlg.DoModal();
				OnBtnClickedTccRefresh();  // cuihu 2008 下达后确认列表状态改变，自动刷新
			}
			break;
		case EXECUTING:
			MessageBox("已执行过的命令，不能再次下达!");
			break;
		case CANCELED:
			MessageBox("已取消的命令，不能下达!");
			break;
		default:
			break;
		}

		//if(IDOK == MessageBox(m_pCurCmd->toString(pInfoProvider), "限速命令下达", MB_OKCANCEL))
		//{
		// 全区间限速
		//if(cmd.bFullLimit && 0 != cmd.nStaID[1])
		//{
		//	ASSERT(0 != cmd.nStaID[1]);
		//
		//	stano_t nLinkStaID = cmd.nStaID[0];
		//	if(nLinkStaID == cmd.nTccID)
		//		nLinkStaID = cmd.nStaID[1];
		//
		//	switch(pInfoProvider->GetStationDirection(cmd.nTccID, nLinkStaID))
		//	{
		//	case DIR_UP:	// 上行区间全限速
		//		cmd.nKmMark[0] = cmd.nKmMark[1] = 0;
		//		break;
		//	case DIR_DOWN:	// 下行区间全限速
		//		cmd.nKmMark[0] = cmd.nKmMark[1] = 9999999;
		//		break;
		//	default:
		//		ASSERT(FALSE);
		//		break;
		//	}
		//}
		//GetTccAdviseSink()->OnSendTccCommand(cmd, GetSafeHwnd());
		//}
	}
}

// 删除，需要向服务器发删除确认命令指令
void CConfirmCommandPage::OnDelPartialStockCommand()
{
	// 权限限制
	if(!GetEditRight())
		return;

	MessageBox("暂不支持限速命令的部分删除!");
	return;

	if(NULL == m_pCurCmd)
		return ;

	if(IDOK == MessageBox(m_pCurCmd->toString(GetTccClientInfoProvider()), "限速命令删除", MB_OKCANCEL))
	{
		if(0 == GetTccAdviseSink()->OnDelStockCommand(*m_pCurCmd, GetCurrentTccID(), GetSafeHwnd()))
			OnBtnClickedTccRefresh();
	}
}

// 删除，需要向服务器发删除确认命令指令
void CConfirmCommandPage::OnDelStockCommand()
{
	// 权限限制
	if(!GetEditRight())
		return;

	if(NULL == m_pCurCmd)
		return ;

	for(int i=0; i<m_pCurCmd->tcc.size(); i++)
	{
		if(m_pCurCmd->tcc[i].nStatus != EXECUTING && m_pCurCmd->tcc[i].nStatus != CANCELED)
		{
			MessageBox("未全部执行或取消的命令不允许删除!");
			return;
		}
	}
	if(IDOK == MessageBox(m_pCurCmd->toString(GetTccClientInfoProvider()), "限速命令删除", MB_OKCANCEL))
	{
		if(0 == GetTccAdviseSink()->OnDelStockCommand(*m_pCurCmd, 0, GetSafeHwnd()))
			OnBtnClickedTccRefresh();
	}
}

bool CConfirmCommandPage::GetExecuteRight()
{
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();
	if(pInfoProvider==NULL)
		return false;

	if(pInfoProvider->IsCTC2GPC())
	{
		if(pInfoProvider->IsTCC2Execute())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CConfirmCommandPage::GetEditRight()
{
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();
	if(pInfoProvider!=NULL)
	{
		if(!pInfoProvider->IsTCC2ReadOnly())
		{
			return true;
		}
	}
	
	return false;
}

void CConfirmCommandPage::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CCellID cellFocus = m_CmdList.GetFocusCell();
	if(cellFocus.row <1 || cellFocus.row >= m_CmdList.GetRowCount())
		return;
	INT nIndex = cellFocus.row;
	if(NULL==m_pCurCmd)
		return;

	CMenu menu;
	menu.CreatePopupMenu();
    
	bool bMenu = false;
	if(GetExecuteRight())
	{
		menu.AppendMenu(
				MF_BYPOSITION|MF_STRING, 
				ID_SEND_LIMITSPEED, 
				"下发限速命令");
		bMenu = true;
	}
	
	if(GetEditRight())
	{
		menu.AppendMenu(
				MF_BYPOSITION|MF_STRING, 
				ID_DEL_STOCKCOMMAND, 
				"删除限速命令");
		
		bMenu = true;
	}

	if(bMenu && gpcdef.IsEnableModifyTsrStatus())
	{
		menu.AppendMenu(
			MF_BYPOSITION|MF_STRING, 
			ID_TCCCOMMAND_STATUS_CHANGE, 
			"修正命令状态");
	}

	if(!bMenu) return;

	menu.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		point.x, point.y, 
		this);
}

void CConfirmCommandPage::OnDestroy()
{
	CPropertyPage::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	STOCK_COMMAND *pcmd = NULL;
	for(int i=m_CmdList.GetFixedRowCount(); i<m_CmdList.GetRowCount(); i++)
	{
		pcmd = (STOCK_COMMAND*)m_CmdList.GetItemData(i, 0);
		if(pcmd)
			delete pcmd;
	}
	m_CmdList.DeleteAllItems();
	m_pCurCmd = NULL;
}

void CConfirmCommandPage::OnCancel()
{
	CPropertyPage::OnCancel();

	 GetTccDlg()->SendMessage(WM_COMMAND, IDCANCEL);
}


void CConfirmCommandPage::OnBtnClickedTccCmdTimeendRadio2()
{
	m_tmEnd.EnableWindow(FALSE);
}

void CConfirmCommandPage::OnBtnClickedTccCmdTimeendRadio()
{
	m_tmEnd.EnableWindow(TRUE);
}

void CConfirmCommandPage::OnBtnClickedTccCmdTimebeginRadio2()
{
	m_tmBegin.EnableWindow(FALSE);
}

void CConfirmCommandPage::OnBtnClickedTccCmdTimebeginRadio()
{
	m_tmBegin.EnableWindow(TRUE);
}

void CConfirmCommandPage::OnTccCommandStatusChange()
{
	if(NULL == m_pCurCmd)
		return ;

	stano_t nTccID = GetCurrentTccID();
	if(0 >= nTccID)
		return;

	CPasswordInputDlg passwdinput(this);
	if(IDOK != passwdinput.DoModal())
		return;

	if(passwdinput.GetPassword() != "1234")
	{
		MessageBox("密码不正确!");
		return;
	}

	CTccCommandStatusChangeDlg dlg(nTccID, *m_pCurCmd, GetTccClientInfoProvider(), GetTccAdviseSink(), this);
	if(IDOK == dlg.DoModal())
	{
		Refresh();
	}
}


};
