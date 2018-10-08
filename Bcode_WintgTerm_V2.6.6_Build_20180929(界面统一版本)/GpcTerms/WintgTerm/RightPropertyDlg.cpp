// UserPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RightListDlg.h"
#include "RightPropertyDlg.h"
#include ".\rightpropertydlg.h"
#include "TgView.h"
#include "memshare.h"

/////////////////////////////////////////////////////////////////////////////
// CRightPropertyDlg dialog
extern CLoadGPCDef m_load;

CRightPropertyDlg::CRightPropertyDlg(bool insert, CWnd* pParent /*=NULL*/)
: CDialog(CRightPropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRightPropertyDlg)
	m_con_name="";
	m_con_id=0;
	m_entity_id=0;
	m_entity_name="";
	m_insert = insert;
	m_parament = 0;
	m_Modify_Acutal=FALSE;
	role_param[0]=0;
	//}}AFX_DATA_INIT
}


void CRightPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRightPropertyDlg)
	DDX_Control(pDX, IDC_CON_LIST, m_con_list);
	DDX_Text(pDX, IDC_CON_ID , m_con_id);
	DDX_Text(pDX, IDC_ENTITY_ID , m_entity_id);
	DDX_Text(pDX, IDC_ENTITY_NAME, m_entity_name);
	DDX_Check(pDX, IDC_CHECK_MODIFY_ACTUAL, m_Modify_Acutal);
	DDX_Text(pDX, IDC_EDIT1_DLT_TIME , m_modify_time);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRightPropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CRightPropertyDlg)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_CON_LIST, OnCbnSelchangeConList)
	ON_BN_CLICKED(IDC_ENTER, OnBnClickedEnter)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDC_CHECK_ENTITY, OnBnClickedCheckEntity)
	ON_BN_CLICKED(IDC_CHECK_MODIFY_ACTUAL, OnBnClickedCheckModifyActual)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRightPropertyDlg message handlers

BOOL CRightPropertyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int section_no=0;
	CString section_name;

	for (int i=0; i<m_load.GetSectionListCount(); i++)	
	{
		section_no = m_load.GetSectionNameByIndex(i, section_name);

		int index = m_con_list.AddString(section_name);
		m_con_list.SetItemData(index, i);
		//m_con_id = section_no;
	}

	if (!m_insert)
	{
		((CComboBox*)GetDlgItem(IDC_CON_LIST))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_ENTITY_ID))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_ENTITY_NAME))->EnableWindow(false);

		GetDlgItem(IDC_STATIC_GPC)->ShowWindow(SW_HIDE);
	}
	else
	{
		m_con_id = 0;
	}

	if (m_parament & 0x0020)
		m_Modify_Acutal=TRUE;

	if (m_Modify_Acutal)
		((CEdit*)GetDlgItem(IDC_EDIT1_DLT_TIME))->EnableWindow(false);
	else
		((CEdit*)GetDlgItem(IDC_EDIT1_DLT_TIME))->EnableWindow(true);

	if (m_con_id!=0)
	{
		m_con_list.SetWindowText(m_con_name);
	}
	ParseRoleParam();
	

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRightPropertyDlg::OnCbnSelchangeConList()
{
	// TODO: 在此添加控件通知处理程序代码
	int i = m_con_list.GetCurSel();
	int index = m_con_list.GetItemData(i);
	int section_no=0;
	CString section_name;

	section_no = m_load.GetSectionNameByIndex(index, section_name);
	m_con_id = section_no;
	
	UpdateData(false);
}

void CRightPropertyDlg::OnBnClickedEnter()
{
	// TODO: 在此添加控件通知处理程序代码
	if (UpdateData() != TRUE)
		return;	

	m_con_list.GetWindowText(m_con_name);

	if (m_Modify_Acutal==TRUE)
		m_parament=0x0020;
	else 
		m_parament=0;
	
	SetRoleParam();
	if (m_entity_id==0)
	{
		AfxMessageBox("没有输入实体号");
		GetDlgItem(IDC_ENTITY_ID)->SetFocus();
	}
	else if (m_entity_name=="")
	{
		AfxMessageBox("请点击\"查询实体\"");
		GetDlgItem(IDC_ENTITY_ID)->SetFocus();
	}
	else
	{
		GPCENTITY entityinfo;
		if (!gpcdef.GetEntityInfo(m_entity_id, entityinfo))
		{
			AfxMessageBox("请检查实体号");
			GetDlgItem(IDC_ENTITY_ID)->SetFocus();
		}
		else
		{
			CString str;
			str.Format("%s",entityinfo.sName);
			if (m_entity_name != str)
			{
				AfxMessageBox("请重新键入实体号");
				GetDlgItem(IDC_ENTITY_ID)->SetFocus();
			}
			else
				CDialog::OnOK();
		}
	}
}

void CRightPropertyDlg::OnBnClickedClose()
{
	CDialog::OnCancel();
}

void CRightPropertyDlg::OnBnClickedCheckEntity()
{	
	if (UpdateData() != TRUE)
		return;	

	if(m_entity_id==0)
	{
		AfxMessageBox("请输入实体号!");
		return;
	}

	GPCENTITY entityinfo;
	if (gpcdef.GetEntityInfo(m_entity_id, entityinfo))
	{
		CString str1,str2;
		str1.Format("%s",entityinfo.sName);
		m_entity_name = str1;

		str2.Format("%s",entityinfo.sExpression);
		((CEdit*)GetDlgItem(IDC_TEXT_GPCID))->SetWindowText(str2);

		UpdateData(false);
	}
	else
	{
		AfxMessageBox("实体号不正确,请重新输入!");
		((CEdit*)GetDlgItem(IDC_ENTITY_ID))->SetFocus();
		return;
	}

}

void CRightPropertyDlg::ParseRoleParam()
{
	m_modify_time=0;
	
	char *token,*ptr=role_param;
	char seps[]=" ,\t\\/=\n";

	token = strtok(ptr, seps);
	if(token!=NULL)
	{
		m_modify_time=(int)atoi(token); 
	}
}

void CRightPropertyDlg::SetRoleParam()
{
	sprintf(role_param, "%d", m_modify_time);
}
void CRightPropertyDlg::OnBnClickedCheckModifyActual()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_MODIFY_ACTUAL))->GetCheck())
		((CEdit*)GetDlgItem(IDC_EDIT1_DLT_TIME))->EnableWindow(false);
	else
		((CEdit*)GetDlgItem(IDC_EDIT1_DLT_TIME))->EnableWindow(true);
}
