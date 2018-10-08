// ShiguOutlineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ShiguOutlineDlg.h"
#include <baseMsg\msgid.h>
#include <BaseMsg/clbth.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WHOLE_DAY_SECOND 24*3600

/////////////////////////////////////////////////////////////////////////////
// CShiguOutlineDlg dialog


CShiguOutlineDlg::CShiguOutlineDlg(int section_id,int shiftid,int schd_type,CWnd* pParent /*=NULL*/)
	: CDialog(CShiguOutlineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShiguOutlineDlg)
	m_pGridCtrl = NULL;
	m_nFixCols = 1;
	m_nFixRows = 1;
	m_nCols = 2;
	m_nRows = 1;
	m_title = _T("");
	m_detail = _T("");
	nAccident= 0;
	CurrentRow = -1;
	schdtype = schd_type;
	m_RecordsPerPrintPage = 50;
	//}}AFX_DATA_INIT

	m_nSectionID=section_id;
	m_nShiftID=shiftid;
}


void CShiguOutlineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShiguOutlineDlg)
	DDX_Control(pDX, IDC_CMB_MODULE, m_Ctrlmodule);
	DDX_Control(pDX, IDC_EDIT_DETAIL, m_EditDetail);
	DDX_Control(pDX, IDC_EDIT_NEW_WORD, m_Newword);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_FRAME, m_Frame);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_title);
	DDV_MaxChars(pDX, m_title, 40);
	DDX_Text(pDX, IDC_EDIT_DETAIL, m_detail);
	DDV_MaxChars(pDX, m_detail, 2400);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CShiguOutlineDlg, CDialog)
	//{{AFX_MSG_MAP(CShiguOutlineDlg)
	ON_BN_CLICKED(IDC_ADD, OnAddNewDetail)
	ON_BN_CLICKED(IDC_WORD_EDIT, OnWordEdit)
	ON_BN_CLICKED(IDC_WORD_DELETE, OnWordDelete)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_WORD_ADD, OnWordAdd)
	ON_MESSAGE(GIVE_ACCIDENT, OnAccidentMsg)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_CBN_SELENDOK(IDC_CMB_MODULE, OnSelendokCmbModule)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShiguOutlineDlg message handlers

BOOL CShiguOutlineDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	TCHAR title[2][20]={_T("序号"), _T("   标题  ")};
	// to Init and show gird ctrl in dialog;
    if(m_pGridCtrl!=NULL)
    	return FALSE;
	m_pGridCtrl=new CGridCtrl;
	if(!m_pGridCtrl) 
		return FALSE;
 
    //set grid frame border in dialog;
 	CRect rc;
	m_Frame.GetWindowRect(rc);
	ScreenToClient(rc);
	//rc.InflateRect(-10,-30,-10,-50);
	m_pGridCtrl->Create (rc,this,IDC_GRID);
    int nwidth = (rc.right-rc.left)/3;
	try{
		m_pGridCtrl->SetRowCount(m_nRows);                //行
		m_pGridCtrl->SetColumnCount(m_nCols);             //列
		m_pGridCtrl->SetFixedRowCount(m_nFixRows);        //固定行
		m_pGridCtrl->SetFixedColumnCount(m_nFixCols);     //固定列		    
		//m_pGridCtrl->SetFixedTextColor(RGB(0,0,0));       //文本颜色
		m_pGridCtrl->SetGridLineColor(RGB(166,202,240));  //线的颜色
		m_pGridCtrl->SetEditable(FALSE);    
	    m_pGridCtrl->SetRowResize(FALSE);
         
		m_pGridCtrl->SetColumnWidth(0,nwidth);
        m_pGridCtrl->SetColumnWidth(1,nwidth*2);
		m_pGridCtrl->SetColumnResize(FALSE);
	}
   	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

  DWORD dwTextStyle = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
#ifndef _WIN32_WCE
    dwTextStyle |= DT_END_ELLIPSIS;
