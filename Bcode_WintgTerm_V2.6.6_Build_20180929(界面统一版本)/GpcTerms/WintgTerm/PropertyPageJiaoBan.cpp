// PropertyPageJiaoBan.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "PropertyPageJiaoBan.h"
#include "sysshare.h"
#include "TgDocument.h"
#include "tgmsg.h"
#define IDC_TRAIN_JIAOBAN_GRID  5113
#define IDC_JIAOBAN_PLANLIST    5114
// CPropertyPageJiaoBan 对话框

IMPLEMENT_DYNAMIC(CPropertyPageJiaoBan, CPropertyPage)
CPropertyPageJiaoBan::CPropertyPageJiaoBan(CTgDocument *pdoc)
	: CPropertyPage(CPropertyPageJiaoBan::IDD)
{
	m_pDoc=pdoc;
	m_pNotifyWindow=NULL;
	m_nSectionNo=pdoc->GetCurrentSectionNo();
}

CPropertyPageJiaoBan::~CPropertyPageJiaoBan()
{
}

void CPropertyPageJiaoBan::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertyPageJiaoBan, CPropertyPage)
END_MESSAGE_MAP()


#define COL_PB_TRAIN_INDEX  0
#define COL_PB_TRAIN_NO  1
#define COL_PB_STATION  2
#define COL_PB_CMD_NO   3
#define COL_PB_FLAG     4
#define COL_PB_TIME     5


PropertyInfoPara g_jbplan_def[]=
{
	{0,0,CTC_DATA_STR,"JHH","车次ID"},
	{0,0,CTC_DATA_STR,"YDLX","车次号"},
	{3,0,CTC_DATA_STATION,"DBH","作业车站"},
	{0,0,CTC_DATA_STR,"MLH","命令号"},
	{0,0,CTC_DATA_STR,"XB","排班标志"},
	{0,0,CTC_DATA_DATETIME,"SXX","排班时间"}
};

BOOL CPropertyPageJiaoBan::OnInitDialog() 
{
	CPropertyPage::OnInitDialog(); 
	CStatic *pWnd;
	pWnd = (CStatic *)GetDlgItem(IDC_FRM_PROPERTY);
	CRect rect;
	pWnd->GetWindowRect(rect);
    ScreenToClient(&rect); 
	m_GridCtrl.CreateGrid(this,rect,IDC_TRAIN_JIAOBAN_GRID,g_jbplan_def,
		sizeof(g_jbplan_def)/sizeof(g_jbplan_def[0]));

	m_GridCtrl.SetEditable(TRUE);
	m_GridCtrl.SetFiledPara(m_nSectionNo);
	pWnd = (CStatic *)GetDlgItem(IDC_FRAME);
	pWnd->GetWindowRect(rect);
    ScreenToClient(&rect); 
	m_PlanList.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,rect,this,IDC_JIAOBAN_PLANLIST);
	InformParent();
	return TRUE;
}



BOOL CPropertyPageJiaoBan::Add1Train(long train_index,const char *train_no)
{
	CString tid_index;
	tid_index.Format("%d",train_index);
	int row=m_GridCtrl.InsertRow(tid_index);
	CGridCell *pCell=(CGridCell *)m_GridCtrl.GetCell(row,COL_PB_TRAIN_NO);
	if(pCell)
	{
		pCell->SetText(train_no);
	}
	m_GridCtrl.Invalidate(); 
	return TRUE;
	
}
void CPropertyPageJiaoBan::InformParent()
{
	TRAIN_SELECT_PARA para;
	CTG_ScheduleInfo schd;
	m_pDoc->GetScheduleInfo( schd);
	TIME shift_start,shift_end;
	schd.GetShiftManagerRange(shift_start,shift_end); 
	para.starttime= shift_start;
	para.endtime= shift_end;
	para.list_type=0; 
	if(m_pNotifyWindow)
		m_pNotifyWindow->SendMessage(UM_TG_TRAIN_SELECTED,(WPARAM)0,(LPARAM)&para);
}