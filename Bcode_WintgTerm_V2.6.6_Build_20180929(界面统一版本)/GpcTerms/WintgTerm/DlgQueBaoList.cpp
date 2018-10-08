// DlgTrainList.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgQueBaoList.h"
#include <baseMsg\msgid.h>
#include <baseMsg\gpc_msgid.h>
#include <BaseMsg\clbth.h>
#include "icommtask.h"
#include "td/td_frame.h"
#include "td/td_data_def.h"
#include "tgpspdata.h"
#include "tg_basicpara.h"
#include ".\dlgquebaolist.h"
#include "sysshare.h"
#define IDC_SBTX_TRAINLIST  5211
#define IDC_SBTX_MARKLIST   5212
// CDlgQueBaoList 对话框

PropertyInfoPara g_quebao_header[]=
{
	//{0,0,CTC_DATA_STR,"RKTM","入库时间"},
	{0,0,CTC_DATA_STR,"COUNT","辆数"},
//	{0,0,CTC_DATA_STR,"SWT","自重(t)"},
	{0,0,CTC_DATA_STR,"LOAD","载重(t)"},
	{0,0,CTC_DATA_STR,"PBCNT","蓬布数"},
	{0,0,CTC_DATA_STR,"LENGTH","换长"},
	{0,0,CTC_DATA_STR,"LD_COUNT","重车数"},
	{0,0,CTC_DATA_STR,"EPY_COUNT","轻车数"},
};

PropertyInfoPara g_quebao_zhongche[]=
{
	//{0,0,CTC_DATA_STR,"RKTM","入库时间"},
	{0,0,CTC_DATA_STR,"FIRSTNM","重车去向"},
	{0,0,CTC_DATA_STR,"COUNT","辆数"},
};
PropertyInfoPara g_quebao_qingche[]=
{
	//{0,0,CTC_DATA_STR,"RKTM","入库时间"},
	{0,0,CTC_DATA_STR,"TYPE","轻车车种"},
	{0,0,CTC_DATA_STR,"COUNT","辆数"},
};

IMPLEMENT_DYNAMIC(CDlgQueBaoList, CDialog)
CDlgQueBaoList::CDlgQueBaoList(int stno,const char *trainno,DWORD gtid,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgQueBaoList::IDD, pParent)
{
	m_szTrainNo=trainno;
	m_TrainGTID=gtid;
	m_nStationID=stno;
}

CDlgQueBaoList::~CDlgQueBaoList()
{

}



void CDlgQueBaoList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgQueBaoList, CDialog)
	ON_NOTIFY(GVN_SELCHANGING,IDC_SBTX_TRAINLIST,OnGridSelectChange)
	ON_BN_CLICKED(ID_REQUEST_DATA, OnBnClickedReq)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_BN_CLICKED(ID_REQUEST_DEPART_DATA, OnBnClickedRequestDepartData)
END_MESSAGE_MAP()

LRESULT CDlgQueBaoList::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	 UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	 CLBTH::MsgStringMessage *pMsg= (CLBTH::MsgStringMessage  *)pMsgStruct->pmsg;
	 const char *token=pMsg->GetToken();
	 const char *content=pMsg->GetContent();
	 if(token && content)
	 {
		 if(!stricmp(token,TOKEN_1_ACK_BZ))
		 {
		    m_grid.DeleteNonFixedRows(); 
			m_gridZhongche.DeleteNonFixedRows(); 
			m_gridQingche.DeleteNonFixedRows(); 
			CString token_string;
			token_string.Format("%s/%s",token,"BZ_RECORD"); 
			m_grid.InsertRows(content,token_string); 
			CString hdr;
			if(TD_BZGenHeaderXML(hdr,content,token))
			{
				m_gridHeader.InitVerticalGrid(hdr,"BZ_HEADER");
				m_gridZhongche.InsertRows(hdr,"BZ_HEADER/BZ_ZHONGCHE");
				m_gridQingche.InsertRows(hdr,"BZ_HEADER/BZ_QINGCHE");
			}
		 }
	 }
	 FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	 return 0;
}
BOOL CDlgQueBaoList::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString  szCaption;
	szCaption.Format("确报查询-车次:%s  ID:%d 车站:%s", m_szTrainNo,m_TrainGTID,gpcdef.GetStationName(m_nStationID));
	SetWindowText(szCaption);
	CRect rect;
	GetDlgItem(IDC_GRID_FRAME)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_grid.m_bRowSelectable=FALSE;
    m_grid.CreateGrid(this,rect,IDC_SBTX_TRAINLIST,g_quebao_def,count_of_quebao_def());
	m_grid.SetEditable(FALSE);

	GetDlgItem(IDC_QB_HEADER)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_gridHeader.m_bRowSelectable=FALSE;
	m_gridHeader.CreateVerticalGrid(this,rect,IDC_QB_HEADER,g_quebao_header,sizeof(g_quebao_header)/sizeof(g_quebao_header[0]));
	m_gridHeader.SetEditable(FALSE);
	m_gridHeader.ExpandColumnsToFit();

	GetDlgItem(IDC_QB_ZHONGCHE)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_gridZhongche.m_bRowSelectable=FALSE;
	m_gridZhongche.CreateGrid(this,rect,IDC_QB_ZHONGCHE,g_quebao_zhongche,sizeof(g_quebao_zhongche)/sizeof(g_quebao_zhongche[0]));
	m_gridZhongche.SetEditable(FALSE);
	m_gridZhongche.ExpandColumnsToFit();

	GetDlgItem(IDC_QB_QINGCHE)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_gridQingche.m_bRowSelectable=FALSE;
	m_gridQingche.CreateGrid(this,rect,IDC_QB_QINGCHE,g_quebao_qingche,sizeof(g_quebao_qingche)/sizeof(g_quebao_qingche[0]));
	m_gridQingche.SetEditable(FALSE);
	m_gridQingche.ExpandColumnsToFit();

	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage,0);
	return TRUE;
}


void CDlgQueBaoList::OnOK()
{
}
// CDlgQueBaoList 消息处理程序


void CDlgQueBaoList::OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult) 
 {
	 NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	 if(pv->iRow>=1)
	 {
	 }
 }

void CDlgQueBaoList::OnBnClickedReq()
{
	CString token,content;
	TD_GenRequestMessage_1_BZ(token,content,m_TrainGTID,m_nStationID,NULL,0); //0: 到达，１出发
	if(!token.IsEmpty())
	{
		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
		TGSendStringMessage(token,content,eid);
	}
}


void CDlgQueBaoList::OnBnClickedRequestDepartData()
{
	CString token,content;
	TD_GenRequestMessage_1_BZ(token,content,m_TrainGTID,m_nStationID,NULL,1); //0: 到达，１出发
	if(!token.IsEmpty())
	{
		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
		TGSendStringMessage(token,content,eid);
	}
}
