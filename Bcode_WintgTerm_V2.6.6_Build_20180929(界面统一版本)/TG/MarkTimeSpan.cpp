// MarkTimeSpan.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "MarkTimeSpan.h"
#include "IF.h"
#include ".\marktimespan.h"

// CMarkTimeSpan 对话框

IMPLEMENT_DYNAMIC(CMarkTimeSpan, CDialog)
CMarkTimeSpan::CMarkTimeSpan(CWnd* pParent /*=NULL*/)
	: CDialog(CMarkTimeSpan::IDD, pParent)
{ 
	 
	
}

CMarkTimeSpan::CMarkTimeSpan(time_t s_time, time_t e_time,
		CWnd* pParent /*=NULL*/): CDialog(CMarkTimeSpan::IDD, pParent)
{
	m_s_time = s_time;
	m_e_time = e_time;
}


CMarkTimeSpan::~CMarkTimeSpan()
{
}

void CMarkTimeSpan::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SGDATE_ST, st_date);
	DDX_Control(pDX, IDC_SGDATE_ET, et_date);
	DDX_Control(pDX, IDC_SGTIME_ST, st_time);
	DDX_Control(pDX, IDC_SGTIME_ET, et_time);
}


BEGIN_MESSAGE_MAP(CMarkTimeSpan, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_SGDATE_ST, OnDtnDatetimechangeDateSt)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_SGTIME_ST, OnDtnDatetimechangeSgtimeSt)
END_MESSAGE_MAP()


// CMarkTimeSpan 消息处理程序

void CMarkTimeSpan::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	//提取重新设置好的时间
	CTime n_st_date,n_et_date,n_st_time,n_et_time;
	st_date.GetTime(n_st_date);
    et_date.GetTime(n_et_date);
    st_time.GetTime(n_st_time);
	et_time.GetTime(n_et_time);
	//将这四个控件中的日期和时间进行合并后再进行格式转换
	CTime new_st_time(n_st_date.GetYear(), n_st_date.GetMonth(), n_st_date.GetDay(),
			n_st_time.GetHour(), n_st_time.GetMinute(), n_st_time.GetSecond());
	CTime new_ed_time(n_et_date.GetYear(), n_et_date.GetMonth(), n_et_date.GetDay(),
			n_et_time.GetHour(), n_et_time.GetMinute(), n_et_time.GetSecond());
    //
    m_s_time = new_st_time.GetTime();
    m_e_time = new_ed_time.GetTime();
    
	if(m_s_time>=m_e_time)
	{
		MessageBox("开始时间不能大于等于结束时间！", "警告", MB_OK|MB_ICONWARNING);
		return;
	}
	OnOK();
}

void CMarkTimeSpan::OnDtnDatetimechangeDateSt(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CMarkTimeSpan::getTime(time_t &s_time, time_t &e_time)
{
	s_time = m_s_time;
	e_time = m_e_time;
}


BOOL CMarkTimeSpan::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//初始化当前时间控件
	CTime ct_s_time(m_s_time);
	CTime ct_e_time(m_e_time);

	st_date.SetTime(&ct_s_time);
	et_date.SetTime(&ct_e_time);
	st_time.SetTime(&ct_s_time);
	et_time.SetTime(&ct_e_time);
	
  	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CMarkTimeSpan::OnDtnDatetimechangeSgtimeSt(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
