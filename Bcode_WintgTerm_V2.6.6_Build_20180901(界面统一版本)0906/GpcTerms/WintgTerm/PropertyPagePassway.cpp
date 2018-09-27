
#include "stdafx.h"
#include "propertypagepassway.h"
#include "DlgPasswayInsert.h"
#include "DlgMessageBox.h"
#include "widgets.h"                 //UM 类的消息定义
#include "Tooltipwnd.h"              //提示类
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include "tg_basicpara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ITEMNUM  7

TG_PassWayInfo       info[200];         //径路数据
TG_PassWaySta        sta[100];          //径路车站数据

/////////////////////////////////////////////////////////////////////////////
CPropertyPagePasswayManagement::CPropertyPagePasswayManagement(int con_id, CWnd* pParent)
: CDialog(CPropertyPagePasswayManagement::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPropertyPagePasswayManagement)
	// NOTE: the ClassWizard will add member initialization here
	m_nCols_1 = 3;
	m_nRows_1 = 1;
	m_pGridCtrl_1 = NULL;

	m_nCols_2 = 7;
	m_nRows_2 = 2;
	m_pGridCtrl_2 = NULL;
	
	//}}AFX_DATA_INIT

	m_conid = con_id;
	nStationNum=0;
	passway_count=0;
	station_count=0;
	nSectionNum=0;
}

CPropertyPagePasswayManagement::~CPropertyPagePasswayManagement()
{
	if(m_pGridCtrl_1)
	{
		m_pGridCtrl_1->DeleteAllItems();
		delete m_pGridCtrl_1;
	}
	if(m_pGridCtrl_2)
	{
		m_pGridCtrl_2->DeleteAllItems();
		delete m_pGridCtrl_2;
	}
}

