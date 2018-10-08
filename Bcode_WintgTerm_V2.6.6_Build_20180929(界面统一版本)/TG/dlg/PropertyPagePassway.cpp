// PropertyPagePassway.cpp : implementation file
//

#include "stdafx.h"
//#include "wintg.h"
#include "PropertyPagePassway.h"
#include "DlgPasswayInsert.h"
#include "DlgMessageBox.h"
#include "widgets.h"                 //UM 类的消息定义
#include "Tooltipwnd.h"              //提示类
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
byte         ddtid;               //调度台id 区段num
byte          ddtid_default;       //缺省的调度台

#define ITEMNUM  7

PASS_WAY_UTILITY    utility[100];      //径路管理数据
PASS_WAY_INFO       info[100];         //径路数据
PASS_WAY_STA        sta[100];          //径路车站数据
int ActivePage;   //当前的活动页
int utility_count;   //径路范围数据
int passway_count;   //径路数目
int station_count;   //车站数目

short analysis_train(short len, const char*point, short prefix_len, char* prefix, ulong *number);

/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayAppoint property page

IMPLEMENT_DYNCREATE(CPropertyPagePasswayAppoint, CPropertyPage)

CPropertyPagePasswayAppoint::CPropertyPagePasswayAppoint() : CPropertyPage(CPropertyPagePasswayAppoint::IDD)
{
	//{{AFX_DATA_INIT(CPropertyPagePasswayAppoint)
    m_nFixCols = 1;
	m_nFixRows = 1;
	m_nCols = 7;
	m_nRows = 1;
    m_pGridCtrl = NULL;
	col1=col2=col3=col4=col5=col6=	FALSE;
	Flag=FALSE;
	Flag_moredata=FALSE;
    ndialogInited=0;
	//}}AFX_DATA_INIT
}

CPropertyPagePasswayAppoint::~CPropertyPagePasswayAppoint()
{
	if(m_pGridCtrl)
	    delete m_pGridCtrl;
}

void CPropertyPagePasswayAppoint::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPagePasswayAppoint)
	DDX_Control(pDX, IDC_STATIC1, m_Frame1);
	DDX_Control(pDX, IDC_SAVE, m_saveCtrl);
	DDX_Control(pDX, IDC_NEW, m_newCtrl);
	DDX_Control(pDX, IDC_DELETE, m_deleteCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPagePasswayAppoint, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertyPagePasswayAppoint)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GRID, OnGridItemChanged)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID, OnGridStartEdit)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit) //LVN_ITEMCHANGING
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
    ON_MESSAGE(UM_DDT_CHANGED_MSG,OnDdtChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayAppoint message handlers
/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayManagement property page

IMPLEMENT_DYNCREATE(CPropertyPagePasswayManagement, CPropertyPage)

CPropertyPagePasswayManagement::CPropertyPagePasswayManagement() : CPropertyPage(CPropertyPagePasswayManagement::IDD)
{
	//{{AFX_DATA_INIT(CPropertyPagePasswayManagement)
		// NOTE: the ClassWizard will add member initialization here
    m_nFixCols_1 = 1;
	m_nFixRows_1 = 1;
	m_nCols_1 = 3;
	m_nRows_1 = 1;
    m_pGridCtrl_1 = NULL;
 
    m_nFixCols_2 = 1;
	m_nFixRows_2 = 1;
	m_nCols_2 = 6;
	m_nRows_2 = 2;
    m_pGridCtrl_2 = NULL;
    s_col2=s_col3=s_col4=s_col5=FALSE;
    Flag_sta=FALSE;
	Flag_info=FALSE;
    p_col1=p_col2=FALSE;
	Flag_moredata=TRUE;
	//}}AFX_DATA_INIT
}

CPropertyPagePasswayManagement::~CPropertyPagePasswayManagement()
{
	if(m_pGridCtrl_1)
	    delete m_pGridCtrl_1;
    if(m_pGridCtrl_2)
	    delete m_pGridCtrl_2;
}

void CPropertyPagePasswayManagement::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPagePasswayManagement)
	DDX_Control(pDX, IDC_STATIC2, m_Frame2);
	DDX_Control(pDX, IDC_STATIC1, m_Frame1);
	DDX_Control(pDX, IDC_STATION_SAVE, m_s_saveCtrl);
	DDX_Control(pDX, IDC_STATION_NEW, m_s_newCtrl);
	DDX_Control(pDX, IDC_STATION_DELETE, m_s_deleteCtrl);
	DDX_Control(pDX, IDC_SAVE, m_saveCtrl);
	DDX_Control(pDX, IDC_DELETE, m_deleteCtrl);
	DDX_Control(pDX, IDC_NEW, m_newCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPagePasswayManagement, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertyPagePasswayManagement)
	ON_BN_CLICKED(IDC_NEW, OnPassWayNew)
	ON_BN_CLICKED(IDC_DELETE, OnPassWayDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GRID, OnGridItemChanged)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID, OnGridStartEdit)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit) //LVN_ITEMCHANGING
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GRID2, OnGrid2ItemChanged)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID2, OnGrid2StartEdit)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID2, OnGrid2EndEdit) //LVN_ITEMCHANGING
	ON_BN_CLICKED(IDC_SAVE, OnPassWaySave)
	ON_BN_CLICKED(IDC_STATION_DELETE, OnStationDelete)
	ON_BN_CLICKED(IDC_STATION_NEW, OnStationNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayManagement message handlers
/////////////////////////////////////////////////////////////////////////////
// CPropertySheetPassway

IMPLEMENT_DYNAMIC(CPropertySheetPassway, CPropertySheet)

CPropertySheetPassway::CPropertySheetPassway(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage,CWintgDoc *pDoc)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSP_USEHICON;
  	m_psh.dwFlags &= ~PSH_HASHELP;  // Lose the Help button
  	m_psh.dwFlags |= PSH_NOAPPLYNOW;    // Lose the Apply Now button
 	AddPage(&Page2);
	AddPage(&Page1);
    m_pDoc=pDoc;
}

CPropertySheetPassway::CPropertySheetPassway(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage,CWintgDoc *pDoc)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSP_USEHICON;
  	m_psh.dwFlags &= ~PSH_HASHELP;  // Lose the Help button
  	m_psh.dwFlags |= PSH_NOAPPLYNOW;    // Lose the Apply Now button
 	AddPage(&Page2);
	AddPage(&Page1);
    m_pDoc=pDoc;
}

CPropertySheetPassway::~CPropertySheetPassway()
{
	if(Page1)
	delete Page1;
	if(Page2)
	delete Page2;
    	
}


BEGIN_MESSAGE_MAP(CPropertySheetPassway, CPropertySheet)
	//{{AFX_MSG_MAP(CPropertySheetPassway)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_MESSAGE(UM_PASSWAY_UTILITY,OnPassWayUtility)
    ON_MESSAGE(UM_PASSWAY_INFO,OnPassWayInfo)
	ON_MESSAGE(UM_PASSWAY_STATION,OnPassWayStation)
	ON_CBN_SELENDOK(IDC_COMBO_CONSOLEID, OnSelendokComboDdtname)////////////////////
		//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetPassway message handlers

void CPropertyPagePasswayAppoint::OnNew() 
{  
	if(!gpcdef.GetOperatorRight(WINTG_EDIT))
       return;
   if(Flag)    
   {   Flag=FALSE;
	   InitCell(utility_count+1);  //初始化cell  显示序号
    
       col1 = col2 = col3 = col4 = col5 = col6=FALSE;
       return;
   }
   if( col1 & col2 & col3 & col4 & col5 & col6)
   {
     
	   InitCell(utility_count+1);  //初始化cell  显示序号
       col1 = col2 = col3 = col4 = col5 = col6=FALSE;
   }
  

}

void CPropertyPagePasswayAppoint::OnGridItemChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
                         
}

void CPropertyPagePasswayAppoint::OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{   
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    CGridCellCombo *pCell;

  	options4.RemoveAll();
    for(int i=0;i<passway_count;i++)                //passwayname 列表
    options4.Add(_T(info[i].Pass_way_name));

	CString content1;
    content1=m_pGridCtrl->GetItemText(pItem->iRow,0);   //获得输入内容
    
	switch(pItem->iColumn)
	{
	case 1:
		break;
	case 2:
		  
		break;
	case 3:
	     m_pGridCtrl->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,3);
		  pCell->SetOptions(options3);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
  
			break;
    case 4:
	     m_pGridCtrl->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,4);
		  pCell->SetOptions(options4);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;

	case 5:
	     m_pGridCtrl->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,5);
		  pCell->SetOptions(options5);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
	case 6:
	     m_pGridCtrl->SetCellType(pItem->iRow ,6, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,6);
		  pCell->SetOptions(options6);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
	}
 

}

