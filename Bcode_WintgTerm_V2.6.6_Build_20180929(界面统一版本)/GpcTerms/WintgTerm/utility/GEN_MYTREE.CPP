// MyTree.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Gen_MyTree.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyGenTree
#define  EDITBARHEIGHT 23
#define  EDITBARWIDTH  220 
CMyGenTree::CMyGenTree()
{
	m_cznum=0;
}

CMyGenTree::~CMyGenTree()
{
}

BEGIN_MESSAGE_MAP(CMyGenTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CMyGenTree)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyGenTree message handlers
static int  ShortCmp(short s[],short d[],int num)
{
	for(int i=0;i<num;i++)
	{	  
		if(s[i]>d[i]) return 1;
		else
			if(s[i]<d[i]) return -1;
	}
	return 0;
}

int  CMyGenTree::GetParent(int son)
{
	for(int i=0;i<m_cznum;i++)
	{
		if(i==son) continue;
		if(m_czs[i].indexnum<=0) continue;
		if((m_czs[i].indexnum+1)!=m_czs[son].indexnum) continue;
		if(ShortCmp(m_czs[i].index,m_czs[son].index,m_czs[i].indexnum))
			continue;

		return i;
	}
	return -1;
}

HTREEITEM  CMyGenTree::InsertTreeItem(int i,HTREEITEM parent,HTREEITEM after) 
{
	TVINSERTSTRUCT tvstru;
	tvstru.hInsertAfter=after;
	tvstru.hParent=parent;
	tvstru.item.mask=TVIF_IMAGE|TVIF_TEXT|TVIF_PARAM|TVIF_STATE;
	tvstru.item.pszText=m_czs[i].name;
	tvstru.item.iImage=0;
	tvstru.item.lParam=i;
	tvstru.item.state=INDEXTOSTATEIMAGEMASK(0);
	tvstru.item.stateMask=TVIS_STATEIMAGEMASK;
	return InsertItem(&tvstru); 			 
}

void CMyGenTree::ExpandAll(HTREEITEM hItem)
{
	if ( hItem == NULL )
		return;
	Expand(hItem, TVE_EXPAND);
	HTREEITEM hChildItem = GetChildItem(hItem);
	while ( hChildItem != NULL )
	{
		ExpandAll(hChildItem);
		hChildItem = GetNextSiblingItem(hChildItem);
	}
}
int CMyGenTree::GetIndexByValue(const char *value)
{
	int i;
	for(i=0;i<m_cznum;i++)
	{
		if(m_czs[i].indexnum<=0)
			continue;
		if(!stricmp(m_czs[i].value,value))
			return i;
	}
	return -1;
}

int CMyGenTree::Insert_1_Item(const char *parentvalue, const char *name, const char *value,int rela_value)
{
	int parent=-1;
	parent=GetIndexByValue(parentvalue);
	if(parent<0)
		return -1;
	return Insert_1_Item(parent,name,value,rela_value);
}

int CMyGenTree::Insert_1_Item(int parent, const char *name, const char *value,int rela_value)
{
	int id;
	id=GetAvaIndex();
	if(id<0) return id;
	strncpy(m_czs[id].name,name,TREENAMEMAXLEN-1);
	m_czs[id].name[TREENAMEMAXLEN-1]=0;
	strncpy(m_czs[id].value,value,TREEVALUEMAXLEN-1);
	m_czs[id].value[TREEVALUEMAXLEN-1]=0;

	m_czs[id].rela_value=rela_value;
	if(parent>=0)
	{
		for(int i=0;i<m_czs[parent].indexnum;i++)
		{
			m_czs[id].index[i]=m_czs[parent].index[i];
		}
		m_czs[id].index[i]=GetAvaSonIndex(parent);
		m_czs[id].indexnum=i+1;
		m_czs[id].handle=InsertTreeItem(id,m_czs[parent].handle,TVI_LAST);
	}
	else // insert the root
	{
		m_czs[id].index[0]=GetAvaSonIndex(parent);
		m_czs[id].indexnum=1;
		m_czs[id].handle=InsertTreeItem(id,TVI_ROOT,TVI_LAST); 	
	}
	return id;
}

void CMyGenTree::Startup()
{
	SetItemHeight(20);
}

int CMyGenTree::GetAvaIndex()
{
	for(int i=0;i<m_cznum;i++)
	{
		if(m_czs[i].indexnum<=0)
			return i;
	}
	if(m_cznum>=TREEITEMNUM) 
		return -1;
	m_cznum++;
	return m_cznum-1;
}

int CMyGenTree::GetAvaSonIndex(int parent)
{ 
	int retva=0;
	for(int i=0;i<m_cznum;i++)
	{
		if(m_czs[i].indexnum<=0) continue;
		if(parent>=0)
		{
			if(i==parent) continue;
			if(m_czs[i].indexnum<=m_czs[parent].indexnum) 
				continue;
			if(ShortCmp(m_czs[i].index,m_czs[parent].index,m_czs[parent].indexnum))
				continue;
			if(retva==m_czs[i].index[m_czs[parent].indexnum])
				retva++;  
		}
		else
		{
			if(retva==m_czs[i].index[0])
				retva++;
		}
	}
	return retva;
}

void CMyGenTree::OnRButtonDown(UINT nFlags, CPoint point) 
{
	HTREEITEM hItem;
	UINT    result;
	hItem=HitTest(point,&result);
	SelectItem(hItem);
	CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CMyGenTree::OnLButtonDown(UINT nFlags, CPoint point) 
{
	HTREEITEM hItem;
	UINT    result;
	CTreeCtrl::OnLButtonDown(nFlags, point);
	hItem=HitTest(point,&result);
	SelectItem(hItem);
}
void CMyGenTree::Reset()
{
	m_cznum=0;
	DeleteAllItems();
}

CString CMyGenTree::GetSelectedValue()
{
	HTREEITEM handle;
	int i;
	handle=GetSelectedItem();
	if(handle==NULL) 
		return "";
	i=GetItemData(handle);
	return m_czs[i].value;
}

int  CMyGenTree::GetSelectedRelaValue()
{
	HTREEITEM handle;
	int i;
	handle=GetSelectedItem();
	if(handle==NULL) 
		return -1;
	i=GetItemData(handle);
	return m_czs[i].rela_value;
}

const char *CMyGenTree::GetIDStr(int id)
{
	static char tmp[1024];
	strcpy(tmp,"");
	for(int i=0;i<m_czs[id].indexnum;i++)
	{
		char t[12];
		sprintf(t,"%02d",m_czs[id].index[i]);
		strcat(tmp,t);
	}
	return tmp;
}

short CMyGenTree::SelectAndExpandObjectByValue(LPCTSTR value)
{
	for(int i=0;i<m_cznum;i++)
	{
		if(!stricmp(m_czs[i].value,value))
		{
			SelectItem(m_czs[i].handle);
			Expand( m_czs[i].handle,TVE_EXPAND);	
			return 1;
		}
	}
	return 0;
}
