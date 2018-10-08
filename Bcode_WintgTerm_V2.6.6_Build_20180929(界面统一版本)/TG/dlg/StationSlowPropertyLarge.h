#if !defined(AFX_STATIONSLOWPROPERTYLARGE_H__42AA9F2D_C5F7_441D_A7E3_93CE2160A768__INCLUDED_)
#define AFX_STATIONSLOWPROPERTYLARGE_H__42AA9F2D_C5F7_441D_A7E3_93CE2160A768__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationSlowProperty.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationSlowPropertyLarge dialog
#include "StationSlowProperty.h"

class CStationSlowPropertyLarge : public CStationSlowProperty
{
public:
	CStationSlowPropertyLarge(CTg_BLOCK * pFigure,CTGDoc *pDoc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CStationSlowPropertyLarge();
	
// Dialog Data
	//{{AFX_DATA(CStationSlowPropertyLarge)
	enum { IDD = IDD_STATION_SLOW_PROPERTY_LARGE };
	//}}AFX_DATA

// Implementation
protected:
	virtual void CreateCheckBox();
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONSLOWPROPERTYLARGE_H__42AA9F2D_C5F7_441D_A7E3_93CE2160A768__INCLUDED_)
