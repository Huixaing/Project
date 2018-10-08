#include "stdafx.h"
#include "txtfile.h"
#include "mylist.h"

void  MyAddRow(CListCtrl *list,MYROW  *r)
{
	char items[20][MAXSTRLEN];
	int  in;
	in=CTxtFile::GetSubStr(r->m_content,items,20,',');
	LV_ITEM lvi;
    lvi.mask =LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = r->m_itemid;
	lvi.iSubItem = 0;
    lvi.pszText ="";
	lvi.iImage = r->m_imageid;
	lvi.stateMask = LVIS_STATEIMAGEMASK;
    lvi.state =INDEXTOSTATEIMAGEMASK(r->m_stateid);
	lvi.lParam = r->m_key;
    int itemid=list->InsertItem(&lvi);	
	if(itemid==-1) return;
	for(int col=0;col<in;col++)
		list->SetItemText(itemid,col,items[col]);
}

void  MySetColumn(CListCtrl *list,MYCOLUMN  c[],int num)
{
	int i;
	LV_COLUMN lvc;
	while(list->DeleteColumn(0));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(i = 0; i<num; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = c[i].m_head;
		lvc.cx = c[i].m_width;
		lvc.fmt = c[i].m_fmt;
		list->InsertColumn(i,&lvc);
	}
}

/////////////////////
static MYCOLUMN  listhead[3]={
	{"Ê±¿Ì",160,LVCFMT_CENTER},
	{"Ãû³Æ",80,LVCFMT_CENTER},
	{"×´Ì¬",80,LVCFMT_CENTER}
};
////////////////////////////
void SetListFile(const CListCtrl &list,const char *filename)
{
		
}

void SetListFile(CListCtrl *list,const char *filename)
{
	list->DeleteAllItems();
	CTxtFile t;
	t.OpenFile(filename);
	const char *l;
	int id=0;
	char line[256],key[64];
	for(int i=0;i<10;i++)
	{
		if(t.GetLineX(line,key)==2)
		{
			if(!stricmp(key,"HEADER"))
				break;
		}
	}
	if(i>=10) 
	{
		SetListType(list,0);
	}
    else
	{
	char items[20][MAXSTRLEN];
	int colnum=CTxtFile::GetSubStr(line,items,20,',');
	MYCOLUMN  mycolumn[20];
	for(i=0;i<colnum;i++)
	{
		mycolumn[i].m_fmt =LVCFMT_CENTER;
		mycolumn[i].m_head =items[i];
		mycolumn[i].m_width =strlen(items[i])*12;
	}
	MySetColumn(list,mycolumn,colnum);
	}
	t.Reset();
	while(l=t.GetLine())
	{
		MYROW r;
		r.m_label=NULL;
	    r.m_imageid=0;
        r.m_stateid=0;	
        r.m_itemid=id++;
        r.m_content=l;
        r.m_token=0;
        r.m_key=0;
		MyAddRow(list,&r);
	}
	t.CloseFile();
}

void SetListType(CListCtrl *list,int flag)
{
	MySetColumn(list,listhead,3);
}

void InsertLineToList(CListCtrl *list,const char *l)
{
		MYROW r;
		r.m_label=NULL;
	    r.m_imageid=0;
        r.m_stateid=0;	
        r.m_itemid=0;
        r.m_content=l;
        r.m_token=0;
        r.m_key=0;
		MyAddRow(list,&r);
}

void  SaveListToFile(CListCtrl *list,FILE *fp)
{
      int rowcnt=list->GetItemCount();
	  int colnum=100;
      for(int i=0;i<rowcnt;i++)
        {
            char buf[64];
            for(int col=0;col<colnum;col++)
			{
              int nRetLen = list->GetItemText(i, col,buf,64);
              if (nRetLen <= 0)  break;
			  buf[nRetLen]=0;
              fprintf(fp,"%s  ",buf);
			}
           fprintf(fp,"\n");
        }
}
int Get1LineOfList(CListCtrl *l,int id,char *buf)
{
  return 0;	
}

#define  LINEPERPAGE 30
void CListPrinter::SetHeaderByType(int type)
{
	if(type==0)
	{
		m_colnum=3;
		m_headertext[0]=listhead[0].m_head;
		m_headertext[1]=listhead[1].m_head;
		m_headertext[2]=listhead[2].m_head;
	}
}

