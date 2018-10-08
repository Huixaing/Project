// ChangeCadVariable.cpp : implementation file
//

#include "stdafx.h"
#include "..\wintg\resource.h"
#include "ChangeCadVariable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeCadVariable dialog


CChangeCadVariable::CChangeCadVariable(CWintgDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CChangeCadVariable::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeCadVariable)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_ctrlSectionRuntimePage = new CSectionRuntimePage(pDoc);
	m_ctrlInputSectionInformPage = new CInputSectionInform;
}


void CChangeCadVariable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeCadVariable)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeCadVariable, CDialog)
	//{{AFX_MSG_MAP(CChangeCadVariable)
	ON_BN_CLICKED(IDC_EDITCAD_CANCEL, OnEditcadCancel)
	ON_BN_CLICKED(IDC_EDITCAD_SAVELOCAL, OnEditcadSavelocal)
	ON_BN_CLICKED(IDC_EDITCAD_SAVEREMOTE, OnEditcadSaveremote)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeCadVariable message handlers

BOOL CChangeCadVariable::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRect rclRange;

	GetWindowRect(&rclRange);
	ScreenToClient(&rclRange);
	// TODO: Add extra initialization here
	// Create the CPrivatePropertyertysheet
	m_ctrlPropertySheet.m_Rect.left = 10;	// set the position on the screen
	m_ctrlPropertySheet.m_Rect.top = 10;
	m_ctrlPropertySheet.m_Rect.right = rclRange.Width( ) - 20;
	m_ctrlPropertySheet.m_Rect.bottom = rclRange.Height( ) - 80;
	m_ctrlPropertySheet.m_nTotalPageNum = -1;		// init this membervariable
	m_ctrlPropertySheet.m_nActPage = 0;		// init this membervariable
	m_ctrlPropertySheet.m_nCtrlID = 2000;	// control-id in the dialog

	// and create it on the screen
	m_ctrlPropertySheet.Create (WS_VISIBLE | WS_CHILD | WS_TABSTOP,
		m_ctrlPropertySheet.m_Rect, this, m_ctrlPropertySheet.m_nCtrlID);

	// Now add the dialogs page per page
	TC_ITEM Item;
	Item.mask = TCIF_TEXT;
	CPrivatePropertyPage *pPropPage;


	int nPageIndex;
	int	nTotalPageNum = 2;	// in Private example I have four pages
    for (nPageIndex = 0; nPageIndex < nTotalPageNum; nPageIndex++) 
	{
		
		pPropPage = new (CPrivatePropertyPage);	// new it

		// Create the tab and the dialog
		switch (nPageIndex) {
		case 0:	// page number 1 - a small example
			Item.pszText = "区间运行时间";
			pPropPage->m_strTitle = "&区间运行时间";	// or get the title of m_fontPage - whatwever you want
			pPropPage->m_pDialogPage = (CPrivatePropertyDialog *)& m_ctrlSectionRuntimePage;
			pPropPage->m_pDialogPage->Create (IDD_SECTIONPAGE_RUNTIME, &m_ctrlPropertySheet);
			break;
		case 1:	// page number 2 - a small example
			Item.pszText = "车站作业间隔时分信息";
			pPropPage->m_strTitle = "&车站作业间隔时分信息";	// or get the title of m_fontPage - whatwever you want
			pPropPage->m_pDialogPage = (CPrivatePropertyDialog *)& m_ctrlInputSectionInformPage;
			pPropPage->m_pDialogPage->Create (IDD_SECTIONPAGE_INFORM, &m_ctrlPropertySheet);
			break;
		}
		m_ctrlPropertySheet.InsertItem (nPageIndex, &Item);	// this is fot CTabWnd
		pPropPage->m_hLocal = NULL;	// nothing is created on the fly
									// important information on delete!

		// add it to the array
		m_ctrlPropertySheet.m_Dialogs.Add (pPropPage);
		m_ctrlPropertySheet.m_nTotalPageNum++;	// one more page


		// the size of CTabWnd is m_rect
		// the size of the dialog is smaller
		pPropPage->m_Rect.top = 30;	// above there must be enough place for the tab-control
		pPropPage->m_Rect.left = 10;	// border of 10 units is good
		pPropPage->m_Rect.bottom = m_ctrlPropertySheet.m_Rect.bottom - m_ctrlPropertySheet.m_Rect.top - 10;
		pPropPage->m_Rect.right = m_ctrlPropertySheet.m_Rect.right - m_ctrlPropertySheet.m_Rect.left - 10;

		// Only the 1. page should be active at startup
		if (nPageIndex > 0) {
			pPropPage->m_pDialogPage->SetWindowPos(NULL, pPropPage->m_Rect.left,
							pPropPage->m_Rect.top, 0, 0,
							SWP_HIDEWINDOW | SWP_NOSIZE); 
		}
		else {
			pPropPage->m_pDialogPage->SetWindowPos(NULL, pPropPage->m_Rect.left,
							pPropPage->m_Rect.top, 0, 0,
							SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		// the class will handle the change of the tab-control
		// and synchronize activate-deactivate the dialogpages
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeCadVariable::OnEditcadCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel( );
}

void CChangeCadVariable::OnEditcadSavelocal() 
{
	// TODO: Add your control notification handler code here
	
//	int nPageNo = m_ctrlNotepadSheet->GetActiveIndex( );
//
//	CPropertyPage *pPage = m_ctrlNotepadSheet->GetActivePage( );
}

void CChangeCadVariable::OnEditcadSaveremote() 
{
	// TODO: Add your control notification handler code here
//	int nPageNo = m_ctrlNotepadSheet->GetActiveIndex( );

//	CPropertyPage *pPage = m_ctrlNotepadSheet->GetActivePage( );
	CDialog::OnOK( );
}

void CChangeCadVariable::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
	delete m_ctrlSectionRuntimePage;
	delete m_ctrlInputSectionInformPage;
}

