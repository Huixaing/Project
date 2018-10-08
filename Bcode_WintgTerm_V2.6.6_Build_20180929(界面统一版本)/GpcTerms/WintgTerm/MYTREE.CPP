// MyTree.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MyTree.h"
#include "inifile.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyTree
#define  EDITBARHEIGHT 23
#define  EDITBARWIDTH  220 
CMyTree::CMyTree()
{
	m_cznum=0;
	m_flag=0;
}

CMyTree::~CMyTree()
{
}


BEGIN_MESSAGE_MAP(CMyTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CMyTree)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP

	ON_COMMAND(ID_TREE_DELETE,OnItemDelete)
	ON_COMMAND(ID_TREE_INSERT,OnItemInsert)
	ON_COMMAND(ID_TREE_EDIT,OnItemEdit)
	ON_COMMAND(ID_SET_ENABLE,OnEnable)
	ON_COMMAND(ID_SET_DISABLE,OnDisable)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyTree message handlers
#define DISPLEN 20
static char *GetDispStr(const char *name,const char *value)
{
	static char tmp[256];
	/*
	memset(tmp,' ',DISPLEN);
	tmp[DISPLEN]=0;
	tmp[DISPLEN+1]=0;
	memcpy(tmp,name,strlen(name));
	memcpy(tmp+DISPLEN-strlen(value),value,strlen(value));*/
	//if(strcmp(value,""))
	 // sprintf(tmp,"%s (%s)",name,value);
	//else
	//sprintf(tmp,"%s",name,value);
	strncpy(tmp,name,sizeof(tmp));
	return tmp;
}

static char *GetEditStr(const char *name,const char *value)
{
	static char tmp[256];
	sprintf(tmp,"%s   %s",name,value);
	return tmp;
}

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

int  CMyTree::GetParent(int son)
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

HTREEITEM  CMyTree::InsertTreeItem(int i,HTREEITEM parent,HTREEITEM after) 
{
	TVINSERTSTRUCT tvstru;
	tvstru.hInsertAfter=after;
	tvstru.hParent=parent;
	tvstru.item.mask=TVIF_IMAGE|TVIF_TEXT|TVIF_PARAM|TVIF_STATE;
    tvstru.item.pszText=GetDispStr(m_czs[i].name,m_czs[i].value);
	tvstru.item.iImage=0;
	tvstru.item.lParam=i;
	tvstru.item.state=INDEXTOSTATEIMAGEMASK(1);
	tvstru.item.stateMask=TVIS_STATEIMAGEMASK;
	return InsertItem(&tvstru); 			 
}

void CMyTree::SetTreeItems()
{
	DeleteAllItems();
	HTREEITEM parenthd;
	for(int num=1;num<=5;num++)
	{
	 for(int i=0;i<m_cznum;i++)
		{
		   if(m_czs[i].indexnum!=num) continue;
		   if(num==1)
			   parenthd=TVI_ROOT;
		   else
		   {
			   int pr=GetParent(i);
			   if(pr<0) continue;
			   if(m_czs[pr].handle==0) continue;
			   parenthd=m_czs[pr].handle; 			  
			   if(m_czs[i].index[m_czs[i].indexnum-1]>m_czs[pr].maxsonindex)
				   m_czs[pr].maxsonindex=m_czs[i].index[m_czs[i].indexnum-1];	   
		   }
		   m_czs[i].handle=InsertTreeItem(i,parenthd,TVI_LAST); 	
		}
	}
}

