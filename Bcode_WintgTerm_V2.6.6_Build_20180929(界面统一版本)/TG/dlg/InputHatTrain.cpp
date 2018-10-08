// InputHatTrain.cpp : implementation file
//

#include "stdafx.h"
#include "IF.h"

#include "InputHatTrain.h"
#include "function.h"
#include "messages.h"

#include <msgid.h>
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CInputHatTrain dialog


CInputHatTrain::CInputHatTrain(CTGDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CInputHatTrain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputHatTrain)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
}


void CInputHatTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputHatTrain)
		// NOTE: the ClassWizard will add DDX and DDV calls here
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
	
	m_pDoc->InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputHatTrain::OnOK() 
{
	// TODO: Add extra validation here
	CString m_HatTrainId,m_GraphicTrainId;
	GetDlgItem(IDC_HAT_TRAIN)->GetWindowText(m_HatTrainId);
	GetDlgItem(IDC_GRAPHIC_TRAIN)->GetWindowText(m_GraphicTrainId);
	if(m_HatTrainId.IsEmpty() || m_GraphicTrainId.IsEmpty())
	{
		AfxMessageBox("内容不能为空");
		return;
	}
	char buffer[1024],*ptr;
	ushort count;
	ptr = buffer;
 
	ptr += 2;//for count

	*(ushort *)ptr =  (TRAIN_SCHEDULE_INSERT_INFORM);
	ptr += 2;
	*ptr = m_pDoc->m_nCurrentSectionNo;
	ptr++;
	if(m_pDoc->m_ptgctrl->mConfigTg.context.m_tgtype == TG_TYPE::WORK_TG)
		*ptr =  0x05;
	else if(m_pDoc->m_ptgctrl->mConfigTg.context.m_tgtype == TG_TYPE::BASIC_TG)
		*ptr =  0x01;
	else
		return;

	ptr += 1;
	*ptr = ADD_HAT_TRAIN;//basic graphic
	ptr += 1;

    *(ushort *)ptr = NettoHostUShort(-1);
	ptr += 2;
	if(m_pDoc->m_nShiftNo == 2)
	{
		*(time_t *)ptr = NettoHostUInteger(long_time(m_pDoc->m_nShiftDate, 6*3600));
	}
	else
	{
		*(time_t *)ptr = NettoHostUInteger(long_time(OsGetPrevDate(0,m_pDoc->m_nShiftDate), 18*3600));
	}
	ptr += 4;
	if(m_pDoc->m_nShiftNo == 2)
	{
		*(time_t *)ptr = NettoHostUInteger(long_time(OsGetNextDate(0,m_pDoc->m_nShiftDate),6*3600));
	}
	else
	{
		*(time_t *)ptr = NettoHostUInteger(long_time(m_pDoc->m_nShiftDate, 18*3600));
	}
	ptr += 4;


	*(ushort *)ptr = NettoHostUShort(1);
	ptr += 2;
	strcpy(ptr,m_GraphicTrainId);
	ptr += 10;
	strcpy(ptr,m_HatTrainId);
	ptr += 10;


	count = (ushort)(ptr - buffer);
	*(ushort *)(buffer ) =  (count );
//	SendTGCommMessage(count,buffer);	
	CDialog::OnOK();
}
 