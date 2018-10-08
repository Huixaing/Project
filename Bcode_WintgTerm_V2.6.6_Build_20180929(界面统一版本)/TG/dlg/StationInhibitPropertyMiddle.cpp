// StationInhibitPropertyMiddle.cpp : implementation file
//

#include "stdafx.h"
#include "IF.h"
#include "StationInhibitPropertyMiddle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationInhibitPropertyMiddle dialog


CStationInhibitPropertyMiddle::CStationInhibitPropertyMiddle(CTg_BLOCK * pFigure,CTGDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CStationInhibitProperty(pFigure, pDoc, pParent, CStationInhibitPropertyMiddle::IDD)
{
}

CStationInhibitPropertyMiddle::~CStationInhibitPropertyMiddle()
{
}

/////////////////////////////////////////////////////////////////////////////
// CStationInhibitPropertyMiddle message handlers
BOOL CStationInhibitPropertyMiddle::OnInitDialog() 
{
	CStationInhibitProperty::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}