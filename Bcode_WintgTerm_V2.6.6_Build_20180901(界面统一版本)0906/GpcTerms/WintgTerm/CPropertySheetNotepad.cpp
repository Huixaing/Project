// CPropertyPageNotepad.cpp : implementation file
//

#include "stdafx.h"
//#include "wintg.h"
#include <io.h>


#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"

#include "function.h"
#include "tgmsg.h"
#include "messages.h"
#include "memshare.h"
#include "function.h"
#include <baseMsg\msgid.h>
#include <baseMsg\gpc_msgid.h>
#include <BaseMsg\clbth.h>
#include "db_msgproc.h"
#include "TgDocument.h"
#include "TGPspData.h"
#include "CPropertysheetNotepad.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define WHOLE_DAY_SECOND 24*3600
#define EXT_TAB_WIDTH  450	

//根据文件全路径创建需要的目录
bool CreateRecursiveDir(char* filename);
/*********************************************
char* filename //格式如 c:\temp\test\1.txt
//c:\temp\test\
*******************************************/





CEdit* g_curEdit;//当前常用词汇要输入的编辑框

BOOL Query_Notebook(int con_id,unsigned long select_shift_id,int schd_type);					 //查询  记事本内容
BOOL Set_Notebook(int con_id,unsigned long select_shift_id,int schd_type);                     //设置  记事本内容


void f_PrintJISHI(CDC* pDC,CPrintInfo* pInfo,void* pVoid); 
void DrawTextInRect(CDC *pDC,const char *str,RECT *prect,int coltap,int rowtap,BYTE type);


BEGIN_MESSAGE_MAP(CPropertySheetNotepad, CPropertySheet)
	//{{AFX_MSG_MAP(CPropertySheetNotepad)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_LBN_DBLCLK(IDC_LISTALLWORDS, OnDblclkListallwords)
	ON_LBN_DBLCLK(IDC_LISTSCHDINFO, OnDblclkSchdInfo)
	ON_LBN_SELCHANGE(IDC_LISTSCHDINFO, OnDblclkSchdInfo)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_NOTIFY(TCN_SELCHANGE,IDC_WORD_TABCTRL,OnSelchangeTabctrl)
	ON_CBN_SELCHANGE(IDC_CBO_STARTTIME, OnSelchangeSbtEndtime)
	ON_CBN_SELCHANGE(IDC_CBO_ENDTIME, OnSelchangeSbtStarttime)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_TG_TRAIN_SELECTED,OnSchdTrainSelected)
	
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CPropertySheetNotepad, CPropertySheet)


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetNotepad message handlers
CPropertySheetNotepad::CPropertySheetNotepad(LPCTSTR pszCaption,CTgDocument *pDoc,int schdtype)
:CPropertySheet(pszCaption, NULL, 0)
,m_page_notepad(pDoc)
,m_page_phasenote_plan(pDoc)
,m_page_send_stplan(pDoc)
,m_page_shuaigua(pDoc)
,m_page_jiaoban(pDoc)
,m_page_shigu_outline(pDoc)
{
	m_pDoc=pDoc;
	m_focusedit = NULL;
	m_nStarttime=0;
	m_nEndtime=0;
	m_nPageCount=0;
	m_page_subao.SetDocument(pDoc);
	m_bModaless=FALSE;
	m_bLoadPassengerTrain=TRUE;
	m_bLoadFreightTrain=TRUE;
	m_nStationNo=NULL_STATION;
	m_nSectionNo=0;
	m_bEnableWordSelected=TRUE;
	m_bEnableStPlanWaring=FALSE;
	m_bEnableScheduleInfoSelected=TRUE;
	m_page_shuaigua.m_nScheduleType=schdtype;
}

void CPropertySheetNotepad::AddUserPage(int page_choice)
{
	CPropertyPage *page=GetUserPage(page_choice);
	if(!page)
		return;
	m_aPageID[m_nPageCount]=page_choice;
	m_nPageCount++;
	AddPage(page);

}

int CPropertySheetNotepad::GetCurActiveUserPage()
{
	int index=GetActiveIndex();
	if(index>=0)
		return m_aPageID[index];
	else
		return -1;
}

CPropertyPage *CPropertySheetNotepad::GetUserPage(int page_choice)
{
	if(page_choice==PHASENOTE_PAGE)
		return &m_page_notepad;
	else if(page_choice==PHASENOTE_SEND_PAGE)
		return &m_page_phasenote_plan;
	else if(page_choice==SHIGU_PAGE)
		return &m_page_shigu_outline;
	else if(page_choice==JIAOBAN_PAGE)
		return &m_page_jiaoban;
	else if(page_choice==STPLAN_PAGE)
		return &m_page_send_stplan;
	else if(page_choice==SUBAO_PAGE)
		return &m_page_subao;
	else if(page_choice==SHUAIGUA_PAGE)
		return &m_page_shuaigua;
	else
		return NULL;
}

void CPropertySheetNotepad::SelAllStplanRecEntity()
{
	if(!GetTGFunctionConfig()->IsStplanSelectAllEntity())
		return;

	CPropertyPage *pSendPlanDlg = GetUserPage(STPLAN_PAGE);
	if(NULL != pSendPlanDlg)
	{
		((CSendStationPlanDlgEx*)pSendPlanDlg)->SelAllStplanRecEntity();
	}
}

// 生成基本图信息
void CPropertySheetNotepad::SetBasicSchduleInfo()
{
	CPropertyPage *pSendPlanDlg = GetUserPage(STPLAN_PAGE);
	if(NULL != pSendPlanDlg)
	{
		((CSendStationPlanDlgEx*)pSendPlanDlg)->SetBasicSchduleInfo();
	}
}

