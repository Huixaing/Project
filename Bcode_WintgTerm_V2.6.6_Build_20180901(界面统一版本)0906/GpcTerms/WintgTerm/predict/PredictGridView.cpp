#include "StdAfx.h"
#include ".\predictgridview.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

#define  GET_NODE_STRVALUE(dest,pXMLDoc,node) \
{MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); \
	if(pNode) \
{ \
	_variant_t v; \
	v = pNode->GetnodeTypedValue(); \
	dest = CString((v.bstrVal)); \
	SysFreeString(v.bstrVal); \
	v.Clear(); \
} \
			else dest.Empty();};

#define  GET_NODE_INTVALUE(dest,pXMLDoc,node) \
{MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); \
	if(pNode) \
{ \
	_variant_t v; \
	v = pNode->GetnodeTypedValue(); \
	dest = atoi(CString((v.bstrVal))); \
	SysFreeString(v.bstrVal); \
	v.Clear(); \
} \
			else dest=-1;};

CPredictGridView::CPredictGridView(void)
{
	m_fields_num=0;
	memset(m_fields, 0, sizeof(m_fields));
	m_bSingleLine=TRUE;
	m_bRowSelectable=FALSE;
}

CPredictGridView::~CPredictGridView(void)
{
}

IMPLEMENT_DYNAMIC(CPredictGridView, CGridCtrl)

BEGIN_MESSAGE_MAP(CPredictGridView, CGridCtrl)
	//{{AFX_MSG_MAP(CPredictGridView)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT_EX(GVN_BEGINLABELEDIT, OnBeginEdit)
	ON_NOTIFY_REFLECT_EX(GVN_ENDLABELEDIT, OnEndEdit)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, OnSelectChanged)
	//}}AFX_MSG_MAP

	ON_COMMAND_RANGE(WM_USER+1, WM_USER+3, OnPopMenuMsg)
END_MESSAGE_MAP()

void CPredictGridView::GetHeaderFromXML(CString xml)
{
	m_fields_num=0;
	memset(m_fields, 0, sizeof(m_fields));

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}

	if(VARIANT_TRUE != pXMLDoc->loadXML(_bstr_t( xml )))
	{
		return;
	}

	MSXML::IXMLDOMNodePtr pRoot = pXMLDoc->selectSingleNode( "FIELDS");
	if(pRoot == NULL)
	{
		return;
	}

	_variant_t v;
	MSXML::IXMLDOMNodePtr pNode, pChildNode;
	MSXML::IXMLDOMNodeListPtr pList = pRoot->selectNodes("FIELD");
	if(pList->Getlength() <= 0)
	{
		return;
	}
	for(int idx = 0; idx < pList->Getlength()&&idx<20; idx++)
	{
		pNode = pList->Getitem(idx);
		pChildNode = pNode->selectSingleNode( "@caption" );
		if(pChildNode)
		{
			v = pChildNode->GetnodeTypedValue();
			CString tmp = v.bstrVal;
			if(!tmp.IsEmpty())
			{
				strncpy(m_fields[m_fields_num].caption, tmp, 31);
			}
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode( "@attr_name" );
		if(pChildNode)
		{
			v = pChildNode->GetnodeTypedValue();
			CString tmp = v.bstrVal;
			if(!tmp.IsEmpty())
			{
				strncpy(m_fields[m_fields_num].attr_name, tmp, 31);
			}
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		m_fields_num++;
	}

	return;
}

BOOL CPredictGridView::CreateGrid(CString& xmlfield, CWnd *parent, const RECT &rect, UINT ctrlid)
{
	m_parent=parent;
	GetHeaderFromXML(xmlfield);

	if(m_fields_num<1)
		return FALSE;
	if(GetSafeHwnd())
		DestroyWindow(); 
	Create(rect,parent, ctrlid);
	try 
	{
		SetEditable(FALSE);
		EnableDragAndDrop(FALSE);
		SetRowCount(1);
		SetColumnCount(m_fields_num);
		SetFixedRowCount(1);
		//if(!m_bRowSelectable)
		//	SetFixedColumnCount(1);

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

	for(int col = 0; col < m_fields_num; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = col;
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE;
		Item.strText = m_fields[col].caption; 
		SetItem(&Item);
		SetItemBkColour(0,col,GetSysColor(COLOR_3DFACE));
	}
	SetSingleRowSelection(TRUE);

	return TRUE;
}

void  CPredictGridView::AutoWidth()
{
	CSize size;
    CDC* pDC = GetDC();
    if (!pDC)
        return;

	int nNumColumns = GetColumnCount();
    for (int nCol = 0; nCol < nNumColumns; nCol++)
    {
		if (nCol < 0 || nCol >= m_nCols)
			continue;

		//  Skip hidden columns when autosizing
		if( GetColumnWidth( nCol) <=0 )
			continue;

		m_arColWidths[nCol] = 78;
	}

    ResetScrollBars();
}

BOOL CPredictGridView::OnSelectChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	if(pv->iRow>=0 && pv->iColumn>=0)
	{
		CGridCell *pCell=(CGridCell *)GetCell(pv->iRow, 0);
		if(pCell==NULL)
			return TRUE;
		int train_index = pCell->GetData();
		pCell=(CGridCell *)GetCell(pv->iRow, 1);
		int rec_index = pCell->GetData();

		m_parent->SendMessage(10101, train_index, rec_index);
	}
	return TRUE;
}

BOOL CPredictGridView::OnBeginEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW *pItem=(NM_GRIDVIEW*) pNMHDR;
	BeginEditCell(pItem->iRow,pItem->iColumn);
	*pResult = 0;
	return FALSE;
}

