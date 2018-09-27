#include "stdafx.h"
#include "propertypage_shigu.h"
#include "utility/xml_utility.h"
#include "tgdocument.h"
#define  IDC_PSP_SHIGU  5112
#define WHOLE_DAY_SECOND 24*3600

extern CEdit* g_curEdit;//当前常用词汇要输入的编辑框

// 20090918 XIAN 下述变量太大，做全局变量
static   	ACCIDENT_INFO accident[MAX_ACCIDENT];
static      _AccidentModule sAM;
/////////////////////////////////////////////////////////////////////////////
// CShiguOutlineDlg dialog

CShiguOutlineDlg::CShiguOutlineDlg(CTgDocument   *pdoc)
	: CPropertyPage(CShiguOutlineDlg::IDD)
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
	m_RecordsPerPrintPage = 50;
	//}}AFX_DATA_INIT

	m_pNotifyWindow=NULL;
	m_pDoc=pdoc;

	m_nSchdtype=m_pDoc->GetScheduleType();
	m_nSectionID=m_pDoc->GetCurrentSectionNo();
	m_nShiftID=m_pDoc->GetCurrentShiftID();
	m_xmlcontent="";
}

CShiguOutlineDlg::~CShiguOutlineDlg()
{
	if(NULL != m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
}

void CShiguOutlineDlg::OnDestroy()
{
	if(NULL != m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
	CPropertyPage::OnDestroy();
}

void CShiguOutlineDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShiguOutlineDlg)
	//DDX_Control(pDX, IDC_CMB_MODULE, m_Ctrlmodule);
	DDX_Control(pDX, IDC_EDIT_DETAIL, m_EditDetail);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CShiguOutlineDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CShiguOutlineDlg)
	ON_BN_CLICKED(IDC_ADD, OnAddNewDetail)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_CBN_SELENDOK(IDC_CMB_MODULE, OnSelendokCmbModule)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////
struct PropertyInfoPara  g_shigu_property[]
={
//	{0,0,CTC_DATA_STR,"","事故概况信息"},
	{2,0,CTC_DATA_STR,"kscz","开始车站"},
	{2,0,CTC_DATA_STR,"jscz","结束车站"},
	{2,0,CTC_DATA_STR,"wzkm","位置(公里)"},
	{2,0,CTC_DATA_STR,"wzm","位置(米)"},
	{2,0,CTC_DATA_STR,"pd","坡道千分率"},
	{2,0,CTC_DATA_STR,"qx","曲线半径(米)"},
	{2,0,CTC_DATA_STR,"cch","车次号"},
	{2,0,CTC_DATA_STR,"lclx","列车类型"},
	{2,0,CTC_DATA_STR,"tq","天气"},
	{2,0,CTC_DATA_STR,"jcxh","机车型号"},
	{2,0,CTC_DATA_STR,"jch","机车号"},
	{3,0,CTC_DATA_STR,"bjxh","补机型号"},
	{2,0,CTC_DATA_STR,"bjjch","补机机车号"},
	{2,0,CTC_DATA_STR,"qyds","牵引定数"},
	{2,0,CTC_DATA_STR,"ssd","所属段"},
	{2,0,CTC_DATA_STR,"xccls","现车辆数"},
	{2,0,CTC_DATA_STR,"xczz","现车总重"},
	{2,0,CTC_DATA_STR,"xcjc","现车计长"},
	{2,0,CTC_DATA_STR,"fssj","发生时间"},
	{2,0,CTC_DATA_STR,"fjsx","复旧上行(单)"},
	{2,0,CTC_DATA_STR,"fjxx","复旧下行"},
	{2,0,CTC_DATA_STR,"ktsx","开通上行(单)线路"},
	{2,0,CTC_DATA_STR,"ktxx","开通下行线路"},
	{2,0,CTC_DATA_STR,"zdzxsx","中断正线上行(单)"},
	{2,0,CTC_DATA_STR,"zdzxxx","中断正线下行"},
	{2,0,CTC_DATA_STR,"sj","司机"},
	{2,0,CTC_DATA_STR,"fsj","副司机"},
	{2,0,CTC_DATA_STR,"yzcz","运转车长"},
	{2,0,CTC_DATA_STR,"lcz","列车长"},
	{2,0,CTC_DATA_STR,"zby","值班员"},
	{2,0,CTC_DATA_STR,"xhy","信号员"},
	{2,0,CTC_DATA_STR,"xhg","信号工"},
	{2,0,CTC_DATA_STR,"dcz","调车长"},
	{2,0,CTC_DATA_STR,"xlg","线路工"},
	{2,0,CTC_DATA_STR,"jcy","检车员"},
	{2,0,CTC_DATA_STR,"xz","性质"},
	{2,0,CTC_DATA_STR,"xm","项目"},
	{2,0,CTC_DATA_STR,"yylx","原因类型"}
};

