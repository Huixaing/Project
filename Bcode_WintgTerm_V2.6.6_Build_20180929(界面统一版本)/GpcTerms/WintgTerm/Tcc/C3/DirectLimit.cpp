// DialogDirectTccCommand.cpp : 实现文件
//

#include "stdafx.h"
#include "DirectLimit.h"
//#include "sysruninfo.h"
//#include "tcc_dlg.h"
//#include "tcc_start.h"
#include "TccCommandSendDlg.h"
#include "StockCommandInputDlg.h"
#include "TccCommandSketchMapDlg.h"

namespace TCC_C3
{
// CDirectLimit 对话框

IMPLEMENT_DYNAMIC(CDirectLimit, CDialog)
CDirectLimit::CDirectLimit(ITccClientInfoProvider *pInfoProvider, ITccAdviseSink *pAdviseSink, CWnd* pParent /*=NULL*/)
	: CDialog(CDirectLimit::IDD, pParent)
	, m_pInfoProvider(pInfoProvider)
	, m_pAdviseSink(pAdviseSink)
	, m_bCommandChecked(FALSE)
{
}

CDirectLimit::~CDirectLimit()
{
}

void CDirectLimit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TCC_COMMAND_NEW,   m_btnNew);
	DDX_Control(pDX, IDC_TCC_COMMAND_EDIT,  m_btnEdit);
	DDX_Control(pDX, IDC_TCC_COMMAND_VALID, m_btnValid);
	DDX_Control(pDX, IDC_TCC_COMMAND_SEND,  m_btnSend);

	DDX_Control(pDX, IDC_TCC_CMD_STA, m_TccList);

	DDX_Text(pDX, IDC_EDIT_TCCCMDCONTENT, m_szCmdString);
}

BEGIN_MESSAGE_MAP(CDirectLimit, CDialog)
	ON_BN_CLICKED(IDC_TCC_COMMAND_NEW, OnNew)
	ON_BN_CLICKED(IDC_TCC_COMMAND_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_TCC_COMMAND_VALID, OnValid)
	ON_BN_CLICKED(IDC_TCC_COMMAND_SEND, OnSend)
	ON_BN_CLICKED(IDC_LS_SKETCHMAP, OnSketchMap)
END_MESSAGE_MAP()

// CDirectLimit 消息处理程序
BOOL CDirectLimit::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if(NULL == m_pInfoProvider)
	{
		MessageBox("没有安装TccClientInfoProvider接口!");
		return FALSE;
	}
	ITccAdviseSink *pAdviseSink = m_pAdviseSink;
	if(NULL == pAdviseSink)
	{
		MessageBox("没有安装TccAdviseSink接口!");
		return FALSE;
	}
	if(!TccInitialize(m_pInfoProvider))
		return FALSE;

	//OnNew();    // 避免对话框弹出后不在父窗口中间
	m_btnEdit.EnableWindow(FALSE );
	m_btnValid.EnableWindow(FALSE);
	m_btnSend.EnableWindow(FALSE);
	GetDlgItem(IDC_LS_SKETCHMAP)->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDirectLimit::OnNew()
{
	CStockCommandInputDlg dlg(NULL, FALSE, m_pInfoProvider, m_pAdviseSink, this);
	if(IDOK == dlg.DoModal())
	{
		m_cmd = *dlg.GetCommand();

		m_szCmdString = m_cmd.toString(m_pInfoProvider);
		UpdateData(FALSE);

		//m_btnEdit.EnableWindow(TRUE );
		m_btnValid.EnableWindow(TRUE);
		m_btnSend.EnableWindow(FALSE);
		GetDlgItem(IDC_LS_SKETCHMAP)->EnableWindow(TRUE);
		m_TccList.ResetContent();
	}
	else
	{
		m_szCmdString = "";
		UpdateData(FALSE);

		//m_btnEdit.EnableWindow(TRUE);
		m_btnValid.EnableWindow(FALSE);
		m_btnSend.EnableWindow(FALSE);
		GetDlgItem(IDC_LS_SKETCHMAP)->EnableWindow(FALSE);
	}
	m_bCommandChecked = FALSE;
}

