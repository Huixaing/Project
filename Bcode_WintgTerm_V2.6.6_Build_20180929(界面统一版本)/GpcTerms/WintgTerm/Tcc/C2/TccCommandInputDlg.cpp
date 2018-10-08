// Tcc\TccCommandInputDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "tcc_base.h"
#include "TccCommandInputDlg.h"
#include "PasswordInputDlg.h"
#include "CmdTypeSelectDlg.h"

#include <algorithm>

namespace TCC {

/*
 * 站内限速和区间限速由用户输入公里标
 * 区间全限速 系统自动填入公里标 0--0
 * 侧线限速   系统自动填入公里标 0--9999999
 */

BOOL RawStockCommandInput(
		RAW_STOCK_COMMAND *pcmd, 
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink,
		CWnd *pParent)
{
	ASSERT(pcmd);
	CTccCommandInputDlg dlg(NULL, FALSE, pInfoProvider, pAdviseSink, pParent);
	if(IDOK == dlg.DoModal())
	{
		*pcmd = *dlg.GetCommand();
		return TRUE;
	}
	return FALSE;
}

BOOL RawStockCommandReEdit(
		RAW_STOCK_COMMAND *pcmd, 
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink,
		CWnd *pParent)
{
	ASSERT(pcmd);
	CTccCommandInputDlg dlg(pcmd, FALSE, pInfoProvider, pAdviseSink, pParent);
	if(IDOK == dlg.DoModal())
	{
		*pcmd = *dlg.GetCommand();
		return TRUE;
	}
	return FALSE;
}

BOOL RawStockCommandValid(
		RAW_STOCK_COMMAND *pcmd, 
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink,
		CWnd *pParent)
{
	ASSERT(pcmd);

	CTccCommandInputDlg dlg(pcmd, TRUE, pInfoProvider, pAdviseSink, pParent);
	if(IDOK == dlg.DoModal())
	{
		*pcmd = *dlg.GetCommand();
		return TRUE;
	}
	return FALSE;
}

// CTccCommandInputDlg 对话框

IMPLEMENT_DYNAMIC(CTccCommandInputDlg, CDialog)
CTccCommandInputDlg::CTccCommandInputDlg(
	RAW_STOCK_COMMAND* pcmd,
	BOOL bCheckCommand,
	ITccClientInfoProvider *pInfoProvider, 
	ITccAdviseSink *pAdviseSink,
	CWnd* pParent /*=NULL*/)
	: CDialog(CTccCommandInputDlg::IDD, pParent)
	, m_pInfoProvider(pInfoProvider)
	, m_pAdviseSink(pAdviseSink)
	, m_bCheckCommand(bCheckCommand)
	, m_nCmdID(0)
	, m_nKmBegin(0)
	, m_nMBegin(0)
	, m_nKmEnd(0)
	, m_nMEnd(0)
	, m_bTimeBegin(1)
	, m_bTimeEnd(1)
	, m_bCancelLimit(FALSE)
	, m_bGenCancelCommand(FALSE)
	, m_bCmdIDConfirmed(false)
	, m_bManualChangeCommandID(FALSE)
{
	if(pcmd)
	{
		m_cmd = *pcmd;
		if(0 != m_cmd.nCmdID)
			m_bCmdIDConfirmed = true;
	}
	else
	{
		if(m_bCheckCommand)
			ASSERT(FALSE);
	}
}

CTccCommandInputDlg::~CTccCommandInputDlg()
{
}

void CTccCommandInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCC_CMD_TYPE, m_CmdType);
	DDX_Control(pDX, IDC_TCC_CMDID, m_CmdID);
	DDX_Text(pDX, IDC_TCC_CMDID, m_nCmdID);
	DDV_MinMaxUInt(pDX, m_nCmdID, 0, 9999);
	DDX_Control(pDX, IDC_CHANGE_TCCCMDID, m_btnChangeCmdID);
	DDX_Control(pDX, IDC_AUTOCOMPUTE_TCCCMDID, m_btnAutoComputeCmdID);

	DDX_Control(pDX, IDC_TCC_CMD_STA, m_StaBegin);
	DDX_Control(pDX, IDC_TCC_CMD_STA2,m_StaEnd);

	DDX_Control(pDX, IDC_TCC_CMD_ABSLINE, m_AbsLine);


	DDX_Text(pDX, IDC_TCC_CMD_KM, m_nKmBegin);
	DDV_MinMaxInt(pDX, m_nKmBegin, 0, 9999);
	DDX_Text(pDX, IDC_TCC_CMD_M, m_nMBegin);
	DDV_MinMaxInt(pDX, m_nMBegin, 0, 999);
	DDX_Control(pDX, IDC_TCC_CMD_KM, m_KmBegin);
	DDX_Control(pDX, IDC_TCC_CMD_M, m_MBegin);
	DDX_Text(pDX, IDC_TCC_CMD_KM2, m_nKmEnd);
	DDV_MinMaxInt(pDX, m_nKmEnd, 0, 9999);
	DDX_Text(pDX, IDC_TCC_CMD_M2, m_nMEnd);
	DDV_MinMaxInt(pDX, m_nMEnd, 0, 999);
	DDX_Control(pDX, IDC_TCC_CMD_KM2, m_KmEnd);
	DDX_Control(pDX, IDC_TCC_CMD_M2, m_MEnd);

	DDX_Control(pDX, IDC_TCC_CMD_LONGKM, m_StartLongKM);
	DDX_Control(pDX, IDC_TCC_CMD_LONGKM2, m_EndLongKM);
	DDX_Control(pDX, IDC_TCC_CMD_LONGM, m_StartLongM);
	DDX_Control(pDX, IDC_TCC_CMD_LONGM2, m_EndLongM);
	DDX_Radio(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO, m_bTimeBegin);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN, m_tmBegin);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO, m_BeginRadio);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEBEGIN_RADIO2, m_BeginRadio2);
	DDX_Radio(pDX, IDC_TCC_CMD_TIMEEND_RADIO, m_bTimeEnd);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND, m_tmEnd);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND_RADIO, m_EndRadio);
	DDX_Control(pDX, IDC_TCC_CMD_TIMEEND_RADIO2, m_EndRadio2);

	DDX_Check(pDX, IDC_TCC_CANCE_LIMIT, m_bCancelLimit);
	DDX_Control(pDX, IDC_TCC_CANCE_LIMIT, m_CancelLimit);
	DDX_Check(pDX, IDC_TCC_CANCELCOMMAND, m_bGenCancelCommand);
	DDX_Control(pDX, IDC_TCC_CANCELCOMMAND, m_GenCancelCommand);

	DDX_Control(pDX, IDC_TCC_LIMIT2, m_Limit[1]);
	DDX_Control(pDX, IDC_TCC_LIMIT3, m_Limit[2]);
	DDX_Control(pDX, IDC_TCC_LIMIT4, m_Limit[3]);
	DDX_Control(pDX, IDC_TCC_LIMIT5, m_Limit[4]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL,  m_LimitVal[0]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL2, m_LimitVal[1]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL3, m_LimitVal[2]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL4, m_LimitVal[3]);
	DDX_Control(pDX, IDC_TCC_CMD_LIMITVAL5, m_LimitVal[4]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID,  m_TrainID[0]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID2, m_TrainID[1]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID3, m_TrainID[2]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID4, m_TrainID[3]);
	DDX_Control(pDX, IDC_TCC_CMD_TRAINID5, m_TrainID[4]);

	DDX_Text(pDX, IDC_TCC_CMD_NOTE, m_CmdString);
}


