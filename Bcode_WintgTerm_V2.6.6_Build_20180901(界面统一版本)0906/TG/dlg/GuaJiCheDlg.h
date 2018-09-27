#if !defined(AFX_GUAJICHEDLG_H__62E42A0C_8ACB_4A63_A87A_2C09AECEF5EE__INCLUDED_)
#define AFX_GUAJICHEDLG_H__62E42A0C_8ACB_4A63_A87A_2C09AECEF5EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GuaJiCheDlg.h : header file
//

#include "resource.h"
#include "tgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CGuaJiCheDlg dialog
struct EngineInfo
{
	CString     engine_id;            // 机车号
	CString     engine_name;
	long        engine_status;        // bit 7-6:  from dispatcher/subao
                                      // bit 5-4:  0: traction  1 - attach
									  // bit 0-3:  0:内燃 1:电力
	short       attach_sta;           // index of trainrecord
	short       unattach_sta;
};
class CGuaJiCheDlg : public CDialog
{
// Construction
public:
	CGuaJiCheDlg(CWnd *pParent);// standard constructor
public:
	void fillLeftContent(int curSel);
	BOOL initLeftColumn();
	BOOL InitialListCtrl();

// Dialog Data
	//{{AFX_DATA(CGuaJiCheDlg)
	enum { IDD = IDD_DLG_GUAJICHE };
	CListCtrl	m_List;
	CString	m_jiCheNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGuaJiCheDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGuaJiCheDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdate();
	afx_msg void OnDelete();
	afx_msg void OnAdd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CWordArray   locoId;
	CWordArray   cascoId;
	CStringArray locoPref;
	CStringArray locoName;

	int GetCascoIdByPref(CString pref);
	int GetLocoIdByPref(CString pref);
	CString GetLocoNameByPref(CString pref);
	int GetCascoIdByLocoId(int id);
public:
	afx_msg void OnBnClickedOk();
	
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnCbnSelchangeComboLocolType();
	afx_msg void OnBnClickedCancel();

public:
	CTG_TRAIN m_pTrain;
	ULONG m_train_index;
	CTGDoc *m_pDoc;

private:
  	void SendTrainUpdate();
	void SetCenterPosition();
	int m_engineCnt;
	EngineInfo m_engineAry[MAX_ENGINEER_NUM];
protected:
	virtual void PostNcDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GUAJICHEDLG_H__62E42A0C_8ACB_4A63_A87A_2C09AECEF5EE__INCLUDED_)