void CPropertyPagePasswayAppoint::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;   //获得当前的行和列数的结构
  
	CString content,content1;
	char  point[30];byte result;
    int length;

    content1=m_pGridCtrl->GetItemText(pItem->iRow,0);   //获得输入内容
    if(atoi(content1)<=0) 
	{ *pResult = (1)? -1 : 0;
	  return;
	}

    content=m_pGridCtrl->GetItemText(pItem->iRow,pItem->iColumn);   //获得输入内容
	strcpy(point,content);	
	length=strlen(point);
 
	switch(pItem->iColumn)
	{
	case 1:   
		if (FALSE == analysis_train(length, point, 
			sizeof(utility[pItem->iRow-1].PREFIX), utility[pItem->iRow-1].PREFIX, 
			&(utility[pItem->iRow -1].START_TRAIN_NUM)))
		{
			break;
		}
	     
		col1=TRUE;
		break;

	case 2:  		
		content1=m_pGridCtrl->GetItemText(pItem->iRow,1);   //获得输入内容
		if(content1=="")
		{ 	 *pResult = (1)? -1 : 0;   
		break;
		}
		{
			char tmp[128];

			if (FALSE == analysis_train(length, point, 
				sizeof(utility[pItem->iRow-1].PREFIX), tmp, 
				&(utility[pItem->iRow -1].END_TRAIN_NUM)))
			{
				break;
			}
			if (strcmp(utility[pItem->iRow-1].PREFIX, tmp))
			{
				AfxMessageBox("车次号前缀应相同");
				break;
			}
		}
		col2=TRUE;
		break;

	case 3:   content1=m_pGridCtrl->GetItemText(pItem->iRow,2);   //获得输入内容
		     if(content1=="")
			 { 	 *pResult = (1)? -1 : 0;   
			     break;
			 }
	 
		result=(content=="上行" ? 0: 1);
        utility[pItem->iRow-1].DIRECTIONS=result;  //输入内容
	    m_pGridCtrl->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCell)); 
		col3=TRUE;
		break;
    case 4:  content1=m_pGridCtrl->GetItemText(pItem->iRow,3);   //获得输入内容
		     if(content1=="")
			 { 	 *pResult = (1)? -1 : 0;   
			     break;
			 }
        utility[pItem->iRow -1].PASS_WAY_ID=Get_Passwayid(point) ;  //输入内容
	    m_pGridCtrl->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCell)); 
		col4=TRUE;
		break;
	case 5:  content1=m_pGridCtrl->GetItemText(pItem->iRow,4);   //获得输入内容
		     if(content1=="")
			 { 	 *pResult = (1)? -1 : 0;   
			     break;
			 }
	    result=(content=="始发" ? 1: 2);
		//content.Format("%d",result);AfxMessageBox(content);
        utility[pItem->iRow-1].ACPT_FLAG=result;  //输入内容
		 m_pGridCtrl->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCell)); 
        col5=TRUE;
		break;
	case 6: content1=m_pGridCtrl->GetItemText(pItem->iRow,5);   //获得输入内容
		     if(content1=="")
			 { 	 *pResult = (1)? -1 : 0;   
			     break;
			 }
        result=(content=="终到" ? 3: 4);
        utility[pItem->iRow-1].GVT_FLAG=result;  //输入内容
	  	m_pGridCtrl->SetCellType(pItem->iRow ,6, RUNTIME_CLASS(CGridCell)); 
        col6=TRUE;
		break;
	default:break;
	}
    //m_pGridCtrl->SetItemBkColour(pItem->iRow , pItem->iColumn , RGB(0xFF,0xFF,0x00));    
	if(pItem->iRow >utility_count+1)  
	 { 
		 *pResult = (1)? -1 : 0;                       //禁止编辑有效 
         m_pGridCtrl->SetItemBkColour(pItem->iRow , pItem->iColumn , RGB(0xFF,0xFF,0xFF));
	     return;
	 }
	if( col1 & col2 & col3 & col4 & col5 & col6)
	  utility_count+=1;
 
}
//1 径路信息列表
LRESULT CPropertySheetPassway::OnPassWayInfo(WPARAM wParam,LPARAM lParam)
{ 
 
	if(!Page2.Flag_moredata)
		return  TRUE;                  //没有多余数据返回
    char *ptr=(char *)lParam;
	byte more_data,ddt_id_t;
	ushort count;              
    CString tempstr,str1;
//    char passway_name[20];
 
    more_data =*ptr;  //more data?
	Page2.Flag_moredata=more_data ? 1:0;
	ptr+=1;     

    ddt_id_t=*ptr;    //ddt_id  
     
	ptr+=1;
    Page2.Flag_info =TRUE;

    count=NettoHostUShort(*(USHORT *)ptr);          //count
	if(count<=0) return TRUE;
	passway_count+=count;
 
	ptr+=2;
    
	for(int i=passway_count-count;i<passway_count;i++)
	{
		info[i].Pass_way_id=NettoHostUShort(*(USHORT *)(ptr));            //passway_id
		ptr+=2;
		
		strncpy(info[i].Pass_way_name, ptr, sizeof(info[i].Pass_way_name)-1);  
		ptr+=40;
		
		info[i].Begin_sta=NettoHostUShort(*(USHORT *)(ptr));            //start starion //
	 
		ptr+=2;
		
		info[i].End_sta=NettoHostUShort(*(USHORT *)(ptr));            //end station   //
	 
		ptr+=2;
	}

 
    Page2.Show_Passway_Info();
 
	return TRUE;

}
 
//2 单条径路明细
LRESULT CPropertySheetPassway::OnPassWayStation(WPARAM wParam,LPARAM lParam)
{ 
    char *ptr=(char *)lParam;
	byte ddt_id_t;
	ushort count;              
    CString tempstr,str1;
     
    ddt_id_t= *ptr;                                  //ddt_id  
    if(ddt_id_t!=ddtid) return  TRUE;
	ptr+=1;
    Page2.Flag_sta=TRUE;                     

    int Pass_way_id=NettoHostUShort(*(USHORT *)(ptr));            //passway_id
	ptr+=2;

    count=NettoHostUShort(*(USHORT *)ptr);                        //count
	if(count<=0) return TRUE;
	ptr+=2;
    station_count=count;     

	for(int i=0;i<count;i++)
	{
		sta[i].Pass_way_id=Pass_way_id;                                  //passway_id

		sta[i].Sta_lst_idx=NettoHostUShort(*(USHORT *)(ptr));            //sta_idx  
		ptr+=2;
		
        sta[i].Station=NettoHostUShort(*(USHORT *)(ptr));                //sta_no 
		ptr+=2;

		sta[i].Entry= *ptr;                                             //entry / 
		ptr+=1;
		
        sta[i].Exit=*ptr;            //exit 
		ptr+=1;
	}

     
    Page2.Show_StationInfo();
    
    return TRUE;
}

