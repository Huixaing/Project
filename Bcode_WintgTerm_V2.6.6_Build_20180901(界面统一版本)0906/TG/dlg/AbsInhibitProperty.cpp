// AbsInhibitProperty.cpp : implementation file
//

#include "stdafx.h"
#include "IF.h"
#include "AbsInhibitProperty.h"
#include ".\absinhibitproperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsInhibitProperty dialog
const BYTE WIDTHSPACE  = 5;
const BYTE HEIGHTSPACE = 5;
const BYTE NUM_OF_ROW  = 2;
const BYTE NUM_OF_COL  = 4;


CAbsInhibitProperty::CAbsInhibitProperty(CTGDoc *pDoc,CTg_BLOCK * pFigure,CWnd* pParent /*=NULL*/)
: CDialog(CAbsInhibitProperty::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbsInhibitProperty)
	m_s_kilo = 0;
	m_e_meter = 0;
	m_s_meter = 0;
	m_e_kilo = 0;
	//}}AFX_DATA_INIT
	m_pFigure = pFigure;
	m_pDoc = pDoc;
	m_pcheckBoxList = NULL;
	
}

CAbsInhibitProperty::~CAbsInhibitProperty()
{
	if(m_pcheckBoxList != NULL)
	{
		delete []m_pcheckBoxList;
		m_pcheckBoxList = NULL;
	}
}
void CAbsInhibitProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsInhibitProperty)
	DDX_Control(pDX, IDC_COMBO_LINEPOS, m_LinePos);
	DDX_Control(pDX, IDC_COMBO_UPDOWNPOS, m_TextPos);
	DDX_Control(pDX, IDC_COMBO2, m_strReason);
	DDX_Control(pDX, IDC_COMBO1, m_nDirect);

	DDX_Text(pDX, IDC_EDIT1, m_s_kilo);
	DDV_MinMaxInt(pDX, m_s_kilo, 0, 9999);
	DDX_Text(pDX, IDC_E_METER, m_e_meter);
	DDV_MinMaxInt(pDX, m_e_meter, 0, 9999);
	DDX_Text(pDX, IDC_S_METER, m_s_meter);
	DDV_MinMaxInt(pDX, m_s_meter, 0, 9999);
	DDX_Text(pDX, IDC_E_KILO, m_e_kilo);
	DDV_MinMaxInt(pDX, m_e_kilo, 0, 9999);

	DDX_CBString(pDX, IDC_COMBO2, m_strreason);
	DDV_MaxChars(pDX, m_strreason, 255);  //实际上最大为63
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbsInhibitProperty, CDialog)
	//{{AFX_MSG_MAP(CAbsInhibitProperty)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsInhibitProperty message handlers
#define NUM_REASON     5

_TCHAR *_gszReason[NUM_REASON] = 
{
		"区间检修",
		"涵洞大修",
		"区间卸料",
		"区间停运",
		"区间作业"
};

void CAbsInhibitProperty::InitIdentify()
{
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_COMBO1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO2);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_LINEPOS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_UPDOWNPOS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_E_KILO);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_S_METER);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_E_METER);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
		pWnd->EnableWindow(FALSE);
	}
}

void CAbsInhibitProperty::OnCbnSelchangeCombo1()
{
	int selIndex = m_nDirect.GetCurSel();
	for(int i=0; i<m_linecount; i++)
	{
		m_pcheckBoxList[i].EnableWindow(TRUE);
		m_pcheckBoxList[i].SetCheck(0);

		if(selIndex == 0)
		{
			if(m_line_list[i].dir == 0 || m_line_list[i].dir == 3)
				m_pcheckBoxList[i].SetCheck(1);
			else
				m_pcheckBoxList[i].EnableWindow(FALSE);
		}
		else if(selIndex == 1)
		{
			if(m_line_list[i].dir == 1 || m_line_list[i].dir == 3)
				m_pcheckBoxList[i].SetCheck(1);
			else
				m_pcheckBoxList[i].EnableWindow(FALSE);
		}
		else if(selIndex == 3)
		{
			if(m_line_list[i].dir == 3)
				m_pcheckBoxList[i].SetCheck(1);
			else
				m_pcheckBoxList[i].EnableWindow(FALSE);
		}
		else
		{
			m_pcheckBoxList[i].SetCheck(1);
		}
	}
}

