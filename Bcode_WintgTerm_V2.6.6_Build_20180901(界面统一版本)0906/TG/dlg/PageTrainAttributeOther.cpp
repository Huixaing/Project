// PageTrainAttributeOther.cpp : implementation file
//

#include "stdafx.h"
#include "PageTrainAttributeOther.h"
#include ".\pagetrainattributeother.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageTrainAttributeOther dialog
IMPLEMENT_DYNCREATE(CPageTrainAttributeOther, CPropertyPage)

#define CHANGE_TRAIN_INFO  0x01    
#define CHANGE_TRAIN_TIME  0x02

CPageTrainAttributeOther::CPageTrainAttributeOther() : CPropertyPage(CPageTrainAttributeOther::IDD)
{
	//{{AFX_DATA_INIT(CPageTrainAttributeOther)
	m_check1 = FALSE;
	m_check2 = FALSE;
	m_check3 = FALSE;
	m_check4 = FALSE;
	m_check5 = FALSE;
	m_check6 = FALSE;
	m_behind_no = _T("");
	m_e_a_time = _T("");
	m_engine = _T("");
	m_f_a_time = _T("");
	m_f_plan_time = _T("");
	m_length = _T("");
	m_motoman = _T("");
	m_speed = _T("");
	m_train_id = _T("");
	m_weight = _T("");
	m_e_plan_time = _T("");
	m_e_station = _T("");
	m_f_station = _T("");
	m_train_num = _T("");
	//}}AFX_DATA_INIT
}

CPageTrainAttributeOther::CPageTrainAttributeOther(CTG_TRAIN* train,CTGDoc *pDoc) : 
CPropertyPage(CPageTrainAttributeOther::IDD)
{
	//{{AFX_DATA_INIT(CPageTrainAttributeOther)
	m_check1 = FALSE;
	m_check2 = FALSE;
	m_check3 = FALSE;
	m_check4 = FALSE;
	m_check5 = FALSE;
	m_check6 = FALSE;
	m_behind_no = _T("");
	m_e_a_time = _T("");
	m_engine = _T("");
	m_f_a_time = _T("");
	m_f_plan_time = _T("");
	m_length = _T("");
	m_motoman = _T("");
	m_speed = _T("");
	m_train_id = _T("");
	m_weight = _T("");
	m_e_plan_time = _T("");
	m_e_station = _T("");
	m_f_station = _T("");
	m_train_num = _T("");
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_pTrain = train;
}

CPageTrainAttributeOther::~CPageTrainAttributeOther()
{
}

