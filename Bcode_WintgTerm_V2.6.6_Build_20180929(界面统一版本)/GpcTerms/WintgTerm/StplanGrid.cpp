#include "StdAfx.h"
#include <algorithm>
#include "tg_function_config.h"
#include ".\StplanGrid.h"

CStplanGrid::CStplanGrid(void)
{
	m_bSingleLine=TRUE;
}

CStplanGrid::~CStplanGrid(void)
{
}

IMPLEMENT_DYNAMIC(CStplanGrid, CGridCtrl)

BEGIN_MESSAGE_MAP(CStplanGrid, CGridCtrl)
END_MESSAGE_MAP()

BOOL CStplanGrid::CreateGrid(CWnd *parent, const RECT &rect, UINT ctrlid)
{
	m_parent=parent;
	
	if(GetSafeHwnd())
		DestroyWindow(); 
	Create(rect,parent, ctrlid);
	try 
	{
		SetEditable(FALSE);
		EnableDragAndDrop(FALSE);
		SetRowCount(1);
		SetColumnCount(1);
		SetFixedRowCount(1);
		SetFixedColumnCount(0);

		SetFixedTextColor(RGB(0,0,0));
		SetGridLineColor(RGB(166,202,240));
		SetFixedBkColor(GetSysColor(COLOR_3DFACE));
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	GV_ITEM Item;
	Item.mask = GVIF_TEXT|GVIF_FORMAT;
	Item.row = 0;
	Item.col = 0;
	Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE;
	Item.strText = "阶段计划报警底色(橙色:一级 黄色:二级 白色:其它)"; 
	SetItem(&Item);
	SetItemBkColour(0,0,GetSysColor(COLOR_3DFACE));
	
	SetSingleRowSelection(TRUE);
	WarnIdLeval cfg;
	GetTGFunctionConfig()->GetWarnIdLeval(1, cfg);
	m_class1.clr=cfg.clr;
	CString includes, tmp;
	for(int i=0; i<cfg.id_count; i++)
	{
		m_class1.id_range.push_back(cfg.id[i]);
		if(i==0)
			tmp.Format("%d", cfg.id[i]);
		else
			tmp.Format(",%d", cfg.id[i]);
		includes+=tmp;
	}
	GpcLogSysMessage(0, "[%s]=%s", cfg.text, includes);

	GetTGFunctionConfig()->GetWarnIdLeval(2, cfg);
	m_class2.clr=cfg.clr;
	includes="";
	for(int i=0; i<cfg.id_count; i++)
	{
		m_class2.id_range.push_back(cfg.id[i]);
		if(i==0)
			tmp.Format("%d", cfg.id[i]);
		else
			tmp.Format(",%d", cfg.id[i]);
		includes+=tmp;
	}
	GpcLogSysMessage(0, "[%s]=%s", cfg.text, includes);

	GetTGFunctionConfig()->GetWarnIdLeval(3, cfg);
	m_class.clr=cfg.clr;
	includes="";
	for(int i=0; i<cfg.id_count; i++)
	{
		m_class.id_range.push_back(cfg.id[i]);
		if(i==0)
			tmp.Format("%d", cfg.id[i]);
		else
			tmp.Format(",%d", cfg.id[i]);
		includes+=tmp;
	}
	GpcLogSysMessage(0, "[%s]=%s", cfg.text, includes);

	return TRUE;
}

bool  CStplanGrid::HaveStplanWarnInfo()
{
	if(m_class1.list.GetCount()>0)
		return true;
	if(m_class2.list.GetCount()>0)
		return true;
	if(m_class.list.GetCount()>0)
		return true;
	return false;
}

void  CStplanGrid::ClearView()
{
	m_class1.list.RemoveAll();
	m_class2.list.RemoveAll();
	m_class.list.RemoveAll();

	DeleteNonFixedRows();
	Refresh();
}

void  CStplanGrid::DisplayWarnInfo()
{
	DeleteNonFixedRows();

	for(int i=0; i<m_class1.list.GetCount(); i++)
	{
		int row=InsertRow("");
		if(row<0)
			break;
		GV_ITEM Item;

		Item.lfFont.lfWeight = FW_HEAVY;
		Item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
		Item.row = row;
		Item.col = 0;
		Item.lParam=0;
		
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE;
		Item.strText = m_class1.list.GetAt(i);
				
		SetItem(&Item);
		GetCell(row,0)->SetBackClr(m_class1.clr);
	}
	for(int i=0; i<m_class2.list.GetCount(); i++)
	{
		int row=InsertRow("");
		if(row<0)
			break;
		GV_ITEM Item;

		Item.lfFont.lfWeight = FW_HEAVY;
		Item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
		Item.row = row;
		Item.col = 0;
		Item.lParam=0;
		
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE;
		Item.strText = m_class2.list.GetAt(i);
				
		SetItem(&Item);
		GetCell(row,0)->SetBackClr(m_class2.clr);
	}
	for(int i=0; i<m_class.list.GetCount(); i++)
	{
		int row=InsertRow("");
		if(row<0)
			break;
		GV_ITEM Item;

		Item.lfFont.lfWeight = FW_HEAVY;
		Item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
		Item.row = row;
		Item.col = 0;
		Item.lParam=0;
		
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE;
		Item.strText = m_class.list.GetAt(i);
				
		SetItem(&Item);
		GetCell(row,0)->SetBackClr(m_class.clr);
	}
	AutoSizeColumns();
	Refresh();
}
    
void CStplanGrid::AddWarnInfo(int nId, CString& text)
{
	if(m_class1.id_range.end()!=std::find(m_class1.id_range.begin(), m_class1.id_range.end(), nId))
	{
		GpcLogTrainMessage(0,0,"下计划时检查出不合理计划:[%d]一级报警:%s", nId, text);
		m_class1.list.Add(text);
		return;
	}
	if(m_class2.id_range.end() != std::find(m_class2.id_range.begin(), m_class2.id_range.end(), nId))
	{
		GpcLogTrainMessage(0,0,"下计划时检查出不合理计划:[%d]二级报警:%s", nId, text);
		m_class2.list.Add(text);
		return;
	}
	
	GpcLogTrainMessage(0,0,"下计划时检查出不合理计划:[%d]其它报警:%s", nId, text);
	m_class.list.Add(text);
	return;
}