void CMyTree::ExpandAll(HTREEITEM hItem)
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
int  atoix(const char *s,int l)
{
	int x=0;
	while(*s && l>0)
	{
		if(*s>='0' && *s<='9') x=x*10+*s-'0';
		else break;
		l--;
		s++;
	}
	return x;
}
void CMyTree::LoadFromFile(const char *file,const char *root)
{
	CIniFile f;
	char key[32];
	char buf[256];
	m_cznum=0;
	f.OpenFile(file);
	if(f.IsOK())
	{
      f.GotoSection("TREE");
	  while(f.GetContent(key,buf,sizeof(buf)-1) && m_cznum<TREEITEMNUM)	
	  		  {
				  int rootlen=root?strlen(root):0;
				if(rootlen>0)
                   if(strncmp(key,root,rootlen))
					   continue;
		        int num=strlen(key)/2;
		        for(int i=0;i<num;i++)
					 m_czs[m_cznum].index[i]=atoix(key+i*2,2);
/*				char *tmp=strchr(buf,',');
				if(tmp)
				{
					strcpy(m_czs[m_cznum].value,tmp+1);
					*tmp=0;
				}
				else
					strcpy(m_czs[m_cznum].value,"");
				m_czs[m_cznum].has_sub_tree=atoi(m_czs[m_cznum].value);																	
		  		strcpy(m_czs[m_cznum].name,buf);
*/				
				m_czs[m_cznum].name[0] = '\0';
				m_czs[m_cznum].value[0] = '\0';
				m_czs[m_cznum].alias[0] = '\0';
				m_czs[m_cznum].has_sub_tree = 0;
				m_czs[m_cznum].expand = false;
				m_czs[m_cznum].rela_value=0;
				m_czs[m_cznum].state=0;
				char *strToken = strtok(buf, ",\0");
				if ( strToken != NULL )
				{
					m_czs[m_cznum].rela_value=atoi(strToken);
					strToken = strtok(NULL, ",\0");
					if ( strToken != NULL )
				    { 
					strcpy(m_czs[m_cznum].name,strToken);
					strToken = strtok(NULL, ",\0");
					if ( strToken != NULL )
					{
						strcpy(m_czs[m_cznum].value,strToken);
						m_czs[m_cznum].has_sub_tree = atoi(strToken);
						strToken = strtok(NULL, ",\0");
						if ( strToken != NULL )
						{
							if ( stricmp(strToken, "expand") == 0 )
								m_czs[m_cznum].expand = true;
							else
								strcpy(m_czs[m_cznum].alias,strToken); 
						}
					}
					}
				}
				m_czs[m_cznum].indexnum=num;
				m_czs[m_cznum].maxsonindex=0;
				m_cznum++;
			  }
	  for(int sub=1;sub<10;sub++)
	  {
		 char tmpkey[10];
		 sprintf(tmpkey,"SUBTREE%02d",sub);
		 if(!f.GotoSection(tmpkey)) break;
		 struct{
			char  key;
			char  name[24];
			int   para;
			char  alias[24];
		 }subitems[10];
		 int subitemscount,j;
		 subitemscount=0;
		 for(j=0;j<10;j++)
		 {
			if(!f.GetContent(key,buf,64)) break;
			subitems[j].key=atoi(key);
			char *strToken = strtok(buf, ", \0");
			if(strToken)
				strcpy(subitems[j].name,strToken);
			else break;
			strToken = strtok(NULL,", \0");
			if(strToken)
				subitems[j].para=atoi(strToken);
			else
				break;
			strToken = strtok(NULL,", \0");
			if(strToken)
				strcpy(subitems[j].alias,strToken);
			else
				break;
			subitemscount++;
		 }
 		 int tmp=m_cznum;
	     for(j=0;j<tmp;j++)
		 {
			if(m_czs[j].has_sub_tree==sub)
			{
				for(int s=0;s<subitemscount;s++)
				{
				  for(int t=0;t<m_czs[j].indexnum;t++)
					 m_czs[m_cznum].index[t]=m_czs[j].index[t];
				  m_czs[m_cznum].index[t]=subitems[s].key;
				  m_czs[m_cznum].indexnum=t+1;
				  m_czs[m_cznum].has_sub_tree=0;
				  strcpy(m_czs[m_cznum].name,subitems[s].name);
				  m_czs[m_cznum].maxsonindex=0;
				  m_czs[m_cznum].expand = false;
				  if(subitems[s].para>0)
					m_czs[m_cznum].rela_value=((long)subitems[s].para<<16)+m_czs[j].rela_value; 
				  else
					m_czs[m_cznum].rela_value=m_czs[j].rela_value; 
				  m_czs[m_cznum].state=0;
  				  sprintf(m_czs[m_cznum].alias,subitems[s].alias,m_czs[j].rela_value);
				  m_cznum++;
				}
			}
		 }
	  }
	  
	  
	}
	SetTreeItems();

	for ( int i = 0; i < m_cznum; i++ )
	{
		if ( m_czs[i].expand )
			Expand( m_czs[i].handle, TVE_EXPAND );
	}
}

