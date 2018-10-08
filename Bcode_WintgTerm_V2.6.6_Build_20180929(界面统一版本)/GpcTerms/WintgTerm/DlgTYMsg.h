#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDlgTYMsg.h : header file
//
#include "resource.h"
#include "TgDocument.h"
#include <vector>
/////////////////////////////////////////////////////////////////////////////
// CDlgTYMsg dialog

class CDlgTYMsg : public CDialog
{
	// Construction
public:
	CDlgTYMsg(CTgDocument *pdoc,CWnd* pParent = NULL);   // standard constructor
	~CDlgTYMsg();
	void  UpdateDisplay(const char *str);
	// Dialog Data
	//{{AFX_DATA(CDlgTYMsg)
	enum { IDD = IDD_DLG_TYMSG };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTYMsg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTYMsg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTymsgFinish();

public:
	void SetNotReady();
	void SetDlgWindowFocus();
	void SetThisPacketInfo(int src, CString msgid, int count, CString jht);
	bool AddSeq(int src, CString msgid, int count, int nSeq);
	bool IsNotReady() {return m_bReady;}
	int  GetRecSeqCount() {return m_SeqList.size();}
private:
	bool    m_bReady; // true
	long     m_nSRCCODE;
	CString m_nMSGID;
	int     m_nCount; // 特运计划数量
	CString m_nJHTNM; // 计划台台号
	CTgDocument *m_pDoc;
	std::vector<int> m_SeqList; // 追踪序号
};