#if !defined(AFX_STATICLABEL_H__03AFE8F4_F03E_11D1_9D71_000021000CE6__INCLUDED_)
#define AFX_STATICLABEL_H__03AFE8F4_F03E_11D1_9D71_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// StaticLabel.h : header file
//
#define STATUS_ACTIVE	0
#define STATUS_DEAD		1
#define STATUS_ERROR	2

#define UP_STYLE			0
#define DOWN_STYLE		1
#define NORMAL_STYLE		2
/////////////////////////////////////////////////////////////////////////////
// CStaticLabel window

class  CStaticLabel : public CWnd
{
// Construction
public:
	CStaticLabel();

// Attributes
public:
	int m_nStatus;
	COLORREF m_clrText;
	COLORREF m_clrTextBk;
	COLORREF m_clrBkgnd;

	CBrush m_brushRed,m_brushNormal;
	CFont m_font;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticLabel)
	public:
	virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetWindowText(LPCTSTR text,int nFlag = 0);
	void SetLabel3DStyle(int n3DStyle);
	int m_nLabel3DStyle;
	void SetTextStyle(int nStyle);
	int m_nTextStyle;
	void SetFont(int nPoint,LPCTSTR sFontName = NULL);
	void SetFont(LOGFONT* lpLogFont);
	void ResetStatus(int nStatus);
	int GetStatus( );
	void SetStatus(int nStatus);
	virtual ~CStaticLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticLabel)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICLABEL_H__03AFE8F4_F03E_11D1_9D71_000021000CE6__INCLUDED_)