void CMyTree::SaveToFile(const char *file)
{
	FILE *fp;
	fp=fopen(file,"w");
	if(!fp) return;
	fprintf(fp,"[TREE]\n");
	for(int i=0;i<m_cznum;i++)
	{
      if(m_czs[i].indexnum<=0) continue;
	  for(int j=0;j<m_czs[i].indexnum;j++)
		  fprintf(fp,"%02d",m_czs[i].index[j]);
	  fprintf(fp,"=%s,%s\n",m_czs[i].name,m_czs[i].value);
	}
	fclose(fp);
}

int CMyTree::Insert_1_Item(int parent, const char *name, const char *value)
{
	int id;
	id=GetAvaIndex();
	if(id<0) return id;
	strcpy(m_czs[id].name,name);
	strcpy(m_czs[id].value,value);
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

const char * CMyTree::GetWholeValue(int i)
{
	static char tmp[128];
	strcpy(tmp,"");
	for(int j=1;j<=m_czs[i].indexnum;j++)
	{
		int p=GetItemByIndex(m_czs[i].index,j);
		if(p<0) break;
		strcat(tmp,m_czs[p].value);
	}
	return tmp;
}

int CMyTree::GetItemByIndex(short index[], short indexnum)
{
	for(int i=0;i<m_cznum;i++)
	{
		if(m_czs[i].indexnum!=indexnum) continue;
		if(!ShortCmp(m_czs[i].index,index,indexnum))
			return i;
	}
	return -1;
}

void CMyTree::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	if(pTVDispInfo->item.pszText)// not canceled
	{
	 int i=pTVDispInfo->item.lParam;
	
	 sscanf(pTVDispInfo->item.pszText,"%s %s",
			m_czs[i].name,m_czs[i].value);
	 
     SetItemText(pTVDispInfo->item.hItem,
			    GetDispStr(m_czs[i].name,m_czs[i].value));
	}
	*pResult = 0;
}


void CMyTree::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	int i=pTVDispInfo->item.lParam;
	GetEditControl()->SetWindowText(GetEditStr(m_czs[i].name,m_czs[i].value));
	GetEditControl()->SetLimitText(DISPLEN);
	GetEditControl()->SetSel(-1,0);
	*pResult = 0;
}


void CMyTree::Startup()
{
	SetItemHeight(20);
	if(m_flag!=0 && GetItemHeight()<EDITBARHEIGHT+2)
	{
		SetItemHeight(EDITBARHEIGHT+2);
	}
	m_statelist.Create(IDB_STATE_BMP,16,4,RGB(255,255,255));
	SetImageList(&m_statelist,TVSIL_STATE);
	
}

void CMyTree::OnEnable()
{
	GetParentOwner()->SendMessage(WM_COMMAND,ID_SET_ENABLE,0);
}

void CMyTree::OnDisable()
{
	GetParentOwner()->SendMessage(WM_COMMAND,ID_SET_DISABLE,0);
}

void CMyTree::OnItemDelete()
{
	if(MessageBox("确认删除此项及其所有子项么?","提示",MB_YESNOCANCEL|MB_ICONQUESTION)!=IDYES)
		return;


	HTREEITEM handle;
	int i;
	handle=GetSelectedItem();
	if(handle==NULL) return;
	i=GetItemData(handle);
	Delete_1_Item(i);													
}

void CMyTree::OnItemInsert()
{
	HTREEITEM handle;
	int i=-1;
	handle=GetSelectedItem();
	if(handle)
	{
	  i=GetItemData(handle);
	  if(m_czs[i].indexnum>=5)
	  {
		MessageBox("已不能纵向增加!","提示");
		return;
	  }
	  Expand(handle,TVE_EXPAND);
    }
	int son=Insert_1_Item(i,"","");	
	if(son>=0)
	{
		SelectItem(m_czs[son].handle);
	}
}

void CMyTree::OnItemEdit()
{
	HTREEITEM handle;
	int i;
 	handle=GetSelectedItem();
	if(handle==NULL) return;
	i=GetItemData(handle);
}