#endif
 int row,col;
	/*显示第一行的标题栏*/
	for ( row = 0; row < 1; row++)
	for ( col = 0; col < 2; col++)
	{ 
		GV_ITEM Item,Item2;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.col = col;
  		Item.nFormat = DT_CENTER|DT_WORDBREAK;
		if(row==0)
		{   
			Item.row = 0;
			Item.strText.Format(title[col]);
		}
	 	 
		m_pGridCtrl->SetItem(&Item);
	    
	}
   
    ////m_pGridCtrl->ExpandColumnsToFit();
  

	CString datadir = gpcdef.GetDataDir();
	CStdioFile f_words;
	
	if(f_words.Open(datadir+"\\行车事故词汇.txt",CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead))
	{
		CString linetext;
		while(f_words.ReadString(linetext))
			m_List.AddString(linetext);
	}
	f_words.Close();

	g_curEdit = (CEdit*)GetDlgItem(IDC_EDIT_DETAIL);

	char path[256];
	sprintf(path,"%s\\行车事故模板\\*.cfg",gpcdef.GetDataDir());
    memset(&sAM,0x00,sizeof(sAM));
	//read from file
	ReadModuleFile(path);
	//sort moudle from small no. to big no.
	SortModuleFile();
    //show moudle in combo;
	for(int j = 0;j < sAM.module_num;j++)
	{
		CString str,tmp;
		tmp = sAM.details[j].no;
		str = tmp + "   " + sAM.details[j].type_name;
		if(atoi(sAM.details[j].no) < 10000)
		m_Ctrlmodule.InsertString(j,str);
	}
    m_Ctrlmodule.SetCurSel(0);
	OnSelendokCmbModule();

	Query_Accident();
    InitIdentify();
	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CShiguOutlineDlg::InitIdentify()
{
	
	if( !gpcdef.GetOperatorRight(WINTG_EDIT)
	   || (schdtype == HISTORY_SCHEDULE) 
	   || (schdtype ==	UNCHANGE_HISTORY_SCHEDULE))
	{
		CWnd *pWnd;
		
		pWnd = GetDlgItem(IDC_ADD);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT);
		pWnd->EnableWindow(FALSE);
		
		CButton *pButton;
		pButton= (CButton *)GetDlgItem(IDC_DELETE);
		pButton->EnableWindow(FALSE);
		
	}
	
}
void CShiguOutlineDlg::OnAddNewDetail() 
{
 	if(nAccident >MAX_ACCIDENT)
	{	AfxMessageBox("行车事故记录太多!");
        return;
	}

	UpdateData(true);
/*
    nAccident +=1;
	m_nRows +=1;
	m_pGridCtrl->SetRowCount(m_nRows);                //行
 
	for (int  col = 0; col < 2; col++)
	{ 
		GV_ITEM Item,Item2;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.col = col;
  		Item.nFormat = DT_CENTER|DT_WORDBREAK|DT_NOPREFIX;
		Item.row = m_nRows-1;
		if(col==0)
			Item.strText.Format("%d",nAccident);
	    else if(col ==1)
	 	   	Item.strText.Format("%s", m_title);
		
		m_pGridCtrl->SetItem(&Item);
	    
	}
   m_pGridCtrl->Invalidate(false);
   */
  /*
  ushort  index;
	 char  subject[40];
	 char  detail[2400];
  */

   ////nAccident +=1;
   ////accident[nAccident-1].index = nAccident;
   ////strcpy(accident[nAccident-1].subject,m_title);
   ////strcpy(accident[nAccident-1].detail,m_detail);

   //Add_OR_Change_Accident(0,GetMaxIndex());
   SendMsgAccidentOper(CLBTH::MsgAccidentOper::ACCIDENT_INSERT_UPDATE,GetMaxIndex());
   m_title =m_detail ="";
   UpdateData(false);
   nAccident =0;
   m_nRows =1;
  	CurrentRow = -1;

   Query_Accident();

}

void CShiguOutlineDlg::OnWordEdit() 
{						
	CString datadir = CString(gpcdef.GetDataDir()) +"\\行车事故词汇.txt";
	CString execmd = "notepad.exe '" + datadir + "'";
	WinExec(execmd,SW_SHOW);
	
}

void CShiguOutlineDlg::OnWordDelete() 
{
	int cur_sel_num,words_count;
	cur_sel_num = m_List.GetCurSel();
	words_count = m_List.GetCount();
				
	if(cur_sel_num<0)
		return ;
				
	m_List.DeleteString(cur_sel_num);
				
	if(cur_sel_num == words_count-1)//删除了最后一条记录
		m_List.SetCurSel(0);
	else
		m_List.SetCurSel(cur_sel_num);
				
	CString datadir = gpcdef.GetDataDir();
	CStdioFile f_words;
				
	if(f_words.Open(datadir+"\\行车事故词汇.txt",CFile::modeCreate|CFile::modeWrite))
	{
		CString linestr;
		for(int i=0; i<m_List.GetCount();i++)
		{
			m_List.GetText(i,linestr);
			f_words.WriteString(linestr);
			f_words.WriteString("\n");
		}
	}
				
	f_words.Close();
	
}

