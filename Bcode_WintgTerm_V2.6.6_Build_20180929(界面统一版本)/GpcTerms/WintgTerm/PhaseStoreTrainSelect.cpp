// PhaseStoreTrainSelect.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PhaseStoreTrainSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhaseStoreTrainSelect dialog


CPhaseStoreTrainSelect::CPhaseStoreTrainSelect(int nbanbie,CWnd* pParent /*=NULL*/)
	: CDialog(CPhaseStoreTrainSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPhaseStoreTrainSelect)
		// NOTE: the ClassWizard will add member initialization here
	banbie = nbanbie;
	m_nPhase = -1;
	//}}AFX_DATA_INIT
}


void CPhaseStoreTrainSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhaseStoreTrainSelect)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPhaseStoreTrainSelect, CDialog)
	//{{AFX_MSG_MAP(CPhaseStoreTrainSelect)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhaseStoreTrainSelect message handlers

 

BOOL CPhaseStoreTrainSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    CWnd *pWnd1 = GetDlgItem(IDC_RADIO1);
	CWnd *pWnd2 = GetDlgItem(IDC_RADIO2);
	CWnd *pWnd3 = GetDlgItem(IDC_RADIO3);
	CButton *radioWnd;
	if(banbie == 2 )
	{   
		pWnd1->SetWindowText("6点");
		pWnd2->SetWindowText("12点");
		pWnd3->SetWindowText("18点");
	}
	else
    {   
		pWnd1->SetWindowText("18点");
		pWnd2->SetWindowText("0点");
		pWnd3->SetWindowText("6点");
	}

	radioWnd = (CButton *)GetDlgItem(IDC_RADIO1);
	radioWnd->SetCheck(1);
    SetPhase(0) ;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPhaseStoreTrainSelect::OnOK() 
{
 	
	CDialog::OnOK();
}

void CPhaseStoreTrainSelect::OnRadio1() 
{ 
	SetPhase(0) ;
}

int CPhaseStoreTrainSelect::SetPhase(int index)
{
	int value=0;
	if(banbie == 2 )
	{   
		if(index == 0)
			value = STACAR_PHASE::STACAR_PHASE_6; //6
		if(index == 1 )
			value =STACAR_PHASE::STACAR_PHASE_12; //12
		if(index == 2 )
			value = STACAR_PHASE::STACAR_PHASE_18; //18
	}
	else
    {   
		if(index == 0)
		   value = STACAR_PHASE::STACAR_PHASE_18; //18
		if(index == 1 )
			value = STACAR_PHASE::STACAR_PHASE_0; //0
		if(index == 2 )
			value = STACAR_PHASE::STACAR_PHASE_6; //6
	}

    m_nPhase =  value;
	return TRUE;
}

void CPhaseStoreTrainSelect::OnRadio2() 
{
	SetPhase(1) ;	
}

void CPhaseStoreTrainSelect::OnRadio3() 
{
	SetPhase(2) ;	
}
