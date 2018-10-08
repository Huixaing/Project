#ifndef __MYLISTINCLUDE
#define __MYLISTINCLUDE
#include "txtfilex.h"
typedef struct{
   TCHAR  *m_head;
   int     m_width;
   int     m_fmt;
}MYCOLUMN;

typedef struct{
   TCHAR  *m_label;
   int   m_imageid;
   int	 m_stateid;	
   int   m_itemid;
   CString m_content;
   int    m_token;
   long   m_key;
}MYROW;
void  MyAddRow(CListCtrl *list,MYROW  *r);
void  MySetColumn(CListCtrl *list,MYCOLUMN  c[],int num);
void SetListFile(CListCtrl *list,const char *filename);
void SetListFile(const CListCtrl &list,const char *filename);
void SetListType(CListCtrl *list,int flag);
void InsertLineToList(CListCtrl *list,const char *l);
void  SaveListToFile(CListCtrl *list,FILE *fp);
#define PRINTCOLNUM 16
class CListPrinter{
public:
  CDC  *m_pDC;
  CTxtFileX m_file;	  
  CString m_titletext;
  CFont  *m_titlefont;
  CFont  *m_headerfont;
  CFont  *m_normalfont;
  COLORREF  m_titlecolor;
  COLORREF  m_headercolor;
  COLORREF  m_normalcolor;
  CString   m_headertext[PRINTCOLNUM];
  int     m_colnum;
  RECT    m_pagerect;
  int     m_from,m_to;
  int     m_linecount;
  int     m_currow;
  char    m_rowitems[PRINTCOLNUM][MAXSTRLEN];	
  CListPrinter()
  {

	m_linecount=0;
    m_titlecolor=m_headercolor=m_normalcolor=RGB(0,0,0);
	m_titlefont=m_headerfont=m_normalfont=NULL;
	m_colnum=0;
	m_from=m_to=0;
	m_pDC=NULL;
  }
  ~CListPrinter()
  {
	Cleanup();
  }
  CFont *CreateFontByPara(const char *para,CDC *pdc);
  void SetHeaderByType(int type);
  void Output(CDC *pDC, RECT rect,int pageno);
  void PrepareOutput(const char *title);
  void BeginOutput(CDC *pdc);
  void EndOutput();
  int  OpenRPTFile(const char *file);
  const char *GetItemText(int row,int col);
  void Cleanup();
  int  GetPageCount();
};
#endif

