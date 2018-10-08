#include "stdafx.h"
#include "xmlview.h"
#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"
#include "gridctrl_src/GridCellDateTime.h"
#include "xml_utility.h"
#import  <msxml.dll> named_guids
using namespace MSXML;

BEGIN_MESSAGE_MAP(CXMLDataGrid, CGridCtrl)
	//{{AFX_MSG_MAP(CXMLDataGrid)
	ON_NOTIFY_REFLECT_EX(GVN_BEGINLABELEDIT, OnBeginEdit)
	ON_NOTIFY_REFLECT_EX(GVN_ENDLABELEDIT, OnEndEdit)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, OnColumnSelectChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXMLDataGrid::CXMLDataGrid()
{
	m_bSingleLine=FALSE;
	m_bVertical=FALSE;
	m_bRowSelectable=FALSE;
}

CXMLDataGrid::~CXMLDataGrid()
{
}

BOOL  CXMLDataGrid::CreateGridBody(CWnd *parent,const RECT &rect,UINT ctrlid,const PropertyInfoPara fielddef[],int fieldcnt,int row)
{
	if(row<1)
		return FALSE;
	if(GetSafeHwnd())
		DestroyWindow(); 
	Create(rect,parent, ctrlid);
	try {
		SetEditable(FALSE);
		EnableDragAndDrop(FALSE);
		SetRowCount(row);
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
		Item.strText = fielddef[col].caption; 
		SetItem(&Item);
		SetItemBkColour(0,col,GetSysColor(COLOR_3DFACE));
	}
	SetSingleRowSelection(TRUE);
	if(m_bRowSelectable)
		SetCellType(0,0,RUNTIME_CLASS(CGridCellCheck));
	AutoSizeColumns();
	return TRUE;
}

BOOL  CXMLDataGrid::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid,const PropertyInfoPara fielddef[],int fieldcnt)
{
	if(fieldcnt>MAX_FIELD_IN_GRID)
		return FALSE;
	m_nFieldNum=fieldcnt;
	for(int i=0;i<m_nFieldNum;i++)
		m_FiledInfo[i]=fielddef[i];
	m_bVertical=FALSE;
	if(!CreateGridBody(parent,rect,ctrlid,fielddef,fieldcnt,1))
		return FALSE;
	return TRUE;
}

