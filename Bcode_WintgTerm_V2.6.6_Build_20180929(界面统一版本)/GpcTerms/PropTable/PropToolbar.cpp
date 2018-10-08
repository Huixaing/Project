
#include "stdafx.h"
#include "afxpriv.h"
#include "resource.h"
#include "PropToolbar.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropToolbar

//you need add your command id to here, and add the first item and last item to message map
static const UINT toolbarItems[] =
{
	ID_PROP_CATEGORIZE,
	ID_PROP_ALPHABETIC
};


CPropToolbar::CPropToolbar()
{

}

CPropToolbar::~CPropToolbar()
{

}


BEGIN_MESSAGE_MAP(CPropToolbar, CToolBarCtrl)
	//{{AFX_MSG_MAP(CPropToolbar)
	ON_NOTIFY_RANGE( TTN_NEEDTEXTA, ID_PROP_CATEGORIZE, ID_PROP_ALPHABETIC, OnNeedTextA)
	ON_NOTIFY_RANGE( TTN_NEEDTEXTW, ID_PROP_CATEGORIZE, ID_PROP_ALPHABETIC, OnNeedTextW)

	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


BOOL CPropToolbar::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	BOOL bRet = CToolBarCtrl::Create(dwStyle, rect, pParentWnd, nID);

	int btnCount = sizeof( toolbarItems) / sizeof(toolbarItems[0]);

	VERIFY(AddBitmap(btnCount, nID) != -1);

	TBBUTTON *pTBButton = new TBBUTTON[btnCount];

	for (int nIndex = 0; nIndex < btnCount; nIndex++)
	{
		CString string;
		string.LoadString(nIndex + toolbarItems[nIndex]);

		// Add second '\0'
		int nStringLength = string.GetLength() + 1;
		TCHAR * pString = string.GetBufferSetLength(nStringLength);
		pString[nStringLength] = 0;

		VERIFY((pTBButton[nIndex].iString = AddStrings(pString)) != -1);

		string.ReleaseBuffer();

		pTBButton[nIndex].fsState = TBSTATE_ENABLED;
		pTBButton[nIndex].fsStyle = TBSTYLE_BUTTON | TBSTYLE_CHECK;
		pTBButton[nIndex].dwData = 0;
		pTBButton[nIndex].iBitmap = nIndex;
		pTBButton[nIndex].idCommand = toolbarItems[nIndex];
	}
	
	VERIFY(AddButtons(btnCount, pTBButton));
	
	delete [] pTBButton;

	return bRet;
}

void  CPropToolbar::InsertSeparateBtn(UINT index) //index 0 based
{
	ASSERT(index < (UINT) GetButtonCount());

	TBBUTTON sepButton;

	sepButton.idCommand = 0;
	sepButton.fsStyle = TBSTYLE_SEP;
	sepButton.fsState = TBSTATE_ENABLED;
	sepButton.iString = 0;
	sepButton.iBitmap = 0;
	sepButton.dwData = 0;

	VERIFY(InsertButton(index, &sepButton));
}


/////////////////////////////////////////////////////////////////////////////
// CPropToolbar message handlers



// Helper function for tooltips

CString CPropToolbar::NeedText( UINT nID, NMHDR * pNotifyStruct, LRESULT * /* lResult */ )
{
	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct ;
	ASSERT(nID == lpTTT->hdr.idFrom);

	CString toolTipText;
	toolTipText.LoadString(nID);

	// szText length is 80
	int nLength = (toolTipText.GetLength() > 79) ? 79 : toolTipText.GetLength();

	toolTipText = toolTipText.Left(nLength);

	return toolTipText;
}

void CPropToolbar::OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CString toolTipText = NeedText(nID, pNotifyStruct, lResult);

	LPTOOLTIPTEXTW lpTTT = (LPTOOLTIPTEXTW)pNotifyStruct;

#ifndef _UNICODE
	mbstowcs(lpTTT->szText,(LPCSTR)toolTipText, toolTipText.GetLength() + 1);
#else
	_tcsncpy(lpTTT->szText, toolTipText, toolTipText.GetLength() + 1);
#endif
}

void CPropToolbar::OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CString toolTipText = NeedText(nID, pNotifyStruct, lResult);

	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct;

	_tcscpy(lpTTT->szText,(LPCTSTR)toolTipText);
}


///////////////////////////////////////////////////////////////////////
// This has been overridden so we can handle the tooltip TTN_NEEDTEXT//
// notification message                                              //
///////////////////////////////////////////////////////////////////////


BOOL CPropToolbar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(pResult != NULL);
	NMHDR* pNMHDR = (NMHDR*)lParam;
	HWND hWndCtrl = pNMHDR->hwndFrom;

	// get the child ID from the window itself
	// UINT nID = _AfxGetDlgCtrlID(hWndCtrl);

	//////////////////////////////////////////////////////////////////
	// If TTN_NEEDTEXT we cannot get the ID from the tooltip window //
	//////////////////////////////////////////////////////////////////

	int nCode = pNMHDR->code;

	// if it is the following notification message
	// nID has to obtained from wParam
	//

	if (nCode == TTN_NEEDTEXTA || nCode == TTN_NEEDTEXTW)
	{
		UINT nID;   // = _AfxGetDlgCtrlID(hWndCtrl);
		nID = (UINT)wParam;


		ASSERT((UINT)pNMHDR->idFrom == (UINT)wParam);
		UNUSED(wParam);  // not used in release build
		ASSERT(hWndCtrl != NULL);
		ASSERT(::IsWindow(hWndCtrl));

		if (AfxGetThreadState()->m_hLockoutNotifyWindow == m_hWnd)
			return TRUE;        // locked out - ignore control notification

	// reflect notification to child window control
		if (ReflectLastMsg(hWndCtrl, pResult))
			return TRUE;        // eaten by child

		AFX_NOTIFY notify;
		notify.pResult = pResult;
		notify.pNMHDR = pNMHDR;
		return OnCmdMsg(nID, MAKELONG(nCode, WM_NOTIFY), &notify, NULL);
	}

	return CToolBarCtrl::OnNotify(wParam, lParam, pResult);
}


void CPropToolbar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if ( HitTest(&point) >= 0) //current click on buttton items
	{
		CToolBarCtrl::OnLButtonDblClk(nFlags, point);
	}

	//take out of  left button double click, to avoid redundant shadow on other windows
	//CToolBarCtrl::OnLButtonDblClk(nFlags, point);

}
