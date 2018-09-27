// DlgMMPlanInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgMMPlanInfo.h"
#include "tgview/tgdocument.h"
#include "TGPspData.h"
#include "utility/xml_utility.h"
// CDlgMMPlanInfo 对话框
extern BOOL  CallMessageLoop();

IMPLEMENT_DYNAMIC(CDlgMMPlanInfo, CDialog)
CDlgMMPlanInfo::CDlgMMPlanInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMMPlanInfo::IDD, pParent)
{

	m_bCanClose=TRUE;
}

CDlgMMPlanInfo::~CDlgMMPlanInfo()
{
}

void CDlgMMPlanInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MPLAN_TAB, m_tab);
}


BEGIN_MESSAGE_MAP(CDlgMMPlanInfo, CDialog)
	ON_NOTIFY(TCN_SELCHANGE,IDC_MPLAN_TAB,OnSelchangeTabctrl)
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_BTN_REFRESH, OnBnClickedBtnRefresh)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()



int CDlgMMPlanInfo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}


BOOL CDlgMMPlanInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    CString caption;
	caption.Format("施工计划信息-日期:%s-计划号:%s",GET_RQSTR_FROM_PLANID(m_planid),GET_JHH_FROM_PLANID(m_planid));
	SetWindowText(caption);

	CRect rect(0,0,0,0);
	GetDlgItem(IDC_TAB_FRAME)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_jieshi_edit.Create(WS_CHILD|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY|WS_BORDER,rect,this,12345);
	m_trn_grid.CreateGrid(this,rect,12346,g_mplan_trninfo_def,count_of_mplan_trninfo_def());
	m_trn_grid.SetEditable(FALSE);
	m_info_grid.CreateVerticalGrid(this,rect,12347,g_mmplan_note_view_info,count_of_g_mmplan_note_view_info());
	m_info_grid.SetEditable(TRUE);

	m_info_grid.ExpandColumnsToFit(FALSE);
	m_info_grid.ExpandRowsToFit(FALSE); 

	{
	TCITEM  tcItem;
	tcItem.mask = TCIF_TEXT|TCIF_PARAM;
	tcItem.pszText = "施工计划信息";
	tcItem.lParam =0;
	m_tab.InsertItem(0, &tcItem);
	}
	{
	TCITEM  tcItem;
	tcItem.mask = TCIF_TEXT|TCIF_PARAM;
	tcItem.pszText = "施工揭示命令";
	tcItem.lParam =1;
	m_tab.InsertItem(1, &tcItem);
	}
	{
	TCITEM  tcItem;
	tcItem.mask = TCIF_TEXT|TCIF_PARAM;
	tcItem.pszText = "路用车信息";
	tcItem.lParam =2;
	m_tab.InsertItem(2, &tcItem);
	}
	UpdateShow();
	OnSelchangeTabctrl(NULL,NULL);
	return TRUE;
}
void CDlgMMPlanInfo::OnSelchangeTabctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{ 
	
	int x=m_tab.GetCurSel();//所选标签号
	TCITEM item;
	item.mask =TCIF_PARAM;
	m_tab.GetItem(x,&item);
	int iPage=item.lParam; 
	if(iPage==0) // 施工计划信息
	{
		m_tab.SetCurSel(0);
		m_info_grid.ShowWindow(SW_SHOW); //输入新词汇
		m_trn_grid.ShowWindow(SW_HIDE);
		m_jieshi_edit.ShowWindow(SW_HIDE);
	}
	else
	if(iPage==1) // 施工揭示命令
	{
		m_tab.SetCurSel(1);
		m_info_grid.ShowWindow(SW_HIDE); //输入新词汇
		m_trn_grid.ShowWindow(SW_HIDE);
		m_jieshi_edit.ShowWindow(SW_SHOW);
	}
	else
	if(iPage==2) // 路用车信息
	{
		m_tab.SetCurSel(2);
		m_info_grid.ShowWindow(SW_HIDE); //输入新词汇
		m_trn_grid.ShowWindow(SW_SHOW);
		m_jieshi_edit.ShowWindow(SW_HIDE);
	}
}

void CDlgMMPlanInfo::UpdateShow()
{
	
	if(!m_pDoc->ReadMPLANFile(m_info,m_planid,XMLTYPE_MPLAN_INFO))
			 {
				m_info="";
			 }
	if(!m_pDoc->ReadMPLANFile(m_jieshi,m_planid,XMLTYPE_MPLAN_JIESHI))
			 {
				m_jieshi=""; 
			 }
	if(!m_pDoc->ReadMPLANFile(m_trninfo,m_planid,XMLTYPE_MPLAN_TRNINFO))
			 {
				 m_trninfo="";
			 }

	CTD_MPLAN_SG_WX mmobj;
	CString infoxml;
	mmobj.LoadSG_WXFromTDXML(m_info,XMLTYPE_MPLAN_INFO);
    mmobj.GenViewInfoXML(infoxml,"MMPLAN_INFO");
	m_info_grid.InitVerticalGrid(infoxml,"MMPLAN_INFO");
	
	m_trn_grid.DeleteNonFixedRows();
	CString subnode;
	subnode.Format("%s/RECORD",XMLTYPE_MPLAN_TRNINFO); 
	m_trn_grid.InsertRows(m_trninfo,subnode);
	m_trn_grid.ExpandColumnsToFit(); 
	m_trn_grid.Refresh();

	CString cap;
	GetXMLNodeValue(cap,m_jieshi,XMLTYPE_MPLAN_JIESHI);
	m_jieshi_edit.SetWindowText(cap); 
}

void CDlgMMPlanInfo::OnBnClickedBtnRefresh()
{
	int index=m_pDoc->FindMMPlanBySubID(m_planid);
    if(index>=0)
	  {
		m_pDoc->RequestMMPlanInfoFromOPMS(index,0x01);  
		GpcLogTrainMessage(0, 0, "施工计划:获取单个信息");
	  }
    m_pDoc->OpenWaitWindow("请稍候正在接收数据...");
	m_bCanClose=FALSE; // 不能关闭窗口
	time_t waitfrom=time(NULL);
	while(CallMessageLoop())  // 等待commwindow接收消息
	{
		if((time(NULL)-waitfrom>2))
			break;
	}
	m_pDoc->OpenWaitWindow(NULL);
	UpdateShow();
	m_bCanClose=TRUE; 
}

void CDlgMMPlanInfo::OnClose()
{
	if(m_bCanClose)
		CDialog::OnClose();
}

void CDlgMMPlanInfo::OnBnClickedCancel()
{
	if(m_bCanClose)
		CDialog::OnCancel();
}