BEGIN_MESSAGE_MAP(CTccCommandInputDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEBEGIN_RADIO2, OnBtnClickedTccCmdTimebeginRadio2)
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEBEGIN_RADIO,  OnBtnClickedTccCmdTimebeginRadio)
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEEND_RADIO2,   OnBtnClickedTccCmdTimeendRadio2)
	ON_BN_CLICKED(IDC_TCC_CMD_TIMEEND_RADIO,    OnBtnClickedTccCmdTimeendRadio)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_TYPE, OnCbnSelchangeTccCmdType)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_STA, OnCbnSelchangeTccCmdSta)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_STA2, OnCbnSelchangeTccCmdSta2)
	ON_CBN_SELCHANGE(IDC_TCC_CMD_ABSLINE, OnCbnSelchangeTccLine)
	ON_BN_CLICKED(IDC_TCC_CANCE_LIMIT, OnSelCancelLimit)
	ON_BN_CLICKED(IDC_TCC_LIMIT2, OnBtnClickedLimit2)
	ON_BN_CLICKED(IDC_TCC_LIMIT3, OnBtnClickedLimit3)
	ON_BN_CLICKED(IDC_TCC_LIMIT4, OnBtnClickedLimit4)
	ON_BN_CLICKED(IDC_TCC_LIMIT5, OnBtnClickedLimit5)
	ON_BN_CLICKED(IDC_CHANGE_TCCCMDID, OnChangeTccCmdID)
	ON_BN_CLICKED(IDC_AUTOCOMPUTE_TCCCMDID, OnBtnClickedAutoComputeCmdID)
	
	ON_MESSAGE(UM_SERVER_CONFIRM_COMMAND_ID, OnServerConfirmCommandID)
END_MESSAGE_MAP()

void ResetEndStationList(
	CComboBox &StaBegin,
	CComboBox &StaEnd,
	ITccClientInfoProvider *pInfoProvider)
{
	INT nIndex = StaBegin.GetCurSel();
	if(CB_ERR == nIndex)
		return ;

	// 开始站, 结束站
	stano_t nStaID, nStaID2;
	nStaID = (stano_t)StaBegin.GetItemData(nIndex);

	// 删除结束站列表内容
	StaEnd.ResetContent();

	// 结束站列表添加邻站
	std::vector<stano_t> staList, staLinkedList;
	pInfoProvider->GetStationList(staList);
	pInfoProvider->GetLinkedStationList(nStaID, staLinkedList);

	for(int i=0; i<staLinkedList.size(); i++)
	{
		nStaID2 = staLinkedList[i];
		if(staList.end() == std::find(staList.begin(), staList.end(), nStaID2))
			continue;

		nIndex = StaEnd.AddString(pInfoProvider->GetStationName(nStaID2));
		ASSERT(CB_ERR != nIndex);
		StaEnd.SetItemData(nIndex, nStaID2);
	}
	StaEnd.SetCurSel(-1);
}

