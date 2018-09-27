// StationInhibitProperty.cpp : implementation file
//

#include "stdafx.h"
#include "IF.h"
#include "StationInhibitProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationInhibitProperty dialog


CStationInhibitProperty::CStationInhibitProperty(CTg_BLOCK * pFigure,CTGDoc *pDoc,CWnd* pParent, UINT temp)
	: CDialog(temp, pParent)
{
	//{{AFX_DATA_INIT(CStationInhibitProperty)
	m_strScope = _T("");
	m_strReason = _T("");
	m_s_kilo = 0;
	m_e_meter = 0;
	m_s_meter = 0;
	m_e_kilo = 0;
	//}}AFX_DATA_INIT
	m_pFigure = pFigure;
	checkBoxList = NULL;//add by cf
	stationNo = 0; //add by cf
	m_pDoc = pDoc;

}

CStationInhibitProperty::~CStationInhibitProperty()
{
	if(checkBoxList != NULL)
	{
		delete[] checkBoxList;
		checkBoxList = NULL;
	}
}

void CStationInhibitProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationInhibitProperty)
	DDX_Control(pDX, IDC_COMBO_LINEPOS, m_ComboLinePos);
	DDX_Control(pDX, IDC_COMBO_REASON, m_ctlReason);
	DDX_CBString(pDX, IDC_COMBO_REASON, m_strReason);
	DDV_MaxChars(pDX, m_strReason, 255);  //实际上最大为255
 
	DDX_Text(pDX, IDC_EDIT1, m_s_kilo);
	DDV_MinMaxInt(pDX, m_s_kilo, 0, 9999);
	DDX_Text(pDX, IDC_E_METER, m_e_meter);
	DDV_MinMaxInt(pDX, m_e_meter, 0, 9999);
	DDX_Text(pDX, IDC_S_METER, m_s_meter);
	DDV_MinMaxInt(pDX, m_s_meter, 0, 9999);
	DDX_Text(pDX, IDC_E_KILO, m_e_kilo);
	DDV_MinMaxInt(pDX, m_e_kilo, 0, 9999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStationInhibitProperty, CDialog)
	//{{AFX_MSG_MAP(CStationInhibitProperty)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationInhibitProperty message handlers

