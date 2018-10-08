// DlgSelectShift.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "tg_schedule_base.h"
#include "tgview/tgdocument.h"
#include "DlgSelectShift.h"
#include "tgmsg.h"

// CDlgSelectShift 对话框

IMPLEMENT_DYNAMIC(CDlgSelectShift, CDialog)
CDlgSelectShift::CDlgSelectShift(CWnd* pParent, int nShiftID)
: CDialog(CDlgSelectShift::IDD, pParent)
{
	m_nShiftID=nShiftID;
	m_nNextShiftID=CTG_ScheduleInfo::Get_NextShiftid(m_nShiftID);
}

CDlgSelectShift::~CDlgSelectShift()
{
}

void CDlgSelectShift::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgSelectShift, CDialog)
END_MESSAGE_MAP()

// CDlgSelectShift 消息处理程序
BOOL CDlgSelectShift::OnInitDialog()
{
	CDialog::OnInitDialog();
	CComboBox *pComBoBox=(CComboBox *)GetDlgItem(IDC_COMBO_SHIFT);
	if(pComBoBox==NULL)
		return TRUE;
	
	CTG_ScheduleInfo schdinfo(CTG_ScheduleInfo::SCHEDULE_TYPE_NULL,0, m_nShiftID);
	CTG_ScheduleInfo nextschdinfo(CTG_ScheduleInfo::SCHEDULE_TYPE_NULL,0, m_nNextShiftID);
	
	int sel=0;
	const char *shiftname=NULL;
	
	shiftname=schdinfo.GetShiftName();
	sel=pComBoBox->AddString(shiftname);
	pComBoBox->SetItemData(sel,m_nShiftID);
	
	shiftname=nextschdinfo.GetShiftName();
	sel=pComBoBox->AddString(shiftname);
	pComBoBox->SetItemData(sel,m_nNextShiftID);

	pComBoBox->SetCurSel(0);

	return TRUE;
}

void CDlgSelectShift::OnOK()
{
	CComboBox *pComBoBox=(CComboBox *)GetDlgItem(IDC_COMBO_SHIFT);
	if(pComBoBox==NULL)
		return;

	int sel=pComBoBox->GetCurSel();
	if(sel<0)
	{
		MessageBox("请选择 班次", "提示", MB_OK|MB_ICONWARNING);
		return;
	}

    m_nShiftID	=pComBoBox->GetItemData(sel); 

	CTG_ScheduleInfo schdinfo(CTG_ScheduleInfo::SCHEDULE_TYPE_NULL,0,m_nShiftID);
	schdinfo.GetShiftManagerRange(m_ntStartTime,m_ntEndTime);

	CDialog::OnOK();
}

