// UnEditAbleGridCell.cpp: implementation of the CUnEditAbleGridCell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UnEditAbleGridCell.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CUnEditAbleGridCell, CGridCell)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3D4F1E420240
CUnEditAbleGridCell::CUnEditAbleGridCell()
{

}

//##ModelId=3D4F1E420241
CUnEditAbleGridCell::~CUnEditAbleGridCell()
{

}
//##ModelId=3D4F1E420243
BOOL CUnEditAbleGridCell::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
    return TRUE;
}

