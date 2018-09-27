// DlgNoteText.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "DlgNoteText.h"
#include ".\dlgnotetext.h"


// CDlgNoteText 对话框
static char *szRomeLetter[10] = { "Ⅰ","Ⅱ", "Ⅲ", "Ⅳ", "Ⅴ", "Ⅵ", "Ⅶ", "Ⅷ", "Ⅸ", "Ⅹ" };

IMPLEMENT_DYNAMIC(CDlgNoteText, CDialog)
CDlgNoteText::CDlgNoteText(int displayType,CTg_BLOCK * pFigure,CTGDoc *pDoc,CWnd* pParent)
	: CDialog(CDlgNoteText::IDD, pParent)
	, m_text(_T(""))
	, m_text2(_T(""))
{
	m_displayType = displayType;
	m_pFigure = pFigure;
	m_pDoc = pDoc;
	m_nType = 0;
}

CDlgNoteText::~CDlgNoteText()
{
}

void CDlgNoteText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEXT, m_text);
	DDV_MaxChars(pDX, m_text, 255);
	DDX_CBString(pDX, IDC_COMBO1, m_text2);
	DDV_MaxChars(pDX, m_text2, 255);
}


BEGIN_MESSAGE_MAP(CDlgNoteText, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_SHIGU, OnBnClickedRadioShigu)
	ON_BN_CLICKED(IDC_RADIO_NOTE, OnBnClickedRadioNote)
	ON_BN_CLICKED(IDC_RADIO_TEXT, OnBnClickedRadioText)
	ON_BN_CLICKED(IDC_RADIO_IMPORT, OnBnClickedRadioImport)
	ON_BN_CLICKED(IDC_RADIO_BAO, OnBnClickedRadioBao)
	ON_BN_CLICKED(IDC_RADIO_NO, OnBnClickedRadioNo)
	ON_BN_CLICKED(IDC_RADIO_RED_RECT, OnBnClickedRadioRedRect)
	ON_BN_CLICKED(IDC_RADIO_BLUE_RECT, OnBnClickedRadioBlueRect)
	ON_BN_CLICKED(IDC_RADIO_RED_ROUND, OnBnClickedRadioRedRound)
	ON_BN_CLICKED(IDC_RADIO_BLUE_ROUND, OnBnClickedRadioBlueRound)
	ON_BN_CLICKED(IDC_RADIO_RED_WORD, OnBnClickedRadioRedWord)
	ON_BN_CLICKED(IDC_RADIO_BLUE_WORD, OnBnClickedRadioBlueWord)
END_MESSAGE_MAP()


