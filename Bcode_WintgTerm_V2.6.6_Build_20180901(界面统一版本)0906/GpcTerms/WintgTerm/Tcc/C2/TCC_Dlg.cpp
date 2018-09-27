// TCC.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "TCC_Dlg.h"
#include <algorithm>

namespace TCC {

static HWND hWndTccUI = NULL;
HWND GetTccDialogWndHandle()
{
	return hWndTccUI;
}

void RegisterTCCMsgProcHandleWnd()
{
	if(hWndTccUI==NULL)
		return;

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandList,0);//316
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandList, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccReport,0);// 306
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccReport, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandSendResult,0); //314
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandSendResult, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommand,0);// 307
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommand, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommandData2,0);//361
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommandData2, hWndTccUI);

	RegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandIDRequest,0);//413
	GpcLogTrainMessage(0,0, "[TCC]: RegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandIDRequest, hWndTccUI);
}
	
void UnRegisterTCCMsgProcHandleWnd()
{
	if(hWndTccUI==NULL)
		return;

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandList);//316
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandList, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccReport);// 306
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccReport, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandSendResult); //314
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandSendResult, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommand);// 307
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommand, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgTccCommandData2);//361
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgTccCommandData2, hWndTccUI);

	UnRegisterMsgProcHandleWnd(hWndTccUI,MSGID_MsgStccCommandIDRequest);//413
	GpcLogTrainMessage(0,0, "[TCC]: UnRegisterMsg(msg_id=%d, hwnd=%d)", MSGID_MsgStccCommandIDRequest, hWndTccUI);
}

IMPLEMENT_DYNAMIC(CTccDlg, CDialog);

CTccDlg::CTccDlg() : m_bAutoDelete(FALSE)
{
	m_nCurrentTccID = 0;
}

CTccDlg::CTccDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: m_bAutoDelete(FALSE)
{
	m_nCurrentTccID = 0;
}

CTccDlg::CTccDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: m_bAutoDelete(FALSE)
{
	m_nCurrentTccID = 0;
}

CTccDlg::~CTccDlg()
{
	m_pTccClientInfoProvider->Release();
	m_pTccAdviseSink->Release();
}

void CTccDlg::SetInterface(ITccAdviseSink *pTccAdviseSink, ITccClientInfoProvider *pInfoProvider)
{
	pTccAdviseSink->AddRef();
	this->m_pTccAdviseSink = pTccAdviseSink;

	pInfoProvider->AddRef();
	m_pTccClientInfoProvider = pInfoProvider;
}

STOCK_COMMAND* CTccDlg::GetConfirmCommand(DWORD nConsoleID, sysid_t& sysid, WORD nCmdID)
{
	return m_ConfirmCommandPage.GetCommand(nConsoleID, sysid, nCmdID);
}

STOCK_COMMAND* CTccDlg::GetLocalCommand(DWORD nConsoleID, sysid_t& sysid, WORD nCmdID)
{
	return m_ConfirmCommandPage.GetCommand(nConsoleID, sysid, nCmdID);
}

BOOL CTccDlg::Create(CWnd* pParentWnd, DWORD dwStyle, DWORD dwExStyle)
{
	if(hWndTccUI)
	{
		delete this;
		return FALSE;
	}

	m_bAutoDelete = TRUE;

	BOOL b = CDialog::Create(IDD, pParentWnd);
	if(!b)
		delete this;

	return b;
}

void ResetTccList(CComboBox& TccList, ITccClientInfoProvider *pInfoProvider)
{
	TccList.ResetContent();

	// 列控中心列表
	std::vector<stano_t> staList;
	pInfoProvider->GetStationList(staList);
	for(std::vector<stano_t>::iterator iter = staList.begin(); iter != staList.end(); iter++)
	{
		INT nIndex = TccList.AddString(
			pInfoProvider->GetStationName(*iter));
		ASSERT(CB_ERR != nIndex);
		TccList.SetItemData(nIndex, *iter);
	}

	TccList.SetCurSel(-1);
}

