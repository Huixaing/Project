// SingleDocTemplateForActiveX.cpp: implementation of the CSingleDocTemplateForActiveX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TGCtrl.h"
#include ".\tgDoc.h"
#include ".\tgFrame.h"

#include "SingleDocTemplateForActiveX.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSingleDocTemplateForActiveX::CSingleDocTemplateForActiveX(UINT nIDResource,
	CRuntimeClass* pDocClass,
	CRuntimeClass* pFrameClass,
	CRuntimeClass* pViewClass)
		: CSingleDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

CSingleDocTemplateForActiveX::~CSingleDocTemplateForActiveX()
{
}

CWnd* CSingleDocTemplateForActiveX::NewDVF(CWnd *pParent)
{
	ASSERT(m_pOnlyDoc == NULL);

	CDocument* pDocument = NULL;
	CFrameWnd* pFrame = NULL;
	BOOL bCreated = FALSE;      // => doc and frame created
	BOOL bWasModified = FALSE;

	// create a new document
	pDocument = CreateNewDocument();
	ASSERT(pFrame == NULL);     // will be created below
	bCreated = TRUE;

	if (pDocument == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return NULL;
	}
	ASSERT(pDocument == m_pOnlyDoc);
	
		
	if (pFrame == NULL)
	{
		ASSERT(bCreated);

		// create frame - set as main document frame
		BOOL bAutoDelete = pDocument->m_bAutoDelete;
		pDocument->m_bAutoDelete = FALSE;
		// don't destroy if something goes wrong
		pFrame = CreateNewFrame(pDocument, pParent);
		pDocument->m_bAutoDelete = bAutoDelete;
		if (pFrame == NULL)
		{
			AfxMessageBox("´´½¨CFrameWndÊ§°Ü!");
			delete pDocument;       // explicit delete on error
			return NULL;
		}
	}
	
	// create a new document
	SetDefaultTitle(pDocument);
	((CTGDoc*)pDocument)->m_ptgctrl = (CTGCtrl*)pParent;
	((CTGDoc*)pDocument)->myPDataManage = &(((CTGFrame*)pFrame)->myDataManage);

	if (!pDocument->OnNewDocument())
	{
		// user has been alerted to what failed in OnNewDocument
		TRACE0("CDocument::OnNewDocument returned FALSE.\n");
		if (bCreated)
			pFrame->DestroyWindow();    // will destroy document
		return NULL;
	}
	
	((CTGFrame*)pFrame)->SetMyDocument((CTGDoc*)pDocument);
	((CTGFrame*)pFrame)->SetMyCtrl((CTGCtrl*)pParent);
	
	InitialUpdateFrame(pFrame, pDocument, TRUE);

	return pFrame;
}

CFrameWnd* CSingleDocTemplateForActiveX::CreateNewFrame(CDocument* pDoc, CWnd* pParent)
{
	if (pDoc != NULL)
		ASSERT_VALID(pDoc);
	// create a frame wired to the specified document

	ASSERT(m_nIDResource != 0); // must have a resource ID to load from
	CCreateContext m_context;
	m_context.m_pCurrentFrame = NULL;
	m_context.m_pCurrentDoc = pDoc;
	m_context.m_pNewViewClass = m_pViewClass;
	m_context.m_pNewDocTemplate = this;

	if (m_pFrameClass == NULL)
	{
		TRACE(traceAppMsg, 0, "Error: you must override CDocTemplate::CreateNewFrame.\n");
		ASSERT(FALSE);
		return NULL;
	}
	CFrameWnd* pFrame = (CFrameWnd*)m_pFrameClass->CreateObject();
	if (pFrame == NULL)
	{
		TRACE(traceAppMsg, 0, "Warning: Dynamic create of frame %hs failed.\n",
			m_pFrameClass->m_lpszClassName);
		return NULL;
	}
	ASSERT_KINDOF(CFrameWnd, pFrame);

	if (m_context.m_pNewViewClass == NULL)
		TRACE(traceAppMsg, 0, "Warning: creating frame with no default view.\n");

	((CTGFrame*)pFrame)->SetMyCtrl((CTGCtrl*)pParent);

	// create new from resource
	if (!pFrame->LoadFrame(m_nIDResource,
			WS_CHILD,//|WS_CLIPSIBLINGS,	//WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,   // default frame styles
			pParent, &m_context))
	{
		TRACE(traceAppMsg, 0, "Warning: CDocTemplate couldn't create a frame.\n");
		// frame will be deleted in PostNcDestroy cleanup
		return NULL;
	}

	// it worked !
	return pFrame;
}

