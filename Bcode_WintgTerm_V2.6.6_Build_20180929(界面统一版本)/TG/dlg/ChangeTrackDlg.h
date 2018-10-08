#if !defined(AFX_CHANGETRACKDLG_H__5A084FDE_A384_4B48_8546_57D84F491A6E__INCLUDED_)
#define AFX_CHANGETRACKDLG_H__5A084FDE_A384_4B48_8546_57D84F491A6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeTrackDlg.h : header file
//
#include "resource.h"
#include "tgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CChangeTrackDlg dialog

class CChangeTrackDlg : public CDialog
{
// Construction
public:
	CChangeTrackDlg(CWnd* pParent);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CChangeTrackDlg)
	enum { IDD = IDD_DLG_CHANGE_TRACK };
		// NOTE: the ClassWizard will add data members here
	CComboBox	m_combo_arrive_track;
	CComboBox	m_combo_depart_track;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeTrackDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CChangeTrackDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	TgTRAIN_RECORD m_rec;
	ULONG m_train_index;
	int   m_rec_index;
	int m_foucs_station;
	CTGDoc *m_pDoc;

private:
	void InitIdentify();
  	int SendTrainUpdate();
	void SetCenterPosition();
protected:
	virtual void PostNcDestroy();
	afx_msg void OnCbnSelArriveSide();
	afx_msg void OnCbnSelDepartSide();
public:
	afx_msg void OnBnClickedCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGETRACKDLG_H__5A084FDE_A384_4B48_8546_57D84F491A6E__INCLUDED_)
