// MyStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "oscalls.h"
#include "memshare.h"
#include "resource.h"
#include "MyStatusBar.h"
#include "function.h"
#include "db_msgproc.h"
#include "icommtask.h"
#include "tgview/tgview.h"
#include "tgx.h"

#include "TGPspData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyStatusBar
#define DEFAULT_PANEL_BACK_COLOR RGB(240,240,240)
#define DEFAULT_PANEL_FRONT_COLOR RGB(0,0,0)
CMyStatusBar::CMyStatusBar()
{
	for(int i=0;i<32;i++)
	{
		m_aPanelExtInfo[i].m_bBoldFont=FALSE;
		m_aPanelExtInfo[i].m_cBackColor=DEFAULT_PANEL_BACK_COLOR;
		m_aPanelExtInfo[i].m_cFrontColor=DEFAULT_PANEL_FRONT_COLOR;
	}
	m_nFlashFlag=0;
}

CMyStatusBar::~CMyStatusBar()
{
}

BEGIN_MESSAGE_MAP(CMyStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CMyStatusBar)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
//	ON_WM_PAINT()
	//}}AFX_MSG_MAP
 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyStatusBar message handlers
int CMyStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
 	VERIFY(SetTimer(101,1000, NULL) != 0);    //timer
 
	return 0;
}

const char *GetSelectedTrainString()
{
	static CString panel_str;

	CMDIChildWnd *pchild=((CMDIFrameWnd *)AfxGetMainWnd())->MDIGetActive();
	if(!pchild)
		return NULL;
	CView *pv=pchild->GetActiveView();
	if(!pv)
		return NULL;
	
	if(!pv->IsKindOf(RUNTIME_CLASS(CTgView)))
	{
		return NULL;
	}
	CTgDocument *pdoc=(CTgDocument *)pv->GetDocument();
	if(!pdoc)
		return NULL;
	CTG_LogicObject *pobj=pdoc->GetCurrentActiveObject();
	if(!pobj || pobj->GetObjectType()!=TG_OBJ_TRAIN)
		return NULL;
	CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
	
	panel_str="";
	panel_str+=ptrain->GetTrainID();
    
	// G05
	// flag:1 TRACT, 2:ATTACH, 3: ALL
	const CString engine_info=GetTrainEngineDesc(ptrain,3); 
	
	CString str_temp;
	panel_str+="(";
	if(!engine_info.IsEmpty())
	{
		panel_str+="机车:";
		panel_str+=engine_info;
	}
	
	panel_str+="辆数:";
	str_temp.Format("%d",(ptrain->GetTrainInfo()->m_data.status>>16&0xFFFF));
	panel_str+=str_temp;

	panel_str+="换长:";
	long train_length=ptrain->GetTrainInfo()->m_data.train_length;
	str_temp.Format("%d.%d", train_length/10, train_length%10);
	panel_str+=str_temp;
	
	panel_str+="总重:";
	long train_weight=ptrain->GetTrainInfo()->m_data.train_weight;
	str_temp.Format("%d.%d", train_weight/10, train_weight%10);
	panel_str+=str_temp;
	panel_str+=")";
	
	CTG_TrainRecord *prec0=ptrain->GetTrainRecord(0);
	CTG_TrainRecord *prec1=ptrain->GetTrainRecord(ptrain->GetTrainRecordCount()-1);
	if(prec0 && prec1)
	{
		CString stastr;
		stastr.Format(" %s-%s ",gpcdef.GetStationName(prec0->GetStationNo()),gpcdef.GetStationName(prec1->GetStationNo()));
		panel_str+=stastr;
	}
	return (const char *)panel_str;
}
const char *GetSyncStatusString()
{
	CMDIChildWnd *pchild=((CMDIFrameWnd *)AfxGetMainWnd())->MDIGetActive();
	if(!pchild)
		return NULL;
	CView *pv=pchild->GetActiveView();
	if(!pv)
		return NULL;
	if(pv->IsKindOf(RUNTIME_CLASS(CTgView)))
	{
		return ((CTgView *)pv)->GetSyncStatusInfo();
	}
	return NULL;
}
const char *GetTGCommStatusString()
{
	CMDIChildWnd *pchild=((CMDIFrameWnd *)AfxGetMainWnd())->MDIGetActive();
	if(!pchild)
		return NULL;
	CView *pv=pchild->GetActiveView();
	if(!pv)
		return NULL;
	if(pv->IsKindOf(RUNTIME_CLASS(CTgView)))
	{
		return ((CTgView *)pv)->GetCommStatusInfo();
	}
	return NULL;
}

