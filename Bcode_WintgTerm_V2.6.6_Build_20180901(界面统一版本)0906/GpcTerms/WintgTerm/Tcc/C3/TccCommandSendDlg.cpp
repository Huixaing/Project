// TccCommandSendDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
//#include "tcc.h"
#include "tcc_dlg.h"
//#include "sysruninfo.h"
#include "TccCommandSendDlg.h"

namespace TCC_C3
{

	HWND hWndTccCommandSendDlg = NULL;
	HWND GetTccCommandSendDlgHandle()
	{
		return hWndTccCommandSendDlg;
	}

// CTccCommandSendDlg 对话框

IMPLEMENT_DYNAMIC(CTccCommandSendDlg, CDialog)
CTccCommandSendDlg::CTccCommandSendDlg(
		TCC_COMMAND& cmd, int op_flag,
		STOCK_COMMAND *pStockCmd,
		ITccClientInfoProvider *pInfoProvider,
		ITccAdviseSink *pAdviseSink,
		CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
	, m_pStockCmd(pStockCmd)
	, m_sContent(_T(""))
	, m_sStatus(_T(""))
	, m_bCanExecuteTccCommand(FALSE)
	, m_TccCommand(cmd)
	, m_opflag(op_flag)
{
	ASSERT(pInfoProvider && pAdviseSink);
	m_pInfoProvider = pInfoProvider;
	m_pInfoProvider->AddRef();

	m_pAdviseSink = pAdviseSink;
	m_pAdviseSink->AddRef();

	m_bCanClose = TRUE;
}

CTccCommandSendDlg::~CTccCommandSendDlg()
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

void CTccCommandSendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TCCCMDCONTENT, m_sContent);
	DDX_Text(pDX, IDC_EDIT_TCCCMDSTATUS, m_sStatus);
}

