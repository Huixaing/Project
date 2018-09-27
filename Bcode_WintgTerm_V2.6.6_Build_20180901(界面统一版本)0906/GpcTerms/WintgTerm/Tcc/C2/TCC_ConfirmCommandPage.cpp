/*
 * 本地限速命令管理
 */

#include "stdafx.h"
#include "resource.h"
#include <sysshare.h>
#include "Tcc_Dlg.h"

namespace TCC {



void InitListCtrl(CGridCtrl& grid)
{
	struct COLINFO { TCHAR* label;	INT width;	INT fmt; }
	colinfo[] =
	{
		{"命令号", 60, DT_RIGHT | DT_WORDBREAK|DT_NOPREFIX },
		{"开始站", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"结束站", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"上下行", 100, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"开始公里标", 80, DT_CENTER | DT_WORDBREAK|DT_NOPREFIX },
		{"结束公里标", 80, DT_CENTER | DT_WORDBREAK|DT_NOPREFIX},
		{"限速值", 60, DT_RIGHT | DT_WORDBREAK|DT_NOPREFIX },
		{"状态", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"本站状态", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"生成时间", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"本站执行时间", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"开始时间", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"结束时间", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
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

	// 开始站
	item.col ++;
	if(cmd.nStaID[0])
		item.strText = pInfoProvider->GetStationName(cmd.nStaID[0]);
	else
		item.strText="";
	//item.lParam = cmd.nStaID[0];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 结束站
	item.col ++;
	if(cmd.nStaID[1])
		item.strText = pInfoProvider->GetStationName(cmd.nStaID[1]);
	else
		item.strText="";
	//item.lParam = cmd.nStaID[1];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	item.mask |= GVIF_STATE;
	item.nState = GVIS_READONLY;

	// 上下行
	item.col ++;
	item.strText = pInfoProvider->GetTccLineName(cmd.nStaID[0], cmd.nLineID);
	if(item.strText.IsEmpty())
		item.strText = gpcdef.GetC3TccLineName(cmd.nLineID);	
	//item.lParam = cmd.nLineID;
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 开始公里标
	item.col ++;
	if(cmd.bFullLimit)
		str.Format("全限速");
	else
		str = cmd.nKmMark[0].toString();
	item.strText = str;
	//item.lParam = cmd.nKmMark[0];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 结束公里标
	item.col ++;
	if(cmd.bFullLimit)
		str.Format("全限速");
	else
		str = cmd.nKmMark[1].toString();
	item.strText = str;
	//item.lParam = cmd.nKmMark[1];
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

	// 设置颜色
	COLORREF clrBk = grid.GetGridBkColor();
	COLORREF clrFg = grid.GetGridColor();
	str.Empty();
	if(nConfirmNum == cmd.tcc.size())
	{
		grid.SetItemFgColour(item.row, 0, RGB(0, 255, 0));
		str.Format("全部确认");
	}
	else if(nExecNum == cmd.tcc.size())
	{
        grid.SetItemFgColour(item.row, 1, RGB(0, 0, 255));
		str.Format("全部执行");
	}
	else if(nCancelNum == cmd.tcc.size())
	{
		if(grid.GetItemText(item.row, 2).IsEmpty())
			grid.SetItemBkColour(item.row, 2, RGB(255, 0, 0));
		else
			grid.SetItemFgColour(item.row, 2, RGB(255, 0, 0));
		str.Format("全部取消");
	}
	else
	{
		if(nUnknownNum)
		{
			str += "存在未知状态";
		}
		if(nExecNum)
		{
			grid.SetItemFgColour(item.row, 1, RGB(0, 0, 255));
			str += "部分执行";
		}
		if(nCancelNum)
		{
			if(grid.GetItemText(item.row, 2).IsEmpty())
				grid.SetItemBkColour(item.row, 2, RGB(255, 0, 0));
			else
				grid.SetItemFgColour(item.row, 2, RGB(255, 0, 0));
			str += "部分取消";
		}
		//if(nConfirmNum)
		//{
		//	grid.SetItemFgColour(item.row, 0, RGB(0, 255, 0));
		//	str = "全部确认" + str;
		//}
	}

	// 设置状态
	item.col ++;
	item.strText = str;
	//item.lParam = 0;
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 本站状态
	item.col ++;
	for(i=0; i<cmd.tcc.size(); i++)
	{
		if (cmd.tcc[i].nID != GetCurrentTccID())
			continue;

		switch(cmd.tcc[i].nStatus)
		{
		case CONFIRM:
			item.strText = "已确认";
			break;
		case EXECUTING:
			item.strText = "已执行";
			break;
		case CANCELED:
			item.strText = "已取消";
			break;
		default:
			item.strText.Format("未知状态 %d", cmd.tcc[i].nStatus);
			break;
		}
		break;		
	}
	item.lParam = 0;
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	CTime ct;
	// 生成时间
	item.col ++;
	if(cmd.nCreateTime <= 0)
		str.Format("0");
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
	return GetTccDlg()->GetCurrentTccID();
}

void CConfirmCommandPage::Refresh()
{
	OnBtnClickedTccRefresh();
}

STOCK_COMMAND* CConfirmCommandPage::GetCommand(DWORD nConsoleID, sysid_t& sysid, WORD nCmdID)
{
	sysid_t nullsysid;
	UuidCreateNil(&nullsysid);
	if(sysid == nullsysid)
		return NULL;

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
		/*else if(pcmd->nCmdID == nCmdID && !pcmd->bCancelLimit)
		{
			WORD status = pcmd->GetTccStatus(GetCurrentTccID());
			if(EXECUTING == status)
			{
				p = pcmd;
				break;
			}
			if(CANCELED != status)
				p = pcmd;
		}*/
	}
	return p;
}

void CConfirmCommandPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_TCC_CMD_NOTE, m_CmdString);
	DDX_Control(pDX, IDC_TCC_CMD_STATUS, m_OperPrompt);
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