void CPropertyPagePasswayManagement::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPagePasswayManagement)
	DDX_Control(pDX, IDC_STATIC2, m_Frame2);
	DDX_Control(pDX, IDC_STATIC1, m_Frame1);
	DDX_Control(pDX, IDC_COMBO1, m_concombox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPropertyPagePasswayManagement, CDialog)
	//{{AFX_MSG_MAP(CPropertyPagePasswayManagement)	
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeComboConId)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayManagement message handlers
BOOL CPropertyPagePasswayManagement::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnChangeSection();

	/*if(gpcdef.GetUserIdentify() != GPC_TEST)
	{
	}*/

	TCHAR title[3][40]={_T("序号"),_T("径路号"), _T("径路名称") };
	if(m_pGridCtrl_1!=NULL)
		return FALSE;
	m_pGridCtrl_1=new CGridCtrl;
	if(!m_pGridCtrl_1) return FALSE;
	CStatic * pWnd_S;
	pWnd_S=(CStatic *)GetDlgItem(IDC_GRID);
	
	CRect rc;
	m_Frame1.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.InflateRect(-10,-30,-10,-10);
	m_pGridCtrl_1->Create (rc,this,IDC_GRID);

	try
	{
		m_pGridCtrl_1->SetRowCount(m_nRows_1);              
		m_pGridCtrl_1->SetColumnCount(m_nCols_1);           
		m_pGridCtrl_1->SetFixedRowCount(1);        
		m_pGridCtrl_1->SetFixedColumnCount(1);     

		m_pGridCtrl_1->SetFixedTextColor(RGB(0,0,0));      //文本颜色
		
		m_pGridCtrl_1->SetGridLineColor(RGB(166,202,240));       //线的颜色
		
		m_pGridCtrl_1->SetFixedBkColor(RGB(255,242,240));
		m_pGridCtrl_1->SetColumnWidth(0, 36);    // 设置网格宽度
		m_pGridCtrl_1->SetColumnWidth(1, 40);
		m_pGridCtrl_1->SetColumnWidth(2, 180);

	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	int col;
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

	m_pGridCtrl_1->ExpandColumnsToFit();
	
	TCHAR title2[7][18]={_T("序号"),_T("径路号"), _T("入口站序"),_T("出口站序"),_T("车站"),
		_T("入口"), _T("出口")};
	if(m_pGridCtrl_2!=NULL)
		return FALSE;
	m_pGridCtrl_2=new CGridCtrl;
	if(!m_pGridCtrl_2) return FALSE;

	m_Frame2.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.InflateRect(-10,-30,-10,-10);
	m_pGridCtrl_2->Create (rc,this,IDC_GRID2);

	try
	{
		m_pGridCtrl_2->SetRowCount(m_nRows_2);            
		m_pGridCtrl_2->SetColumnCount(m_nCols_2);            
		m_pGridCtrl_2->SetFixedRowCount(1);       
		m_pGridCtrl_2->SetFixedColumnCount(1);     
		m_pGridCtrl_2->SetFixedTextColor(RGB(0,0,0));      
		m_pGridCtrl_2->SetGridLineColor(RGB(166,202,240));       
		m_pGridCtrl_2->SetFixedBkColor(RGB(255,242,240));
		m_pGridCtrl_2->SetColumnWidth(0, 36);
		m_pGridCtrl_2->SetColumnWidth(1, 50);
		m_pGridCtrl_2->SetColumnWidth(2, 56);
		m_pGridCtrl_2->SetColumnWidth(3, 56);
		m_pGridCtrl_2->SetColumnWidth(4, 80);
		m_pGridCtrl_2->SetColumnWidth(5, 36);
		m_pGridCtrl_2->SetColumnWidth(6, 36);
	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	for ( col = 0; col < 7; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = col;
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format(title2[col]);
		m_pGridCtrl_2->SetItem(&Item);
	}

	m_pGridCtrl_2->ExpandColumnsToFit();
	m_pGridCtrl_2->Invalidate(FALSE);
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
	gpcdef.GetStationList(gpcdef.GetFirstSection(),nStationNum,nStationList);

	
	nSectionNum = sizeof(nSectionList) / sizeof(nSectionList[0]); //区段的数目
	gpcdef.GetSectionList(nSectionNum,nSectionList);	//区段列表
	for (int i = 0; i < nSectionNum; i++)//依次显示区段名
	{
		CString strDdyName = gpcdef.GetSectionName(nSectionList[i]);
		m_concombox.AddString(strDdyName);
		if(m_conid==nSectionList[i])
			m_concombox.SetCurSel(i);
	}	

	LoadPassWayInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyPagePasswayManagement::LoadPassWayInfo()
{
	int pass_info_count=200;
	if(GetTGBasicParam()->GetPasswayInfo(m_conid,info,&pass_info_count))
	{
		passway_count=pass_info_count;
		Show_Passway_Info();
		return;
	}
}

void CPropertyPagePasswayManagement::OnSelchangeComboConId() 
{
	int nSel = m_concombox.GetCurSel();
	if(nSel == -1)
		return;
	m_conid = nSectionList[nSel];
	OnChangeSection();
	LoadPassWayInfo();
}

void CPropertyPagePasswayManagement::LoadPassWayStation(short PassWayId)
{
	// 最多100个，见前面全局变量定义
	int sta_rec_count=100;  
	if(GetTGBasicParam()->GetPasswaySta(m_conid,PassWayId,sta,&sta_rec_count))
	{
		station_count=sta_rec_count;
		Show_StationInfo();
		return;
	}
}

//显示径路信息
void CPropertyPagePasswayManagement::Show_Passway_Info( )
{
	if(passway_count <0) return;
	m_nRows_1 = passway_count+1;
	m_pGridCtrl_1->SetRowCount(m_nRows_1);

	TCHAR buffer[60];
	for (int row = 1; row < m_nRows_1; row++)
	{
		for (int  col = 0; col < 3; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;   
			Item.col = col;   
			Item.nFormat = DT_LEFT|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
			switch(col)
			{
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
	}
	m_pGridCtrl_1->AutoSizeColumns();
	m_pGridCtrl_1->Invalidate(); 
}

int CPropertyPagePasswayManagement::GetStationNoByName(CString sName)
{
	for(int i=0;i<nStationNum;i++)
		if(!strcmp(gpcdef.GetStationName(nStationList[i]),sName ))
			return nStationList[i];
	return 0;
}

void CPropertyPagePasswayManagement::Show_StationInfo()
{   
	if(station_count <0) return;
	m_nRows_2 = station_count+1;
	m_pGridCtrl_2->SetRowCount(m_nRows_2);

	TCHAR buffer[20];
	for (int row = 1; row < m_nRows_2; row++)
	{
		for (int  col = 0; col < 7; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;  //行
			Item.col = col;  //列
			Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
			switch(col)
			{
			case 0:
				sprintf(buffer,"%d",row);
				break;
			case 1:
				sprintf(buffer,"%d",sta[row-1].Pass_way_id);
				break;
			case 2:
				sprintf(buffer,"%d",sta[row-1].in_sta_idx);
				break;
			case 3:
				sprintf(buffer,"%d",sta[row-1].out_sta_idx);
				break;
			case 4:
				sprintf(buffer,"%s",gpcdef.GetStationName(sta[row-1].Station));
				break;
			case 5:
				sprintf(buffer,"%d",sta[row-1].Entry);
				break;
			case 6:
				sprintf(buffer,"%d",sta[row-1].Exit);
				break;
			default:
				break;
			}
			Item.strText.Format(buffer);
			ASSERT(m_pGridCtrl_2);
			m_pGridCtrl_2->SetItem(&Item);
		}
	}
}

BOOL CPropertyPagePasswayManagement::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	int   idCtrl = (int) wParam;
	if(idCtrl!=IDC_GRID && idCtrl!=IDC_GRID2 ) 
		return CDialog::OnNotify(wParam, lParam, pResult);

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


			if (nRow >= 1 && nRow<=passway_count )  
			{   
				m_pGridCtrl_1->SetSelectedRange(nRow,0,nRow,2, FALSE);
				m_pGridCtrl_1->Invalidate();

				m_pGridCtrl_2->SetRowCount(1);
				// 
				LoadPassWayStation(info[nRow-1].Pass_way_id);
			}	

		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult); 
}

void CPropertyPagePasswayManagement::OnChangeSection()
{
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
	gpcdef.GetStationList(m_conid,nStationNum,nStationList);
	return;
}
