#if !defined(AFX_STATIONSLOWPROPERTY_MIDDLE_H__42AA9F2D_C5F7_441D_A7E3_93CE2160A768__INCLUDED_)
#define AFX_STATIONSLOWPROPERTY_MIDDLE_H__42AA9F2D_C5F7_441D_A7E3_93CE2160A768__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationSlowPropertyMiddle.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationSlowPropertyMiddle dialog
#include "resource.h"
#include "tgdoc.h"

#include "StationSlowProperty.h"

class CStationSlowPropertyMiddle : public CStationSlowProperty
{
// Construction
public:
	CStationSlowPropertyMiddle(CTg_BLOCK * pFigure,CTGDoc *pDoc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CStationSlowPropertyMiddle();

// Dialog Data
	//{{AFX_DATA(CStationSlowPropertyMiddle)
	enum { IDD = IDD_STATION_SLOW_PROPERTY_MIDDLE };
	//}}AFX_DATA

protected:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONSLOWPROPERTY_MIDDLE_H__42AA9F2D_C5F7_441D_A7E3_93CE2160A768__INCLUDED_)