BOOL CStationInhibitProperty::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_s_kilo = m_pFigure->position;  
	m_s_meter = m_pFigure->s_meter;
	m_e_kilo  = m_pFigure->e_kilo ;
	m_e_meter = m_pFigure->e_meter;
	//读取数据
	CString datadir = m_pDoc->GetDataPath();
	CStdioFile f_words;
	
	if(f_words.Open(datadir+"\\mark_text.cfg", CFile::modeRead))
	{
		CString linetext;
		while(f_words.ReadString(linetext))
		{
			m_ctlReason.InsertString(-1,linetext);
		}
		f_words.Close();
	}
	else
	{
		_TCHAR *_gszReason[5] = 
		{
				"股道检修",
				"更换磁头",
				"卸料",
				"停运",
				"作业"
		};
		for(int i = 0; i< 5; i++)
			m_ctlReason.InsertString(i,_gszReason[i]);
	}

	
	int couting ;
	for( couting=0;couting < m_pDoc->m_nDispDistanceNum;couting++)
	{
		if(m_pDoc->m_sDispDistance[couting].station_number  == m_pFigure->station)
			break;
	}  	
	if(couting >= m_pDoc->m_nDispDistanceNum)
		return FALSE;
	stationNo = couting;
	CreateCheckBox();

	CString dcmdno, reason;
	m_pDoc->ParseMarkReasonText(m_pFigure->text, dcmdno, reason);

	m_strReason = reason;
		
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
	if(NULL != pWnd)
		pWnd->SetWindowText(dcmdno);

	
	//文字位置
	m_ComboLinePos.InsertString(-1,"上");
	m_ComboLinePos.InsertString(-1,"下");
	m_ComboLinePos.InsertString(-1,"中");
	if(IsLineTextPosBottom(m_pFigure->line))
	{
		m_ComboLinePos.SetCurSel(1);
	}
	else if(IsLineTextPosMiddle(m_pFigure->line))
	{
		m_ComboLinePos.SetCurSel(2);
	}
	else
	{
		m_ComboLinePos.SetCurSel(0);
	}

	UpdateData(FALSE);

	this->InitIdentify();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStationInhibitProperty::OnOK() 
{
	UpdateData(TRUE);

	m_pFigure->position = m_s_kilo;
	m_pFigure->s_meter = m_s_meter;
	m_pFigure->e_kilo = m_e_kilo;
	m_pFigure->e_meter = m_e_meter;

	CString dcmdno = "";
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
	if(NULL != pWnd)
		pWnd->GetWindowText(dcmdno);

	CString str = m_pDoc->CombineReasonText(dcmdno, m_strReason);
	int len = str.GetLength();
	if(len >=  MAX_SCHD_BLOCK_TEXT_LEN)
	{
		AfxMessageBox("文字<原因>太长,不能超过128个汉字，请重新输入!");
		return;
	}
	strncpy(m_pFigure->text,(LPCTSTR)str, len);
	m_pFigure->text[len] = 0;

	int countofbox;
	
	m_pFigure->count = 0;

	countofbox = m_pDoc->m_sDispDistance[stationNo].number_of_side;
	 
	int sidenumber=0;
	for(int i=0;i<countofbox;i++)
	{
		if(checkBoxList[i].GetCheck() == 1 && m_pFigure->count < MAX_SCHD_BLOCK_ELE_COUNT)
		{
            sidenumber = m_pDoc->m_sDispDistance[stationNo].side[i].side_number;
			m_pFigure->ele_number[m_pFigure->count ++] = sidenumber;
		}
	}
	
	int selIndex = m_ComboLinePos.GetCurSel();
	if(selIndex == 1)
		SetLineTextPosBottom(m_pFigure->line);
	else if(selIndex == 2)
		SetLineTextPosMiddle(m_pFigure->line);
	else
		SetLineTextPosTop(m_pFigure->line);

	CDialog::OnOK();
}

//this function is add by cf

void CStationInhibitProperty::CreateCheckBox()
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
	bottom = buttonRect.bottom-20;
	top = buttonRect.top;
	row = countofbox/5 + 1;
	width = (right - left - WIDTHSPACE * 6)/5;
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
		buttonRect.left = left + ((i) % 5) * (width+WIDTHSPACE);
		buttonRect.right = right + ((i)%5) * (width+WIDTHSPACE);
		buttonRect.top = top + ((i)/5) * (height +HEIGHTSPACE4);
		buttonRect.bottom = bottom + ((i)/5) *(height + HEIGHTSPACE4);
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

//add by cf
void CStationInhibitProperty::OnButton1() 
{
	// TODO: Add your control notification handler code here
	int countofbox;
	
	countofbox = m_pDoc->m_sDispDistance[stationNo].number_of_side;
	
	for(int i =0;i < countofbox;i++)
	{
		checkBoxList[i].SetCheck(TRUE);
	}
	
}

void CStationInhibitProperty::OnButton2() 
{
	int countofbox;
	
	countofbox = m_pDoc->m_sDispDistance[stationNo].number_of_side;
	
	for(int i =0;i < countofbox;i++)
	{
		checkBoxList[i].SetCheck(FALSE);
	}
	
}

void CStationInhibitProperty::InitIdentify()
{
	if(m_pFigure->type == SIDE_OCCUPY_MARK)
		this->SetWindowText("站内股道占用");
	else
	if(m_pFigure->type == SIDE_TIANCHUANG_MARK)
		this->SetWindowText("站内天窗");
	else
		this->SetWindowText("站内封锁");
		
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_COMBO_REASON);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BUTTON1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BUTTON2);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_E_KILO);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_S_METER);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_E_METER);
		pWnd->EnableWindow(FALSE);
	}
}
