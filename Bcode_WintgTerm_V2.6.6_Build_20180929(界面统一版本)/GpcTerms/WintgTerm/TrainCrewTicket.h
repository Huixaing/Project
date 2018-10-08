#if !defined(AFX_TRAINCREWTICKET_H__E55DBA43_5427_4B2F_A4BC_1B53DB0BD972__INCLUDED_)
#define AFX_TRAINCREWTICKET_H__E55DBA43_5427_4B2F_A4BC_1B53DB0BD972__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainCrewTicket.h : header file
//
#include <vector>
#include "resource.h"
#include "tgmsg.h"
class  CTgDocument;
/////////////////////////////////////////////////////////////////////////////
// CTrainCrewTicket dialog
class CTrainCrewTicket : public CDialog
{
// Construction
public:
	CTrainCrewTicket(CWnd* pParent, CTgDocument *pdoc, TRAIN_INDEX shift_index, CString trainid);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTrainCrewTicket)
	enum { IDD = IDD_DIALOG_CREW_TICKET };
	CListCtrl	m_engine_list;
	CListBox    m_list_box;
	//}}AFX_DATA
		
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainCrewTicket)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTrainCrewTicket)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual void PostNcDestroy();
	afx_msg void OnClose();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam); 
	afx_msg void OnBnClickedRefresh();
private:
	CTgDocument *m_pDoc;
	TRAIN_INDEX m_shift_index;
	CString m_trainid;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAINCREWTICKET_H__E55DBA43_5427_4B2F_A4BC_1B53DB0BD972__INCLUDED_)