BOOL CTccDlg::OnInitDialog()
{
	BOOL bResult = CDialog::OnInitDialog();

	if(!TccInitialize(m_pTccClientInfoProvider))
		return FALSE;
	//ModifyStyle(WS_SYSMENU, 0, NULL);

	// TODO:  在此添加您的专用代码
	if(NULL == m_pTccClientInfoProvider)
	{
		MessageBox("没有安装TccClientInfoProvider接口!");
		return FALSE;
	}
	if(NULL == m_pTccAdviseSink)
	{
		MessageBox("没有安装TccAdviseSink接口!");
		return FALSE;
	}

	ITccClientInfoProvider *pInfoProvider = m_pTccClientInfoProvider;

	// 列控中心列表
	ResetTccList(m_TccList, pInfoProvider);
	
	m_sheet.AddPage("列控实时命令", &m_LircCommandPage, CLircCommandPage::IDD);
	m_sheet.AddPage("中心命令", &m_ConfirmCommandPage, CConfirmCommandPage::IDD);
	//m_sheet.AddPage("原始命令", &m_LocalCommandPage, CLocalCommandPage::IDD);
	m_sheet.AddPage("历史命令", &m_HistoryCommandPage, CHistoryCommandPage::IDD);
	m_sheet.Show();

	CenterWindow();
    
	RegisterTCCMsgProcHandleWnd();
	return bResult;
}

BEGIN_MESSAGE_MAP(CTccDlg, CDialog)
	ON_MESSAGE(UM_RECEIVE_TCC_COMMAND, OnReceiveTccCommand)
	ON_MESSAGE(UM_RECEIVE_TCC_COMMAND_RESULT, OnReceiveTccCommandResult)
	ON_MESSAGE(UM_RECEIVE_TCCLINE_INITIAL_COMMAND_RESULT, OnReceiveTccLineInitialCommandResult)
	ON_MESSAGE(UM_RECEIVE_TCC_REPORT, OnReceiveTccReport)
	ON_MESSAGE(UM_RECEIVE_CONFIRM_COMMAND, OnReceiveConfirmCommand)
	ON_MESSAGE(UM_CLIENTENV_CHANGED, OnClientEnvChanged)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	ON_CBN_SELCHANGE(IDC_TCC_LIST, OnSelectTcc)
	ON_BN_CLICKED(IDC_TCC_REFRESH, OnRefreshTcc)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
END_MESSAGE_MAP()

struct _TCC_COMMAND_RESULT {
	TCC_COMMAND cmd;
	DWORD nStatus;
	CString strComment;
};