//3 指定径路信息
LRESULT CPropertySheetPassway::OnPassWayUtility(WPARAM wParam,LPARAM lParam)
{   
	if(!Page1.Flag_moredata )
		return  TRUE;

    char *ptr=(char *)lParam;
	byte more_data,ddt_id_t;
	ushort count;              
 
    more_data =*ptr;  //more data?
	Page1.Flag_moredata=more_data ? 1:0;
	ptr+=1;     

    ddt_id_t=*ptr;                                 //ddt_id *********  
	ptr+=1;
    
	Page1.Flag =TRUE;

    count=NettoHostUShort(*(USHORT *)ptr);          //count
	if(count<=0) return TRUE;

	utility_count+=count;
	ptr+=2;
    
	for(int i=utility_count-count;i<utility_count;i++)
	{
		strncpy(utility[i].PREFIX, ptr, 9);   
		ptr+=10;
		
		utility[i].START_TRAIN_NUM=NettoHostULong(*(unsigned long  *)(ptr));  //start starion //
		ptr+=4;

	    utility[i].END_TRAIN_NUM=NettoHostULong(*(unsigned long  *)(ptr));  //end  starion //
		ptr+=4;
		
		utility[i].Station=NettoHostUShort(*(USHORT *)(ptr));
		ptr+=2;

		utility[i].Entry=*ptr;    ptr++;
		utility[i].Exit=*ptr;     ptr++;

		utility[i].DIRECTIONS=*ptr;                                               //direction   //
		ptr+=1;
        
	    utility[i].PASS_WAY_ID=NettoHostUShort(*(USHORT *)(ptr));                 //passway_id  //
		ptr+=2;

	    utility[i].ACPT_FLAG=*ptr;                                                 //acpt_flag  //
		ptr+=1;

        utility[i].GVT_FLAG=*ptr;                                                  //gvt_flag  //
		ptr+=1;

	}

    Page1.m_pGridCtrl->SetRedraw (FALSE);   
    for(int j=0;j<utility_count;j++)
      Page1.Show_Utility(j,j+1);
    Page1.m_pGridCtrl->SetRedraw (TRUE);   
    Page1.m_pGridCtrl->Invalidate();   
    
	return TRUE;
}
void CPropertySheetPassway::OnSelendokComboDdtname() 
{ 
	CString sName;                   //区段名称
    int num=m_Combo1.GetCurSel();
    int n = m_Combo1.GetLBTextLen(num);
    m_Combo1.GetLBText( num, sName.GetBuffer(n) );

    char *  ddt[20];         
    int nSectionNum,nSectionList[30];
	nSectionNum = sizeof(nSectionList) / sizeof(nSectionList[0]);
	gpcdef.GetSectionList(nSectionNum,nSectionList);
	for (int i = 0; i < nSectionNum; i++)   
	{ 
	    ddt[i] = gpcdef.GetSectionName(nSectionList[i]);
	    if(ddt[i]==sName)
	      ddtid=nSectionList[i];  //获得当前选定的调度台
	}
 
 
	Page2.LoadPassWayInfo();             //

	if(ActivePage==1){
		Page1.LoadUtility(ddtid);
	}

	if(ddtid_default!=ddtid)
	{  
		if(ActivePage==1)
		{   
			Page1.m_newCtrl.EnableWindow(FALSE);
			Page1.m_deleteCtrl.EnableWindow(FALSE);
			Page1.m_saveCtrl.EnableWindow(FALSE);
		}
		Page2.m_newCtrl.EnableWindow(FALSE);
		Page2.m_deleteCtrl.EnableWindow(FALSE);
		Page2.m_saveCtrl.EnableWindow(FALSE);
		Page2.m_s_newCtrl.EnableWindow(FALSE);
		Page2.m_s_deleteCtrl.EnableWindow(FALSE);
		Page2.m_s_saveCtrl.EnableWindow(FALSE);
	}
	else
	{   
		if(ActivePage==1)
		{
			Page1.m_newCtrl.EnableWindow(TRUE);
			Page1.m_deleteCtrl.EnableWindow(TRUE);
			Page1.m_saveCtrl.EnableWindow(TRUE);
		}
		Page2.m_newCtrl.EnableWindow(TRUE);
		Page2.m_deleteCtrl.EnableWindow(TRUE);
		Page2.m_saveCtrl.EnableWindow(TRUE);
		Page2.m_s_newCtrl.EnableWindow(TRUE);
		Page2.m_s_deleteCtrl.EnableWindow(TRUE);
		Page2.m_s_saveCtrl.EnableWindow(TRUE);
		
	}

	Page2.OnChangeSection(ddtid);
    
	if(Page1.ndialogInited == 1)
	    Page1.SendMessage(UM_DDT_CHANGED_MSG);

}
BOOL CPropertySheetPassway::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
 
 	CRect rectSheet;
	GetWindowRect(rectSheet);
	rectSheet.right+=130;
	rectSheet.bottom+=-30;
    MoveWindow(rectSheet);
   
    CRect TempRect(0,0,100,100);
	m_Combo1.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_SORT | 
		WS_BORDER | WS_VSCROLL,TempRect,this,IDC_COMBO_CONSOLEID );
     
    ddtid=m_pDoc->m_nCurrentSectionNo;  
    ddtid_default=ddtid;

    char *  ddt[20];    //调度台名字     
    int nSectionNum,nSectionList[30];
	nSectionNum = sizeof(nSectionList) / sizeof(nSectionList[0]); //区段的数目
	gpcdef.GetSectionList(nSectionNum,nSectionList);	//区段列表
	for (int i = 0; i < nSectionNum; i++)//依次显示区段名
	{
	     ddt[i] = gpcdef.GetSectionName(nSectionList[i]);
		 m_Combo1.InsertString (i,_T(ddt[i]) ); //插入 
		 if(ddtid_default==nSectionList[i])
	     m_Combo1.SetCurSel(i);//显示默认区段
	}	
     

    m_Static1.Create("   请选择行调台 ",WS_CHILD | WS_VISIBLE | SS_LEFT,
		 TempRect,this,IDC_STATIC_CONSOLEID);
    CWnd * pStatic=GetDlgItem(IDC_STATIC_CONSOLEID);
	CRect rectStatic;
	pStatic->GetWindowRect(rectStatic);
      
    CPropertyPage * pPage=GetPage(0);
	CRect rectPage;
	pPage->GetWindowRect(rectPage);
    //行调台1
	rectStatic.top=rectPage.top+60;
	rectStatic.bottom=rectStatic.top+20;
	rectStatic.left=rectPage.right+10;
	rectStatic.right=rectStatic.left+130;
	ScreenToClient(rectStatic);
	pStatic->MoveWindow(rectStatic);
    
	//combox2
    CWnd * pCombo=GetDlgItem(IDC_COMBO_CONSOLEID); //IDC_COMBO_CONSOLEID
	if(pCombo==NULL)  
		return FALSE;
	CRect rectCombo;
	pCombo->GetWindowRect(rectCombo);
      
	rectCombo.top=rectPage.top+90;
	rectCombo.bottom=rectCombo.top+60;
	rectCombo.left=rectPage.right+20;
	rectCombo.right=rectCombo.left+100;
	ScreenToClient(rectCombo);
	pCombo->MoveWindow(rectCombo);
    
	//返回按钮3
    CWnd *pbtnOk=GetDlgItem(IDOK);
	ASSERT(pbtnOk);
	CRect rectOkBtn;
	pbtnOk->GetWindowRect(rectOkBtn);

	int cx=rectOkBtn.Width();
	int cy=rectOkBtn.Height();
	rectOkBtn.top=rectPage.top+350;  //rectCombo.bottom
	rectOkBtn.bottom=rectOkBtn.top+cy+10;
	rectOkBtn.left=rectPage.right+30;
	rectOkBtn.right=rectOkBtn.left+cx+10;
	ScreenToClient(rectOkBtn);
	pbtnOk->MoveWindow(rectOkBtn);

	int ids[]={ID_APPLY_NOW, IDCANCEL, IDOK, IDHELP};
	for( i=0;i<sizeof ids / sizeof ids[0];i++)
	{
      CWnd * pWnd=GetDlgItem(ids[i]);
	  if(pWnd==NULL)
        break;
      switch(i)
	  {
	  case 0:
		     //pWnd->SetWindowText(_T("保存"));
			 //pWnd->EnableWindow(TRUE);
             pWnd->ShowWindow(FALSE);
			 break;
	  case 1:
		     //pWnd->SetWindowText(_T("取消"));
             pWnd->ShowWindow(FALSE);
			 break;

	  case 2:
             pWnd->SetWindowText(_T("返回"));
			 break;

	  case 3:
		     pWnd->ShowWindow(FALSE);
		     break;
	  default:break;
	  }// switch
	}// for
    
	InitDlgPosition(this);
     
	Page2.LoadPassWayInfo();   //请求本台已有的径路
	return bResult;
}