// CTccCommandInputDlg 消息处理程序
BOOL TCC::CTccCommandInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if(NULL == m_pInfoProvider)
	{
		MessageBox("没有安装TccClientInfoProvider接口!");
		return FALSE;
	}

	if(!TccInitialize(m_pInfoProvider))
		return FALSE;

	if(m_bManualChangeCommandID)
		m_btnChangeCmdID.SetWindowText("人工确认");
	else
		m_btnChangeCmdID.SetWindowText("修改命令号");

	RegisterConfirmCommandIDReceiver( GetSafeHwnd() );

	m_tmBegin.SetFormat("yyy'/'MM'/'dd HH':'mm");
	m_tmEnd.SetFormat("yyy'/'MM'/'dd HH':'mm");

	CString str;
	// 初始化命令类型列表
	INT nIndex = m_CmdType.AddString("站内限速");
	m_CmdType.SetItemData(nIndex, STATION_LIMIT);
	nIndex = m_CmdType.AddString("区间限速");
	m_CmdType.SetItemData(nIndex, ABSAPP_LIMIT);
	nIndex = m_CmdType.AddString("区间全限速");
	m_CmdType.SetItemData(nIndex, ABSAPP_FULL_LIMIT);
	nIndex = m_CmdType.AddString("侧线限速");
	m_CmdType.SetItemData(nIndex, BLOCK_LIMIT);

	// 初始化开始站、结束站、线路列表
	std::vector<stano_t> staList;
	m_pInfoProvider->GetStationList(staList);
	for(int i=0; i<staList.size(); i++)
	{
		str = m_pInfoProvider->GetStationName(staList[i]);
		INT nIndex = m_StaBegin.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_StaBegin.SetItemData(nIndex, staList[i]);
	}
	m_StaBegin.SetCurSel(-1);
	//OnCbnSelchangeTccCmdSta();

	// 限速值
	//CString limitspeed[5] = {"45", "60", "80", "120", "180" };
	extern std::vector<int> AvailableLimitSpeedList;
	for(i=0; i<AvailableLimitSpeedList.size(); i++)
	{
		str.Format("%d", AvailableLimitSpeedList[i]);
		
		//小于45km/h的处理
		if(AvailableLimitSpeedList[i] < 45 )
			str+="(45)";

		INT nIndex;
		for(int j=0; j<5; j++)
		{
			nIndex = m_LimitVal[j].AddString(str);
			ASSERT(CB_ERR != nIndex);
			m_LimitVal[j].SetItemData(nIndex, AvailableLimitSpeedList[i]);
		}
	}
	//长链
	extern std::map<int,CString> LongKmMap;
	 std::map<int,CString>::iterator  iter;

	 for(iter=LongKmMap.begin(); iter!=LongKmMap.end(); iter++)
	{
		str.Format("%s", iter->second);
		INT nIndex = m_StartLongKM.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_StartLongKM.SetItemData(nIndex, iter->first);

		str.Format("%s", iter->second);
		nIndex = m_EndLongKM.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_EndLongKM.SetItemData(nIndex, iter->first);
	}

	m_StartLongKM.SetCurSel(-1);
	m_EndLongKM.SetCurSel(-1);

	extern std::map<int,CString> LongMMap;

	for(iter=LongMMap.begin(); iter!=LongMMap.end(); iter++)
	{
		str.Format("%s", iter->second);
		INT nIndex = m_StartLongM.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_StartLongM.SetItemData(nIndex, iter->first);

		str.Format("%s", iter->second);
		nIndex = m_EndLongM.AddString(str);
		ASSERT(CB_ERR != nIndex);
		m_EndLongM.SetItemData(nIndex, iter->first);
	}

	m_StartLongM.SetCurSel(-1);
	m_EndLongM.SetCurSel(-1);

	//end 长链

	m_LimitVal[0].SetCurSel(-1);
	for(i=1; i<5; i++)
	{
		m_Limit[i].EnableWindow(FALSE);
		m_LimitVal[i].EnableWindow(FALSE);
		m_TrainID[i].EnableWindow(FALSE);
	}
	m_Limit[1].EnableWindow(TRUE);
	m_LimitVal[0].EnableWindow(TRUE);
	m_TrainID[0].EnableWindow(TRUE);

	CRect rcl;
	GetDlgItem(IDC_TCC_LIST)->GetWindowRect(&rcl);
	ScreenToClient(rcl);
	m_TccList.Create(rcl, this, 8341);
	InitListCtrl(m_TccList);

	m_bCancelLimit = m_cmd.bCancelLimit;
	m_CancelLimit.EnableWindow(FALSE);
	m_bGenCancelCommand = m_cmd.bGenCancelCommand;
	m_GenCancelCommand.EnableWindow(FALSE);

	//有命令号之后，不可以改命令号
	m_nCmdID = m_cmd.nCmdID;
	if(0 != m_cmd.nCmdID)
	{
		m_CmdID.EnableWindow(FALSE);
		m_btnChangeCmdID.ShowWindow(SW_HIDE);
		m_btnAutoComputeCmdID.ShowWindow(SW_HIDE);
	}

	if(m_bCheckCommand)//校验
	{
		str.Format("限速命令校验(%s)", SysidToString(m_cmd.raw_sysid));
		SetWindowText(str);

		m_tmBegin.EnableWindow(FALSE);
		m_tmEnd.EnableWindow(FALSE);

		OnCbnSelchangeTccCmdType();
		m_CmdString = m_cmd.toString(m_pInfoProvider);

		if(m_bCancelLimit)
		{
			m_Limit[1].EnableWindow(FALSE);
			m_LimitVal[0].EnableWindow(FALSE);
			m_TrainID[0].EnableWindow(FALSE);
		}

		UpdateData(FALSE);
	}
	else
	{
		//隐藏校验时命令信息提示窗
		CWnd *pWnd = GetDlgItem(IDC_TCC_CMD_NOTE);
		ASSERT(NULL != pWnd);

		CRect rcWnd;
		GetWindowRect(&rcWnd);

		CRect rc;
		pWnd->GetWindowRect(&rc);
		rcWnd.bottom = rc.top;
		SetWindowPos(NULL, 0, 0, rcWnd.Width(), rcWnd.Height(), SWP_NOMOVE);
		CenterWindow();
		pWnd->ShowWindow(SW_HIDE);

		do {
			// 编辑命令状态，将原数据取出
			sysid_t sysid;
			SysidCreateNil(&sysid);
			if(m_cmd.raw_sysid == sysid)
			{
				SysidCreate(&m_cmd.raw_sysid);
			}
			if(0 == m_cmd.nCmdID)
			{
				CCmdTypeSelectDlg dlg(this);
				if(IDOK == dlg.DoModal())
				{
					m_bCancelLimit = m_cmd.bCancelLimit = FALSE;
				}
				else
				{
					m_bCancelLimit = m_cmd.bCancelLimit = TRUE;
				}

				if(!m_bCancelLimit)
				{
					m_nCmdID = m_pInfoProvider->GetCommandID();

					RequestCommandID(m_nCmdID);
				}
			}
			UpdateData(FALSE);

			str.Format("限速命令输入(%s)", SysidToString(m_cmd.raw_sysid));
			SetWindowText(str);
			
			if(m_bCancelLimit)
			{
				m_bGenCancelCommand = FALSE;
	
				m_CmdID.EnableWindow(TRUE);
				m_btnChangeCmdID.ShowWindow(SW_HIDE);
				m_btnAutoComputeCmdID.ShowWindow(SW_HIDE);

				m_LimitVal[0].EnableWindow(FALSE);
				m_TrainID[0].EnableWindow(FALSE);
				m_Limit[1].EnableWindow(FALSE);
			}
			else
				m_GenCancelCommand.EnableWindow(TRUE);

			if(m_cmd.nStaID[1] == 0)
			{
				if(m_pInfoProvider->IsTccBlockLine(m_cmd.nStaID[0], m_cmd.nLineID))
					m_CmdType.SetCurSel(3);
				else
					m_CmdType.SetCurSel(0);
			}
			else
			{
				if(m_cmd.bFullLimit)
					m_CmdType.SetCurSel(2);
				else
					m_CmdType.SetCurSel(1);
			}
			OnCbnSelchangeTccCmdType();


			// 开始站
			for(int i=0; i<m_StaBegin.GetCount(); i++)
			{
				if(m_StaBegin.GetItemData(i) == m_cmd.nStaID[0])
				{
					m_StaBegin.SetCurSel(i);
					OnCbnSelchangeTccCmdSta();
					break;
				}
			}

			// 结束站
			for(i=0; i<m_StaEnd.GetCount(); i++)
			{
				if(m_StaEnd.GetItemData(i) == m_cmd.nStaID[1])
				{
					m_StaEnd.SetCurSel(i);
					OnCbnSelchangeTccCmdSta2();
					break;
				}
			}

			// 线路
			for(i=0; i<m_AbsLine.GetCount(); i++)
			{
				if(m_AbsLine.GetItemData(i) == m_cmd.nLineID)
				{
					m_AbsLine.SetCurSel(i);
					OnCbnSelchangeTccLine();
					break;
				}
			}
			//限速值
			if(!m_cmd.bCancelLimit)
			{
				for(i=0; i<5; i++)
				{
					if(m_cmd.train_cmd[i].nSpeedMaxVal <= 0)
						continue;

					if(i>0)	m_Limit[i].SetCheck(TRUE);
					for(int j=0; j<m_LimitVal[i].GetCount(); j++)
					{
						if(m_LimitVal[i].GetItemData(j) == m_cmd.train_cmd[i].nSpeed)
						{
							m_LimitVal[i].SetCurSel(j);
							break;
						}
					}
					m_TrainID[i].SetWindowText(m_cmd.train_cmd[i].szTrainID);
					m_LimitVal[i].EnableWindow(TRUE);
					m_TrainID[i].EnableWindow(TRUE);
					if(i<4)
						m_Limit[i+1].EnableWindow(TRUE);
				}
			}
			//开始时间
			if(m_cmd.bStartNow || m_cmd.nTime[0] == 0)
			{
				// 立即开始，取发令时间
				m_bTimeBegin = 1;
				m_tmBegin.EnableWindow(FALSE);
			}
			else
			{
				m_bTimeBegin = 0;
				m_tmBegin.SetTime(&CTime(m_cmd.nTime[0]));
				m_tmBegin.EnableWindow(TRUE);
			}
			
			if(m_cmd.bEndless || m_cmd.nTime[1] == 0)
			{
				// 永久有效，取发令时间
				m_bTimeEnd = 1;
				m_tmEnd.EnableWindow(FALSE);
			}
			else
			{
				m_bTimeEnd = 0;
				m_tmEnd.SetTime(&CTime(m_cmd.nTime[1]));
				m_tmEnd.EnableWindow(TRUE);
			}

			// 公理标, 长链后
			m_nKmBegin = m_cmd.nKmMark[0].val / 1000;
			m_nMBegin  = m_cmd.nKmMark[0].val % 1000;
			m_nKmEnd   = m_cmd.nKmMark[1].val / 1000;
			m_nMEnd    = m_cmd.nKmMark[1].val % 1000;
			{
				BYTE nFlag = m_cmd.nKmMark[0].km_cl;//GetLongKmFlag(m_cmd.nKmMark[0]);
				for(i=0; i<m_StartLongKM.GetCount(); i++)
				{
					if(m_StartLongKM.GetItemData(i) == nFlag)
					{
						m_StartLongKM.SetCurSel(i);
						break;
					}
				}

				nFlag = m_cmd.nKmMark[0].m_cl;//GetLongMFlag(m_cmd.nKmMark[0]);
				for(i=0; i<m_StartLongM.GetCount(); i++)
				{
					if(m_StartLongM.GetItemData(i) == nFlag)
					{
						m_StartLongM.SetCurSel(i);
						break;
					}
				}

				nFlag = m_cmd.nKmMark[1].km_cl;//GetLongKmFlag(m_cmd.nKmMark[1]);
				for(i=0; i<m_EndLongKM.GetCount(); i++)
				{
					if(m_EndLongKM.GetItemData(i) == nFlag)
					{
						m_EndLongKM.SetCurSel(i);
						break;
					}
				}

				nFlag = m_cmd.nKmMark[1].m_cl;//GetLongMFlag(m_cmd.nKmMark[1]);
				for(i=0; i<m_EndLongM.GetCount(); i++)
				{
					if(m_EndLongM.GetItemData(i) == nFlag)
					{
						m_EndLongM.SetCurSel(i);
						break;
					}
				}
			
			}
		}while(0);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CTccCommandInputDlg::OnDestroy()
{
	CDialog::OnDestroy();

	UnRegisterConfirmCommandIDReceiver( GetSafeHwnd() );
}


// 选择开始站时，结束站将只有邻站可供选择，同时线路相应变化
void CTccCommandInputDlg::OnCbnSelchangeTccCmdSta()
{
	INT nIndex = m_StaBegin.GetCurSel();
	if(CB_ERR == nIndex)
		return;
	CString szStaName;
	m_StaBegin.GetLBText(nIndex, szStaName);
	stano_t nStaID = (stano_t)m_StaBegin.GetItemData(nIndex);

	m_StaEnd.ResetContent();
	m_AbsLine.ResetContent();

	nIndex = m_CmdType.GetCurSel();
	ASSERT(CB_ERR != nIndex);
	int nCmdType = m_CmdType.GetItemData(nIndex);
	if(STATION_LIMIT == nCmdType)
	{
		// 添加线路
		std::vector<lineid_t> lines;
		m_pInfoProvider->GetTccLineList(nStaID, lines);

		for(int i=0; i<lines.size(); i++)
		{
			nIndex = m_AbsLine.AddString(m_pInfoProvider->GetTccLineName(nStaID, lines[i]) );
			ASSERT(CB_ERR != nIndex);
			m_AbsLine.SetItemData(nIndex, lines[i]);
		}
		m_AbsLine.SetCurSel(-1);

		// 设置受令站
		m_TccList.DeleteNonFixedRows();
	}
	else if(BLOCK_LIMIT == nCmdType)
	{
		// 添加线路
		std::vector<lineid_t> lines;
		m_pInfoProvider->GetTccBlockLineList(nStaID, lines);

		for(int i=0; i<lines.size(); i++)
		{
			nIndex = m_AbsLine.AddString(m_pInfoProvider->GetTccBlockLineName(nStaID, lines[i]) );
			ASSERT(CB_ERR != nIndex);
			m_AbsLine.SetItemData(nIndex, lines[i]);
		}
		m_AbsLine.SetCurSel(-1);

		// 设置受令站
		m_TccList.DeleteNonFixedRows();
	}
	else
	{
		ResetEndStationList(m_StaBegin, m_StaEnd, m_pInfoProvider);

		// 设置受令站
		m_TccList.DeleteNonFixedRows();
	}
	m_TccList.Refresh();
}

// 选择结束站时，开始站不变化，但线路相应变化
void CTccCommandInputDlg::OnCbnSelchangeTccCmdSta2()
{
	// 开始站，结束站
	stano_t nStaID, nStaID2;
	CString szStaName, szStaName2;

	INT nIndex = m_StaBegin.GetCurSel();
	if(CB_ERR == nIndex)
		return;
	nStaID = (stano_t)m_StaBegin.GetItemData(nIndex);
	m_StaBegin.GetLBText(nIndex, szStaName);

	nIndex = m_StaEnd.GetCurSel();
	if(CB_ERR == nIndex)
		return;
	nStaID2 = (stano_t)m_StaEnd.GetItemData(nIndex);
	m_StaEnd.GetLBText(nIndex, szStaName2);

	switch(m_pInfoProvider->GetStationDirection(nStaID, nStaID2))
	{
	case DIR_UP:	// 上行区间全限速
		m_KmBegin.SetWindowText("0");
		m_MBegin.SetWindowText("0");
		m_KmEnd.SetWindowText("0");
		m_MEnd.SetWindowText("0");
		break;
	case DIR_DOWN:	// 下行区间全限速
		m_KmBegin.SetWindowText("0");
		m_MBegin.SetWindowText("0");
		m_KmEnd.SetWindowText("0");
		m_MEnd.SetWindowText("0");
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	//删除线路列表内容
	m_AbsLine.ResetContent();

	// 添加线路
	std::vector<lineid_t> lines;
	m_pInfoProvider->GetTccLineList(nStaID, nStaID2, lines);

	for(int i=0; i<lines.size(); i++)
	{
		nIndex = m_AbsLine.AddString(m_pInfoProvider->GetTccLineName(nStaID, lines[i]) );
		ASSERT(CB_ERR != nIndex);
		m_AbsLine.SetItemData(nIndex, lines[i]);
	}
	m_AbsLine.SetCurSel(-1);

	// 设置受令站
	m_TccList.SetRowCount(m_TccList.GetFixedRowCount());

	int row = m_TccList.InsertRow(szStaName, -1);
	m_TccList.SetItemData(row, 0, nStaID);
	m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));
	m_TccList.SetItemData(row, 1, 0);
	m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
	m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
	m_TccList.SetItemData(row,  2, 0);
	m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));

	row = m_TccList.InsertRow(szStaName2, -1);
	m_TccList.SetItemData(row, 0, nStaID2);
	m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));
	m_TccList.SetItemData(row, 1, 0);
	m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
	m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
	m_TccList.SetItemData(row,  2, 0);
	m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));

	m_TccList.Refresh();
}

