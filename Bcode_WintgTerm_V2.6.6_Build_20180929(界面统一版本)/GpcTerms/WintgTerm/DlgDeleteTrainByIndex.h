#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDeleteTrainByIndex.h : header file
//

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgDeleteTrainByIndex dialog

class CDlgDeleteTrainByIndex : public CDialog
{
	// Construction
public:
	CDlgDeleteTrainByIndex(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CUserPropertyDlg)
	enum { IDD = IDD_DLG_DELETE_TRAIN_BY_INDEX };
	long m_nTrainIndex;

	//}}AFX_DATA

	DECLARE_MESSAGE_MAP()
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
