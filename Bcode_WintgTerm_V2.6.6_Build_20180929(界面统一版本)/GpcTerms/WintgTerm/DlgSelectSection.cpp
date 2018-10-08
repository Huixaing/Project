// DlgSelectSection.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "sysshare.h"
#include "DlgSelectSection.h"


// CDlgSelectSection 对话框

IMPLEMENT_DYNAMIC(CDlgSelectSection, CDialog)
CDlgSelectSection::CDlgSelectSection(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectSection::IDD, pParent)
{
	m_nSectionNo=-1;
	m_nNeighborOfSection=-1;
}

CDlgSelectSection::~CDlgSelectSection()
{
}

void CDlgSelectSection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSelectSection, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSelectSection 消息处理程序
BOOL CDlgSelectSection::OnInitDialog() 
{
	int nSectionNum,nSectionList[MAX_SECTION_NUM];
	int cursel=0;
	nSectionNum = sizeof(nSectionList) / sizeof(nSectionList[0]);
	if(m_nNeighborOfSection==-1) // all section
		gpcdef.GetSectionList(nSectionNum,nSectionList);
	else
		gpcdef.GetNeighberViewPlanSectionList(m_nNeighborOfSection,nSectionNum,nSectionList); 
	for (int i = 0; i < nSectionNum; i++)//int
	{
 		CString strSection = gpcdef.GetSectionName(nSectionList[i]);
		int sel=((CComboBox *)GetDlgItem(IDC_CBO_SECTION))->AddString(strSection);
		if(sel>=0)
			((CComboBox *)GetDlgItem(IDC_CBO_SECTION))->SetItemData(sel,nSectionList[i]); 
		if(nSectionList[i]==m_nSectionNo)
			cursel=sel;
	}
	((CComboBox *)GetDlgItem(IDC_CBO_SECTION))->SetCurSel(cursel);
	return TRUE;
}

void CDlgSelectSection::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel=((CComboBox *)GetDlgItem(IDC_CBO_SECTION))->GetCurSel();
	if(sel>=0)
		m_nSectionNo=((CComboBox *)GetDlgItem(IDC_CBO_SECTION))->GetItemData(sel);
	else
		m_nSectionNo=-1;
	OnOK();
}