struct _TCCLINE_INITIAL_COMMAND_RESULT {
	stano_t nTccID;
	lineid_t nTccLineID;
	dir_t dir;
	DWORD nStatus;
	CString strComment;
};
LRESULT CTccDlg::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	CLBTH::BaseMsg  *pBase =  ( CLBTH::BaseMsg *)pMsgStruct->pmsg;

	int id = pBase->getTypeIDStatic();
	HWND hWnd = GetTccDialogWndHandle();
	if(::IsWindow(hWnd))
	{
		// 收到自律机发来的列控实时限速命令
		if(MSGID_MsgStccCommandList==pMsgStruct->msg_id)
		{
			CLBTH::MsgStccCommandList  *pMsg = (CLBTH::MsgStccCommandList  *)pBase;

			TCC::TCC_COMMAND cmd;
			cmd.nConsoleID = pMsg->console_no;		// 调度台 台号
			TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);		// 系统标识
			cmd.nCmdID = pMsg->cmd_no;			// 命令号（0~9999）
			cmd.nStaID[0] = pMsg->sta_no[0];		// 开始、结束站号
			cmd.nStaID[1] = pMsg->sta_no[1];
			cmd.nLineID = pMsg->line_no;		// 线路号
			cmd.bFullLimit = pMsg->bFullLimit;		// 是否全线限速
			cmd.nKmMark[0] = pMsg->km_marker[0];	// 开始、结束公里标（以m为单位）
			cmd.nKmMark[1] = pMsg->km_marker[1];
			cmd.bStartNow = pMsg->bStartNow;		// 是否立即开始
			cmd.bEndless = pMsg->bEndless;		// 是否永久有效
			cmd.nTime[0] = pMsg->time[0];		// 开始、结束时间
			cmd.nTime[1] = pMsg->time[1];
			cmd.nSpeedMaxVal = pMsg->speed;			// 限速值

			cmd.nTccID = pMsg->tcc_no;			// 受令站（列控）
			cmd.nTccLineID = pMsg->tcc_line_no;	// 受令站线路
			cmd.nTccSenderID = pMsg->tcc_sender;	// 受令站发令人

			::SendMessage(hWnd, UM_RECEIVE_TCC_COMMAND, 0, (LPARAM)&cmd);
		}
		// 列控状态
		else if (MSGID_MsgStccReport==pMsgStruct->msg_id)
		{
			CLBTH::MsgStccReport  *pMsg = (CLBTH::MsgStccReport  *)pBase;

			TCC::TCC_STATUS status;
			int nTccID = pMsg->sta_no;	// 站号
			status.comm_active = pMsg->comm_active;	// 列控通信状态	(0=断;1=单机;2=双机)
			for(int i=0; i<2; i++)
				status.stcc_master[i] = pMsg->stcc_master[i];	// 列控主备状态
			for(i=0; i<2; i++)
				status.ilock_channel[i] = pMsg->ilock_channel[i];	// 联锁通道状态
			for(i=0; i<2; i++)
				status.ctc_channel[i] = pMsg->ctc_channel[i];	// CTC通道状态
			for(i=0; i<4; i++)
				status.leu_channel[i]= pMsg->leu_channel[i];	// LEU通道状态


			for(i=0; i<4; i++)
				status.leu_ack[i]  = pMsg->leu1_ans[i];	// LEU1应答器状态
			for(i=0; i<4; i++)
				status.leu_ack[4+i]= pMsg->leu2_ans[i];	// LEU2应答器状态
			for(i=0; i<4; i++)
				status.leu_ack[8+i]= pMsg->leu3_ans[i];	// LEU3应答器状态
			for(i=0; i<4; i++)
				status.leu_ack[12+i]= pMsg->leu4_ans[i];	// LEU4应答器状态

			status.init_section_num = pMsg->init_section_num;
			for(i=0; i<status.init_section_num; i++)
			{
				status.init_line_no[i] = pMsg->init_line_no[i];

				status.init_direction[i] = TCC::DIR_UNKNOWN;
				switch(pMsg->init_direction[i])
				{
				case 0:
					status.init_direction[i] = TCC::DIR_UP;
					break;
				case 1:
					status.init_direction[i] = TCC::DIR_DOWN;
					break;
				default:
					break;
				}
			}

			::SendMessage(hWnd, UM_RECEIVE_TCC_REPORT, nTccID, (LPARAM)&status);
		}
		else if (MSGID_MsgStccCommandSendResult==pMsgStruct->msg_id)
		{
			CLBTH::MsgStccCommandSendResult  *pMsg = (CLBTH::MsgStccCommandSendResult  *)pBase;
			if(pMsg->bInitOpration)
			{
				TCC::dir_t dir = TCC::DIR_UNKNOWN;
				switch(pMsg->section_dir)
				{
				case 0:
					dir = TCC::DIR_UP;
					break;
				case 1:
					dir = TCC::DIR_DOWN;
					break;
				case 2:
					dir = TCC::DIR_DUAL;
					break;
				default:
					break;
				}

				char buf[1024];
				memset(buf, 0x0, sizeof(buf));
				pMsg->GetComment(buf, 1024);
				//AfxMessageBox(buf);

				//TCC::Receive_TccLineInitialCommandResult(pMsg->tcc_no, pMsg->tcc_line_no, dir, pMsg->result, buf);
				_TCCLINE_INITIAL_COMMAND_RESULT data;
				data.nTccID = pMsg->tcc_no;
				data.nTccLineID = pMsg->tcc_line_no;
				data.dir = dir;
				data.nStatus = pMsg->result;
				data.strComment = buf;
				::SendMessage(hWnd, UM_RECEIVE_TCCLINE_INITIAL_COMMAND_RESULT, 0, (LPARAM)&data);
			}
			else
			{
				TCC::TCC_COMMAND cmd;
				cmd.nConsoleID = pMsg->console_no;
				TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, sizeof(pMsg->sys_id));
				cmd.nCmdID = pMsg->cmd_no;		// 命令号(0~9999)
				cmd.nStaID[0] = pMsg->sta_no[0];		// 开始站
				cmd.nStaID[1] = pMsg->sta_no[1];		// 结束站
				cmd.nLineID = pMsg->line_no;		// 线路号
				cmd.bFullLimit = pMsg->bFullLimit;	// 是否全线限速
				cmd.nKmMark[0] = pMsg->km_marker[0];	// 开始公理标
				cmd.nKmMark[1] = pMsg->km_marker[1];	// 结束公理标
				cmd.bStartNow = pMsg->bStartNow;		// 是否立即开始
				cmd.bEndless = pMsg->bEndless;		// 是否永久有效
				cmd.nTime[0] = pMsg->time[0];			// 开始时间(秒)
				cmd.nTime[1] = pMsg->time[1];			// 结束时间(秒)
				cmd.nSpeedMaxVal = pMsg->speed;		

				cmd.nTccID = pMsg->tcc_no;
				cmd.nTccLineID = pMsg->tcc_line_no;
				cmd.nTccSenderID = pMsg->tcc_sender;
				cmd.bCancelLimit = pMsg->bCancelLimit;

				char buf[1024];
				memset(buf, 0x0, sizeof(buf));
				pMsg->GetComment(buf, 1024);
				//AfxMessageBox(buf);

				//TCC::Receive_TccCommandResult(cmd, pMsg->result, buf);
				_TCC_COMMAND_RESULT data;
				data.cmd = cmd;
				data.nStatus = pMsg->result;
				data.strComment = buf;
				::SendMessage(hWnd, UM_RECEIVE_TCC_COMMAND_RESULT, 0, (LPARAM)&data);
			}
		}
		else if (MSGID_MsgTccCommand==pMsgStruct->msg_id)
		{
			CLBTH::MsgTccCommand  *pMsg = (CLBTH::MsgTccCommand  *)pBase;
			// 从消息中提取确认命令
			TCC::STOCK_COMMAND cmd;
			cmd.nConsoleID = pMsg->console_no;
			TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);
			TCC::SysidFromByteArray(cmd.raw_sysid, pMsg->raw_sysid, 16);
			cmd.nCmdID = pMsg->cmd_no;
			cmd.nStaID[0] = pMsg->sta_no[0];		// 开始、结束站号
			cmd.nStaID[1] = pMsg->sta_no[1];
			cmd.nLineID = pMsg->line_no;		// 线路号
			cmd.bFullLimit = pMsg->bFullLimit;		// 是否全线限速
			cmd.nKmMark[0] = pMsg->km_marker[0];	// 开始、结束公里标（以m为单位）
			cmd.nKmMark[1] = pMsg->km_marker[1];
			cmd.bStartNow  = pMsg->bStartNow;		// 是否立即开始
			cmd.bEndless   = pMsg->bEndless;		// 是否永久有效
			cmd.nTime[0] = pMsg->time[0];		// 开始、结束时间
			cmd.nTime[1] = pMsg->time[1];
			cmd.nSpeedMaxVal = pMsg->speed;			// 限速值
			cmd.bCancelLimit = pMsg->bCancelLimit;		// 是否 取消限速命令

			cmd.nCreatorID = pMsg->creator;		// 原始命令草拟人
			cmd.nCheckorID = pMsg->checkor;		// 中心校验者
			cmd.nConfirmorID = pMsg->confirmor;	// 确认分解者

			cmd.tcc.resize(pMsg->tcc_num);		// 受令站个数
			for(int i=0; i<cmd.tcc.size(); i++)
			{
				cmd.tcc[i].nID = pMsg->tcc_no[i];		// 受令站（列控）
				cmd.tcc[i].nTccLineID = pMsg->tcc_line_no[i]; // 受令站线路
				cmd.tcc[i].nCheckorID = pMsg->tcc_checkor[i];
				cmd.tcc[i].nTime = pMsg->tcc_time[i];
				cmd.tcc[i].nStatus = pMsg->tcc_status[i];	// 受令站命令状态
				cmd.tcc[i].nSenderID = pMsg->tcc_sender[i];	// 受令站发令人（车站、助调、行调）
			}

			// 确认命令UI显示
			//TCC::Receive_StockCommand(cmd);
			::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 0, (LPARAM)&cmd);
		}
		else if (MSGID_MsgTccCommandData2==pMsgStruct->msg_id)
		{
			CLBTH::MsgTccCommandData2  *pMsg = (CLBTH::MsgTccCommandData2  *)pBase;
			if (1)
			{
				//发给C2的界面接收
				TCC::STOCK_COMMAND cmd;
				cmd.dcmd_id = (char*)pMsg->dcmd_no;
				cmd.dcmd_send_entity = pMsg->dcmd_entity;	//调度命令索引
				cmd.dcmd_create_time = pMsg->create_time;

				cmd.nConsoleID = pMsg->console_no;
				TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);
				TCC::SysidFromByteArray(cmd.raw_sysid, pMsg->raw_sysid, 16);
				cmd.nCmdID = pMsg->cmd_no;
				cmd.nStaID[0] = pMsg->sta_no[0];		// 开始、结束站号
				cmd.nStaID[1] = pMsg->sta_no[1];
				cmd.nLineID = pMsg->line_no;		// 线路号
				cmd.bFullLimit = pMsg->bFullLimit;		// 是否全线限速
				cmd.nKmMark[0] = (pMsg->km_marker[0].val & 0x0FFFFFF) + ((pMsg->km_marker[0].k_cl & 0x3F) << 24) + ((pMsg->km_marker[0].m_cl & 0x01) << 30);	// 开始、结束公里标（以m为单位）
				cmd.nKmMark[1] = (pMsg->km_marker[1].val & 0x0FFFFFF) + ((pMsg->km_marker[1].k_cl & 0x3F) << 24) + ((pMsg->km_marker[1].m_cl & 0x01) << 30);
				cmd.bStartNow  = pMsg->bStartNow;		// 是否立即开始
				cmd.bEndless   = pMsg->bEndless;		// 是否永久有效
				cmd.nTime[0] = pMsg->time[0];		// 开始、结束时间
				cmd.nTime[1] = pMsg->time[1];
				cmd.nSpeedMaxVal = pMsg->speed;			// 限速值
				cmd.bCancelLimit = pMsg->bCancelLimit;		// 是否 取消限速命令

				cmd.nCreatorID = pMsg->creator;		// 原始命令草拟人
				cmd.nCheckorID = pMsg->checkor;		// 中心校验者
				cmd.nConfirmorID = pMsg->confirmor;	// 确认分解者
				cmd.nCreateTime = pMsg->create_time;

				cmd.tcc.resize(pMsg->tcc_num);		// 受令站个数
				for(int i=0; i<cmd.tcc.size(); i++)
				{
					cmd.tcc[i].nID = pMsg->tcc_no[i];		// 受令站（列控）
					cmd.tcc[i].nTccLineID = pMsg->tcc_line_no[i]; // 受令站线路
					cmd.tcc[i].nCheckorID = pMsg->tcc_checkor[i];
					cmd.tcc[i].nTime = pMsg->tcc_time[i];
					cmd.tcc[i].nStatus = pMsg->tcc_status[i];	// 受令站命令状态
					cmd.tcc[i].nSenderID = pMsg->tcc_sender[i];	// 受令站发令人（车站、助调、行调）
				}

				// 确认命令UI显示
				switch(pMsg->type)
				{
				case CLBTH::MsgTccCommandData2::QUERY_CURRENT:
					//TCC::Receive_StockCommand(cmd);
					::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 0, (LPARAM)&cmd);
					break;
				case CLBTH::MsgTccCommandData2::QUERY_HISTORY:
					//TCC::Receive_HistoryCommand(cmd);
					::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 1, (LPARAM)&cmd);
					break;
				default:
					break;
				}
			}
		}
		else if(MSGID_MsgStccCommandIDRequest==pMsgStruct->msg_id)
		{
			CLBTH::MsgStccCommandIDRequest *pMsg = (CLBTH::MsgStccCommandIDRequest *)pBase;
			bool bValid;
			switch(pMsg->nStatus)
			{
			case CLBTH::MsgStccCommandIDRequest::TCC_CMDID_REQUEST_SUCCESS:
				bValid = true;
				break;
			case CLBTH::MsgStccCommandIDRequest::TCC_CMDID_REQUEST_FAIL:
				bValid = false;
				break;
			default:
				return CLBTH::HRCODE(CLBTH::HRCODE::HC_FAIL);
				break;
			}

			TCC::ReceiveCommandIDConfirmation(bValid, pMsg->console_no, pMsg->cmd_no, pMsg->dwCookie);
		}
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