void CMyTree::Delete_1_Item(int id)
{
	for(int i=0;i<m_cznum;i++)
	{
		if(m_czs[i].indexnum<=0) continue;
		if(i==id) continue;
		if(!ShortCmp(m_czs[id].index,m_czs[i].index,m_czs[id].indexnum))						
			m_czs[i].indexnum=0;												
	}
	m_czs[id].indexnum=0;												
	DeleteItem(m_czs[id].handle);
}

int CMyTree::GetAvaIndex()
{
 for(int i=0;i<m_cznum;i++)
 {
	if(m_czs[i].indexnum<=0)
		return i;
 }
 if(m_cznum>=TREEITEMNUM) return -1;
 m_cznum++;
 return m_cznum-1;
}

int CMyTree::GetAvaSonIndex(int parent)
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



void CMyTree::OnRButtonDown(UINT nFlags, CPoint point) 
{
	HTREEITEM hItem;
	UINT   result;
	hItem=HitTest(point,&result);
	SelectItem(hItem);
	ClientToScreen(&point);
	CMenu popmenu;
	popmenu.CreatePopupMenu(); 
	if(hItem)
	{
	 SelectItem(hItem);
	 popmenu.AppendMenu(MF_STRING,ID_SET_ENABLE,"设置有电");
	 popmenu.AppendMenu(MF_STRING,ID_SET_DISABLE,"设置无电");
    }
    popmenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                point.x, point.y,
                this);	
   //	CTreeCtrl::OnRButtonDown(nFlags, point);
}



void CMyTree::OnLButtonDown(UINT nFlags, CPoint point) 
{
	HTREEITEM hItem;
	UINT    result;
	CTreeCtrl::OnLButtonDown(nFlags, point);
	hItem=HitTest(point,&result);
	SelectItem(hItem);
	Invalidate();
}
void CMyTree::Reset()
{
	m_cznum=0;
	DeleteAllItems();
}

const char *CMyTree::GetSelectedCaption()
{
	HTREEITEM handle;
	int i;
 	handle=GetSelectedItem();
	if(handle==NULL) return "";
	i=GetItemData(handle);
	return m_czs[i].name;
}

int  CMyTree::GetSelectedRelaValue()
{
	HTREEITEM handle;
	int i;
 	handle=GetSelectedItem();
	if(handle==NULL) return NULL;
	i=GetItemData(handle);
	return m_czs[i].rela_value;
}

int  CMyTree::GetSubRelaValues(HTREEITEM handle,int varry[],int arraysize)
{
	int retcount=0,index;
	index=GetItemData(handle);
    if(arraysize<1 || index<0)
       return 0;
	varry[retcount++]=m_czs[index].rela_value;
	HTREEITEM hChildItem = GetChildItem(handle);
	while ( hChildItem != NULL )
	{
		retcount+=GetSubRelaValues(hChildItem,varry+retcount,arraysize-retcount);
		hChildItem = GetNextSiblingItem(hChildItem);
	}
	return retcount;
}

int  CMyTree::GetSelectedSubTreeRelaValues(int varry[],int arraysize) // 取出自身和所有子节点值
{
	HTREEITEM handle;
 	handle=GetSelectedItem();
	if(handle==NULL) return -1;
	return GetSubRelaValues(handle,varry,arraysize);
}

void  CMyTree::UpdateParentState(int i)
{
    int parent;
	parent=i;
	while((parent=GetParent(parent))>=0)
	{
		int minstatus=100,maxstatus=0,st;
		for(int i=0;i<m_cznum;i++)
		{
			if(i==parent) continue;
			if(m_czs[i].indexnum!=m_czs[parent].indexnum+1)
				continue;
			if(ShortCmp(m_czs[i].index,m_czs[parent].index,m_czs[parent].indexnum))
			    continue;
			// found sons
			if(m_czs[i].state<minstatus)
				minstatus=m_czs[i].state;
			if(m_czs[i].state>maxstatus)
				maxstatus=m_czs[i].state;	
		}
		
		if(minstatus==3)
			st=3;
		else
		if(maxstatus<=1)
			st=maxstatus;
		else
			st=2;
		if(m_czs[parent].rela_value!=0)
			continue;
		if(m_czs[parent].state!=st)
		{
			m_czs[parent].state=st;
			SetItemState(m_czs[parent].handle,
				INDEXTOSTATEIMAGEMASK(st),
				TVIS_STATEIMAGEMASK);
		}
	}
}

