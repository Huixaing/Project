// DlgLogFilter.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgLogFilter.h"
#include "sysshare.h"
#include <vector>

// CDlgLogFilter 对话框

IMPLEMENT_DYNAMIC(CDlgLogFilter, CDialog)
CDlgLogFilter::CDlgLogFilter(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogFilter::IDD, pParent)
	, m_szFilterKey(_T(""))
{
}

CDlgLogFilter::~CDlgLogFilter()
{
}

void CDlgLogFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgLogFilter, CDialog)
	ON_LBN_DBLCLK(IDC_LIST_STATION_NAME, OnLbnDblclkListStationName)
	ON_LBN_DBLCLK(IDC_LIST_USEFUL_KEYS, OnLbnDblclkListUsefulKeys)
	ON_BN_CLICKED(ID_CLEAR_KEY, OnBnClickedClearKey)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgLogFilter 消息处理程序

BOOL CDlgLogFilter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int i;
	GetDlgItem(IDC_ED_FILTER_KEY)->SetWindowText(m_szFilterKey);
	vector<int> sec_list;
	gpcdef.GetSectionListOfDisp( sec_list );
	for(i=0; i<sec_list.size(); i++)
	{
		vector<int> sta_list;
		gpcdef.GetStationListofDisp(sec_list[i], sta_list);
		for(int j=0; j<sta_list.size(); j++)
		{
			 CString name=gpcdef.GetStationName(sta_list[j]);
			((CListBox *)GetDlgItem(IDC_LIST_STATION_NAME))->AddString(name);
		}
	}
	FILE *fp;
	char log_key_file[256];
	sprintf(log_key_file,"%s\\logkeys.txt",gpcdef.GetDataDir());
	fp=fopen(log_key_file,"rt");
	if(fp)
	{
		char line[256];
		while(fgets(line,sizeof(line)-1,fp))
			((CListBox *)GetDlgItem(IDC_LIST_USEFUL_KEYS))->AddString(line);
		fclose(fp);
	}
	return TRUE;
}
void CDlgLogFilter::OnLbnDblclkListStationName()
{
	int sel=((CListBox *)GetDlgItem(IDC_LIST_STATION_NAME))->GetCurSel();
	if(sel>=0)
     {
		 CString cap;
		((CListBox *)GetDlgItem(IDC_LIST_STATION_NAME))->GetText(sel,cap);
	    GetDlgItem(IDC_ED_FILTER_KEY)->GetWindowText(m_szFilterKey);
		m_szFilterKey+=";";
		m_szFilterKey+=cap;
		GetDlgItem(IDC_ED_FILTER_KEY)->SetWindowText(m_szFilterKey);
	} 
}

void CDlgLogFilter::OnLbnDblclkListUsefulKeys()
{
	int sel=((CListBox *)GetDlgItem(IDC_LIST_USEFUL_KEYS))->GetCurSel();
	if(sel>=0)
     {
		 CString cap;
		((CListBox *)GetDlgItem(IDC_LIST_USEFUL_KEYS))->GetText(sel,cap);
	    GetDlgItem(IDC_ED_FILTER_KEY)->GetWindowText(m_szFilterKey);
		m_szFilterKey+=";";
		m_szFilterKey+=cap;
		GetDlgItem(IDC_ED_FILTER_KEY)->SetWindowText(m_szFilterKey);
	}
}

void CDlgLogFilter::OnBnClickedClearKey()
{
	 GetDlgItem(IDC_ED_FILTER_KEY)->SetWindowText("");
}

void CDlgLogFilter::OnBnClickedOk()
{
	GetDlgItem(IDC_ED_FILTER_KEY)->GetWindowText(m_szFilterKey);
	OnOK();
}
