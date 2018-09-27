#if !defined(AFX_HEADERLIST_H__B58678E3_E9F5_11D1_9D68_000021000CE6__INCLUDED_)
#define AFX_HEADERLIST_H__B58678E3_E9F5_11D1_9D68_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HeaderList.h : header file
//

#define OFFSET_FIRST	2
#define OFFSET_OTHER	6

/////////////////////////////////////////////////////////////////////////////
// CHeaderList window

class  CHeaderList : public CListCtrl
{
// Construction
public:
	CHeaderList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeaderList)
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetCurSel( );
	void SetHeaderLabel(int nCount,TCHAR *label[ ],int *width,int *fmt);
	void RepaintSelectedItems(  );
	LPCTSTR MakeShortString(CDC * pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);
	void DrawListControl(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	int SetCurSel(int iSel);
	virtual ~CHeaderList();

	COLORREF m_clrText;
	COLORREF m_clrTextBk;
	COLORREF m_clrBkgnd;
	BOOL m_bFullRowSel;
	BOOL m_bClientWidthSel;
	int m_cxClient;
	int m_cxStateImageOffset;
	// Generated message map functions
protected:
	//{{AFX_MSG(CHeaderList)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEADERLIST_H__B58678E3_E9F5_11D1_9D68_000021000CE6__INCLUDED_)
