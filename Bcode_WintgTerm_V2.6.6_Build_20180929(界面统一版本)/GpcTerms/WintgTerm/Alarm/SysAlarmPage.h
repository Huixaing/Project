#if !defined(AFX_SYSALARMPAGE_H__1A4892E4_0407_4F43_AD09_0D31B8A90FDB__INCLUDED_)
#define AFX_SYSALARMPAGE_H__1A4892E4_0407_4F43_AD09_0D31B8A90FDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
#include "HeaderList.h"
#include "isocket.h"
#include <BaseMsg/clbth.h>

#pragma warning(disable:4786)

#include <list>
#include <map>
#include <vector>
using namespace std;

// SysAlarmPage.h : header file
//

struct ALARM_VISIBLE_STRUCT {
	DWORD bVisible : 1;	//可见
	DWORD bCannotChanged : 1;	//可变更

	ALARM_VISIBLE_STRUCT(DWORD dw = 1) { *(DWORD*)this = dw; }
	operator bool() { return bVisible; }
};

/////////////////////////////////////////////////////////////////////////////
// CSysAlarmPage dialog

class CSysAlarmPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSysAlarmPage)

// Construction
public:
	CSysAlarmPage();
	~CSysAlarmPage();

// Dialog Data
	//{{AFX_DATA(CSysAlarmPage)
	enum { IDD = IDD_SYSALARM };
//	CComboBox	m_SourceBox;
	CHeaderList	m_list;
	CHeaderList	m_listNeedAnswer;
	//}}AFX_DATA

    

	enum CLASS {
		APP_REMOTE = 0,
		SYS_REMOTE
		//SYS_LOCAL = 0,	// 
		//APP_LOCAL,
	};
	enum LEVEL {
		ERROR_LOG = 0,
		WARNING_LOG,
		INFORM_LOG,
		UNKNOWN_LOG,
	} ;

	struct ELEMENT {
		time_t tm;
		BYTE   nClass : 1;//CLASS
		BYTE   nLevel : 3; //LEVEL
		BYTE   bNeedRespond : 1;
		BYTE   bAnswered : 1;
		BYTE          : 2;
		INT  nAlarmID;
		CString szContent;
		CLBTH::ENTITY_ID SourceEntityID;
		CString szSourceTitle;
		CLBTH::MsgAlarm *pAlarm;
		ELEMENT()
			: tm(0), nClass(SYS_REMOTE), nLevel(INFORM_LOG), 
			  bNeedRespond(0), bAnswered(0), nAlarmID(0),
			  pAlarm(0)
		{}
	};

private:
	CImageList m_SmallImageList, m_StateImageList;
	CImageList m_SmallImageListNeedAnswer, m_StateImageListNeedAnswer;
	
	int m_imageWidth, m_imageHeight;

	//typedef list<ELEMENT> ALARM_DATA;
	//ALARM_DATA m_alarm_data;

	ALARM_VISIBLE_STRUCT m_bLevelOne, m_bLevelTwo, m_bLevelThree;
	ALARM_VISIBLE_STRUCT m_bClassSys, m_bClassApp;

    BOOL m_bUpdateScroll;
	BOOL m_bEnableFilter;

	BOOL	m_bOtherStationSel;
	map<int, bool> m_StationFilter;

	ALARM_VISIBLE_STRUCT	m_bOtherAlarmSel;
	map<int, bool> m_AlarmIDFilter;

//	DWORD m_SourceEntityID;

	//CToolBar m_wndToolBar;