void CTccCommandInputDlg::OnCbnSelchangeTccLine()
{
	stano_t nStaID = 0,	nStaID2 = 0;

	// 开始站
	INT nIndex = m_StaBegin.GetCurSel();
	ASSERT(CB_ERR != nIndex);
	nStaID = (stano_t)m_StaBegin.GetItemData(nIndex);

	// 结束站
	nIndex = m_StaEnd.GetCurSel();
	if(CB_ERR != nIndex)
		nStaID2 = (stano_t)m_StaEnd.GetItemData(nIndex);
	
	// 线路号
	nIndex = m_AbsLine.GetCurSel();
	if(CB_ERR == nIndex)
		return;
	lineid_t nTccLineID = (lineid_t)m_AbsLine.GetItemData(nIndex);
	CString szTccLineName = m_pInfoProvider->GetTccLineName(nStaID, nTccLineID);

	nIndex = m_CmdType.GetCurSel();
	if(CB_ERR == nIndex)
		return;
	int nCmdType = m_CmdType.GetItemData(nIndex);

	if(0 != nStaID2)
	{
		//m_TccList.SetItemText(1, 1, szTccLineName);
		//m_TccList.SetItemData(1, 1, nTccLineID);

		//m_TccList.SetItemText(2, 1, szTccLineName);
		//m_TccList.SetItemData(2, 1, m_pInfoProvider->GetTccLine(nStaID2, nStaID, nTccLineID));
	}
	else if(BLOCK_LIMIT == nCmdType)
	{
		// 侧线限速
		CString szTccLineName = m_pInfoProvider->GetTccBlockLineName(nStaID, nTccLineID);

		// 设置受令站
		m_TccList.DeleteNonFixedRows();

		int row = -1;
		CString szStaName;
		CGridCellCheck *pCell = NULL;
	
		szStaName = m_pInfoProvider->GetStationName(nStaID);
		row = m_TccList.InsertRow(szStaName, -1);
		m_TccList.SetItemData(row, 0, nStaID);
		m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));	
		
		m_TccList.SetItemText(1, 1, szTccLineName);
		m_TccList.SetItemData(1, 1, nTccLineID);
		m_TccList.SetItemData(row, 1, 0);
		m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
	
		m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
		m_TccList.SetItemData(row,  2, 0);
		m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));
	}
	else
	{
		// 站内限速

		// 设置受令站
		m_TccList.DeleteNonFixedRows();

		int row = -1;
		CString szStaName;
		
		szStaName = m_pInfoProvider->GetStationName(nStaID);
		row = m_TccList.InsertRow(szStaName, -1);
		m_TccList.SetItemData(row, 0, nStaID);
		m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));
		m_TccList.SetItemData(row, 1, 0);
		m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
	
		m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
		m_TccList.SetItemData(row,  2, 0);
		m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));

		stano_t nLinkStaID = 0;
		nLinkStaID = m_pInfoProvider->GetLinkedStation(nStaID, nTccLineID, DIR_UP);
		if(0 != nLinkStaID)
		{
			szStaName = m_pInfoProvider->GetStationName(nLinkStaID);
			row = m_TccList.InsertRow(szStaName, -1);
			m_TccList.SetItemData(row, 0, nLinkStaID);
			m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));
			m_TccList.SetItemData(row, 1, 0);
			m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
			m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
			m_TccList.SetItemData(row,  2, 0);
			m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));
		}

		nLinkStaID = m_pInfoProvider->GetLinkedStation(nStaID, nTccLineID, DIR_DOWN);
		if(0 != nLinkStaID)
		{
			szStaName = m_pInfoProvider->GetStationName(nLinkStaID);
			row = m_TccList.InsertRow(szStaName, -1);
			m_TccList.SetItemData(row, 0, nLinkStaID);
			m_TccList.SetItemState(row, 0, GVIS_READONLY | m_TccList.GetItemState(row, 0));
			m_TccList.SetItemData(row, 1, 0);
			m_TccList.SetItemState(row, 1, GVIS_READONLY | m_TccList.GetItemState(row, 1));
			m_TccList.SetCellType(row,  2, RUNTIME_CLASS(CGridCellCheck));
			m_TccList.SetItemData(row,  2, 0);
			m_TccList.SetItemState(row, 2, m_TccList.GetItemState(row, 2));
		}
	}

		// 设置相邻站线路
		for(int row=m_TccList.GetFixedRowCount(); row<m_TccList.GetRowCount(); row++)
		{
			stano_t nLinkStaID = (stano_t)m_TccList.GetItemData(row, 0);
			lineid_t nLinkTccLineID = 0;

			if(nStaID == nLinkStaID)
			{
				m_TccList.SetItemText(row, 1, szTccLineName);
				m_TccList.SetItemData(row, 1, nTccLineID);

				nLinkTccLineID = nTccLineID;
			}
			else
			{
				// 每次线路变更时必须清空上次的线路选择
				m_TccList.SetItemText(row, 1, NULL);

				// 设置相邻站的线路下拉列表
				CStringArray options;
				std::vector<lineid_t> lines;

				m_TccList.SetItemState(row, 1, ~GVIS_READONLY & m_TccList.GetItemState(row, 1));
				m_pInfoProvider->GetTccLineList(nLinkStaID, nStaID, lines);
				for(int j=0; j<lines.size(); j++)
					options.Add( m_pInfoProvider->GetTccLineName(nLinkStaID, lines[j]) );
				
				m_TccList.SetCellType(row, 1, RUNTIME_CLASS(CGridCellCombo));
				CGridCellCombo *pCell= (CGridCellCombo*) m_TccList.GetCell(row, 1);
				pCell->SetOptions(options);
				pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST ); //CBS_DROPDOWNLIST

				// ReEdit或某站校验时, 先填入本次计算的线路
				if(!m_bCheckCommand)
				{
					nLinkTccLineID = m_pInfoProvider->GetTccLine(nLinkStaID, nStaID, nTccLineID);
					if(0 != nLinkTccLineID)
						pCell->SetText(m_pInfoProvider->GetTccLineName(nLinkStaID, nLinkTccLineID));
				}
			}

			// ReEdit或某站校验时
			if(!m_bCheckCommand)
			{
				bool bTccValidate = false;

				for(int i=0; i<m_cmd.tcc.size(); i++)
				{
					if(m_cmd.tcc[i].nID == nLinkStaID)
					{
						if(nLinkTccLineID == m_cmd.tcc[i].nTccLineID)
						{
							bTccValidate = m_cmd.tcc[i].bTccValidate;
						}
						else
						{
							//计算的线路号与输入的值不同，必须留给用户选择
							m_TccList.SetItemText(row, 1, NULL);
						}

						break;
					}
				}
				
				CGridCellCheck *pCheckCell = (CGridCellCheck *)m_TccList.GetCell(row, 2);
				if(pCheckCell)
					pCheckCell->SetCheck(bTccValidate);
			}
		}
	
	m_TccList.Refresh();
}

