#if !defined(AFX_SOFTKEYBOARDDLG_H__109F4891_5140_4957_8782_DC521EAC8E97__INCLUDED_)
#define AFX_SOFTKEYBOARDDLG_H__109F4891_5140_4957_8782_DC521EAC8E97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SoftKeyBoardDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSoftKeyBoardDlg dialog
#define  KB_TOP		 0x01
#define  KB_LEFT	 0x02
#define  KB_BOTTOM	 0x03
#define  KB_RIGHT	 0x04
//for initial the caps or lower 
#define  KB_CAPS	0x10
#define  KB_LOWER	0x20

class CSoftKeyBoardDlg : public CDialog
{
	// Construction
public:
	CSoftKeyBoardDlg(CWnd* pParent, BYTE nKBStyle );   // standard constructor
	~CSoftKeyBoardDlg();
	
	// Dialog Data
	//{{AFX_DATA(CSoftKeyBoardDlg)
	enum { IDD = IDD_KEYBOARD };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoftKeyBoardDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
public:
	static char m_KeyArray[36];
	static CString m_KeyText[52];
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CSoftKeyBoardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMyKeyDown(UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CWnd	*m_pParent;
	BYTE    m_nKBStyle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOFTKEYBOARDDLG_H__109F4891_5140_4957_8782_DC521EAC8E97__INCLUDED_)