void  CMyTree::SetObjStateByRelaValue(long  rela_va,int state)
{
for(int i=0;i<m_cznum;i++)
 {
	if(m_czs[i].rela_value==rela_va)
	{
		if(m_czs[i].has_sub_tree)
			return;   // Not direct set parent node status
		SetItemState(m_czs[i].handle,
				INDEXTOSTATEIMAGEMASK(state),
				TVIS_STATEIMAGEMASK);
		m_czs[i].state=state;
		UpdateParentState(i);
		break;
	}
 }
}

void  CMyTree::SetObjStateByAlias(const char *alias,int state)
{
for(int i=0;i<m_cznum;i++)
 {
	if(!stricmp(m_czs[i].alias,alias))
	{
		if(m_czs[i].has_sub_tree)
			return;   // Not direct set parent node status
		SetItemState(m_czs[i].handle,
				INDEXTOSTATEIMAGEMASK(state),
				TVIS_STATEIMAGEMASK);
		m_czs[i].state=state;
		UpdateParentState(i);
		break;
	}
 }
}

void  CMyTree::SetObjState(const char *id,int state)
{
 for(int i=0;i<m_cznum;i++)
 {
	if(m_czs[i].indexnum<=0) continue;
	const char *str=GetIDStr(i);
	if(!str) continue;
	if(!stricmp(str,id))
	{
		SetItemState(m_czs[i].handle,
				INDEXTOSTATEIMAGEMASK(state),
				TVIS_STATEIMAGEMASK);
		m_czs[i].state=state;
		UpdateParentState(i);
		break;
	}
 }
}

void  CMyTree::SetObjValue(const char *id,const char *value)
{
for(int i=0;i<m_cznum;i++)
 {
	if(m_czs[i].indexnum<=0) continue;
	const char *str=GetIDStr(i);
	if(!str) continue;
	if(!stricmp(str,id))
	{
		SetItemText(m_czs[i].handle,GetDispStr(m_czs[i].name,m_czs[i].value));
		break;
	}
 }
}

const char *CMyTree::GetIDStr(int id)
{
	static char tmp[64];
	strcpy(tmp,"");
	for(int i=0;i<m_czs[id].indexnum;i++)
	{
		char t[12];
		sprintf(t,"%02d",m_czs[id].index[i]);
		strcat(tmp,t);
	}
	return tmp;
}

short CMyTree::SelectAndExpandObject(LPCTSTR alias, LPCTSTR strid, long rela_va)
{
	for(int i=0;i<m_cznum;i++)
	{
		if(alias && stricmp(m_czs[i].alias,alias))
			continue;
		if(strid && stricmp(GetIDStr(i),strid))
			continue;
		if(rela_va>0 && m_czs[i].rela_value!=rela_va)
			continue;
		SelectItem(m_czs[i].handle);
		Expand( m_czs[i].handle,TVE_EXPAND);		
		return 1;
	}
	return 0;
}

int CMyTree::GetArmStationByArmNo(int armno, int varry[],int arraysize, CString& armname)
{
	armname.Format("未知供电单元号:%d", armno);
	HTREEITEM hRoot = GetRootItem();
	if(hRoot==NULL)
		return 0;
	int num=0;
	HTREEITEM hRootChild=GetChildItem(hRoot);
	while(hRootChild!=NULL)
	{
		int elecarmno=0;

		int count=GetSubRelaValues(hRootChild,varry,arraysize);
        for(int i=0; i<count; i++)
		{
			elecarmno=(varry[i]/10000);
			if(elecarmno!=0)
				break;
		}

		if(elecarmno==armno)
		{
			num=count;
			int i=GetItemData(hRootChild);
			if(i>=0 && i<m_cznum)
			{
				armname=m_czs[i].name;
			}
			break;
		}
		
		hRootChild=GetNextSiblingItem(hRootChild);
	}

	return num;
}
