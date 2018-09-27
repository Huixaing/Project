#if !defined(AFX_STATIONINHIBITPROPERTYMIDDLE_H__B9D61F0E_9849_46C5_A0DE_754E8AA1CD7E__INCLUDED_)
#define AFX_STATIONINHIBITPROPERTYMIDDLE_H__B9D61F0E_9849_46C5_A0DE_754E8AA1CD7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationInhibitPropertyMiddle.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationInhibitPropertyMiddle dialog
#include "StationInhibitProperty.h"

class CStationInhibitPropertyMiddle : public CStationInhibitProperty
{
// Construction
public:
	CStationInhibitPropertyMiddle(CTg_BLOCK * pFigure,CTGDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CStationInhibitPropertyMiddle();
	
// Dialog Data
	//{{AFX_DATA(CStationInhibitPropertyMiddle)
	enum { IDD = IDD_STATION_INHIBIT_PROPERTY_MIDDLE };
	//}}AFX_DATA

// Implementation
protected:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONINHIBITPROPERTYMIDDLE_H__B9D61F0E_9849_46C5_A0DE_754E8AA1CD7E__INCLUDED_)
