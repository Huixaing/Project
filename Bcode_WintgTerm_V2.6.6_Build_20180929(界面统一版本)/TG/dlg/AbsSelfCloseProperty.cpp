// AbsSelfCloseProperty.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "Tg_BLOCK.h"
#include "AbsSelfCloseProperty.h"
#include ".\absselfcloseproperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsSelfCloseProperty dialog
const BYTE WIDTHSPACE = 5;
const BYTE HEIGHTSPACE = 5;
const BYTE NUM_OF_ROW  = 3;
const BYTE NUM_OF_COL  = 15;

CAbsSelfCloseProperty::CAbsSelfCloseProperty(CTGDoc *pDoc,CTg_BLOCK * pFigure,CWnd* pParent /*=NULL*/)
	: CDialog(CAbsSelfCloseProperty::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbsSelfCloseProperty)
	m_pFigure = pFigure;
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
}


void CAbsSelfCloseProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsSelfCloseProperty)
	DDX_Control(pDX, IDC_COMBO1, m_nDirect);
	DDX_Control(pDX, IDC_COMBO_UPDOWNPOS, m_TextPos);
	DDX_Control(pDX, IDC_COMBO3, m_nBlockType);
	DDX_Control(pDX, IDC_COMBO_REASON, m_strReason);

	DDX_CBString(pDX, IDC_COMBO_REASON, m_strreason);
	DDV_MaxChars(pDX, m_strreason, 255);  //实际上最大为255
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAbsSelfCloseProperty, CDialog)
	//{{AFX_MSG_MAP(CAbsSelfCloseProperty)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsSelfCloseProperty message handlers
#define NUM_DIRECTION  3
#define NUM_REASON     5
#define NUM_BLOCK      3

BOOL CAbsSelfCloseProperty::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
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

	m_nBlockType.AddString("半自动闭塞");
	m_nBlockType.AddString("自动闭塞");
	m_nBlockType.AddString("电话闭塞");
	m_nBlockType.AddString("停基改电"); 
	m_nBlockType.AddString("停用基本闭塞");
	m_nBlockType.AddString(" ");
	if(0 <= m_pFigure->flag && m_pFigure->flag <= 4)	
		m_nBlockType.SetCurSel((int)m_pFigure->flag);
	else
		m_nBlockType.SetWindowText("");

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
		_TCHAR *_gszReason[5] = {
		"大修",
		"更换磁头",
		"区间卸料",
		"区间停运",
		"区间作业"
		};

		for(int i = 0; i< 5; i++)
			m_strReason.InsertString(i,_gszReason[i]);
	}

	CString dcmdno, reason;
	m_pDoc->ParseMarkReasonText(m_pFigure->text, dcmdno, reason);

	m_strreason = reason;
	m_strReason.SetWindowText(reason);
	
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
	if(NULL != pWnd)
		pWnd->SetWindowText(dcmdno);
	
	UpdateData(FALSE);	
	
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		CWnd* pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);
	}

	CreateCheckBox();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CAbsSelfCloseProperty::OnOK() 
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
    strncpy(m_pFigure->text,(LPCTSTR)str,len);
	m_pFigure->text[len] = 0;

	//封锁方向 	0"上行", 1"下行", 2"上下行" 3"单线"
	m_pFigure->count = 0;
	int selIndex = m_nDirect.GetCurSel();
	if(selIndex == 0)
	{
		SetLineDirectionUp(m_pFigure->line);
		for(int j=0; j<m_linecount; j++)
		{
			if((m_line_list[j].dir == 0 || m_line_list[j].dir == 3)  && m_pcheckBoxList[j].GetCheck() == 1 && m_pFigure->count < MAX_SCHD_BLOCK_ELE_COUNT)
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
			if((m_line_list[j].dir == 1 || m_line_list[j].dir == 3)  && m_pcheckBoxList[j].GetCheck() == 1 && m_pFigure->count < MAX_SCHD_BLOCK_ELE_COUNT)
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
		MessageBox("请选中合法变更闭塞区间", "提示");
		return;
	}

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

	m_pFigure->flag = m_nBlockType.GetCurSel();
	
	CDialog::OnOK();
}

void CAbsSelfCloseProperty::CreateCheckBox()
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
void CAbsSelfCloseProperty::OnCbnSelchangeCombo1()
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
				m_pcheckBoxList[i].EnableWindow(0);
		}
		else if(selIndex == 1)
		{
			if(m_line_list[i].dir == 1 || m_line_list[i].dir == 3)
				m_pcheckBoxList[i].SetCheck(1);
			else
				m_pcheckBoxList[i].EnableWindow(0);
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