void CDirectLimit::OnEdit()
{
	CStockCommandInputDlg dlg(&m_cmd, FALSE, m_pInfoProvider, m_pAdviseSink, this);
	if(IDOK == dlg.DoModal())
	{
		m_cmd = *dlg.GetCommand();
		m_szCmdString = m_cmd.toString(m_pInfoProvider);
		UpdateData(FALSE);

		m_btnEdit.EnableWindow(TRUE);
		m_btnValid.EnableWindow(TRUE);
		m_btnSend.EnableWindow(FALSE);
		GetDlgItem(IDC_LS_SKETCHMAP)->EnableWindow(TRUE);
		m_TccList.ResetContent();
	}
}

void CDirectLimit::OnValid()
{
	CStockCommandInputDlg dlg(&m_cmd, TRUE, m_pInfoProvider, m_pAdviseSink, this);
	if(IDOK == dlg.DoModal())
	{
		m_cmd = *dlg.GetCommand();
		m_szCmdString = m_cmd.toString(m_pInfoProvider);
		UpdateData(FALSE);

		m_btnEdit.EnableWindow(FALSE);
		m_btnValid.EnableWindow(FALSE);
		m_btnSend.EnableWindow(TRUE);

		m_bCommandChecked = TRUE;

		int nIndex;
		for(int i=0; i<m_cmd.tcc.size(); i++)
		{
			nIndex = m_TccList.AddString( m_pInfoProvider->GetStationName(m_cmd.tcc[i].nID) );
			ASSERT(nIndex != CB_ERR);
			m_TccList.SetItemData(nIndex, m_cmd.tcc[i].nID);
		}
	}
}

void CDirectLimit::OnSend()
{
	if(!m_bCommandChecked)
	{
		MessageBox("命令没有校验!");
		return ;
	}
	int nIndex = m_TccList.GetCurSel();
	if(CB_ERR == nIndex)
	{
		MessageBox("没有选择发送的站!");
		return ;
	}
	stano_t nTccID = m_TccList.GetItemData(nIndex);

	m_cmd.nConsoleID = m_pInfoProvider->GetConsoleID();
	m_cmd.nCreatorID = m_pInfoProvider->GetCurrentEntityID();
	m_cmd.nCheckorID = m_pInfoProvider->GetCurrentEntityID();

	STOCK_COMMAND stock_cmd[1];
	int num = m_cmd.Decompose(stock_cmd, sizeof(stock_cmd)/sizeof(stock_cmd[0]));
	if(num)
	{
		TCC_COMMAND tcc_cmd;
		if(stock_cmd[0].GetTccCommand(&tcc_cmd, nTccID))
		{
			STOCK_COMMAND stockcmd = stock_cmd[0];
			stockcmd.tcc.clear();

			for(int i=0; i<stock_cmd[0].tcc.size(); i++)
			{
				if(stock_cmd[0].tcc[i].nID == nTccID)
				{
					stockcmd.tcc.push_back(stock_cmd[0].tcc[i]);
					break;
				}
			}
			CTccCommandSendDlg dlg(tcc_cmd, CTccCommandSendDlg::TCC_COMMAND_DIRECT, &stockcmd, m_pInfoProvider, m_pAdviseSink);
			dlg.DoModal();
		}
	}
}

void CDirectLimit::OnSketchMap()
{
	if(GetLineStruct(m_cmd.nLineID)==NULL)
	{
		MessageBox("侧线限速没有示意图","提示",MB_OK|MB_ICONEXCLAMATION|MB_ICONWARNING);
		return;
	}
	if(m_cmd.nLineID)
	{
		CTccCommandSketchMapDlg dlg(m_pInfoProvider, this);
		dir_t dir = DIR_UNKNOWN;
		switch (m_cmd.nDirection)
		{
		case 1:
			dir = DIR_DOWN;
			break;
		case 2:
			dir = DIR_UP;
			break;
		case 3:
			dir = DIR_DUAL;
			break;
		default:
			dir = DIR_UNKNOWN;
			break;
		}
		dlg.m_SketchMap.dir_limit=dir;
		dlg.SetKmMark(m_cmd.nLineID, m_cmd.nKmMark[0], m_cmd.nKmMark[1]);
		dlg.DoModal();
	}
}


}