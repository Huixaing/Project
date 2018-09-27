// InputHatTrain.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "InputHatTrain.h"
#include "function.h"
#include "messages.h"
#include "sysshare.h"
#include <baseMsg\msgid.h>
#include "tgdocument.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CInputHatTrain dialog


CInputHatTrain::CInputHatTrain(CTgDocument *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CInputHatTrain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputHatTrain)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_strTrainId="";
	m_strTrainHatId="";
}


void CInputHatTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputHatTrain)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	    DDX_Text(pDX, IDC_GRAPHIC_TRAIN, m_strTrainId);
		DDV_MaxChars(pDX, m_strTrainId, 9);

	    DDX_Text(pDX, IDC_HAT_TRAIN, m_strTrainHatId);
		DDV_MaxChars(pDX, m_strTrainHatId, 9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputHatTrain, CDialog)
	//{{AFX_MSG_MAP(CInputHatTrain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputHatTrain message handlers

BOOL CInputHatTrain::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputHatTrain::OnOK() 
{
	// TODO: Add extra validation here
	GetDlgItem(IDC_HAT_TRAIN)->GetWindowText(m_HatTrainId);
	GetDlgItem(IDC_GRAPHIC_TRAIN)->GetWindowText(m_GraphicTrainId);
	if(m_HatTrainId.IsEmpty() || m_GraphicTrainId.IsEmpty())
	{
		AfxMessageBox("内容不能为空");
		return;
	}
	CDialog::OnOK();
}
 