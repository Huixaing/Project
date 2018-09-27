// FenjiekouTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "FenjiekouTimeDlg.h"
#include ".\fenjiekoutimedlg.h"


// CFenjiekouTimeDlg 对话框

IMPLEMENT_DYNAMIC(CFenjiekouTimeDlg, CDialog)
CFenjiekouTimeDlg::CFenjiekouTimeDlg(CTGDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CFenjiekouTimeDlg::IDD, pParent)
{
	m_sborder_plan_tm=0;
	m_eborder_plan_tm=0;
	m_sborder_real_tm=0;
	m_eborder_real_tm=0;
	m_pDoc = pDoc;
	m_trainIndex=0;
}

CFenjiekouTimeDlg::~CFenjiekouTimeDlg()
{
}

void CFenjiekouTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER3, m_sborder_plan);
	DDX_Control(pDX, IDC_DATETIMEPICKER4, m_eborder_plan);
	DDX_Control(pDX, IDC_DATETIMEPICKER5, m_sborder_real);
	DDX_Control(pDX, IDC_DATETIMEPICKER6, m_eborder_real);
}


BEGIN_MESSAGE_MAP(CFenjiekouTimeDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CFenjiekouTimeDlg 消息处理程序
BOOL CFenjiekouTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_sborder_plan.SetFormat("yyyy-MM-dd HH:mm:ss");
	m_eborder_plan.SetFormat("yyyy-MM-dd HH:mm:ss");
	m_sborder_real.SetFormat("yyyy-MM-dd HH:mm:ss");
	m_eborder_real.SetFormat("yyyy-MM-dd HH:mm:ss");

	if(m_sborder_plan_tm<0)
		m_sborder_plan_tm=0;
	CTime timeTm1(m_sborder_plan_tm);
	m_sborder_plan.SetTime(&timeTm1);

	if(m_eborder_plan_tm<0)
		m_eborder_plan_tm=0;
	CTime timeTm2(m_eborder_plan_tm);
	m_eborder_plan.SetTime(&timeTm2);

	if(m_sborder_real_tm<0)
		m_sborder_real_tm=0;
	CTime timeTm3(m_sborder_real_tm);
	m_sborder_real.SetTime(&timeTm3);

	if(m_eborder_real_tm<0)
		m_eborder_real_tm=0;
	CTime timeTm4(m_eborder_real_tm);
	m_eborder_real.SetTime(&timeTm4);

	if(!m_pDoc->IsCanEditTrain())
	{
		m_sborder_plan.EnableWindow(FALSE);
		m_eborder_plan.EnableWindow(FALSE);
		m_sborder_real.EnableWindow(FALSE);
		m_eborder_real.EnableWindow(FALSE);
	}
	
	CRect rect, rect1;
	CWnd* pWnd = this->GetParent();
	if(NULL != pWnd)
	{
		pWnd->GetWindowRect(&rect);
		this->GetWindowRect(&rect1);
		int x = rect.CenterPoint().x-rect1.Width()/2;
		int y = rect.CenterPoint().y-rect1.Height()/2;
		this->SetWindowPos(pWnd, x,  y, rect1.Width(), rect1.Height(), SWP_SHOWWINDOW);
	}

	return TRUE;
}
void CFenjiekouTimeDlg::OnBnClickedOk()
{
	CTG_TRAIN* pTrain = m_pDoc->GetTrainByIndex(m_trainIndex);
	if(NULL == pTrain)
	{
		MessageBox("请注意: 该车可能已经删除, 修改分界口早晚点信息失败", "错误");
		return;
	}

	m_pDoc->sysprint(4,0,"[TG] CTGForm::CFenjiekouTimeDlg 修改分界口早晚点信息");
	CTime tm1;
	m_sborder_plan.GetTime(tm1);
	pTrain->myTrainInfo.sborder_plan_time = tm1.GetTime();
	m_pDoc->sysprint(4,0,"[TG] 计划开始 %d", pTrain->myTrainInfo.sborder_plan_time);

	m_eborder_plan.GetTime(tm1);
	pTrain->myTrainInfo.eborder_plan_time = tm1.GetTime();
	m_pDoc->sysprint(4,0,"[TG] 计划结束 %d", pTrain->myTrainInfo.eborder_plan_time);

	m_sborder_real.GetTime(tm1);
	pTrain->myTrainInfo.sborder_actual_time = tm1.GetTime();
	m_pDoc->sysprint(4,0,"[TG] 实际开始 %d", pTrain->myTrainInfo.sborder_actual_time);

	m_eborder_real.GetTime(tm1);
	pTrain->myTrainInfo.eborder_actual_time = tm1.GetTime();
	m_pDoc->sysprint(4,0,"[TG] 实际结束 %d", pTrain->myTrainInfo.eborder_actual_time);
	
	m_pDoc->SendUpdateTrain(pTrain);
	
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CFenjiekouTimeDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CDialog::PostNcDestroy();
}

void CFenjiekouTimeDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	this->DestroyWindow();
}