void TccDialogUpdate(DWORD dwReason)
{
	HWND hWnd = GetTccDialogWndHandle();
	if(::IsWindow(hWnd))
		::SendMessage(hWnd, UM_CLIENTENV_CHANGED, dwReason, 0);
}

LRESULT CTccDlg::OnClientEnvChanged(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case STATIONLIST_CHANGED:
		break;
	case LOC_COMMAND_STORE_CHANGED:
		break;
	}
	return TRUE;
}


// 收到自律机发来的列控实时限速命令
//BOOL Receive_TccCommand(TCC_COMMAND& cmd, DWORD nStatus)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		::SendMessage(hWnd, UM_RECEIVE_TCC_COMMAND, nStatus, (LPARAM)&cmd);
//		return TRUE;
//	}
//	return FALSE;
//}

LRESULT CTccDlg::OnReceiveTccCommand(WPARAM wParam, LPARAM lParam)
{
	TCC_COMMAND& cmd = *(TCC_COMMAND*)lParam;
	if(cmd.nTccID != m_nCurrentTccID)
		return FALSE;

	DWORD nStatus = wParam;
	m_LircCommandPage.AppendCommand(cmd, nStatus);

	return TRUE;
}


//void Receive_TccCommandResult(TCC_COMMAND& cmd, DWORD nStatus, LPCSTR strComment)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		_TCC_COMMAND_RESULT data;
//		data.cmd = cmd;
//		data.nStatus = nStatus;
//		data.strComment = strComment;
//		::SendMessage(hWnd, UM_RECEIVE_TCC_COMMAND_RESULT, 0, (LPARAM)&data);
//	}
//}

