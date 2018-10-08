#pragma once
#include "tgmsg.h"                  //消息和数据结构定义
#include "resource.h"
#include "GridCtrl_src/GridCtrl.h"  //包含表格控件的类

#include "messages.h"
#include "function.h"
#include "memshare.h"
#include "Tooltipwnd.h"
#include ".\ext_src\CJFlatComboBox.h"  //office 风格的COMBOX
#include "Dlgmainfrm.h"
#include "afxdtctl.h"
#include "editex.hxx"
#include ".\GridCtrl_src\gridcellcheck.h"
#include "utility/pspview.h"
#include "tg_other_structdef.h"
class CTgDocument;
/////////////////////////////////////////////////////////////////////////////
// CPageNotepadPlan dialog

class CPageNotepadPlan : public CPropertyPage
{
	//DECLARE_DYNCREATE(CPageNotepadPlan)
private:
    int  nSelectedNum;
	int  nSelectedList[MAX_STATIONS_PER_SECTION];
	int  sendTime;
	int  sendDate;
// Construction
public:
	int		m_StageNum;
	int		m_nCurStageNo;

	BOOL firstRun;
	void LoadWorkPhasePlan();
	BOOL CleanLocal();
	void SendMessageToCad(int flag);
	void commonControl();
	BOOL InitialGridCtrl();
	void FillGridCtrl();
	void UpdateShow(int phase_id);

    CTgDocument *m_pDoc;
 	CGridCtrl *m_pGridCtrl;
	int		m_nFixCols;
	int		m_nFixRows;
	int		m_nCols;
	int		m_nRows;
    int nUnitNum;
	UINT m_nPrevStyle;
	CEditEx m_edittemp1;
	CEditEx m_edittemp2;
	CEditEx m_edittemp3;

	
	void InitIdentify();

	CPageNotepadPlan(CTgDocument *pdoc);
	virtual ~CPageNotepadPlan();

// Dialog Data
	//{{AFX_DATA(CPageNotepadPlan)
	enum { IDD = IDD_PROPPAGE_NOTE_PLAN };
	CComboBox	m_cboWeagherEnd;
	CComboBox	m_cboWeagherStart;
	CDateTimeCtrl	m_timeCtrl;
	CDateTimeCtrl	m_DateCtrl;
	CEditEx m_Content;
	CString	m_Start_station;
	CString	m_End_station;
	CString	m_strCommander;
	CString	m_strSendDate;
	CString	m_strSendTime;
	CString	m_strSendTimeEx;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageNotepadPlan)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
 
	bool bAllStationFlag;
	// Generated message map functions
	//{{AFX_MSG(CPageNotepadPlan)
	virtual BOOL OnInitDialog();
	afx_msg void OnPhase1();
	afx_msg void OnPhase2();
	afx_msg void OnPhase3();
	afx_msg void OnSetfocusEditContent();
	afx_msg void OnSetfocusEditCommander();
	afx_msg void OnSetfocusEditTime();
	afx_msg void OnSetfocusEditSendTime();
	//}}AFX_MSG
	afx_msg void OnGrid();
	afx_msg void OnSendCurrent();
	afx_msg	void OnSave();
	afx_msg void OnLoad();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam);
	BOOL  ProcessMessage(const MsgWordPhasePlanSend *pBuf);
	BOOL  ProcessMessage(const MsgWordPhasePlanStatus *pBuf);
	afx_msg void  OnDestroy();
	DECLARE_MESSAGE_MAP()
private:
	CWnd *m_pNotifyWindow;
public:
	void InformParent();
	void SetNotifyWindow(CWnd *p) {m_pNotifyWindow=p;}
	void UpdateShowSendStatus(int phase_id,int entity_id);
	void RefreshSendStatusRow(int row);
};
