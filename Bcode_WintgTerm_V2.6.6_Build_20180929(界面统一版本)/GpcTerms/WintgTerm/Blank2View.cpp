// Blank2View.cpp : implementation file
//

#include "stdafx.h"
#include "Blank2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBlank2View

IMPLEMENT_DYNCREATE(CBlank2View, CView)

CBlank2View::CBlank2View()
{
}

CBlank2View::~CBlank2View()
{
}


BEGIN_MESSAGE_MAP(CBlank2View, CView)
	//{{AFX_MSG_MAP(CBlank2View)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlank2View drawing

void CBlank2View::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CBlank2View diagnostics

#ifdef _DEBUG
void CBlank2View::AssertValid() const
{
	CView::AssertValid();
}

void CBlank2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBlank2View message handlers
