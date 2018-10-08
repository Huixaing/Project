/*
 * 本地限速命令管理
 */

#include "stdafx.h"
#include "resource.h"
#include "Tcc_Dlg.h"

namespace TCC {



BOOL CLocalCommandPage::Load(LPCSTR szFilename)
{
	do {
		MSXML2::IXMLDOMDocumentPtr pXMLDOMDoc;
		HRESULT hr;
		hr = pXMLDOMDoc.CreateInstance(
			__uuidof(MSXML2::DOMDocument40), 
			NULL, 
			CLSCTX_INPROC_SERVER);

		pXMLDOMDoc->put_async(VARIANT_FALSE);
		pXMLDOMDoc->put_validateOnParse(VARIANT_TRUE);
		pXMLDOMDoc->put_resolveExternals(VARIANT_TRUE);

		_variant_t vXml(szFilename);
		VARIANT_BOOL vBool = pXMLDOMDoc->load(vXml);
		SysFreeString(vXml.bstrVal);
		if(VARIANT_FALSE == vBool)
			return FALSE;

		MSXML2::IXMLDOMParseErrorPtr pError = pXMLDOMDoc->GetparseError();
		if(S_OK != pError->GeterrorCode())
		{
			CString str; 
			str.Format("\nLine: %d", pError->Getline());

			_bstr_t strRes = _bstr_t("Validation failed on ") + szFilename
				+ _bstr_t("\n=============")
				+ _bstr_t("\nReason: ") + pError->Getreason()
				+ _bstr_t("\nText: ") + pError->GetsrcText()
				+ _bstr_t(str);

			MessageBox(strRes);
			break;
		}

		// 取数据
		MSXML2::IXMLDOMNodeListPtr pNodeList = pXMLDOMDoc->selectNodes("RAW_STOCK_COMMAND/COMMAND");

		TCC_CMD_LIST& cmdlist = GetTccCmdList();
		long length = pNodeList->Getlength();
		for(int i=0; i<length; i++)
		{
			RAW_STOCK_COMMAND cmd;
			LoadRawStockCommand( pNodeList->Getitem( i ), cmd);
			cmdlist.push_back(cmd);
		}

		return TRUE;
	} while(0);

	return FALSE;
}



void CLocalCommandPage::InitListCtrl(CGridCtrl& grid)
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
		{"开始时间", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"结束时间", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"状态", 100, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
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
	
	grid.SetSingleRowSelection();
}

// 
void CLocalCommandPage::AppendCommand(RAW_STOCK_COMMAND& cmd)
{
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

	CGridCtrl& grid = m_CmdList;

	GV_ITEM item;
	item.row = item.col = 0;

	item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
	item.nFormat = DT_CENTER|DT_WORDBREAK|DT_NOPREFIX;

	CString str;
	str.Format("%4d", cmd.nCmdID);

	int nPos = 0;
	for(int i=1; i<grid.GetRowCount(); i++)
	{
		RAW_STOCK_COMMAND *pCmd = (RAW_STOCK_COMMAND *)grid.GetItemData(i, 0);
		if(pCmd->nCmdID == cmd.nCmdID)
			nPos = i;
		if(pCmd == &cmd)
			break;
	}
	if(i == grid.GetRowCount())
	{
		if(0 == nPos || i == nPos+1)
			item.row = grid.InsertRow(str, -1);
		else
			item.row = grid.InsertRow(str, nPos + 1);
		grid.SetItemFormat(item.row, item.col, grid.GetItemFormat(0, item.col));
	}
	else
	{
		item.row = i;
		grid.SetItemText(item.row, 0, str);
	}
	grid.EnsureVisible(item.row, 0);
	if(m_pCurCmd)
		grid.SetFocusCell(item.row, 0);
	else
		grid.SetFocusCell(-1, -1);
	grid.SetItemData(item.row, 0, (DWORD)&cmd);
	if(cmd.nCmdID == 0)
		grid.SetItemFgColour(item.row, item.col);
	else
	{
		/*// 0:accept, 1:error, 2:send
		INT nStatus[2];
		nStatus[0] = nStatus[1]  = 2;
		for(int i=0; i<2; i++)
		{
			if(0 == cmd.nTccID[i]
				|| !pInfoProvider->ValidateStationID(cmd.nTccID[i]))
			{
				nStatus[i] = 0;
				continue;
			}

			switch(cmd.nTccStatus[i])
			{
			case TCC_ACCEPT:
			case TCC_RECEIVED:
			case TCC_WAITFOR_EXEC:
			case TCC_EXECUTING:
			case TCC_ZOMBIE:
			case TCC_CANCELED:
				nStatus[i] = 0;
				break;

			case TCC_UNKNOWN:
			case TCC_ERR_KM:
			case TCC_ERR_SPEED:
			case TCC_ERR_REGION_LONG:
			case TCC_ERR_REGION_EXIST:
			case TCC_ERR_REGION_INVALID:
			case TCC_ERR_DUP_CMDNO:
			case TCC_ERR_TIME_FAIL:
				nStatus[i] = 1;
				break;

			case LIRC_RECEIVED:
			case LOC_WAITFOR_CANCEL:
			case LOC_SENDED:
			case LOC_SEND_FAIL:
				nStatus[i] = 2;
				break;
			default:
				break;
			}
		}
		if(1 == nStatus[0] || 1 == nStatus[1])// error
			grid.SetItemFgColour(item.row, item.col, RGB(255, 0, 0));
		else if(0 == nStatus[0] && 0 == nStatus[1])// 2 accept
			grid.SetItemFgColour(item.row, item.col, RGB(0, 255, 0));
		else if(0 == nStatus[0] || 0 == nStatus[1])// 1 accept
			grid.SetItemFgColour(item.row, item.col, RGB(0, 125, 0));
		else
			grid.SetItemFgColour(item.row, item.col, RGB(125, 125, 125));
		*/
	}

	// 开始站
	item.col ++;
	item.strText = pInfoProvider->GetStationName(cmd.nStaID[0]);
	item.lParam = cmd.nStaID[0];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 结束站
	item.col ++;
	item.strText = pInfoProvider->GetStationName(cmd.nStaID[1]);
	item.lParam = cmd.nStaID[1];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	item.mask |= GVIF_STATE;
	item.nState = GVIS_READONLY;

	// 上下行
	item.col ++;
	item.strText = pInfoProvider->GetTccLineName(cmd.nStaID[0], cmd.nLineID);
	item.lParam = cmd.nLineID;
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 开始公里标
	item.col ++;
	if(cmd.bFullLimit)
		str.Format("全限速");
	else
		str = cmd.nKmMark[0].toString();
	item.strText = str;
	item.lParam = cmd.nKmMark[0].GetDWORD();
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 结束公里标
	item.col ++;
	if(cmd.bFullLimit)
		str.Format("全限速");
	else
		str = cmd.nKmMark[1].toString();
	item.strText = str;
	item.lParam = cmd.nKmMark[1].GetDWORD();
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	//// 限速值
	//item.col ++;
	//if(cmd.bCancelLimit)
	//{
	//	grid.SetItemFgColour(item.row, item.col, RGB(255, 0, 0) );
	//	str.Format("取消限速");
	//}
	//else
	//{
	//	grid.SetItemFgColour(item.row, item.col);
	//	str.Format("%3dKm/h", cmd.nSpeedMaxVal);
	//}
	//item.strText = str;
	//item.lParam = cmd.nSpeedMaxVal;
	//item.nFormat = grid.GetItemFormat(0, item.col);
	//grid.SetItem(&item);

	// 限速开始时间
	item.col ++;
	CTime ct;
	if(cmd.bStartNow || cmd.nTime[0] == 0)
	{
		str = "立即开始";	// 立即开始，取发令时间
	}
	else
	{
		ct = cmd.nTime[0];
		str = ct.Format("%m/%d %H:%M");
	}
	item.strText = str;
	item.lParam = cmd.nTime[0];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 限速结束时间
	item.col ++;
	if(cmd.bEndless || cmd.nTime[1] == 0)
		str.Format("永久有效");	//ct = cmd.tmSending;	// 永久有效，取发令时间
	else
	{
		ct = cmd.nTime[1];
		str= ct.Format("%m/%d %H:%M");
	}
	item.strText = str;
	item.lParam = cmd.nTime[1];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);

