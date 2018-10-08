#if !defined(AFX_STATIONINHIBITPROPERTYLARGE_H__B9D61F0E_9849_46C5_A0DE_754E8AA1CD7E__INCLUDED_)
#define AFX_STATIONINHIBITPROPERTYLARGE_H__B9D61F0E_9849_46C5_A0DE_754E8AA1CD7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationInhibitPropertyLarge.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationInhibitPropertyLarge dialog
#include "StationInhibitProperty.h"

class CStationInhibitPropertyLarge : public CStationInhibitProperty
{
// Construction
public:
	CStationInhibitPropertyLarge(CTg_BLOCK * pFigure,CTGDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CStationInhibitPropertyLarge();
	
	

// Dialog Data
	//{{AFX_DATA(CStationInhibitPropertyLarge)
	enum { IDD = IDD_STATION_INHIBIT_PROPERTY_LARGE };
	//}}AFX_DATA

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual void CreateCheckBox(); 
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONINHIBITPROPERTYLARGE_H__B9D61F0E_9849_46C5_A0DE_754E8AA1CD7E__INCLUDED_)
