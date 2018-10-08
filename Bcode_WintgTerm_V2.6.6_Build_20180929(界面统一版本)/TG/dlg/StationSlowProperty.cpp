// StationSlowProperty.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "StationSlowProperty.h"
#include "IF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationSlowProperty dialog


CStationSlowProperty::CStationSlowProperty(CTg_BLOCK * pFigure,CTGDoc *pDoc,CWnd* pParent, UINT temp)
	: CDialog(temp, pParent)
{
	//{{AFX_DATA_INIT(CStationSlowProperty)
	m_strScope = _T("");
	m_nSpeed = 0;
	m_strReason = _T("");
	m_s_kilo = 0;
	m_e_meter = 0;
	m_s_meter = 0;
	m_e_kilo = 0;
	//}}AFX_DATA_INIT
	m_pFigure = pFigure;
	checkBoxList = NULL;
	stationNo = 0; 
	m_pDoc = pDoc;
}


void CStationSlowProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationSlowProperty)
	DDX_Control(pDX, IDC_COMBO_LINEPOS, m_ComboLinePos);
	DDX_Control(pDX, IDC_COMBO_STRREASON, m_ctlReason);
	DDX_Text(pDX, IDC_EDIT5, m_nSpeed);
	DDV_MinMaxInt(pDX, m_nSpeed, 0, 9999);
	DDX_CBString(pDX, IDC_COMBO_STRREASON, m_strReason);
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


BEGIN_MESSAGE_MAP(CStationSlowProperty, CDialog)
	//{{AFX_MSG_MAP(CStationSlowProperty)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationSlowProperty message handlers

BOOL CStationSlowProperty::OnInitDialog() 
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
				"换股道",
				"更换磁头",
				"卸料",
				"停运",
				"作业"
		};
		for(int i = 0; i< 5; i++)
			m_ctlReason.InsertString(i,_gszReason[i]);
	}

	int couting = 0;
	for(couting=0; couting < m_pDoc->m_nDispDistanceNum; couting++)
	{
		if(m_pDoc->m_sDispDistance[couting].station_number == m_pFigure->station)
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

	m_nSpeed =  GetSlowSpeed(m_pFigure->flag);

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

void CStationSlowProperty::OnOK() 
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
  	
	m_pFigure->flag = SetSlowSpeed(m_nSpeed);
	
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

void CStationSlowProperty::CreateCheckBox()
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
	row = countofbox/5 + 1;
	width = (right - left - WIDTHSPACE * 6)/5;
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
		buttonRect.left = left + ((i) % 5) * (width+WIDTHSPACE);
		buttonRect.right = right + ((i)%5) * (width+WIDTHSPACE);
		buttonRect.top = top + ((i)/5) * (height +HEIGHTSPACE3);
		buttonRect.bottom = bottom + ((i)/5) *(height + HEIGHTSPACE3);
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

CStationSlowProperty::~CStationSlowProperty()
{
   if(checkBoxList != NULL)
   {
	   delete[] checkBoxList;
	   checkBoxList = NULL;
   }
}

void CStationSlowProperty::OnButton1() 
{
	int countofbox;
	
	countofbox = m_pDoc->m_sDispDistance[stationNo].number_of_side;
	
	for(int i =0;i < countofbox;i++)
	{
		checkBoxList[i].SetCheck(TRUE);
	}
}

void CStationSlowProperty::OnButton2() 
{
	int countofbox;
	
	countofbox = m_pDoc->m_sDispDistance[stationNo].number_of_side;
	
	for(int i =0;i < countofbox;i++)
	{
		checkBoxList[i].SetCheck(FALSE);
	}
}


void CStationSlowProperty::InitIdentify()
{
	if( !m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_COMBO_STRREASON);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT5);
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
