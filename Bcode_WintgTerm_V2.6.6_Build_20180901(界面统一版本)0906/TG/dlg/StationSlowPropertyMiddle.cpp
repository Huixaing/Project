// StationSlowPropertyMiddle.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "StationSlowPropertyMiddle.h"
#include "IF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationSlowPropertyMiddle dialog

CStationSlowPropertyMiddle::CStationSlowPropertyMiddle(CTg_BLOCK * pFigure,CTGDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CStationSlowProperty(pFigure, pDoc, pParent, CStationSlowPropertyMiddle::IDD)
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CStationSlowPropertyMiddle::OnInitDialog() 
{
	CStationSlowProperty::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CStationSlowPropertyMiddle::~CStationSlowPropertyMiddle()
{
}