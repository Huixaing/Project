// ReceiveNeighberPlanSheet.cpp : implementation file
//

#include "stdafx.h"
#include "SheetTGProperty.h"
#include "tgview.h"
#include "ext_src/UserMessageID.h"
#include "function.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_TAB_CTRL_ID 40000
#define IDC_TREE_TG_PPT 40001
#define IDC_LIST_TG_PPT 40002
#define IDC_LIST_TG_IE  40003
#define IDC_LIST_TG_SUM 40004
#define IDC_ED_INPUT_TRAIN_NO 40005
#define IDC_BTN_GO 40006



/////////////////////////////////////////////////////////////////////////////
// CSheetTGProperty

IMPLEMENT_DYNAMIC(CSheetTGProperty, CWnd)

CSheetTGProperty::CSheetTGProperty()
{
	m_nSelStationNo=0;
	m_nSelStationRecIndex=-1;
	m_pTgView=NULL;
	m_pCurrentObject=NULL;
	m_PspType=PSP_TYPE_NULL;
	m_pDoc=NULL;
	m_bReadOnly=TRUE;
	m_nCurrentTrainIndex=0;
}
CSheetTGProperty::~CSheetTGProperty()
{
}

BEGIN_MESSAGE_MAP(CSheetTGProperty, CStatic)
	//{{AFX_MSG_MAP(CSheetTGProperty)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_NOTIFY(TCN_SELCHANGE,IDC_TAB_CTRL_ID,OnSelchangeTabctrl)
	ON_MESSAGE(WM_USER_SET_DATA,OnPropertyChange)
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_LIST_TG_PPT, OnTrainInfoItemChanged)

	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSheetTGProperty message handlers
struct{
	char  caption[24];
}g_TabPara[TRAIN_PROPERTY_TAB_COUNT]={
	{"区段汇总"},
	{"车次标记"},
};
BOOL CSheetTGProperty::CreateMe(const RECT &rect,CWnd *pParentWnd)
{
	if(!Create("",WS_CHILD|WS_VISIBLE|WS_BORDER,rect,pParentWnd))
		return FALSE;

	return TRUE;
}

int  CSheetTGProperty::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRect rect(0,0,0,0);
	m_tabctrl.Create(WS_CHILD|WS_VISIBLE|TCS_TABS|TCS_FIXEDWIDTH,rect,this,IDC_TAB_CTRL_ID);
	CFont * f; 
	f = new CFont; 
	f->CreateFont(12, // nHeight 
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
	m_tabctrl.SetFont(f);
	f->Detach(); 
	delete f;
	TCITEM  tcItem;
	for(int i=0;i<TRAIN_PROPERTY_TAB_COUNT;i++)
	{
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText =  g_TabPara[i].caption;
		m_tabctrl.InsertItem(i, &tcItem);
	}

	m_trainInfoControl.Create(WS_VISIBLE|WS_CHILD|PTS_NOTIFY,rect,&m_tabctrl,IDC_LIST_TG_PPT);
	m_sumInfoControl.Create(WS_VISIBLE|WS_CHILD|PTS_NOTIFY,rect,&m_tabctrl,IDC_LIST_TG_SUM);

	CreateProperty(PSP_TYPE_SUM);

	return 0;
}

void CSheetTGProperty::OnClose()
{
	ShowWindow(SW_HIDE);
}

void CSheetTGProperty::OnSize(UINT nType, int cx, int cy) 
{
	RECT lpRect;

	// Get size of dialog window.
	GetClientRect(&lpRect);

	if(m_tabctrl.GetSafeHwnd())
	{
		m_tabctrl.MoveWindow(&lpRect, TRUE);
		m_tabctrl.AdjustRect(FALSE,&lpRect);
		if(m_trainInfoControl.GetSafeHwnd() && cx<10000 && cy<10000 && cx>0 && cy>0)
			m_trainInfoControl.MoveWindow(&lpRect);
		if(m_sumInfoControl.GetSafeHwnd() && cx<10000 && cy<10000 && cx>0 && cy>0)
			m_sumInfoControl.MoveWindow(&lpRect);

	} 

	CStatic::OnSize(nType, cx, cy);
}

void CSheetTGProperty::SetCurPage(int page_index)
{
	m_tabctrl.SetCurSel(page_index); 
	OnSelchangeTabctrl(NULL,NULL);
}

void CSheetTGProperty::OnSelchangeTabctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{ 
	int iPage=m_tabctrl.GetCurSel();//所选标签号
	if(iPage==TABINDEX_TRAIN_INFO) 
		m_trainInfoControl.ShowWindow(SW_SHOW);
	else
		m_trainInfoControl.ShowWindow(SW_HIDE);

	if(iPage==TABINDEX_TRAIN_SUM)
	{
		const CTG_TrainScheduleBase *pbase=m_pDoc->GetTGDataHolder()->GetBasicScheduleData();
		UpdatePropertyData(pbase);
		
		m_sumInfoControl.ShowWindow(SW_SHOW);
	}
	else
		m_sumInfoControl.ShowWindow(SW_HIDE);
}

BOOL  CSheetTGProperty::UpdateLocalData(const CTG_LogicObject *pobj) 
{
	return m_pTgView->GetMMIInterface()->UpdateObject_ByProperty(pobj);
}

LRESULT CSheetTGProperty::OnPropertyChange(WPARAM wParam, LPARAM lParam)
{	
	return 0;
}	

void CSheetTGProperty::SetReadOnly(BOOL va)
{
	m_bReadOnly=va;
	if(m_bReadOnly)
	{
		if(m_trainInfoControl.GetSafeHwnd())
			m_trainInfoControl.SetAllItemsReadOnly(TRUE); 
	}
}