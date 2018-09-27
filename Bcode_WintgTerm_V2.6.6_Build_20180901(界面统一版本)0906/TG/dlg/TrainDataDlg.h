#if !defined(AFX_TRAINDATADLG_H__81669771_DB25_11D6_81DF_00C04F4A78E8__INCLUDED_)
#define AFX_TRAINDATADLG_H__81669771_DB25_11D6_81DF_00C04F4A78E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainDataDlg.h : header file
//
#include "tgdoc.h"
#include "resource.h"
#include "GridCtrl.h"
#include <afxdtctl.h>

/////////////////////////////////////////////////////////////////////////////
// CTrainDataDlg dialog

class CTrainDataDlg : public CDialog
{
// Construction
public:
	CTrainDataDlg(CTG_TRAIN* train, CTGDoc *pDoc, int tg_type, CWnd* pParent);   // standard constructor
    CTGDoc *m_pDoc;
 	CGridCtrl *m_pGridCtrl;
	int		m_nFixCols;
	int		m_nFixRows;
	int		m_nCols;
	int		m_nRows;
	int     m_nTgType;
	CTG_TRAIN m_Train;
	int m_RecordsPerPrintPage;
// Dialog Data
	//{{AFX_DATA(CTrainDataDlg)
	enum { IDD = IDD_TRAIN_DATA_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTrainDataDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void show_data(int col_count);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAINDATADLG_H__81669771_DB25_11D6_81DF_00C04F4A78E8__INCLUDED_)