BOOL CPropertyPagePasswayAppoint::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
   
	if(!gpcdef.GetOperatorRight(WINTG_EDIT))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_NEW);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_DELETE);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_SAVE);
		pWnd->EnableWindow(FALSE);
	}

	options3.Add(_T("上行"));
    options3.Add(_T("下行"));
    options5.Add(_T("接入"));
    options5.Add(_T("始发"));
    options6.Add(_T("终到"));
    options6.Add(_T("交出"));

	TCHAR title[ITEMNUM][18]={_T("序号"), _T("起始车次"),_T("结束车次"),
		_T("方向"),_T(" 径路 "),_T("接入/始发"), _T("终到/交出")};
    
   if(m_pGridCtrl!=NULL)
	return FALSE;
	m_pGridCtrl=new CGridCtrl;
	if(!m_pGridCtrl) return FALSE;
	CStatic * pWnd_S;
	pWnd_S=(CStatic *)GetDlgItem(IDC_GRID);


	//CRect rect(20,70,670,370);
	CRect rc;
	m_Frame1.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.InflateRect(-10,-30,-10,-10);
	m_pGridCtrl->Create (rc,this,IDC_GRID);
	//m_pGridCtrl->SetEditable(FALSE);            
	//m_pGridCtrl->EnableDragAndDrop (TRUE);       

	try{
		m_pGridCtrl->SetRowCount(m_nRows);                //行
		m_pGridCtrl->SetColumnCount(m_nCols);             //列
		m_pGridCtrl->SetFixedRowCount(m_nFixRows);        //固定行
		m_pGridCtrl->SetFixedColumnCount(m_nFixCols);     //固定列
		    
		m_pGridCtrl->SetFixedTextColor(RGB(0,0,0));      //文本颜色
		//m_pGridCtrl->SetFixedBkColor(RGB(192,192,192));   //背景色
		m_pGridCtrl->SetGridLineColor(RGB(166,202,240));       //线的颜色
	    m_pGridCtrl->SetFixedBkColor(RGB(255,242,240));
         //RGB(166,202,240
        m_pGridCtrl->SetColumnWidth(0, 60);
        m_pGridCtrl->SetColumnWidth(1, 90);
        m_pGridCtrl->SetColumnWidth(2, 90);
        m_pGridCtrl->SetColumnWidth(3, 70);
        m_pGridCtrl->SetColumnWidth(4, 180);
		m_pGridCtrl->SetColumnWidth(5, 80);
        m_pGridCtrl->SetColumnWidth(6, 80);
	 
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
 
//  int row,col;
 
//	for ( row = 0; row < 5; row++)
	 for (int col = 0; col < ITEMNUM; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = col;
  		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;  
 		Item.strText.Format(title[col]);
		m_pGridCtrl->SetItem(&Item);
	}
  
    InitCell(1);  //初始化cellcombo
	utility_count=0;
 
	LoadUtility(ddtid);
    
	ndialogInited=1;

	return TRUE;  
}
void CPropertyPagePasswayManagement::OnGridItemChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
    
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
  	//int nRow = m_pGridCtrl_1->GetFocusCell().row;
  
	 /*
    if (nRow >= 1 && nRow<=passway_count)       
    {   current_passway=nRow-1;                //当前径路号；
	    LoadPassWayStation(info[nRow-1].Pass_way_id);
	    m_pGridCtrl_1->Invalidate();
    }	
	*/
}

void CPropertyPagePasswayManagement::OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    //Trace(_T("Start Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
    //m_pGridCtrl_1->SetItemBkColour(pItem->iRow , pItem->iColumn , RGB(0xFF,0xFF,0xE0));
 
}



void CPropertyPagePasswayManagement::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;      

}
void CPropertyPagePasswayManagement::OnGrid2ItemChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
 
}

void CPropertyPagePasswayManagement::OnGrid2StartEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
   
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    CGridCellCombo *pCell;
    CStringArray options4;
	options4.Add(_T("1"));
    options4.Add(_T("2"));
	options4.Add(_T("3"));
  	options4.Add(_T("4"));

	CString content1;
    content1=m_pGridCtrl_2->GetItemText(pItem->iRow,0);   //获得输入内容
    if(atoi(content1)<=0) 
	return;

	CStringArray options;
	for(int i=0;i<nStationNum;i++)
    options.Add( _T(gpcdef.GetStationName(nStationList[i])) );
    
	switch(pItem->iColumn)
	{
	case 1:
		break;
	case 2:
		  
		break;
	case 3:
	     m_pGridCtrl_2->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,3);
		  pCell->SetOptions(options);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
	case 4:
		  m_pGridCtrl_2->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,4);
		  pCell->SetOptions(options4);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
		break;
	case 5:
		  m_pGridCtrl_2->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,5);
		  pCell->SetOptions(options4);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
   
		break;
	default:break;
		
	}

}


void CPropertyPagePasswayManagement::OnGrid2EndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
     
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;   
    
 
}
BOOL CPropertyPagePasswayManagement::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	 
	if(gpcdef.GetUserIdentify() != GPC_TEST)
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_NEW);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_DELETE);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_SAVE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_STATION_NEW);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_STATION_DELETE);
		pWnd->EnableWindow(FALSE);
 
	}
 
	TCHAR title[3][40]={_T("序号"),_T("径路号"), _T("径路名称") };
	if(m_pGridCtrl_1!=NULL)
	return FALSE;
	m_pGridCtrl_1=new CGridCtrl;
	if(!m_pGridCtrl_1) return FALSE;
	CStatic * pWnd_S;
	pWnd_S=(CStatic *)GetDlgItem(IDC_GRID);
	//CRect rect(10,70,280,400);
	CRect rc;
	m_Frame1.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.InflateRect(-10,-30,-10,-10);
	m_pGridCtrl_1->Create (rc,this,IDC_GRID);
 
	try{
		m_pGridCtrl_1->SetRowCount(m_nRows_1);              
		m_pGridCtrl_1->SetColumnCount(m_nCols_1);           
		m_pGridCtrl_1->SetFixedRowCount(m_nFixRows_1);        
		m_pGridCtrl_1->SetFixedColumnCount(m_nFixCols_1);     
		    
		m_pGridCtrl_1->SetFixedTextColor(RGB(0,0,0));      //文本颜色
		//m_pGridCtrl->SetFixedBkColor(RGB(192,192,192));   //背景色
		m_pGridCtrl_1->SetGridLineColor(RGB(166,202,240));       //线的颜色
        //m_pGridCtrl_1->GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
       	m_pGridCtrl_1->SetFixedBkColor(RGB(255,242,240));
        m_pGridCtrl_1->SetColumnWidth(0, 36);    // 设置网格宽度
        m_pGridCtrl_1->SetColumnWidth(1, 46);
        m_pGridCtrl_1->SetColumnWidth(2, 150);
	
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
 
  int col;
 
//	for ( row = 0; row < 5; row++)
	 for ( col = 0; col < 3; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = col;
  		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
 		Item.strText.Format(title[col]);
		m_pGridCtrl_1->SetItem(&Item);
	}
 

/********************径路经由站**************************/	 
  TCHAR title2[6][18]={_T("序号"),_T("径路号"), _T("站序号"),_T("车站"),
                       _T("入口"), _T("出口")};
	if(m_pGridCtrl_2!=NULL)
	return FALSE;
	m_pGridCtrl_2=new CGridCtrl;
	if(!m_pGridCtrl_2) return FALSE;
 
	//CRect rect2(300,70,690,340);
	//CRect rc;
	m_Frame2.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.InflateRect(-10,-30,-10,-10);
	m_pGridCtrl_2->Create (rc,this,IDC_GRID2);
 
	try{
		m_pGridCtrl_2->SetRowCount(m_nRows_2);            
		m_pGridCtrl_2->SetColumnCount(m_nCols_2);            
		m_pGridCtrl_2->SetFixedRowCount(m_nFixRows_2);       
		m_pGridCtrl_2->SetFixedColumnCount(m_nFixCols_2);     
		m_pGridCtrl_2->SetFixedTextColor(RGB(0,0,0));      
		m_pGridCtrl_2->SetGridLineColor(RGB(166,202,240));       
	    m_pGridCtrl_2->SetFixedBkColor(RGB(255,242,240));
        m_pGridCtrl_2->SetColumnWidth(0, 36);
        m_pGridCtrl_2->SetColumnWidth(1, 50);
        m_pGridCtrl_2->SetColumnWidth(2, 50);
        m_pGridCtrl_2->SetColumnWidth(3, 56);
        m_pGridCtrl_2->SetColumnWidth(4, 40);
		m_pGridCtrl_2->SetColumnWidth(5, 40);
	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}


//	for ( row = 0; row < 5; row++)
	 for ( col = 0; col < 6; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = col;
  		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
 		Item.strText.Format(title2[col]);
		m_pGridCtrl_2->SetItem(&Item);
	}

    m_pGridCtrl_2->AutoSizeColumns();
	m_pGridCtrl_2->Invalidate(FALSE);
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
    gpcdef.GetStationList(gpcdef.GetFirstSection(),nStationNum,nStationList);
   

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPropertyPagePasswayAppoint::LoadUtility(int consoleid)
{
	SetBlankCells(0);
	Flag_moredata=TRUE;
    utility_count=0;

	char buff[128], *ptr;
 	ptr = buff;
	ushort count;

 

	ptr += 2;           //for msglen

 	*(ushort *)ptr =  (LOAD_PASSWAY_UTILITY); 
	ptr += 2;   
	
 	*ptr = ddtid;    //consoleid
	ptr += 1;   
	
 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: 请求径路管理数据失败！");
		return ;
    }
	utility_count=0;             //总数0；
	return ;
}
 

