// DlgEditStationSideName.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgEditStationSideName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEditStationSideName dialog


CDlgEditStationSideName::CDlgEditStationSideName(struct _tagStationStoreTrain * pInfo,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditStationSideName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditStationSideName)
	m_strSideName = _T("");
	//}}AFX_DATA_INIT
    m_pInfo = pInfo;
    m_nCurSel = 0;
}


void CDlgEditStationSideName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditStationSideName)
	DDX_Control(pDX, IDC_LIST, m_ListSideName);
	DDX_Text(pDX, IDC_SIDE_NAME, m_strSideName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditStationSideName, CDialog)
	//{{AFX_MSG_MAP(CDlgEditStationSideName)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditStationSideName message handlers

BOOL CDlgEditStationSideName::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_nCurSel = 0;
	CString strSideName;
	for(int i=0;i<m_pInfo->usSideNum;i++){
		strSideName = CString(m_pInfo->sideinfo[i].pszSideName);
        m_ListSideName.InsertString(i,strSideName);
		
	}
 	m_ListSideName.SetCurSel(m_nCurSel);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditStationSideName::OnAdd() 
{
    UpdateData();
	if(m_strSideName == _T("")){
		MessageBox("请输入股道名称!");
	    return;
	}
	m_nCurSel = m_ListSideName.GetCurSel();
   
    int count;
	count = m_ListSideName.GetCount();
	CString str;
	for(int i=0;i<count;i++){
	    m_ListSideName.GetText(i,str);
		if(str == m_strSideName)
			break;
	}
	if(i<count){
		CString str;
		str.Format("%s股道已存在!",m_strSideName);
 		MessageBox(str);
        return;
	}

	m_nCurSel ++;
	m_ListSideName.InsertString(m_nCurSel,m_strSideName);
	m_strSideName = _T("");
	m_ListSideName.SetCurSel(m_nCurSel);

	UpdateData(FALSE);
}

void CDlgEditStationSideName::OnDel() 
{
	int count = m_ListSideName.GetCount();
	if(count == 0) return;

 	CString strSideName;
	m_ListSideName.GetText(m_nCurSel,strSideName);

	for(int i=0;i<m_pInfo->usSideNum;i++){
		if(CString(m_pInfo->sideinfo[i].pszSideName) == strSideName){
			if(MessageBox("此股道的所有存车信息都将丢失!", "注意!", MB_OKCANCEL) != IDOK)
                return;
		}
	}

 	m_ListSideName.DeleteString(m_nCurSel);
    if(m_ListSideName.GetCount() > 0 && m_nCurSel == 0)
	   m_nCurSel = 0;
	else
	   m_nCurSel --;
	m_ListSideName.SetCurSel(m_nCurSel);
    UpdateData(FALSE);	
}

void CDlgEditStationSideName::OnOK() 
{
	struct _tagStationStoreTrain info;
    memcpy(&info,(void*)m_pInfo,sizeof(struct _tagStationStoreTrain));

    int count = m_ListSideName.GetCount();
	CString strSideName;
	bool bNewSide = TRUE;
	for(int i=0;i<count;i++)
	{
	    m_ListSideName.GetText(i,strSideName);
		bNewSide = TRUE;
        for(int k=0;k< info.usSideNum;k++)
		{
			if(info.sideinfo[k].pszSideName == strSideName)
			{
				memcpy(&m_pInfo->sideinfo[i],&info.sideinfo[k],sizeof(struct _tagSideStoredTrain));
			    bNewSide = FALSE;
			}
		}
        if(bNewSide){
           memset(&m_pInfo->sideinfo[i],0x00,sizeof(struct _tagSideStoredTrain));   
		}
 		m_pInfo->sideinfo[i].usSideNo = i;
        strcpy(m_pInfo->sideinfo[i].pszSideName,(char*)(LPCTSTR)strSideName);
	}
	m_pInfo->usSideNum = count;

	CDialog::OnOK();
}

void CDlgEditStationSideName::OnEdit() 
{
	int count = m_ListSideName.GetCount();
	if(count == 0) return;

	UpdateData();
	if(m_strSideName == _T("")){
		MessageBox("请输入股道名称!");
	    return;
	}
	CString strSideName;
	int oldSel = m_nCurSel;
	m_nCurSel = m_ListSideName.GetCurSel();
    if(m_nCurSel <0) return;   
	m_ListSideName.GetText(m_nCurSel,strSideName);
    for(int i=0;i< m_pInfo->usSideNum;i++){
		if(CString(m_pInfo->sideinfo[i].pszSideName) == strSideName){
			strcpy(m_pInfo->sideinfo[i].pszSideName,(char*)(LPCTSTR)m_strSideName);
			break;
		}
	}

 	m_ListSideName.DeleteString(m_nCurSel);
 	m_ListSideName.InsertString(m_nCurSel,m_strSideName);
	m_ListSideName.SetCurSel(oldSel);

 	UpdateData(FALSE);
}

void CDlgEditStationSideName::OnSelchangeList() 
{
    m_nCurSel = m_ListSideName.GetCurSel();
	m_ListSideName.GetText(m_nCurSel,m_strSideName);
	UpdateData(FALSE);

}

void CDlgEditStationSideName::OnUp() 
{
  	CString strSideName;
	m_nCurSel = m_ListSideName.GetCurSel();
	if(m_nCurSel <= 0) return;
	m_ListSideName.GetText(m_nCurSel,strSideName);
 	m_ListSideName.DeleteString(m_nCurSel);
	m_nCurSel --;
 	m_ListSideName.InsertString(m_nCurSel,strSideName);
	m_ListSideName.SetCurSel(m_nCurSel);
 	UpdateData(FALSE);	
}

void CDlgEditStationSideName::OnDown() 
{
  	CString strSideName;
	m_nCurSel = m_ListSideName.GetCurSel();
    int count = m_ListSideName.GetCount();
	if(m_nCurSel == count - 1) return;

	m_ListSideName.GetText(m_nCurSel,strSideName);
 	m_ListSideName.DeleteString(m_nCurSel);
	m_nCurSel ++;
 	m_ListSideName.InsertString(m_nCurSel,strSideName);
	m_ListSideName.SetCurSel(m_nCurSel);
 	UpdateData(FALSE);	
}