	// 增加权限控制
	if(!GetExecuteRight())
	{
		CButton* pBtn = (CButton*)GetDlgItem(IDC_TCC_SENDCMD_BTN);
		if(pBtn)
		{
			pBtn->EnableWindow(FALSE);
		}
	}

	if(!GetEditRight())
	{
		CButton* pBtn = (CButton*)GetDlgItem(IDC_TCC_DELCMD_BTN);
		if(pBtn)
		{
			pBtn->EnableWindow(FALSE);
		}
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
	ON_BN_CLICKED(IDC_TCC_DELCMD_BTN, OnDelStockCommand)
	ON_COMMAND(ID_SEND_LIMITSPEED, OnSendTccCommand)
	ON_COMMAND(ID_DEL_TCCCOMMAND, OnDelPartialStockCommand)
	ON_COMMAND(ID_DEL_STOCKCOMMAND, OnDelStockCommand)
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

	GetTccAdviseSink()->OnRequestStockCommandList(nTccID, GetSafeHwnd(), 0);
}

// 执行权限判断
bool CConfirmCommandPage::GetExecuteRight()
{
	TCC::ITccClientInfoProvider* pIProvider = GetTccClientInfoProvider();
	if(pIProvider == NULL)
		return false;
	
	if(pIProvider->IsCTCGPC())
	{
		if(pIProvider->IsTCCExecute())
			return true;
		else
			return false;
	}
	else
	{
		return false;
	}
}

bool CConfirmCommandPage::GetEditRight()
{
	// 增加权限控制
	TCC::ITccClientInfoProvider* pIProvider = GetTccClientInfoProvider();
	if(pIProvider == NULL)
		return false;
	if(!pIProvider->IsTCCReadOnly())
		return true;
	else
		return false;
}

void CConfirmCommandPage::OnSendTccCommand()
{
	// 增加权限控制
	if(!GetExecuteRight())
		return;
			
	if(NULL == m_pCurCmd)
		return ;

	stano_t nTccID = GetCurrentTccID();

	if(CONFIRM != m_pCurCmd->GetTccStatus(nTccID))
	{
		MessageBox("命令不能重复执行!");
		return;
	}
	
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();
	
	TCC_COMMAND cmd;
	if(!m_pCurCmd->GetTccCommand(&cmd, nTccID))
		return ;
	
	if(IDOK != MessageBox(m_pCurCmd->toString(pInfoProvider), "限速命令下达", MB_OKCANCEL))
		return ;
	
	// 全区间限速
	if(cmd.bFullLimit && 0 != cmd.nStaID[1])
	{
		ASSERT(0 != cmd.nStaID[1]);
		
		stano_t nLinkStaID = cmd.nStaID[0];
		if(nLinkStaID == cmd.nTccID)
			nLinkStaID = cmd.nStaID[1];
		
		switch(pInfoProvider->GetStationDirection(cmd.nTccID, nLinkStaID))
		{
		case DIR_UP:	// 上行区间全限速
			cmd.nKmMark[0] = cmd.nKmMark[1] = 0;
			break;
		case DIR_DOWN:	// 下行区间全限速
			cmd.nKmMark[0] = cmd.nKmMark[1] = 9999999;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	
	GetTccAdviseSink()->OnSendTccCommand(cmd, GetSafeHwnd());
	m_OperPrompt.SetWindowText("");
}


// 删除，需要向服务器发删除确认命令指令
void CConfirmCommandPage::OnDelPartialStockCommand()
{
	// 增加权限控制
	if(!GetEditRight())
		return;
		
	if(NULL == m_pCurCmd)
		return;

	if(IDOK == MessageBox(m_pCurCmd->toString(GetTccClientInfoProvider()), "限速命令删除", MB_OKCANCEL))
	{
		if(0 == GetTccAdviseSink()->OnDelStockCommand(*m_pCurCmd, GetCurrentTccID(), GetSafeHwnd()))
			OnBtnClickedTccRefresh();
	}
}

// 删除，需要向服务器发删除确认命令指令
void CConfirmCommandPage::OnDelStockCommand()
{
	// 增加权限控制
	if(!GetEditRight())
		return;
	
	if(NULL == m_pCurCmd)
		return ;

	if(IDOK == MessageBox(m_pCurCmd->toString(GetTccClientInfoProvider()), "限速命令删除", MB_OKCANCEL))
	{
		if(0 == GetTccAdviseSink()->OnDelStockCommand(*m_pCurCmd, 0, GetSafeHwnd()))
			OnBtnClickedTccRefresh();
	}
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
			"发送");
		bMenu = true;
	}
	
	if(GetEditRight())
	{
		//menu.AppendMenu(
		//	MF_BYPOSITION|MF_STRING, 
		//	ID_DEL_TCCCOMMAND, 
		//	"部分删除----删除本受令站");
		menu.AppendMenu(
			MF_BYPOSITION|MF_STRING, 
			ID_DEL_STOCKCOMMAND, 
			"删除");
			//"完全删除----删除所有受令站");
		bMenu = true;
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



};