// CDlgNoteText 消息处理程序
BOOL CDlgNoteText::OnInitDialog() 
{
	CDialog::OnInitDialog();
   
	if(m_pFigure != NULL)
	{
		m_roundFlag = 0x0007&m_pFigure->slow_time;

		switch(m_pFigure->flag)
		{
		case 1://文字注解
			{
				CButton* pButton = (CButton *)GetDlgItem(IDC_RADIO_TEXT);
				if(pButton)
					pButton->SetCheck(BST_CHECKED);
				m_nType = 1;

				CWnd* pWnd = GetDlgItem(IDC_EDIT_TEXT);
				pWnd->SetWindowText(m_pFigure->text);
				m_text = CString(m_pFigure->text);
			}
			break;
		case 2://事故注解
			{
				CButton* pButton = (CButton *)GetDlgItem(IDC_RADIO_SHIGU);
				if(pButton)
					pButton->SetCheck(BST_CHECKED);
				m_nType = 2;
				
				CComboBox* pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
				for(int j=0;j<10;j++)
				{
					pCombo->InsertString(j, szRomeLetter[j]);
				}
				
				if(m_pFigure->text[0]==0)
				    pCombo->SetWindowText(szRomeLetter[0]);
				else
					pCombo->SetWindowText(m_pFigure->text);

				m_text.Empty();
			}
			break;
		case 3://重点注解
			{
				CButton* pButton = (CButton *)GetDlgItem(IDC_RADIO_IMPORT);
				if(pButton)
					pButton->SetCheck(BST_CHECKED);
				m_nType = 3;

				CWnd* pWnd = GetDlgItem(IDC_EDIT_TEXT);
				pWnd->EnableWindow(FALSE);
				pWnd->SetWindowText(m_pFigure->text);
				m_text = CString(m_pFigure->text);
			}
			break;
		case 4://记事注解
			{
				CButton* pButton = (CButton *)GetDlgItem(IDC_RADIO_NOTE);
				if(pButton)
					pButton->SetCheck(BST_CHECKED);
				m_nType = 4;
				
				CComboBox* pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
				pCombo->EnableWindow(TRUE);
				for(int i=0;i<10;i++)
				{
					CString temp;
					temp.Format("%d",i+1);

					pCombo->InsertString(i,temp);
				}
				if(m_pFigure->text[0]==0)
				    pCombo->SetWindowText("1");
				else
					pCombo->SetWindowText(m_pFigure->text);
				m_text.Empty();
			}
			break;
		case 5://保留车
			{
				CButton* pButton = (CButton *)GetDlgItem(IDC_RADIO_BAO);
				if(pButton)
					pButton->SetCheck(BST_CHECKED);
				m_nType = 5;

				CWnd* pWnd = GetDlgItem(IDC_EDIT_TEXT);
				pWnd->EnableWindow(FALSE);
				pWnd->SetWindowText(m_pFigure->text);
				m_text = CString(m_pFigure->text);
			}
			break;
		default:
			{
				CWnd* pWnd = GetDlgItem(IDC_EDIT_TEXT);
				pWnd->SetWindowText(m_pFigure->text);
				m_text = CString(m_pFigure->text);
				
				CButton* pButton = (CButton *)GetDlgItem(IDC_RADIO_TEXT);
				pButton->SetCheck(BST_CHECKED);

				m_nType = 0;
			}
		} 

		CButton* pButton = NULL;
		if(m_roundFlag == 1)
		{
			pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_RECT);
			pButton->SetCheck(BST_CHECKED);
			OnBnClickedRadioBlueRect();
		}
		else
		if(m_roundFlag == 2)
		{
			pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_RECT);
			pButton->SetCheck(BST_CHECKED);
			OnBnClickedRadioRedRect();
		}
		else
		if(m_roundFlag == 3)
		{
			pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_ROUND);
			pButton->SetCheck(BST_CHECKED);
			OnBnClickedRadioRedRound();
		}
		else
		if(m_roundFlag == 4)
		{
			pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_ROUND);
			pButton->SetCheck(BST_CHECKED);
			OnBnClickedRadioBlueRound();
		}
		else
		{
			pButton = (CButton *)GetDlgItem(IDC_RADIO_NO);
			pButton->SetCheck(BST_CHECKED);
			OnBnClickedRadioNo();
		}

		
		
		if(IsWordBlue(m_pFigure->slow_time))
		{
			pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_WORD);
			pButton->SetCheck(BST_CHECKED);
		}
		else
		{
			pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_WORD);
			pButton->SetCheck(BST_CHECKED);
		}


		m_colorFlag=0;
	}

	this->InitIdentify();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgNoteText::InitIdentify()
{
	CWnd *pWnd = NULL;
	CButton* pButton = NULL;
	//文字注释
	if(m_displayType == 1)
	{
		pButton = (CButton *)GetDlgItem(IDC_RADIO_SHIGU);
		pButton->EnableWindow(FALSE);

		pButton = (CButton *)GetDlgItem(IDC_RADIO_NOTE);
		pButton->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO1);
		pWnd->EnableWindow(FALSE);

		m_roundFlag = 0;

		pWnd = GetDlgItem(IDC_EDIT_TEXT);
		pWnd->SetFocus();
	}

	//记事注解
	if(m_displayType == 2)
	{
		pButton = (CButton *)GetDlgItem(IDC_RADIO_TEXT);
		pButton->EnableWindow(FALSE);
		
		pButton = (CButton *)GetDlgItem(IDC_RADIO_IMPORT);
		pButton->EnableWindow(FALSE);
		
		pButton = (CButton *)GetDlgItem(IDC_RADIO_BAO);
		pButton->EnableWindow(FALSE);

		pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_RECT);
		pButton->EnableWindow(FALSE);
		
		pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_RECT);
		pButton->EnableWindow(FALSE);
		
		pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_ROUND);
		pButton->EnableWindow(FALSE);

		pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_ROUND);
		pButton->EnableWindow(FALSE);

		pButton = (CButton *)GetDlgItem(IDC_RADIO_NO);
		pButton->EnableWindow(FALSE);

		
		pWnd = GetDlgItem(IDC_EDIT_TEXT);
		pWnd->EnableWindow(FALSE);
	}

	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		pButton = (CButton *)GetDlgItem(IDC_RADIO_TEXT);
		pButton->EnableWindow(FALSE);
		
		pButton = (CButton *)GetDlgItem(IDC_RADIO_IMPORT);
		pButton->EnableWindow(FALSE);
		
		pButton = (CButton *)GetDlgItem(IDC_RADIO_BAO);
		pButton->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_TEXT);
		pWnd->EnableWindow(FALSE);

		pButton = (CButton *)GetDlgItem(IDC_RADIO_SHIGU);
		pButton->EnableWindow(FALSE);

		pButton = (CButton *)GetDlgItem(IDC_RADIO_NOTE);
		pButton->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);
	}
}