#define SHIGU_PROPERTY_NUM  sizeof(g_shigu_property)/sizeof(g_shigu_property[0])

////////////// COMM  消息
LRESULT CShiguOutlineDlg::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	 UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	 CLBTH::MsgAccidentOper *MsgAccidentOper= ( CLBTH::MsgAccidentOper *)pMsgStruct->pmsg;
	ProcessMessage(MsgAccidentOper);
	 FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	 return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CShiguOutlineDlg message handlers

BOOL CShiguOutlineDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	TCHAR title[2][20]={_T("序号"), _T("   标题  ")};
	// to Init and show gird ctrl in dialog;
    if(m_pGridCtrl!=NULL)
    	return FALSE;
	m_pGridCtrl=new CGridCtrl;
	if(!m_pGridCtrl) 
		return FALSE;
 
    //set grid frame border in dialog;
 	CRect rc;
	GetDlgItem(IDC_FRAME)->GetWindowRect(rc);
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
   
	CRect  pspRect;
	GetDlgItem(IDC_FRM_PROPERTY)->GetWindowRect(&pspRect);
	ScreenToClient(&pspRect);
	
	m_pspCtrl.CreateVerticalGrid(this,pspRect,IDC_PSP_SHIGU,g_shigu_property,SHIGU_PROPERTY_NUM);
	m_pspCtrl.ExpandColumnsToFit(FALSE);
	m_pspCtrl.SetEditable(TRUE);
	

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
   // m_Ctrlmodule.SetCurSel(0);
	OnSelendokCmbModule();
	Query_Accident();
    InitIdentify();
	InitDlgPosition(this);
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgAccidentOper,0);
	InformParent();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CShiguOutlineDlg::InitIdentify()
{
	if(m_pDoc->IsQueryMode())  // 权限未考虑
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
   m_xmlcontent="";
   UpdateData(false);
   nAccident =0;
   m_nRows =1;
  	CurrentRow = -1;

   Query_Accident();

}


void CShiguOutlineDlg::Query_Accident()
{   
	CLBTH::MsgAccidentQuery mMsgAccidentQuery;
	mMsgAccidentQuery.con_id = m_nSectionID;
	mMsgAccidentQuery.schd_type = m_nSchdtype;
	mMsgAccidentQuery.shift_id = m_nShiftID;

	CLBTH::Archive ar( CLBTH::Archive::store);
	mMsgAccidentQuery.serialize(ar);
	
	long entity_id=GetSpecialEntityID(SERVER_TYPE_DB,mMsgAccidentQuery.con_id);
	if(!SendTGBMsg(0,&mMsgAccidentQuery,entity_id))
		AfxMessageBox("无法获取服务器ID");
}

