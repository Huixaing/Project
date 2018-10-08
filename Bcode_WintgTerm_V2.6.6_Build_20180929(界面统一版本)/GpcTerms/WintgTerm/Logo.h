#pragma once
class  CLogo : public CWnd
{
// Construction
public:
	CLogo();
// Operations
public:
    void SetPicSize(const CRect& rt, UINT id, bool bRealSize = false);
	void DrawBitmapFile(CPaintDC *pDC,int left,int top,int w,int h);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogo)
	public:
	virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLogo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLogo)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool flag;
	CBrush m_Brush;
	CRect rect;
	CString strClassName;
	bool bRealSize;
	UINT bResourceId;
};