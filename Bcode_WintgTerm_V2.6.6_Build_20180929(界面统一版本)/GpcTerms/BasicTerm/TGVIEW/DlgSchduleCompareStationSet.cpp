
#include "stdafx.h"
#include "resource.h"
#include "tg_basicpara.h"
#include "sysshare.h"
#include "function.h"
#include "tgdocument.h"
#include "DlgSchduleCompareStationSet.h"
#include ".\DlgSchduleCompareStationSet.h"
#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"
BOOL CStationCheckGrid::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	m_result_col_count=0;
	strcpy(m_result_def[m_result_col_count].caption,"车站");
	sprintf(m_result_def[m_result_col_count].attr_name,"station");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"基本图");
	sprintf(m_result_def[m_result_col_count].attr_name,"b");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=2;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"日班图");
	sprintf(m_result_def[m_result_col_count].attr_name,"s");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=2;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"调整图");
	sprintf(m_result_def[m_result_col_count].attr_name,"w");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=2;
	m_result_col_count++;

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_result_def,m_result_col_count);

	SetColumnWidth(0, 100);
	SetColumnWidth(1, 80);
	SetColumnWidth(2, 80);
	SetColumnWidth(3, 80);
		
	return TRUE;
}

void  CStationCheckGrid::AddRow(int station, int b, int s, int w)
{
	int row=InsertRow("");
	if(row<0)
		return;
	CString sta_name=gpcdef.GetStationName(station);
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
			Item.strText = sta_name;
			SetItem(&Item);
			GetCell(row,0)->SetData(station);
		}
		else
		{
			Item.strText = "";
			SetItem(&Item);
			SetCellType(row ,col, RUNTIME_CLASS(CGridCellCheck));
			BOOL bSetCheck=FALSE;
			if(col==1 && b==1)
				bSetCheck=TRUE;
			if(col==2 && s==1)
				bSetCheck=TRUE;
			if(col==3 && w==1)
				bSetCheck=TRUE;

			((CGridCellCheck*)GetCell(row,col))->SetCheck(bSetCheck);
		}
	}
}

CDlgSchdCompareStationSet::CDlgSchdCompareStationSet(int section, CWnd* pParent)
: CDialog(CDlgSchdCompareStationSet::IDD, pParent)
{
	m_section=section;
	m_station_count=0;
	memset(m_station_list, 0, sizeof(m_station_list));
		
	m_station_count = GetTGFunctionConfig()->GetThisSectionStation(section, m_station_list, 128);
}

CDlgSchdCompareStationSet::~CDlgSchdCompareStationSet()
{
}

void CDlgSchdCompareStationSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_btnSave);
}

BEGIN_MESSAGE_MAP(CDlgSchdCompareStationSet, CDialog)
	ON_COMMAND(IDC_BUTTON1,OnBnClickedSave)
END_MESSAGE_MAP()

BOOL CDlgSchdCompareStationSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	CheckStationsOfSection st_check;
	GetTGFunctionConfig()->LoadCompareStationList(m_section, st_check);

	CStatic *pWnd;
	pWnd = (CStatic *)GetDlgItem(IDC_GRID4);
	CRect rect;
	pWnd->GetWindowRect(rect);
	ScreenToClient(&rect);

	mGridCtrl.CreateGrid(this,rect,IDC_GRID4);
	mGridCtrl.SetSingleRowSelection(TRUE);
	mGridCtrl.SetEditable(TRUE);
	for (int row = 0; row < m_station_count; row++)
	{
		int station=m_station_list[row];
		int b=0, s=0, w=0;
		for(int i=0; i<st_check.count; i++)
		{
			if(station==st_check.station_list[i].station)
			{
				b=st_check.station_list[i].setbcheck;
				s=st_check.station_list[i].setscheck;
				w=st_check.station_list[i].setwcheck;
				break;
			}
		}
		mGridCtrl.AddRow(station, b, s, w);
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CDlgSchdCompareStationSet::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str.Format("%s\\user_cfg\\CompareStationList-%d.xml", gpcdef.GetDataDir(),m_section);
	CStdioFile file;
	if(!file.Open(str, CFile::modeCreate | CFile::modeReadWrite))
	{
		MessageBox("保存失败,请确认data目录下是否存在目录user_cfg!", "错误");
		return;
	}
	file.WriteString("<root>");
	CString line;
	for(int row=1;row<mGridCtrl.GetRowCount();row++ )
	{
		int station = mGridCtrl.GetCell(row,0)->GetData();
		int b=0, s=0, w=0;
		CGridCellCheck *pCell = (CGridCellCheck*)mGridCtrl.GetCell(row,1);
		if(pCell && (pCell->GetCheck()==TRUE))
		{
			b=1;
		}
		pCell = (CGridCellCheck*)mGridCtrl.GetCell(row,2);
		if(pCell && (pCell->GetCheck()==TRUE))
		{
			s=1;
		}
		pCell = (CGridCellCheck*)mGridCtrl.GetCell(row,3);
		if(pCell && (pCell->GetCheck()==TRUE))
		{
			w=1;
		}

		line.Format("\n\t<station no=\"%d\" work=\"%d\" basic=\"%d\" shift=\"%d\" />", 
			station, w, b, s);

		file.WriteString(line);
	}
	file.WriteString("\n</root>");
	file.Close();

	CString str1;
	str1.Format("计划比较车站设置成功,存放在%s中", str);
	MessageBox(str1, "提示");
}