void CPropertySheetNotepad::SendStplanOnlySelLIRC()
{
	if(!GetTGFunctionConfig()->IsStplanOnlySelectLirc())
		return;

	CPropertyPage *pSendPlanDlg = GetUserPage(STPLAN_PAGE);
	if(NULL != pSendPlanDlg)
	{
		((CSendStationPlanDlgEx*)pSendPlanDlg)->SendStplanOnlySelLIRC();
	}
}
void CPropertySheetNotepad::SendNoticeToDCmd()
{
	if(!GetTGFunctionConfig()->IsEnableSendPlanNoticeDCmd())
		return;

	CPropertyPage *pSendPlanDlg = GetUserPage(STPLAN_PAGE);
	if(NULL != pSendPlanDlg)
	{
		((CSendStationPlanDlgEx*)pSendPlanDlg)->SendNoticeToDCmd(1);
	}
}
#include "tg_binxml.h"
LRESULT CPropertySheetNotepad::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	if(!GetTGFunctionConfig()->IsEnableSendPlanNoticeDCmd())
		return TRUE;

	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if(pMsgStruct->msg_id!=MSGID_MsgStringMessage)
		return 0;

	CLBTH::MsgStringMessage *pMsg= (CLBTH::MsgStringMessage  *)pMsgStruct->pmsg;
	const char *token=pMsg->GetToken();
	const char *content=pMsg->GetContent();
	if(token && content)
	{
		if(	!stricmp(token, TOKEN_SECTIONPLAN_DCMD_RESULT))
		{
			CString text = content;
			//if(text.GetLength()>=230)
			//	GpcLogTrainMessage(0, 0, "%s", text.Left(230));
			//else
			//	GpcLogTrainMessage(0, 0, "%s", text);

			CString warning, id;
			int nResult = GetDCmdPlanResult(text, warning, id);
			if(warning.GetLength()>=230)
				GpcLogTrainMessage(0, 0, "%s", warning.Left(230));
			else
				GpcLogTrainMessage(0, 0, "%s", warning);
			GpcLogTrainMessage(0, 0, "%s result %d", id, nResult);


			if(nResult >= 0)
				OnRecievePlanDCmdResult(nResult, warning, id);
		}
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

void CPropertySheetNotepad::OnRecievePlanDCmdResult(int result, CString warning, CString id)
{
	int page=GetCurActiveUserPage();
	if(page!=STPLAN_PAGE)
	{
		this->SetActivePage(0);
	}
	
	CPropertyPage *pSendPlanDlg = GetUserPage(STPLAN_PAGE);
	if(NULL != pSendPlanDlg)
	{
		//if(((CSendStationPlanDlgEx*)pSendPlanDlg)->GetPlanDCmdId() != id)
		//{
		//	AfxMessageBox("请注意, 调度命令终端返回的唯一索引不正确");
		//	return;
		//}

		if(::IsWindow(pSendPlanDlg->m_hWnd) && !m_HideWnd)
		{
			if(result == 1)
			    ((CSendStationPlanDlgEx*)pSendPlanDlg)->OnOK();
			else
			{
				m_HideWnd=true;
				ShowWindow(SW_HIDE);
			}
		}
	}
}

CPropertySheetNotepad::CPropertySheetNotepad(int page_choice,LPCTSTR pszCaption, CTgDocument *pDoc,int schdtype)
:CPropertySheet(pszCaption, NULL, 0)
,m_page_notepad(pDoc)
,m_page_phasenote_plan(pDoc)
,m_page_send_stplan(pDoc)
,m_page_shuaigua(pDoc)
,m_page_jiaoban(pDoc)
,m_page_shigu_outline(pDoc)
{
	m_nPageCount=0;
	AddUserPage(page_choice);

	//设置编组为默认活动页
	SetActivePage(0);

	m_pDoc=pDoc;

	m_focusedit = NULL;
	m_nStarttime=0;
	m_nEndtime=0;
	m_page_subao.SetDocument(pDoc);
	m_bModaless=FALSE;
	m_bLoadPassengerTrain=TRUE;
	m_bLoadFreightTrain=TRUE;
	m_nStationNo=NULL_STATION;
	m_nSectionNo=0;
	m_bEnableWordSelected=TRUE;
	m_bEnableStPlanWaring=FALSE;
	m_bEnableScheduleInfoSelected=TRUE;
	m_page_shuaigua.m_nScheduleType=schdtype;
}

CPropertySheetNotepad::~CPropertySheetNotepad()
{

}
int CPropertySheetNotepad::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

BOOL CPropertySheetNotepad::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (pHandlerInfo == NULL)
	{
		if (nID == IDC_BUTTONADD)
		{
			if (nCode == CN_COMMAND) // Handles WM_COMMAND messages
			{
				char str[256];
				memset(str,0,256);
				if(m_newword.GetLine(0,str,254)<=0)
				{
					m_newword.SetFocus();
					return FALSE;
				}
				str[255]=0;

				CString datadir = gpcdef.GetDataDir();
				CStdioFile f_words;
				if(f_words.Open(datadir+"\\notepad.txt",CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
				{
					f_words.SeekToEnd();
					f_words.WriteString(str);
					f_words.WriteString("\n");
					m_listallwords.AddString(str);

					f_words.Close();
				}
				m_newword.SetWindowText("");
				m_newword.SetFocus();
				return TRUE;
			}


		}
		else if(nID == IDC_BUTTONDEL)
		{
			if (nCode == CN_COMMAND)
				// Handles WM_COMMAND messages
				// sent by the button.
			{
				int cur_sel_num,words_count;
				cur_sel_num = m_listallwords.GetCurSel();
				words_count = m_listallwords.GetCount();

				if(cur_sel_num<0)
					return FALSE;

				m_listallwords.DeleteString(cur_sel_num);

				if(cur_sel_num == words_count-1)//删除了最后一条记录
					m_listallwords.SetCurSel(0);
				else
					m_listallwords.SetCurSel(cur_sel_num);

				CString datadir = gpcdef.GetDataDir();
				CStdioFile f_words;

				if(f_words.Open(datadir+"\\notepad.txt",CFile::modeCreate|CFile::modeWrite))
				{
					CString linestr;
					for(int i=0; i<m_listallwords.GetCount();i++)
					{
						m_listallwords.GetText(i,linestr);
						f_words.WriteString(linestr);
						f_words.WriteString("\n");
					}

					f_words.Close();
				}		
				return TRUE;
			}
		}
		else if(nID == IDC_BUTTONEDIT)
		{
			if (nCode == CN_COMMAND)
				// Handles WM_COMMAND messages
				// sent by the button.
			{

				CString datadir = CString(gpcdef.GetDataDir()) +"\\notepad.txt";
				CString execmd = "notepad.exe '" + datadir + "'";
				WinExec(execmd,SW_SHOW);

				return TRUE;
			}
		}
		else if(nID == IDC_BTN_CHAOXIAN)
		{
			OnChaoXianInfo();
		}
		else if(nID == IDC_BTN_QUJIANZUOYE)
		{
			OnQujianZuoyeInfo();

		}
		else if(nID == IDC_BTN_JUNTEYUN)
		{
			OnJunteYunInfo();

		}
		else if(nID == IDC_BTN_SHUAIGUA)
		{
			OnShuaiGuaInfo();

		}
		else if(nID == IDC_BTN_SHIGONG)
		{
			OnShiGongInfo();

		}
		else if(nID == IDC_BTN_MY_CANCEL)
		{
			ShowWindow(SW_HIDE);
		}
		else if(nID == IDC_BTN_ALL)
		{
			OnAllInfo();
		}
		else
		{
			;
		}
	}

	return CPropertySheet::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CPropertySheetNotepad::OnDblclkListallwords() 
{
	CString curtext;
	m_listallwords.GetText(m_listallwords.GetCurSel(),curtext);
	if(g_curEdit != NULL)
	{
		g_curEdit->ReplaceSel(curtext,true);
		g_curEdit->SetFocus();
	}
}

void CPropertySheetNotepad::OnDblclkSchdInfo()
{
	CString curtext;
	m_listSchdInfo.GetText(m_listSchdInfo.GetCurSel(),curtext);
	int page=GetCurActiveUserPage();
	TRAIN_INDEX train_index=m_listSchdInfo.GetItemData(m_listSchdInfo.GetCurSel());
	if(page==JIAOBAN_PAGE)
	{
		m_page_jiaoban.Add1Train( train_index,curtext); 
	}

	CWnd *pSubPage=GetUserPage(page);
	if(pSubPage)
		pSubPage->SendMessage(UM_TG_TRAIN_SELECTED,(WPARAM)train_index,0);
	if(page!=SUBAO_PAGE)  // 在速报界面是输入车次
	{
		if(g_curEdit != NULL)
		{
			g_curEdit->ReplaceSel(curtext,true);
			g_curEdit->SetFocus();
		}
	}
}

void CPropertySheetNotepad::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CPropertySheet::OnLButtonDblClk(nFlags, point);
}

BOOL CPropertySheetNotepad::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int   nID   =   (int)wParam; 
	if(nID   ==   IDC_BTN_MY_CANCEL) 
	{ 
		this->OnClose();
	}
	return CPropertySheet::OnCommand(wParam, lParam);
}

BOOL CPropertySheetNotepad::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_LBUTTONDBLCLK)
	{
		Sleep(0);
	}
	return CPropertySheet::PreTranslateMessage(pMsg);
}