void CTccCommandInputDlg::OnBtnClickedTccCmdTimeendRadio2()
{
	m_tmEnd.EnableWindow(FALSE);
}

void CTccCommandInputDlg::OnBtnClickedTccCmdTimeendRadio()
{
	m_tmEnd.EnableWindow(TRUE);
}

void CTccCommandInputDlg::OnBtnClickedTccCmdTimebeginRadio2()
{
	m_tmBegin.EnableWindow(FALSE);
}

void CTccCommandInputDlg::OnBtnClickedTccCmdTimebeginRadio()
{
	m_tmBegin.EnableWindow(TRUE);
}

void CTccCommandInputDlg::OnBtnClickedLimit2()
{
	int i=1;
	if(m_Limit[i].GetCheck())
	{
		if(m_LimitVal[i -1].GetCurSel() == CB_ERR)
		{
			MessageBox("上一趟限速没有选择!");
			m_Limit[i].SetCheck(BST_UNCHECKED);
			return;
		}

		m_LimitVal[i].EnableWindow(TRUE);
		m_TrainID[i].EnableWindow(TRUE);

		m_Limit[i+1].EnableWindow(TRUE);
	}
	else
	{
		for(int j=i; j<5; j++)
		{
			if(j != i)
			{
				m_Limit[j].SetCheck(FALSE);
				m_Limit[j].EnableWindow(FALSE);
			}

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
			m_TrainID[j].SetWindowText("");
		}
	}
}

void CTccCommandInputDlg::OnBtnClickedLimit3()
{
	int i=2;
	if(m_Limit[i].GetCheck())
	{
		if(m_LimitVal[i -1].GetCurSel() == CB_ERR)
		{
			MessageBox("上一趟限速没有选择!");
			m_Limit[i].SetCheck(BST_UNCHECKED);
			return;
		}

		m_LimitVal[i].EnableWindow(TRUE);
		m_TrainID[i].EnableWindow(TRUE);

		m_Limit[i+1].EnableWindow(TRUE);
	}
	else
	{
		for(int j=i; j<5; j++)
		{
			if(j != i)
			{
				m_Limit[j].SetCheck(FALSE);
				m_Limit[j].EnableWindow(FALSE);
			}

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
			m_TrainID[j].SetWindowText("");
		}
	}
}