LRESULT CTccDlg::OnReceiveTccCommandResult(WPARAM wParam, LPARAM lParam)
{
	_TCC_COMMAND_RESULT *pdata = (_TCC_COMMAND_RESULT*)lParam;
	if(pdata->cmd.nTccID == m_nCurrentTccID)
	{
		m_ConfirmCommandPage.SetCommandOperResult(pdata->cmd, pdata->nStatus, pdata->strComment);
	}

	return TRUE;
}


//void Receive_TccLineInitialCommandResult(stano_t nTccID, lineid_t nTccLineID, dir_t dir, DWORD nStatus, LPCSTR strComment)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		_TCCLINE_INITIAL_COMMAND_RESULT data;
//		data.nTccID = nTccID;
//		data.nTccLineID = nTccLineID;
//		data.dir = dir;
//		data.nStatus = nStatus;
//		data.strComment = strComment;
//		::SendMessage(hWnd, UM_RECEIVE_TCCLINE_INITIAL_COMMAND_RESULT, 0, (LPARAM)&data);
//	}
//}

LRESULT CTccDlg::OnReceiveTccLineInitialCommandResult(WPARAM wParam, LPARAM lParam)
{
	_TCCLINE_INITIAL_COMMAND_RESULT *pdata = (_TCCLINE_INITIAL_COMMAND_RESULT*)lParam;
	if(pdata->nTccID == m_nCurrentTccID)
	{
		m_LircCommandPage.SetTccLineInitialCommandOperResult(
			pdata->nTccID, 
			pdata->nTccLineID, 
			pdata->dir,
			pdata->nStatus,
			pdata->strComment);
	}

	return TRUE;
}

