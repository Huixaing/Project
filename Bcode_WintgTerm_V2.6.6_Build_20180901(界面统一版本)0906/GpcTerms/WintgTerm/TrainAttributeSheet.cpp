// TrainAttributeSheet.cpp : implementation file
//

#include "stdafx.h"
#include "TrainAttributeSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTrainAttributeSheet

IMPLEMENT_DYNAMIC(CTrainAttributeSheet, CPropertySheet)

CTrainAttributeSheet::CTrainAttributeSheet(UINT nIDCaption,CWintgDoc *pDoc,int schd_type, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_pDoc = pDoc;
	m_nSchdType = schd_type;
	AddPropertyPage();
}

CTrainAttributeSheet::CTrainAttributeSheet(LPCTSTR pszCaption, CWintgDoc *pDoc, int schd_type,CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_pDoc = pDoc;
	m_nSchdType = schd_type;
 	AddPropertyPage();

}

CTrainAttributeSheet::~CTrainAttributeSheet()
{
	delete m_page1;
	delete m_page2;
}


BEGIN_MESSAGE_MAP(CTrainAttributeSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CTrainAttributeSheet)
//	ON_COMMAND(ID_APPLY_NOW, OnApplyNow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/*
void CTrainAttributeSheet::OnApplyNow()
{
	Default();

	CFrameWnd* pFrameWnd = STATIC_DOWNCAST(CFrameWnd, AfxGetMainWnd());
	CView* pView = pFrameWnd->GetActiveFrame()->GetActiveView();
	pView->SendMessage(WM_USER_CHANGE_OBJECT_PROPERTIES, 0, 0);
	m_stylePage.SetModified(FALSE);
	m_colorPage.SetModified(FALSE);
	SendMessage(PSM_CANCELTOCLOSE);
}
*/
/////////////////////////////////////////////////////////////////////////////
// CTrainAttributeSheet message handlers

void CTrainAttributeSheet::AddPropertyPage()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_psh.dwFlags |= PSP_USEHICON;
  	m_psh.dwFlags &= ~PSH_HASHELP;  // Lose the Help button
  	m_psh.dwFlags |= PSH_NOAPPLYNOW;    // Lose the Apply Now button
	m_psh.hIcon = m_hIcon;
    
	m_page1 = new  CPageTrainAttribute(m_pDoc,m_nSchdType);
	m_page2 = new  CPageTrainAttributeOther(m_pDoc,m_nSchdType);
    m_page1->m_psp.dwFlags &= ~PSP_HASHELP;
    m_page2->m_psp.dwFlags &= ~PSP_HASHELP;
	AddPage(m_page1);
 	AddPage(m_page2); 
 
}

void CTrainAttributeSheet::SetSheetPropsFromDoc(CWintgDoc* pDoc)
{
}

void CTrainAttributeSheet::SetDocFromSheet(CWintgDoc* pDoc)
{

}

BOOL CTrainAttributeSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO: Add your specialized code here
	InitDlgPosition(this);
    InitIdentify();
	return bResult;
}
void CTrainAttributeSheet::InitIdentify()
{
	if(!gpcdef.GetOperatorRight(WINTG_EDIT)||
		((m_nSchdType == HISTORY_SCHEDULE) && m_pDoc->m_bHistUnChange))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);
			
	 
	}
	
	if( m_pDoc != NULL && m_pDoc->m_bHistUnChange == TRUE && m_nSchdType == HISTORY_SCHEDULE)
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);
		
 
	 
	}
}