void CPageTrainAttributeOther::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageTrainAttributeOther)
	DDX_Control(pDX, IDC_COMBO_ZY, m_combo_zy);
	DDX_Control(pDX, IDC_COMBO_JJ, m_combo_jj);
	DDX_Control(pDX, IDC_COMBO_CX, m_combo_cx);
	DDX_Control(pDX, IDC_COMBO_CC, m_combo_cc);
	DDX_Check(pDX, IDC_CHECK2, m_check2);
	DDX_Check(pDX, IDC_CHECK3, m_check3);
	DDX_Check(pDX, IDC_CHECK4, m_check4);
	DDX_Check(pDX, IDC_CHECK5, m_check5);
	DDX_Check(pDX, IDC_CHECK6, m_check6);
	DDX_Text(pDX, IDC_EDIT_BEHIND_NO, m_behind_no);
	DDX_Text(pDX, IDC_EDIT_E_A_TIME, m_e_a_time);
	DDV_MaxChars(pDX, m_e_a_time, 8);
	DDX_Text(pDX, IDC_EDIT_ENGINE, m_engine);
	DDX_Check(pDX, IDC_CHECK1, m_check1);
	DDX_Text(pDX, IDC_EDIT_F_A_TIME, m_f_a_time);
	DDV_MaxChars(pDX, m_f_a_time, 8);
	DDX_Text(pDX, IDC_EDIT_F_PLAN_TIME, m_f_plan_time);
	DDV_MaxChars(pDX, m_f_plan_time, 8);
	DDX_Text(pDX, IDC_EDIT_LENGTH, m_length);
	DDX_Text(pDX, IDC_EDIT_MOTOMAN, m_motoman);
	DDX_Text(pDX, IDC_EDIT_SPEED, m_speed);
	DDV_MaxChars(pDX, m_speed, 3);
	DDX_Text(pDX, IDC_EDIT_TRAIN_ID, m_train_id);
	DDX_Text(pDX, IDC_EDIT_WEIGHT, m_weight);
	DDX_Text(pDX, IDC_EDIT1_E_PLAN_TIME, m_e_plan_time);
	DDV_MaxChars(pDX, m_e_plan_time, 8);
	DDX_Text(pDX, IDC_EDIT1_E_STATION, m_e_station);
	DDV_MaxChars(pDX, m_e_station, 32);
	DDX_Text(pDX, IDC_EDIT1_F_STATION, m_f_station);
	DDV_MaxChars(pDX, m_f_station, 32);
	DDX_Text(pDX, IDC_EDIT1_TRAIN_NUM, m_train_num);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageTrainAttributeOther, CPropertyPage)
	//{{AFX_MSG_MAP(CPageTrainAttributeOther)
 	ON_CBN_SELCHANGE(IDC_COMBO_CC, OnSelchangeAttributeTrainCC)
 	ON_CBN_SELCHANGE(IDC_COMBO_CX, OnSelchangeAttributeTrainCX)
 	ON_CBN_SELCHANGE(IDC_COMBO_JJ, OnSelchangeAttributeTrainJJ)
 	ON_CBN_SELCHANGE(IDC_COMBO_ZY, OnSelchangeAttributeTrainZY)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT1_F_STATION, OnEnChangeEdit1FStation)
	ON_EN_CHANGE(IDC_EDIT1_E_STATION, OnEnChangeEdit1EStation)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageTrainAttributeOther message handlers
BOOL CPageTrainAttributeOther::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
 
    if(m_pTrain == NULL) 
		return FALSE;

	int i=0,type_index = 0;
    int  combo_num = 4;

	_TCHAR *ComboTypeName[4] = {
		_T("正常"),             //0
		_T("一级"),             //1
		_T("二级"),             //2
		_T("三级"),             //3
	};

	m_pTrainInfo = &(m_pTrain->myTrainInfo);

	m_train_id =  m_pDoc->GetTrainShowId(m_pTrain);

	int speed_limit=0;
	/*for(i=0; i<m_pTrain->GetRecordNum(); i++)
	{
		TgTRAIN_RECORD* prec=m_pTrain->GetRecordByRecIndex(i);
		if(prec)
			speed_limit=prec->speed_limit;
		if(speed_limit>0)
			break;
	}*/
	
	CString str;
	str.Format ("%d", speed_limit);
    m_speed = str;
	if(m_pTrain->myEngine.size() > 0)
	{
        m_engine = m_pTrain->myEngine[0].engine_id;
	}
	else
	    m_engine = "";
	str.Format("%d",m_pTrainInfo->train_length);
	m_length = str;
	str.Format ("%d",m_pTrainInfo->train_weight);
	m_weight = str;
	
    m_check1 = m_pTrainInfo->no_meet_flag & 0x01;
    m_check2 = m_pTrainInfo->no_meet_flag & 0x02>>1;
    m_check3 = m_pTrainInfo->no_meet_flag & 0x04>>2;
    m_check4 = m_pTrainInfo->no_meet_flag & 0x08>>3;
    m_check5 = m_pTrainInfo->no_meet_flag & 0x10>>4;
    m_check6 = m_pTrainInfo->no_meet_flag & 0x20>>5;
    
	m_f_station = m_pTrainInfo->start_sta_name;
	m_e_station = m_pTrainInfo->end_sta_name;

	m_f_a_time = ConvertTimetoString(m_pTrainInfo->sborder_actual_time);
	m_f_plan_time = ConvertTimetoString(m_pTrainInfo->sborder_plan_time);
	m_e_a_time = ConvertTimetoString(m_pTrainInfo->eborder_actual_time);
	m_e_plan_time = ConvertTimetoString(m_pTrainInfo->eborder_plan_time);
 
	int combo_index = 0;  
	_TCHAR *ComboTypeNameCX[5] = {
		_T("正常"),             //0
        _T("超级"),             //1
		_T("一级"),             //2
		_T("二级"),             //3
		_T("三级"),             //4
	};
 	for( i=0;i < 5;i++)
	{
		m_combo_cx.AddString(ComboTypeNameCX[i]);
	}

	m_combo_cx.SetCurSel (m_pTrainInfo->space_rank); 
 	m_nCX = m_pTrainInfo->space_rank; 
 	
	
	m_combo_cc.AddString("正常");
	m_combo_cc.AddString("超长");
	
	m_combo_cc.SetCurSel (m_pTrainInfo->length_rank);
	m_nCC = m_pTrainInfo->length_rank;
	
 	for( i=0;i < combo_num;i++)
	{
		m_combo_zy.AddString(ComboTypeName[i]);
	}
	m_combo_zy.SetCurSel (m_pTrainInfo->super_rank); 	
	m_nZY =  m_pTrainInfo->super_rank; 	

	for( i=0;i < combo_num;i++)
	{
		m_combo_jj.AddString(ComboTypeName[i]);
	}
	m_combo_jj.SetCurSel (m_pTrainInfo->military_rank); 	
	m_nJJ = m_pTrainInfo->military_rank;	

	m_nChange = 0;

	UpdateData(FALSE);
    bNotChanged = FALSE;

	this->InitIdentify();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPageTrainAttributeOther::OnSelchangeAttributeTrainCC()
{
    if (m_nCC == m_combo_cc.GetCurSel()) return;

	m_nCC = m_combo_cc.GetCurSel();
	m_pTrainInfo->length_rank = m_nCC;
	m_nChange |= CHANGE_TRAIN_INFO;
}