void CDlgNoteText::OnBnClickedOk()
{
	UpdateData(TRUE);
  
	if(m_nType == 0)
	{
		AfxMessageBox("请选择文字注解类型!");
	    return;
	}
	else if(m_nType ==1 || m_nType ==3 || m_nType ==5)
	{
		if(m_text.GetLength() >= MAX_SCHD_BLOCK_TEXT_LEN)
		{
			AfxMessageBox("文字<原因>太长,不能超过128个汉字，请重新输入!");
			return;
		}
		else if(m_text.IsEmpty())
		{
			AfxMessageBox("文字注解内容不能为空!");
			return;
		}
	}
	else if(m_nType ==2 || m_nType ==4)
	{
		if(m_text2.GetLength() >= MAX_SCHD_BLOCK_TEXT_LEN)
		{
			AfxMessageBox("文字<原因>太长,不能超过128个汉字，请重新输入!");
			return;
		}
		else if(m_text2.IsEmpty())
		{
			AfxMessageBox("记事注解内容不能为空!");
			return;
		}
	}

	m_roundFlag = 0;
	
	CButton* pButton = NULL;
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_RECT);
	if(pButton->GetCheck())
		m_roundFlag = 1;
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_RECT);
	if(pButton->GetCheck())
		m_roundFlag = 2;
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_ROUND);
	if(pButton->GetCheck())
		m_roundFlag = 3;
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_ROUND);
	if(pButton->GetCheck())
		m_roundFlag = 4;

	m_colorFlag=0;
	
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_WORD);
	if(pButton->GetCheck())
		m_colorFlag = 1;
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_WORD);
	if(pButton->GetCheck())
		m_colorFlag = 2;
	
	OnOK();
}

void CDlgNoteText::OnBnClickedRadioShigu()
{
	CButton*pButton = (CButton *)GetDlgItem(IDC_RADIO_NOTE);
	if(pButton)
		pButton->SetCheck(BST_UNCHECKED);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_SHIGU);
	if(pButton)
		pButton->SetCheck(BST_CHECKED);
	m_nType = 2;

	CComboBox* pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
	pCombo->EnableWindow();
	pCombo->ResetContent();
	for(int j=0;j<10;j++)
	{
		pCombo->InsertString(j, szRomeLetter[j]);
	}
	
	if(m_pFigure->text[0]==0)
		pCombo->SetWindowText(szRomeLetter[0]);
	else
		pCombo->SetWindowText(m_pFigure->text);

	OnBnClickedRadioRedRound();
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_ROUND);
	pButton->SetCheck(BST_CHECKED);
}

void CDlgNoteText::OnBnClickedRadioNote()
{
	CButton*pButton = (CButton *)GetDlgItem(IDC_RADIO_SHIGU);
	if(pButton)
		pButton->SetCheck(BST_UNCHECKED);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_NOTE);
	if(pButton)
		pButton->SetCheck(BST_CHECKED);

	CComboBox* pCombo = (CComboBox *)GetDlgItem(IDC_COMBO1);
	pCombo->EnableWindow();
	pCombo->ResetContent();
	for(int i=0;i<10;i++)
	{
		CString temp;
		temp.Format("%d",i+1);

		pCombo->InsertString(i,temp);
	}
	if(m_pFigure->text[0]==0)
		pCombo->SetWindowText("1");
	else
		pCombo->SetWindowText(m_pFigure->text);

	m_nType = 4;

	OnBnClickedRadioBlueRound();
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_ROUND);
	pButton->SetCheck(BST_CHECKED);
}

void CDlgNoteText::OnBnClickedRadioText()
{
	CButton*pButton = (CButton *)GetDlgItem(IDC_RADIO_IMPORT);
	if(pButton)
		pButton->SetCheck(BST_UNCHECKED);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BAO);
	if(pButton)
		pButton->SetCheck(BST_UNCHECKED);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_TEXT);
	if(pButton)
		pButton->SetCheck(BST_CHECKED);
	CWnd* pWnd = GetDlgItem(IDC_EDIT_TEXT);
	pWnd->EnableWindow(TRUE);
	m_nType = 1;
}

