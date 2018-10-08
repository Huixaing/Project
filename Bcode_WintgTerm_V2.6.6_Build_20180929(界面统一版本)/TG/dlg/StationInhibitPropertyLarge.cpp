// stationinhibitpropertyLarge.cpp : implementation file
//

#include "stdafx.h"
#include "IF.h"
#include "stationinhibitpropertyLarge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationInhibitPropertyLarge dialog


CStationInhibitPropertyLarge::CStationInhibitPropertyLarge(CTg_BLOCK * pFigure,CTGDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CStationInhibitProperty(pFigure, pDoc, pParent, CStationInhibitPropertyLarge::IDD)
{
}

CStationInhibitPropertyLarge::~CStationInhibitPropertyLarge()
{
}

/////////////////////////////////////////////////////////////////////////////
// CStationInhibitPropertyLarge message handlers
BOOL CStationInhibitPropertyLarge::OnInitDialog() 
{
	CStationInhibitProperty::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStationInhibitPropertyLarge::CreateCheckBox()
{
	int countofbox;
	int i = 0;
	CRect buttonRect;
	long  left,right,bottom,top;
	int row;
	long width,height;
	CString str;

	countofbox = m_pDoc->m_sDispDistance[stationNo].number_of_side;
		

	CWnd *boxWnd = GetDlgItem(IDC_STATIC_BOX1);
	
	boxWnd->GetClientRect(&buttonRect);
	boxWnd->MapWindowPoints(this,&buttonRect);
	left = buttonRect.left;
	right = buttonRect.right;
	bottom = buttonRect.bottom;
	top = buttonRect.top;
	row = countofbox/10 + 1;
	width = (right - left - WIDTHSPACE * 11)/10;
    height = (bottom - top - HEIGHTSPACE4 * (row + 1))/row;

	buttonRect.left = left + WIDTHSPACE;
	buttonRect.right = buttonRect.left + width;
	buttonRect.top = top + HEIGHTSPACE4*2;
	buttonRect.bottom = buttonRect.top + height;

	left = buttonRect.left;
	right = buttonRect.right;
	bottom = buttonRect.bottom;
	top = buttonRect.top;

	str.Empty();

	checkBoxList = new CButton[countofbox];
	//modi by cf after 8-24
	for(i=1;i<=countofbox;i++)
	{
		str.Format("%s",m_pDoc->m_sDispDistance[stationNo].side[i-1].sidename);
		checkBoxList[i-1].Create(str,WS_CHILD | WS_VISIBLE | WS_TABSTOP|BS_AUTOCHECKBOX,buttonRect,this,IDC_CHECKBOX_START + i);
		buttonRect.left = left + ((i) % 10) * (width+WIDTHSPACE);
		buttonRect.right = right + ((i)%10) * (width+WIDTHSPACE);
		buttonRect.top = top + ((i)/10) * (height +HEIGHTSPACE4);
		buttonRect.bottom = bottom + ((i)/10) *(height + HEIGHTSPACE4);
	}
	//end of cf
	int sidenumber=0;
	int elenumber = 0;
	for(i = 0;i< m_pFigure->count ;i ++)
	{
		elenumber = m_pFigure->ele_number[i];
		//modi by cf after 8-24
		for(int j=0;j<countofbox;j++)
		{
			sidenumber = m_pDoc->m_sDispDistance[stationNo].side[j].side_number; 
			if(sidenumber == elenumber)
			{
				checkBoxList[j].SetCheck(TRUE);
				break;
			}

		}
		//end of cf
	}

	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		for(int j=0;j<countofbox;j++)
		{
			checkBoxList[j].EnableWindow(FALSE);
		}
	}

}
