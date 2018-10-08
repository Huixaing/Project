// AbsFixProperty.cpp : implementation file
//

#include "stdafx.h"
#include "IF.h"

#include "AbsFixProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsFixProperty dialog
const BYTE WIDTHSPACE = 5;
const BYTE HEIGHTSPACE = 5;
const BYTE NUM_OF_ROW  = 3;
const BYTE NUM_OF_COL  = 15;
CAbsFixProperty::CAbsFixProperty(CTGDoc *pDoc,CTg_BLOCK * pFigure,CWnd* pParent /*=NULL*/)
	: CDialog(CAbsFixProperty::IDD, pParent)
	, m_str_reason(_T(""))
{
	//{{AFX_DATA_INIT(CAbsFixProperty)
		m_lockprestation = FALSE;
		m_locknextstation = FALSE;
		m_s_kilo = 0;
		m_e_meter = 0;
		m_s_meter = 0;
		m_e_kilo = 0;
	//}}AFX_DATA_INIT
	m_pFigure = pFigure;
	m_pDoc = pDoc;
	m_pcheckBoxList = NULL;
}

CAbsFixProperty::~CAbsFixProperty()
{
	if(m_pcheckBoxList != NULL)
	{
		delete []m_pcheckBoxList;
		m_pcheckBoxList = NULL;
	}
}

void CAbsFixProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsFixProperty)
	DDX_Control(pDX, IDC_COMBO2, m_strReason);
	DDX_CBString(pDX, IDC_COMBO2, m_str_reason);
	DDV_MaxChars(pDX, m_str_reason, 250);
	DDX_Control(pDX, IDC_COMBO_UPDOWNPOS, m_TextPos);
	DDX_Control(pDX, IDC_COMBO_LINEPOS, m_LinePos);
	DDX_Control(pDX, IDC_COMBO1, m_nDirect);
	DDX_Check(pDX, IDC_LOCK_PRESTATION, m_lockprestation);
	DDX_Check(pDX, IDC_LOCK_NEXTSTATION, m_locknextstation);

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

BEGIN_MESSAGE_MAP(CAbsFixProperty, CDialog)
	//{{AFX_MSG_MAP(CAbsFixProperty)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsFixProperty message handlers
BOOL CAbsFixProperty::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_s_kilo = m_pFigure->position;  
	m_s_meter = m_pFigure->s_meter;
	m_e_kilo  = m_pFigure->e_kilo ;
	m_e_meter = m_pFigure->e_meter;

	m_linecount = 0;
	memset(m_line_list,0,sizeof(m_line_list));

	int upIdx   = m_pFigure->station;
	int downIdx = m_pFigure->index; 
	if(m_pFigure->station > m_pFigure->index)
	{
		upIdx   = m_pFigure->index;
		downIdx = m_pFigure->station; 
	}
	std::vector<int> all_lineno;
	m_pDoc->GetMutiNeibAbsLines(upIdx, downIdx, all_lineno);
	for(int i=0; i<all_lineno.size()&&i<128; i++)
	{
		int line_no = all_lineno[i];
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

	m_nDirect.AddString("上行");   // 0
	m_nDirect.AddString("下行");   // 1 
	m_nDirect.AddString("上下行"); // 2
	m_nDirect.AddString("单线"); // 3

	m_nDirect.SetCurSel((m_pFigure->line&0x0003));
	
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

	//读取数据绘制原因
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
		_TCHAR *_gszReason[5] = 
		{
				"区间检修",
				"涵洞大修",
				"区间卸料",
				"区间停运",
				"区间作业"
		};
		for(int i = 0; i< 5; i++)
			m_strReason.InsertString(i,_gszReason[i]);
	}
	if(m_pFigure->slow_time&0x0001)//低位为1
		m_lockprestation = TRUE;
	else
		m_lockprestation = FALSE;

	if(m_pFigure->slow_time&0x0002)//第二位为1
		m_locknextstation = TRUE;
	else
		m_locknextstation = FALSE;

	UpdateData(FALSE);

	CString dcmdno, reason;
	m_pDoc->ParseMarkReasonText(m_pFigure->text, dcmdno, reason);

	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
	if(NULL != pWnd)
		pWnd->SetWindowText(dcmdno);

	m_strreason = reason;
	m_strReason.SetWindowText(reason);
   
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_LINEPOS);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_LOCK_PRESTATION);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_LOCK_NEXTSTATION);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_UPDOWNPOS);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDOK);
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
	CreateCheckBox();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CAbsFixProperty::OnOK() 
{
	if(m_pFigure == NULL)
		CDialog::OnOK();
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

	CString reason;
	m_strReason.GetWindowText (reason);
	
	CString str = m_pDoc->CombineReasonText(dcmdno, reason);
	int len = str.GetLength();
	if(len >= MAX_SCHD_BLOCK_TEXT_LEN)
	{
	     AfxMessageBox("文字<原因>太长,不能超过125个汉字，请重新输入!");
		 return;
	}
	strncpy(m_pFigure->text,(LPCTSTR)str, len);
	m_pFigure->text[len] = 0;
 
	//维修区域 	0"上行", 1"下行", 2"上下行" 3"单线"
	m_pFigure->count = 0;
	int selIndex = m_nDirect.GetCurSel();
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
	
	//selIndex = m_LinePos.GetCurSel();
	//if(selIndex == 0)//中间线的位置 	0"上",1"中",2"下"
	//	SetLineLinePosTop(m_pFigure->line);
	//else if(selIndex == 2)
	//	SetLineLinePosBottom(m_pFigure->line);
	//else 
	//	SetLineLinePosMiddle(m_pFigure->line);

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

		
	//是否封锁上下方车站,0,都不封锁，1，上方车站封锁，2，下方车站封锁,3上下方都封锁
	if(m_lockprestation)
		m_pFigure->slow_time |= 0x0001;//最低位置1
	else
		m_pFigure->slow_time &= 0xFFFE;//最低位置0

	if(m_locknextstation)
		m_pFigure->slow_time |= 0x0002;//第二位置1
	else
		m_pFigure->slow_time &= 0xFFFD;//第二位置0

	if(m_pFigure->count == 0)
	{
		MessageBox("请选中合法综合维修区间", "提示");
		return;
	}

	CDialog::OnOK();
}


void CAbsFixProperty::CreateCheckBox()
{
	if(m_linecount<=0)
		return;

	CRect buttonRect;
	CWnd *boxWnd = GetDlgItem(IDC_STATIC_LINEBOX);
    if(boxWnd == NULL)
		return;

	boxWnd->GetClientRect(&buttonRect);
	boxWnd->MapWindowPoints(this,&buttonRect);

	int width  = (buttonRect.Width() - 2*WIDTHSPACE) / NUM_OF_ROW;
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
void CAbsFixProperty::OnCbnSelchangeCombo1()
{
	int selIndex = m_nDirect.GetCurSel();
	for(int i=0; i<m_linecount; i++)
	{
		if(selIndex == 0)
		{
			if(m_line_list[i].dir == 0)
				m_pcheckBoxList[i].SetCheck(1);
			else
				m_pcheckBoxList[i].SetCheck(0);
		}
		else if(selIndex == 1)
		{
			if(m_line_list[i].dir == 1)
				m_pcheckBoxList[i].SetCheck(1);
			else
				m_pcheckBoxList[i].SetCheck(0);
		}
		else
			m_pcheckBoxList[i].SetCheck(1);

	}
}