void CShiguOutlineDlg::OnDblclkList() 
{

	CString curtext;
	m_List.GetText(m_List.GetCurSel(),curtext);
	if(g_curEdit != NULL)
	{
		g_curEdit->ReplaceSel(curtext,true);
		g_curEdit->SetFocus();
	}	
}

void CShiguOutlineDlg::OnWordAdd() 
{
	 
	char str[255];
	if(m_Newword.GetLine(0,str,254)<=0)
	{
		m_Newword.SetFocus();
		return ;
	}
				
	CString datadir = gpcdef.GetDataDir();
	CStdioFile f_words;
	if(f_words.Open(datadir+"\\行车事故词汇.txt",CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
	{
		f_words.SeekToEnd();
		f_words.WriteString(str);
		f_words.WriteString("\n");
		m_List.AddString(str);
	}
				
	f_words.Close();
	m_Newword.SetWindowText("");
	m_Newword.SetFocus();	
}
void CShiguOutlineDlg::Query_Accident()
{   
	CLBTH::MsgAccidentQuery mMsgAccidentQuery;
	mMsgAccidentQuery.con_id = m_nSectionID;
	mMsgAccidentQuery.schd_type = schdtype;
	mMsgAccidentQuery.shift_id = m_nShiftID;

	CLBTH::Archive ar( CLBTH::Archive::store);
	mMsgAccidentQuery.serialize(ar);
	
	char *ptr = (char *)ar.getBuffer();
    int count =  ar.getCount();
	
	SendTGCommMessage(ar.getCount(),(char *)ar.getBuffer()); 
}
void CShiguOutlineDlg::Del_Accident(ushort nIndex)
{
	/*delete accident format :
	 <msg_len(2) <DEL_ONE_ACCIDENT(2)> <con_id(1)>  <schd_type(2)> <shift_id(4)> 
	 */
	char buff[128], *ptr;
	ptr = buff;
	ushort count;
	
	*ptr =PMTG_MESSAGE;   
	ptr+=1;
	
	ptr += 2;           //for msglen
	
	*(ushort *)ptr = NettoHostUShort(DEL_ONE_ACCIDENT); 
	ptr += 2;   
	
	*ptr = m_nSectionID;
	
	ptr += 1;   
	
	*ptr = schdtype;
	ptr +=1;
	
	unsigned long shift_id;
	shift_id = m_nShiftID;	 
    *(unsigned long *)ptr=NettoHostULong(shift_id);       // shift_id   
	ptr+=4;     

	*(ushort *)ptr = NettoHostUShort(nIndex);
	ptr +=2;

	count = ptr -buff;
	*(ushort *)(buff + 1) = NettoHostUShort(count - 1);
    SendTGCommMessage(count,buff);	
	 
}
void CShiguOutlineDlg::Add_OR_Change_Accident(int type,ushort nIndex)
{
	/*add one accident format :
	 <msg_len(2) <ADD_ONE_ACCIDENT(2)> <con_id(1)> <schd_type(2)> <shift_id(4)> 
	 <index(2)> <subject_len(2)> <subject(40 max)> <note_len(2)> <note(2400 MAX)>
	 */
	char buff[2500], *ptr;
	ptr = buff;
	ushort count;
	
	*ptr =PMTG_MESSAGE;   
	ptr+=1;
	
	ptr += 2;           //for msglen
	
	if(type ==0)
	{
	  *(ushort *)ptr = NettoHostUShort(ADD_ONE_ACCIDENT); 
	  ptr += 2;   
	}
	else
	{
	  *(ushort *)ptr = NettoHostUShort(CHANGE_ONE_ACCIDENT); 
	  ptr += 2; 
	}

	*ptr = m_nSectionID;
	
	ptr += 1;   
	
	*ptr = schdtype;
	ptr +=1;

	unsigned long shift_id;
	shift_id = m_nShiftID;	 
    *(unsigned long *)ptr=NettoHostULong(shift_id);       // shift_id   
	ptr+=4;     

	*(ushort *)ptr = NettoHostUShort(nIndex);
	ptr +=2;

	///ushort nlen = strlen(accident[nAccident-1].subject);
    ushort nlen = m_title.GetLength();
	*(ushort *)ptr = NettoHostUShort(nlen);
	ptr +=2;

	//strncpy(ptr,accident[nAccident-1].subject,nlen);
    strncpy(ptr,m_title,nlen);
    ptr +=nlen;

	// nlen = strlen(accident[nAccident-1].detail);
     nlen = m_detail.GetLength();
	*(ushort *)ptr = NettoHostUShort(nlen);
	ptr +=2;

	//strncpy(ptr,accident[nAccident-1].detail,nlen);
	strncpy(ptr,m_detail,nlen);
    ptr +=nlen;

    count = ptr -buff;
	*(ushort *)(buff + 1) = NettoHostUShort(count - 1);
    SendTGCommMessage(count,buff);	
 
}
LRESULT CShiguOutlineDlg::OnAccidentMsg(WPARAM wParam, LPARAM lParam)
{  
	CLBTH::MsgAccidentOper *pBuf = (CLBTH::MsgAccidentOper *)wParam;

	do{
		if(schdtype != pBuf->schd_type || pBuf->oper != CLBTH::MsgAccidentOper::ACCIDENT_SERVER_ECHO)
         break;
      if(gpcdef.GetLoginShiftid() != pBuf->shift_id)
		  break;

	  nAccident +=1;
      accident[nAccident-1].index = pBuf->index; //index
      strncpy(accident[nAccident-1].subject,pBuf->subject,sizeof(pBuf->subject));// subject 
      memcpy(accident[nAccident-1].detail,pBuf->note, pBuf->note_len);// note

 	  accident[nAccident-1].detail[pBuf->note_len]=0;
	  
	  m_nRows +=1;
	  m_pGridCtrl->SetRowCount(m_nRows);                //行
	  
	  for (int  col = 0; col < 2; col++)
	  { 
		  GV_ITEM Item,Item2;
		  Item.mask = GVIF_TEXT|GVIF_FORMAT;
		  Item.col = col;
		  
		  Item.row = m_nRows-1;
		  if(col==0)
		  {	Item.strText.Format("%d",nAccident);
		  Item.nFormat = DT_CENTER;
		  }
		  else if(col ==1)
		  { Item.strText.Format("%s", accident[nAccident-1].subject);	
          Item.nFormat = DT_LEFT;
		  }
		  m_pGridCtrl->SetItem(&Item);
		  
	  }
	  	 
	m_pGridCtrl->Invalidate(false);
	}while(0);
   
	return TRUE;
}

BOOL CShiguOutlineDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
 	int   idCtrl = (int) wParam;
	if(idCtrl!=IDC_GRID ) 
		return CShiguOutlineDlg::OnNotify(wParam, lParam, pResult);
	
    NM_GRIDVIEW *nmgv=(NM_GRIDVIEW*)  lParam;;
	int column=nmgv->iColumn;
	CurrentRow=nmgv->iRow ;  //CurrentRow

	if(CurrentRow>=1)
	{
		m_title.Format("%s",accident[CurrentRow-1].subject); 
        m_detail.Format("%s",accident[CurrentRow-1].detail); 
		UpdateData(false);
	}
    int nMessageType=nmgv->hdr.code;
//    int j;
    switch(nMessageType)
	{
	case GVN_BEGINLABELEDIT:
		*pResult = (1)? -1 : 0; 
		TRACE("[BEGIN LABEL EDIT]Current row is: %d \n",CurrentRow);
		break;
	case GVN_ENDLABELEDIT:
		*pResult = (1)? -1 : 0;      
		TRACE("[END LABEL EDIT]Current row is: %d \n",CurrentRow);
		break;
    case LVN_ITEMCHANGED: 
		break;
	case LVN_COLUMNCLICK:
		break;
	default:
		break;
	}	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CShiguOutlineDlg::OnDelete() 
{
   if(CurrentRow<=0)
   {
	   AfxMessageBox("请选中一条行车事故!");
	   return;
   }
   ////Del_Accident(accident[CurrentRow-1].index);
    SendMsgAccidentOper(CLBTH::MsgAccidentOper::ACCIDENT_DELETE,accident[CurrentRow-1].index);
   // reset all and query ;
   m_title =m_detail ="";
   UpdateData(false);

   nAccident =0;
   m_nRows =1;
   CurrentRow = -1;
   Query_Accident();


}

void CShiguOutlineDlg::OnEdit() 
{
   if(CurrentRow<=0)
   {
	   AfxMessageBox("请选中一条行车事故!");
	   return;
   }
   UpdateData(true);
   //Add_OR_Change_Accident(1,accident[CurrentRow-1].index);
   SendMsgAccidentOper(CLBTH::MsgAccidentOper::ACCIDENT_INSERT_UPDATE,accident[CurrentRow-1].index);
   // reset all and query ;
   m_title =m_detail ="";
   UpdateData(false);

   nAccident =0;
   m_nRows =1;
   	CurrentRow = -1;
   Query_Accident();
}

int CShiguOutlineDlg::GetMaxIndex()
{
	if(nAccident<=0)  return 1;
	int result =0,temp =0;
    for(int i=0; i<nAccident; i++)
	{   
		temp = accident[nAccident-1].index;
	    if(result <temp)
			result = temp;
	}
	return result+1;
}

//获取打印设备函数
//////////////////////////////////////////////////////////////
//  此函数通过一个设备名字获得 devname 和 devMode
//
//  注意调用此函数的外部函数必须释放devname 和 devMode 所分配的空间
//
//  returns a DEVMODE and DEVNAMES for the printer name specified
//////////////////////////////////////////////////////////////
extern BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);
void CShiguOutlineDlg::OnPrint() 
{
	if(AfxMessageBox("打印行车事故概况信息?",MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES  )
		return; 
	
	CWinApp* cApp=AfxGetApp();
	ASSERT_VALID(cApp);
	
	HGLOBAL hDevMode;
	HGLOBAL hDevNames;
	
	CString csPlotterName;
	csPlotterName.Format("%s",gpcdef.GetPrinterName());
	if (!GetPrinterDevice((char *)(LPCTSTR)csPlotterName, &hDevNames, &hDevMode))
	{
		AfxMessageBox(_T("指定的打印机设备不存在!\n"));
		return;
	}
	
	CPrintInfo pInfo;
	HDC prnDc;
	
	pInfo.m_pPD->m_pd.hDevMode = hDevMode;
	pInfo.m_pPD->m_pd.hDevNames = hDevNames;
	prnDc = pInfo.m_pPD->CreatePrinterDC();
	
	CDC printDC;
	printDC.Attach(prnDc);
	
	DOCINFO printDocumentName;
	CString sPrintName;
	sPrintName = "行车事故概况";
	
	char sPrintDocName[32];
	memcpy(sPrintDocName,sPrintName,32);
	
	printDocumentName.cbSize = sizeof(DOCINFO);
	printDocumentName.lpszDocName = sPrintDocName;
	printDocumentName.lpszOutput = NULL;
	printDC.StartDoc(&printDocumentName);
	
	
	int pageNo = 1;
 
	pInfo.SetMaxPage(pageNo);
	pInfo.SetMinPage(1);

	for(int i=1; i<=pageNo; i++)
	{
    	printDC.StartPage( );
		pInfo.m_nCurPage = i;
		//打印每页的信息;
		PrintEveryPageInfo(&printDC,&pInfo);
		printDC.EndPage( );
	}
	

	printDC.EndDoc( );
	printDC.Detach( );
	DeleteDC(prnDc);
	
	GlobalFree(hDevNames);
	GlobalFree(hDevMode);
}

 void CShiguOutlineDlg::PrintEveryPageInfo(CDC *pDC, CPrintInfo *pInfo)
{
	if(this->GetSafeHwnd() == NULL || !::IsWindow(this->GetSafeHwnd()))
		return;
	
	CPen pen,*pOldPen;
	CFont font,*poldfont;
		
	int nWidth=pDC->GetDeviceCaps(HORZRES);
	int nHeight=pDC->GetDeviceCaps(VERTRES);
	
	CString title=_T("行车事故概况");
	CRect DrawRect(100,100,nWidth,nHeight),rect,tempRect;
	CRect temp;
	
	pen.CreatePen(PS_SOLID,2,CLR_BLACK);
	font.CreateFont(-MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));
	
	pOldPen = pDC->SelectObject(&pen);
	poldfont = pDC->SelectObject(&font);
	
	//标题栏
	tempRect=DrawRect;
	pDC->DrawText(title,&tempRect,DT_CENTER|DT_TOP|DT_SINGLELINE);
	pDC->SelectObject(poldfont);
	font.DeleteObject();
	
	//框架
	DrawRect.top  = DrawRect.top - MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72) ;
	DrawRect.InflateRect(-50,0,-150,-150);
	//pDC->Rectangle(DrawRect);
	
	//选项信息	
    font.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));
	poldfont = pDC->SelectObject(&font);
	
	
	CString tempString;
	CSize tempSize;
	//打印车站名称和查询起始范围
	CString sectionname,date_start,date_end;

	int m_CurSection;
	 m_CurSection =m_nSectionID;

	sectionname = gpcdef.GetSectionName(m_CurSection);
	int nShiftDate = (m_nShiftID/1000000 - 1990) * 380 +
						(m_nShiftID/10000-(m_nShiftID/1000000)*100 - 1) * 31 + 
						m_nShiftID/100 - (m_nShiftID/10000)*100  - 1 ;
	CTime tempTime(long_time(nShiftDate ,18*3600)-WHOLE_DAY_SECOND);
	CTime tempTime2(long_time(nShiftDate,18*3600));

	date_start.Format("%d/%d/%d 18:00",tempTime.GetYear(),tempTime.GetMonth(),tempTime.GetDay());
	date_end.Format("%d/%d/%d 18:00",tempTime2.GetYear(),tempTime2.GetMonth(),tempTime2.GetDay());
		
	tempRect=DrawRect;
	tempRect.top = tempRect.top + 30;
	tempRect.bottom = tempRect.top + 180;
	tempRect.InflateRect(-60,0,60,0);

	tempString = "区间：" + sectionname;
	//tempString = "调度台：" + tempString;	
	pDC->DrawText(tempString,tempRect,DT_LEFT|DT_SINGLELINE|DT_VCENTER);

	tempString = "日期：" + date_start + "--" + date_end;
	tempRect.InflateRect(0,-150,0,150);

	pDC->DrawText(tempString,tempRect,DT_CENTER|DT_SINGLELINE|DT_VCENTER);

	DrawRect.top = tempRect.bottom;

	pDC->SelectObject(poldfont);
	pDC->SelectObject(pOldPen);
	font.DeleteObject();
	pen.DeleteObject();

    
	font.CreateFont(-MulDiv(10,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	poldfont = pDC->SelectObject(&font);

	DrawRect.top = tempRect.bottom + 10;
	 
	//书写内容
	//根据字体宽度、高度计算
	//每行最大字数及每页最大行数
	
	//取打印纸张高度和宽度
	int nPageHeight, nPageWidth;
	nPageHeight = pDC->GetDeviceCaps(VERTRES);
	nPageWidth = pDC->GetDeviceCaps(HORZRES);
	
	TEXTMETRIC TextM;
	pDC->GetTextMetrics(&TextM);
	//字体高度
	int m_LineHeight = (unsigned short)TextM.tmHeight;
	//字体平均宽度
	//int m_CharWidth=(unsigned short)
	//TextM.tmAveCharWidth;
	//每行最大字数
	//int m_MaxLineChar = nPageWidth / m_CharWidth - 8; 
	//每页最大行数
	int m_LinesPerPage = nPageHeight/ m_LineHeight; 
	tempRect = DrawRect;
    int offsset_y;
    //打印每项事故的序号和标题 ,内容;
	for(int i=0;i<nAccident;i++)
    {
		tempString.Format(" %d. %s",i+1,accident[i].subject );
		CString left; left.Format(" %d.");
		tempSize = pDC->GetTextExtent(left);
		//标题
        pDC->DrawText(tempString,tempRect,DT_LEFT|DT_TOP|DT_SINGLELINE);
        tempRect.InflateRect(0,-m_LineHeight,0,0);

		tempString.Format("%s",accident[i].detail);
		//内容
		temp = tempRect;
		tempRect.InflateRect(-tempSize.cx,0,0,0);
		//get offset_y;
        offsset_y = pDC->DrawText(tempString,tempRect,DT_CALCRECT );
        tempRect = temp;
		tempRect.InflateRect(-tempSize.cx,0,0,0);
		//draw the content of one record;
        pDC->DrawText(tempString,-1,tempRect,DT_WORDBREAK );

		temp.InflateRect(0,-offsset_y,0,0);
        tempRect = temp;
	}
    
	//签字
	tempRect.top = tempRect.bottom - 300;
	tempString.Format("        签字:");
	pDC->DrawText(tempString,tempRect,DT_CENTER|DT_SINGLELINE);	

	//打印页码
	tempRect.top = tempRect.bottom - 100;
	tempString.Format("第%d页 共%d页",pInfo->m_nCurPage,pInfo->GetMaxPage());
	pDC->DrawText(tempString,tempRect,DT_CENTER|DT_SINGLELINE);	

	pDC->SelectObject(poldfont);
	font.DeleteObject();
 

}
//读模板文件
bool  CShiguOutlineDlg:: ReadModuleFile(char *lpszFileName)
{//-1 is fail,0 is success
	////sAM->module_num = 0;//init
	////memset(sAM,0x00,sizeof(sAM));
	BOOL flag = TRUE;
	CFileFind m_Findfile;
	if(m_Findfile.FindFile(lpszFileName) == FALSE)
		return false;
	do{
		flag = m_Findfile.FindNextFile();
		CString filetitle;
		filetitle = m_Findfile.GetFilePath();

		char buffer[2048];
		memset(buffer,0x00,sizeof(buffer));
		char *ptr,*substr;
		int len;
		
		FILE *fp;
		fp = fopen((char *)(LPCTSTR)filetitle,"r");
		int i = 0,ch;
		if(fp == NULL)
		{
			sysprint(0,0,"can't open module file %s",(char *)(LPCTSTR)filetitle);
			return false;
		}
		//将所有的命令文件拷贝至str中
		ch = fgetc(fp);
		while(!feof(fp))
		{
			buffer[i] = (char)ch;
			ch = fgetc(fp);
			i++;
		}
		buffer[i] = '\0';
		fclose(fp);
		//将buffer给结构赋值
		ptr = buffer;
		if(substr = strstr(ptr,"@"))
		{
			len = substr - ptr;
			strncpy(sAM.details[sAM.module_num].no, ptr,len);
		}
		ptr = ptr + len + 1;
		if(substr = strstr(ptr,"@"))
		{
			len = substr - ptr;
			strncpy(sAM.details[sAM.module_num].type_name, ptr,len);
		}
		ptr += len + 1;
		if(substr = strstr(ptr,"@"))
		{
			len = substr - ptr;
			strncpy(sAM.details[sAM.module_num].module_content, ptr,len);
		}
		ptr += len + 1;
		sAM.module_num += 1;
	}while(flag);
	m_Findfile.Close();
	return true;
}
void CShiguOutlineDlg:: SortModuleFile()
{  
	for(int i = 0;i < sAM.module_num;i ++)
		for(int k = i; k < sAM.module_num;k++)
		{
			if(atoi(sAM.details[k].no) < atoi(sAM.details[i].no))
			{
				struct _AccidentModuleDetails temp;
				memcpy(&temp,&(sAM.details[k]),sizeof(sAM.details[k]));
				memcpy(&(sAM.details[k]),&(sAM.details[i]),sizeof(sAM.details[i]));
				memcpy(&(sAM.details[i]),&temp,sizeof(temp));
			}
		}
}

void CShiguOutlineDlg::OnSelendokCmbModule() 
{   
	int nIndex;
	nIndex = m_Ctrlmodule.GetCurSel();
	if(nIndex < 0 || nIndex >sAM.module_num) return;
    m_title = sAM.details[nIndex].type_name;
	m_detail = sAM.details[nIndex].module_content;

	UpdateData(FALSE);
}
 
void CShiguOutlineDlg::SendMsgAccidentOper(ushort nOpetype, ushort nIndex)
{
   	CLBTH::MsgAccidentOper mMsgAccidentOper;
	mMsgAccidentOper.con_id = m_nSectionID;
	
    mMsgAccidentOper.schd_type = schdtype;
	mMsgAccidentOper.shift_id = gpcdef.GetLoginShiftid();
	mMsgAccidentOper.index = nIndex;

	strncpy(mMsgAccidentOper.subject,m_title,m_title.GetLength());
	mMsgAccidentOper.note_len = m_detail.GetLength();
    strncpy(mMsgAccidentOper.note,m_detail,m_detail.GetLength());
 
	mMsgAccidentOper.oper = (CLBTH::MsgAccidentOper::AccidentOper)nOpetype;

	SendTGBMsg(&mMsgAccidentOper,0);
	GpcLogTrainMessage(PMTG.ThreadId,gpcdef.GetConsoleid(),"[ACCCID]: conid:%d,shift_id:%d,subject:%s",
		mMsgAccidentOper.con_id,
		mMsgAccidentOper.shift_id,
		mMsgAccidentOper.subject
		);
	
}