
#include "stdafx.h"
#include "resource.h"
#include "tg_basicpara.h"
#include "sysshare.h"
#include "function.h"
#include "tgdocument.h"
#include "DlgSideCalTrainSet.h"
#include ".\dlgsidecaltrainset.h"

CDlgSideCalTrainSet::CDlgSideCalTrainSet(int section, CWnd* pParent)
: CDialog(CDlgSideCalTrainSet::IDD, pParent)
{
	m_section=section;
	m_count=0;
}

CDlgSideCalTrainSet::~CDlgSideCalTrainSet()
{
}

void CDlgSideCalTrainSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_btnSave);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Control(pDX, IDC_LIST2, m_list2);
	DDX_Control(pDX, IDC_LIST3, m_list3);
}

BEGIN_MESSAGE_MAP(CDlgSideCalTrainSet, CDialog)
	ON_COMMAND(IDC_BUTTON1,OnBnClickedSave)
	ON_LBN_SELCHANGE(IDC_LIST1, OnDisplayTrainNoRange)
	ON_LBN_DBLCLK(IDC_LIST1, OnAppendSelectTrainType)
	ON_LBN_DBLCLK(IDC_LIST3, OnDeleteSelectTrainType)
END_MESSAGE_MAP()

BOOL CDlgSideCalTrainSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_count=0;
	CTGBasicParam * p=GetTGBasicParam();
	if(p!=NULL)
	{
		std::map<short, TRAIN_TYPES> train_types=p->m_cTrainTypeInfo.GetTrainType();
		std::map<short, TRAIN_TYPES>::iterator it=train_types.begin();
		for(;it!=train_types.end(); it++)
		{
			if(it->second.id>=128)
				continue;

			for(int i=0; i<m_count; i++)
			{
				if(m_traintype[i].id==it->second.id)
				{
					break;
				}
			}
			if(i>=m_count)
			{
				m_traintype[m_count].id=it->second.id;
				m_traintype[m_count].name=it->second.name.c_str();
				m_count++;
			}
		}
	}

	for(int i=0; i<m_count; i++)
	{
		int iSel=m_list1.AddString(m_traintype[i].name);
		m_list1.SetItemData(iSel, m_traintype[i].id);
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDlgSideCalTrainSet::OnDisplayTrainNoRange()
{
	// TODO: 在此添加控件通知处理程序代码
	m_list2.ResetContent();
	int iSel=m_list1.GetCurSel();
	if(iSel>=0)
	{
		int train_type=m_list1.GetItemData(iSel);
		CTGBasicParam * p=GetTGBasicParam();
		if(p!=NULL)
		{
			std::map<short, TRAIN_TYPES> train_types=p->m_cTrainTypeInfo.GetTrainType();
			std::map<short, TRAIN_TYPES>::iterator it=train_types.begin();
			for(;it!=train_types.end(); it++)
			{
				if(it->second.id!=train_type)
					continue;
				CString str;
				str.Format("%s字头 车次范围%d--%d", it->second.prefix.c_str(), it->second.start, it->second.end);
				int iPos=m_list2.AddString(str);
				if(iPos>=0)
				{
					m_list2.SetItemData(iPos, it->second.id);
				}
			}
		}
	}
}

void CDlgSideCalTrainSet::OnAppendSelectTrainType()
{
	// TODO: 在此添加控件通知处理程序代码
	int iSel=m_list1.GetCurSel();
	if(iSel>=0)
	{
		int train_type=m_list1.GetItemData(iSel);
		CString str;
		m_list1.GetText(iSel, str);

		int iPos=m_list3.AddString(str);
		if(iPos>=0)
		{
			m_list3.SetItemData(iPos, train_type);
		}
	}
}

void CDlgSideCalTrainSet::OnDeleteSelectTrainType()
{
	// TODO: 在此添加控件通知处理程序代码
	int iSel=m_list3.GetCurSel();
	m_list3.DeleteString(iSel);
}

void CDlgSideCalTrainSet::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFileName;
	strFileName.Format("%s\\user_cfg\\CalSideTrainList-%d.xml", gpcdef.GetDataDir(),m_section);
	CStdioFile file;
	if(!file.Open(strFileName, CFile::modeCreate | CFile::modeReadWrite))
	{
		MessageBox("保存失败,请确认data目录下是否存在目录user_cfg!", "错误");
		return;
	}
	
	CString str="<?xml version=\"1.0\" encoding=\"GB2312\"?>";
	file.WriteString(str);
	file.WriteString("\n<root>");
	CString line;
	for(int n=0; n<m_list3.GetCount(); n++)
	{
		CString str;
		int id=m_list3.GetItemData(n);
		
		m_list3.GetText(n, str);
		line.Format("\n\t<TRAIN train_type=\"%d\" name=\"%s\" />", id, str);
		file.WriteString(line);
	}
	file.WriteString("\n</root>");
	file.Close();

	CString str1;
	str1.Format("日班股道计算列车类型设置成功,存放在%s中", strFileName);
	MessageBox(str1, "提示");
}