void CPropertyPagePasswayManagement::LoadPassWayInfo()
{
	char buff[128], *ptr;
 	ptr = buff;
	ushort count;

 
	ptr += 2;           //for msglen

 	*(ushort *)ptr =  (LOAD_PASSWAY_INFO); 
	ptr += 2;   
	
 	*ptr = ddtid;  //Console id  (ddtid
	ptr += 1;  
	
 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: 请求径路数据失败！");
		return ;
    }
    Flag_moredata=TRUE;                //可以接受subao数据
	passway_count=0;  
	return ;
}

void CPropertyPagePasswayManagement::LoadPassWayStation(short PassWayId)
{
 
	char buff[128], *ptr;
 	ptr = buff;
	ushort count;

 

	ptr += 2;           //for msglen

 	*(ushort *)ptr =  (LOAD_PASSWAY_STATION); 
	ptr += 2;   
	
 	*ptr = ddtid;  //Console id
	ptr += 1;  

	*(ushort *)ptr = NettoHostUShort(PassWayId);  //PassWayId
	ptr += 2;

 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: 请求径路数据失败！");
		return ;
    }
    
	return ;
}

BOOL CPropertyPagePasswayAppoint::PreTranslateMessage(MSG* pMsg) 
{
 
	 if( pMsg->message == WM_KEYDOWN )
    {
        if(pMsg->wParam == VK_RETURN
            || pMsg->wParam == VK_ESCAPE )
        {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);
            return TRUE;                    // DO NOT process further
        }
    }
    	  
	return CPropertyPage::PreTranslateMessage(pMsg);
 
}

BOOL CPropertyPagePasswayManagement::PreTranslateMessage(MSG* pMsg) 
{
 	
	return CPropertyPage::PreTranslateMessage(pMsg);
}

LRESULT CPropertyPagePasswayAppoint::OnDdtChanged(WPARAM wParam, LPARAM lParam)
{
   LoadUtility(ddtid);
   return TRUE;

}
void CPropertyPagePasswayAppoint::OnDelete() 
{   
	if( !gpcdef.GetOperatorRight(WINTG_EDIT))
    return;

	ASSERT_VALID(m_pGridCtrl);
	int nRow = m_pGridCtrl->GetFocusCell().row;
    if (nRow >= 1 && nRow<=utility_count)
    {
		if(AfxMessageBox("要删除此条径路?",MB_YESNO | MB_DEFBUTTON2 |MB_ICONQUESTION) != IDYES  )
			return; 
		m_pGridCtrl->DeleteRow(nRow);	
		m_pGridCtrl->Invalidate();
		
		Del_Utility(nRow-1);
   	    utility_count-=1;
		for(int i=nRow-1;i<utility_count;i++)
			utility[i]=utility[i+1];
		utility_count-=1;
		
	}

}

void CPropertyPagePasswayAppoint::OnSave() 
{
	if(!gpcdef.GetOperatorRight(WINTG_EDIT))
        return;
	Set_Utility();         //设置径路至CAD
    
	LoadUtility(ddtid);    //从CAD请求径路
}

void CPropertyPagePasswayManagement::OnPassWayNew() 
{  
	CDlgPasswayInsert dlg;
	if(dlg.DoModal() == IDOK)
	{
		short passwayid;
		CString passwayname;

		passwayid = dlg.m_nPasswayId;
		passwayname = dlg.m_nPasswayName;

		passway_count ++;

		info[passway_count-1].Pass_way_id = passwayid;
	 
        strcpy(info[passway_count-1].Pass_way_name, passwayname);

		m_pGridCtrl_1->InsertRow("新增");

		int row = passway_count;
		TCHAR buffer[10];
		for (int  col = 0; col < 3; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;  //行
			Item.col = col;  //列
			Item.crBkClr = RGB(0xFF,0xFf,0xFF);        
			Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;

			if(col==0)
				sprintf(buffer,"%d",row);
			else if(col==1)
			    sprintf(buffer,"%d",passwayid);
		    else if(col==2)
			   sprintf(buffer,"%s",passwayname );
	
			Item.strText.Format(buffer);
			ASSERT_VALID(m_pGridCtrl_1);
			m_pGridCtrl_1->SetItem(&Item);            
		}  
		m_pGridCtrl_1->Invalidate();
	}

	
 
}