	// 状态
	str.Empty();
	/*if(cmd.nTccStatus[0] == cmd.nTccStatus[1])
	{
		str = GetTccCmdStatusString(cmd.nTccStatus[0]);
	}
	else
	{
		for(int i=0; i<2; i++)
		{
			if(0 == cmd.nTccID[i] 
				|| !pInfoProvider->ValidateStationID(cmd.nTccID[i]))
				continue;
			
			CString strStatus = GetTccCmdStatusString(cmd.nTccStatus[i]);
			if(!strStatus.IsEmpty())
			{
				str += pInfoProvider->GetStationName(cmd.nTccID[i]);
				str += strStatus;
				str += ", ";
			}
		}
		str.TrimRight(", ");
	}

	item.col ++;
	item.strText = str;
	item.lParam = (cmd.nTccStatus[1] << 16) + cmd.nTccStatus[0];
	item.nFormat = grid.GetItemFormat(0, item.col);
	grid.SetItem(&item);*/

	grid.RedrawRow(i);
}

// CLocalCommandPage 对话框

IMPLEMENT_DYNAMIC(CLocalCommandPage, CPropertyPage)
CLocalCommandPage::CLocalCommandPage()
	: CPropertyPage(CLocalCommandPage::IDD)
	, m_pCurCmd(NULL)
{
}

