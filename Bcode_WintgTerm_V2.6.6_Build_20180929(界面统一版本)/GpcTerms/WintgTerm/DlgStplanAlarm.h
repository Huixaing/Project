#if !defined(AFX_CDlgStplanAlarm_H__C2824465_ED28_4899_A981_257ED0328A34__INCLUDED_)
#define AFX_CDlgStplanAlarm_H__C2824465_ED28_4899_A981_257ED0328A34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgStplanAlarm.h : header file
//
#include <vector>

struct AlarmTM
{
	BYTE hour;
	BYTE minute;
	AlarmTM()
	{
		hour=0;
		minute=0;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CDlgStplanAlarm dialog
class CDlgStplanAlarm : public CDialog
{
// Construction
public:
	CDlgStplanAlarm(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CDlgStplanAlarm)
	enum { IDD = IDD_DLG_STPLAN_ALARM};
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgStplanAlarm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgStplanAlarm)
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	bool LoadStplanAlarmXML(int section_id);
	bool IsAlarm();
	void Alarm();
private:
	bool m_known;
	std::vector<AlarmTM> m_StplanAlarmTmList;
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDlgStplanAlarm_H__C2824465_ED28_4899_A981_257ED0328A34__INCLUDED_)
