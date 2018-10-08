// UnEditAbleGridCell.h: interface for the CUnEditAbleGridCell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNEDITABLEGRIDCELL_H__015864AE_1937_40D0_B1AC_31B79462184F__INCLUDED_)
#define AFX_UNEDITABLEGRIDCELL_H__015864AE_1937_40D0_B1AC_31B79462184F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GRIDCTRL_SRC\GridCell.h"
//add by cf after 7-18 all
//##ModelId=3D4F1E42022C
class CUnEditAbleGridCell : public CGridCell  
{
	friend class CGridCtrl;
    DECLARE_DYNCREATE(CUnEditAbleGridCell)
public:
	//##ModelId=3D4F1E420240
	CUnEditAbleGridCell();
	//##ModelId=3D4F1E420241
	virtual ~CUnEditAbleGridCell();
public:
	//##ModelId=3D4F1E420243
    virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
};

#endif // !defined(AFX_UNEDITABLEGRIDCELL_H__015864AE_1937_40D0_B1AC_31B79462184F__INCLUDED_)
