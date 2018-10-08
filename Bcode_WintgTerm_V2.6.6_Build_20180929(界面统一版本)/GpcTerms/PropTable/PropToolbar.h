
/////////////////////////////////////////////////////////////////////////////
// CPropToolbar window
#pragma once

class CPropToolbar : public CToolBarCtrl
{
// Construction
public:
	CPropToolbar();

// Attributes
public:
	void InsertSeparateBtn(UINT index); //index 0 based,  

// Operations
public:
	virtual BOOL Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

private:
	CString NeedText(UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult);

// Implementation
public:
	virtual ~CPropToolbar();

	// Generated message map functions
protected:
	afx_msg void OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	afx_msg void OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

