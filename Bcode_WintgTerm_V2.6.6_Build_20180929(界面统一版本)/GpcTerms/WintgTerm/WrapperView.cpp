// WrapperView.cpp : implementation file
//


#include "stdafx.h"
#include "WrapperView.h"
#include "MyPreviewView.h"
#include "DlgMainFrm.h"

//#include "memshare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);
BOOL CALLBACK _AfxMyPreviewCloseProc(CFrameWnd* pFrameWnd);

/////////////////////////////////////////////////////////////////////////////
// CWrapperView

IMPLEMENT_DYNCREATE(CWrapperView, CScrollView)

CWrapperView::CWrapperView()
{
	m_pFrameWnd = NULL;
	m_nMapMode = MM_TEXT; //default
}

CWrapperView::~CWrapperView()
{
}


BEGIN_MESSAGE_MAP(CWrapperView, CScrollView)
	//{{AFX_MSG_MAP(CWrapperView)
		// NOTE - the ClassWizard will add and remove mapping macros here.	
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWrapperView::OnDraw(CDC *pDC)
{
	
}

void CWrapperView::OnPrint(CDC* pDC,CPrintInfo* pInfo)
{
	if(pDC)
	{
		CDlgMainFrame* pf=(CDlgMainFrame*)::AfxGetMainWnd();		
		(*pf->Draw)(pDC,pInfo,(void*)pf->pCallWnd);		
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWrapperView diagnostics

#ifdef _DEBUG
void CWrapperView::AssertValid() const
{
	CView::AssertValid();
}

void CWrapperView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWrapperView message handlers

BOOL CALLBACK _AfxMyPreviewCloseProc(CFrameWnd* pFrameWnd)
{
	ASSERT_VALID(pFrameWnd);

	CMyPreviewView* pView = (CMyPreviewView*) pFrameWnd->GetDlgItem(AFX_IDW_PANE_FIRST);
	ASSERT_KINDOF(CPreviewView, pView);

	pView->OnPreviewClose();
	return FALSE;
}

BOOL CWrapperView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// default preparation
   // set to non-default printer without changing default app printer

//	return TRUE;
	return DoPreparePrinting(pInfo);
}

void CWrapperView::OnFilePrint()
{
	CDlgMainFrame* pf=(CDlgMainFrame*)::AfxGetMainWnd();
	if(pf->bDirectPrint)
		pf->ShowWindow(SW_HIDE);
//	CView::OnFilePrint();	
	{
		HGLOBAL hDevMode;
		HGLOBAL hDevNames;
		
		if (!GetPrinterDevice((char *)(LPCTSTR)m_sPlotName, &hDevNames, &hDevMode))
		{
			AfxMessageBox(_T("GetPrinterDevice called failed\n"));
			return ;
		}
		
		
		CPrintInfo pInfo;
		HDC prnDc;

		pInfo.m_bDirect = TRUE;
		pInfo.m_pPD->m_pd.hDevMode = hDevMode;
		pInfo.m_pPD->m_pd.hDevNames = hDevNames;
		prnDc = pInfo.m_pPD->CreatePrinterDC();
		
		CDC printDC;
		printDC.Attach(prnDc);
		printDC.m_bPrinting = 1;

		

		DOCINFO printDocumentName;
		CString sPrintName;
		sPrintName.Format("CASCO¹«Ë¾");
		char sPrintDocName[32];
		memcpy(sPrintDocName,sPrintName,32);
		printDocumentName.cbSize = sizeof(DOCINFO);
		printDocumentName.lpszDocName = sPrintDocName;
		printDocumentName.lpszOutput = NULL;
		if(printDC.StartDoc(&printDocumentName) < 0)
		{
			return;
		};
		if(printDC.StartPage( ) < 0)
		{
			return;
		};




		//printDC.TextOut(0,0,"help");
		OnPrint(&printDC,&pInfo);

		printDC.EndPage( );
		printDC.EndDoc( );
		OnEndPrinting(&printDC,&pInfo);
		printDC.Detach();
		DeleteDC(prnDc);
		GlobalFree(hDevNames);
		GlobalFree(hDevMode);
	}

	if(pf->bDirectPrint)
	{
		//CWinApp *pApp=AfxGetApp();
		CWinThread *pThread = AfxGetThread( );
		pThread->m_pMainWnd=pf->pOldWnd;
		pf->DestroyWindow();	
	}
}


void CWrapperView::OnFilePrintPreview(CFrameWnd *pFrame)
{	
	m_pFrameWnd = pFrame;

	CPrintPreviewState* pState = new CPrintPreviewState;
	pState->lpfnCloseProc = _AfxMyPreviewCloseProc;

	if (!DoPrintPreview(AFX_IDD_PREVIEW_TOOLBAR, this,RUNTIME_CLASS(CMyPreviewView), pState))
	{		

		TRACE0("Error: DoPrintPreview failed.\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		delete pState;
	}
}

BOOL CWrapperView::DoPrintPreview(UINT nIDResource, CView* pPrintView,
	CRuntimeClass* pPreviewViewClass, CPrintPreviewState* pState)
{
	ASSERT_VALID_IDR(nIDResource);
	ASSERT_VALID(pPrintView);
	ASSERT(pPreviewViewClass != NULL);
	ASSERT(pPreviewViewClass->IsDerivedFrom(RUNTIME_CLASS(CPreviewView)));
	ASSERT(pState != NULL);

	ASSERT(m_pFrameWnd != NULL);
	CFrameWnd* pParent = m_pFrameWnd; //STATIC_DOWNCAST(CFrameWnd, AfxGetMainWnd()); //
	ASSERT_VALID(pParent);

	CCreateContext context;
	context.m_pCurrentFrame = pParent;
	context.m_pCurrentDoc = GetDocument();
	context.m_pLastView = this;

	// Create the preview view object
	CMyPreviewView* pView = (CMyPreviewView*)pPreviewViewClass->CreateObject();
	if (pView == NULL)
	{
		TRACE0("Error: Failed to create preview view.\n");
		return FALSE;
	}
	ASSERT_KINDOF(CPreviewView, pView);
	pView->m_pPreviewState = pState;        // save pointer

	pParent->OnSetPreviewMode(TRUE, pState);    // Take over Frame Window

	// Create the toolbar from the dialog resource
	pView->m_pToolBar = new CDialogBar;
	if (!pView->m_pToolBar->Create(pParent, MAKEINTRESOURCE(nIDResource),
		CBRS_TOP, AFX_IDW_PREVIEW_BAR))
	{
		TRACE0("Error: Preview could not create toolbar dialog.\n");
		pParent->OnSetPreviewMode(FALSE, pState);   // restore Frame Window
		delete pView->m_pToolBar;       // not autodestruct yet
		pView->m_pToolBar = NULL;
		pView->m_pPreviewState = NULL;  // do not delete state structure
		delete pView;
		return FALSE;
	}
	pView->m_pToolBar->m_bAutoDelete = TRUE;    // automatic cleanup

	if (!pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0,0,0), pParent, AFX_IDW_PANE_FIRST, &context))
	{
		TRACE0("Error: couldn't create preview view for frame.\n");
		pParent->OnSetPreviewMode(FALSE, pState);   // restore Frame Window
		pView->m_pPreviewState = NULL;  // do not delete state structure
		delete pView;
		return FALSE;
	}

	pState->pViewActiveOld = pParent->GetActiveView();
	CWrapperView* pActiveView = (CWrapperView*)pParent->GetActiveFrame()->GetActiveView();
	if (pActiveView != NULL)
		pActiveView->OnActivateView(FALSE, pActiveView, pActiveView);

	if (!pView->SetPrintView(pPrintView))
	{
		pView->OnPreviewClose();
		return TRUE;            // signal that OnEndPrintPreview was called
	}

	pParent->SetActiveView(pView);  // set active view - even for MDI

	// update toolbar and redraw everything
	pView->m_pToolBar->SendMessage(WM_IDLEUPDATECMDUI, (WPARAM)TRUE);
	pParent->RecalcLayout();            // position and size everything
	pParent->UpdateWindow();

	return TRUE;
}