void CDlgNoteText::OnBnClickedRadioImport()
{
	m_text = "＊";
	UpdateData(FALSE);

	CButton*pButton = (CButton *)GetDlgItem(IDC_RADIO_TEXT);
	if(pButton)
		pButton->SetCheck(BST_UNCHECKED);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BAO);
	if(pButton)
		pButton->SetCheck(BST_UNCHECKED);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_IMPORT);
	if(pButton)
		pButton->SetCheck(BST_CHECKED);
	CWnd* pWnd = GetDlgItem(IDC_EDIT_TEXT);
	pWnd->EnableWindow(FALSE);
	m_nType = 3;
}

void CDlgNoteText::OnBnClickedRadioBao()
{
	m_text = "保";
	UpdateData(FALSE);

	CButton*pButton = (CButton *)GetDlgItem(IDC_RADIO_TEXT);
	if(pButton)
		pButton->SetCheck(BST_UNCHECKED);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_IMPORT);
	if(pButton)
		pButton->SetCheck(BST_UNCHECKED);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BAO);
	if(pButton)
		pButton->SetCheck(BST_CHECKED);

	CWnd* pWnd = GetDlgItem(IDC_EDIT_TEXT);
	pWnd->EnableWindow(FALSE);
	m_nType = 5;
}

void CDlgNoteText::OnBnClickedRadioNo()
{
	CButton* pButton = NULL;
		
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_RECT);
	pButton->SetCheck(BST_UNCHECKED);
	
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_RECT);
	pButton->SetCheck(BST_UNCHECKED);
	
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_ROUND);
	pButton->SetCheck(BST_UNCHECKED);
	
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_ROUND);
	pButton->SetCheck(BST_UNCHECKED);
}

void CDlgNoteText::OnBnClickedRadioRedRect()
{
	CButton* pButton = NULL;

	pButton = (CButton *)GetDlgItem(IDC_RADIO_NO);
	pButton->SetCheck(BST_UNCHECKED);
		
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_RECT);
	pButton->SetCheck(BST_UNCHECKED);
	
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_ROUND);
	pButton->SetCheck(BST_UNCHECKED);
	
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_ROUND);
	pButton->SetCheck(BST_UNCHECKED);
}

void CDlgNoteText::OnBnClickedRadioBlueRect()
{
	CButton* pButton = NULL;

	pButton = (CButton *)GetDlgItem(IDC_RADIO_NO);
	pButton->SetCheck(BST_UNCHECKED);
		
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_RECT);
	pButton->SetCheck(BST_UNCHECKED);
		
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_ROUND);
	pButton->SetCheck(BST_UNCHECKED);
	
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_ROUND);
	pButton->SetCheck(BST_UNCHECKED);
}

void CDlgNoteText::OnBnClickedRadioRedRound()
{
	CButton* pButton = NULL;

	pButton = (CButton *)GetDlgItem(IDC_RADIO_NO);
	pButton->SetCheck(BST_UNCHECKED);
		
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_RECT);
	pButton->SetCheck(BST_UNCHECKED);
	
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_RECT);
	pButton->SetCheck(BST_UNCHECKED);
		
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_ROUND);
	pButton->SetCheck(BST_UNCHECKED);
}

void CDlgNoteText::OnBnClickedRadioBlueRound()
{
	CButton* pButton = NULL;

	pButton = (CButton *)GetDlgItem(IDC_RADIO_NO);
	pButton->SetCheck(BST_UNCHECKED);
		
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_RECT);
	pButton->SetCheck(BST_UNCHECKED);
	
	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_RECT);
	pButton->SetCheck(BST_UNCHECKED);
	
	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_ROUND);
	pButton->SetCheck(BST_UNCHECKED);
}
void CDlgNoteText::OnBnClickedRadioRedWord()
{
	CButton* pButton = NULL;

	pButton = (CButton *)GetDlgItem(IDC_RADIO_BLUE_WORD);
	pButton->SetCheck(BST_UNCHECKED);
}

void CDlgNoteText::OnBnClickedRadioBlueWord()
{
	CButton* pButton = NULL;

	pButton = (CButton *)GetDlgItem(IDC_RADIO_RED_WORD);
	pButton->SetCheck(BST_UNCHECKED);
}