BOOL CPropertySheetNotepad::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	CString as;

	HWND hd = GetSafeHwnd();
	RegisterMsgProcHandleWnd(hd,MSGID_MsgStringMessage,0);
	unsigned long shift_id;
	m_bReSize = false;
	shift_id=m_pDoc->GetCurrentShiftID();
	////////////////////////////////////////
	int ids[]={ID_APPLY_NOW, IDCANCEL, IDOK, IDHELP};
	for(int i=0;i<sizeof ids / sizeof ids[0];i++)
	{
		CWnd * pWnd=GetDlgItem(ids[i]);
		if(pWnd==NULL)
			break;
		switch(i)
		{
		case 0:
			pWnd->SetWindowText(_T("保存"));
			pWnd->EnableWindow(TRUE);
			pWnd->ShowWindow(FALSE);
			break;
		case 1:
			pWnd->SetWindowText(_T("关闭"));
			pWnd->EnableWindow(TRUE);
			if(m_bModaless)
				pWnd->ShowWindow(FALSE);
			else
				pWnd->ShowWindow(TRUE);
			break;

		case 2:
			pWnd->SetWindowText(_T("确定"));
			pWnd->ShowWindow(FALSE);
			break;
		case 3:
			pWnd->ShowWindow(FALSE);
			break;

		default:break;
		}// switch
	}// for

	CRect cur_rect,new_rect;
	CWnd *pCancelWnd;
	if(m_bModaless)
	{
		GetWindowRect(&cur_rect);
		new_rect = cur_rect;
		new_rect.bottom+=30;
		MoveWindow(&new_rect,TRUE);
		CRect tmprc(0,0,0,0);
		m_buttonCancel.Create("关闭",WS_CHILD|WS_VISIBLE,tmprc,this,IDC_BTN_MY_CANCEL);
		pCancelWnd=&m_buttonCancel;
	}
	else
		pCancelWnd=GetDlgItem(IDCANCEL);
	GetClientRect(&cur_rect);
	new_rect.left=(cur_rect.Width()-100)/2;
	new_rect.right=new_rect.left+100;
	new_rect.top=cur_rect.bottom-28; 
	new_rect.bottom=cur_rect.bottom-2;
	pCancelWnd->MoveWindow(&new_rect); 

	GetWindowRect(&cur_rect);
	new_rect = cur_rect;
	new_rect.right = cur_rect.right + EXT_TAB_WIDTH;
	MoveWindow(&new_rect,TRUE);
	CenterWindow();

	GetClientRect(&cur_rect);


	//设置输入新词汇的编辑框位置
	new_rect = cur_rect;
	new_rect.top+=5;
	new_rect.left = cur_rect.right - EXT_TAB_WIDTH;
	new_rect.right = new_rect.left + EXT_TAB_WIDTH-10;
	new_rect.bottom = new_rect.bottom-2;

	m_tab.Create(WS_CHILD | WS_VISIBLE ,new_rect,this,IDC_WORD_TABCTRL);


	if(m_bEnableWordSelected)
	{
		TCITEM  tcItem;
		tcItem.mask = TCIF_TEXT|TCIF_PARAM;
		tcItem.pszText = "常用词汇";
		tcItem.lParam =0;
		m_tab.InsertItem(0, &tcItem);
	}
	if(m_bEnableScheduleInfoSelected)
	{
		TCITEM  tcItem;
		tcItem.mask = TCIF_TEXT|TCIF_PARAM;
		tcItem.pszText = "计划信息";
		tcItem.lParam =1;
		m_tab.InsertItem(1, &tcItem);
	}

	if(m_bEnableStPlanWaring)
	{
		TCITEM  tcItem;
		tcItem.mask = TCIF_TEXT|TCIF_PARAM;
		tcItem.pszText = "计划报警";
		tcItem.lParam =2;
		m_tab.InsertItem(2, &tcItem);
	}

	cur_rect=new_rect;

	new_rect.top+=25;
	new_rect.bottom-=4;
	new_rect.left+=8;
	new_rect.right-=8;


	new_rect.bottom = new_rect.top + 25;
    int nTop=new_rect.top;
	CRect cbo_rect=new_rect;
	int tmpw=new_rect.Width()/4;
	cbo_rect.right=cbo_rect.left+tmpw;  
	m_txtStartTime.Create("起始:",WS_CHILD | WS_VISIBLE ,cbo_rect,this,IDC_STATIC);
	m_txtEndTime.Create("结束:",WS_CHILD | WS_VISIBLE,cbo_rect,this,IDC_STATIC);
	m_cboStartTime.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,cbo_rect,this,IDC_CBO_STARTTIME);
	m_cboEndTime.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,cbo_rect,this,IDC_CBO_ENDTIME);
	m_buttonCHAOXIAN.Create("施工",WS_CHILD|WS_VISIBLE,cbo_rect,this,IDC_BTN_CHAOXIAN);
	m_buttonQUJIANZUOYE.Create("区间",WS_CHILD|WS_VISIBLE,cbo_rect,this,IDC_BTN_QUJIANZUOYE);
	m_buttonJUNTEYUN.Create("特运",WS_CHILD|WS_VISIBLE,cbo_rect,this,IDC_BTN_JUNTEYUN);
	m_buttonSHUAIGUA.Create("甩挂",WS_CHILD|WS_VISIBLE,cbo_rect,this,IDC_BTN_SHUAIGUA);
	m_buttonAll.Create("全部",WS_CHILD|WS_VISIBLE,cbo_rect,this,IDC_BTN_ALL);
	CTime  ctm=CTime::GetCurrentTime(); 
	CString text;
	for(int i= 0;i < 23;i++)
	{
		text.Format("%02d",i);
		int sel=m_cboStartTime.AddString(text);
		m_cboStartTime.SetItemData(sel,i);
		sel=m_cboEndTime.AddString(text);
		m_cboEndTime.SetItemData(sel,i);
	}
	text.Format("%02d",ctm.GetHour());  
	m_cboStartTime.SelectString(-1,text);
	text.Format("%02d",(ctm.GetHour()+6)%24); 
	m_cboEndTime.SelectString(-1,text);
	m_txtStartTime.MoveWindow(new_rect.left,new_rect.top,tmpw,new_rect.Height()); 
	m_cboStartTime.MoveWindow(new_rect.left+tmpw,new_rect.top,tmpw,new_rect.Height()*6);
	m_txtEndTime.MoveWindow(new_rect.left+tmpw*2,new_rect.top,tmpw,new_rect.Height()); 
	m_cboEndTime.MoveWindow(new_rect.left+tmpw*3,new_rect.top,tmpw,new_rect.Height()*6);


	m_newword.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL ,new_rect,this,IDC_EDITNEWWORD);
	m_newword.LimitText(50);



	//设置添加按扭的位置
	CRect addbutrect;
	new_rect.OffsetRect(0,40);
	new_rect.left+=4;
	new_rect.right=new_rect.left+50;

	m_buttonadd.Create("增加",WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,new_rect,this,IDC_BUTTONADD);
	m_buttonCHAOXIAN.MoveWindow(&new_rect);

	//设置删除按扭的位置
	CRect delbutrect;
	delbutrect = new_rect;
	delbutrect.left = new_rect.right+5; 
	delbutrect.right=delbutrect.left+50;

	m_buttondel.Create("删除",WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,delbutrect,this,IDC_BUTTONDEL);
	m_buttonQUJIANZUOYE.MoveWindow(&delbutrect);

	//设置编辑按扭的位置
	CRect editbutrect=delbutrect;
	editbutrect.left = delbutrect.right+5; 
	editbutrect.right=editbutrect.left+50;

	m_buttonedit.Create("编辑",WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,editbutrect,this,IDC_BUTTONEDIT);
	m_buttonJUNTEYUN.MoveWindow(&editbutrect);


	editbutrect.left = editbutrect.right+5; 
	editbutrect.right=editbutrect.left+50;

	m_buttonSHUAIGUA.MoveWindow(&editbutrect);


	editbutrect.left = editbutrect.right+5; 
	editbutrect.right=editbutrect.left+50;

	m_buttonAll.MoveWindow(&editbutrect);
	//设置下拉列表的位置
	CRect listboxrect=cur_rect;
	listboxrect.top = editbutrect.bottom+8; 
	listboxrect.bottom = cur_rect.bottom -4;
	listboxrect.left +=8;
	listboxrect.right -=8;

	m_listallwords.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | WS_VSCROLL | LBS_NOTIFY ,listboxrect,this,IDC_LISTALLWORDS);

	m_listSchdInfo.Create(WS_CHILD | WS_TABSTOP | WS_BORDER | WS_VSCROLL | LBS_NOTIFY| WS_HSCROLL ,listboxrect,this,IDC_LISTSCHDINFO);
   	
	listboxrect.top=nTop;
	m_stplangrid.CreateGrid(this, listboxrect, IDC_STPLAN_WARN);
	

	//读取数据
	CString datadir = gpcdef.GetDataDir();
	CStdioFile f_words;

	if(f_words.Open(datadir+"\\notepad.txt",CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead))
	{
		CString linetext;
		while(f_words.ReadString(linetext))
			m_listallwords.AddString(linetext);

		f_words.Close();
	}

	CFont * pFont; 
	pFont = new CFont; 
	pFont->CreateFont(12, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_MEDIUM, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("宋体")); // lpszFac 

	SetFont(pFont);   


	CWnd   *pw   =   GetWindow(GW_CHILD);   
	while(pw   !=   NULL)   
	{   
		pw->SetFont(pFont);   
		pw   =   pw->GetWindow(GW_HWNDNEXT);   
	};   
	pFont->Detach();
	delete pFont;


	m_page_subao.SetNotifyWindow(this);
	m_page_phasenote_plan.SetNotifyWindow(this);
	m_page_notepad.SetNotifyWindow(this);
	m_page_shigu_outline.SetNotifyWindow(this);
	m_page_jiaoban.SetNotifyWindow(this);
	m_page_shuaigua.SetNotifyWindow(this);

	InitDlgPosition(this);
	m_nLoadSchdInfoFlag=-1;

	m_page_send_stplan.SetStplanWarnList(&m_stplangrid);
	if(GetCurActiveUserPage()==STPLAN_PAGE)  // || GetCurActiveUserPage()==JIAOBAN_PAGE)
	{
		m_tab.SetCurSel(2); 
		OnSelchangeTabctrl(NULL,NULL);
	}
	else
	{
		m_tab.SetCurSel(0); 
		OnSelchangeTabctrl(NULL,NULL);
		if(GetCurActiveUserPage()==SUBAO_PAGE)  // || GetCurActiveUserPage()==JIAOBAN_PAGE)
		{
			m_page_subao.InformParent();
		}
	}
	return bResult;
}

