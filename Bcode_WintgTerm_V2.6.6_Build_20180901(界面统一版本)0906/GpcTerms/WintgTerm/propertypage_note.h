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

class CTgDocument;
class CPageNotepad : public CPropertyPage
{
	//DECLARE_DYNCREATE(CPageNotepad)

// Construction
private:
	// 协议
	BOOL Query_Notebook();
	BOOL Set_Notebook();
public:
	void PrintNotepadDirectly();
	void initStationList();
	int nStationList[64];
	int stationCount;

	void GetGridData(CString &note_content);
	CString parsecontent[19];//必须比内容多一个
	void fillGrid(int start_pos);
	int m_nRowCount;
	void parseNotes();
	int m_nTimeValue_JiaoBan;
	void initialGridCtrl();
//	CCFTimeEdit  m_cfTime;
	CGridCtrl   *m_pGridCtrl;

	CEditEx   m_edit_jiaoban;
	CEditEx   m_edit; 
	CString  print_content;
	unsigned long select_shift_id;
	void OnPrint(CDC* pDC,CPrintInfo* pInfo);
	int schd_type;
	CTgDocument *m_pDoc;
	void InitIdentify();
	CPageNotepad(CTgDocument *pdoc);
	virtual ~CPageNotepad();

// Dialog Data
	//{{AFX_DATA(CPageNotepad)
	enum { IDD = IDD_PROPPAGE_NOTE };
	CString	m_notebook;
	CString	m_cfTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageNotepad)
	public:
	virtual void OnOK();
	virtual BOOL OnApply();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageNotepad)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeNotebook();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnSetfocusNotebook();
	afx_msg void OnSetfocusEditContent();
	afx_msg void OnSetfocusEditTime();
	afx_msg void  OnDestroy();
	//}}AFX_MSG
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	CWnd *m_pNotifyWindow;
public:
	BOOL  ProcessMessage(const MsgNotePadFormat *pBuf);
	void SetNotifyWindow(CWnd *p) {m_pNotifyWindow=p;}
	void InformParent();
};

