#if !defined(AFX_TOOLTIPWND_H__2C52D3E4_2F5B_11D2_8FC9_000000000000__INCLUDED_)
#define AFX_TOOLTIPWND_H__2C52D3E4_2F5B_11D2_8FC9_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ToolTipWnd.h : header file
//


struct BTOOLINFO {

	HWND	hwndTool;
	CString strToolText;
	COLORREF clrToolTextClr;

};


class CToolTipWnd : public CWnd
{

private:
// Construction
	LPCTSTR lpWndCls;
public:
	CToolTipWnd();
	HWND pCurrwnd;
	// Attributes
public:
	void RelayEvent(LPMSG);
	BOOL Create(CWnd*);
	bool m_bStuck;

	void AddTool(CWnd *pWnd, CString strText, COLORREF clrTextColor=NULL);
	void SetWidth(int iWidth) { m_iWidth = iWidth; }
	void SetHeight(int iHeight) { m_iHeight = iHeight; }
	void SetBkColor(COLORREF clrRef) { m_clrBkColor = clrRef; }
	void SetFrameColor(COLORREF clrRef) { m_clrFrameColor = clrRef; }
	void SetDefTextColor(COLORREF clrRef) { m_clrTextColor = clrRef; }
	void SetFontHeight(int iHeight) { m_iFontHeight = iHeight; }
	void SetFontName(CString strFontName) { m_strFontName = strFontName; }

private:
	CRgn rgn;
	CRgn rgnComb;
	CRgn rgnTri;
	CRect m_RectText;
	CFont m_fontText;
	CMapPtrToPtr m_ToolPtr;
	
	CString m_strText;
	bool m_bMouseIn;
	COLORREF m_clrTextColor;
	COLORREF m_clrBkColor;
	COLORREF m_clrFrameColor;
	int m_iWidth;
	int m_iHeight;
	int m_iFontHeight;
	CString m_strFontName;

	HWND m_hParentWnd;
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTipWnd)
	protected:
	//}}AFX_VIRTUAL
	

// Implementation
public:
	void ShowWindowMethos(int nCmdShow,CPoint pt,CString strText);
	virtual ~CToolTipWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTipWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTIPWND_H__2C52D3E4_2F5B_11D2_8FC9_000000000000__INCLUDED_)