void  CPropertySheetNotepad::LoadSchdInfo(int flag,TIME starttm,TIME endtm) 
{
	m_listSchdInfo.ResetContent();
	CWorkScheduleHolder *pholder=m_pDoc->GetTGDataHolder();
	CTG_TrainScheduleBase *pdata=pholder->GetWorkScheduleData();
	CTG_ScheduleInfo schd;
	pdata->GetScheduleInfo(schd); 
	CTG_TrainPtrGroup group;
	TIME start,end;
	schd.GetShiftManagerRange(start,end);
	if(starttm>0 && endtm>0)
	{
		start=starttm;
		end=endtm;
	}

	pdata->FindTrains(group,m_nStationNo,start,end);
	for(int i=0;i<group.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=group.GetTrainPtr(i);
		if(!m_bLoadPassengerTrain  && ptrain->IsPassengerTrain())
			continue;
		if(!m_bLoadFreightTrain  && !ptrain->IsPassengerTrain())
			continue;
		if(flag==1) // train information
		{
			CString infostr=GetObjectImportantInfo(ptrain);
			if(!infostr.IsEmpty()) 
			{
				int sel=m_listSchdInfo.AddString(infostr);
				m_listSchdInfo.SetItemData(sel,ptrain->GetTrainIndex());  
			}
		}
		else
			if(flag==0) // train index & no
			{
				CString cap;
				CTG_TrainRecord *pRec=ptrain->GetTrainRecord(0);
				if(!pRec) continue;
				CTime depatm=pRec->GetDepartTime();
				cap.Format("车次 %8s  %02d:%02d %s站发车",ptrain->GetTrainID(),depatm.GetHour(),depatm.GetMinute(),gpcdef.GetStationName(pRec->GetStationNo()));
				int sel=m_listSchdInfo.AddString(cap);
				m_listSchdInfo.SetItemData(sel,ptrain->GetTrainIndex());  
			}
	}
}