CLocalCommandPage::~CLocalCommandPage()
{
}


// 收到下发限速命令的状态
void CLocalCommandPage::Tcc_SetCmdStatus(DWORD nSysID, DWORD nCmdID, DWORD nTccID, DWORD nStatus)
{
	/*TCC_CMD_LIST& listCmd = GetTccCmdList();
	TCC_CMD_LIST::iterator iter = listCmd.begin();
	for(; iter != listCmd.end(); iter++)
	{
		STOCK_COMMAND &cmd = *iter;
		if(cmd.nSysID!=nSysID || cmd.nCmdID != nCmdID)
			continue;
		
		if(cmd.nTccID[0] == nTccID)
			cmd.nTccStatus[0] = nStatus;
		else if(cmd.nTccID[1] == nTccID)
			cmd.nTccStatus[1] = nStatus;

		DWORD status = 0;
		if(TCC_ACCEPT == cmd.nTccStatus[0]
			|| TCC_ACCEPT == cmd.nTccStatus[1])
			status = 2;
		else if(TCC_CONFIRM_OK == cmd.nTccStatus[0]
			|| TCC_CONFIRM_OK == cmd.nTccStatus[1])
			status = 1;
		if(255 == cmd.nSpeedMaxVal && 0 < status)
		{
			status = 3;
		}

		if(0 < status)
		GetTccAdviseSink()->OnTccCommandStatusChanged(cmd, status, iter->dwParam);

		// 列表中显示
		AddCmdToListCtrl(cmd);

		// 详细内容显示
		if(m_pCurCmd == &cmd)
			ShowCmd(m_pCurCmd);

		// 保存到历史文件
		SaveCommandInHistory(cmd, GetTccClientInfoProvider());

		// 保存到当前文件
		SaveCommand(cmd, GetTccClientInfoProvider());

		break;
	}*/
}

CLocalCommandPage::TCC_CMD_LIST& CLocalCommandPage::GetTccCmdList()
{
	return listTcc;
}

CTccDlg* CLocalCommandPage::GetTccDlg()
{
	CWnd *pWnd = GetParent();
	ASSERT(pWnd);
	pWnd = pWnd->GetParent();
	ASSERT(pWnd);

	return (CTccDlg*)pWnd;
}

ITccAdviseSink* CLocalCommandPage::GetTccAdviseSink()
{
	return GetTccDlg()->GetTccAdviseSink();
}

ITccClientInfoProvider* CLocalCommandPage::GetTccClientInfoProvider()
{
	return GetTccDlg()->GetTccClientInfoProvider();
}

RAW_STOCK_COMMAND* CLocalCommandPage::GetRawCommand(sysid_t& raw_sysid)
{
	for(int row=m_CmdList.GetFixedRowCount(); row<m_CmdList.GetRowCount();  row++)
	{
		RAW_STOCK_COMMAND *pcmd = (RAW_STOCK_COMMAND*)m_CmdList.GetItemData(row, 0);
		if(pcmd && pcmd->raw_sysid == raw_sysid)
			return pcmd;
	}

	return NULL;
}

void CLocalCommandPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_TCC_CMD_NOTE, m_CmdString);
}

void CLocalCommandPage::ShowCmd(RAW_STOCK_COMMAND *pcmd)
{
	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

	if(pcmd)
		m_CmdString = pcmd->toString(pInfoProvider);
	else
		m_CmdString.Empty();
	
	UpdateData(FALSE);
}

	struct greater{
		bool operator()(RAW_STOCK_COMMAND& lhs, RAW_STOCK_COMMAND& rhs)
		{
			return lhs.nCmdID < rhs.nCmdID;
		}
	};
