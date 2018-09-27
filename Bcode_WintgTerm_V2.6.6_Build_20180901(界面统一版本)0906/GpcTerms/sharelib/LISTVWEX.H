#ifndef __LISTVIEWEXINCLUDE
#define __LISTVIEWEXINCLUDE
#include "afxcview.h"
#include "mylist.h"

class CListViewEx : public CListView
{
	DECLARE_DYNCREATE(CListViewEx)

// Construction
public:
	CListViewEx();

// Attributes
protected:
	BOOL m_bFullRowSel;

public:
	BOOL SetFullRowSel(BOOL bFillRowSel);
	BOOL GetFullRowSel();

	BOOL m_bClientWidthSel;

// Overrides
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListViewEx)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListViewEx();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	static LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);
	void RepaintSelectedItems();

// Implementation - client area width
	int m_cxClient;

// Implementation - state icon width
	int m_cxStateImageOffset;
	afx_msg LRESULT OnSetImageList(WPARAM wParam, LPARAM lParam);

// Implementation - list view colors
	COLORREF m_clrText;
	COLORREF m_clrTextBk;
	COLORREF m_clrBkgnd;
	afx_msg LRESULT OnSetTextColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTextBkColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam);

// Generated message map functions
protected:
	//{{AFX_MSG(CListViewEx)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
    CImageList   m_imagelistbig;
	CImageList   m_imagelistsmall;
	CImageList   m_imageliststate;
protected:
	virtual int EditItem(int id,int flag){return 0;};

public:
	int	  m_type;
	int	  m_lvimask;
	int   m_colnum;
	BOOL  SetViewType(DWORD dwViewType);
	DWORD GetViewType();
	void  CreateBigImageList(UINT idBitmap,int size,COLORREF maskcolor=RGB(255,255,255));
	void  CreateSmallImageList(UINT idBitmap,int size,COLORREF maskcolor=RGB(255,255,255));
	void  CreateStateImageList(UINT idbitmap,int size,COLORREF maskcolor=RGB(255,255,255));
	void  SetColumn(MYCOLUMN  c[],int num);
	void  AddRow(MYROW  *r);
	void  ClearAll();
        void  SaveToFile(FILE *fp);
};

/////////////////////////////////////////////////////////////////////////////
#endif
