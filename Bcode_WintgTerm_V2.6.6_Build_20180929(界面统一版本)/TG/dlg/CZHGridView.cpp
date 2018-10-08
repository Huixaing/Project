#include "StdAfx.h"
#include ".\czhgridview.h"

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

CCZHGridView::CCZHGridView(void)
{
	m_fields_num=0;
	memset(m_fields, 0, sizeof(m_fields));
	m_bSingleLine=TRUE;
	m_bRowSelectable=FALSE;
}

CCZHGridView::~CCZHGridView(void)
{
}

IMPLEMENT_DYNAMIC(CCZHGridView, CGridCtrl)

BEGIN_MESSAGE_MAP(CCZHGridView, CGridCtrl)
	//{{AFX_MSG_MAP(CCZHGridView)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT_EX(GVN_BEGINLABELEDIT, OnBeginEdit)
	ON_NOTIFY_REFLECT_EX(GVN_ENDLABELEDIT, OnEndEdit)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, OnSelectChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCZHGridView::GetHeaderFromXML(CString xml)
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
			CString tmp = CString(v.bstrVal);
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
			CString tmp(v.bstrVal);
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

BOOL CCZHGridView::CreateGrid(CString& xmlfield, CWnd *parent, const RECT &rect, UINT ctrlid)
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
		if(!m_bRowSelectable)
			SetFixedColumnCount(1);

		SetFixedTextColor(RGB(0,0,255));
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

	AutoSizeColumns();

	return TRUE;
}

void CCZHGridView::AutoWidth()
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

		if(nCol==0)
			m_arColWidths[nCol] = 40;
		else if(nCol==1)
			m_arColWidths[nCol] = 80;
		else if(nCol==2||nCol==3)
			m_arColWidths[nCol] = 160;
		else
			m_arColWidths[nCol] = 140;
	}

    ResetScrollBars();
}

BOOL CCZHGridView::OnSelectChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	if(pv->iRow>=0 && pv->iColumn>=0)
	{
		CGridCell *pCell=(CGridCell *)GetCell(pv->iRow, 1);
		if(pCell==NULL)
			return TRUE;
		ULONG trainindex=pCell->GetData();
		CString trainid=pCell->GetText();

		memset(seltrainid, 0, 20);
		if(pv->iRow==0)
		{
			trainindex=0;
		}
		else
		{
			strncpy(seltrainid, trainid, 19);
			seltrainid[19]=0;
		}
		m_parent->SendMessage(10101, (WPARAM)seltrainid, trainindex);
	}
	return TRUE;
}

BOOL CCZHGridView::OnBeginEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW *pItem=(NM_GRIDVIEW*) pNMHDR;
	BeginEditCell(pItem->iRow,pItem->iColumn);
	*pResult = 0;
	return FALSE;
}

BOOL CCZHGridView::OnEndEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW *pItem=(NM_GRIDVIEW*) pNMHDR;
	EndEditCell(pItem->iRow,pItem->iColumn);
	*pResult = 0;
	return FALSE;
}

void CCZHGridView::BeginEditCell(int row,int col)
{
}

void CCZHGridView::EndEditCell(int row,int col)
{
}

void CCZHGridView::SrcollToRow(UINT row)
{

}

BOOL CCZHGridView::AddRowDataFromXML(CString& xml, char* rootnode)
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
			
			if(col==0)
				value.Format("%d", i+1);
			else
			{
				attr = m_fields[col].attr_name;
				GET_NODE_STRVALUE(value, pList->Getitem(i), attr);
			}

			Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE;
			Item.strText = value;
			
			SetItem(&Item);
		}
		
		CString value="";
		GET_NODE_STRVALUE(value, pList->Getitem(i), "TRAIN_INDEX");
		SetItemData(row, 1, atol(value));
	}
	
	Refresh();
	return TRUE;
}