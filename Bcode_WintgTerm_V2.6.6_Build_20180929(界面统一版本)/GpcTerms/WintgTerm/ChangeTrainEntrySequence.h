#if !defined(AFX_CHANGETRAINENTRYSEQUENCE_H__3B9250F3_94DA_48E9_98ED_180CFEF9A723__INCLUDED_)
#define AFX_CHANGETRAINENTRYSEQUENCE_H__3B9250F3_94DA_48E9_98ED_180CFEF9A723__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeTrainEntrySequence.h : header file
//
#include "tooltipwnd.h"
#include "wintgdoc.h"
#include "SectionStationTrainSequence.h"

extern void OsInitializeAllStationEntryLinkInform(int nAbsRuntimeInfoNum,struct _tagAbsRuntimeInform **abs_runtime_info);
extern void OsDeInitializeAllStationEntryLinkInform( );
extern ushort OsGetStationEntry(ushort usSourceStationNo,ushort usDestineStationNo,
						 ushort usDirFlag);
extern ushort OsGetStationEntryDirectionFlag(ushort usStationID,
		ushort usEntryNo);

struct _tagStationEntryLink {
	ushort usSourceStationNo;
	ushort usSourceStationEntryNo;
	ushort usDestineStationEntryNo;
	ushort usDestineStationNo;
	ushort usDirectionFlag;
};

struct _tagEntrySequence {
	ushort usChangeFlag;
	ushort usEntryID;
	ushort usFirstStationNo;
	ushort usSecondStationNo;
	ushort spare;
	ushort usTrainNum;
	char sTrainList[64][12];
};

struct _tagTrainStationEntryRunSequence {
	ushort usStationNo;
	ushort usEntryNum;
	struct _tagEntrySequence rec[1];
};

struct _tagTrainEntryRec {
	char lpszTrainID[12];
	long usRunDate;
	long usRunTime;
};

struct _tagMiddleStationRunSequence {
	ushort usStationNo;
	ushort usDirFlag;
	ushort usExitEntryNo;
	ushort usTrainRecNum;
	struct _tagTrainEntryRec rec[128];
};

extern ushort OsGetStationEntrylinkInfo(ushort usStationNo,ushort *usEntryNum,struct _tagStationEntryLink *sEntryLinkInfo);
/////////////////////////////////////////////////////////////////////////////
// CChangeTrainEntrySequence dialog

class CChangeTrainEntrySequence : public CDialog
{
// Construction
public:
	CChangeTrainEntrySequence(CWintgDoc *pDoc,CWnd* pParent = NULL);   // standard constructor

	CWintgDoc *m_pDoc;

	CListBox *m_sFocusEntryList;
	int m_nFocusSelection;
	int m_nTrainStationEntryRunSequenceNum;
	struct _tagTrainStationEntryRunSequence *info[64];
	void ChangeWindowSize(ushort usEntryNum);

	struct _tagMiddleStationRunSequence (*train_list)[ ];
// Dialog Data
	//{{AFX_DATA(CChangeTrainEntrySequence)
	enum { IDD = IDD_DLG_TRAIN_ENTRY_SEQU };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeTrainEntrySequence)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangeTrainEntrySequence)
	afx_msg void OnSesAck();
	afx_msg void OnSesCancel();
	afx_msg void OnSesDelete();
	afx_msg void OnSesDown();
	afx_msg void OnSesInsert();
	afx_msg void OnSelchangeSesStationlist();
	afx_msg void OnSesUp();
	virtual BOOL OnInitDialog();
	afx_msg void OnReceiveStationEntryRunSequence(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnSetfocusSesList1();
	afx_msg void OnSetfocusSesList2();
	afx_msg void OnSetfocusSesList3();
	afx_msg void OnSetfocusSesList4();
	afx_msg void OnSetfocusSesList5();
	afx_msg void OnSetfocusSesList6();
	afx_msg void OnSetfocusSesList7();
	afx_msg void OnSetfocusSesList8();
	afx_msg void OnSetfocusSesList9();
	afx_msg void OnSetfocusSesList10();
	afx_msg void OnSetfocusSesList11();
	afx_msg void OnSetfocusSesList12();
	afx_msg void OnSetfocusSesList13();
	afx_msg void OnSetfocusSesList14();
	afx_msg void OnSetfocusSesList15();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGETRAINENTRYSEQUENCE_H__3B9250F3_94DA_48E9_98ED_180CFEF9A723__INCLUDED_)