BOOL CShiguOutlineDlg::ProcessMessage(CLBTH::MsgAccidentOper *pBuf)
{  
	do{
		if(m_nSchdtype != pBuf->schd_type || pBuf->oper != CLBTH::MsgAccidentOper::ACCIDENT_SERVER_ECHO)
         break;
      if(m_nShiftID!= pBuf->shift_id)
		  break;

	  nAccident +=1;
      accident[nAccident-1].index = pBuf->index; //index
	  memset(accident[nAccident-1].subject,0,sizeof(accident[nAccident-1].subject));
      strncpy(accident[nAccident-1].subject,pBuf->subject,sizeof(pBuf->subject)-1);// subject 
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
		return CPropertyPage::OnNotify(wParam, lParam, pResult);
	
    NM_GRIDVIEW *nmgv=(NM_GRIDVIEW*)  lParam;;
	int column=nmgv->iColumn;
	CurrentRow=nmgv->iRow ;  //CurrentRow

	if(CurrentRow>=1)
	{
		m_title.Format("%s",accident[CurrentRow-1].subject); 
        m_xmlcontent=accident[CurrentRow-1].detail; 
		m_pspCtrl.InitVerticalGrid(m_xmlcontent,"Accident");
		CString tmpstr;

		GetDlgItem(IDC_EDIT_TITLE)->SetWindowText(m_title); 
		GetXMLNodeValue(tmpstr,m_xmlcontent,"Accident/DETAIL");
		GetDlgItem(IDC_EDIT_DETAIL)->SetWindowText(tmpstr); 
		GetXMLNodeValue(tmpstr,m_xmlcontent,"Accident/REASON");
		GetDlgItem(IDC_EDIT_REASON)->SetWindowText(tmpstr);
		GetXMLNodeValue(tmpstr,m_xmlcontent,"Accident/EQUIP");
		GetDlgItem(IDC_EDIT_EQUIP_BAD)->SetWindowText(tmpstr);
		GetXMLNodeValue(tmpstr,m_xmlcontent,"Accident/RESCUE");
		GetDlgItem(IDC_EDIT_RESCUE_INFO)->SetWindowText(tmpstr);
		GetXMLNodeValue(tmpstr,m_xmlcontent,"Accident/MANUNIT");
		GetDlgItem(IDC_EDIT_MANAGE_UNIT)->SetWindowText(tmpstr);

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
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
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
		int len=0;
		
		FILE *fp;
		fp = fopen((char *)(LPCTSTR)filetitle,"r");
		int i = 0,ch;
		if(fp == NULL)
		{
			m_Findfile.Close();
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
	/*
	int nIndex;
	nIndex = m_Ctrlmodule.GetCurSel();
	if(nIndex < 0 || nIndex >sAM.module_num) return;
    m_title = sAM.details[nIndex].type_name;
	m_detail = sAM.details[nIndex].module_content;

	UpdateData(FALSE);*/
}
 
void CShiguOutlineDlg::SendMsgAccidentOper(ushort nOpetype, ushort nIndex)
{
	CString tmpstr;
	GetDlgItem(IDC_EDIT_TITLE)->GetWindowText(tmpstr);
	if(tmpstr.IsEmpty())
	{
		MessageBox("请输入标题!","提示");
		return;
	}
	CString xml;
	
	xml="<Accident>";
	xml+="<TITLE>";	xml+=tmpstr;	xml+="</TITLE>";
	
	GetDlgItem(IDC_EDIT_DETAIL)->GetWindowText(tmpstr);
	if(!tmpstr.IsEmpty())
	{
		xml+="<DETAIL>";	xml+=tmpstr;	xml+="</DETAIL>";
	}
	GetDlgItem(IDC_EDIT_REASON)->GetWindowText(tmpstr);
	if(!tmpstr.IsEmpty())
	{
		xml+="<REASON>";	xml+=tmpstr;	xml+="</REASON>";
	}
	GetDlgItem(IDC_EDIT_EQUIP_BAD)->GetWindowText(tmpstr);
	if(!tmpstr.IsEmpty())
	{
		xml+="<EQUIP>";	xml+=tmpstr;	xml+="</EQUIP>";
	}
	GetDlgItem(IDC_EDIT_RESCUE_INFO)->GetWindowText(tmpstr);
	if(!tmpstr.IsEmpty())
	{
		xml+="<RESCUE>";	xml+=tmpstr;	xml+="</RESCUE>";
	}
	GetDlgItem(IDC_EDIT_MANAGE_UNIT)->GetWindowText(tmpstr);
	if(!tmpstr.IsEmpty())
	{
		xml+="<MANUNIT>";	xml+=tmpstr;	xml+="</MANUNIT>";
	}
	
	m_pspCtrl.SaveToXML(xml,NULL,NULL);
	xml+="</Accident>";
   	CLBTH::MsgAccidentOper mMsgAccidentOper;
	mMsgAccidentOper.con_id = m_nSectionID;
	
    mMsgAccidentOper.schd_type = m_nSchdtype;
	mMsgAccidentOper.shift_id = m_nShiftID;
	mMsgAccidentOper.index = nIndex;
	GetDlgItem(IDC_EDIT_TITLE)->GetWindowText(m_title); 
	strncpy(mMsgAccidentOper.subject,m_title,m_title.GetLength());
	int len=xml.GetLength();
	if(len>sizeof(mMsgAccidentOper.note)-1)
		len=sizeof(mMsgAccidentOper.note)-1;
	mMsgAccidentOper.note_len =len ;
    strncpy(mMsgAccidentOper.note,xml,len);
	mMsgAccidentOper.note[len]=0;
	mMsgAccidentOper.oper = (CLBTH::MsgAccidentOper::AccidentOper)nOpetype;


	GpcLogTrainMessage(PMTG.ThreadId,gpcdef.GetConsoleid(),"[ACCCID]: conid:%d,shift_id:%d,subject:%s",
		mMsgAccidentOper.con_id,
		mMsgAccidentOper.shift_id,
		mMsgAccidentOper.subject
		);

	SendTGBMsg(mMsgAccidentOper.con_id,&mMsgAccidentOper,0);	
	
}
void CShiguOutlineDlg::InformParent()
{
	TRAIN_SELECT_PARA para;
	CTG_ScheduleInfo schd;
	m_pDoc->GetScheduleInfo( schd);
	TIME shift_start,shift_end;
	schd.GetShiftManagerRange(shift_start,shift_end); 
	para.starttime= shift_start;
	para.endtime= shift_end;
	para.list_type=0; 
	if(m_pNotifyWindow)
		m_pNotifyWindow->SendMessage(UM_TG_TRAIN_SELECTED,(WPARAM)0,(LPARAM)&para);
}