void CMyStatusBar::OnTimer(UINT nIDEvent) 
{
	if(101 != nIDEvent)
		return;
	
	 
	m_nFlashFlag=1-m_nFlashFlag;
	
	static CString strVersion = "";
	if(strVersion.IsEmpty())
	{
		CTgX tg;
		if(tg.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0,-1,100,100), this, 1500, NULL))
			strVersion = "版本:WinTG:2.6.5 TG"+tg.GetTgVersion();
		strVersion =strVersion+" BaseMsg:"+MSG_DEF_VERSION;
		GpcLogTrainMessage(0, 0, "当前运行图: %s", strVersion);
	}
	if(!strVersion.IsEmpty() && gpcdef.IsMSSAlarmEnable())
		SetMSVersionAlarm(LPCTSTR(strVersion));

	SetPaneText(CommandToIndex(IDS_SYNC_STATUS), strVersion);

	const char *ptmpstr=GetSyncStatusString();
	if(ptmpstr)
 		SetPaneText(CommandToIndex(ID_ADJUST_STATUS),ptmpstr);
	else
		SetPaneText(CommandToIndex(ID_ADJUST_STATUS),"");
	
	ptmpstr=GetSelectedTrainString();
	if(ptmpstr)
 		SetPaneText(CommandToIndex(IDS_TRAINSSELECTED),ptmpstr);
	else
	    SetPaneText(CommandToIndex(IDS_TRAINSSELECTED),"");

	ptmpstr=GetTGCommStatusString();
	int commstatus=GetCommTaskStatus();
	if(ptmpstr)
	{
		if(stricmp(ptmpstr,"服务连接") == 0)
			SetPanelColor(CommandToIndex(IDS_SCROLL_TEXT),DEFAULT_PANEL_BACK_COLOR,DEFAULT_PANEL_FRONT_COLOR,FALSE);
		else 
		{
			if (m_nFlashFlag==1)
				SetPanelColor(CommandToIndex(IDS_SCROLL_TEXT),RGB(255,0,0),RGB(255,255,255),FALSE);	
			else
				SetPanelColor(CommandToIndex(IDS_SCROLL_TEXT),DEFAULT_PANEL_BACK_COLOR,DEFAULT_PANEL_FRONT_COLOR,FALSE);
			Invalidate();
		}
		SetPaneText(CommandToIndex(IDS_SCROLL_TEXT),ptmpstr) ;
 		return;
	}
	else
	if(commstatus<2)
	{
		SetPaneText(CommandToIndex(IDS_SCROLL_TEXT), "网络中断");
 		return;
	}
	else
	if(commstatus==2) 
	{
  		SetPaneText(CommandToIndex(IDS_SCROLL_TEXT), "网络连接 ");
 		return;
	}
	else
	if(commstatus==3) 
	{
  		SetPaneText(CommandToIndex(IDS_SCROLL_TEXT), "联网成功! ");
 		return;
	}

	CStatusBar::OnTimer(nIDEvent);
}

void CMyStatusBar::OnDestroy() 
{
	CStatusBar::OnDestroy();
	KillTimer(101);
}

void CMyStatusBar::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CStatusBar::OnActivate(nState, pWndOther, bMinimized);
}

void CMyStatusBar::_SetPaneText(UINT index, LPCTSTR text)
{
	this->SetPaneText(CommandToIndex(index),text);
	return;
}

void CMyStatusBar::SetOpeator()
{
	int user = gpcdef.GetUserIdentify();
	switch(user)
	{
	case GPC_SCHEDULE: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"计划员");
		break;
	case GPC_DISPATCHER: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"调度员");
		break;
	case GPC_OPERATOR: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"助调员");
		break;
	case GPC_DISPATCHERS: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"调度长");
		break;
	case GPC_SCREEN_LAYOUT: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"背投");
		break;
	case GPC_TRAIN: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"培训");
		break;
	case GPC_SERVICE: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"q");
		break;
	case GPC_MAINTAIN: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"电务维修");
		break;


	case GPC_PLOT: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"绘图");
		break;
	case GPC_MACHINE: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"机调员");
		break;
	case GPC_SUPER_OPER: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"助调(施工)员");
		break;
	case GPC_SCREEN_TG:
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"统计员");
		break;

	case GPC_AFFAIR_SERVER: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"");
		break;
	case GPC_SUPER_DISPATCH: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"");
		break;

	case GPC_OTHER_DISPATCH: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"");
		break;
	case GPC_SUPER_MAINTAIN: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"系统维护");
		break;
	case GPC_TEST: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"系统调试");
		break;
	case 18: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"施工调");
		break;
	default: 
		SetPaneText(CommandToIndex(IDS_OPERATOR_IS_WHO),"");
		break;
	}
}

void CMyStatusBar::DrawItem(LPDRAWITEMSTRUCT lpdi)
{
	//if(lpdi->itemID == 3)
    
		CFont mfont,*poldfont;

		CDC dc;
		dc.Attach(lpdi->hDC);

		CBrush brush(m_aPanelExtInfo[lpdi->itemID].m_cBackColor);
		dc.FillRect(&lpdi->rcItem,&brush);
		dc.SetTextColor(m_aPanelExtInfo[lpdi->itemID].m_cFrontColor);
		CFont *font=NULL;

		if(m_aPanelExtInfo[lpdi->itemID].m_bBoldFont)
		{
		 font= dc.GetCurrentFont();
		 if (font)
		 {
			LOGFONT lf;
			font->GetLogFont(&lf);
			lf.lfWeight = FW_BOLD;
			mfont.CreateFontIndirect(&lf);
			poldfont = dc.SelectObject(&mfont);
		 }
		}
		//poldfont = dc.SelectObject(&mfont);		
		UINT oldmode = dc.SetBkMode(TRANSPARENT);		
		dc.DrawText(GetPaneText(lpdi->itemID),&lpdi->rcItem,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		
		if(font != NULL)
		{
			dc.SelectObject(poldfont);
			mfont.DeleteObject();
		}
			
		dc.SetBkMode(oldmode);
		dc.Detach();
		
		return;
	

	//CMyStatusBar::DrawItem(lpdi);
}
void CMyStatusBar::SetPanelColor(UINT index,COLORREF bc,COLORREF fc,BOOL bold)
{
	if(index>=32)
		return;
	m_aPanelExtInfo[index].m_cBackColor=bc;
	m_aPanelExtInfo[index].m_cFrontColor=fc;
	m_aPanelExtInfo[index].m_bBoldFont=bold;
//	Invalidate();
}

void CMyStatusBar::SetPanelBoldText(UINT index,BOOL bold)
{
	if(index>=32)
		return;
	m_aPanelExtInfo[index].m_bBoldFont=bold;
}