#if !defined(AFX_BASETABCTRL_H__C45DF303_FBBC_44DF_861D_9EA155ACD89F__INCLUDED_)
#define AFX_BASETABCTRL_H__C45DF303_FBBC_44DF_861D_9EA155ACD89F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBaseTabCtrl window
#define MAX_PAGE_COUNT 16

class AFX_EXT_CLASS CBaseTabCtrl : public CTabCtrl
{
// Construction
public:
	CBaseTabCtrl();
// Attributes
public:
	INT m_nPageCount;
	//页面属性
	INT m_nCurPage;
	UINT m_PageIDDs[MAX_PAGE_COUNT];
	UINT m_BitmapIDDs[MAX_PAGE_COUNT];
	CDialog* m_pPages[MAX_PAGE_COUNT];
	//图标属性
	CImageList* m_pImagelist;

// Operations
public:
	BOOL DrawTabText(CDC *pDC, const char *lpszText, const COLORREF rgb, CRect *pRect);
	BOOL DrawBkBitmap( CDC *pDC, const int iTab, const CRect *pRect);
	BOOL DrawTabIcon(CDC *pDC, const TC_ITEM tci, CRect *pRect );

private:
	COLORREF m_curItemColor;
	COLORREF m_itemColor;
	COLORREF m_bkColor;
	int m_nYOffset;
	int m_nXOffset;
	CFont m_textFont;
	COLORREF m_textColor;

	void SetRect(int nIndex);
protected:
	BOOL OnCommPaint();
	BOOL OnButStylePaint();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseTabCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetItemColor(COLORREF rgb);	// 设置未选中项的颜色
	void SetCurItemColor(COLORREF rgb);	// 设置选中项的颜色
	void SetTextYOffset(int y);			// 设置文本在Y方向上的偏移
	void SetTextXOffset(int x);			// 设置文本在X方向上的偏移
	void SetTextFont(HFONT hFont);		// 设置文本的字体
	void SetTextFont(LOGFONT *logfont);	// 设置文本的字体
	void SetTextColor(COLORREF rgb);	// 设置文本的颜色
	void SetBkColor( COLORREF clr );	// 设置Tab控件的背景颜色
	/*添加一个对话框控件到TAB页*/
	BOOL AddPage(LPCTSTR lpTitle, CDialog *pDialog, UINT ID, INT nIcon=-1, UINT nBkBitmapID=0);
	BOOL SetImageList(CImageList* pImageList);
	BOOL SetCurPage( int nIndex );
	BOOL UpdateData( BOOL flag );
	BOOL SetImageList( int nX, int nY, UINT uStartID, UINT uEndID );
	CDialog* GetPage(int nIndex);
	virtual ~CBaseTabCtrl();
	// Generated message map functions
protected:
	//{{AFX_MSG(CBaseTabCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASETABCTRL_H__C45DF303_FBBC_44DF_861D_9EA155ACD89F__INCLUDED_)
