#if !defined(AFX_PROPERTYPAGEPASSWAY_H__327BDD70_8E4C_47A9_8C22_BF1C4DA49A66__INCLUDED_)
#define AFX_PROPERTYPAGEPASSWAY_H__327BDD70_8E4C_47A9_8C22_BF1C4DA49A66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"
#include "GridCtrl_src/GridCtrl.h"  //包含表格控件的类
#include "GridCtrl_src/GridURLCell.h"///////////////////////
#include "GridCtrl_src/GridCellCombo.h"///////////////////////

#include "messages.h"
#include "function.h"
#include "memshare.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayManagement dialog
class CPropertyPagePasswayManagement : public CDialog
{
// Construction
public:
	CPropertyPagePasswayManagement(int con_id, CWnd* pParentWnd);
	~CPropertyPagePasswayManagement();

// Dialog Data
	//{{AFX_DATA(CPropertyPagePasswayManagement)
	enum { IDD = IDD_DLG_PASSWAY_MANAGEMENT };
	CButton	m_Frame2;
	CButton	m_Frame1;
	//}}AFX_DATA
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPagePasswayManagement)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyPagePasswayManagement)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void OnChangeSection();
	void LoadPassWayStation(short PassWayId);
	void LoadPassWayInfo();
	void Show_StationInfo();
	void Show_Passway_Info();
	int  GetStationNoByName(CString sName );
	afx_msg void OnSelchangeComboConId();
private:
	int m_conid;
	CComboBox m_concombox;

	int nStationList[64];
    int nStationNum;

	int nSectionNum,nSectionList[64];

	int passway_count;   //径路数目
    int station_count;   //车站数目

	CGridCtrl *m_pGridCtrl_1;
	int		m_nCols_1;      //列
	int		m_nRows_1;      //行
 
    CGridCtrl *m_pGridCtrl_2;
	int		m_nCols_2;      //列
	int		m_nRows_2;      //行
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGEPASSWAY_H__327BDD70_8E4C_47A9_8C22_BF1C4DA49A66__INCLUDED_)