private:
	BOOL AddElementToListBox(CHeaderList *plist,ELEMENT *pele, BOOL bEnableFilter=TRUE,BOOL bScroll=TRUE);
	void RefreshListNeedAnswer();
	bool IsAlarmHide(int nAlarmID, int nLevel, int nType, int nStaID);
	void SetAlarmFilter();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSysAlarmPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSysAlarmPage)
	virtual BOOL OnInitDialog();
	//afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkAlarmList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemclickAlarmList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickAlarmList(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnSelchangeSelectSourceentity();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCtcalarmTb();
	afx_msg void OnCtcalarmTb_UpdateUI(CCmdUI* pCmdUI);
	afx_msg void OnSysalarmSetfilter();
	afx_msg void OnUpdateSysalarmSetfilter(CCmdUI *pCmdUI);
	afx_msg void OnSysalarmUnsetfilter();
	afx_msg void OnUpdateSysalarmUnsetfilter(CCmdUI *pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT OnAlarm(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnAlarmSys();
	afx_msg void OnUpdateAlarmSys(CCmdUI *pCmdUI);
	afx_msg void OnAlarmTrain();
	afx_msg void OnUpdateAlarmTrain(CCmdUI *pCmdUI);
	afx_msg void OnAlarmUpdateScroll();
	afx_msg void OnUpdateAlarmUpdateScroll(CCmdUI *pCmdUI);
	afx_msg void OnAlarmShowlog();
	afx_msg void OnAlarmEmpty();
	afx_msg void OnAlarmLvl1();
	afx_msg void OnUpdateAlarmLvl1(CCmdUI *pCmdUI);
	afx_msg void OnAlarmLvl2();
	afx_msg void OnUpdateAlarmLvl2(CCmdUI *pCmdUI);
	afx_msg void OnAlarmLvl3();
	afx_msg void OnUpdateAlarmLvl3(CCmdUI *pCmdUI);
};


	struct ITEMDATA {
		DWORD nID : 31;
		DWORD bSel : 1;
		ITEMDATA(DWORD dw = 0) { *(int*)this = dw; }
		operator DWORD() { return *(int*)this; }
	};
class CFilterList : public CHeaderList
{
public:
	void AddItem(bool bSelected, int nID, LPCSTR szTitle);
	bool SetItemSelected(int nItem, bool bSelected);
	void SetAllItemSelected(bool bSelected);
	void ToggleItemSelected(int nItem);
	DWORD GetItemSelectedCount();
};

/////////////////////////////////////////////////////////////////////////////
// CSysAlarmFilterDlg dialog

class CSysAlarmFilterDlg : public CDialog
{
// Construction
public:
	CSysAlarmFilterDlg(map<int, bool>& stationList, map<int, bool>& alarmList, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSysAlarmFilterDlg)
	enum { IDD = IDD_SYSALARM_FILTER };
	CFilterList	m_list;
	CFilterList m_listAlarm;
	CButton m_Level[3];
	CButton m_Type[2];

	ALARM_VISIBLE_STRUCT	m_bLevel[3];
	ALARM_VISIBLE_STRUCT	m_bType[2];
	BOOL	m_bAllStationSel;
	BOOL	m_bAllAlarmSel;
	//}}AFX_DATA

	BOOL	m_bOtherStationSel;
	map<int, bool> m_StationFilter;

	ALARM_VISIBLE_STRUCT	m_bOtherAlarmSel;
	map<int, bool> m_AlarmIDFilter;

private:
	CImageList m_StateImageList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysAlarmFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSysAlarmFilterDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickStationList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickAlarmList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClassApp();
	afx_msg void OnClassSys();
	afx_msg void OnLevelOne();
	afx_msg void OnLevelTwo();
	afx_msg void OnLevelThree();
	afx_msg void OnSysalarmAllstation();
	afx_msg void OnSysalarmAllAlarm(); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};




// 报警响应采取CASCO消息方式, 存在的缺点
//   1、消息线程没有WINDOWS消息循环，无法显示DIALOG
//   2、报警响应后，不允许再次让GPC响应
// 报警响应采取回调函数方式
//   1、DIALOG生存于报警线程
//   2、报警响应后，很容易作记录。

#include <BaseMsg/clbth.h>
typedef void (*PFN_ALARM_ANSWER)(CLBTH::MsgAlarm*, CLBTH::ENTITY_ID, BOOL*);

#define DEFINE_ALARMMSG_PROC(classname, pAlarm, entitySource, pbProced) \
	static void ProcAlarm##classname(\
		CLBTH::BaseMsg* pAlarm, CLBTH::ENTITY_ID entitySource, BOOL* pbProced); \
	bool _bRegister##classname = registerAlarmsg( \
		CLBTH::classname::getTypeIDStatic(), \
		(PFN_ALARM_ANSWER)ProcAlarm##classname); \
	static void ProcAlarm##classname(\
		CLBTH::BaseMsg* pAlarm, CLBTH::ENTITY_ID entitySource, BOOL* pbProced)

typedef map<CLBTH::MSGHEAD::msgid_t, PFN_ALARM_ANSWER> SYSALARM_ANSWER_MAP;
AFX_EXT_CLASS bool registerAlarmsg(CLBTH::MSGHEAD::msgid_t msgid, PFN_ALARM_ANSWER pfn);


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSALARMPAGE_H__1A4892E4_0407_4F43_AD09_0D31B8A90FDB__INCLUDED_)