void CPropertySheetNotepad::OnSelchangeTabctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{ 
	int x=m_tab.GetCurSel();//所选标签号
	TCITEM item;
	item.mask =TCIF_PARAM;
	m_tab.GetItem(x,&item);
	int iPage=item.lParam; 
	if(iPage==0) // 常用词汇
	{
		m_tab.SetCurSel(0);
		m_newword.ShowWindow(SW_SHOW); //输入新词汇
		m_listallwords.ShowWindow(SW_SHOW);
		m_buttonadd.ShowWindow(SW_SHOW);
		m_buttondel.ShowWindow(SW_SHOW);
		m_buttonedit.ShowWindow(SW_SHOW);
	}
	else
	{
		m_newword.ShowWindow(SW_HIDE); //输入新词汇
		m_listallwords.ShowWindow(SW_HIDE);
		m_buttonadd.ShowWindow(SW_HIDE);
		m_buttondel.ShowWindow(SW_HIDE);
		m_buttonedit.ShowWindow(SW_HIDE);
	}

	if(iPage==1) // 计划信息
	{
		m_tab.SetCurSel(1);
		m_cboStartTime.ShowWindow(SW_SHOW);
		m_cboEndTime.ShowWindow(SW_SHOW);
		m_txtStartTime.ShowWindow(SW_SHOW);
		m_txtEndTime.ShowWindow(SW_SHOW);
		m_buttonCHAOXIAN.ShowWindow(SW_SHOW);
		m_buttonQUJIANZUOYE.ShowWindow(SW_SHOW);
		m_buttonJUNTEYUN.ShowWindow(SW_SHOW);
		m_buttonSHUAIGUA.ShowWindow(SW_SHOW);
		m_buttonAll.ShowWindow(SW_SHOW);
		m_listSchdInfo.ShowWindow(SW_SHOW);	
	}
	else
	{
		m_cboStartTime.ShowWindow(SW_HIDE);
		m_cboEndTime.ShowWindow(SW_HIDE);
		m_txtStartTime.ShowWindow(SW_HIDE);
		m_txtEndTime.ShowWindow(SW_HIDE);
		m_buttonCHAOXIAN.ShowWindow(SW_HIDE);
		m_buttonQUJIANZUOYE.ShowWindow(SW_HIDE);
		m_buttonJUNTEYUN.ShowWindow(SW_HIDE);
		m_listSchdInfo.ShowWindow(SW_HIDE);	
		m_buttonSHUAIGUA.ShowWindow(SW_HIDE);
		m_buttonAll.ShowWindow(SW_HIDE);
	}


	if(iPage==2) // 计划报警
	{
		m_tab.SetCurSel(2);
		m_stplangrid.ShowWindow(SW_SHOW);
	}
	else
	{
		m_stplangrid.ShowWindow(SW_HIDE);
	}

	int nSectionNo = m_pDoc->GetCurrentSectionNo();
	if(GetCurActiveUserPage()==STPLAN_PAGE)
	{
		m_cboStartTime.EnableWindow(FALSE);
		m_cboEndTime.EnableWindow(FALSE);
		m_txtStartTime.EnableWindow(FALSE);
		m_txtEndTime.EnableWindow(FALSE);

		if(!m_bReSize && !GetTGFunctionConfig()->IsCheckPlanValid(nSectionNo))
		{
			m_bReSize = true;
			CRect cur_rect;
			GetWindowRect(&cur_rect);
			cur_rect.right -= EXT_TAB_WIDTH;
			MoveWindow(&cur_rect,TRUE);
			CenterWindow();
		}
	}
	else
	{
		m_cboStartTime.EnableWindow(TRUE);
		m_cboEndTime.EnableWindow(TRUE);
		m_txtStartTime.EnableWindow(TRUE);
		m_txtEndTime.EnableWindow(TRUE);

		if(m_bReSize && !GetTGFunctionConfig()->IsCheckPlanValid(nSectionNo))
		{
			m_bReSize = false;
			CRect cur_rect;
			GetWindowRect(&cur_rect);
			cur_rect.right += EXT_TAB_WIDTH;
			MoveWindow(&cur_rect,TRUE);
			CenterWindow();
		}
	}

	if(GetCurActiveUserPage()!=PHASENOTE_PAGE && GetCurActiveUserPage()!=PHASENOTE_SEND_PAGE)
	{
		m_buttonCHAOXIAN.EnableWindow(FALSE);
		m_buttonQUJIANZUOYE.EnableWindow(FALSE);
		m_buttonJUNTEYUN.EnableWindow(FALSE);
		m_buttonSHUAIGUA.EnableWindow(FALSE);
		m_buttonAll.EnableWindow(FALSE);
	}
	else
	{
		m_buttonCHAOXIAN.EnableWindow(TRUE);
		m_buttonQUJIANZUOYE.EnableWindow(TRUE);
		m_buttonJUNTEYUN.EnableWindow(TRUE);
		m_buttonSHUAIGUA.EnableWindow(TRUE);
		m_buttonAll.EnableWindow(TRUE);
	}
}

