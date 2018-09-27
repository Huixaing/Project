/*
 * 列控 当前限速命令列表
 */

#include "stdafx.h"
#include "resource.h"
#include <sysshare.h>
#include "Tcc_Dlg.h"
#include "TccCommandSendDlg.h"
#include "TccCommand_FullCancel.h"
#include "PasswordInputDlg.h"

namespace TCC_C3 {


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

	int builtin_id = pInfoProvider->GetLineID(cmd.nTccID, cmd.nTccLineID);
	// 上下行
	lvi.iSubItem = COL_ABSLINEID;
	str.Format("%s(builtin_id=%d)", pInfoProvider->GetTccLineName(cmd.nTccID, cmd.nTccLineID), cmd.nLineID);
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// 开始公里标
	lvi.iSubItem = COL_START_KM;
	if(cmd.bFullLimit)
		str.Format("全限速");
	else
		str = cmd.nKmMark[0].toString(pInfoProvider, builtin_id);
	lvi.pszText = (LPSTR)(LPCSTR)str;
	pListCtrl->SetItem(&lvi);

	// 结束公里标
	lvi.iSubItem = COL_END_KM;
	if(cmd.bFullLimit)
		str.Format("全限速");
	else
		str = cmd.nKmMark[1].toString(pInfoProvider, builtin_id);
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
		{"开始站",      0, LVCFMT_LEFT },
		{"结束站",      0, LVCFMT_LEFT },
		{"上下行",		100, LVCFMT_LEFT },
		{"开始公里标",	80, LVCFMT_LEFT },
		{"结束公里标",	80, LVCFMT_LEFT },
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

