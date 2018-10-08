#if !defined(AFX_MANUALREPORTHISTORY_H__67390A36_5B38_4FBE_B62B_6F14D2F2E659__INCLUDED_)
#define AFX_MANUALREPORTHISTORY_H__67390A36_5B38_4FBE_B62B_6F14D2F2E659__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManualReportHistory.h : header file
//
#include "headerlist.h"
#include "timeedit.h"
/////////////////////////////////////////////////////////////////////////////
// CManualReportHistory dialog
class CTgDocument;
struct tagManualReportRec 
{
	ulong uTrainIndex;
	char  lpszTrainID[12];
	int   nDirectFlag;
	int   rec_index;
	long   nPlanArrivalDate;
	long   nPlanArrivalTime;
	long   nPlanDepartDate;
	long   nPlanDepartTime;
	long   nActualArrivalDate;
	long   nActualArrivalTime;
	long   nActualDepartDate;
	long   nActualDepartTime;
	
	ushort adjust_status;
	tagManualReportRec()
	{
		memset(this, 0, sizeof(tagManualReportRec));
	}
};

class CWintgViewMMI;
class CManualReportHistory : public CDialog
{
// Construction
public:
	CManualReportHistory(CWintgViewMMI *pMMI,int sectionno,int schetype,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CManualReportHistory)
	enum { IDD = IDD_DLG_MANUAL_REPORT };
	CTimeEdit	m_ctrlActualDepartTime;
	CTimeEdit	m_ctrlActualArriveTime;
	//}}AFX_DATA
	CHeaderList m_list;
	CWintgViewMMI *m_pMMI;

	int m_nCurrentSectionNo;
	int m_nCurrentStationNo;
	int m_nCurrentSel;
    int m_nScheduleType;
	int m_nCurrentSelDirect;
		
	void ShowRecord(int index);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManualReportHistory)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CManualReportHistory)
	afx_msg void OnMrpAck();
	afx_msg void OnSelchangeMrpStationlist();
	
	virtual BOOL OnInitDialog();
	
	afx_msg void OnMrpAlldir();
	afx_msg void OnMrpDowndir();
	afx_msg void OnMrpUpdir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetInfo(int station);
private:
	char * OsConvertTimetoStringIgnoreSecond(long uValue);

	void ShowStationMRPREC(int flag);
	void SendMessageToCad(tagManualReportRec *rec);
	std::vector<tagManualReportRec> mrprec;
public:
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnClose();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANUALREPORTHISTORY_H__67390A36_5B38_4FBE_B62B_6F14D2F2E659__INCLUDED_)