BOOL CPropertySheetNotepad::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *phr=(NMHDR *)lParam;
	if(phr->idFrom ==IDC_WORD_TABCTRL)
		return CPropertySheet::OnNotify(wParam, lParam, pResult);
	if(phr->code !=TCN_SELCHANGE)
		return CPropertySheet::OnNotify(wParam, lParam, pResult);


	int page=GetCurActiveUserPage();
	if(page==STPLAN_PAGE)  // 发送阶段计划,无须常用词汇,并且计划信息由其内部控制
	{
		m_tab.SetCurSel(2); 
		OnSelchangeTabctrl(NULL,NULL);
		m_stplangrid.ClearView();
	}
	else
	{
		if(page==PHASENOTE_PAGE)
			m_page_notepad.InformParent();
		else if(page==PHASENOTE_SEND_PAGE)
			m_page_phasenote_plan.InformParent(); 
		else if(page==SHIGU_PAGE)
			m_page_shigu_outline.InformParent();
		else if(page==JIAOBAN_PAGE)
			m_page_jiaoban.InformParent();
		else if(page==SUBAO_PAGE)
			m_page_subao.InformParent();
		else if(page==SHUAIGUA_PAGE)
			m_page_shuaigua.InformParent();

		//if(page==SUBAO_PAGE || page==SHUAIGUA_PAGE || page==JIAOBAN_PAGE || page==SHIGU_PAGE)
		//{
		//	m_tab.SetCurSel(1); 
		//}
		OnSelchangeTabctrl(NULL,NULL);
	}
	return CPropertySheet::OnNotify(wParam, lParam, pResult);
}

