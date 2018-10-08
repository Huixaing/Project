// PropTreeItemEdit.cpp : implementation file
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.

#include "stdafx.h"
#include "proptree.h"
#include "PropTreeItemDateTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropTreeItemDateTime

CPropTreeItemDateTime::CPropTreeItemDateTime()
{
	m_tTimeValue=0;
}

CPropTreeItemDateTime::~CPropTreeItemDateTime()
{
}


BEGIN_MESSAGE_MAP(CPropTreeItemDateTime, CDateTimeCtrl)
	//{{AFX_MSG_MAP(CPropTreeItemDateTime)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropTreeItemDateTime message handlers
void CPropTreeItemDateTime::DrawAttribute(CDC* pDC, const RECT& rc)
{
	ASSERT(m_pProp!=NULL);

	// verify the window has been created
	if (!IsWindow(m_hWnd))
	{
		TRACE0("CPropTreeItemDateTime::DrawAttribute() - The window has not been created\n");
		return;
	}

//	pDC->SelectObject(IsReadOnly() ? m_pProp->GetNormalFont() : m_pProp->GetBoldFont());
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);

	CRect r = rc;
	if( m_tTimeValue<=0)
	{
		pDC->DrawText("", r, DT_SINGLELINE | DT_VCENTER);
		return;
	}
	CString s;
	
	CTime timeTime;
	GetTime(timeTime);
	 if ((GetStyle() & DTS_TIMEFORMAT) == DTS_TIMEFORMAT)
         s = timeTime.Format(_T("%X"));
      else
         s = timeTime.Format(_T("%x"));

	if (IsReadOnly())
	{
		COLORREF oldColor = pDC->SetTextColor(RGB(128, 128, 128));
		pDC->DrawText(s, r, DT_SINGLELINE | DT_VCENTER);
		pDC->SetTextColor(oldColor);
	}
	else
	{
		pDC->DrawText(s, r, DT_SINGLELINE | DT_VCENTER);
	}
}

LPARAM CPropTreeItemDateTime::GetItemValue()
{
	CTime curtm;
	GetTime(curtm);
	return (LPARAM)curtm.GetTime();  
}

int  CPropTreeItemDateTime::GetRelaItemValue() // cuihu INT value
{
	return (int)GetItemValue();
}

void CPropTreeItemDateTime::SetRelaItemValue(int va)
{
	SetItemValue((LPARAM)va);
}

void CPropTreeItemDateTime::SetItemValue(LPARAM lParam)
{
	m_tTimeValue=lParam;
	if(lParam<=0)
	{
		return;
	}
	CTime reftm(1980,1,1,0,0,0);
	if(m_tTimeValue<reftm.GetTime())
		m_tTimeValue+=reftm.GetTime(); 
	CTime curtm;
	curtm=(time_t)m_tTimeValue;
	SetTime(&curtm);
}


void CPropTreeItemDateTime::OnMove()
{
	if (IsWindow(m_hWnd))
		SetWindowPos(NULL, m_rc.left, m_rc.top-1, m_rc.Width(), m_rc.Height()+1, SWP_NOZORDER|SWP_NOACTIVATE);
}


void CPropTreeItemDateTime::OnRefresh()
{
	//if (IsWindow(m_hWnd))
	//	SetWindowText(m_sEdit);
}


void CPropTreeItemDateTime::OnCommit()
{
	// hide edit control
	ShowWindow(SW_HIDE);

	// store edit text for GetItemValue
	//GetWindowText(m_sEdit);
}


void CPropTreeItemDateTime::OnActivate()
{
	// Check if the edit control needs creation
	if (!IsWindow(m_hWnd))
	{
		DWORD dwStyle;

		dwStyle = WS_CHILD | ES_AUTOHSCROLL;
		Create(dwStyle, m_rc, m_pProp->GetCtrlParent(), GetCtrlID());
		SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);
	}
	//modified for effect when active edit control, skybird, 2004.08.06
	//SetWindowPos(NULL, m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);
	SetWindowPos(NULL, m_rc.left, m_rc.top , m_rc.Width(), m_rc.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);
	//TRACE("when active: (%d, %d, %d, %d) for %s\n", m_rc.left, m_rc.top, m_rc.right, m_rc.bottom, m_sEdit);
	SetFocus();
}


UINT CPropTreeItemDateTime::OnGetDlgCode() 
{
	return CDateTimeCtrl::OnGetDlgCode()|DLGC_WANTALLKEYS;
}


void CPropTreeItemDateTime::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar==VK_RETURN)
		CommitChanges();
	
	CDateTimeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CPropTreeItemDateTime::OnKillfocus(NMHDR *pNMHDR, LRESULT *pResult) 
{
	CommitChanges();	
}

BOOL CPropTreeItemDateTime::CreateCtrl(DWORD dwStyle)
{
	ASSERT(m_pProp!=NULL);

	if (IsWindow(m_hWnd))
		DestroyWindow();

	// force as not visible child window
	dwStyle = (WS_CHILD|dwStyle) & ~WS_VISIBLE;

	if (!Create(dwStyle, CRect(0,0,0,0), m_pProp->GetCtrlParent(), GetCtrlID()))
	{
		TRACE0("CPropTreeItemDateTime::CreateCtrl() - failed to create combo box\n");
		return FALSE;
	}

	SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);

	return TRUE;
}