BEGIN_MESSAGE_MAP(CTccCommandSendDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_TCCCMD_TRY_EXECUTE, OnBnClickedBtnTryExecute)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_MESSAGE(UM_RECEIVE_TCC_COMMAND_RESULT, OnReceiveTccCommandResult)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CTccCommandSendDlg 消息处理程序
BOOL CTccCommandSendDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	hWndTccCommandSendDlg = GetSafeHwnd();

	switch(m_opflag)
	{
	case TCC_COMMAND_DIRECT:	//直接限速
	case TCC_COMMAND_CONFIRM:	//确认列表下限速
		SetWindowText("临时限速命令下达");
		m_sContent = m_TccCommand.toString(m_pInfoProvider);
		UpdateData(FALSE);
		break;
	case TCC_COMMAND_CANCEL:	//取消限速
		ASSERT(m_TccCommand.bCancelLimit);
		SetWindowText("临时限速取消命令下达");
		m_sContent = m_TccCommand.toString(m_pInfoProvider);
		UpdateData(FALSE);
		break;
	case TCC_COMMAND_FULLCANCEL:		//强制全线无限速
		ASSERT(m_TccCommand.nCmdID == 0xFFFF);
		SetWindowText("强制全线无限速命令下达");
		m_sContent.Format(
			"命令号: %d"
			"\r\n目标站: %s(%d)"
			"\r\n线路:%s(%d)",
			m_TccCommand.nCmdID,
			m_pInfoProvider->GetStationName(m_TccCommand.nTccID), m_TccCommand.nTccID,
			m_pInfoProvider->GetTccLineName(m_TccCommand.nTccID, m_TccCommand.nTccLineID), m_TccCommand.nTccLineID
			);
		UpdateData(FALSE);
		break;
	case TCC_COMMAND_INITCOMPLETE:	//初始化完成
		ASSERT(m_TccCommand.nCmdID == 10000);
		SetWindowText("初始化完成命令下达");
		m_sContent.Format(
			"命令号: %d"
			"\r\n目标站: %s(%d)",
			m_TccCommand.nCmdID,
			m_pInfoProvider->GetStationName(m_TccCommand.nTccID), m_TccCommand.nTccID
			);
		//m_sContent = m_TccCommand.toString(m_pInfoProvider);
		UpdateData(FALSE);
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CTccCommandSendDlg::OnBnClickedCancel()
{
	/*if(!m_bCanClose)
	{
		if(IDOK != MessageBox(
			"列控执行结果未反馈，强行关闭窗口后，服务器状态可能会不正确!",
			"命令下达",
			MB_OKCANCEL)
			)
		{
			return ;
		}
	}*/

	OnCancel();
}


void CTccCommandSendDlg::OnBnClickedBtnTryExecute()
{
	if(TCC_COMMAND_INITCOMPLETE == m_opflag)  // TCC_COMMAND_FULLCANCEL == m_opflag //也必须校验
	{
		MessageBox("特殊命令不需要校验, 请直接发送");
		return ;
	}
	if(m_bCanExecuteTccCommand)
	{
		MessageBox("已经校验成功，不需要继续校验；如果超时，请关闭本窗口后重新发送命令。");
		return ;
	}

	TCC_COMMAND tcccmd = m_TccCommand;

	m_sStatus = "准备发送 临时限速+校验";

	if(m_pAdviseSink->OnSendTccCommand(tcccmd, 1, m_hWnd) != 0)
	{
		m_sStatus = "发送 临时限速+校验 失败";
		MessageBox("发送 临时限速校验 失败");
	}
	else
	{
		m_sStatus = "已经发送 临时限速+校验";

		//校验后必须等待回执
		m_bCanClose = FALSE;
	}
	UpdateData(FALSE);
}

void CTccCommandSendDlg::OnBnClickedOk()
{
	int op_flag = 2;
	switch(m_opflag)
	{
	case TCC_COMMAND_DIRECT:	//直接限速
	case TCC_COMMAND_CONFIRM:	//确认列表下限速
	case TCC_COMMAND_CANCEL:	//取消限速
	default:
		op_flag = 2;
		break;
	case TCC_COMMAND_INITCOMPLETE:	//初始化完成
		m_bCanExecuteTccCommand = TRUE;
		op_flag = 3;
		break;
	case TCC_COMMAND_FULLCANCEL:		//强制全线无限速
		// m_bCanExecuteTccCommand = TRUE; // 也必须校验
		op_flag = 2;
		break;
	}
	if(!m_bCanExecuteTccCommand)
	{
		MessageBox("列控校验成功后才可以发送!");
		return ;
	}

	m_sStatus = "准备发送 临时限速+执行";

	if(m_pAdviseSink->OnSendTccCommand(m_TccCommand, op_flag, m_hWnd) != 0)
	{
		m_sStatus = "发送 临时限速+执行 失败";
		MessageBox("发送 临时限速执行失败");
	}
	else
	{
		m_sStatus = "已经发送 临时限速+执行";

		//执行后必须等待回执
		m_bCanClose = FALSE;
	}
	UpdateData(FALSE);
//	OnOK();
}

LRESULT CTccCommandSendDlg::OnReceiveTccCommandResult(WPARAM wParam, LPARAM lParam)
{
	TCC_COMMAND_RESULT& cmd_result = *(TCC_COMMAND_RESULT *)lParam;
	
	if(m_TccCommand.nCmdID != cmd_result.cmd.nCmdID)
	{
		m_sStatus.Format("命令号不相等(发%d, 收%d)", m_TccCommand.nCmdID, cmd_result.cmd.nCmdID);
	}
	
	else if(m_TccCommand.nTccID != cmd_result.cmd.nTccID)
	{
		m_sStatus.Format("站号不相等(发%d, 收%d)", m_TccCommand.nTccID, cmd_result.cmd.nTccID);
	}
	
	else if (m_TccCommand.nKmMark[0].info.val != cmd_result.cmd.nKmMark[0].info.val) //(m_TccCommand.nKmMark[0].Unequal(cmd_result.cmd.nKmMark[0]))
	{
		m_sStatus.Format("开始公里标不相等(发%s, 收%s)", 
			m_TccCommand.nKmMark[0].toString(m_pInfoProvider, m_TccCommand.nLineID), 
			cmd_result.cmd.nKmMark[0].toString(m_pInfoProvider, m_TccCommand.nLineID));
	}

	else if (m_TccCommand.nKmMark[1].info.val != cmd_result.cmd.nKmMark[1].info.val)//(m_TccCommand.nKmMark[1].Unequal(cmd_result.cmd.nKmMark[1]))
	{
		m_sStatus.Format("结束公里标不相等(发%s, 收%s)", 
			m_TccCommand.nKmMark[1].toString(m_pInfoProvider, m_TccCommand.nLineID), 
			cmd_result.cmd.nKmMark[1].toString(m_pInfoProvider, m_TccCommand.nLineID));
	}

	else if (m_TccCommand.nSpeedMaxVal != cmd_result.cmd.nSpeedMaxVal)
	{
		m_sStatus.Format("速度不相等(发%d, 收%d)", m_TccCommand.nSpeedMaxVal, cmd_result.cmd.nSpeedMaxVal);
	}

	else if (m_TccCommand.nTccLineID != cmd_result.cmd.nTccLineID)
	{
		m_sStatus.Format("线路号不相等(发%d, 收%d)", m_TccCommand.nTccLineID, cmd_result.cmd.nTccLineID);
	}

	else //if(m_TccCommand == cmd_result.cmd)
	{
		m_sStatus.Format("%s", cmd_result.strComment);

		// 命令执行结果
		//     1: 自律机收到
		//     2:列控校验成功
		//     3:列控校验失败
		//     4:列控执行成功
		//     5:列控执行失败
		//     6:列控系统异常
		//     7:自律机检查失败 

		//校验是否成功
		switch(cmd_result.result)
		{
		case 2:
			m_bCanExecuteTccCommand = TRUE;
			break;
		default:
			m_bCanExecuteTccCommand = FALSE;
			break;
		}

		//是否可以关闭窗口
		switch(cmd_result.result)
		{
		case 2:
		case 3:
		case 4:
		case 5:
			m_bCanClose = TRUE;
			break;
		default:
			m_bCanClose = FALSE;
			break;
		}

		//设置服务器的确认命令状态
		if(m_pStockCmd && 4 == cmd_result.result)
		{
			ITccAdviseSink::STOCK_COMMAND_STATUS_CHANGE_INFO info;
			info.nConsoleID = m_TccCommand.nConsoleID;
			info.sys_id = m_TccCommand.sys_id;
			info.nCmdID = m_TccCommand.nCmdID;
			info.nTccID = m_TccCommand.nTccID;
			info.nStatus = EXECUTING;
			time_t tm;
			time(&tm);
			info.nTime = tm;

			info.nCheckorID = 0;
			for(int i=0; i<m_pStockCmd->tcc.size(); i++)
			{
				if(m_pStockCmd->tcc[i].nID == info.nTccID)
				{
					info.nCheckorID = m_pStockCmd->tcc[i].nCheckorID;
					break;
				}
			}
			info.nSenderID  = m_TccCommand.nTccSenderID;

			//直接限速成功后首先向服务器增加一条确认命令
			if(m_opflag == TCC_COMMAND_DIRECT)
				m_pAdviseSink->OnSendStockCommand(*m_pStockCmd, GetSafeHwnd());

			//执行成功后,修改服务器相应命令的状态
			m_pAdviseSink->SetStockCommandStatus(ITccAdviseSink::LIRC_SET_STOCKCOMMAND_STATUS, info, GetSafeHwnd());
		}
	}

	UpdateData(FALSE);
	return TRUE;
}

void CTccCommandSendDlg::OnDestroy()
{
	CDialog::OnDestroy();
	hWndTccCommandSendDlg = NULL;
}

}