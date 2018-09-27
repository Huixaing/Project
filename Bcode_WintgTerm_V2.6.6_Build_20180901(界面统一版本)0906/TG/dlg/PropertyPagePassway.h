#if !defined(AFX_PROPERTYPAGEPASSWAY_H__327BDD70_8E4C_47A9_8C22_BF1C4DA49A66__INCLUDED_)
#define AFX_PROPERTYPAGEPASSWAY_H__327BDD70_8E4C_47A9_8C22_BF1C4DA49A66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPagePassway.h : header file
#include "..\mvc\wintgdoc.h"
#include "tgmsg.h"                  //消息和数据结构定义
#include "resource.h"
#include "GridCtrl_src/GridCtrl.h"  //包含表格控件的类
#include "GridCtrl_src/GridURLCell.h"///////////////////////
#include "GridCtrl_src/GridCellCombo.h"///////////////////////

#include "messages.h"
#include "function.h"
#include "memshare.h"
//#include "Tooltipwnd.h"
#include ".\FlatCombo\CJFlatComboBox.h"  //office 风格的COMBOX
//CCJFlatComboBox	m_sideCtrl;

//#include "GRIDCTRL_SRC\GridCtrl.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayAppoint dialog

 
class CPropertyPagePasswayAppoint : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropertyPagePasswayAppoint)

// Construction
public:
 
	int Flag_moredata;
	char * Get_Passwayname(short id);
	short Get_Passwayid(char * name);
	CStringArray options3;
    CStringArray options4;
    CStringArray options5;
    CStringArray options6;
  
	void Del_Utility(short no);
	void Set_Utility();
	void InitCell(int row);
	void SetBlankCells(int Start);
	void Show_Utility(int no,int row);
 	void LoadUtility(int consoleid);
    // CComboBox m_Combo1;/////////////////////
	CGridCtrl *m_pGridCtrl;
    int		m_nFixCols;   //固定列
	int		m_nFixRows;   //固定行
	int		m_nCols;      //列
	int		m_nRows;      //行
	int ndialogInited;
    BOOL    Flag;

    //CToolTipWnd m_ctrlTooltipWnd;
	CPropertyPagePasswayAppoint();
	~CPropertyPagePasswayAppoint();

// Dialog Data
	//{{AFX_DATA(CPropertyPagePasswayAppoint)
	enum { IDD = IDD_DLG_PASSWAY_APPOINT };
	CStatic	m_Frame1;
	CButton	m_saveCtrl;
	CButton	m_newCtrl;
	CButton	m_deleteCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPagePasswayAppoint)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL CheckSameUtility(int no);
 
	// Generated message map functions
	//{{AFX_MSG(CPropertyPagePasswayAppoint)
	afx_msg void OnNew();
	virtual BOOL OnInitDialog();
    afx_msg void OnGridItemChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnSave();
	afx_msg LRESULT OnDdtChanged(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL col1;
	BOOL col2;
    BOOL col3;
	BOOL col4;
	BOOL col5;
	BOOL col6;
};

/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayManagement dialog

class CPropertyPagePasswayManagement : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropertyPagePasswayManagement)

// Construction
public:
	void OnChangeSection(int sectionNum);
 
	BOOL Flag_sta;
	BOOL Flag_info;  //径路数据更新标志
    int Flag_moredata;   //是否多余数据

    int nStationList[64];
    int nStationNum;
	void Set_Passway();
	void Del_Passway(short id);
	void Show_StationInfo();
	void Show_Passway_Info();
 
	int  GetStationNoByName(CString sName );
	BOOL GetStationInfoFromMMI( );
	void LoadPassWayStation(short PassWayId);
	void LoadPassWayInfo();
	CGridCtrl *m_pGridCtrl_1;
    int		m_nFixCols_1;   //固定列
	int		m_nFixRows_1;   //固定行
	int		m_nCols_1;      //列
	int		m_nRows_1;      //行
 
    CGridCtrl *m_pGridCtrl_2;
    int		m_nFixCols_2;   //固定列
	int		m_nFixRows_2;   //固定行
	int		m_nCols_2;      //列
	int		m_nRows_2;      //行
	CPropertyPagePasswayManagement();
	~CPropertyPagePasswayManagement();

// Dialog Data
	//{{AFX_DATA(CPropertyPagePasswayManagement)
	enum { IDD = IDD_DLG_PASSWAY_MANAGEMENT };
	CButton	m_Frame2;
	CButton	m_Frame1;
	CButton	m_s_saveCtrl;
	CButton	m_s_newCtrl;
	CButton	m_s_deleteCtrl;
	CButton	m_saveCtrl;
	CButton	m_deleteCtrl;
	CButton	m_newCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPagePasswayManagement)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	void InitCell(int row);
	int current_passway;
	short current_passway_id;
	// Generated message map functions
	//{{AFX_MSG(CPropertyPagePasswayManagement)
	virtual BOOL OnInitDialog();
	afx_msg void OnPassWayNew();
	afx_msg void OnPassWayDelete();
	afx_msg void OnPassWaySave();

    afx_msg void OnGridItemChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGrid2ItemChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGrid2StartEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGrid2EndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	 
	afx_msg void OnStationDelete();
	afx_msg void OnStationNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	
	BOOL p_col1;
	BOOL p_col2;

	BOOL s_col2;
	BOOL s_col3;
    BOOL s_col4;
	BOOL s_col5;

};
/////////////////////////////////////////////////////////////////////////////
// CPropertySheetPassway

class CPropertySheetPassway : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropertySheetPassway)

// Construction
public:
	CPropertySheetPassway(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0,CWintgDoc *pDoc=NULL);
	CPropertySheetPassway(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0,CWintgDoc *pDoc=NULL);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertySheetPassway)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	CWintgDoc *m_pDoc;
	CPropertyPagePasswayManagement Page2;
	CPropertyPagePasswayAppoint Page1;
	CCJFlatComboBox m_Combo1;////////////////////////////////////////
    CStatic   m_Static1;///////////////////////////////////////
    //CToolTipWnd m_ctrlTooltipWnd;
	virtual ~CPropertySheetPassway();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertySheetPassway)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg LRESULT OnPassWayUtility(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnPassWayInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnPassWayStation(WPARAM wParam,LPARAM lParam);
    afx_msg void OnSelendokComboDdtname();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGEPASSWAY_H__327BDD70_8E4C_47A9_8C22_BF1C4DA49A66__INCLUDED_)
