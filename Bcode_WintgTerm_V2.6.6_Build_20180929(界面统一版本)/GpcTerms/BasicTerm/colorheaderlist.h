#if !defined(AFX_ColorHeaderList_H__B58678E3_E9F5_11D1_9D68_000021000CE6__INCLUDED_)
#define AFX_ColorHeaderList_H__B58678E3_E9F5_11D1_9D68_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ColorHeaderList.h : header file
//

#define OFFSET_FIRST	2
#define OFFSET_OTHER	6

#define UM_SORT_MANAGE_LIST		WM_USER + 100

struct _tagUserSetColor {
	COLORREF m_clrUserSetText;
	COLORREF m_clrUserSetTextBk;
};

/////////////////////////////////////////////////////////////////////////////
// CColorHeaderList window

class  CColorHeaderList : public CListCtrl
{
// Construction
public:
	CColorHeaderList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorHeaderList)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetColumnColor(int nColumnNum,struct _tagUserSetColor *clrinfo);
	int GetCurSel( );
	void SetHeaderLabel(int nCount,TCHAR *label[ ],int *width,int *fmt);
	void RepaintSelectedItems(  );
	LPCTSTR MakeShortString(CDC * pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);
	void DrawListControl(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	int SetCurSel(int iSel);
	virtual ~CColorHeaderList();

	COLORREF m_clrText;
	COLORREF m_clrTextBk;
	COLORREF m_clrBkgnd;

	int m_nUserSetColorNum;
	int m_nAllocColorNum;
	struct _tagUserSetColor *m_clrUserSet[2048];

	int m_nAllocMemoryNum;
	char *m_nAllocMemory[32];

	BOOL m_bFullRowSel;
	BOOL m_bClientWidthSel;
	int m_cxClient;
	int m_cxStateImageOffset;
	// Generated message map functions
protected:
	//{{AFX_MSG(CColorHeaderList)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ColorHeaderList_H__B58678E3_E9F5_11D1_9D68_000021000CE6__INCLUDED_)