BOOL CAbsInhibitProperty::OnInitDialog() 
{
	CDialog::OnInitDialog();
    	
	m_linecount = 0;
	memset(m_line_list,0,sizeof(m_line_list));

	std::vector<int> abs_list;
	m_pDoc->GetNeibAbsLines(m_pFigure->station, m_pFigure->index, abs_list);
	for(int i=0; i<abs_list.size(); i++)
	{
		int line_no = abs_list[i];
		const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(line_no);
		if(line!=NULL)
		{
			memset(m_line_list[m_linecount].name, 0, 32);
			strncpy(m_line_list[m_linecount].name, line->sLineName, 31);
			m_line_list[m_linecount].lineNo = line_no;
			m_line_list[m_linecount].dir    = line->nDirect;
			m_linecount++;
		}
	}
	abs_list.clear();

	m_nDirect.AddString("上行");   // 0
	m_nDirect.AddString("下行");   // 1 
	m_nDirect.AddString("上下行"); // 2
	m_nDirect.AddString("单线"); // 3

	m_nDirect.SetCurSel((m_pFigure->line&0x0003));

	m_LinePos.AddString("上");
	m_LinePos.AddString("中");
	m_LinePos.AddString("下");
	
	m_LinePos.SetCurSel((m_pFigure->line&0x0300)>>8);

	m_TextPos.AddString("上");
	m_TextPos.AddString("下");
	m_TextPos.AddString("中");
	m_TextPos.AddString("左");
	m_TextPos.AddString("右");

	if(IsLineTextPosBottom(m_pFigure->line))
	{
		m_TextPos.SetCurSel(1);
	}
	else if(IsLineTextPosMiddle(m_pFigure->line))
	{
		m_TextPos.SetCurSel(2);
	}
	else if(IsLineTextPosLeft(m_pFigure->line))
	{
		m_TextPos.SetCurSel(3);
	}
	else if(IsLineTextPosRight(m_pFigure->line))
	{
		m_TextPos.SetCurSel(4);
	}
	else
	{
		m_TextPos.SetCurSel(0);
	}
	
	

	CString datadir = m_pDoc->GetDataPath();
	CStdioFile f_words;

	if(f_words.Open(datadir+"\\mark_text.cfg", CFile::modeRead))
	{
		CString linetext;
		while(f_words.ReadString(linetext))
		{
			m_strReason.InsertString(-1,linetext);
		}
		f_words.Close();
	}
	else
	{
		for(int i = 0; i< NUM_REASON; i++)
			m_strReason.InsertString(i,_gszReason[i]);
	}

	m_s_kilo = m_pFigure->position;  
	m_s_meter = m_pFigure->s_meter;
	m_e_kilo  = m_pFigure->e_kilo ;
	m_e_meter = m_pFigure->e_meter;

	CString dcmdno, reason;
	m_pDoc->ParseMarkReasonText(m_pFigure->text, dcmdno, reason);

	m_strreason = reason;
	
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
	if(NULL != pWnd)
		pWnd->SetWindowText(dcmdno);

	if(IsLineFlagLightTrain(m_pFigure->line))
	{
		SetWindowText("其他");
		CWnd* pwnd = NULL;
		pwnd = GetDlgItem(IDC_EDIT1);
		pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_S_METER);
		pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_E_METER);
		pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_E_KILO);
		pwnd->EnableWindow(FALSE);	

	}
	UpdateData(FALSE);
	InitIdentify();
	CreateCheckBox();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//动态创建checkbox
void CAbsInhibitProperty::CreateCheckBox()
{
	if(m_linecount<=0)
		return;

	CRect buttonRect;
	CWnd *boxWnd = GetDlgItem(IDC_STATIC_LINEBOX);
    if(boxWnd == NULL)
		return;

	boxWnd->GetClientRect(&buttonRect);
	boxWnd->MapWindowPoints(this,&buttonRect);

	int width  = (buttonRect.Width() - 2*WIDTHSPACE)/NUM_OF_ROW;
	int height = (buttonRect.Height()- 3*HEIGHTSPACE)/NUM_OF_COL;

	buttonRect.left   = buttonRect.left + WIDTHSPACE;
	buttonRect.right  = buttonRect.left + width;
	buttonRect.top    = buttonRect.top + 4*HEIGHTSPACE;
	buttonRect.bottom = buttonRect.top + height;

	int left = buttonRect.left;
	int right = buttonRect.right;
	int bottom = buttonRect.bottom;
	int top = buttonRect.top;

	m_pcheckBoxList = new CButton[m_linecount];

	for(int i=1;i<=m_linecount;i++)
	{
		CString strBtnName;
		strBtnName.Format("%s(%d)", m_line_list[i-1].name, m_line_list[i-1].lineNo);
		
		m_pcheckBoxList[i-1].Create(strBtnName, 
			                        WS_CHILD | WS_VISIBLE | WS_TABSTOP|BS_AUTOCHECKBOX, 
			                        buttonRect, 
									this, 
									IDC_CHECKBOX_START + i);
		m_pcheckBoxList[i-1].SetFont(this->GetFont());
		buttonRect.left   = left   + ((i)%NUM_OF_ROW) * (width);
		buttonRect.right  = right  + ((i)%NUM_OF_ROW) * (width);
		buttonRect.top    = top    + ((i)/NUM_OF_ROW) * (height);
		buttonRect.bottom = bottom + ((i)/NUM_OF_ROW) * (height);
	}

	for(int i=0; i<m_pFigure->count; i++)
	{
		for(int j=0; j<m_linecount; j++)
		{
			if(m_line_list[j].lineNo == m_pFigure->ele_number[i])
			{
				m_pcheckBoxList[j].SetCheck(TRUE);
				break;
			}
		}
	}

	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		for(int j=0; j<m_linecount; j++)
		{
			m_pcheckBoxList[j].EnableWindow(FALSE);
		}
	}
}

