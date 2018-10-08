// DlgGridInput.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgGridInput.h"

#define  IDC_GRID_XML_INPUT  5401

// CDlgGridInput 对话框

IMPLEMENT_DYNAMIC(CDlgGridInput, CDialog)
CDlgGridInput::CDlgGridInput(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGridInput::IDD, pParent)
{
	m_nFieldcnt=0;
}

CDlgGridInput::~CDlgGridInput()
{
}

void CDlgGridInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgGridInput, CDialog)
END_MESSAGE_MAP()

void   CDlgGridInput::Init(const char *caption,const PropertyInfoPara fielddef[],int fieldcnt, const char *initxml,const char *node)
{
	if(fieldcnt>MAX_FIELD_NUM)
		fieldcnt=MAX_FIELD_NUM;
	m_nFieldcnt=fieldcnt;
	for(int i=0;i<m_nFieldcnt;i++)
		m_aFielddef[i]=fielddef[i];
	m_xml=initxml;
	m_xmlnode=node;
	m_caption=caption;
}

// CDlgGridInput 消息处理程序

BOOL CDlgGridInput::OnInitDialog()
{
	CRect rect;
	SetWindowText(m_caption);
	GetDlgItem(IDC_GRID_FRAME)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_grid.m_bRowSelectable=TRUE;
    m_grid.CreateVerticalGrid(this,rect,IDC_GRID_XML_INPUT,m_aFielddef,m_nFieldcnt);
	m_grid.InitVerticalGrid(m_xml, m_xmlnode);
	m_grid.ExpandColumnsToFit(FALSE); 
	m_grid.SetEditable(TRUE);
    return TRUE;
}

void CDlgGridInput::OnOK()
{
	m_grid.SaveToXML(m_xml,"",m_xmlnode);
	CDialog::OnOK();
}