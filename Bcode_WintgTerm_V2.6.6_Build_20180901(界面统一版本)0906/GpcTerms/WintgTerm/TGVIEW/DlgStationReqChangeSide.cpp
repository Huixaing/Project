
#include "stdafx.h"
#include "resource.h"
#include "tg_basicpara.h"
#include "sysshare.h"
#include "function.h"
#include "tgdocument.h"
#include "DlgStationReqChangeSide.h"
#include ".\DlgStationReqChangeSide.h"
#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"
BOOL CSideCHGGrid::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	m_result_col_count=0;
	strcpy(m_result_def[m_result_col_count].caption,"车次");
	sprintf(m_result_def[m_result_col_count].attr_name,"trainid");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"车站");
	sprintf(m_result_def[m_result_col_count].attr_name,"station");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"到达股道");
	sprintf(m_result_def[m_result_col_count].attr_name,"arrive_side");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"出发股道");
	sprintf(m_result_def[m_result_col_count].attr_name,"depart_side");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"选择");
	sprintf(m_result_def[m_result_col_count].attr_name,"choice");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_result_def,m_result_col_count);

	SetColumnWidth(0, 80);
	SetColumnWidth(1, 90);
	SetColumnWidth(2, 80);
	SetColumnWidth(3, 80);
	SetColumnWidth(4, 50);
		
	return TRUE;
}

void  CSideCHGGrid::AddSideDisplayRow(const NodeSideInfo& info, CString strName, BOOL choice)
{
	int row=InsertRow("");
	if(row<0)
		return;
	for (int col = 0; col < m_nFieldNum; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
		Item.row = row;
		Item.col = col;
		if(m_bSingleLine)
			Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
		else
			Item.nFormat = DT_LEFT|DT_VCENTER|DT_NOPREFIX;
		Item.lParam=0;

		if(col==0)
		{
			Item.strText = info.trainid;
			SetItem(&Item);
			GetCell(row,col)->SetData(info.train_index);
		}
		else if(col==1)
		{
			Item.strText = strName;
			SetItem(&Item);
			GetCell(row,col)->SetData(info.rec_index);
		}
		else if(col==2)
		{
			Item.strText = info.s_aside;
			SetItem(&Item);
			GetCell(row,col)->SetData(info.a_side);
		}
		else if(col==3)
		{
			Item.strText = info.s_dside;
			SetItem(&Item);
			GetCell(row,col)->SetData(info.d_side);
		}
		else if(col==4)
		{
			Item.strText = "";
			SetItem(&Item);
			SetCellType(row ,col, RUNTIME_CLASS(CGridCellCheck));

			((CGridCellCheck*)GetCell(row,col))->SetCheck(choice);
		}
	}
	if(info.s_aside!=info.s_dside)
	{
		GetCell(row,2)->SetBackClr(RGB(0,255,0));
		GetCell(row,3)->SetBackClr(RGB(0,255,0));
	}
	if(info.s_aside=="?G" || info.s_dside == "?G")
	{
		GetCell(row,2)->SetBackClr(RGB(0,255,0));
		GetCell(row,3)->SetBackClr(RGB(0,255,0));
	}
}

CDlgStationReqChangeSide::CDlgStationReqChangeSide(int station, CWnd* pParent)
: CDialog(CDlgStationReqChangeSide::IDD, pParent)
{
	m_count=0;
	m_nStationNo=station;
}

CDlgStationReqChangeSide::~CDlgStationReqChangeSide()
{
}

void CDlgStationReqChangeSide::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_station);
}

BEGIN_MESSAGE_MAP(CDlgStationReqChangeSide, CDialog)
END_MESSAGE_MAP()

void CDlgStationReqChangeSide::AddTrainSideRecord(const NodeSideInfo& info)
{
	if(m_count < 512)
	{
		siderecord[m_count].train_index=info.train_index;
		siderecord[m_count].rec_index=info.rec_index;
		siderecord[m_count].a_side=info.a_side;
		siderecord[m_count].d_side=info.d_side;
		siderecord[m_count].trainid=info.trainid;
		siderecord[m_count].s_aside=info.s_aside;
		siderecord[m_count].s_dside=info.s_dside;
		m_count++;
	}
}

BOOL CDlgStationReqChangeSide::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	CString staName=gpcdef.GetStationName(m_nStationNo);
	str.Format("%s 请求修改如下股道", staName);
	m_station.SetWindowText(str);
	
	CStatic *pWnd;
	pWnd = (CStatic *)GetDlgItem(IDC_GRID_AUTHOR_SEL);
	CRect rect;
	pWnd->GetWindowRect(rect);
	ScreenToClient(&rect);

	mGridCtrl.CreateGrid(this,rect,IDC_GRID_AUTHOR_SEL);
	mGridCtrl.SetSingleRowSelection(TRUE);
	mGridCtrl.SetEditable(TRUE);
	for (int row = 0; row < m_count; row++)
	{
		mGridCtrl.AddSideDisplayRow(siderecord[row], staName, TRUE);
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDlgStationReqChangeSide::OnCancel()
{
	m_count=0;
	GpcLogTrainMessage(0,0, "调度员选择取消修改车站申请的股道"); 
	CDialog::OnCancel();
}

void CDlgStationReqChangeSide::OnOK()
{
	// TODO: 在此添加控件通知处理程序代码
	if(MessageBox("你确认按照车站申请修改运行图股道吗?","操作提示",MB_YESNO)!=IDYES)
		return;
	GpcLogTrainMessage(0,0, "调度员选择确认修改车站申请的股道"); 
	int count=0;
	for(int row=1;row<mGridCtrl.GetRowCount();row++ )
	{
		CGridCellCheck *pCell = (CGridCellCheck*)mGridCtrl.GetCell(row,4);
		if(pCell && (pCell->GetCheck()==TRUE))
		{
			siderecord[count].train_index = mGridCtrl.GetCell(row,0)->GetData();
			siderecord[count].rec_index = mGridCtrl.GetCell(row,1)->GetData();
			siderecord[count].a_side = mGridCtrl.GetCell(row,2)->GetData();
			siderecord[count].d_side = mGridCtrl.GetCell(row,3)->GetData();
			
			count++;
		}
	}
	m_count=count;

	CDialog::OnOK();
}