void CLocalCommandPage::OnBtnClickedRefresh()
{
	m_CmdList.DeleteNonFixedRows();
	GetTccCmdList().clear();
	m_CmdList.Refresh();

	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();
	CString str;

	// 删除比较老的空文件
	EraseZeroFileInDir( pInfoProvider->GetTccDataDirectory() );

	char buf[1024];
	memset(buf, 0, sizeof(buf));
	GetCurrentDirectory(1024, buf);
	CString path;
	path.Format("%s", pInfoProvider->GetTccDataDirectory());
	SetCurrentDirectory(path);
	CString strFilter;
	strFilter.Format("*.%s", GetCommandExtFileName());
	CFileFind fFind;
	BOOL bFind = fFind.FindFile(strFilter);
	SetCurrentDirectory(buf);
	while(bFind)
	{
		bFind = fFind.FindNextFile();
		CString sz = fFind.GetFileName();
		Load(path + "\\" + sz);
	}
	fFind.Close();
	GetTccCmdList().sort(greater());

	TCC_CMD_LIST& cmdlist = GetTccCmdList();
	TCC_CMD_LIST::iterator iter = cmdlist.begin();
	for(; iter != cmdlist.end(); iter ++)
	{
		stano_t nStaID = (*iter).nStaID[0];
		stano_t nStaID2= (*iter).nStaID[1];
		if((0==nStaID || pInfoProvider->ValidateStationID(nStaID))
			&& (0==nStaID2 || pInfoProvider->ValidateStationID(nStaID2)))
		{
			AppendCommand(*iter);
		}
	}

	m_pCurCmd = NULL;
	ShowCmd(m_pCurCmd);
}

#define GRIDCTRL 101
BOOL CLocalCommandPage::OnInitDialog()
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

	OnBtnClickedRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BEGIN_MESSAGE_MAP(CLocalCommandPage, CPropertyPage)
	ON_BN_CLICKED(IDC_TCC_DELCMD_BTN, OnBnClickedTccDelcmdBtn)
	ON_NOTIFY(NM_CLICK, GRIDCTRL, OnNMClickTccCmdList)
	ON_NOTIFY(NM_RCLICK, GRIDCTRL, OnNMClickTccCmdList)
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_COMMAND(ID_DEL_LIMITSPEED, OnBnClickedTccDelcmdBtn)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_TCC_REFRESH, OnBtnClickedRefresh)
END_MESSAGE_MAP()

// CLocalCommandPage 消息处理程序
//

void CLocalCommandPage::OnNMClickTccCmdList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW *)pNMHDR;
	int nItem = pItem->iRow;
	if(nItem < 1)
		return ;

	// 设置当前选择的命令
	RAW_STOCK_COMMAND *pCmd = (RAW_STOCK_COMMAND *)m_CmdList.GetItemData(nItem, 0);
	if(m_pCurCmd != pCmd)
	{
		m_pCurCmd = pCmd;
		ShowCmd(m_pCurCmd);
	}

	*pResult = 0;
}

// 删除 限速命令(从本地删除)
void CLocalCommandPage::OnBnClickedTccDelcmdBtn()
{
	if(NULL == m_pCurCmd)
		return ;

	//BOOL bEnabled = TRUE;
	//for(int i=0; i<2 && bEnabled; i++)
	//{
	//	bEnabled = !CanDeleteLimitSpeedCommand(m_pCurCmd->nTccStatus[i]);
	//}
	//if(bEnabled)
	//{
	//	MessageBox("不能删除!");
	//	return ;
	//}

	ITccClientInfoProvider *pInfoProvider = GetTccClientInfoProvider();

	CString szFilename;
	szFilename.Format("%s\\%s.%s",
		pInfoProvider->GetTccDataDirectory(),
		GetCommandExtraFileName(m_pCurCmd->raw_sysid, m_pCurCmd->nCmdID),
		GetCommandExtFileName());
	DeleteCommand(*m_pCurCmd, szFilename);

	for(int i=0; i<m_CmdList.GetRowCount(); i++)
	{
		if(m_CmdList.GetItemData(i, 0) == (DWORD)m_pCurCmd)
		{
			m_CmdList.DeleteRow(i);
			m_CmdList.Refresh();
			break;
		}
	}

	TCC_CMD_LIST& listCmd = GetTccCmdList();
	TCC_CMD_LIST::iterator iter = listCmd.begin();
	for(; iter != listCmd.end(); iter++)
	{
		if(&*iter == m_pCurCmd)
		{
			listCmd.erase(iter);
			break;
		}
	}

	m_pCurCmd = NULL;
	ShowCmd(m_pCurCmd);
}

void CLocalCommandPage::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CCellID cellFocus = m_CmdList.GetFocusCell();
	if(cellFocus.row <1 || cellFocus.row >= m_CmdList.GetRowCount())
		return;
	INT nIndex = cellFocus.row;
	if(NULL==m_pCurCmd)
		return;

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(
		MF_BYPOSITION|MF_STRING, 
		ID_DEL_LIMITSPEED, 
		"删除");

	menu.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		point.x, point.y, 
		this);
}

void CLocalCommandPage::OnDestroy()
{
	CPropertyPage::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}



};
