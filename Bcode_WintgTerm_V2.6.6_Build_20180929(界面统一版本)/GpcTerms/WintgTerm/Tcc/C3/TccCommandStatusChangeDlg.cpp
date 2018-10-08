// TccCommandStatusChangeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
//#include "tcc.h"
#include "tcc_dlg.h"
//#include "sysruninfo.h"
#include "TccCommandStatusChangeDlg.h"

namespace TCC_C3
{

// CTccCommandStatusChangeDlg 对话框

IMPLEMENT_DYNAMIC(CTccCommandStatusChangeDlg, CDialog)
CTccCommandStatusChangeDlg::CTccCommandStatusChangeDlg(
		stano_t nTccID,
		STOCK_COMMAND& stockcmd,
		ITccClientInfoProvider *pInfoProvider,
		ITccAdviseSink *pAdviseSink,
		CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
	, m_nTccID(nTccID)
	, m_StockCmd(stockcmd)
	, m_sContent(_T(""))
	, m_sStatus(_T(""))
{
	ASSERT(pInfoProvider && pAdviseSink);
	m_pInfoProvider = pInfoProvider;
	m_pInfoProvider->AddRef();

	m_pAdviseSink = pAdviseSink;
	m_pAdviseSink->AddRef();
}

CTccCommandStatusChangeDlg::~CTccCommandStatusChangeDlg()
{
	if(m_pInfoProvider)
	{
		m_pInfoProvider->Release();
		m_pInfoProvider = NULL;
	}

	if(m_pAdviseSink)
	{
		m_pAdviseSink->Release();
		m_pAdviseSink = NULL;
	}
}

void CTccCommandStatusChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATUS, m_StatusList);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN, m_ExecTime);
}

BEGIN_MESSAGE_MAP(CTccCommandStatusChangeDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_MESSAGE(UM_RECEIVE_TCC_COMMAND_RESULT, OnReceiveTccCommandResult)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CTccCommandStatusChangeDlg 消息处理程序
BOOL CTccCommandStatusChangeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nIndex;
	nIndex = m_StatusList.AddString("确认");
	m_StatusList.SetItemData(nIndex, CONFIRM);
	nIndex = m_StatusList.AddString("执行");
	m_StatusList.SetItemData(nIndex, EXECUTING);
	nIndex = m_StatusList.AddString("取消");
	m_StatusList.SetItemData(nIndex, CANCELED);

	CWnd *pWnd = GetDlgItem(IDC_TCC_CMD_STA);
	ASSERT(pWnd);
	pWnd->SetWindowText(m_pInfoProvider->GetStationName(m_nTccID));

	int status = 0;
	time_t tmExec = 0;
	for(int i=0; i<m_StockCmd.tcc.size(); i++)
	{
		if(m_StockCmd.tcc[i].nID == m_nTccID)
		{
			status  = m_StockCmd.tcc[i].nStatus;
			tmExec = m_StockCmd.tcc[i].nTime;
			break;
		}
	}
	for(i=0; i<m_StatusList.GetCount(); i++)
	{
		if(m_StatusList.GetItemData(i) == status)
		{
			m_StatusList.SetCurSel(i);
			break;
		}
	}

	//执行时间
	m_ExecTime.SetFormat("yyy'-'MM'-'dd HH':'mm");

	CTime ct;
	ct = CTime::GetCurrentTime();

	if(tmExec)
		m_ExecTime.SetTime(tmExec);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CTccCommandStatusChangeDlg::OnBnClickedCancel()
{
	OnCancel();
}


void CTccCommandStatusChangeDlg::OnBnClickedOk()
{
	int nIndex = m_StatusList.GetCurSel();
	if(CB_ERR == nIndex)
		return;

	int status = m_StatusList.GetItemData(nIndex);

	ITccAdviseSink::STOCK_COMMAND_STATUS_CHANGE_INFO info;

	bool bSet = false;
	for(int i=0; i<m_StockCmd.tcc.size(); i++)
	{
		if(m_StockCmd.tcc[i].nID == m_nTccID)
		{
			info.nConsoleID = m_StockCmd.nConsoleID;
			info.sys_id = m_StockCmd.sys_id;
			info.nCmdID = m_StockCmd.nCmdID;
			info.nTccID = m_nTccID;
			info.nCheckorID = m_StockCmd.tcc[i].nCheckorID;
			CTime ct;
			m_ExecTime.GetTime(ct);
			info.nTime = ct.GetTime();// = m_StockCmd.tcc[i].nTime;
			info.nSenderID  = m_StockCmd.tcc[i].nSenderID;

			info.nStatus = status;
			if(info.nStatus != m_StockCmd.tcc[i].nStatus
				|| info.nTime != m_StockCmd.tcc[i].nTime)
			{
				m_pAdviseSink->SetStockCommandStatus(ITccAdviseSink::MAN_SET_STOCKCOMMAND_STATUS, info, this->GetSafeHwnd());
				bSet = true;
			}
			break;
		}
	}

	if(bSet)
		OnOK();
}

LRESULT CTccCommandStatusChangeDlg::OnReceiveTccCommandResult(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

}