//删除选定的径路
void CPropertyPagePasswayManagement::OnPassWayDelete() 
{
	int nRow = m_pGridCtrl_1->GetFocusCell().row;
    if (nRow >= 1 && nRow<=passway_count)
    {   
		if(AfxMessageBox("要删除此径路?",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES  )
			return; 

		Del_Passway(info[nRow-1].Pass_way_id);

		passway_count = 0;
		//重新加载
		LoadPassWayInfo();
	}

}

//显示径路信息
void CPropertyPagePasswayManagement::Show_Passway_Info( )
{
	//m_pGridCtrl_1->SetRedraw (FALSE);   
 
    if(passway_count <0) return;
    m_nRows_1 = passway_count+1;
	m_pGridCtrl_1->SetRowCount(m_nRows_1);

    TCHAR buffer[60];
	for (int row = 1; row < m_nRows_1; row++)
	for (int  col = 0; col < 3; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = row;   
		Item.col = col;   
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		switch(col){
		case 0:
			sprintf(buffer,"%d",row);
			break;
		case 1:
			sprintf(buffer,"%d",info[row-1].Pass_way_id);
			break;
		case 2:
            sprintf(buffer,"%s",info[row-1].Pass_way_name);
			break;
		default:
			break;
		} 
		Item.strText.Format(buffer);
		m_pGridCtrl_1->SetItem(&Item);
	}   

    m_pGridCtrl_1->AutoSizeColumns();
    //m_pGridCtrl_1->SetRedraw (TRUE);    
    m_pGridCtrl_1->Invalidate(); 

}


int  CPropertyPagePasswayManagement::GetStationNoByName(CString sName)
{

	for(int i=0;i<nStationNum;i++)
    if(!strcmp(gpcdef.GetStationName(nStationList[i]),sName ))
		return nStationList[i];
	return 0;

}
BOOL  CPropertyPagePasswayManagement::GetStationInfoFromMMI( )
{

    BOOL bWrong = FALSE;
    int nRowCount;
	do{
         
		 nRowCount = m_pGridCtrl_2->GetRowCount();
		for (int row = 1; row < nRowCount; row++)
		{
			for (int  col = 0; col < 6; col++)
			{ 
				GV_ITEM Item;
				Item.mask = GVIF_TEXT|GVIF_FORMAT;
				Item.row = row;  //行
				Item.col = col;  //列
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
				CString strText;
				strText = m_pGridCtrl_2->GetItemText(row,col);
				if( strText.IsEmpty())
				{
					bWrong = TRUE;
					break;
				}

				switch(col)
				{

				case 0:
					break;
				case 1:  //径路号
					sta[row-1].Pass_way_id = atoi(strText);
					break;
				case 2:  //站序号
					sta[row-1].Sta_lst_idx = atoi(strText) ;
					break;
				case 3: //车站
					sta[row-1].Station = GetStationNoByName(strText) ;
					break;
				case 4://入口
					sta[row-1].Entry = atoi(strText);
					break;
				case 5://出口
					sta[row-1].Exit = atoi(strText) ;
					break;
				default:
					break;
				} 

			}   
			if(bWrong)
				break;
		}
		if(bWrong) 
			return FALSE;
	}while(0);


	station_count = nRowCount-1;
	return TRUE;

}
void CPropertyPagePasswayManagement::Show_StationInfo( )
{   
   
 
    if(station_count <0) return;
    m_nRows_2 = station_count+1;
	m_pGridCtrl_2->SetRowCount(m_nRows_2);


    TCHAR buffer[20];
	for (int row = 1; row < m_nRows_2; row++)
	for (int  col = 0; col < 6; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = row;  //行
		Item.col = col;  //列
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		switch(col){
		case 0:
			sprintf(buffer,"%d",row);
			break;
		case 1:
			sprintf(buffer,"%d",sta[row-1].Pass_way_id);
			break;
		case 2:
            sprintf(buffer,"%d",sta[row-1].Sta_lst_idx);
			break;
   	    case 3:
            sprintf(buffer,"%s",gpcdef.GetStationName(sta[row-1].Station));
			break;
     	case 4:
            sprintf(buffer,"%d",sta[row-1].Entry);
			break;
      	case 5:
            sprintf(buffer,"%d",sta[row-1].Exit);
			break;
		default:
			break;
		}//switch
		Item.strText.Format(buffer);
		ASSERT(m_pGridCtrl_2);
		m_pGridCtrl_2->SetItem(&Item);
	}  //for
    m_pGridCtrl_2->AutoSizeColumns();
   
   m_pGridCtrl_2->Invalidate();  
}

void CPropertyPagePasswayAppoint::Show_Utility(int no, int row)
{   
 
	char buf_s[20],buf_e[20];
       _itoa(utility[no].START_TRAIN_NUM, buf_s, 10 );   //数字－》字符
        _itoa(utility[no].END_TRAIN_NUM, buf_e, 10 );
    TCHAR buffer[40];
 
	Flag=TRUE;
	for (int  col = 0; col < 7; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = row;  //行
		Item.col = col;  //列
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		switch(col){
		case 0:
			sprintf(buffer,"%d",row);
			break;
		case 1:
			sprintf(buffer,"%s",utility[no].PREFIX);
            strcat(buffer,buf_s);
			break;
		case 2:
            sprintf(buffer,"%s",utility[no].PREFIX);
            strcat(buffer,buf_e);
			break;
		case 3:
            sprintf(buffer,"%s",utility[no].DIRECTIONS ? "下行" :"上行");
			break;
		case 4:
             sprintf(buffer,"%s",Get_Passwayname(utility[no].PASS_WAY_ID));
			break;
		case 5:
			sprintf(buffer,"%s",utility[no].ACPT_FLAG==1 ? "始发":"接入");
			break;
        case 6:
			sprintf(buffer,"%s",utility[no].GVT_FLAG==3 ? "终到":"交出");
			break;
		default:
			break;
		}//switch
		Item.strText.Format(buffer);
		ASSERT_VALID(m_pGridCtrl);
		m_pGridCtrl->SetItem(&Item);
 
	}  //for
	//m_pGridCtrl->AutoSizeColumns();
    InitCell(row);  //初始化cell
}

BOOL CPropertyPagePasswayManagement::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{

    int   idCtrl = (int) wParam;
	if(idCtrl!=IDC_GRID && idCtrl!=IDC_GRID2 ) 
		return CPropertyPage::OnNotify(wParam, lParam, pResult);
	//消息具体内容

    NM_GRIDVIEW *pItem=(NM_GRIDVIEW*)  lParam;;
    int nMessageType=pItem->hdr.code;
    int nCol;
	if(idCtrl==IDC_GRID)
	{  
		if(nMessageType==LVN_ITEMCHANGED)   //单击选中消息
		{
			int nRow = m_pGridCtrl_1->GetFocusCell().row;
			 nCol = m_pGridCtrl_1->GetFocusCell().col;

			
			if (nRow >= 1 && nRow<=passway_count )               //当前列的范围在允许的范围内
			{   
				current_passway=nRow-1;                         //当前径路号；
				LoadPassWayStation(info[nRow-1].Pass_way_id);   //发送请求站的消息
				current_passway_id = info[nRow-1].Pass_way_id;
				m_pGridCtrl_1->SetSelectedRange(current_passway+1,0,current_passway+1,2, FALSE);
				m_pGridCtrl_1->Invalidate();

	            m_pGridCtrl_2->SetRowCount(1);
			}	
			
		}
		
		
		if(nMessageType==GVN_ENDLABELEDIT)   //结束编辑消息
		{    
		
		}
	}
    //第二个GRIDCTRL	
	else  if(idCtrl==IDC_GRID2)
	{
		if(pItem->iRow < 0) return FALSE ;

		CStringArray optionscf;
		for(int i=0;i<nStationNum;i++)
			optionscf.Add( _T(gpcdef.GetStationName(nStationList[i])) );
		CGridCellCombo *pCellcf;

		m_pGridCtrl_2->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCellCombo));
		pCellcf= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,3);
		pCellcf->SetOptions(optionscf);
		pCellcf->SetStyle(pCellcf->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);

		//开始编辑
		if(nMessageType==GVN_BEGINLABELEDIT)
		{
			CGridCellCombo *pCell;
			CStringArray options4;
			CString myStr;
			for(int j=1;j<= 12;j++)
			{
				myStr.Format("%2d",j);
			    options4.InsertAt(j-1,myStr);
			}
			
			CStringArray options;
			for(int i=0;i<nStationNum;i++)
				options.Add( _T(gpcdef.GetStationName(nStationList[i])) );
			
			switch(pItem->iColumn)
			{
			case 1:
				break;
			case 2:			
				break;
			case 3:
				m_pGridCtrl_2->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCellCombo));
				pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,3);
				pCell->SetOptions(options);
				pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
				break;
			case 4:
				m_pGridCtrl_2->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCellCombo));
				pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,4);
				pCell->SetOptions(options4);
				pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
				break;
			case 5:
				m_pGridCtrl_2->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCellCombo));
				pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,5);
				pCell->SetOptions(options4);
				pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
				break;
			default:break;

			}		
			
		}
		if(nMessageType==GVN_ENDLABELEDIT)
		{
			switch(pItem->iColumn)
			{
			case 1: //径路号
				break;
			case 2: //站序号
				break;
			case 3: //车站		 
				m_pGridCtrl_2->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCell)); 
				break;
			case 4 : //入口
				m_pGridCtrl_2->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCell)); 	 
				break;
			case 5://出口
				m_pGridCtrl_2->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCell));

				break;

			default: break;	

			}
		}
	//if(idCtrl!=IDC_GRID)
	//{  
	//	int nRow = m_pGridCtrl_1->GetFocusCell().row;
  	//	current_passway = nRow -1;	
   // }
   }

   return CPropertyPage::OnNotify(wParam, lParam, pResult);

}

BOOL CPropertySheetPassway::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    ActivePage=GetActiveIndex();	
 
	return CPropertySheet::OnNotify(wParam, lParam, pResult);
}