BOOL  CXMLDataGrid::CreateVerticalGrid(CWnd *parent,const RECT &rect,UINT ctrlid,const PropertyInfoPara fielddef[],int fieldcnt)
{
	if(fieldcnt>MAX_FIELD_IN_GRID || fieldcnt<1)
		return FALSE;
	m_nFieldNum=fieldcnt;
	for(int i=0;i<m_nFieldNum;i++)
		m_FiledInfo[i]=fielddef[i];
	m_bVertical=TRUE;
	m_bSingleLine=FALSE;
	if(GetSafeHwnd())
		DestroyWindow(); 
	Create(rect,parent, ctrlid);
	try {
		SetEditable(FALSE);
		EnableDragAndDrop(FALSE);
		//SetRowCount(m_nFieldNum);
		SetColumnCount(2);
		SetFixedRowCount(0);
		SetFixedColumnCount(1);
		//SetFixedTextColor(RGB(0,0,0));      //文本颜色
		//SetFixedBkColor(RGB(0,0,255));   //背景色
		SetFixedTextColor(RGB(0,0,255));
		SetGridLineColor(RGB(166,202,240));       //线的颜色
		SetFixedBkColor(GetSysColor(COLOR_3DFACE));
		//AutoSizeColumns();
		//SetEditable(true);
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	for(int i = 0; i < m_nFieldNum; i++)
	{ 
		int row=InsertRow(fielddef[i].caption);
		SetItemFormat(row,1,DT_LEFT|DT_VCENTER|DT_NOPREFIX);
		SetItemBkColour(row,0,GetSysColor(COLOR_3DFACE));
	}
	AutoSizeColumns();
	return TRUE;
}


BOOL  IsDigitalString(const char *s)
{
	static char digistr[]="0123456789+-.";
	if(s==NULL)
		return FALSE;
	while(*s)
	{
		if(!strchr(digistr,*s))
			return FALSE;
		s++;
	}
	return TRUE;
}

BOOL  CXMLDataGrid::InitVerticalGrid(const char *xml,const char *rootnode)
{
	if(!xml || !rootnode)
	{
		for (int col = 0; col < m_nFieldNum; col++)
		{ 
			GetCell(col,1)->SetText("");

		}
		Refresh();
		return TRUE;
	}
	LOAD_XML(xml,rootnode,pRoot);
	for (int col = 0; col < m_nFieldNum; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
		Item.row = col;
		Item.col = 1;
		if(m_bSingleLine)
			Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
		else
			Item.nFormat = DT_LEFT|DT_VCENTER|DT_NOPREFIX;
		Item.lParam=0;


		CString attr;
		CString value="NA";
		attr.Format("%s", m_FiledInfo[col].attr_name);
		GET_NODE_STRVALUE(Item.strText,pRoot,attr);
		if(m_FiledInfo[col].data_type!=CTC_DATA_STR && IsDigitalString(Item.strText))
		{
			const char *ptmpstr=GetCTCValueDispString(m_FiledInfo[col].data_type,atoi(Item.strText));
			if(ptmpstr)
				Item.strText =ptmpstr;
		}

		SetItem(&Item);
	}
	Refresh();
	return TRUE;
}

BOOL  CXMLDataGrid::InsertRows(const char *xml,const char *rootnode)
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

	MSXML::IXMLDOMNodeListPtr pRecAbsList = pXMLDoc->selectNodes(rootnode);//ds_MaintainPlan_List/
	int rec_count = pRecAbsList->Getlength();
	for(int i = 0; i <rec_count; i++)
	{
		int col;
		int row=InsertRow("");
		if(row<0)
			break;
		for ( col = 0; col < m_nFieldNum; col++)
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

			CString attr;
			CString value="NA";
			attr.Format("%s", m_FiledInfo[col].attr_name);

			GET_NODE_STRVALUE(Item.strText,pRecAbsList->Getitem(i),attr);
			if(m_FiledInfo[col].data_type!=CTC_DATA_STR && IsDigitalString(Item.strText))
			{
				const char *ptmpstr=GetCTCValueDispString(m_FiledInfo[col].data_type,atoi(Item.strText));
				if(ptmpstr)
					Item.strText =ptmpstr;
			}
			SetItem(&Item);
		}
		CString colorstr;
		GET_NODE_STRVALUE(colorstr,pRecAbsList->Getitem(i),"COLOR");
		if(!stricmp(colorstr,"RED"))
			SetItemBkColour(row,0,RGB(255,0,0));
		else
			if(!stricmp(colorstr,"GREEN"))
				SetItemBkColour(row,0,RGB(0,255,0));
			else
				if(!stricmp(colorstr,"BLUE"))
					SetItemBkColour(row,0,RGB(0,0,255));
				else
					if(!stricmp(colorstr,"YELLOW"))
						SetItemBkColour(row,0,RGB(255,255,0));
					else
						if(!stricmp(colorstr,"WHITE"))
							SetItemBkColour(row,0,RGB(255,255,255));
						else
							if(!stricmp(colorstr,"GRAY"))
								SetItemBkColour(row,0,RGB(192,192,192));
							else
								SetItemBkColour(row,0,RGB(192,192,0));

		long itemdata=-1;
		GET_NODE_INTVALUE(itemdata,pRecAbsList->Getitem(i),"ITEMDATA1");
		if(itemdata>=0)
			GetCell(row,1)->SetData(itemdata);
		if(m_bRowSelectable)
		{
			SetCellType(row,0,RUNTIME_CLASS(CGridCellCheck));
			int selected=-1;
			GET_NODE_INTVALUE(selected,pRecAbsList->Getitem(i),"SELECTED");
			if(selected>0)
				SetRowSelected(row,TRUE);
		}
	}
	Refresh();
	return TRUE;
}

BOOL CXMLDataGrid::OnColumnSelectChanged(NMHDR* pNMHDR, LRESULT* pResult)   // 全选功能
{
	NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	if(pv->iRow==0 && pv->iColumn==0)
	{
		if(IsRowSelected(0))
			SetAllRowSelected(FALSE); 
		else
			SetAllRowSelected(TRUE); 
	}

	return TRUE;
}