void CTccCommandInputDlg::OnBtnClickedLimit4()
{
	int i=3;
	if(m_Limit[i].GetCheck())
	{
		if(m_LimitVal[i -1].GetCurSel() == CB_ERR)
		{
			MessageBox("上一趟限速没有选择!");
			m_Limit[i].SetCheck(BST_UNCHECKED);
			return;
		}

		m_LimitVal[i].EnableWindow(TRUE);
		m_TrainID[i].EnableWindow(TRUE);

		m_Limit[i+1].EnableWindow(TRUE);
	}
	else
	{
		for(int j=i; j<5; j++)
		{
			if(j != i)
			{
				m_Limit[j].SetCheck(FALSE);
				m_Limit[j].EnableWindow(FALSE);
			}

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
			m_TrainID[j].SetWindowText("");
		}
	}
}

void CTccCommandInputDlg::OnBtnClickedLimit5()
{
	int i=4;
	if(m_Limit[i].GetCheck())
	{
		if(m_LimitVal[i -1].GetCurSel() == CB_ERR)
		{
			MessageBox("上一趟限速没有选择!");
			m_Limit[i].SetCheck(BST_UNCHECKED);
			return;
		}

		m_LimitVal[i].EnableWindow(TRUE);
		m_TrainID[i].EnableWindow(TRUE);

		//m_Limit[i+1].EnableWindow(TRUE);
	}
	else
	{
		for(int j=i; j<5; j++)
		{
			if(j != i)
			{
				m_Limit[j].SetCheck(FALSE);
				m_Limit[j].EnableWindow(FALSE);
			}

			m_LimitVal[j].SetCurSel(-1);
			m_LimitVal[j].EnableWindow(FALSE);
			m_TrainID[j].EnableWindow(FALSE);
			m_TrainID[j].SetWindowText("");
		}
	}
}

void CTccCommandInputDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	if( !UpdateData(TRUE) )
		return ;

	RAW_STOCK_COMMAND cmd = m_cmd;
	//cmd.nConsoleID = m_cmd.nConsoleID;
	//cmd.raw_sysid = m_cmd.raw_sysid;

	CString prompt("内容不正确!");
	BOOL bSuccess = FALSE;
	do {
		cmd.bGenCancelCommand = m_bGenCancelCommand;

		int nIndex = m_CmdType.GetCurSel();
		if(CB_ERR == nIndex)
		{
			prompt.Format("请选择命令类型!");
			break;
		}
		int nCmdType = m_CmdType.GetItemData(nIndex);
		cmd.bFullLimit = (ABSAPP_FULL_LIMIT == nCmdType);

		// 命令号
		cmd.nCmdID = (cmdid_t)m_nCmdID;
		if(0 == m_cmd.nCmdID && !m_pInfoProvider->IsValidCommandID(cmd.nCmdID))
		{	//新输入的命令号必须在合法范围内，已经有命令号的不检查合法性，以便不同的GPC终端分别进行输入和检验
			prompt.Format("限速命令号必须在范围%d--%d内", 
				m_pInfoProvider->GetMinCommandID(), m_pInfoProvider->GetMaxCommandID());
			break;
		}

		// 开始站
		nIndex = m_StaBegin.GetCurSel();
		if(CB_ERR == nIndex)
		{
			prompt.Format("请选择开始站!");
			break;
		}
		cmd.nStaID[0] = (stano_t)m_StaBegin.GetItemData(nIndex);

		// 结束站
		nIndex = m_StaEnd.GetCurSel();
		DWORD dw = 0;
		if(CB_ERR != nIndex)
		{
			dw = m_StaEnd.GetItemData(nIndex);
		}
		else if(cmd.bFullLimit)
		{
			prompt.Format("区间全线限速,必须选择结束站!");
			break;
		}
		cmd.nStaID[1] = (stano_t)dw;

		// 线路号
		nIndex = m_AbsLine.GetCurSel();
		if(CB_ERR == nIndex)
		{
			prompt.Format("请选择线路!");
			break;
		}
		cmd.nLineID = (lineid_t)m_AbsLine.GetItemData(nIndex);

		// 公理标长链,低3字节还是公里标，高1字节的0-5作为KM的长链,6作为M的长链
		LONG nLongKmSel = m_StartLongKM.GetCurSel();
		LONG nLongKm = 0;
		if(nLongKmSel != CB_ERR)
			nLongKm = m_StartLongKM.GetItemData(nLongKmSel);
		LONG nLongMSel = m_StartLongM.GetCurSel();
		LONG nLongM = 0;
		if(nLongMSel != CB_ERR)
			nLongM = m_StartLongM.GetItemData(nLongMSel);
		
		cmd.nKmMark[0].val = m_nKmBegin * 1000 + m_nMBegin;// + (nLongKm<<24) + (nLongM<<30);
		cmd.nKmMark[0].km_cl = nLongKm;
		cmd.nKmMark[0].m_cl  = nLongM;

		nLongKmSel = m_EndLongKM.GetCurSel();
		nLongKm = 0;
		if(nLongKmSel != CB_ERR)
			nLongKm = m_EndLongKM.GetItemData(nLongKmSel);
		nLongMSel = m_EndLongM.GetCurSel();
		nLongM = 0;
		if(nLongMSel != CB_ERR)
			nLongM = m_EndLongM.GetItemData(nLongMSel);
		
		cmd.nKmMark[1].val = m_nKmEnd * 1000 + m_nMEnd;// + (nLongKm<<24) + (nLongM<<30);
		cmd.nKmMark[1].km_cl = nLongKm;
		cmd.nKmMark[1].m_cl  = nLongM;
		//长链end
		if(cmd.nKmMark[0] == cmd.nKmMark[1]
			&& (cmd.nKmMark[0].val == 0 || cmd.nKmMark[0].val == 9999999)
			&& !cmd.bFullLimit && BLOCK_LIMIT != nCmdType)
		{
			prompt.Format("区间全限速不能直接输入公里标，请重新选择命令类型!");
			break;
		}

		// 开始时间
		if(0==m_bTimeBegin)
		{
			CTime ct;
			m_tmBegin.GetTime(ct);
			cmd.nTime[0] = ct.GetTime() - ct.GetSecond();
			cmd.bStartNow = FALSE;
		}
		else
		{
			cmd.nTime[0] = 0;
			cmd.bStartNow = TRUE;
		}

		// 结束时间
		if(0==m_bTimeEnd)
		{
			CTime ct;
			m_tmEnd.GetTime(ct);
			cmd.nTime[1] = ct.GetTime() - ct.GetSecond();
			cmd.bEndless = FALSE;
		}
		else
		{
			cmd.nTime[1] = 0;
			cmd.bEndless = TRUE;
		}

		// 限速值
		cmd.bCancelLimit = m_bCancelLimit;
		if(cmd.bCancelLimit)
		{
			cmd.train_cmd[0].nSpeed = cmd.train_cmd[0].nSpeedMaxVal = 45;
		}
		else
		{
			nIndex = m_LimitVal[0].GetCurSel();
			if(CB_ERR == nIndex)
			{
				prompt.Format("请选择限速!");
				break;
			}
			cmd.train_cmd[0].nSpeed = m_LimitVal[0].GetItemData(nIndex);
			if(cmd.train_cmd[0].nSpeed > 45)
				cmd.train_cmd[0].nSpeedMaxVal = cmd.train_cmd[0].nSpeed;
			else if(cmd.train_cmd[0].nSpeed > 0)
				cmd.train_cmd[0].nSpeedMaxVal = 45;
			m_TrainID[0].GetWindowText(cmd.train_cmd[0].szTrainID);

			int dw = 0;
			for(int i=1; i<5; i++)
			{
				if( m_Limit[i].GetCheck() )
				{
					nIndex = m_LimitVal[i].GetCurSel();
					dw = 0;
					if(CB_ERR != nIndex)
						dw = m_LimitVal[i].GetItemData(nIndex);
					cmd.train_cmd[i].nSpeed =  dw;
					if(cmd.train_cmd[i].nSpeed > 45)
						cmd.train_cmd[i].nSpeedMaxVal = cmd.train_cmd[i].nSpeed;
					else if(cmd.train_cmd[i].nSpeed > 0)
						cmd.train_cmd[i].nSpeedMaxVal = 45;
					m_TrainID[i].GetWindowText(cmd.train_cmd[i].szTrainID);
				}
				else
				{
					cmd.train_cmd[i] = RAW_STOCK_COMMAND::_TRAINCMD();
				}
			}

			for(i=0; i<4; i++)
			{
				if(0 != cmd.train_cmd[i].nSpeed
					&& cmd.train_cmd[i].nSpeed == cmd.train_cmd[i+1].nSpeed)
					break;
			}
			if(i != 4)
			{
				prompt.Format("不允许两趟连续限速值相同!");
				break;
			}
		}

		// 受令站列表
		cmd.tcc.clear();
		for(int i=m_TccList.GetFixedRowCount(); i<m_TccList.GetRowCount(); i++)
		{
			RAW_STOCK_COMMAND::_TCC_STRUCT tcc;
			tcc.nID = (stano_t)m_TccList.GetItemData(i, 0);
			tcc.nTccLineID = (lineid_t)m_TccList.GetItemData(i, 1);
			if(0 == tcc.nTccLineID)
			{
				CString str = m_TccList.GetItemText(i, 1);
				std::vector<lineid_t> lines;
				m_pInfoProvider->GetTccLineList(tcc.nID, cmd.nStaID[0], lines);
				for(int j=0; j<lines.size(); j++)
				{
					if(str == m_pInfoProvider->GetTccLineName(tcc.nID, lines[j]))
					{
						tcc.nTccLineID = lines[j];
						break;
					}
				}
			}
			if(0 == tcc.nTccLineID)
				break;
			
			//车站是否需要校验
			CGridCellCheck *pCell = NULL;
			pCell = (CGridCellCheck *)m_TccList.GetCell(i, 2);
			ASSERT(pCell != NULL);
			BOOL bChecked = pCell->GetCheck();
			tcc.bTccValidate = bChecked;

			cmd.tcc.push_back(tcc);
		}
		if(m_TccList.GetRowCount()!=i)
		{
			prompt.Format("请选择相邻站的线路!");
			break;
		}

		// 设置每趟限速的状态列表
		for(i=0; i<5; i++)
		{
			cmd.train_cmd[i].tcc.resize(cmd.tcc.size());
		}

		//检查受令站列表是否正确
		if(!cmd.bFullLimit)
		{
			std::vector<stano_t> tcclist;
			int builtin_id = m_pInfoProvider->GetLineID(cmd.tcc[0].nID, cmd.tcc[0].nTccLineID);
			if(0 < builtin_id)
			{
				tcclist = m_pInfoProvider->GetTccList(builtin_id, cmd.nKmMark[0], cmd.nKmMark[1]);
			
				for(i=0; i<tcclist.size(); i++)
				{
					for(int j=0; j<cmd.tcc.size() && tcclist[i] != cmd.tcc[j].nID; j++);
					if(j >= cmd.tcc.size())
						break;
				}
				if(i < tcclist.size() || tcclist.size() != cmd.tcc.size())
				{
					prompt = "根据公里标计算出的受令站列表: \r\n";
					CString sz;
					for(i=0; i<tcclist.size(); i++)
					{
						sz.Format("        站 %s(%d)\r\n", m_pInfoProvider->GetStationName(tcclist[i]), tcclist[i]);
						prompt += sz;
					}
					prompt += "\r\n与当前受令站列表不一致!\r\n";
					prompt += "请检查公里标或命令类型是否正确!";

					break;
				}
			}
		}

		bSuccess = TRUE;
	} while(0);
	if(!bSuccess)
	{
		MessageBox(prompt);
		return ;
	}


	if(m_bCheckCommand)
	{
		// 检查一致性
		if(cmd != m_cmd)
		{
			prompt.Format("命令不一致，原因：\n%s", GetDifferenceReason(cmd, m_cmd, m_pInfoProvider));
			MessageBox(prompt);
			return ;
		}
	}
	else
	{
		if(m_bManualChangeCommandID)
		{
			MessageBox("限速命令号修改后未确认，请点击\"人工确认\"或\"自动选号\"!");
			return;
		}

		if(!cmd.bCancelLimit && !m_bCmdIDConfirmed)
		{
			int nRet = MessageBox("服务器没有确认命令号是否有效, 是否要求服务器确认?", "命令号确认", MB_YESNO);
			//MessageBox期间, 确认消息是否已处理
			if(!m_bCmdIDConfirmed)
			{
				if(IDYES == nRet)
					RequestCommandID(m_nCmdID);
			}	
			return;
		}

		// 非取消命令, 保存命令号以备自动增长
		if(!cmd.bCancelLimit && m_cmd.nCmdID != cmd.nCmdID)
		{
			m_pInfoProvider->SaveCommandID(cmd.nCmdID);
		}

		m_cmd = cmd;
	}

	CDialog::OnOK();
}

void CTccCommandInputDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnCancel();
}

void  CTccCommandInputDlg::OnChangeTccCmdID()
{
	if(!UpdateData())
		return;

	if(!m_pInfoProvider->IsValidCommandID(m_nCmdID))
	{
		CString prompt;
		prompt.Format("限速命令号必须在范围%d--%d内", 
			m_pInfoProvider->GetMinCommandID(), m_pInfoProvider->GetMaxCommandID());
		MessageBox(prompt);

		m_CmdID.SetFocus();
		return;
	}

	if(m_bManualChangeCommandID)
	{
		m_CmdID.EnableWindow(FALSE);

		m_btnChangeCmdID.EnableWindow(FALSE);
		m_btnAutoComputeCmdID.EnableWindow(FALSE);

		RequestCommandID(m_nCmdID);
	}
	else
	{
		CPasswordInputDlg passwdinput(this);
		if(IDOK == passwdinput.DoModal())
		{
			if(passwdinput.GetPassword() != "1234")
			{
				MessageBox("密码不正确!");
				return;
			}

			m_bManualChangeCommandID = TRUE;
			m_btnChangeCmdID.SetWindowText("人工确认");

			m_CmdID.EnableWindow(TRUE);
			m_CmdID.SetFocus();
		}
	}
}

void CTccCommandInputDlg::OnBtnClickedAutoComputeCmdID()
{
   m_nCmdID = m_pInfoProvider->GetCommandID();
	CString str;
	str.Format("%d", m_nCmdID);
	m_CmdID.SetWindowText(str);

	m_CmdID.EnableWindow(FALSE);

	m_bManualChangeCommandID = FALSE;
	m_btnChangeCmdID.SetWindowText("修改命令号");

	m_btnAutoComputeCmdID.EnableWindow(FALSE);
	m_btnChangeCmdID.EnableWindow(FALSE);

	RequestCommandID(m_nCmdID);
}

