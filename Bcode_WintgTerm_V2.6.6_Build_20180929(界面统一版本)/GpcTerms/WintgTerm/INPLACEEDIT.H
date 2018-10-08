
#if !defined(AFX_IMPLACEEDIT_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_)
#define AFX_IMPLACEEDIT_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000// InPlaceEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit window

class CInPlaceEdit : public CEdit
{
// Construction
public:
        CInPlaceEdit(int iItem, int iSubItem, CString sInitText);

// Attributes
public:

// Operations
public:

// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CInPlaceEdit)
        public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        //}}AFX_VIRTUAL

// Implementation
public:
        virtual ~CInPlaceEdit();
		void CalculateSize();

        // Generated message map functions
protected:
        //{{AFX_MSG(CInPlaceEdit)
        afx_msg void OnKillFocus(CWnd* pNewWnd);
        afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
private:
        int m_iItem;
        int m_iSubItem;
        CString m_sInitText;
        BOOL    m_bESC;         // To indicate whether ESC key was pressed
};

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_IMPLACEEDIT_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_)