// OnEndPrintPreview is here for swap tuning reasons
//  (see viewprev.cpp for complete preview mode implementation)
void CWrapperView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo,
			POINT, CMyPreviewView* pView)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pView);
	ASSERT(m_pFrameWnd != NULL);

	if (pView->m_pPrintView != NULL)
		OnEndPrinting(pDC, pInfo);

	CFrameWnd* pParent = m_pFrameWnd;
	ASSERT_VALID(pParent);

	// restore the old main window
	pParent->OnSetPreviewMode(FALSE, pView->m_pPreviewState);

	// Force active view back to old one
	pParent->SetActiveView(pView->m_pPreviewState->pViewActiveOld);
	if (pParent != GetParentFrame())
		OnActivateView(TRUE, this, this);   // re-activate view in real frame
	pView->DestroyWindow();     // destroy preview view
			// C++ object will be deleted in PostNcDestroy

	// restore main frame layout and idle message
	pParent->RecalcLayout();
	pParent->SendMessage(WM_SETMESSAGESTRING, (WPARAM)AFX_IDS_IDLEMESSAGE, 0L);
	pParent->UpdateWindow();
}

void CWrapperView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CView::OnEndPrinting(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMyPreviewView, CPreviewView)

CMyPreviewView::CMyPreviewView()
{
}

CMyPreviewView::~CMyPreviewView()
{
}

BEGIN_MESSAGE_MAP(CMyPreviewView, CPreviewView)
	//{{AFX_MSG_MAP(CMyPreviewView)
	ON_COMMAND(AFX_ID_PREVIEW_CLOSE, OnPreviewClose)
	ON_COMMAND(AFX_ID_PREVIEW_PRINT, OnPreviewPrint)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyPreviewView drawing

void CMyPreviewView::OnDraw(CDC* pDC)
{	
	CPreviewView::OnDraw(pDC);	
}

/////////////////////////////////////////////////////////////////////////////
// CMyPreviewView diagnostics

#ifdef _DEBUG
void CMyPreviewView::AssertValid() const
{
	
}

void CMyPreviewView::Dump(CDumpContext& dc) const
{
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyPreviewView message handlers

void CMyPreviewView::OnActivateView(BOOL bActivate, CView* pView1, CView *pView2)
{
	CPreviewView::OnActivateView(bActivate, pView1, pView2);
}

void CMyPreviewView::OnPreviewClose()
{
	CDlgMainFrame* pf=(CDlgMainFrame*)::AfxGetMainWnd();
//	CWinApp *pApp=AfxGetApp();	
	CWinThread *pThread = AfxGetThread( );
	pThread->m_pMainWnd=pf->pOldWnd;
	pf->DestroyWindow();
}

void CMyPreviewView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView) 
{	
	
	CPreviewView::OnEndPrintPreview(pDC, pInfo, point, pView);
}

void CMyPreviewView::OnPreviewPrint()
{
	m_pPrintView->SendMessage(WM_COMMAND, ID_FILE_PRINT);
	OnPreviewClose();               // force close of Preview

}