	//OnRequestTccCmdList();	// 申请状态列表
	// 权限限制
	if(!GetCancelRight())
	{
		CWnd* pWnd = GetDlgItem(IDC_TCC_CANCELCMD_BTN);
		if(pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
	}

	// 权限限制
	if(!GetExecuteRight())
	{
		CWnd* pWnd = GetDlgItem(IDC_TCC_INIT_COMPLETE);
		if(pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
		pWnd = GetDlgItem(IDC_TCC_COMMAND_FULLCANCEL);
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
	ON_BN_CLICKED(IDC_TCC_INIT_COMPLETE, OnTccInitComplete)
	ON_BN_CLICKED(IDC_TCC_COMMAND_FULLCANCEL, OnTccCommand_FullCancel)
	ON_COMMAND(ID_CANCEL_LIMITSPEED, OnCancelLimitSpeed)
	ON_COMMAND(ID_CANCEL_LIMITSPEED_DIRECT, OnCancelLimitSpeedDirect)
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
	m_bInitComplete = FALSE;

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

	// 申请命令列表
	GetTccAdviseSink()->OnRequestTccCmdList(nTccID, GetSafeHwnd());
}

// 取消限速，先通过调度命令检验、确认后分解到服务器
void CLircCommandPage::OnCancelLimitSpeed()
{
	// 权限限制
	if(!GetCancelRight())
		return;

	INT nIndex = m_CmdList.GetCurSel();
	if(CB_ERR == nIndex)
		return ;

	TCC_COMMAND *pcmd = (TCC_COMMAND*)m_CmdList.GetItemData(nIndex);
	if(pcmd)
	{
		STOCK_COMMAND *pStockCmd = GetTccDlg()->GetConfirmCommand(pcmd->nConsoleID, pcmd->sys_id, pcmd->nCmdID);
		if(NULL == pStockCmd)
			pStockCmd = GetTccDlg()->GetLocalCommand(pcmd->nConsoleID, pcmd->sys_id, pcmd->nCmdID);
		GetTccAdviseSink()->OnCancelTccCommand(*pcmd, pStockCmd, GetSafeHwnd());
	}
}

bool CLircCommandPage::GetExecuteRight()
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

bool CLircCommandPage::GetCancelRight()
{
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();
	if(pInfoProvider==NULL)
		return false;

	if(pInfoProvider->IsTCC2ReadOnly())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CLircCommandPage::OnCancelLimitSpeedDirect()
{
	// 权限限制
	if(!GetExecuteRight())
		return;

	if(!gpcdef.IsCanCancelLimitDcmdDirect(3))
		return;
	INT nIndex = m_CmdList.GetCurSel();
	if(CB_ERR == nIndex)
		return ;

	TCC_COMMAND *pcmd = (TCC_COMMAND*)m_CmdList.GetItemData(nIndex);
	if(pcmd)
	{
		ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

		TCC_COMMAND cmd = *pcmd;
		int builtin_id = pInfoProvider->GetLineID(cmd.nTccID, cmd.nTccLineID);
		if(cmd.nLineID != builtin_id)
		{
			MessageBox("自律机返回的线路编号与列控线路号不一致，以列控线路号为主!");
			cmd.nLineID = builtin_id;
		}
		cmd.bCancelLimit = TRUE;

		if(IDOK != MessageBox(cmd.toString(pInfoProvider), "强制限速命令取消", MB_OKCANCEL))
			return;

		//std::vector<CString> pwds = m_pInfoProvider->GetPassword(ITccClientInfoProvider::DIRECT_CANCEL_LIRC_LIMITSPEED);
		CPasswordInputDlg passwdinput(this);
		if(IDOK == passwdinput.DoModal())
		{
			if(passwdinput.GetPassword() != "1234")
			{
				MessageBox("密码不正确!");
				return;
			}

			CTccCommandSendDlg dlg(cmd, CTccCommandSendDlg::TCC_COMMAND_CANCEL, NULL,
				pInfoProvider, GetTccAdviseSink(), this);
			dlg.DoModal();

			//GetTccAdviseSink()->OnSendTccCommand(cmd, GetSafeHwnd());
		}
	}
}

void CLircCommandPage::OnContextMenu(CWnd* pWnd, CPoint point)
{
	INT nIndex = m_CmdList.GetCurSel();
	if(CB_ERR == nIndex)
		return;

	CMenu menu;
	menu.CreatePopupMenu();
    
	bool bMenu = false;
	// 权限限制
	if(GetCancelRight())
	{
		menu.AppendMenu(
				MF_BYPOSITION|MF_STRING, 
				ID_CANCEL_LIMITSPEED, 
				"生成[取消限速]调度命令");
		bMenu = true;
	}

	// 权限限制
	if(GetExecuteRight())
	{
		if(gpcdef.IsCanCancelLimitDcmdDirect(3))
		{
			menu.AppendMenu(
				MF_BYPOSITION|MF_STRING, 
				ID_CANCEL_LIMITSPEED_DIRECT, 
				"强制取消限速");
			bMenu = true;
		}
	}

	if(!bMenu) return;

	menu.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		point.x, point.y, 
		this);
}

void CLircCommandPage::OnTccInitComplete()
{
	// 权限限制
	if(!GetExecuteRight())
		return;

	stano_t nTccID = GetCurrentTccID();
	if(0 == nTccID)
	{
		MessageBox("请选择车站!");
		return ;
	}

	if(m_bInitComplete)
	{
		if(IDOK != MessageBox("注意：列控中心以初始化或进行中，你确认再次初始化吗?", "初始化完成", MB_OKCANCEL|MB_ICONSTOP))
			return;
	}

	if(IDOK != MessageBox("注意：当列控中心故障恢复或重新启动后，需首先发送该站所有限速命令，然后再进行本\"初始化完成\"操作", "初始化完成", MB_OKCANCEL|MB_ICONSTOP))
		return;

	CPasswordInputDlg passwdinput(this);
	if(IDOK == passwdinput.DoModal())
	{
		if(passwdinput.GetPassword() != "1234")
		{
			MessageBox("密码不正确!");
			return;
		}
	}
	else
	{
		return;
	}

	TCC_COMMAND tcccmd;
	tcccmd.nConsoleID = GetTccClientInfoProvider()->GetConsoleID();
	tcccmd.nCmdID = 10000;
	SysidCreate(&tcccmd.sys_id);
	tcccmd.nTccID = nTccID;
	CTccCommandSendDlg dlg(tcccmd, CTccCommandSendDlg::TCC_COMMAND_INITCOMPLETE, NULL, GetTccClientInfoProvider(), GetTccAdviseSink());
	dlg.DoModal();
}

void CLircCommandPage::OnTccCommand_FullCancel()
{
	// 权限限制
	if(!GetExecuteRight())
		return;

	stano_t nTccID = GetCurrentTccID();
	if(0 == nTccID)
	{
		MessageBox("请选择车站!");
		return ;
	}

	if(IDOK != MessageBox("注意：\"无限速\"操作将取消指定车站、指定线路上的所有限速，即设置为无限速状态。确定继续吗?", "无限速", MB_OKCANCEL|MB_ICONSTOP))
		return;

	CTccCommand_FullCancel dlg(nTccID, GetTccClientInfoProvider(), GetTccAdviseSink());
	dlg.DoModal();
}




CStccStatusGrid::CStccStatusGrid()
{
}

CStccStatusGrid::~CStccStatusGrid()
{
}

IMPLEMENT_DYNAMIC(CStccStatusGrid, CGridCtrl)

BEGIN_MESSAGE_MAP(CStccStatusGrid, CGridCtrl)
	ON_WM_TIMER()
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
		grid.SetColumnCount(5);             //列
		//grid.SetFixedRowCount(1);        //固定行
		grid.SetFixedColumnCount(1);     //固定列		    
		grid.SetFixedTextColor(RGB(0,0,0));      //文本颜色
		grid.SetGridLineColor(RGB(166,202,240));       //线的颜色
	//    grid.SetFixedBkColor(RGB(255,242,240));
		grid.SetEditable(FALSE);
		grid.SetColumnWidth(0, 80);
		grid.SetColumnWidth(1, 120);
		grid.SetColumnWidth(2, 100);
		grid.SetColumnWidth(3, 120);
		grid.SetColumnWidth(4, 150);
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

	grid.SetSingleRowSelection();
    //ExpandColumnsToFit();

	return TRUE;
}
void CStccStatusGrid::ClearStatus()
{
	SetRowCount(0);
	KillTimer(8341);
}

void CLircCommandPage::Tcc_Report(DWORD nTccID, TCC_STATUS& status)
{
	if(nTccID != GetCurrentTccID())
		return ;

	m_StccStatusGrid.SetStatus(status);
	KillTimer(8341);
	if(status.init_status == 1 || status.init_status == 2)
	{
		m_bInitComplete = TRUE;

		CWnd* pWnd = GetDlgItem(IDC_TCC_INIT_COMPLETE);
		if(!GetExecuteRight())
		{
			if(pWnd)
			{
				pWnd->EnableWindow(FALSE);
			}
		}
		else
		{
			// 不能多次初始化列控中心
			if(!gpcdef.IsReInitTCC(3))
			{
				pWnd->EnableWindow(FALSE);
			}
			else
			{
				pWnd->EnableWindow(TRUE);
			}
		}
	}
	else
	{
		SetTimer(8341, 500, NULL);

		CWnd* pWnd = GetDlgItem(IDC_TCC_INIT_COMPLETE);
		if(!GetExecuteRight())
		{
			if(pWnd)
			{
				pWnd->EnableWindow(FALSE);
			}
		}
		else
		{
			pWnd->EnableWindow(TRUE);
		}
	}
}

	COLORREF GREEN = RGB(0, 255, 0);
	COLORREF RED = RGB(255, 0, 0);
void GetConnectingUI(BYTE v, CString& str)
{
		switch(v)
		{
		case 0:
			str = "断";
			break;
		case 1:
			str = "通";
			break;
		case 3:
			str = "无效";
			break;
		default:
			str.Format("(?%d)", v);
			break;
		}
}

void CStccStatusGrid::SetStatus(TCC_STATUS& status)
{
	//if(status.nStaNum)
	//	SetRowCount(18*status.nStaNum+1);
	//else
	//	SetRowCount(19);
	//for(int row=0; row<GetRowCount(); row++)
	//{
	//	for(int col=1; col<GetColumnCount(); col+=2)
	//		SetItemBkColour(row ,col , RGB(255,242,240));
	//}

	SetRowCount(0);
	SetBkColor(RGB(255,242,240));
	CString str, str2, str3;
	int row = 0;

	COLORREF clr = RED;

	str.Format("发送站%d", status.nStaID);
	//SetItemText(row, 0, str);
	row = InsertRow(str);

	SetItemText(row, 1, "列控通信");
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
	SetItemFgColour(row, 2, clr);
	SetItemText(row, 2, str);

	SetItemText(row, 3, "初始化状态");
	// 初始化状态	(0=断;1=单机;2=双机)
	switch(status.init_status)
	{
	case 0:
	default:
		str = "未初始化";	
		clr = RED;
		break;
	case 1:
		str = "正初始化...";
		clr = GREEN;
		break;
	case 2:
		str = "已完成";
		clr = GREEN;
		break;
	}
	SetItemFgColour(row, 4, clr);
	SetItemText(row, 4, str);
	
	for(int iSta=0; iSta<status.nStaNum||iSta<1; iSta++)
	{
		str.Format("站%d", status.sta[iSta].nTccID);
		//SetItemText(row, 0, str);
		row = InsertRow(str);
		SetItemBkColour(row, 0, RGB(255, 0, 0));

		// 列控主备状态
		SetItemText(row, 1, "列控状态");
		switch(status.sta[iSta].standby_status)
		{
		case 2:
			switch(status.sta[iSta].master_status)
			{
			case 2:
				str = "A备,B主";
				clr = 0;
				break;
			case 1:
				str = "A主,B备";
				clr = GREEN;
				break;
			default:
				str.Format("双机, 未知%d", status.sta[iSta].master_status);
				clr = 0;
				break;
			}
			clr = GREEN;
			break;
		case 1:
			switch(status.sta[iSta].master_status)
			{
			case 2:
				str = "A断,B主";
				clr = 0;
				break;
			case 1:
				str = "A主,B断";
				clr = GREEN;
				break;
			default:
				str.Format("单机, 未知%d", status.sta[iSta].master_status);
				clr = 0;
				break;
			}
			clr = GREEN;
			break;
		default:
			str.Format("A断,B断(未定义%d)", status.sta[iSta].standby_status);
			clr = RED;
			break;
		}
		SetItemFgColour(row, 2, clr);
		SetItemText(row, 2, str);

		SetItemText(row, 3, "与CTC");
		// CTC通道状态
		GetConnectingUI(status.sta[iSta].ctc_channel[0], str2);
		GetConnectingUI(status.sta[iSta].ctc_channel[1], str3);
		str.Format("A%s,B%s", str2, str3);
		SetItemText(row, 4, str);
		if(1 == status.sta[iSta].ctc_channel[0] || 1 == status.sta[iSta].ctc_channel[1])
			clr = GREEN;
		else
			clr = RED;
		SetItemFgColour(row, 4, clr);

		//row++;

		row = InsertRow("");
		// 联锁通道状态
		SetItemText(row, 1, "与联锁");
		GetConnectingUI(status.sta[iSta].ilock_channel[0], str2);
		GetConnectingUI(status.sta[iSta].ilock_channel[1], str3);
		str.Format("A%s,B%s", str2, str3);
		SetItemText(row, 2, str);
		if(1 == status.sta[iSta].ilock_channel[0] || 1 == status.sta[iSta].ilock_channel[1])
			clr = GREEN;
		else
			clr = RED;
		SetItemFgColour(row, 2, clr);

		//SetItemText(row, 3, "与交换机");
		//GetConnectingUI(status.sta[iSta].switch_channel[0], str2);
		//GetConnectingUI(status.sta[iSta].switch_channel[1], str3);
		//str.Format("A%s,B%s", str2, str3);
		//SetItemText(row, 4, str);
		//if(1 == status.sta[iSta].switch_channel[0] || 1 == status.sta[iSta].switch_channel[1])
		//	clr = GREEN;
		//else
		//	clr = RED;
		//SetItemFgColour(row, 4, clr);

		//row ++;

		for(int iLeu=0; iLeu<16; iLeu++)
		{
			//if(!status.sta[iSta].leu[iLeu].bValid)
			//	continue;
			str.Format("LEU%d", iLeu);
			//SetItemText(row, 0, str);
			row = InsertRow(str);
			
			//LEU通道状态
			SetItemText(row, 1, "通道状态");
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_channel[0], str2);
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_channel[1], str3);
			str.Format("A%s,B%s", str2, str3);
			SetItemText(row, 2, str);
			if(1 == status.sta[iSta].leu[iLeu].leu_channel[0]
				|| 1 == status.sta[iSta].leu[iLeu].leu_channel[1])
				clr = GREEN;
			else
				clr = RED;
			SetItemFgColour(row, 2, clr);

			// LEU应答器状态
			SetItemText(row, 3, "应答器状态");
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_ack[0], str2);
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_ack[1], str3);
			str.Format("0%s,1%s", str2, str3);
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_ack[2], str2);
			GetConnectingUI(status.sta[iSta].leu[iLeu].leu_ack[3], str3);
			str += ",2" + str2 +",3" + str3;

			if(1 == status.sta[iSta].leu[iLeu].leu_ack[0]
				|| 1 == status.sta[iSta].leu[iLeu].leu_ack[1]
				|| 1 == status.sta[iSta].leu[iLeu].leu_ack[2]
				|| 1 == status.sta[iSta].leu[iLeu].leu_ack[3])
				clr = GREEN;
			else
				clr = RED;
			SetItemFgColour(row, 4, clr);
			SetItemText(row, 4, str);
			//row++;
		}

		for(int iZC=0; iZC < 8 && iZC < status.sta[iSta].nZCNum; iZC++)
		{
			str.Format("与中继%d", status.sta[iSta].ZC[iZC].nStaID);
			//SetItemText(row, 0, str);
			row = InsertRow(str);
			
			//通道状态
			SetItemText(row, 1, "通道A");
			GetConnectingUI(status.sta[iSta].ZC[iZC].channel[0], str);
			SetItemText(row, 2, str);
			if(1 == status.sta[iSta].ZC[iZC].channel[0])
				clr = GREEN;
			else
				clr = RED;
			SetItemFgColour(row, 2, clr);
			SetItemText(row, 3, "通道B");
			GetConnectingUI(status.sta[iSta].ZC[iZC].channel[1], str);
			SetItemText(row, 4, str);
			if(1 == status.sta[iSta].ZC[iZC].channel[1])
				clr = GREEN;
			else
				clr = RED;
			SetItemFgColour(row, 4, clr);
			//row++;
		}
	}	
	Refresh();
}


void CStccStatusGrid::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 8341)
	{
		static int nTimerCount = 0;
		nTimerCount ++;

		int row = 1, col = 3;

		if(nTimerCount % 2 == 0)
			SetItemBkColour(row, col, RGB(0,0,0));
		else
			SetItemBkColour(row, col, RGB(255,255,255));

		RedrawCell(row, col);
	}

	CGridCtrl::OnTimer(nIDEvent);
}


};
