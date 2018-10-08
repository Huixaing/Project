#include "stdafx.h"
#include "neibtraingrid.h"
#include "tgpspdata.h"
#include "GridCtrl_src\GridCellCheck.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

PropertyInfoPara g_opms_neib_train_list_def[]=
{
	{0,0,CTC_DATA_STR,	"OPER",			"接受邻台结果"},
	{0,0,CTC_DATA_STR,	"GTID",			"全局ID"},
	{0,0,CTC_DATA_STR,	"TNO",			"车次"},
	{0,0,CTC_DATA_STR,	"STATION",		"车站"},
	{0,0,CTC_DATA_STR,	"DATE",			"接入时间"},
	{0,0,CTC_DATA_STR,	"SOURCE",		"列车来源"},
	{0,0,CTC_DATA_STR,	"RELATION",		"交路处理"},
	{0,0,CTC_DATA_STR,	"TIME",			"到开时刻"},
	{0,0,CTC_DATA_STR,	"STOP_PASS",	"到开关系"},
	{0,0,CTC_DATA_STR,	"WORK_SIDE",	"股道"},
	{0,0,CTC_DATA_STR,	"ENGINE",		"机车"},
	{0,0,CTC_DATA_STR,	"WORK_TM",		"作业时间"},
	{0,0,CTC_DATA_STR,	"ARRIVE_FLAG",	"接入标志"},
	{0,0,CTC_DATA_STR,	"DEPART_FLAG",	"交出标志"},
	{0,0,CTC_DATA_STR,	"STATION_LIST",	"经由车站"},
	{0,500,CTC_DATA_STR,	"PATH",			"径路"},
};

int count_of_opms_neib_train_list_def()
{
	return sizeof(g_opms_neib_train_list_def)/sizeof(g_opms_neib_train_list_def[0]);
}

BOOL CNeibTrainGridView::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	int fieldcnt=count_of_opms_neib_train_list_def();
	if(fieldcnt>MAX_FIELD_IN_GRID)
		return FALSE;
	m_nFieldNum=fieldcnt;
	for(int i=0;i<m_nFieldNum;i++)
		m_FiledInfo[i]=g_opms_neib_train_list_def[i];
	m_bVertical=FALSE;

	if(GetSafeHwnd())
		DestroyWindow(); 
	Create(rect,parent, ctrlid);
	try 
	{
		SetEditable(FALSE);
		EnableDragAndDrop(FALSE);
		SetRowCount(1);
		SetColumnCount(fieldcnt);
		SetFixedRowCount(1);
		if(!m_bRowSelectable)
			SetFixedColumnCount(1);

		SetFixedTextColor(RGB(0,0,255));
		SetGridLineColor(RGB(166,202,240));       //线的颜色
		SetFixedBkColor(GetSysColor(COLOR_3DFACE));

	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	for(int col = 0; col < fieldcnt; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = col;
		Item.nFormat = DT_CENTER|DT_WORDBREAK|DT_NOPREFIX;
		Item.strText = m_FiledInfo[col].caption; 
		SetItem(&Item);
		SetItemBkColour(0,col,GetSysColor(COLOR_3DFACE));
	}
	SetSingleRowSelection(TRUE);
	if(m_bRowSelectable)
		SetCellType(0,0,RUNTIME_CLASS(CGridCellCheck));
	SetCellType(0,6,RUNTIME_CLASS(CGridCellCheck));
	AutoSizeColumns();

	return TRUE;
}

bool CNeibTrainGridView::UpdateGridInfo(int row, CString& xml)
{
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}

	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return false;
	} 

	MSXML::IXMLDOMNodePtr pRoot = pXMLDoc->selectSingleNode("ROOT");
	if(pRoot==NULL)
		return false;

	for(int i=0;i<m_nFieldNum;i++)
	{
		CString strText="无变化";
		int clr=0;
		MSXML::IXMLDOMNodePtr pNode = pRoot->selectSingleNode(m_FiledInfo[i].attr_name);
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			strText = (v.bstrVal);
			SysFreeString(v.bstrVal);
            v.Clear();
		}
		if(pNode)
		{
			MSXML::IXMLDOMNodePtr pAttriNode = pNode->selectSingleNode("@clr");
			if(pAttriNode)
			{
				_variant_t v;
				v = pAttriNode->GetnodeTypedValue();
				clr = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
		CString strOper=m_FiledInfo[i].attr_name;
		if("OPER" == strOper)
		{
			if("修改"==strText)
				GetCell(row,0)->SetBackClr(RGB(181,153,79)); 
			else if("新增"==strText)
				GetCell(row,0)->SetBackClr(RGB(181,153,79));
			else
				GetCell(row,0)->SetBackClr(RGB(255,255,255)); 
		}
		if("RELATION" == strOper)
		{
			strText="平移接续车次";
			SetCellType(row ,i, RUNTIME_CLASS(CGridCellCheck));
			CGridCellCheck * pCell = (CGridCellCheck *) GetCell(row, i);
			if(pCell)
			{
				pCell->SetCheck(0);
			}
		}
		if(clr==1)
		{
			GetCell(row,i)->SetBackClr(RGB(29,135,231)); 
		}
		if(clr==2)
		{
			GetCell(row,i)->SetBackClr(RGB(255,0,0)); 
		}
		 
		GetCell(row,i)->SetText(strText);
	}

	return true;
}

int CNeibTrainGridView::InsertTrainInfo(CString& cap, CString& xml)
{
	int row=CXMLDataGrid::Insert1EmptyRow(cap);
	if(row<0)
		return -1;
	if(UpdateGridInfo(row, xml))
		return row;
	return -1;
}

int CNeibTrainGridView::UpdateTrainInfo(int row, CString& xml)
{
	if(row<0 || row>=this->GetRowCount())
		return -1;

	UpdateGridInfo(row, xml);
	return row;
}

void CNeibTrainGridView::OnColumnSelect(int row, int col)   // 全选功能
{
	if(row==0 && col==6)
	{
		CGridCellCheck * pCell = (CGridCellCheck *) GetCell(row,col);
		if(pCell==NULL)
			return;

		BOOL bIsRowSelected = pCell->GetCheck();
		if(bIsRowSelected)
			pCell->SetCheck(0);
		else
			pCell->SetCheck(1);
		bIsRowSelected = pCell->GetCheck();
		for(int row=1;row<GetRowCount();row++)
		{
			CGridCellCheck * pCell = (CGridCellCheck *) GetCell(row,col);
			if(pCell)
			{
				pCell->SetCheck(bIsRowSelected);
			}
		}
	}

	return;
}

BOOL  CNeibTrainGridView::IsColSetCheck(int row, int col)
{
	CGridCellCheck * pCell;
	pCell = (CGridCellCheck *) GetCell(row,col);
	if(pCell)
		return pCell->GetCheck();
	return FALSE;
}