#if !defined(AFX_STATIONINHIBITPROPERTY_H__B9D61F0E_9849_46C5_A0DE_754E8AA1CD7E__INCLUDED_)
#define AFX_STATIONINHIBITPROPERTY_H__B9D61F0E_9849_46C5_A0DE_754E8AA1CD7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationInhibitProperty.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationInhibitProperty dialog
#include "resource.h"
#include "tgdoc.h"

#define WIDTHSPACE  6  //add by cf
#define HEIGHTSPACE4 8  //add by cf


class CStationInhibitProperty : public CDialog
{
// Construction
public:
	CStationInhibitProperty(CTg_BLOCK * pFigure,CTGDoc *pDoc, CWnd* pParent, UINT tmp=CStationInhibitProperty::IDD);   // standard constructor
	virtual ~CStationInhibitProperty();//add by cf

// Dialog Data
	//{{AFX_DATA(CStationInhibitProperty)
	enum { IDD = IDD_STATION_INHIBIT_PROPERTY };
	CComboBox	m_ComboLinePos;
	CComboBox	m_ctlReason;
	CString	m_strScope;
	CString	m_strReason;
	int		m_s_kilo;
	int		m_e_meter;
	int		m_s_meter;
	int		m_e_kilo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStationInhibitProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStationInhibitProperty)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButton1(); //add by cf
	afx_msg void OnButton2(); //add by cf
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void InitIdentify();
	virtual void CreateCheckBox();

protected:
	int stationNo; //add by cf
	CTGDoc  * m_pDoc;
    CTg_BLOCK * m_pFigure;
	CButton * checkBoxList;
public:
	USHORT m_nSubType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONINHIBITPROPERTY_H__B9D61F0E_9849_46C5_A0DE_754E8AA1CD7E__INCLUDED_)
