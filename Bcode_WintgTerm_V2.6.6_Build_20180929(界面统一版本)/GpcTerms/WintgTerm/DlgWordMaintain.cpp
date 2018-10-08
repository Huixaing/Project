// DlgWordMaintain.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgWordMaintain.h"
#include "sysshare.h"


// CDlgWordMaintain 对话框

IMPLEMENT_DYNAMIC(CDlgWordMaintain, CDialog)
CDlgWordMaintain::CDlgWordMaintain(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWordMaintain::IDD, pParent)
{
	m_bEdited=FALSE;
}

CDlgWordMaintain::~CDlgWordMaintain()
{
}


void CDlgWordMaintain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_CBO_WORD_TYPE,m_cbotype);
	DDX_Control(pDX,IDC_ED_NEWWORD,m_newword);
	DDX_Control(pDX,IDC_LIST_WORD,m_listallwords);
}


BEGIN_MESSAGE_MAP(CDlgWordMaintain, CDialog)
	ON_BN_CLICKED(IDC_BTN_WORD_ADD, OnBnClickedBtnWordAdd)
	ON_BN_CLICKED(IDC_BTN_WORD_EDIT, OnBnClickedBtnWordEdit)
	ON_BN_CLICKED(IDC_BTN_WORD_DELETE, OnBnClickedBtnWordDelete)
	ON_CBN_SELCHANGE(IDC_CBO_WORD_TYPE, OnSelTypeChange)
	ON_LBN_SELCHANGE(IDC_LIST_WORD, OnSelWordChange)
	ON_BN_CLICKED(ID_BTN_SAVE, OnSave)
END_MESSAGE_MAP()


// CDlgWordMaintain 消息处理程序
struct struWordFileDesc{
	char caption[64];
	char filename[64];
}g_word_file_desc[]=
{
	{"阶段计划常用词汇","notepad.txt"},
	{"施工标记常用词汇","mark_text.cfg"}
};

#define FILE_TYPE_COUNT sizeof(g_word_file_desc)/sizeof(g_word_file_desc[0])

void CDlgWordMaintain::RefreshList()
{
	CString filepath;
	filepath.Format("%s/%s",gpcdef.GetDataDir(),m_szFileName);
	CStdioFile f_words;
	m_listallwords.ResetContent(); 
	if(f_words.Open(filepath,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead))
	{
		CString linetext;
		while(f_words.ReadString(linetext))
			m_listallwords.AddString(linetext);

		f_words.Close();
	}
}

void CDlgWordMaintain::OnSelTypeChange()
{
	if(m_bEdited)
	{
		if(MessageBox("您刚才修改的常用词汇还没有保存,要保存吗?","警告",MB_YESNO)==IDYES)
				OnSave();
	}
    int sel=m_cbotype.GetCurSel();
	if(sel>=0)
	{
		int index=m_cbotype.GetItemData(sel); 
		if(index>=0)
		{
			m_szFileName=g_word_file_desc[index].filename;
			RefreshList();
		}
	}
}

void CDlgWordMaintain::OnSelWordChange()
{
	int sel=m_listallwords.GetCurSel();
	if(sel>=0)
	{
		CString cap;
		m_listallwords.GetText(sel,cap); 
		m_newword.SetWindowText(cap);
	}
	else
		m_newword.SetWindowText("");
}

BOOL CDlgWordMaintain::OnInitDialog()
{
	CDialog::OnInitDialog();
	for(int i=0;i<	FILE_TYPE_COUNT;i++)
	{
		int sel=m_cbotype.AddString(g_word_file_desc[i].caption);
	    m_cbotype.SetItemData(sel,i); 
	}
	m_cbotype.SetCurSel(0);
	m_szFileName=g_word_file_desc[0].filename;
	RefreshList();
	m_bEdited=FALSE;
	return TRUE;
}

void CDlgWordMaintain::OnBnClickedBtnWordAdd()
{
	CString cap;
	m_newword.GetWindowText(cap); 
	int sel=m_listallwords.GetCurSel(); 
	if(sel>=0)
		m_listallwords.InsertString(sel,cap);
	else
	   m_listallwords.InsertString(0,cap);
	m_bEdited=TRUE;
}

void CDlgWordMaintain::OnBnClickedBtnWordEdit()
{
	CString cap;
	m_newword.GetWindowText(cap); 
	int sel=m_listallwords.GetCurSel(); 
	if(sel<0)
		return;
	m_listallwords.DeleteString(sel); 
	m_listallwords.InsertString(sel,cap);
	m_bEdited=TRUE;
}

void CDlgWordMaintain::OnBnClickedBtnWordDelete()
{
	int sel=m_listallwords.GetCurSel(); 
	if(sel<0)
		return;
	m_listallwords.DeleteString(sel); 
	m_bEdited=TRUE;
}

void CDlgWordMaintain::OnSave()
{
	CString filepath;
	filepath.Format("%s/%s",gpcdef.GetDataDir(),m_szFileName);
	CStdioFile f_words;	
	if(f_words.Open(filepath,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
	{
		for(int i=0;i<m_listallwords.GetCount();i++)
		{
			CString str;
			m_listallwords.GetText(i,str); 
			f_words.WriteString(str);
			f_words.WriteString("\n");
		}

		f_words.Close();
	}
	m_bEdited=FALSE;
}

void CDlgWordMaintain::OnOK()
{
	if(m_bEdited)
	{
		if(MessageBox("您刚才修改的常用词汇还没有保存,要保存吗?","警告",MB_YESNO)==IDYES)
				OnSave();
	}

	CDialog::OnOK();
}