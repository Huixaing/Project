// TimeEdit.cpp : implementation file
//

#include "stdafx.h"
#include "TimeEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeEdit

CTimeEdit::CTimeEdit()
{
}

CTimeEdit::~CTimeEdit()
{
}


BEGIN_MESSAGE_MAP(CTimeEdit, CEdit)
	//{{AFX_MSG_MAP(CTimeEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeEdit message handlers

void CTimeEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	static CString sep(_T(",.:;|"));
	// TODO: Add your message handler code here and/or call default
	if(nChar >= 0x30 && nChar <= 0x39) 
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	else if(sep.Find(nChar) != -1) 
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	else if(nChar == VK_HOME || nChar == VK_END || 
		nChar == VK_BACK ||	nChar == VK_INSERT || 
		nChar == VK_DELETE ||nChar == VK_RIGHT ||
		nChar == VK_LEFT)
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	else MessageBeep(MB_ICONASTERISK);
}
