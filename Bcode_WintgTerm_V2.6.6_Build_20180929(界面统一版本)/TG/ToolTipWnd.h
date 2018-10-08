#if !defined(AFX_TOOLTIPWND_H__2C52D3E4_2F5B_11D2_8FC9_000000000000__INCLUDED_)
#define AFX_TOOLTIPWND_H__2C52D3E4_2F5B_11D2_8FC9_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ToolTipWnd.h : header file
//

class CToolTipWnd : public CWnd
{
	// Construction
public:
	CToolTipWnd();
	virtual ~CToolTipWnd();
	CString m_strText;
	CFont m_fontText;
// Implementation
public:
	BOOL Create(CWnd* wnd, int width, int height, bool wht=false);
	void HideTipWindow();
	void ShowTipWindow(const CPoint& pt,CString strText);
	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTipWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Attributes
private:
	CBrush bsBkgrdBrush;
	CBrush bsFrameBrush;
	COLORREF m_clrTextColor;
	int m_iWidth;
	int m_iHeight;
	LPCTSTR lpWndCls;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTIPWND_H__2C52D3E4_2F5B_11D2_8FC9_000000000000__INCLUDED_)
