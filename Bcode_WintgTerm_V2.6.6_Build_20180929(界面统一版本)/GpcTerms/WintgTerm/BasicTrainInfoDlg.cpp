// BasicTrainInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "function.h"
#include "BasicTrainInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasicTrainInfoDlg dialog


CBasicTrainInfoDlg::CBasicTrainInfoDlg(CWintgDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CBasicTrainInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBasicTrainInfoDlg)
	m_exit = 0;
	m_entry = 0;
	//}}AFX_DATA_INIT
    m_pDoc = pDoc;
}


void CBasicTrainInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBasicTrainInfoDlg)
	DDX_Text(pDX, IDC_EDIT1, m_exit);
	DDX_Text(pDX, IDC_EDIT2, m_entry);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBasicTrainInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CBasicTrainInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicTrainInfoDlg message handlers

BOOL CBasicTrainInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CSingleTrain *m_pTrain;
	DISPDISTANCE (*pDispDistance)[128]; // add by cf after 7-18
	CString str;
    Pstruct TRAINRECORD *rec;// = m_pTrain->GetIndexRecord(m_pTrain->m_nFocusRecordIndex + 1);

  	m_pTrain = m_pDoc->m_sFocusBasicTrain;
	pDispDistance = m_pDoc->m_sDispDistance;
    if(m_pTrain == NULL) return FALSE;
    
	int index = 0;
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_TRAIN_ID);
	pEdit->SetWindowText ( m_pTrain->m_strTrainId);

	if(m_pTrain->m_nFocusRecordIndex == -1)
		return FALSE;
	
    rec = m_pTrain->GetIndexRecord(m_pTrain->m_nFocusRecordIndex);
    pEdit = (CEdit *)GetDlgItem(IDC_STATION);
	pEdit->SetWindowText (gpcdef.GetStationName(rec->station));
    
    pEdit = (CEdit *)GetDlgItem(IDC_STOP_TIME);
	str = (CString)OsConvertTimetoString(rec->depart_time - rec->arrival_time 
		+ OsGetTwoDateOffset(PMTG.ThreadId,rec->depart_date,rec->arrival_date) * WHOLE_DAY_SECOND);
	pEdit->SetWindowText (str);
 
    str = OsConvertTimetoString(rec->min_stop_time);
    pEdit = (CEdit *)GetDlgItem(IDC_STOP_TIME_MIN);
	pEdit->SetWindowText (str);
	
    str = OsConvertTimetoString(rec->arrival_time);
    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ARRIVAL);
	pEdit->SetWindowText (str);

    str = OsConvertTimetoString(rec->depart_time);
    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DEPART);
	pEdit->SetWindowText (str);
	
    
	
	int couting ;
	for( couting=0;couting < m_pDoc->m_nDispDistanceNum;couting++)
	{
    	if((*m_pDoc->m_sDispDistance)[couting].nStationNo  == rec->station)
			break;
	}  	
	if(couting >= m_pDoc->m_nDispDistanceNum)
		return FALSE;


	int sidenumber=0;
	int countofbox = 0;
	
	countofbox = (*m_pDoc->m_sDispDistance)[couting].distance.number_of_side;
	for(int p=0;p<countofbox;p++)
	{
		sidenumber = (*m_pDoc->m_sDispDistance)[couting].distance.side[p+1].side_number;
		if(sidenumber == rec->arrive_side)  
			break;
	}
	str.Format("%s",(*m_pDoc->m_sDispDistance)[couting].distance.side[p+1].sidename);

	
	//str.Format("%d",rec->arrive_side);
    pEdit = (CEdit *)GetDlgItem(IDC_PRIMARY_SIDE);
	pEdit->SetWindowText (str);


	ushort nAbsNum,nDir;
    ConvertStationEntryToAbsnum(rec->station,rec->exit,&nAbsNum,&nDir);
	index = -1;
 	for(int i=0;i<m_pDoc->m_nAbsLineCount ;i++){
 		if(m_pDoc->abs_line_info[i].nLineNo == nAbsNum){
			str = CString(m_pDoc->abs_line_info[i].sLineName);
			pEdit = (CEdit *)GetDlgItem(IDC_NEXT_ABS);
			pEdit->SetWindowText (str);
 			break;
		}
	}

	{
		m_exit = rec->exit;
		m_entry = rec->entry;
	}

	InitDlgPosition(this);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
