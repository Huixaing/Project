#if !defined(AFX_STATIONSLOWPROPERTY_H__42AA9F2D_C5F7_441D_A7E3_93CE2160A768__INCLUDED_)
#define AFX_STATIONSLOWPROPERTY_H__42AA9F2D_C5F7_441D_A7E3_93CE2160A768__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationSlowProperty.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationSlowProperty dialog
#include "resource.h"
#include "tgdoc.h"

#define WIDTHSPACE  6  //add by cf
#define HEIGHTSPACE3 8  //add by cf

class CStationSlowProperty : public CDialog
{
public:
	CStationSlowProperty(CTg_BLOCK * pFigure,CTGDoc *pDoc,CWnd* pParent, UINT tmp=CStationSlowProperty::IDD);   // standard constructor
	virtual ~CStationSlowProperty();


// Dialog Data
	//{{AFX_DATA(CStationSlowProperty)
	enum { IDD = IDD_STATION_SLOW_PROPERTY };
	CComboBox	m_ComboLinePos;
	CComboBox	m_ctlReason;
	CString	m_strScope;
	int		m_nSpeed;
	CString	m_strReason;
	int		m_s_kilo;
	int		m_e_meter;
	int		m_s_meter;
	int		m_e_kilo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStationSlowProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStationSlowProperty)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButton1(); //add by cf
	afx_msg void OnButton2(); // add by cf
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void InitIdentify();
	virtual void CreateCheckBox();

protected:
	CButton * checkBoxList;
	int stationNo; //add by cf
	CTGDoc *m_pDoc;
    CTg_BLOCK * m_pFigure;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONSLOWPROPERTY_H__42AA9F2D_C5F7_441D_A7E3_93CE2160A768__INCLUDED_)