BOOL CPropertyPagePasswayAppoint::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{   	
	CString content1,content;

	if(ddtid_default!=ddtid)
	{	 
	    m_newCtrl.EnableWindow(FALSE);
        m_deleteCtrl.EnableWindow(FALSE);
        m_saveCtrl.EnableWindow(FALSE); 
    }
   else
   {    
	    m_newCtrl.EnableWindow(TRUE);
        m_deleteCtrl.EnableWindow(TRUE);
        m_saveCtrl.EnableWindow(TRUE);

   }	
 	//#define IDC_GRID                        11004 	
    int   idCtrl = (int) wParam;
	if(idCtrl!=IDC_GRID ) 
	      return CPropertyPage::OnNotify(wParam, lParam, pResult);
    NM_GRIDVIEW *pItem=(NM_GRIDVIEW*)  lParam;;

    int nMessageType=pItem->hdr.code;
//    int j;

    if(nMessageType==GVN_BEGINLABELEDIT)
	{  

		CGridCellCombo *pCell;	
		options4.RemoveAll();
		for(int i=0;i<passway_count;i++)                //passwayname 列表
			options4.Add(_T(info[i].Pass_way_name));		
		//CString content1;
		content1=m_pGridCtrl->GetItemText(pItem->iRow,0);   //获得输入内容		
		switch(pItem->iColumn)
		{
		case 1:
			break;
		case 2:
			
			break;
		case 3:
			m_pGridCtrl->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCellCombo));
			pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,3);
			pCell->SetOptions(options3);
			pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			
			break;
		case 4:
			m_pGridCtrl->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCellCombo));
			pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,4);
			pCell->SetOptions(options4);
			pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
			
		case 5:
			m_pGridCtrl->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCellCombo));
			pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,5);
			pCell->SetOptions(options5);
			pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
		case 6:
			m_pGridCtrl->SetCellType(pItem->iRow ,6, RUNTIME_CLASS(CGridCellCombo));
			pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,6);
			pCell->SetOptions(options6);
			pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
		}	
	}
	if(nMessageType==GVN_ENDLABELEDIT)
	{	
		
		CString content,content1;
		char  point[30];byte result;
		int length;
		
		content1=m_pGridCtrl->GetItemText(pItem->iRow,0);   //获得该行首列输入内容
		if(atoi(content1)<=0) 
		{ 
			*pResult = (1)? -1 : 0;
		    return FALSE;
		}
		
		content=m_pGridCtrl->GetItemText(pItem->iRow,pItem->iColumn);   //获得输入内容
		strcpy(point,content);	
		length=strlen(point);
    
		//判断输入内容是否正确
		switch(pItem->iColumn)
		{
		case 1:   //开始车次
			if (FALSE == analysis_train(length, point, 
				sizeof(utility[pItem->iRow-1].PREFIX), utility[pItem->iRow-1].PREFIX, 
				&(utility[pItem->iRow -1].START_TRAIN_NUM)))
			{
				break;
			}
           
            if(CheckSameUtility(pItem->iRow-1))
			{
				AfxMessageBox("已有重复的记录!");
				break;
			}
			col1=TRUE;
			break;

		case 2:              
			content1=m_pGridCtrl->GetItemText(pItem->iRow,1);   //获得第一列输入
			if(content1=="")
			{ 
				 *pResult = (1)? -1 : 0;   
			     break;
			}

			{
				char tmp[128];

				if (FALSE == analysis_train(length, point, 
					sizeof(utility[pItem->iRow-1].PREFIX), tmp, 
					&(utility[pItem->iRow -1].END_TRAIN_NUM)))
				{
					break;
				}
				if (strcmp(utility[pItem->iRow-1].PREFIX, tmp))
				{
					AfxMessageBox("车次号前缀应相同");
					break;
				}
			}

          
            if(CheckSameUtility(pItem->iRow-1))
			{
				AfxMessageBox("已有重复的记录!");
				break;
			}
			col2=TRUE;
			break;

		case 3:  
			content1=m_pGridCtrl->GetItemText(pItem->iRow,2);   //获得输入内容
			if(content1=="")
			{ 	 *pResult = (1)? -1 : 0;   
			break;
			}
			
			result=(content=="上行" ? 0: 1);
			utility[pItem->iRow-1].DIRECTIONS=result;  //输入内容
			m_pGridCtrl->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCell)); 
			
            if(CheckSameUtility(pItem->iRow-1))
			{
				AfxMessageBox("已有重复的记录!");
				break;
			}
			col3=TRUE;
			break;
		case 4:  
			content1=m_pGridCtrl->GetItemText(pItem->iRow,3);   //获得输入内容
			if(content1=="")
			{ 	
				*pResult = (1)? -1 : 0;   
			    break;
			}
			utility[pItem->iRow -1].PASS_WAY_ID=Get_Passwayid(point) ;  //输入内容
			m_pGridCtrl->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCell)); 
			
            if(CheckSameUtility(pItem->iRow-1))
			{
				AfxMessageBox("已有重复的记录!");
				break;
			}
			col4=TRUE;
			break;
		case 5:  content1=m_pGridCtrl->GetItemText(pItem->iRow,4);   //获得输入内容
			if(content1=="")
			{ 	 *pResult = (1)? -1 : 0;   
			break;
			}
			result=(content=="始发" ? 1: 2);
			//content.Format("%d",result);AfxMessageBox(content);
			utility[pItem->iRow-1].ACPT_FLAG=result;  //输入内容
			m_pGridCtrl->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCell)); 
			col5=TRUE;
			break;
		case 6: content1=m_pGridCtrl->GetItemText(pItem->iRow,5);   //获得输入内容
			if(content1=="")
			{ 	 *pResult = (1)? -1 : 0;   
			break;
			}
			result=(content=="终到" ? 3: 4);
			utility[pItem->iRow-1].GVT_FLAG=result;  //输入内容
			m_pGridCtrl->SetCellType(pItem->iRow ,6, RUNTIME_CLASS(CGridCell)); 
			col6=TRUE;
			break;
		default:break;
		}
		if(pItem->iRow >utility_count+1)  
		{ 
			*pResult = (1)? -1 : 0;                       //禁止编辑有效 
			m_pGridCtrl->SetItemBkColour(pItem->iRow , pItem->iColumn , RGB(0xFF,0xFF,0xFF));
			return FALSE;
		}
		if( col1 & col2 & col3 & col4 & col5 & col6)
			utility_count+=1;		
	}
    if(nMessageType== LVN_ITEMCHANGED)
	{ 
		//*pResult = (1)? -1 : 0;                        //禁止开始编辑
 
	}
	 
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

void CPropertyPagePasswayAppoint::SetBlankCells(int Start)
{
    if(m_pGridCtrl==NULL) return;
    TCHAR buffer[10];
	 for (int row=Start+1;row<100;row++)
	 for (int  col = 0; col < ITEMNUM; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;  //行
			Item.col = col;  //列
            Item.crBkClr = RGB(0xFF,0xFF,0xFF);        
  			Item.nFormat = DT_LEFT|DT_WORDBREAK|DT_NOPREFIX;
            strcpy(buffer,"");
 			Item.strText.Format(buffer);
			ASSERT_VALID(m_pGridCtrl);
			m_pGridCtrl->SetItem(&Item);
	 }  //for
   
     m_pGridCtrl->Invalidate();
}

 
void CPropertyPagePasswayAppoint::InitCell(int row)
{   
	 
	TCHAR buffer[10];
	 for (int  col = 0; col < 1; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;  //行
			Item.col = col;  //列
            Item.crBkClr = RGB(0xFF,0xFf,0xFF);        
  			Item.nFormat = DT_LEFT|DT_WORDBREAK|DT_NOPREFIX;
            sprintf(buffer,"%d",row);
 			Item.strText.Format(buffer);
			ASSERT_VALID(m_pGridCtrl);
			m_pGridCtrl->SetItem(&Item);            
	 }  //for
     m_pGridCtrl->Invalidate();
}

void CPropertyPagePasswayAppoint::Set_Utility()
{

	char buff[4096], *ptr;

	ptr = buff;
	ptr += 2;           //for msg len

	*(ushort *)ptr =  (SET_PASSWAY_UTILITY);   
	ptr += 2;   

	*ptr=ddtid;                                        //ddtid  
	ptr+=1;    

	char *seq = ptr;
	ptr ++;

	char* count_pos = ptr;
	ptr += 2;

	*seq = -1;
	short count = 0;
	for (int i=0; i<utility_count; i++)
	{
		strncpy(ptr,utility[i].PREFIX, 9);
		ptr+=10;

		*(unsigned long *)ptr = NettoHostULong(utility[i].START_TRAIN_NUM);
		ptr += 4;   

		*(unsigned long *)ptr = NettoHostULong(utility[i].END_TRAIN_NUM);
		ptr += 4;   


		*(ushort*) ptr = NettoHostUShort(utility[i].Station);
		ptr += 2;

		*ptr=utility[i].Entry; 
		ptr+=1;

		*ptr=utility[i].Exit; 
		ptr+=1;

		*ptr=utility[i].DIRECTIONS; 
		ptr+=1;    

		*(ushort *)ptr=NettoHostUShort(utility[i].PASS_WAY_ID);
		ptr+=2;

		*ptr=utility[i].ACPT_FLAG;
		ptr+=1;    

		*ptr=utility[i].GVT_FLAG;
		ptr+=1;    

		count++;
		if (ptr+30-buff>sizeof(buff) || i==utility_count-1)
		{
			(*seq)++;
			*(ushort *)count_pos = NettoHostUShort(count);

			count = ptr -buff;
			*(ushort *)buff = count;
			int error=SendTGCommMessage(count,buff);	
			if(error) 
			{ 
				sysprint(0,PMTG.ThreadId,"[PMTG]: 设置utility失败！");
				return ;
			}

			count = 0;
			ptr = count_pos+2;
		}
	}
}

