// UserListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "function.h"
#include "tgmsg.h"
#include "messages.h"
#include "memshare.h"
#include "resource.h"
#include "widgets.h"
#include "tgdocument.h"
#include ".\TrainCrewTicket.h"
#import  <msxml.dll> named_guids

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrainCrewTicket dialog
CTrainCrewTicket::CTrainCrewTicket(CWnd* pParent, CTgDocument *pdoc, TRAIN_INDEX shift_index, CString trainid)
: CDialog(CTrainCrewTicket::IDD, pParent)
{
	m_pDoc=pdoc;
	m_shift_index=shift_index;
	m_trainid=trainid;
}

void CTrainCrewTicket::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTrainCrewTicket, CDialog)
	ON_WM_CLOSE()
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedRefresh)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainCrewTicket message handlers
BOOL CTrainCrewTicket::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString title=m_trainid+"--机务和客票信息";
	this->SetWindowText(title);
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage,0);
	m_pDoc->RequestTrainExpand_ENGINECREW_TICKE_FromOPMS(m_shift_index, 2, m_trainid);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTrainCrewTicket::OnBnClickedRefresh()
{
	CWnd* pWnd=GetDlgItem(IDC_EDIT2);
	if(pWnd)
	{
		pWnd->SetWindowText("");
	}
	pWnd=GetDlgItem(IDC_EDIT1);
	if(pWnd)
	{
		pWnd->SetWindowText("");
	}
	m_pDoc->RequestTrainExpand_ENGINECREW_TICKE_FromOPMS(m_shift_index, 2, m_trainid);
}

LRESULT CTrainCrewTicket::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	CLBTH::MsgStringMessage *pMsg= (CLBTH::MsgStringMessage  *)pMsgStruct->pmsg;
	const char *token=pMsg->GetToken();
	const char *content=pMsg->GetContent();
	if(token && content)
	{
		if(!stricmp(token,TOKEN_30_ACK_PASSENGER_TICKET_INFO))
		{
			CTG_TrainTicketList tk;
			if(TD_GetTicketInfoFromXML(tk,content,token))
			{
				CString text, tmp;
				for(int i=0; i<tk.m_listRecords.size(); i++)
				{
					text+="上车站:";
					text+=tk.m_listRecords[i].on_station_name;
					text+=" 下车站:";
					text+=tk.m_listRecords[i].down_station_name;
					text+=" 席别:";
					text+=tk.m_listRecords[i].seat;
					tmp.Format(" 售%d张", tk.m_listRecords[i].sale);
					text+=tmp;
					text+="\r\n";
				}

				CWnd* pWnd=GetDlgItem(IDC_EDIT2);
				if(pWnd)
				{
					pWnd->SetWindowText(text);
				}
			}
		}
		if(!stricmp(token,TOKEN_31_ACK_ENGINE_CREW_INFO))
		{
			CTG_EngineCrewList engine_crew;
			int opms_seq=0,opms_count=0;
			if(TD_LoadEngineCrewInfoFromXML(engine_crew,content,token,&opms_seq,&opms_count))
			{
				CString text, tmp;
				for(int i=0; i<engine_crew.m_listRecords.size(); i++)
				{
					text+="车站:";
					text+=engine_crew.m_listRecords[i].station_name;

					if(engine_crew.m_listRecords[i].overwork_time>0)
					{
						CTime tm(engine_crew.m_listRecords[i].overwork_time);
						tmp.Format(" 超劳时间:%s", tm.Format("%Y年%m月%d日 %H:%M:%S"));
						text+=tmp;
					}
					text+="\r\n";
				}

				CWnd* pWnd=GetDlgItem(IDC_EDIT1);
				if(pWnd)
				{
					pWnd->SetWindowText(text);
				}
			}
		}
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

void CTrainCrewTicket::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	DestroyWindow();
}

void CTrainCrewTicket::PostNcDestroy() 
{
	delete this;
}