// 收到自律机发来的列控状态
//BOOL Receive_TccReport(DWORD nTccID, TCC_STATUS& status)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		::SendMessage(hWnd, UM_RECEIVE_TCC_REPORT, nTccID, (LPARAM)&status);
//		return TRUE;
//	}
//	return FALSE;
//}

LRESULT CTccDlg::OnReceiveTccReport(WPARAM wParam, LPARAM lParam)
{
	m_LircCommandPage.Tcc_Report((DWORD)wParam, *(TCC_STATUS*)lParam);
	return TRUE;
}

//BOOL Receive_StockCommand(STOCK_COMMAND& cmd)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 0, (LPARAM)&cmd);
//		return TRUE;
//	}
//	return FALSE;
//}

//BOOL Receive_HistoryCommand(STOCK_COMMAND& cmd)
//{
//	HWND hWnd = GetTccDialogWndHandle();
//	if(::IsWindow(hWnd))
//	{
//		::SendMessage(hWnd, UM_RECEIVE_CONFIRM_COMMAND, 1, (LPARAM)&cmd);
//		return TRUE;
//	}
//	return FALSE;
//}

LRESULT CTccDlg::OnReceiveConfirmCommand(WPARAM wParam, LPARAM lParam)
{
	STOCK_COMMAND& cmd = *(STOCK_COMMAND*)lParam;

	TCC_COMMAND tcccmd;
	if(!cmd.GetTccCommand(&tcccmd, m_nCurrentTccID))
		return FALSE;

	/*if(RAW_STOCK_COMMAND *pRawCmd = m_LocalCommandPage.GetRawCommand(cmd.raw_sysid))
	{
		BOOL bChanged = FALSE;
		BOOL bCancelAll = TRUE;
		for(int i=0; i<cmd.tcc.size() && i<8; i++)
		{
			if(pRawCmd->SetStatus(cmd.sys_id, cmd.tcc[i].nID, cmd.tcc[i].nStatus))
				bChanged = TRUE;

			if(pRawCmd->SetSender(cmd.sys_id, cmd.tcc[i].nID, cmd.tcc[i].nSenderID))
				bChanged = TRUE;
				
			if(CANCELED == cmd.tcc[i].nStatus && bCancelAll)
				bCancelAll = FALSE;
		}
		if(bChanged)
		{
			m_LocalCommandPage.AppendCommand(*pRawCmd);
			// 保存命令状态到原始命令文件
			CString szFilename;
			szFilename.Format("%s\\%s.%s", 
				m_pTccClientInfoProvider->GetTccDataDirectory(), 
				GetCommandExtraFileName(cmd.raw_sysid, cmd.nCmdID),
				GetCommandExtFileName());
			SaveCommand(*pRawCmd, szFilename);
		}
	}*/


	// 删除服务器已全部取消的命令

	switch(wParam)
	{
	case 0:
		m_ConfirmCommandPage.AppendCommand(cmd);
		break;
	case 1:
		m_HistoryCommandPage.AppendCommand(cmd);
		break;
	default:
		break;
	}

	return TRUE;
}

int CTccDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CoInitialize(NULL);
	hWndTccUI = GetSafeHwnd();

	return 0;
}

void CTccDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TAB1, m_sheet);
	DDX_Control(pDX, IDC_TCC_LIST, m_TccList);
}

void CTccDlg::OnSelectTcc()
{
	INT nIndex = m_TccList.GetCurSel();
	if(CB_ERR == nIndex)
		return ;

	m_nCurrentTccID = (stano_t)m_TccList.GetItemData(nIndex);
	OnRefreshTcc();
}

void CTccDlg::OnRefreshTcc()
{
	if(CB_ERR == m_TccList.GetCurSel())
	{
		MessageBox("请选择车站!");
		return ;
	}

	m_LircCommandPage.Refresh();
	m_ConfirmCommandPage.Refresh();
	m_HistoryCommandPage.Refresh();
}

void CTccDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	CoUninitialize();
	hWndTccUI = NULL;
}

void CTccDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	if(m_bAutoDelete)
		delete this;

	CDialog::PostNcDestroy();
}

void CTccDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 在此处添加消息处理程序代码
	if(m_pTccClientInfoProvider->IsStationListChanged() )
	{
		// 列控中心列表
		ResetTccList(m_TccList, m_pTccClientInfoProvider);
	}
}

};