void CAbsInhibitProperty::OnOK() 
{    
	if(m_pFigure == NULL)
		CDialog::OnOK();

	UpdateData(TRUE);

	CString dcmdno = "";
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
	if(NULL != pWnd)
		pWnd->GetWindowText(dcmdno);

	CString reason;
	m_strReason.GetWindowText (reason);
	
	CString str = m_pDoc->CombineReasonText(dcmdno, reason);
	int len = str.GetLength();
	if(len >=  MAX_SCHD_BLOCK_TEXT_LEN)
	{
		AfxMessageBox("文字<原因>太长,不能超过128个汉字，请重新输入!");
		return;
	}
	strncpy(m_pFigure->text,(LPCTSTR)str, len);
	m_pFigure->text[len] = 0;

	int selIndex;
	//封锁方向 	0"上行", 1"下行", 2"上下行" 3"单线"
	m_pFigure->count = 0;
	selIndex = m_nDirect.GetCurSel();
	if(selIndex == 0)
	{
		SetLineDirectionUp(m_pFigure->line);
		for(int j=0; j<m_linecount; j++)
		{
			if((m_line_list[j].dir == 0 || m_line_list[j].dir == 3) && m_pcheckBoxList[j].GetCheck() == 1 && m_pFigure->count < MAX_SCHD_BLOCK_ELE_COUNT)
			{
				m_pFigure->ele_number[m_pFigure->count] = m_line_list[j].lineNo;
				m_pFigure->count++;
			}
		}
	}
	else if(selIndex == 1)
	{
		SetLineDirectionDown(m_pFigure->line);
		for(int j=0; j<m_linecount; j++)
		{
			if((m_line_list[j].dir == 1 || m_line_list[j].dir == 3) && m_pcheckBoxList[j].GetCheck() == 1 && m_pFigure->count < MAX_SCHD_BLOCK_ELE_COUNT)
			{
				m_pFigure->ele_number[m_pFigure->count] = m_line_list[j].lineNo;
				m_pFigure->count++;
			}
		}
	}
	else
	{
		if(selIndex == 2)
			SetLineDirectionDual(m_pFigure->line);
		else
			SetLineDirectionNone(m_pFigure->line);
		
		for(int j=0; j<m_linecount; j++)
		{
			if(m_pcheckBoxList[j].GetCheck() == 1 && m_pFigure->count < MAX_SCHD_BLOCK_ELE_COUNT)
			{
				m_pFigure->ele_number[m_pFigure->count] = m_line_list[j].lineNo;
				m_pFigure->count++;
			}
		}
	}

	if(m_pFigure->count == 0)
	{
		MessageBox("请选中合法封锁区间", "提示");
		return;
	}

	//中间线的位置 	0"上",1"中",2"下"
	selIndex = m_LinePos.GetCurSel();
	if(selIndex == 0)
		SetLineLinePosTop(m_pFigure->line);
	else if(selIndex == 2)
		SetLineLinePosBottom(m_pFigure->line);
	else 
		SetLineLinePosMiddle(m_pFigure->line);

	//文字的位置 	0"上",1"下"
	selIndex = m_TextPos.GetCurSel();
	if(selIndex == 0)
		SetLineTextPosTop(m_pFigure->line);
	else if(selIndex == 1)
		SetLineTextPosBottom(m_pFigure->line);
	else if(selIndex == 2)
		SetLineTextPosMiddle(m_pFigure->line);
	else if(selIndex == 3)
		SetLineTextPosLeft(m_pFigure->line);
	else if(selIndex == 4)
		SetLineTextPosRight(m_pFigure->line);


	m_pFigure->position = m_s_kilo;
	m_pFigure->s_meter = m_s_meter;
	m_pFigure->e_kilo = m_e_kilo;
	m_pFigure->e_meter = m_e_meter;

	CDialog::OnOK();
}

