void CListPrinter::Output(CDC *pDC, RECT bound,int pageno)
{
     m_pDC=pDC;
	 m_from=(pageno-1)*LINEPERPAGE;
	 m_to=m_from+LINEPERPAGE;
	 if(m_to>=m_linecount) 
		 m_to=m_linecount;
	 
	////////////////////////////////////////////
	 int py=20,px;
  	int  colwidth[100];																
	///  print page HEADER
    CFont *poldfont;
	CSize  cs;
	COLORREF oldcolor;
	CRect  rect;
	rect=bound;
	px=rect.Width()/10;
	int tmpct=0;
	for(int i=0;i<m_colnum;i++)
		tmpct+=m_headertext[i].GetLength();
	for(i=0;i<m_colnum;i++)
        colwidth[i]=rect.Width()*m_headertext[i].GetLength()/tmpct;
	poldfont=(CFont *)pDC->SelectObject(m_titlefont);
	oldcolor=pDC->SetTextColor(m_titlecolor);
	cs=pDC->GetTextExtent(m_titletext);
	rect.top=py;
	rect.bottom=py+cs.cy;
	pDC->DrawText(m_titletext,&rect,DT_CENTER);

	pDC->SelectObject(m_headerfont);
	pDC->SetTextColor(m_headercolor);
	cs=pDC->GetTextExtent("ºº×Ö"); 
	rect.top=rect.bottom+py*3;
	rect.bottom=rect.top+cs.cy;
	rect.right=bound.left;
	for(i=0;i<m_colnum;i++)
	{
		rect.left=rect.right;
		rect.right=rect.left+colwidth[i];
		pDC->DrawText(m_headertext[i],&rect,DT_CENTER); 
	}
	
	// Draw a  line
	CPen pen,*poldpen;
	pen.CreatePen(PS_SOLID,3,RGB(0,0,0));
	poldpen=pDC->SelectObject(&pen);
	pDC->MoveTo(0,rect.bottom+py);
	pDC->LineTo(rect.right,rect.bottom+py);
	pDC->SelectObject(poldpen);
	pen.DeleteObject(); 
	//

	pDC->SelectObject(m_normalfont);
	pDC->SetTextColor(m_normalcolor);
	cs=pDC->GetTextExtent("ºº×Ö"); 
	rect.bottom+=py;
	for(int r=m_from;r<m_to;r++)
	{
		rect.top=rect.bottom+py;
		rect.bottom=rect.top+cs.cy;
		rect.right=bound.left;
		for(int c=0;c<m_colnum;c++)
		{
		 rect.left=rect.right;
		 rect.right=rect.left+colwidth[c];
		 pDC->DrawText(GetItemText(r,c),&rect,DT_CENTER); 		
		}
	}
	pDC->SelectObject(poldfont);
	pDC->SetTextColor(oldcolor);

}

void CListPrinter::PrepareOutput(const char *file)
{
	OpenRPTFile(file);
}

void CListPrinter::EndOutput()
{
	 Cleanup();
}

CFont *CListPrinter::CreateFontByPara(const char *para,CDC *pdc)
{
	char items[4][MAXSTRLEN];
	if(CTxtFile::GetSubStr(para,items,2)==2)
	{
	  CFont *pf=new CFont();
	  pf->CreatePointFont(atoi(items[1]),items[0],pdc);
	  return pf;
	}
	else
	 return NULL;
}

void CListPrinter::Cleanup()
{
	if(m_titlefont) delete m_titlefont;
	if(m_headerfont) delete m_headerfont;
	if(m_normalfont) delete m_normalfont;
	m_titlefont=0;
	m_headerfont=0;
	m_normalfont=0;
	m_colnum=0;

}

int  CListPrinter::OpenRPTFile(const char *file)
{
	if(!file) return 0;  // used to reset
	m_file.OpenFile(file);
	if(!m_file.IsOK()) return 0;
	m_linecount=m_file.GetNormalLineCount();
	m_file.Reset();
	return 1;
}

void CListPrinter::BeginOutput(CDC *pdc)
{
	m_pDC=pdc;
	Cleanup();

	char  content[256],key[32];
	for(int i=0;i<10;i++) // look in first 10 lines to find REPORT definition							
	{
	  if(m_file.GetLineByNo(i,content,key)==2)
	  {
		  if(!stricmp(key,"TITLE"))
			  m_titletext=content;
		  else
          if(!stricmp(key,"TITLEFONT"))
              m_titlefont=CreateFontByPara(content,m_pDC);
		  else
          if(!stricmp(key,"HEADER"))
		  {
			   char *p0=content,*p1;
			   while(*p0)
			   {
				   p1=strchr(p0,',');
				   if(p1) *p1=0;
                   m_headertext[m_colnum++]=p0;
				   if(p1) p0=p1+1;
				   else break;
			   }
		  }
		  else
		  if(!stricmp(key,"HEADERFONT"))
			  m_headerfont=CreateFontByPara(content,m_pDC);
		  else
		  if(!stricmp(key,"NORMALFONT"))
			  m_normalfont=CreateFontByPara(content,m_pDC);  
	  }
	}// end for,
    if(m_colnum<=0) 
	{
		SetHeaderByType(0);
		m_titletext="°´Å¥²Ù×÷¼ÇÂ¼";
	}
	if(!m_titlefont)
	{
		m_titlefont=new CFont();
		m_titlefont->CreatePointFont(300,"ºÚÌå",m_pDC);
	}
	if(!m_headerfont)
	{
		m_headerfont=new CFont();
		m_headerfont->CreatePointFont(240,"ºÚÌå",m_pDC);
	}
	if(!m_normalfont)
	{
		m_normalfont=new CFont();
		m_normalfont->CreatePointFont(160,"ËÎÌå",m_pDC);
	}
	m_file.Reset();
	m_currow=-1;
}

const char *CListPrinter::GetItemText(int row,int col)
{
	if(row==m_currow)
		return m_rowitems[col];
	else
	{
		int ret;
		char line[256],key[32];
		/*
		do{
			
		   ret=m_file.GetLineX(line,key);
		   if(!stricmp(key,"NORMALFONT"))
			{
			   if(m_normalfont) 
				   delete m_normalfont;
			    m_normalfont=CreateFontByPara(line,m_pDC);
			}
		}while(ret==2);
		if(ret==0) return NULL;
		*/
        if(m_file.GetNormalLineByNo(row,line)<0) return NULL;
		for(int i=0;i<m_colnum;i++)
			m_rowitems[i][0]=0;
		CTxtFile::GetSubStr(line,m_rowitems,m_colnum,',');
		m_currow=row;
		return m_rowitems[col];
	}
}
int  CListPrinter::GetPageCount()
{
	return (m_linecount/LINEPERPAGE+1);
}