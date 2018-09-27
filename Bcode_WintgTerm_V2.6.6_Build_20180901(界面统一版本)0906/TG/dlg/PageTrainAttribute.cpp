// PageTrainAttribute.cpp : implementation file
//

#include "stdafx.h"
#include "PageTrainAttribute.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void  SetTrainAbsTimeType(CTG_TRAIN* ptrain, BYTE run_type)
{
	long nTmpAbsTimeType=run_type;
	nTmpAbsTimeType = nTmpAbsTimeType<<24;
	nTmpAbsTimeType &= 0xFF000000;
	nTmpAbsTimeType |= 0x80000000;  // 高位置1
				
	long train_source=ptrain->myTrainInfo.train_source;
	train_source &= 0x00FFFFFF;
	train_source |= nTmpAbsTimeType;

	ptrain->myTrainInfo.train_source=train_source;
}

BYTE  GetTrainAbsTimeType(CTG_TRAIN* ptrain)
{
	long train_source=ptrain->myTrainInfo.train_source;
	if(train_source&0x80000000)
	{
		train_source &= 0x7F000000;
		BYTE tmp=(train_source>>24);
		return tmp;
	}
	else
	{
		return ptrain->myTrainInfo.run_type;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPageTrainAttribute property page

IMPLEMENT_DYNCREATE(CPageTrainAttribute, CPropertyPage)

CPageTrainAttribute::CPageTrainAttribute() : CPropertyPage(CPageTrainAttribute::IDD)
{
	//{{AFX_DATA_INIT(CPageTrainAttribute)
	m_check_join_adjust = FALSE;
	m_nTrainId = _T("");
	//}}AFX_DATA_INIT
}

CPageTrainAttribute::CPageTrainAttribute(CTG_TRAIN* train, CTGDoc *pDoc) : 
CPropertyPage(CPageTrainAttribute::IDD)
{
	//{{AFX_DATA_INIT(CPageTrainAttribute)
	m_check_join_adjust = FALSE;
	m_check_lock_arrrival = FALSE;
	m_check_lock_depart = FALSE;

	m_nTrainId = _T("");
	//}}AFX_DATA_INIT
	m_pTrain = train;
	m_pDoc   = pDoc;
}


CPageTrainAttribute::~CPageTrainAttribute()
{
}

void CPageTrainAttribute::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageTrainAttribute)
	DDX_Control(pDX, IDC_ATTRIBUTE_ABS, m_ctlTrainAbs);
	DDX_Control(pDX, IDC_ATTRIBUTE_TRAINTYPE, m_ctrTrainType);
	DDX_Control(pDX, IDC_ATTRIBUTE_RUNCLASS, m_ctrAbsTimeType);
	DDX_Text(pDX, IDC_TRAIN_ID, m_nTrainId);
	DDX_Check(pDX, IDC_AUTO_ADJUST, m_check_join_adjust);
	DDX_Check(pDX, IDC_CHECK1, m_check_lock_arrrival);
	DDX_Check(pDX, IDC_CHECK2, m_check_lock_depart);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CPageTrainAttribute, CPropertyPage)
	//{{AFX_MSG_MAP(CPageTrainAttribute)
	ON_CBN_SELCHANGE(IDC_ATTRIBUTE_TRAINTYPE, OnSelchangeAttributeTraintype)
	ON_CBN_SELCHANGE(IDC_ATTRIBUTE_ABS, OnSelTrainRunType)
	ON_CBN_SELCHANGE(IDC_ATTRIBUTE_RUNCLASS, OnSelTrainRuntimeType)
	ON_BN_CLICKED(IDC_CHECK1,OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2,OnCheck2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageTrainAttribute message handlers

BOOL CPageTrainAttribute::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if(m_pTrain == NULL) 
		return FALSE;

	int type_index = -1;
    
	m_nTrainType = m_pTrain->myTrainInfo.type;
	m_nTrainAbs  = m_pTrain->myTrainInfo.run_type;

	m_nTrainId =  m_pDoc->GetTrainShowId(m_pTrain);

	//1 列车类型
	std::map<int, TrainAttribute>::iterator it = m_pDoc->mTrainType.mTrainAttributeMap.begin();
	for(int i = 0 ;it != m_pDoc->mTrainType.mTrainAttributeMap.end(); it++,i++)
	{
		m_ctrTrainType.AddString( (*it).second.name);
		m_ctrTrainType.SetItemData(i, (*it).second.type);
		if(m_pTrain->myTrainInfo.type == (*it).second.type)
		{
			type_index = i;
		}
	}

	if(type_index == -1)
	{
		CEdit *pEdit;
		pEdit = (CEdit *)GetDlgItem(IDC_ATTRIBUTE_TRAINTYPE);
		pEdit->SetWindowText("未知列车");
	}
	else
	{
		m_ctrTrainType.SetCurSel(type_index); 	
	}

	type_index = -1; 

	//2 区间运行点
	std::map<int, CString>::iterator it1 = m_pDoc->mTrainType.mRunTypeNameMap.begin();
	for(int i = 0 ;it1 != m_pDoc->mTrainType.mRunTypeNameMap.end(); it1++, i++)
	{
		m_ctlTrainAbs.AddString((*it1).second);
		m_ctlTrainAbs.SetItemData(i, (*it1).first);
		if(m_pTrain->myTrainInfo.run_type == (*it1).first)
		{
			type_index = i;
		}
	}

	if(type_index == -1)
	{
		CEdit *pEdit;
		pEdit = (CEdit *)GetDlgItem(IDC_ATTRIBUTE_ABS);
		pEdit->SetWindowText("未知列车");
	}
	else
	{
		m_ctlTrainAbs.SetCurSel(type_index); 	
	}

	type_index = -1; 

	//3 区间时分类型
	m_nAbsTimeType=GetTrainAbsTimeType(m_pTrain);
	if(m_pDoc->mTrainType.mAbsTimeTypeNameMap.empty())
	{
		m_ctrAbsTimeType.ShowWindow(SW_HIDE);
		m_ctrAbsTimeType.EnableWindow(FALSE);
		CWnd* pWnd=GetDlgItem(IDC_STATIC_ABS_TIME);
		if(pWnd)
			pWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		std::map<int, RunTypeNode>::iterator it2 = m_pDoc->mTrainType.mAbsTimeTypeNameMap.begin();
		for(;it2 != m_pDoc->mTrainType.mAbsTimeTypeNameMap.end(); it2++)
		{
			int index=m_ctrAbsTimeType.AddString(it2->second.name);
			m_ctrAbsTimeType.SetItemData(index, it2->second.run_type);
			if(m_nAbsTimeType == it2->second.run_type)
			{
				type_index = index;
			}
		}

		if(type_index == -1)
		{
			CEdit *pEdit;
			pEdit = (CEdit *)GetDlgItem(IDC_ATTRIBUTE_RUNCLASS);
			pEdit->SetWindowText("未知列车");
		}
		else
		{
			m_ctrAbsTimeType.SetCurSel(type_index); 	
		}
	}

	m_check_join_adjust = m_pTrain->myTrainInfo.adjust_flag & 0x1;

	TgTRAIN_RECORD *rec;
	rec = m_pTrain->GetFirstRecord();
	if(rec == NULL)
		return FALSE;
	m_check_lock_arrrival = IsArriveLock(*rec);
	m_check_lock_depart  = IsDepartLock(*rec);

	if(IsFlagStart(rec->flag))
	{
		CWnd *pButton;
		pButton = GetDlgItem(IDC_CHECK1);
		pButton->EnableWindow(FALSE);
	}
	else
	{
		CWnd *pButton;
		pButton = GetDlgItem(IDC_CHECK2);
		pButton->EnableWindow(FALSE);
	}

	UpdateData(FALSE);

	this->InitIdentify();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPageTrainAttribute::OnSelchangeAttributeTraintype() 
{
	int m_nCurSel = m_ctrTrainType.GetCurSel();
	m_nTrainType = m_ctrTrainType.GetItemData(m_nCurSel);

	std::map<int, TrainAttribute>::iterator it = m_pDoc->mTrainType.mTrainAttributeMap.find(m_nTrainType);
	if(it != m_pDoc->mTrainType.mTrainAttributeMap.end())
	{
		for(int i = 0; i < m_ctlTrainAbs.GetCount(); i++)
		{
			if(m_ctlTrainAbs.GetItemData(i) == (*it).second.run_type)
			{
				m_ctlTrainAbs.SetCurSel(i);
				m_nTrainAbs  = (*it).second.run_type;
				break;
			}
		}
	}
}

void CPageTrainAttribute::OnSelTrainRunType()
{
	int m_nCurSel = m_ctlTrainAbs.GetCurSel();
	m_nTrainAbs = m_ctlTrainAbs.GetItemData(m_nCurSel);
}

void CPageTrainAttribute::OnSelTrainRuntimeType()
{
	if(m_pDoc->mTrainType.mAbsTimeTypeNameMap.empty())
	{
		m_nAbsTimeType=GetTrainAbsTimeType(m_pTrain);
	}
	else
	{
		int m_nCurSel = m_ctrAbsTimeType.GetCurSel();
		m_nAbsTimeType = m_ctrAbsTimeType.GetItemData(m_nCurSel);
	}
}

void CPageTrainAttribute::OnCheck1()
{
	UpdateData(TRUE);
	TgTRAIN_RECORD *rec;
	rec = m_pTrain->GetRecordByRecIndex(0);
	if(rec == NULL)
		return;
	if(m_check_lock_arrrival)
	{
		if(IsFlagStart(rec->flag))
		{
			SetDepartLock(*rec);
		}
		SetArriveLock(*rec);
	}
	else
	{
		if(IsFlagStart(rec->flag))
		{
			ReSetDepartLock(*rec);
		}
		ReSetArriveLock(*rec);
	}
}

void CPageTrainAttribute::OnCheck2()
{
	UpdateData(TRUE);
	TgTRAIN_RECORD *rec;
	rec = m_pTrain->GetRecordByRecIndex(0);
	if(rec == NULL)
		return;

	if(m_check_lock_depart)
	{
		if(IsFlagStart(rec->flag))
		{
			SetArriveLock(*rec);
		}
		SetDepartLock(*rec);
	}
	else
	{
		if(IsFlagStart(rec->flag))
		{
			ReSetArriveLock(*rec);
		}
		ReSetDepartLock(*rec);
	}
}

void CPageTrainAttribute::InitIdentify()
{
	if(!m_pDoc->IsCanEditTrain())
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_ATTRIBUTE_TRAINTYPE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_ATTRIBUTE_ABS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_ATTRIBUTE_RUNCLASS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_AUTO_ADJUST);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHECK1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHECK2);
		pWnd->EnableWindow(FALSE);
	}
}