BOOL  CXMLDataGrid::OnBeginEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW *pItem=(NM_GRIDVIEW*) pNMHDR;
	BeginEditCell(pItem->iRow,pItem->iColumn);
	*pResult = 0;
	return FALSE;
}

BOOL  CXMLDataGrid::OnEndEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW *pItem=(NM_GRIDVIEW*) pNMHDR;
	EndEditCell(pItem->iRow,pItem->iColumn);
	*pResult = 0;
	return FALSE;
}

void  CXMLDataGrid::BeginEditCell(int row,int col)
{
	int field_index;
	if(m_bVertical)
		field_index=row;
	else
		field_index=col;
	if(field_index<0 || field_index>=m_nFieldNum)
		return ;
	if(m_FiledInfo[field_index].flag==3) //LIST
	{
		CGridCellCombo *pCell;
		CStringArray options;
		int value;
		for(int i=0;i<100;i++)
			options.Add( _T(GetCTCTypeChoiceString(m_FiledInfo[field_index].data_type,i,&value,m_FiledInfo[field_index].para)));

		SetCellType(row ,col, RUNTIME_CLASS(CGridCellCombo));
		pCell= (CGridCellCombo*)GetCell(row,col);
		pCell->SetOptions(options);
		pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWN);
	}
	else
		if(m_FiledInfo[field_index].flag==5) // date
		{
			CGridCellDateTime *pCell;
			SetCellType(row ,col, RUNTIME_CLASS(CGridCellDateTime));
			pCell= (CGridCellDateTime*)GetCell(row,col);
			pCell->Init(DTS_SHORTDATEFORMAT);
		}
		else
			if(m_FiledInfo[field_index].flag==4) // time
			{
				CGridCellDateTime *pCell;
				SetCellType(row ,col, RUNTIME_CLASS(CGridCellDateTime));
				pCell= (CGridCellDateTime*)GetCell(row,col);
				pCell->Init(DTS_TIMEFORMAT);
			}

}

void  CXMLDataGrid::EndEditCell(int row,int col)
{
	int field_index;
	if(m_bVertical)
		field_index=row;
	else
		field_index=col;

	SetCellType(row ,col,RUNTIME_CLASS(CGridCell));
	if(col>0)
	{
		CString headstr=GetCell(row,0)->GetText();
		/*
		if(headstr.GetAt(0)!='*')
		headstr.SetAt(0,'*');
		GetCell(row,0)->SetText(headstr);*/
		//GetCell(row,0)->SetData(1);  // changed! HE YONGXIANG 此处若不屏蔽，将会将车次序列号改为1，当第二次点击单元格时，程序会有问题
		GetCell(row,0)->SetTextClr(RGB(255,0,0));
		//GetCell(row,col)->SetData(1);
		//GetCell(row,col)->SetTextClr(RGB(255,0,0));
	}
}
void  CXMLDataGrid::SetFiledPara(long para,int index)
{
	for(int i=0;i<m_nFieldNum;i++)
	{
		if(index<0 || i==index)
			m_FiledInfo[i].para=para;
	}
}

void  CXMLDataGrid::SetRowSelectable(BOOL flag)
{
	m_bRowSelectable=flag;
	if(m_bRowSelectable)
	{
	}
}