void CPageTrainAttributeOther::OnSelchangeAttributeTrainCX()
{
    if (m_nCX == m_combo_cx.GetCurSel()) return;

	m_nCX = m_combo_cx.GetCurSel();
	m_pTrainInfo->space_rank = m_nCX;
	m_nChange |= CHANGE_TRAIN_INFO;
}

void CPageTrainAttributeOther::OnSelchangeAttributeTrainJJ()
{
    if (m_nJJ == m_combo_jj.GetCurSel()) return;

	m_nJJ = m_combo_jj.GetCurSel();
	m_pTrainInfo->military_rank = m_nJJ;
	m_nChange |= CHANGE_TRAIN_INFO;
}

void CPageTrainAttributeOther::OnSelchangeAttributeTrainZY()
{
    if (m_nZY == m_combo_zy.GetCurSel()) return;

	m_nZY = m_combo_zy.GetCurSel();
	m_pTrainInfo->super_rank = m_nZY;
	m_nChange |= CHANGE_TRAIN_INFO;
}

void CPageTrainAttributeOther::InitIdentify()
{
	if(!m_pDoc->IsCanEditTrain())
	{
		m_combo_cx.EnableWindow(FALSE);
		m_combo_cc.EnableWindow(FALSE);
		m_combo_zy.EnableWindow(FALSE);
		m_combo_jj.EnableWindow(FALSE);
		CWnd* pWnd = GetDlgItem(IDC_EDIT_SPEED);
		if(pWnd)
			pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT1_F_STATION);
		if(pWnd)
			pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT1_E_STATION);
		if(pWnd)
			pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_F_PLAN_TIME);
		if(pWnd)
			pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT1_E_PLAN_TIME);
		if(pWnd)
			pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_F_A_TIME);
		if(pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_EDIT_E_A_TIME);
		if(pWnd)
			pWnd->EnableWindow(FALSE);
	}
}

void CPageTrainAttributeOther::OnEnChangeEdit1FStation()
{
	UpdateData(TRUE);
	if (m_f_station == m_pTrainInfo->start_sta_name)
		return;

	m_pTrainInfo->start_sta_name = m_f_station;
	m_nChange |= CHANGE_TRAIN_TIME;
}


void CPageTrainAttributeOther::OnEnChangeEdit1EStation()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_e_station == m_pTrainInfo->end_sta_name)
		return;

	m_pTrainInfo->end_sta_name = m_e_station;
	m_nChange |= CHANGE_TRAIN_TIME;
}
