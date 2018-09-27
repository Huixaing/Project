// ChangeTrainRoute.cpp : implementation file
//

#include "stdafx.h"
#include "function.h"
#include "ChangeTrainRoute.h"
 #include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include "tg_basicpara.h"
#include "tg_function_config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeTrainRoute dialog


CChangeTrainRoute::CChangeTrainRoute(int con_id,int station,int flag, CWnd* pParent /*=NULL*/)
	: CDialog(CChangeTrainRoute::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeTrainRoute)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nSectionID=con_id;
	m_nRouteNo=-1;
	m_nStationID=station;
	m_nFlag=flag;
}


void CChangeTrainRoute::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeTrainRoute)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeTrainRoute, CDialog)
	//{{AFX_MSG_MAP(CChangeTrainRoute)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeTrainRoute message handlers

BOOL CChangeTrainRoute::OnInitDialog() 
{
	CDialog::OnInitDialog();
	LoadPasswayInfo();
	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeTrainRoute::OnOK() 
{
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_LIST_ROUTE);
	int nCurSel = 0;
	nCurSel = pListBox->GetCurSel();
    m_nRouteNo = (int)pListBox->GetItemData(nCurSel);

	CDialog::OnOK();
}

void CChangeTrainRoute::LoadPasswayInfo()
{
	TG_PassWayInfo info[256];
	int cnt=256;
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_LIST_ROUTE);
	if(GetTGBasicParam()->GetPasswayInfo(m_nSectionID,info,&cnt))
	{
		for(int i=0;i<cnt;i++)
		{
			if((m_nFlag>0)&&(m_nStationID>0 && info[i].Begin_sta!=m_nStationID)) 
				continue;
			int sel=pListBox->AddString(info[i].Pass_way_name);
			pListBox->SetItemData(sel,info[i].Pass_way_id);
		}
	}
}