BOOL CPredictGridView::OnEndEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW *pItem=(NM_GRIDVIEW*) pNMHDR;
	EndEditCell(pItem->iRow,pItem->iColumn);
	*pResult = 0;
	return FALSE;
}

void CPredictGridView::BeginEditCell(int row,int col)
{
}

void CPredictGridView::EndEditCell(int row,int col)
{
}

void CPredictGridView::SrcollToRow(UINT row)
{

}

void CPredictGridView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	return;
	CMenu menu;
	menu.CreatePopupMenu();
	
	menu.AppendMenu(
			MF_BYPOSITION|MF_STRING, 
			WM_USER+1, 
			"同意");

	menu.AppendMenu(MF_BYPOSITION|MF_SEPARATOR);

	menu.AppendMenu(
			MF_BYPOSITION|MF_STRING, 
			WM_USER+2, 
			"预告");

	menu.AppendMenu(
			MF_BYPOSITION|MF_STRING, 
			WM_USER+3, 
			"取消发车");
		
	
	menu.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		point.x, point.y, 
		this);
}

void CPredictGridView::OnPopMenuMsg(UINT nID)
{
}

int CPredictGridView::GetTrainDirectionByTrainId(CString strTrainId)
{
	// 去掉非数字
	CString strLastNumTrainId="0";
	int len = strTrainId.GetLength();
	for(int i= len-1;i>=0;i--)
	{
		char lastchar = strTrainId.GetAt(i);
		if(0x30 <= lastchar && lastchar <= 0x39)
		{
			strLastNumTrainId = lastchar;
			break;
		}
	}

	len = atoi(strLastNumTrainId);
	if(len%2 == 0)
		return 0;
	else
		return 1;
}

BOOL CPredictGridView::AddRowDataFromXML(CString& xml, char* rootnode)
{
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return FALSE;
	}

	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return FALSE;
	} 

	MSXML::IXMLDOMNodeListPtr pList = pXMLDoc->selectNodes(rootnode);
	int rec_count = pList->Getlength();
	for(int i = 0; i <rec_count; i++)
	{
		int col;
		int row=InsertRow("");
		if(row<0)
			break;

		int bHistory=0;
		GET_NODE_INTVALUE(bHistory, pList->Getitem(i), "HISTORY");
		int nSelect=0;
		GET_NODE_INTVALUE(nSelect, pList->Getitem(i), "SELECT");
		CString attr;
		for(col = 0; col < m_fields_num; col++)
		{ 
			GV_ITEM Item;

			Item.lfFont.lfWeight = FW_HEAVY;
			Item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
			Item.row = row;
			Item.col = col;
			Item.lParam=0;
			
			CString value="NA";
			attr.Format("%s", m_fields[col].attr_name);

			GET_NODE_STRVALUE(value, pList->Getitem(i), attr);

			if(attr=="PLAN_ARRIVE" || attr=="ARRIVE" || attr== "NEIB_YUGAO_TIME" || attr=="TONGYI_NEIB_TIME" || attr=="PLAN_DEPART" || attr=="DEPART" || attr=="YUGAO_NEIB_TIME" || attr=="NEIB_TONGYI_TIME")
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE;
			else if(attr=="ARRIVE_TRAINID" || attr=="DEPART_TRAINID")
			{
				if(GetTrainDirectionByTrainId(value)==0)
					Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE;
				else
					Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
			}
			else
				Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE;
			
			Item.strText = value;
			
			SetItem(&Item);

			if(1==bHistory)
			{
				SetItemBkColour(row,col, RGB(192,192,192));
			}
			if(1==nSelect)
			{
				SetItemBkColour(row,col, RGB(0,255,0));
			}
			CString colorstr="";
			if(attr=="ARRIVE_TRAINID" || attr=="DEPART_TRAINID")
			{
				GET_NODE_STRVALUE(colorstr,pList->Getitem(i),"TRAINID_COLOR");
			}
			else if(attr=="ARRIVE")
			{
				GET_NODE_STRVALUE(colorstr,pList->Getitem(i),"ARRIVE_COLOR");
			}
			else if(attr=="DEPART" )
			{
				GET_NODE_STRVALUE(colorstr,pList->Getitem(i),"DEPART_COLOR");
			}

			if(colorstr == "RED")
				SetItemFgColour(row,col,RGB(255,0,0));
			else if(colorstr == "BLUE")
				SetItemFgColour(row,col,RGB(0,0,255));
			else if(colorstr == "BLACK")
				SetItemFgColour(row,col,RGB(0,0,0));

			colorstr="";
			if(attr=="TONGYI_NEIB_TIME")
			{
				GET_NODE_STRVALUE(colorstr,pList->Getitem(i),"TONGYI_NEIB_BK_COLOR");
			}
			if(attr=="NEIB_TONGYI_TIME")
			{
				GET_NODE_STRVALUE(colorstr,pList->Getitem(i),"NEIB_TONGYI_BK_COLOR");
			}

			if(colorstr == "RED")
				SetItemBkColour(row,col,RGB(255,0,0));
			else if(colorstr == "BLUE")
				SetItemBkColour(row,col,RGB(0,0,255));
		}
        
		long itemdata=-1;
		GET_NODE_INTVALUE(itemdata, pList->Getitem(i),"TRAIN_INDEX");
		if(itemdata>=0)
			GetCell(row,0)->SetData(itemdata);
		GET_NODE_INTVALUE(itemdata, pList->Getitem(i),"REC_INDEX");
		if(itemdata>=0)
			GetCell(row,1)->SetData(itemdata);
	}
	
	return TRUE;
}