unsigned long CPropertySheetNotepad::Getnextday()
{
	int  CurYear,CurMonth,CurDay;
	CString  NextDay;
	unsigned long result;


	CTime t=CTime::GetCurrentTime();
	CurYear=t.GetYear();
	CurMonth=t.GetMonth();
	CurDay=t.GetDay();
	ASSERT(CurDay>=1 && CurDay<=31);
	switch(CurDay)
	{
	case 28:
		if( (CurYear %4==0 && CurYear %100!=0)||(CurYear %400==0) )  
			NextDay.Format ("%4d%2d%2d",CurYear,CurMonth,29);
		else
			NextDay.Format ("%4d%2d%2d",CurYear,CurMonth+1,  1);
		break;

	case 29:
		if(CurMonth==2)
			NextDay.Format ("%4d%2d%2d",CurYear,CurMonth+1,1);
		else
			NextDay.Format ("%4d%2d%2d",CurYear,CurMonth,30);
		break;

	case 30:
		if(CurMonth==1 || CurMonth==3 || CurMonth==5 || CurMonth==7 || CurMonth==8
			|| CurMonth==10 ||CurMonth==12)
			NextDay.Format ("%4d%2d%2d",CurYear,CurMonth,31);
		else
			NextDay.Format ("%4d%2d%2d",CurYear,CurMonth+1,1);
		break;

	case 31:
		if(CurMonth==12)
			NextDay.Format ("%4d%2d%2d",CurYear+1,1,1);
		else
			NextDay.Format ("%4d%2d%2d",CurYear,CurMonth+1,1);
		break;

	default:
		NextDay.Format ("%4d%2d%2d",CurYear,CurMonth,CurDay+1);
		break;

	}

	NextDay.Replace(" ","0");  
	result=atol(NextDay);
	return result;
}

unsigned long CPropertySheetNotepad::GetToday()
{

	int  iYear,iMonth,iDay;
	CString  sYear,sMonth,sDay,Today;
	unsigned long result;

	CTime t=CTime::GetCurrentTime();
	iYear=t.GetYear();
	iMonth=t.GetMonth();
	iDay=t.GetDay();

	sYear.Format ("%4d",iYear);

	if(iMonth>9)
		sMonth.Format("%2d",iMonth);
	else
		sMonth.Format("0%d",iMonth);

	if(iDay>9)
		sDay.Format("%2d",iDay);
	else
		sDay.Format("0%d",iDay);

	Today=sYear+sMonth+sDay;
	result=atol(Today);

	return result;
}


void CPropertySheetNotepad::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{


	CPropertySheet::OnChar(nChar, nRepCnt, nFlags);
}

void CPropertySheetNotepad::PostNcDestroy() 
{

	CPropertySheet::PostNcDestroy();
}

void CPropertySheetNotepad::OnClose() 
{
	if(GetTGFunctionConfig()->IsEnableSendPlanNoticeDCmd())
	{
		CPropertyPage *pSendPlanDlg = GetUserPage(STPLAN_PAGE);
		if(NULL != pSendPlanDlg)
		{
			((CSendStationPlanDlgEx*)pSendPlanDlg)->SendNoticeToDCmd(0);
		}
	}
	m_HideWnd=true;
	ShowWindow(SW_HIDE);
	//CPropertySheet::OnClose();
}

void CPropertySheetNotepad::OnSelchangeSbtEndtime() 
{
	int nStartTime,nEndTime;
	int cursel=m_cboEndTime.GetCurSel();
	if(cursel==LB_ERR)
		return;
	nEndTime=m_cboEndTime.GetItemData(cursel);
	cursel=m_cboStartTime.GetCurSel();
	if(cursel==LB_ERR)
		return;
	nStartTime=m_cboStartTime.GetItemData(cursel);
	CTime curtm=CTime::GetCurrentTime(); 

	m_nStarttime=ConvertToLongTime(nStartTime,0,0,curtm.GetTime(),6,12);
	m_nEndtime=ConvertToLongTime(nEndTime,0,0,curtm.GetTime(),6,12);
	if(m_nEndtime<m_nStarttime)
	{
		MessageBox("时间范围不合法");
		return;
	}
	LoadSchdInfo(m_nLoadSchdInfoFlag,m_nStarttime,m_nEndtime); 	

}

void CPropertySheetNotepad::OnSelchangeSbtStarttime() 
{
	OnSelchangeSbtEndtime();
}
void CPropertySheetNotepad::OnQujianZuoyeInfo()
{
	CString curtext;
	curtext="<区间作业信息>\r\n";
	if(g_curEdit != NULL)
	{
		g_curEdit->ReplaceSel(curtext,true);
		g_curEdit->SetFocus();
	}
}
void CPropertySheetNotepad::OnJunteYunInfo()
{
	CString curtext;
	curtext="<军特运信息>\r\n";
	GenSchInfoString(curtext,INFO_TYPE_TEYUN,m_nStarttime,m_nEndtime);
	if(g_curEdit != NULL)
	{
		g_curEdit->ReplaceSel(curtext,true);
		g_curEdit->SetFocus();
	}

}
void CPropertySheetNotepad::OnChaoXianInfo()
{
	CString curtext;
	curtext="<施工信息>\r\n";
	GenSchInfoString(curtext,INFO_TYPE_SHIGONG,m_nStarttime,m_nEndtime);
	if(g_curEdit != NULL)
	{
		g_curEdit->ReplaceSel(curtext,true);
		g_curEdit->SetFocus();
	}

}

void CPropertySheetNotepad::OnShuaiGuaInfo()
{
	CString curtext;
	curtext="<甩挂信息>\r\n";
	GenSchInfoString(curtext,INFO_TYPE_SHUAIGUA,m_nStarttime,m_nEndtime);
	if(g_curEdit != NULL)
	{
		g_curEdit->ReplaceSel(curtext,true);
		g_curEdit->SetFocus();
	}

}
void CPropertySheetNotepad::OnShiGongInfo()
{
	CString curtext;
	curtext="<施工信息>\r\n";
	GenSchInfoString(curtext,INFO_TYPE_SHIGONG,m_nStarttime,m_nEndtime);
	if(g_curEdit != NULL)
	{
		g_curEdit->ReplaceSel(curtext,true);
		g_curEdit->SetFocus();
	}

}

