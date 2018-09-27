// StationSlowPropertyLarge.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "StationSlowPropertyLarge.h"
#include "IF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationSlowPropertyLarge dialog

CStationSlowPropertyLarge::CStationSlowPropertyLarge(CTg_BLOCK * pFigure,CTGDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CStationSlowProperty(pFigure, pDoc, pParent, CStationSlowPropertyLarge::IDD)
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CStationSlowPropertyLarge::OnInitDialog() 
{
	CStationSlowProperty::OnInitDialog();
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStationSlowPropertyLarge::CreateCheckBox()
{
	int countofbox;
	int i = 0;
	CRect buttonRect;
	long  left,right,bottom,top;
	int row;
	long width,height;
	CString str;

	countofbox = m_pDoc->m_sDispDistance[stationNo].number_of_side;
		
	CWnd *boxWnd = GetDlgItem(IDC_STATIC_BOX);
	
	boxWnd->GetClientRect(&buttonRect);
	boxWnd->MapWindowPoints(this,&buttonRect);
	left = buttonRect.left;
	right = buttonRect.right;
	bottom = buttonRect.bottom-30;
	top = buttonRect.top;
	row = countofbox/10 + 1;
	width = (right - left - WIDTHSPACE * 11)/10;
    height = (bottom - top - HEIGHTSPACE3 * (row + 1))/row;

	buttonRect.left = left + WIDTHSPACE;
	buttonRect.right = buttonRect.left + width;
	buttonRect.top = top + HEIGHTSPACE3*2;
	buttonRect.bottom = buttonRect.top + height;

	left = buttonRect.left;
	right = buttonRect.right;
	bottom = buttonRect.bottom;
	top = buttonRect.top;

	str.Empty();

	checkBoxList = new CButton[countofbox];
	
	for(i=1;i<=countofbox;i++)
	{
		str.Format("%s",m_pDoc->m_sDispDistance[stationNo].side[i-1].sidename);
		checkBoxList[i-1].Create(str,WS_CHILD | WS_VISIBLE | WS_TABSTOP|BS_AUTOCHECKBOX,buttonRect,this,IDC_CHECKBOX_START + i);
		buttonRect.left = left + ((i) % 10) * (width+WIDTHSPACE);
		buttonRect.right = right + ((i)%10) * (width+WIDTHSPACE);
		buttonRect.top = top + ((i)/10) * (height +HEIGHTSPACE3);
		buttonRect.bottom = bottom + ((i)/10) *(height + HEIGHTSPACE3);
	}

	int sidenumber=0;
	int elenumber = 0;
	for(i = 0;i< m_pFigure->count ;i ++)
	{
		elenumber = m_pFigure->ele_number[i];
		for(int j=0;j<countofbox;j++)
		{
			sidenumber = m_pDoc->m_sDispDistance[stationNo].side[j].side_number; 
			if(sidenumber == elenumber)
			{
				checkBoxList[j].SetCheck(TRUE);
				break;
			}
		}
	}

	if( !m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		for(int j=0;j<countofbox;j++)
		{
			checkBoxList[j].EnableWindow(FALSE);
		}
	}
}

CStationSlowPropertyLarge::~CStationSlowPropertyLarge()
{
}