void CTccCommandInputDlg::RequestCommandID(cmdid_t nCmdID)
{
	//正常限速时, 命令号变更由服务器来确认命令号是否有效
	int who_confirm_command_id = ITccAdviseSink::SELF_CONFIRM_COMMAND_ID;

	dwServerConfirmCookie ++;
	m_bCmdIDConfirmed = false;

	m_pAdviseSink->ConfirmCommandID(m_pInfoProvider->GetConsoleID(), m_nCmdID, 
		m_pInfoProvider->GetMinCommandID(), m_pInfoProvider->GetMaxCommandID(),
		who_confirm_command_id, dwServerConfirmCookie, GetSafeHwnd());
	
	if(ITccAdviseSink::SELF_CONFIRM_COMMAND_ID == who_confirm_command_id)
		m_bCmdIDConfirmed = true;
}

LRESULT CTccCommandInputDlg::OnServerConfirmCommandID(WPARAM wParam, LPARAM lParam)
{
	// 服务器已经确认过
	if(m_bCmdIDConfirmed)
		return FALSE;

	bool bValid = (bool)wParam;
	SERVER_CONFIRM_COMMAND_ID_STRUCT& ds = *(SERVER_CONFIRM_COMMAND_ID_STRUCT*)lParam;

	if(ds.dwCookie != dwServerConfirmCookie)
		return FALSE;

	if(ds.bValid)
	{
		m_bCmdIDConfirmed = true;

		m_btnChangeCmdID.EnableWindow(TRUE);
		m_bManualChangeCommandID = FALSE;
		m_btnChangeCmdID.SetWindowText("修改命令号");
		m_btnAutoComputeCmdID.EnableWindow(TRUE);
	}
	else
	{
		if(m_bManualChangeCommandID)
		{
			MessageBox("人工修改的命令号已经存在!");
			
			m_btnChangeCmdID.EnableWindow(TRUE);
			m_btnAutoComputeCmdID.EnableWindow(TRUE);

			m_CmdID.EnableWindow(TRUE);
			m_CmdID.SetFocus();

			return FALSE;
		}

		// 服务器确认不成功, 计算一个新的命令号
		if(m_nCmdID == m_pInfoProvider->GetCommandID())
			m_pInfoProvider->NextCommandID();

       m_nCmdID = m_pInfoProvider->GetCommandID();
	   CString str;
	   str.Format("%d", m_nCmdID);
	   m_CmdID.SetWindowText(str);

	   RequestCommandID(m_nCmdID);
	}

	return TRUE;
}

void CTccCommandInputDlg::OnCbnSelchangeTccCmdType()
{
	INT nIndex = m_CmdType.GetCurSel();
	if(CB_ERR == nIndex)
	{
		m_StaBegin.SetCurSel(-1);
		m_StaBegin.EnableWindow(FALSE);
		m_StaEnd.SetCurSel(-1);
		m_StaEnd.EnableWindow(FALSE);
		m_AbsLine.SetCurSel(-1);
		m_AbsLine.EnableWindow(FALSE);
		return ;
	}

	m_nKmBegin = m_nMBegin = m_nKmEnd = m_nMEnd = 0;

	switch(m_CmdType.GetItemData(nIndex))
	{
	default:
		ASSERT(FALSE);
		break;
	case STATION_LIMIT:	// 站内限速
		m_StaBegin.EnableWindow(TRUE);
		m_StaEnd.SetCurSel(-1);
		m_StaEnd.EnableWindow(FALSE);
		OnCbnSelchangeTccCmdSta();
		m_AbsLine.EnableWindow(TRUE);
		m_KmBegin.EnableWindow(TRUE);
		m_MBegin.EnableWindow(TRUE);
		m_KmEnd.EnableWindow(TRUE);
		m_MEnd.EnableWindow(TRUE);

		m_StartLongKM.EnableWindow(TRUE);
		m_StartLongM.EnableWindow(TRUE);
		m_EndLongKM.EnableWindow(TRUE);
		m_EndLongM.EnableWindow(TRUE);
		break;
	case ABSAPP_LIMIT:	// 区间限速
		m_StaBegin.EnableWindow(TRUE);
		m_StaEnd.EnableWindow(TRUE);
		OnCbnSelchangeTccCmdSta();
		m_AbsLine.EnableWindow(TRUE);
		m_KmBegin.EnableWindow(TRUE);
		m_MBegin.EnableWindow(TRUE);
		m_KmEnd.EnableWindow(TRUE);
		m_MEnd.EnableWindow(TRUE);

		m_StartLongKM.EnableWindow(TRUE);
		m_StartLongM.EnableWindow(TRUE);
		m_EndLongKM.EnableWindow(TRUE);
		m_EndLongM.EnableWindow(TRUE);
		break;
	case ABSAPP_FULL_LIMIT:	// 区间全线限速
		m_StaBegin.EnableWindow(TRUE);
		m_StaEnd.EnableWindow(TRUE);
		OnCbnSelchangeTccCmdSta();
		m_AbsLine.EnableWindow(TRUE);
		m_KmBegin.EnableWindow(FALSE);
		m_MBegin.EnableWindow(FALSE);
		m_KmEnd.EnableWindow(FALSE);
		m_MEnd.EnableWindow(FALSE);

		m_StartLongKM.EnableWindow(FALSE);
		m_StartLongKM.SetCurSel(-1);
		m_StartLongM.EnableWindow(FALSE);
		m_StartLongM.SetCurSel(-1);
		m_EndLongKM.EnableWindow(FALSE);
		m_EndLongKM.SetCurSel(-1);
		m_EndLongM.EnableWindow(FALSE);
		m_EndLongM.SetCurSel(-1);
		break;
	case BLOCK_LIMIT:
		m_StaBegin.EnableWindow(TRUE);
		m_StaEnd.SetCurSel(-1);
		m_StaEnd.EnableWindow(FALSE);
		OnCbnSelchangeTccCmdSta();
		m_AbsLine.EnableWindow(TRUE);
		m_KmBegin.EnableWindow(FALSE);
		m_MBegin.EnableWindow(FALSE);
		m_KmEnd.EnableWindow(FALSE);
		m_MEnd.EnableWindow(FALSE);

		m_nKmBegin = 0;
		m_nMBegin = 0;
		m_nKmEnd = 9999;
		m_nMEnd = 999;

		m_StartLongKM.EnableWindow(FALSE);
		m_StartLongKM.SetCurSel(-1);
		m_StartLongM.EnableWindow(FALSE);
		m_StartLongM.SetCurSel(-1);
		m_EndLongKM.EnableWindow(FALSE);
		m_EndLongKM.SetCurSel(-1);
		m_EndLongM.EnableWindow(FALSE);
		m_EndLongM.SetCurSel(-1);
		break;
	}
	UpdateData(FALSE);
}

void CTccCommandInputDlg::InitListCtrl(CGridCtrl& grid)
{
	struct COLINFO { TCHAR* label;	INT width;	INT fmt; }
	colinfo[] =
	{
		{"受令站", 100, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"线路",   140, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
		{"需校验", 80, DT_LEFT | DT_WORDBREAK|DT_NOPREFIX },
	};

	CRect rcClient;
	grid.GetClientRect(&rcClient);

	try{
		grid.SetRowCount(1);                //行
		grid.SetColumnCount(sizeof(colinfo)/sizeof(colinfo[0]));             //列
		grid.SetFixedRowCount(1);        //固定行
		grid.SetFixedColumnCount(0);     //固定列		    
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

void CTccCommandInputDlg::OnSelCancelLimit() 
{
 
  if(m_CancelLimit.GetCheck())
	  MessageBox("请注意：,取消限速命令的“限速命令号”必须和前发相应的设置限速命令的号码一致!");;
}

};