int  CXMLDataGrid::Insert1EmptyRow(const char *headstr,int row)
{
	int newrow=InsertRow(headstr,row);
	for(int i=1;i<m_nFieldNum;i++)
	{
		//if(m_FiledInfo[i].data_type==CTC_DATA_STR)
		if(m_bSingleLine)
			SetItemFormat(newrow,i,DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
		else
			SetItemFormat(newrow,i,DT_LEFT|DT_VCENTER|DT_NOPREFIX);
	}
	if(m_bRowSelectable)
		SetCellType(newrow,0,RUNTIME_CLASS(CGridCellCheck));
	return newrow;
}

void  CXMLDataGrid::SaveVerticalToXML(CString &dest,const char *subnode)
{
	if(subnode)
	{
		dest+="<";
		dest+=subnode;
		dest+=">";
	}
	for(int j=0;j<m_nFieldNum;j++)
	{
		CString itemvalue=GetCell(j,1)->GetText();
		if(itemvalue.IsEmpty())
			continue;

		dest+="<";
		dest+=m_FiledInfo[j].attr_name;
		dest+=">";
		dest+=itemvalue;
		dest+="</";
		dest+=m_FiledInfo[j].attr_name;
		dest+=">";

	}
	if(subnode)
	{
		dest+="</";
		dest+=subnode;
		dest+=">";
	}
}

void  CXMLDataGrid::SaveRowToXML(CString &dest,int row,const char *subnode)
{
	if(subnode)
	{
		dest+="<";
		dest+=subnode;
		dest+=">";
	}
	for(int j=0;j<m_nFieldNum;j++)
	{
		CString itemvalue=GetCell(row,j)->GetText();
		if(itemvalue.IsEmpty())
			continue;

		dest+="<";
		dest+=m_FiledInfo[j].attr_name;
		dest+=">";
		dest+=itemvalue;
		dest+="</";
		dest+=m_FiledInfo[j].attr_name;
		dest+=">";

	}
	if(subnode)
	{
		dest+="</";
		dest+=subnode;
		dest+=">";
	}
}
// head 不为NULL则初始化dest,rootnode不为NULL则增加大节点
void  CXMLDataGrid::SaveToXML(CString &dest,const char *head,const char *rootnode) 
{
	CStringArray nodearray;
	CString tmp;
	if(rootnode)
		tmp=rootnode;
	if(head)
		dest=head;
	while(1)
	{
		int x=tmp.Find('/',0);
		if(x<0)
			break;
		nodearray.Add(tmp.Left(x));
		x++;
		tmp=tmp.Mid(x);
	}
	CString subnode=tmp;
	int i;
	for(i=0;i<nodearray.GetCount();i++)
	{
		dest+="<";
		dest+=nodearray.ElementAt(i);
		dest+=">";
	}
	if(m_bVertical)
	{
		if(subnode.IsEmpty())
			SaveVerticalToXML(dest,NULL);
		else
			SaveVerticalToXML(dest,subnode);
	}
	else
	{
		for(i=1;i<GetRowCount();i++) // skip caption!
		{
			if(GetCell(i,0)->GetData()==0) // no changed
				continue;
			if(subnode.IsEmpty())
				SaveRowToXML(dest,i,NULL);
			else
				SaveRowToXML(dest,i,subnode);
		}
	}

	for(i=nodearray.GetCount()-1;i>=0;i--)
	{
		dest+="</";
		dest+=nodearray.ElementAt(i);
		dest+=">";
	}

}

BOOL CXMLDataGrid::GetFieldString(int row,const char *attr_name,CString &dest)
{
	if(row<1 || row>=GetRowCount())
		return FALSE;
	if(GetColumnCount()!=m_nFieldNum) // vertical grid?
		return FALSE;
	for(int j=0;j<m_nFieldNum;j++)
	{
		if(!stricmp(m_FiledInfo[j].attr_name,attr_name))
		{
			dest=GetCell(row,j)->GetText();
			return TRUE;
		}
	}
	return NULL;
}

BOOL  CXMLDataGrid::IsRowSelected(int row)
{
	if(!m_bRowSelectable)
		return FALSE;
	CGridCellCheck * pCell;
	pCell = (CGridCellCheck *) GetCell(row,0);
	if(pCell)
		return pCell->GetCheck();
	return FALSE;
}
void CXMLDataGrid::SetRowSelected(int row,BOOL va)
{
	if(!m_bRowSelectable)
		return;
	CGridCellCheck * pCell;
	pCell = (CGridCellCheck *) GetCell(row,0);
	if(pCell)
	{
		pCell->SetCheck(va);
	}

}

void CXMLDataGrid::SetAllRowSelected(BOOL va)
{
	if(!m_bRowSelectable)
		return;
	for(int row=0;row<GetRowCount();row++)
	{
		CGridCellCheck * pCell;
		pCell = (CGridCellCheck *) GetCell(row,0);
		if(pCell)
		{
			pCell->SetCheck(va);
		}
	}
}

int   CXMLDataGrid::GetColumnByID(int id)
{
	for(int i=0;i<m_nFieldNum;i++)
	{
		if(m_FiledInfo[i].id==id)
			return i;
	}
	return -1;
}