void CPropertyPagePasswayManagement::Del_Passway(short id)
{ 
	char buff[128], *ptr;
 	ptr = buff;
	ushort count;

	ptr += 2;             //for msglen

 	*(ushort *)ptr =  (DEL_PASSWAY); 
	ptr += 2;   
	
 	*ptr = ddtid;        //Console id  
	ptr += 1;  
	
    *(ushort *)ptr = NettoHostUShort(id); 
	ptr += 2;   
	
 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: 请求径路数据失败！");
		return ;
    }
	return ;

}
//设置径路
void CPropertyPagePasswayManagement::Set_Passway()
{
	CString tempstr;

	char buff[2000], *ptr;
 	ptr = buff;
	ushort count;

	ptr += 2;           //for msg len

 	*(ushort *)ptr =  (SET_PASSWAY);   
	ptr += 2;   

	*ptr=ddtid;                                       
	ptr += 1;    
    
	if(current_passway< 0 || current_passway>100) return;
    *(ushort *)ptr = NettoHostUShort(info[current_passway].Pass_way_id);    // 1 info[0].Pass_way_id
	ptr += 2;

    tempstr=info[current_passway].Pass_way_name;                            // 2 
	strcpy(ptr,tempstr);
    ptr+=40;
    
     // 3  
    *(ushort *)ptr=NettoHostUShort(sta[0].Station);   
	ptr += 2;
    
    // 4
	*(ushort *)ptr = NettoHostUShort(sta[station_count-1].Station); 
	ptr += 2;

   *(ushort *)ptr = NettoHostUShort(station_count);                         //station count 
	ptr += 2;
     
	for (int j=0;j<station_count;j++)
    {
	   *(ushort *)ptr = NettoHostUShort(sta[j].Sta_lst_idx);               // idx
	   ptr += 2;
       
       *(ushort *)ptr = NettoHostUShort(sta[j].Station);                   // no
	   ptr += 2;

	   *ptr=sta[j].Entry;                                     
	   ptr+=1;  
       
       *ptr=sta[j].Exit;                                     
	   ptr+=1;  

	}  


 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: 设置utility失败！");
		return ;
    }

   return ;

}

void CPropertyPagePasswayAppoint::Del_Utility(short no)
{

	char buff[2000], *ptr;
 	ptr = buff;
	ushort count;

	ptr += 2;           //for msg len

 	*(ushort *)ptr =  (DEL_PASSWAY_UTILITY);   
	ptr += 2;   

	*ptr=ddtid;                                        //ddtid  
	ptr+=1;    
 
	int i =no;
	strncpy(ptr,utility[i].PREFIX, 9);
	ptr+=10;

	*(unsigned long *)ptr = NettoHostULong(utility[i].START_TRAIN_NUM);
	ptr += 4;   

	*(unsigned long *)ptr = NettoHostULong(utility[i].END_TRAIN_NUM);
	ptr += 4;   


	*(ushort*) ptr = NettoHostUShort(utility[i].Station);
	ptr += 2;

	*ptr=utility[i].Entry; 
	ptr+=1;

	*ptr=utility[i].Exit; 
	ptr+=1;

	*ptr=utility[i].DIRECTIONS; 
	ptr+=1;    

	*(ushort *)ptr=NettoHostUShort(utility[i].PASS_WAY_ID);
	ptr+=2;

	*ptr=utility[i].ACPT_FLAG;
	ptr+=1;    

	*ptr=utility[i].GVT_FLAG;
	ptr+=1; 

 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: 发送删除utility失败！");
		return ;
    }
    
	//utility_count-=1;
   return ;


}

void CPropertyPagePasswayManagement::OnPassWaySave() 
{
	 if(! GetStationInfoFromMMI())
	 {
		 CString strText;
         strText="输入有误,请检查!";
         CDlgMessageBox dlg(strText);
		 dlg.DoModal();
         return;
	 }

	  Set_Passway();
	  m_pGridCtrl_1->Invalidate();
	  m_pGridCtrl_2->Invalidate();
}

short CPropertyPagePasswayAppoint::Get_Passwayid(char * name)
{
     
	for(int i=0;i<passway_count;i++)
	{	if(strcmp(info[i].Pass_way_name,name)==0)
		return info[i].Pass_way_id;
	}
	return -1;
}


char * CPropertyPagePasswayAppoint::Get_Passwayname(short id)
{
 
	for(int i=0;i<passway_count;i++)
	{ 
	  if(info[i].Pass_way_id==id)
      {   
		  return info[i].Pass_way_name;}
	}
	return NULL;
 
}

//******************删除某条径路的车站***************************
void CPropertyPagePasswayManagement::OnStationDelete() 
{
   int nRow = m_pGridCtrl_2->GetFocusCell().row;
    if (nRow >= 1 && nRow<=station_count)
    {    
		if(AfxMessageBox("要删除此车站?",MB_YESNO | MB_DEFBUTTON2 |MB_ICONQUESTION) != IDYES  )
			return; 

	    m_pGridCtrl_2->DeleteRow(nRow);			
		station_count-=1;                     //车站数目减1
        for(int i=nRow-1;i<station_count;i++)
			sta[i]=sta[i+1];                  //车站重新整理     
	   
        Show_StationInfo();
             	     
    }	
 
}

void CPropertyPagePasswayManagement::OnStationNew() 
{ 
    
	int nRow = m_pGridCtrl_2->GetRowCount();
	m_pGridCtrl_2->SetRowCount(nRow+1);

	TCHAR buffer[10];
	for (int  col = 0; col < 3; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = nRow;  //行
		Item.col = col;  //列
		Item.crBkClr = RGB(0xFF,0xFf,0xFF);        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;

		if(col==0)
			sprintf(buffer,"%d",nRow); //序号
		if(col==1)
			sprintf(buffer,"%d",current_passway_id);  //径路ID
		if(col==2)
			sprintf(buffer,"%d",nRow); //站序号
	 
		Item.strText.Format(buffer);
		ASSERT_VALID(m_pGridCtrl_1);
		m_pGridCtrl_2->SetItem(&Item);            
	}  
	m_pGridCtrl_2->Invalidate();
  
}

void CPropertyPagePasswayManagement::InitCell(int row)
{
	TCHAR buffer[10];
	 for (int  col = 0; col < 1; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;  //行
			Item.col = col;  //列
            Item.crBkClr = RGB(0xFF,0xFf,0xFF);        
  			Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;

		
            sprintf(buffer,"%d",row);

 			Item.strText.Format(buffer);
			ASSERT_VALID(m_pGridCtrl_2);
		m_pGridCtrl_2->SetItem(&Item);            
	 }  //for
     m_pGridCtrl_2->Invalidate();
}



BOOL CPropertySheetPassway::PreTranslateMessage(MSG* pMsg) 
{
	return CPropertySheet::PreTranslateMessage(pMsg);
}

BOOL CPropertyPagePasswayAppoint::CheckSameUtility(int no)
{
	for(int i=0; i< utility_count; i++)
	{ 
		if(i==no)
			continue;
		if(
			utility[i].START_TRAIN_NUM==utility[no].START_TRAIN_NUM &&
			utility[i].END_TRAIN_NUM==utility[no].END_TRAIN_NUM &&
			utility[i].DIRECTIONS==utility[no].DIRECTIONS &&
		    utility[i].PASS_WAY_ID==utility[no].PASS_WAY_ID )
		   return TRUE;
 			
	}

	return FALSE;
}

void CPropertyPagePasswayManagement::OnChangeSection(int sectionNum)
{
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
    gpcdef.GetStationList(sectionNum,nStationNum,nStationList);
	return;
}

short analysis_train(short length, const char*point, short prefix_len, char* prefix, ulong *number)
{
	for(int i=0; i<length; i++)
	{
		if(! (point[length-1-i]>= 0x30 && point[length-1-i] <= 0x39)) //是否非数字
		{ 
			if (length-i+1 >= prefix_len)
			{
				AfxMessageBox("车次号前缀过长");
				return FALSE;
			}

			memcpy(prefix, point, length-i); 
			prefix[length-i]=0;
			*number = atoi(point+length-i);

			return TRUE;
		}
	}

	//没有找到字母，则全部是数字；
	memset(prefix, 0x00, prefix_len);
	*number = atoi(point);

	return TRUE;
}