void  CPropertySheetNotepad::OnAllInfo()
{
	CString curtext;
	CString sub;
	GenSchInfoString(sub,INFO_TYPE_CHAOXIAN,m_nStarttime,m_nEndtime);
	if(sub.IsEmpty()) 
		curtext="一、超限车信息: 无\r\n";
	else
	{
		curtext="一、超限车信息:\r\n";
		curtext+=sub;
	}
	sub.Empty(); 
	GenSchInfoString(sub,INFO_TYPE_TEYUN,m_nStarttime,m_nEndtime);
	if(sub.IsEmpty()) 
		curtext+="二、军特运信息: 无\r\n";
	else
	{
		curtext+="二、军特运信息: \r\n";
		curtext+=sub;
	}
	sub.Empty(); 
	GenSchInfoString(sub,INFO_TYPE_SHUAIGUA,m_nStarttime,m_nEndtime);
	if(sub.IsEmpty()) 
		curtext+="三、甩挂计划信息: 无\r\n";
	else
	{
		curtext+="三、甩挂计划信息: \r\n";
		curtext+=sub;
	}
	sub.Empty(); 
	GenSchInfoString(sub,INFO_TYPE_QUJIANZUOYE,m_nStarttime,m_nEndtime);
	if(sub.IsEmpty()) 
		curtext+="四、区间作业信息: 无\r\n";
	else
	{
		curtext+="四、区间作业信息: \r\n";
		curtext+=sub;
	}
	sub.Empty(); 
	GenSchInfoString(sub,INFO_TYPE_SHIGONG,m_nStarttime,m_nEndtime);
	if(sub.IsEmpty()) 
		curtext+="五、施工信息: 无\r\n";
	else
	{
		curtext+="五、施工信息: \r\n";
		curtext+=sub;
	}
	curtext+="六、其他信息:";

	if(g_curEdit != NULL)
	{
		g_curEdit->ReplaceSel(curtext,true);
		g_curEdit->SetFocus();
	}
}

void  CPropertySheetNotepad::GenSchInfoString(CString &dest,int flag,TIME starttm,TIME endtm)
{
	CWorkScheduleHolder *pholder=m_pDoc->GetTGDataHolder();
	CTG_TrainScheduleBase *pdata=pholder->GetWorkScheduleData();
	CTG_ScheduleInfo schd;
	pdata->GetScheduleInfo(schd); 
	TIME start,end;
	schd.GetShiftManagerRange(start,end);
	if(starttm>0 && endtm>0)
	{
		start=starttm;
		end=endtm;
	}
	if(flag==INFO_TYPE_SHIGONG) 
	{
		CTG_ObjectPtrGroup  group;
		pdata->FindMarks(group,start,end);
		for(int i=0;i<group.GetCount();i++)
		{
			CString sub;
			CTG_SingleMark *pmark=(CTG_SingleMark *)group.GetPtr(i);
			if(!pmark->IsAbsMarkType()  && !pmark->IsStationMarkType())
				continue;
			CString ptmpstr=GetObjectWorkInfo(pmark);
			if(ptmpstr.IsEmpty())
				continue;
			sub=ptmpstr;
			sub+="\r\n";
			if(!sub.IsEmpty())
			{
				dest+=sub;
				dest+="\r\n";
			}
		}
		return;
	}
	CTG_TrainPtrGroup group;
	pdata->FindTrains(group,NULL_STATION,start,end);
	for(int i=0;i<group.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=group.GetTrainPtr(i);
		CString sub;
		if(flag==INFO_TYPE_TEYUN) 
		{
			if(ptrain->GetMilitaryRank()>0 || ptrain->GetSpaceRank()>0)
			{
				sub=GetObjectImportantInfo(ptrain);
			}
		}
		else
			if(flag==INFO_TYPE_CHAOXIAN) 			
			{
				if(ptrain->GetSpaceRank()>0) 
				{
					sub=GetObjectImportantInfo(ptrain);
				}
			}
			else
				if(flag==INFO_TYPE_SHUAIGUA)
				{
					sub=ptrain->GetImportInfo(TRAIN_TXT_INFO_TYPE_SHUAIGUA);
				}
				if(!sub.IsEmpty())
				{
					dest+=sub;
					dest+="\r\n";
				}
	}


}

LRESULT CPropertySheetNotepad::OnSchdTrainSelected(WPARAM wp,LPARAM lp)
{
	TRAIN_SELECT_PARA *pPara=(TRAIN_SELECT_PARA *)lp;	
	CTime t;
	CString text;
	TRAIN_INDEX train_index=wp;
	if(pPara && (pPara->starttime>0 && pPara->endtime>0))
	{
		t=pPara->starttime;
		text.Format("%02d",t.GetHour());  
		m_cboStartTime.SelectString(-1,text);

		t=pPara->endtime;
		text.Format("%02d",t.GetHour());  
		m_cboEndTime.SelectString(-1,text);

		m_nStarttime=pPara->starttime;
		m_nEndtime=pPara->endtime;
		if(pPara->train_type>=0)
		{
			if(pPara->train_type==0 || pPara->train_type==1)
				m_bLoadPassengerTrain=TRUE;
			else
				m_bLoadPassengerTrain=FALSE;
			if(pPara->train_type==0 || pPara->train_type==2)
				m_bLoadFreightTrain=TRUE;
			else
				m_bLoadFreightTrain=FALSE;
		}
		if(pPara->station_no>=0) 
			m_nStationNo=pPara->station_no;
		if(pPara->list_type>=0)
			m_nLoadSchdInfoFlag=pPara->list_type;
		LoadSchdInfo(m_nLoadSchdInfoFlag,m_nStarttime,m_nEndtime); 
	}

	if(train_index>0)
	{
		for(int i=0;i<m_listSchdInfo.GetCount();i++)
		{
			if(m_listSchdInfo.GetItemData(i)==wp)
			{
				m_listSchdInfo.SetCurSel(i);
			}
		}
	}
	OnSelchangeTabctrl(NULL,NULL);
	return 0;
}