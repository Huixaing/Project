#ifndef  ToolTipWndEx_H
#define ToolTipWndEx_H

// ToolTipWndEx.h : header file
//

class CToolTipWndEx : public CWnd
{
	// Construction
public:
	CToolTipWndEx();
	virtual ~CToolTipWndEx();
	CString m_strText;
	CFont m_fontText;
// Implementation
public:
	BOOL Create(CWnd* wnd, int width, int height, bool wht=false);
	void HideTipWindow();
	void ShowTipWindow(int x, int y,CString strText);
	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTipWndEx)
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

#endif 
