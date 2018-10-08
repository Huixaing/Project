// QueryStationCurrentStoreTrain.cpp : implementation file
//

#include "stdafx.h"
#include "oscalls.h"
#include "sysshare.h"
#include "resource.h"
#include "QueryStationCurrentStoreTrain.h"
#include "messages.h"
#include "function.h" 
#include "dlgeditstationsidename.h" 
#include <baseMsg\msgid.h>
#include "tgmsg.h"


#include "db_msgproc.h"
#include ".\querystationcurrentstoretrain.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//打印使用的绘图函数
void Drawf_QuseryStation(CDC* pDC,CPrintInfo* pInfo,void* pVoid=NULL);

//车站股道信息由LOADTLE程序根据站场描述数据自动生成
#define SELECT_STATION_COMMAND_BASE		1700

extern void SetStartEndtime(BYTE flag, int m_nShiftID, int& nEnd_time, int&nStart_time);                                          

int OsGetStationSideInfo(ushort usStationNo,ushort *pusSideNum,ushort *usSideList)
{   
	*pusSideNum = 0;
	STATION_SIDES_INFO stationSideInfo;
	if(gpcdef.GetSidesOfStation(usStationNo, stationSideInfo))
	{
		int nCount = stationSideInfo.sideNum;
		for(int i=0;i<nCount;i++)
			usSideList[i]=stationSideInfo.sideno[i];

		*pusSideNum=nCount;
	}
	return *pusSideNum;
}


#define STATIONSTORETRAINCOLUMNS		3
_TCHAR *_gszStationStoreTrainLabel[STATIONSTORETRAINCOLUMNS] =
{
	_T("股道"), _T("现存车信息"),_T("数目"),
};


int _gnStationStoreTrainFmt[STATIONSTORETRAINCOLUMNS] = 
{
	LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT
};

int _gnStationStoreTrainWidth[STATIONSTORETRAINCOLUMNS] = 
{
	50,430,40,
};

#define SIDESTORECARRIAGECOLUMNS		4
_TCHAR *_gszSideStoreCarriageLabel[SIDESTORECARRIAGECOLUMNS] =
{
	_T("车厢类型"), _T("车厢(辆)数"), _T("去向"),_T("运用")  // /重车去向
};


int _gnSideStoreCarriageFmt[SIDESTORECARRIAGECOLUMNS] = 
{
	LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT
};

int _gnSideStoreCarriageWidth[SIDESTORECARRIAGECOLUMNS] = 
{
	90,70,90,80
};


_TCHAR *_gszTrainStatisticLabel[3] =
{
	_T("车种"), _T("空车"),_T("重车"),
};

int _gnTrainStatisticFmt[3] = 
{
	LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT
};

int _gnTrainStatisticWidth[3] = 
{
	75,30,23,
};


/////////////////////////////////////////////////////////////////////////////
// CQueryStationCurrentStoreTrain dialog
CQueryStationCurrentStoreTrain::CQueryStationCurrentStoreTrain(
	 CTGBasicParam *pBasicPara,
	int nConid,
	int nShiftid,
	int nStationNo,////
	int nPhase,////
	BOOL ReadOnly,
	CWnd* pParent /*=NULL*/)
	: CDialog(CQueryStationCurrentStoreTrain::IDD, pParent)
	, m_nCarriage_num(0)
{
	//{{AFX_DATA_INIT(CQueryStationCurrentStoreTrain)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bReadOnly=ReadOnly;
	m_nSectionID = nConid;
	m_nShiftID=nShiftid;
	nStationCarriageCount=0; 
	nSideCarriageCount=0;     

	m_nCurrentSelectStation = nStationNo;   
	m_nCurrentPhase = nPhase;   
	bChangdedFlag = FALSE;
	m_pBasicPara = pBasicPara;
}


void CQueryStationCurrentStoreTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryStationCurrentStoreTrain)
	DDX_Control(pDX, IDC_SCST_CARRIAGE_EXPLAIN, m_carriageexplainCtrl);
	DDX_Control(pDX, IDC_SCST_CARRIAGE_TYPE, m_carriagetypeCtrl);
	DDX_Control(pDX, IDC_SCST_CARRIAGE_STYLE, m_carriageStytleCtrl);
	DDX_Control(pDX, IDC_SCST_CARRIAGE_APPLY, m_carriageApplyCtrl);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_SCST_CARRIAGE_NUMBER, m_nCarriage_num);
	DDV_MinMaxShort(pDX, m_nCarriage_num, 0, 128);
}

BEGIN_MESSAGE_MAP(CQueryStationCurrentStoreTrain, CDialog)
	//{{AFX_MSG_MAP(CQueryStationCurrentStoreTrain)
	ON_BN_CLICKED(IDC_SCST_ACK, OnScstAck)
	ON_BN_CLICKED(IDC_SCST_CANCEL, OnScstCancel)
	ON_BN_CLICKED(IDC_SCST_SIDE_CHANGE, OnScstSideChange)
	ON_BN_CLICKED(IDC_SCST_SIDE_DELETE, OnScstSideDelete)
	ON_BN_CLICKED(IDC_SCST_SIDE_INSERT, OnScstSideInsert)
	ON_BN_CLICKED(IDC_SCST_STATIONNAME, OnScstStationname)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_SCST_CARRIAGE_EXPLAIN, OnSelchangeScstCarriageExplain)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_EDIT_SIDE, OnEditSide)
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnButtonPrint)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_SCST_CARRIAGE_STYLE, OnCbnSelchangeScstCarriageStyle)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryStationCurrentStoreTrain message handlers

BOOL CQueryStationCurrentStoreTrain::PreTranslateMessage(MSG* lpMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ((lpMsg->message == WM_KEYDOWN) &&
		((lpMsg->wParam == VK_RETURN) || (lpMsg->wParam == VK_ESCAPE)))
	{
		CWnd *pWndFocus = NULL;
		TCHAR szClass[10];
		if ((lpMsg->wParam == VK_RETURN) &&
			((pWndFocus = GetFocus()) != NULL) &&
			IsChild(pWndFocus)  &&
			GetClassName(pWndFocus->m_hWnd, szClass, 10) &&
			(_tcsicmp(szClass, _T("EDIT")) == 0)
			){
				return TRUE;
			} else if (lpMsg->wParam == VK_ESCAPE)
				return TRUE;
	}	



	//commented by Qianguorong 2003-4-12 ends here
	return CDialog::PreTranslateMessage(lpMsg);
}

BOOL CQueryStationCurrentStoreTrain::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CTime t = CTime::GetCurrentTime( );
	long m_usCurrentTime = (t.GetHour( ) * 60 + t.GetMinute( )) * 60 + t.GetSecond( );
	long m_usCurrentDate = (t.GetYear( ) - 1990) * 380 + (t.GetMonth( ) - 1) * 31
		+ t.GetDay( ) - 1;

	//获取编辑时间，这个函数可以改进
	this->InitIdentify();


	m_ctrlStationStoreTrainList.SubclassDlgItem(IDC_SCST_STATION_STORE_LIST,this);
	//head of side list  
	m_ctrlStationStoreTrainList.SetHeaderLabel(STATIONSTORETRAINCOLUMNS,_gszStationStoreTrainLabel,
		_gnStationStoreTrainWidth,_gnStationStoreTrainFmt);

	m_ctrlSideStoreTrainList.SubclassDlgItem(IDC_SCST_SIDE_STORE_LIST,this);
	//head of train list 
	m_ctrlSideStoreTrainList.SetHeaderLabel(SIDESTORECARRIAGECOLUMNS,_gszSideStoreCarriageLabel,
		_gnSideStoreCarriageWidth,_gnSideStoreCarriageFmt);
	//关于统计
	m_ctrlTrainStatisticList.SubclassDlgItem(IDC_SCST_STATION_STATISTICS,this);
	for(int j = 0; j < m_pBasicPara->m_carKindType.size()+1; j++)
	{
		if(j == 0)
		{
			m_ctrlTrainStatisticList.InsertColumn(j, 
				_gszTrainStatisticLabel[j], 
				_gnTrainStatisticFmt[j],
				_gnTrainStatisticWidth[j], -1);
		}
		else
		{
			m_ctrlTrainStatisticList.InsertColumn(j, 
				m_pBasicPara->m_carKindType[j-1].shortName, 
				_gnTrainStatisticFmt[1],
				_gnTrainStatisticWidth[1], -1);
		}
	}

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 0;
	int n=0;
	CString sTrainTypeName;
	for(int i=0;i < m_pBasicPara->m_carKindType.size();i++)    
	{  
		for(int j=0; j<m_pBasicPara->m_carKindType[i].carKind.size(); j++)
		{
			sTrainTypeName = m_pBasicPara->m_carKindType[i].carKind[j].name;

			bool bFind=false;
			for(int k=0; k<m_ctrlTrainStatisticList.GetItemCount();k++)
			{
				CString strTitle = m_ctrlTrainStatisticList.GetItemText(k, 0);
				if(strTitle == sTrainTypeName)
				{
					bFind = true;
					break;
				}
			}

			if(!bFind)
			{
				lvi.iItem = n+1;
				lvi.pszText = (char *)(LPCTSTR)sTrainTypeName;
				m_ctrlTrainStatisticList.InsertItem(&lvi);
				n++;
			}
		}
	}

	// 类别
	for(int i=0;i<m_pBasicPara->m_carKindType.size();i++)
	{      
		m_carriageStytleCtrl.AddString(m_pBasicPara->m_carKindType[i].name);
	}
	if(m_pBasicPara->m_carKindType.size() > 0)
		m_carriageStytleCtrl.SetCurSel(0);

	// 车种
	if(m_pBasicPara->m_carKindType.size()>0)    
	{  
		for(int j=0; j<m_pBasicPara->m_carKindType[0].carKind.size(); j++)
		{
			m_carriagetypeCtrl.AddString(m_pBasicPara->m_carKindType[0].carKind[j].name);
		}

		if(m_pBasicPara->m_carKindType[0].carKind.size() > 0)
			m_carriagetypeCtrl.SetCurSel(0);
	}

	// 辆数
	CWnd *pWnd = GetDlgItem(IDC_SCST_CARRIAGE_NUMBER);
	if(pWnd)
		pWnd->SetWindowText("1");

	CSpinButtonCtrl *spinWnd;

	spinWnd = (CSpinButtonCtrl *)GetDlgItem(IDC_SCST_CARRIAGE_NUMBER_SPIN);
	spinWnd->SetBuddy(pWnd);
	spinWnd->SetRange(1,128);    
	spinWnd->SetPos(1);

	// 作业说明
	for(int i=0;i < m_pBasicPara->m_aryWorkExplain.GetCount();i++)
	{
		m_carriageexplainCtrl.AddString(m_pBasicPara->m_aryWorkExplain.GetAt(i));
	}
	if(m_pBasicPara->m_aryWorkExplain.GetCount()>0)
	{
		m_carriageexplainCtrl.SetCurSel(0);
	}

	// 运用
	for(int i=0;i < m_pBasicPara->m_aryCarrApply.GetCount();i++)
	{
		m_carriageApplyCtrl.AddString(m_pBasicPara->m_aryCarrApply.GetAt(i));
	}
	if(m_pBasicPara->m_aryCarrApply.GetCount()>0)
	{
		m_carriageApplyCtrl.SetCurSel(0);
	}

	//select station button 
	int nCurrentStationNo;

	m_ctrlStationMenuButton.SubclassDlgItem(IDC_SCST_STATIONNAME,this);

	//创建车站的弹出菜单
	m_menuStation.CreatePopupMenu( );

	int nStationIndexFlag[32];
	memset(nStationIndexFlag,0x00,sizeof(nStationIndexFlag));
	int nCount = 0;

	int nStationList[64];
	int nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);

	gpcdef.GetSectionPhaseStoreStationList(m_nSectionID,nStationNum,nStationList);

	for(i=0;i<nStationNum;i++)
	{
		// 车站名字加入弹出菜单
		m_menuStation.AppendMenu(MF_STRING|MF_ENABLED,SELECT_STATION_COMMAND_BASE + nCount,
			gpcdef.GetStationName(nStationList[i]));
		m_nAllStationArray[nCount] = nStationList[i];
		nCount += 1;
	}

	m_menuStation.SetDefaultItem(m_nCurrentSelectStation,true);

	CButton *pButton;
	pButton = (CButton *)GetDlgItem(IDC_SCST_STATIONNAME);
	//显示默认车站名字
	pButton->SetWindowText(gpcdef.GetStationName(m_nAllStationArray[m_nCurrentSelectStation]));	
	pButton->EnableWindow(FALSE);


	m_nTotalStationNum = nCount;                  //station num total 
	m_nCurrentCarriageIndex = 0;
	m_nCurrentSideIndex = -1;


	nCurrentStationNo = m_nAllStationArray[m_nCurrentSelectStation];

	m_cur_side_rownum = -1;		//当前股道在LIST CTRL中的行号
	m_cur_carriage_rownum = -1;	//当前股道股道车辆在LIST CTRL中的行号

	//各种车辆的总数，打印用，在显示车站信息时候进行统计
	m_totalCount = 0;
	m_pCount = m_cCount = m_pbCount = m_yCount = m_sCount = m_bCount = 0;
	m_dCount = m_jzCount = m_szCount = m_kCount =  m_axCount = m_qCount = 0;
	m_pCountK = m_cCountK = m_pbCountK = m_yCountK = m_sCountK = m_bCountK = 0;
	m_dCountK = m_jzCountK = m_szCountK = m_kCountK = m_axCountK = m_qCountK = 0;


	m_PreFrame = NULL;//打印句柄

	//历史图调阅不能更改数据
	if(m_bReadOnly)
	{
		CWnd* pwnd = NULL;
		pwnd = GetDlgItem(IDC_SCST_SIDE_INSERT);
		pwnd->EnableWindow(FALSE);

		pwnd = GetDlgItem(IDC_SCST_SIDE_CHANGE);
		pwnd->EnableWindow(FALSE);

		pwnd = GetDlgItem(IDC_SCST_SIDE_DELETE);
		pwnd->EnableWindow(FALSE);

		pwnd = GetDlgItem(IDC_SAVE);
		pwnd->EnableWindow(FALSE);

		pwnd = GetDlgItem(IDC_EDIT_SIDE);
		pwnd->EnableWindow(FALSE);

		pwnd = GetDlgItem(IDC_SCST_ACK);
		pwnd->EnableWindow(FALSE);

	}

	ShowStationSideTrain();

	ShowTrainStatisticInfo();

	InitDlgPosition(this);
	return TRUE;    
}

void CQueryStationCurrentStoreTrain::OnCbnSelchangeScstCarriageStyle()
{
	CString strCarTypeName;
	// 类别
	m_carriageStytleCtrl.GetWindowText(strCarTypeName);

	// 车种
	for(int i=0; i<m_pBasicPara->m_carKindType.size(); i++)
	{
		if(strCarTypeName == m_pBasicPara->m_carKindType[i].name)
		{
			if(m_pBasicPara->m_carKindType[i].carKind.size()>0)
				m_carriagetypeCtrl.ResetContent();

			for(int j=0; j<m_pBasicPara->m_carKindType[i].carKind.size(); j++)
			{
				m_carriagetypeCtrl.AddString(m_pBasicPara->m_carKindType[i].carKind[j].name);
			}

			if(m_pBasicPara->m_carKindType[i].carKind.size()>0)
				m_carriagetypeCtrl.SetCurSel(0);
			break;
		}
	}
}

void CQueryStationCurrentStoreTrain::OnScstAck() 
{ 
	m_nCurrentStationIndex = m_nAllStationArray[m_nCurrentSelectStation];
	SendSideinfo(m_nCurrentStationIndex);
	bChangdedFlag = true;
	CDialog::OnOK( );
}

void CQueryStationCurrentStoreTrain::OnScstCancel() 
{
	CDialog::OnCancel( );
}

void CQueryStationCurrentStoreTrain::OnScstSideChange() 
{
	//change Updated info 
	do 
	{
		if(m_nCurrentSideIndex<0) 
			return;
		struct _tagSideStoreCarriage tmp;

		// 车种
		BYTE nSubSel = m_carriagetypeCtrl.GetCurSel( );
		if(nSubSel == CB_ERR)
			break;

		// 类别
		BYTE nSel = m_carriageStytleCtrl.GetCurSel();
		if(nSel == CB_ERR)
			break;
		
		tmp.usCarriageType = m_pBasicPara->GetCarriageType(nSel, nSubSel);

		// 辆数
		CWnd *pWnd = GetDlgItem(IDC_SCST_CARRIAGE_NUMBER);
		if(pWnd == NULL)
			break;
		CString text;
		pWnd->GetWindowText(text);
		if(text.GetLength( ) == 0)
			break;
		if(IsAllSpace((char *)(LPCTSTR)text))
			break;
		tmp.usCarriageNum = atoi((char *)(LPCTSTR)text);
		pWnd = GetDlgItem(IDC_SCST_CARRIAGE_DIRECTION);
		if(pWnd == NULL)
			break;

		pWnd->GetWindowText(text);
		strcpy(tmp.lpszCarriageRemark,(char *)(LPCTSTR)text);

		m_carriageApplyCtrl.GetWindowText(text);
		strcpy(tmp.lpszCarriageApply,(char *)(LPCTSTR)text);

		tmp.usCarriageExplain = m_carriageexplainCtrl.GetCurSel();


		struct _tagSideStoredTrain *sideinfo;
		sideinfo = &info.sideinfo[m_nCurrentSideIndex];

		for(int i=0;i<sideinfo->usRecNum;i++)
		{ 
			if( sideinfo->rec[i].usCarriageType  ==tmp.usCarriageType 
				&& sideinfo->rec[i].usCarriageNum  ==tmp.usCarriageNum 
				&& ( strcmp(tmp.lpszCarriageRemark,sideinfo->rec[i].lpszCarriageRemark)==0 )
				&& sideinfo->rec[i].usCarriageExplain ==tmp.usCarriageExplain
				&& ( strcmp(tmp.lpszCarriageApply,sideinfo->rec[i].lpszCarriageApply)==0 ))
			{
				AfxMessageBox("此股道中已有此种车厢，请修改该车厢数目!",MB_OK);
				return;
			}
		}

		sideinfo->rec[m_nCurrentCarriageIndex] = tmp;

		m_pBasicPara->GetCarTypeAndKindIndex(tmp.usCarriageType, nSel, nSubSel);

		if(nSel<m_pBasicPara->m_carKindType.size())
		{
			text = m_pBasicPara->m_carKindType[nSel].shortName;
			if(text.IsEmpty())
				text = m_pBasicPara->m_carKindType[nSel].name;
		}

		CString temp;

		temp.Format("%s%s ",
			m_pBasicPara->m_aryWorkExplain.GetAt(tmp.usCarriageExplain),
			m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].name);

		text += temp;

		m_ctrlSideStoreTrainList.SetItemText(m_nCurrentCarriageIndex,0,text);
		text.Format("%d",tmp.usCarriageNum);
		m_ctrlSideStoreTrainList.SetItemText(m_nCurrentCarriageIndex,1,text);
		text.Format("%s",tmp.lpszCarriageRemark);
		m_ctrlSideStoreTrainList.SetItemText(m_nCurrentCarriageIndex,2,text);

		text = "";
		nSideCarriageCount=0;
		nStationCarriageCount-=sideinfo->CarriageCounts;  
		for( i=0;i < sideinfo->usRecNum;i++)
		{   
			BYTE nSel = 0;
			BYTE nSubSel = 0;
			m_pBasicPara->GetCarTypeAndKindIndex(sideinfo->rec[i].usCarriageType, nSel, nSubSel);
			
			temp.Format("%s%s%d ",
				m_pBasicPara->m_aryWorkExplain.GetAt(sideinfo->rec[i].usCarriageExplain),
				m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
				sideinfo->rec[i].usCarriageNum);

			text += temp;

			nSideCarriageCount+=sideinfo->rec[i].usCarriageNum;   
			sideinfo->CarriageCounts=nSideCarriageCount;
		}

		nStationCarriageCount+=sideinfo->CarriageCounts; 

	}while(0);
	ShowStationSideTrain();
}

void CQueryStationCurrentStoreTrain::OnScstSideDelete() 
{ 
	do 
	{ 
		if(m_nCurrentSideIndex<0)
			return;

		struct _tagSideStoredTrain *sideinfo;
		sideinfo = &info.sideinfo[m_nCurrentSideIndex];

		if(m_nCurrentCarriageIndex < 0 || m_nCurrentCarriageIndex >= sideinfo->usRecNum)
			break;
		//删除车厢相应项
		m_ctrlSideStoreTrainList.DeleteItem(m_nCurrentCarriageIndex);
		//车厢数目减少
		sideinfo->usRecNum -= 1;  /// 1
		//车厢数组更新
		for(int i=m_nCurrentCarriageIndex;i < sideinfo->usRecNum;i++)  /// 2
			sideinfo->rec[i] = sideinfo->rec[i + 1];
		if(m_nCurrentCarriageIndex >= sideinfo->usRecNum)
		{
			m_nCurrentCarriageIndex = 0;
			m_ctrlSideStoreTrainList.EnsureVisible(m_nCurrentCarriageIndex,false);
		}
		//选择  第一组车厢
		//m_ctrlSideStoreTrainList.SetCurSel(m_nCurrentCarriageIndex);   
		//显示  第一组车厢
		ShowCarriageRecord(&sideinfo->rec[m_nCurrentCarriageIndex]);

		CString temp;
		CString text;

		nStationCarriageCount-=sideinfo->CarriageCounts; ///减去现有股道的车厢数目
		nSideCarriageCount=sideinfo->CarriageCounts=0;
		for(i=0;i < sideinfo->usRecNum;i++)
		{  
			if(sideinfo->rec[i].usCarriageNum<=0)
				continue;
			CString temp;
			BYTE nSel = 0;
			BYTE nSubSel = 0;
			m_pBasicPara->GetCarTypeAndKindIndex(sideinfo->rec[i].usCarriageType, nSel, nSubSel);
			if(nSel < m_pBasicPara->m_carKindType.size())
			{
				text = m_pBasicPara->m_carKindType[nSel].shortName;
				if(text.IsEmpty())
					text = m_pBasicPara->m_carKindType[nSel].name;

				temp.Format("%s%s%d",
					m_pBasicPara->m_aryWorkExplain.GetAt(sideinfo->rec[i].usCarriageExplain),
					m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,//类型
					sideinfo->rec[i].usCarriageNum);//车厢数目 	
				text += temp;
			}
			nSideCarriageCount+=sideinfo->rec[i].usCarriageNum;
			sideinfo->CarriageCounts=nSideCarriageCount;  
			text += temp;
		}

		nStationCarriageCount+=sideinfo->CarriageCounts;///加上现股道所有的车厢数目

	}while(0);
	ShowStationSideTrain();
}

void CQueryStationCurrentStoreTrain::OnScstSideInsert() 
{
	// 插入1条车厢记录
	do 
	{ 
		if(m_nCurrentSideIndex<0) 
			return;
		struct _tagSideStoreCarriage tmp;     //当前的车厢结构，下面填充此结构

		BYTE nSubSel = m_carriagetypeCtrl.GetCurSel();
		if(nSubSel == CB_ERR)
			break;
		
		BYTE nSel = m_carriageStytleCtrl.GetCurSel();
		if(nSel == CB_ERR)
			break;
		
		tmp.usCarriageType = m_pBasicPara->GetCarriageType(nSel, nSubSel);

		//获得车厢数目
		CWnd *pWnd = GetDlgItem(IDC_SCST_CARRIAGE_NUMBER);
		if(pWnd == NULL)
			break;
		CString text;
		pWnd->GetWindowText(text);
		if(text.GetLength( ) == 0)
			break;
		if(IsAllSpace((char *)(LPCTSTR)text))
			break;
		tmp.usCarriageNum = atoi((char *)(LPCTSTR)text);  ///////// 2 数目


		pWnd = GetDlgItem(IDC_SCST_CARRIAGE_DIRECTION);
		if(pWnd == NULL)
			break;
		pWnd->GetWindowText(text);
		strcpy(tmp.lpszCarriageRemark,(char *)(LPCTSTR)text);////// 3 作业说明

		m_carriageApplyCtrl.GetWindowText(text);
		strcpy(tmp.lpszCarriageApply,(char *)(LPCTSTR)text);////// 3 作业说明

		nSel = m_carriageexplainCtrl.GetCurSel();
		if(nSel == CB_ERR)
			break;
		tmp.usCarriageExplain = nSel;

		//当前股道指针
		struct _tagSideStoredTrain *sideinfo;
		sideinfo = &info.sideinfo[m_nCurrentSideIndex];

		for(int i=0;i<sideinfo->usRecNum;i++)
		{ 
			if( sideinfo->rec[i].usCarriageType  ==tmp.usCarriageType 
				&& sideinfo->rec[i].usCarriageNum  ==tmp.usCarriageNum 
				&& ( strcmp(tmp.lpszCarriageRemark,sideinfo->rec[i].lpszCarriageRemark)==0 )
				&& sideinfo->rec[i].usCarriageExplain ==tmp.usCarriageExplain
				&& ( strcmp(tmp.lpszCarriageApply,sideinfo->rec[i].lpszCarriageApply)==0 ))
			{
				AfxMessageBox("此股道中已有此种车厢，请修改该车厢数目!",MB_OK);
				return;
			}
		}

		sideinfo->rec[sideinfo->usRecNum] = tmp;    //将此车厢结构赋值给 缓冲区

		//显示此结构在车厢列表中， 右上 
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT ;
		lvi.iSubItem = 0;
		CString temp;

		m_pBasicPara->GetCarTypeAndKindIndex(tmp.usCarriageType, nSel, nSubSel);
		if(nSel<m_pBasicPara->m_carKindType.size())
		{
			text = m_pBasicPara->m_carKindType[nSel].shortName;
			if(text.IsEmpty())
				text = m_pBasicPara->m_carKindType[nSel].name;
		}

		temp.Format("%s%s ",
			m_pBasicPara->m_aryWorkExplain.GetAt(tmp.usCarriageExplain),
			m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].name);

		text += temp;
		lvi.iItem = sideinfo->usRecNum;            //
		lvi.pszText = (char *)(LPCTSTR)text;
		m_ctrlSideStoreTrainList.InsertItem(&lvi);
		text.Format("%d",tmp.usCarriageNum);
		m_ctrlSideStoreTrainList.SetItemText(sideinfo->usRecNum,1,text);
		text.Format("%s",tmp.lpszCarriageRemark);
		m_ctrlSideStoreTrainList.SetItemText(sideinfo->usRecNum,2,text);
		sideinfo->usRecNum += 1;   //类型车厢 数目

		//显示此股道所有的车厢信息，  左面
		CString text2;
		text = "";
		nSideCarriageCount=0;
		nStationCarriageCount-=sideinfo->CarriageCounts;////减去未增加前的该股道的车厢数目
		//重新计算该股道的车辆数目
		for(int i=0;i < sideinfo->usRecNum;i++)
		{ 
			if(sideinfo->rec[i].usCarriageNum<=0)
				continue;

			BYTE nSel = 0;
			BYTE nSubSel = 0;
			m_pBasicPara->GetCarTypeAndKindIndex(sideinfo->rec[i].usCarriageType, nSel, nSubSel);
			temp.Format("%s%s%d ",
				m_pBasicPara->m_aryWorkExplain.GetAt(sideinfo->rec[i].usCarriageExplain),
				m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
				sideinfo->rec[i].usCarriageNum); 

			nSideCarriageCount+=sideinfo->rec[i].usCarriageNum ;
			sideinfo->CarriageCounts=nSideCarriageCount;

			text += temp;
		}
		nStationCarriageCount+=sideinfo->CarriageCounts ;  //// 加上新的改股道的车厢数目

		ClearAllEdit();
	}while(0);
	ShowStationSideTrain();
}

/**********************选择车站************************/
void CQueryStationCurrentStoreTrain::OnScstStationname() 
{
	// 显示车站名字列表
	m_nCurrentSideIndex = 0;
	nStationCarriageCount=0;///////////////
	nSideCarriageCount=0;   ////////////////  
	GetDlgItem(IDC_EDIT_TIME)->SetWindowText("");
	GetDlgItem(IDC_TOTAL_SUM)->SetWindowText("");

	POINT point;

	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_SCST_STATIONNAME);
	pWnd->GetWindowRect(&rect);
	point.x = rect.left;
	point.y = rect.bottom;

	// -[Feral]-----------------------------------------------------------
	// show and track the menu
	DWORD dwSelectionMade = m_menuStation.TrackPopupMenu( (TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD),
		point.x, point.y, this
		);
	do 
	{
		if(dwSelectionMade == 0)
			break;

		dwSelectionMade -= SELECT_STATION_COMMAND_BASE;
		if(dwSelectionMade < 0 || dwSelectionMade >= m_nTotalStationNum)
			break;

		if(dwSelectionMade == m_nCurrentSelectStation)
			break;

		m_nCurrentSelectStation = (ushort)dwSelectionMade;
		CButton *pButton = (CButton *)GetDlgItem(IDC_SCST_STATIONNAME);
		//显示默认车站名字
		pButton->SetWindowText(gpcdef.GetStationName(m_nAllStationArray[m_nCurrentSelectStation]));


		//初始化车站现车内容
		nSideNum = sizeof(nSideList) / sizeof(nSideList[0]);
		OsGetStationSideInfo(m_nAllStationArray[m_nCurrentSelectStation],&nSideNum,nSideList);
		// CString as; as.Format("sideNum:%d",nSideNum);AfxMessageBox(as);
		if(nSideNum<1)
		{
			GetDlgItem(IDC_SCST_SIDE_INSERT)->EnableWindow(FALSE);
			GetDlgItem(IDC_SCST_SIDE_CHANGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_SCST_SIDE_DELETE)->EnableWindow(FALSE);
			GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
			//break;
		}
		else
		{
			GetDlgItem(IDC_SCST_SIDE_INSERT)->EnableWindow(TRUE);
			GetDlgItem(IDC_SCST_SIDE_CHANGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_SCST_SIDE_DELETE)->EnableWindow(TRUE);
			GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
		}

		this->InitIdentify();

		memset(&info,0x00,sizeof(struct _tagStationStoreTrain)); 
		GetStationCarsInfo(m_nAllStationArray[m_nCurrentSelectStation]); ////// 

	}while(0);

	ShowStationSideTrain();
}



//显示类型车厢的存车情况
void CQueryStationCurrentStoreTrain::ShowCarriageRecord(struct _tagSideStoreCarriage *rec)
{
	do 
	{
		if(rec == NULL)
			break;

		BYTE nSel = 0;
		BYTE nSubSel = 0;
		m_pBasicPara->GetCarTypeAndKindIndex(rec->usCarriageType, nSel, nSubSel);

		m_carriageStytleCtrl.SetCurSel(nSel);
		m_carriagetypeCtrl.ResetContent();
		if(nSel < m_pBasicPara->m_carKindType.size())
		{
			for(int i=0; i<m_pBasicPara->m_carKindType[nSel].carKind.size(); i++)
			{
				m_carriagetypeCtrl.AddString(m_pBasicPara->m_carKindType[nSel].carKind[i].name);
			}

			if(nSubSel < m_pBasicPara->m_carKindType[nSel].carKind.size())
				m_carriagetypeCtrl.SetCurSel(nSubSel);
		}
		
		CString text;
		CWnd *pWnd = GetDlgItem(IDC_SCST_CARRIAGE_NUMBER);                       //车厢数目
		text.Format("%d",rec->usCarriageNum);

		pWnd->SetWindowText(text);

		CSpinButtonCtrl *spinWnd;
		spinWnd = (CSpinButtonCtrl *)GetDlgItem(IDC_SCST_CARRIAGE_NUMBER_SPIN);  //数目
		spinWnd->SetPos(rec->usCarriageNum);

		pWnd = GetDlgItem(IDC_SCST_CARRIAGE_DIRECTION);
		pWnd->SetWindowText(rec->lpszCarriageRemark);                           //车厢方向：上海

		for(int i=0;i < m_pBasicPara->m_aryCarrApply.GetCount();i++)
		{
			if(m_pBasicPara->m_aryCarrApply.GetAt(i) == rec->lpszCarriageApply)
			{
				m_carriageApplyCtrl.SetCurSel(i);
				break;
			}
		}

		m_carriageexplainCtrl.SetCurSel(rec->usCarriageExplain);

	}while(0);

}

//显示股道的存车情况 
void CQueryStationCurrentStoreTrain::ShowSideRecord(int nCurrentSideIndex)
{
	if(nCurrentSideIndex < 0 || nCurrentSideIndex >= sizeof(info.sideinfo)/sizeof(info.sideinfo[0]))
		return;

	CString text;

	struct _tagSideStoredTrain *sideinfo;
	sideinfo = &info.sideinfo[nCurrentSideIndex];

	CWnd *pWnd = GetDlgItem(IDC_SCST_SIDE_NAME);
	text.Format("%s 股道现在站存车信息",sideinfo->pszSideName); 
	pWnd->SetWindowText(text);

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT ;
	lvi.iSubItem = 0;

	m_ctrlSideStoreTrainList.DeleteAllItems( );
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 0;
	struct _tagSideStoreCarriage *rec;

	for(int j=0;j <sideinfo->usRecNum;j++)
	{    
		rec = &sideinfo->rec[j];
		if(rec->usCarriageNum <=0)
			continue;

		BYTE nSel = 0;
		BYTE nSubSel = 0;
		m_pBasicPara->GetCarTypeAndKindIndex(rec->usCarriageType, nSel, nSubSel);

		if(nSel < m_pBasicPara->m_carKindType.size())
		{
			text = m_pBasicPara->m_carKindType[nSel].shortName;
			if(text.IsEmpty())
				text = m_pBasicPara->m_carKindType[nSel].name;

			CString temp;
			temp.Format("%s%s ",
				m_pBasicPara->m_aryWorkExplain.GetAt(rec->usCarriageExplain),
				m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].name);
			text += temp;
		}
		lvi.iItem = j;
		lvi.pszText = (char *)(LPCTSTR)text;

		m_ctrlSideStoreTrainList.InsertItem(&lvi);

		text.Format("%d",rec->usCarriageNum);
		m_ctrlSideStoreTrainList.SetItemText(j,1,text);

		text.Format("%s",rec->lpszCarriageRemark);  
		m_ctrlSideStoreTrainList.SetItemText(j,2,text);

		text.Format("%s",rec->lpszCarriageApply);  
		m_ctrlSideStoreTrainList.SetItemText(j,3,text);


	}

	pWnd = GetDlgItem(IDC_EDIT_TIME);  
	CTime t=info.EditTime;
	CString s = t.Format( "%Y-%m-%d %H:%M" );
	pWnd->SetWindowText(s);

	m_ctrlSideStoreTrainList.SetCurSel(0);
	if(sideinfo->usRecNum > 0)
	{
		ShowCarriageRecord(&sideinfo->rec[0]);
	}
}


void CQueryStationCurrentStoreTrain::OnDestroy() 
{
	CDialog::OnDestroy();	
}

//选择车厢内容: 空,待卸,待装 
void CQueryStationCurrentStoreTrain::OnSelchangeScstCarriageExplain() 
{
	GetDlgItem(IDC_SCST_CARRIAGE_DIRECTION)->SetWindowText("");
}

//发送车站存车信息
void CQueryStationCurrentStoreTrain::SendSideinfo(int StationIndex)
{
	time_t t_input;
	CLBTH::Archive ar( CLBTH::Archive::store);

	CTime t = CTime::GetCurrentTime();
	if(m_nCurrentPhase!=CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_CURRENT)
	{
		SetStartEndtime(m_nCurrentPhase, m_nShiftID, nEnd_time, nStart_time);
		t_input =  nStart_time;
	}
	else	
	{
		t_input =  t.GetTime();   //时间
	}

	//msg begin
	CLBTH::MsgSimpleStationCarCtrl  mMsgSimpleStationCarCtrlStart;
	mMsgSimpleStationCarCtrlStart.input_time = t_input;
	mMsgSimpleStationCarCtrlStart.station = StationIndex;
	mMsgSimpleStationCarCtrlStart.phase = (CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE)m_nCurrentPhase;
	mMsgSimpleStationCarCtrlStart.status = CLBTH::MsgSimpleStationCarCtrl::MSG_BEGIN;
	mMsgSimpleStationCarCtrlStart.serialize(ar);
	//end of msg begin

	//msg data begin
	CLBTH::MsgSimpleStationCarFormat mMsgSimpleStationCarFormat;
	int nSendIndex =0;

	mMsgSimpleStationCarFormat.phase = (CLBTH::MsgSimpleStationCarFormat::STACAR_PHASE)m_nCurrentPhase;
	mMsgSimpleStationCarFormat.input_time = t_input;
	mMsgSimpleStationCarFormat.station = StationIndex;

	byte bSideNameLen;

	struct _tagSideStoreCarriage *rec;

	for(int i = 0;i < info.usSideNum;i++)
	{
		mMsgSimpleStationCarFormat.side = info.sideinfo[i].usSideNo;

		bSideNameLen = strlen(info.sideinfo[i].pszSideName);
		memset(mMsgSimpleStationCarFormat.side_name, 0, sizeof(mMsgSimpleStationCarFormat.side_name));
		strncpy(mMsgSimpleStationCarFormat.side_name,info.sideinfo[i].pszSideName,bSideNameLen);

		//股道上面有存车
		if(info.sideinfo[i].usRecNum>0)
		{
			for(int k=0;k < info.sideinfo[i].usRecNum;k++)   //该股道的多辆车厢
			{
				rec = &info.sideinfo[i].rec[k];

				mMsgSimpleStationCarFormat.index = nSendIndex++; ///

				mMsgSimpleStationCarFormat.car_flag = rec->usCarriageExplain;   //方向标志
				mMsgSimpleStationCarFormat.car_type = rec->usCarriageType;      //车辆类型	 
				mMsgSimpleStationCarFormat.car_count = rec->usCarriageNum;      //车辆数目


				CString strText;
				int len = strlen(rec->lpszCarriageApply);
				if(len>0)
				{
					strText.Format("%s", rec->lpszCarriageApply);//运用
					strText +="^";
				}
				CString sRemark;
				sRemark.Format("%s", rec->lpszCarriageRemark);
				strText = strText + sRemark;

				memset(mMsgSimpleStationCarFormat.destine, 0, sizeof(mMsgSimpleStationCarFormat.destine));
				strncpy(mMsgSimpleStationCarFormat.destine, strText, sizeof(mMsgSimpleStationCarFormat.destine)-1);      //车辆去向

				mMsgSimpleStationCarFormat.serialize(ar);////
			}
		}
		//只有空的股道
		else if(info.sideinfo[i].usRecNum == 0)
		{
			mMsgSimpleStationCarFormat.index = nSendIndex++; ///		
			mMsgSimpleStationCarFormat.car_flag =0;   //方向标志
			mMsgSimpleStationCarFormat.car_type = 0;      //车辆类型	 
			mMsgSimpleStationCarFormat.car_count = 0;      //车辆数目	
			strcpy(mMsgSimpleStationCarFormat.destine,"");      //车辆去向		
			mMsgSimpleStationCarFormat.serialize(ar);////
		}
	}
	//end of msg data  

	//msg end
	CLBTH::MsgSimpleStationCarCtrl  mMsgSimpleStationCarCtrlEnd;
	mMsgSimpleStationCarCtrlEnd.input_time = t_input;
	mMsgSimpleStationCarCtrlEnd.station = StationIndex;
	mMsgSimpleStationCarCtrlEnd.phase = (CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE)m_nCurrentPhase;
	mMsgSimpleStationCarCtrlEnd.status = CLBTH::MsgSimpleStationCarCtrl::MSG_END;
	mMsgSimpleStationCarCtrlEnd.serialize(ar);
	//end msg 

	long eid=GetSpecialEntityID(SERVER_TYPE_DB,m_nSectionID);
	SendTGCommMessage(m_nSectionID,ar.getCount(),(char *)ar.getBuffer(),eid);
	GpcLogTrainMessage(0,0,"[更改站存车]: svr_entity %d Station:%d",
		eid, StationIndex);
}

//向主机发送申请数据，请求一个车站的股道数据
void CQueryStationCurrentStoreTrain::GetStationCarsInfo(byte type,ushort StationIndex)
{
	CLBTH::MsgSimpleStationCarQuery mMsgSimpleStationCarQuery;
	SetStartEndtime(type, m_nShiftID, nEnd_time, nStart_time);
	mMsgSimpleStationCarQuery.begin_time = nStart_time;
	mMsgSimpleStationCarQuery.end_time = nEnd_time;
	mMsgSimpleStationCarQuery.phase = (CLBTH::MsgSimpleStationCarQuery::STACAR_PHASE)type;
	//only one station
	mMsgSimpleStationCarQuery.sta_count = 1;
	mMsgSimpleStationCarQuery.sta_list[0] = StationIndex;

	long eid=GetSpecialEntityID(SERVER_TYPE_DB,m_nSectionID);
	SendTGBMsg(m_nSectionID,&mMsgSimpleStationCarQuery,eid);
}

void CQueryStationCurrentStoreTrain::OnSave() 
{ 
	if(m_nCurrentSideIndex<0) 
		return;

	m_nCurrentStationIndex = m_nAllStationArray[m_nCurrentSelectStation];

	SendSideinfo(m_nCurrentStationIndex);


	//显示最新修改时间
	CWnd *pWnd = GetDlgItem(IDC_EDIT_TIME);  
	if(pWnd==NULL)
		return;
	CTime t=CTime::GetCurrentTime();

	CString s = t.Format( "%Y-%m-%d %H:%M" );
	pWnd->SetWindowText(s);

	bChangdedFlag = true;
}

//从配置文件中 line.cfg 中获得各个车站的股道信息
BOOL CQueryStationCurrentStoreTrain::InitStationSideInfo()
{ 
	return TRUE;
}

/************清除编辑框的内容****************/
void CQueryStationCurrentStoreTrain::ClearAllEdit()
{
	m_carriagetypeCtrl.SetCurSel(0);

	CString text;
	CWnd *pWnd = GetDlgItem(IDC_SCST_CARRIAGE_NUMBER);    //车厢数目
	if(pWnd==NULL) 
		return;
	pWnd->SetWindowText("");

	CSpinButtonCtrl *spinWnd;
	spinWnd = (CSpinButtonCtrl *)GetDlgItem(IDC_SCST_CARRIAGE_NUMBER_SPIN);  //数目
	if(spinWnd==NULL)
		return;
	spinWnd->SetPos(1);

	pWnd = GetDlgItem(IDC_SCST_CARRIAGE_DIRECTION);
	if(pWnd==NULL)
		return;
	pWnd->SetWindowText("");                           //车厢方向：上海

	m_carriageexplainCtrl.SetCurSel(0);
}


void CQueryStationCurrentStoreTrain::InitIdentify()
{
	if(!gpcdef.GetOperatorRight(STATION_STORE_TRAIN))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_SCST_SIDE_INSERT);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_EDIT_SIDE);
		pWnd->EnableWindow(FALSE);	
		pWnd = GetDlgItem(IDC_SCST_ACK);
		pWnd->EnableWindow(FALSE);	

		pWnd = GetDlgItem(IDC_SCST_SIDE_CHANGE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_SAVE);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_SCST_SIDE_DELETE);
		pWnd->EnableWindow(FALSE);	
	}
}

void CQueryStationCurrentStoreTrain::OnEditSide() 
{
	CDlgEditStationSideName dlg(&info);
	if(dlg.DoModal() == IDOK)
	{
		ShowStationSideTrain();
	}
}

void CQueryStationCurrentStoreTrain::ShowStationSideTrain()
{
	m_ctrlStationStoreTrainList.DeleteAllItems();

	struct _tagSideStoreCarriage *rec;	
	CString text,text2,tmp;

	text =text2 = "";

	if(info.usSideNum == 0)
	{
		return;
	}
	CString strSideName;
	struct _tagSideStoredTrain *sideinfo;

	int jzCount,jzCountK,szCount,szCountK,axCount,axCountK;
	jzCount = jzCountK = szCount = szCountK = axCount = axCountK = 0;


	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 0;


	int current_side_added_rownum;	//当前股道由于数据无法完全显示增加的行数
	int current_side_rownum;		//当前股道第一次加入列表时候的行号
	int all_sides_added_rownum;		//所有股道由于数据无法完全显示增加的行数
	int current_toadded_rownum;		//要增加的行的行号
	int sides_carriages_col_width;	//显示股道车辆的该列的长度

	CString current_side_fitted_str;	//没有超出数据长度标准的数据

	current_side_added_rownum = 0;
	all_sides_added_rownum = 0;
	current_toadded_rownum = 0;
	sides_carriages_col_width = 0;
	current_side_fitted_str = "";

	sides_carriages_col_width = m_ctrlStationStoreTrainList.GetColumnWidth(1);

	m_totalCount=0;
	for(int i=0; i<m_pBasicPara->m_carKindType.size(); i++)
	{
		for(int k=0; k<m_pBasicPara->m_carKindType[i].carKind.size(); k++)
		{
			m_pBasicPara->m_carKindType[i].carKind[k].count=0;
		}
	}

	for(int j=0;j < info.usSideNum;j++)    
	{  
		current_side_fitted_str = "";

		text = "";
		nSideCarriageCount=0;
		sideinfo = &info.sideinfo[j];
		strSideName = CString(sideinfo->pszSideName);

		current_toadded_rownum = j + all_sides_added_rownum;	//当前要增加的行号比股道数目可能大
		lvi.iItem = current_toadded_rownum;
		current_side_rownum = current_toadded_rownum;
		current_side_added_rownum = 0;	//当前股道已经增加的行数为0

		lvi.pszText = (char *)(LPCTSTR)strSideName;
		m_ctrlStationStoreTrainList.InsertItem(&lvi);

		CString strStyle;
		for(int k=0;k < sideinfo->usRecNum;k++)
		{ 
			rec = &sideinfo->rec[k];
			if(rec->usCarriageNum <=0)
				continue;
			strStyle = "";

			BYTE nSubSel = 0;
			BYTE nSel = 0;
			m_pBasicPara->GetCarTypeAndKindIndex(rec->usCarriageType, nSel, nSubSel);

			if(nSel >= m_pBasicPara->m_carKindType.size())
				continue;

			if(nSubSel >= m_pBasicPara->m_carKindType[nSel].carKind.size())
				continue;

			m_totalCount += rec->usCarriageNum;
			m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].count += rec->usCarriageNum;

			strStyle = m_pBasicPara->m_carKindType[nSel].shortName;
			if(strStyle.IsEmpty())
				strStyle = m_pBasicPara->m_carKindType[nSel].name;

			text += strStyle;
			
			CString strRemark = rec->lpszCarriageRemark; strRemark.Trim();
			CString strCarriageApply = rec->lpszCarriageApply; strCarriageApply.Trim();
			if(strRemark.GetLength() > 0 || strCarriageApply.GetLength()>0)
			{
				tmp.Format("%s%s%d(%s %s) ",
					m_pBasicPara->m_aryWorkExplain.GetAt(rec->usCarriageExplain),
					m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
					rec->usCarriageNum,		
					strRemark, strCarriageApply);
			}
			else
			{
				tmp.Format("%s%s%d ",
					m_pBasicPara->m_aryWorkExplain.GetAt(rec->usCarriageExplain),
					m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
					rec->usCarriageNum);
			}
			text += tmp;

			nSideCarriageCount+=  rec->usCarriageNum;
			sideinfo->CarriageCounts=nSideCarriageCount;   ///该股道的车厢数目
			text2.Format("%d",nSideCarriageCount);	 

			//要显示的数据超出了该列长度
			if(m_ctrlStationStoreTrainList.GetStringWidth(text) >= sides_carriages_col_width) 
			{
				//我们必须插入新行，如果第一次数据显示就超出了长度，我们不进行处理
				if(current_side_fitted_str.IsEmpty())
					m_ctrlStationStoreTrainList.SetItemText(current_side_rownum + current_side_added_rownum,1,text);      //该股道的信息;
				else	//不是第一次数据，我们增加一行
				{
					m_ctrlStationStoreTrainList.SetItemText(current_side_rownum + current_side_added_rownum,1,current_side_fitted_str);      //该股道的信息

					lvi.iItem = current_side_rownum + current_side_added_rownum+1;
					strcpy(lvi.pszText,"");
					m_ctrlStationStoreTrainList.InsertItem(&lvi);

					current_side_added_rownum++;
					all_sides_added_rownum++;

					text = strStyle + tmp;
					current_side_fitted_str = "";
				}
			}

			//没有超出，采用覆盖方式写原来位置
			//这儿有两种情况，一个就是没有超出的情况，还有一个就是已经超出以后被分段了
			current_side_fitted_str = text;
			m_ctrlStationStoreTrainList.SetItemText(current_side_rownum + current_side_added_rownum,1,text);      //该股道的信息;

			//股道数量总是记录在该股道分配的第一个行号上面
			m_ctrlStationStoreTrainList.SetItemText(current_side_rownum,2,text2);   //该股道的数量
		}

		CString strCount;

		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_TOTAL_SUM);
		strCount.Format("%d", m_totalCount); 
		if(pWnd != NULL)
			pWnd->SetWindowText(strCount);

		ShowSideRecord(m_nCurrentSideIndex);     //显示首条股道的信息
		ShowTrainStatisticInfo();
	}
}
BOOL CQueryStationCurrentStoreTrain::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	int idCtrl = (int) wParam;
	if(idCtrl != IDC_SCST_STATION_STORE_LIST && idCtrl != IDC_SCST_SIDE_STORE_LIST) 
		return CDialog::OnNotify(wParam, lParam, pResult);

	NMHDR *nmgv = (NMHDR*)lParam;
	int nMessageType=nmgv->code;


	POSITION pos = NULL;
	int nItem;

	if(idCtrl == IDC_SCST_STATION_STORE_LIST)
	{
		int newsideindex;
		switch(nMessageType)
		{
		case LVN_ITEMCHANGED:

			pos = m_ctrlStationStoreTrainList.GetFirstSelectedItemPosition();
			if(pos == NULL)
				break;
			nItem = m_ctrlStationStoreTrainList.GetNextSelectedItem(pos);

			newsideindex = SideRowNumToIndex(nItem);
			if( newsideindex == m_nCurrentSideIndex)	//要改变的记录和原来一样，不管
				break;
			else
			{
				m_nCurrentSideIndex = newsideindex;
				ShowSideRecord(m_nCurrentSideIndex);
			}

			break;

		default:
			break;
		}

	}

	if(idCtrl == IDC_SCST_SIDE_STORE_LIST)
	{
		switch(nMessageType)
		{
		case LVN_ITEMCHANGED:

			pos = m_ctrlSideStoreTrainList.GetFirstSelectedItemPosition();
			if(pos == NULL)
				break;
			nItem = m_ctrlSideStoreTrainList.GetNextSelectedItem(pos);

			if(nItem == m_nCurrentCarriageIndex)	//要改变的记录和原来一样，不管
				break;
			else

			{
				m_nCurrentCarriageIndex = nItem;
				ShowCarriageRecord(&(info.sideinfo[m_nCurrentSideIndex].rec[m_nCurrentCarriageIndex]));

			}

			break;

		default:
			break;
		}

	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}


//把在LIST CTRL中的股道行号转换成对应在info中的索引号
int CQueryStationCurrentStoreTrain::SideRowNumToIndex(int rownum)
{

	if(rownum <=0 )
		return rownum;

	int ret_index = 0;
	char str[100];
	for(int i=0; i<=rownum;i++)//判断到该行总共的股道名字不为空的行的总数
		//那么相对保存股道数据的索引就是总数-1
	{
		m_ctrlStationStoreTrainList.GetItemText(i,0,str,100);
		if(strlen(str)>0)
			ret_index++;
	}

	return ret_index - 1;

}



void Drawf_QuseryStation(CDC* pDC,CPrintInfo* pInfo,void* pVoid)
{	
	((CQueryStationCurrentStoreTrain*)pVoid)->OnPrint(pDC,pInfo);	
}

void CQueryStationCurrentStoreTrain::OnPrint(CDC* pDC,CPrintInfo* pInfo)
{
	CString disp_msg,station_name;
	CWnd* cwnd_cur_std;
	cwnd_cur_std = GetDlgItem(IDC_SCST_STATIONNAME);
	cwnd_cur_std->GetWindowText(station_name);

	CPen pen;
	pen.CreatePen(PS_SOLID,2,CLR_BLACK);
	CPen *pOldPen=pDC->SelectObject(&pen);

	CFont font,*oldfont;
	CPoint point;
	//打印标题
	CString ss=_T(station_name + "站现存车数统计");
	int nWidth=pDC->GetDeviceCaps(HORZRES);
	int nHeight=pDC->GetDeviceCaps(VERTRES);
	int ghdf=pDC->GetDeviceCaps(LOGPIXELSY);
	CRect DrawRect(0,0,nWidth,nHeight),rect,tempRect;		
	DrawRect.InflateRect(-100,-50,-100,-20);
	//pDC->Rectangle(&DrawRect);		
	font.CreateFont(-MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));
	oldfont=pDC->SelectObject(&font);
	tempRect=DrawRect;
	tempRect.InflateRect(0,-5,0,0);

	rect=tempRect;rect.InflateRect(10,0);
	point=CPoint(DrawRect.left+(DrawRect.Width()-rect.Width())/2,rect.bottom+2);
	pDC->MoveTo(point);
	pDC->LineTo(point.x+rect.Width(),point.y);
	point.Offset(0,2);
	pDC->MoveTo(point);
	pDC->LineTo(point.x+rect.Width(),point.y);
	tempRect.right=DrawRect.right;
	pDC->DrawText(ss,&tempRect,DT_CENTER|DT_TOP|DT_SINGLELINE);	
	pDC->SelectObject(oldfont);
	font.DeleteObject();
	//显示统计的参数


	CString strStation="统计车站：";
	strStation +=station_name;

	long loginshiftid =  m_nShiftID;

	CString strDate ="班别: " + ConvertShiftIdToString(loginshiftid);
	CString temp;


	font.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	pDC->SelectObject(&font);

	pDC->TextOut(rect.left+100,rect.top+250,strDate);
	pDC->TextOut(rect.left+100,rect.top+350,strStation);



	CFont font_title;
	font_title.CreateFont(-MulDiv(11,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	CFont font_detail;
	font_detail.CreateFont(-MulDiv(8,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));

	//rect.InflateRect(-100,-630,-100,-200);
	rect.InflateRect(-100,-500,-100,-200);
	pDC->Rectangle(&rect);	     


	//要打印的行数
	int row_count = m_ctrlStationStoreTrainList.GetItemCount()+1;
	CString side_name,side_name_next;
	side_name = "";
	side_name_next = "";

	int yStep =	150;//(rect.bottom-rect.top)/2/row_count;
	int i = 0;

	//自动调整行距
	while((row_count+8)*yStep > (rect.bottom - rect.top))
		yStep-=5;

	//实际行数还要加上标题拦这一行
	for(i=0; i<=row_count; i++)  //行数的绘制
	{

		if(i>=2)//正式画股道的线，如果发现下面一行股道号为空，那么不画线
		{
			side_name = m_ctrlStationStoreTrainList.GetItemText(i-2,0);

			if(i < row_count)
				side_name_next = m_ctrlStationStoreTrainList.GetItemText(i-1,0);
			else
				side_name_next = "temp";

			if( side_name_next == "")  //该行股道号为空，表示为上面股道的信息太长而追加的行，不画线
			{
				//pDC->MoveTo(rect.left,rect.top + yStep*2);
				//pDC->TextOut(rect.left+10,rect.top ,"列车分类");
				//pDC->LineTo(rect.right,rect.top + yStep*2);
			}
			else
			{
				pDC->MoveTo(rect.left,rect.top + yStep*i);
				pDC->LineTo(rect.right,rect.top + yStep*i);
			}
		}
		else //画标题拦的底线
		{
			pDC->MoveTo(rect.left,rect.top + yStep*i);
			pDC->LineTo(rect.right,rect.top + yStep*i);
		}


	}


	//列数为三列，根据比例来绘制
	float first_col,second_col,third_col;
	int total_col_width;
	first_col = m_ctrlStationStoreTrainList.GetColumnWidth(0);
	second_col = m_ctrlStationStoreTrainList.GetColumnWidth(1);
	third_col = m_ctrlStationStoreTrainList.GetColumnWidth(2);
	total_col_width = first_col + second_col + third_col;
	first_col = first_col / total_col_width;
	second_col = second_col / total_col_width;
	third_col = third_col / total_col_width;

	for(int j=1; j<=4; j++)
	{
		if(j==1)	//可以不画，和矩形左边框重叠的那一条线
		{
			pDC->MoveTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.top + yStep*row_count);
		}
		else if(j==2)
		{
			pDC->MoveTo(rect.left + first_col*(rect.right-rect.left), rect.top);
			pDC->LineTo(rect.left + first_col*(rect.right-rect.left), rect.top + yStep*row_count);
		}
		else if(j==3)
		{
			pDC->MoveTo(rect.left + (first_col+second_col)*(rect.right-rect.left), rect.top);
			pDC->LineTo(rect.left + (first_col+second_col)*(rect.right-rect.left), rect.top + yStep*row_count);
		}

	}

	//


	//绘制标题栏文字

	CRect cellrect;
	int cell_left,cell_top,cell_right,cell_bottom;

	cell_left = rect.left;
	cell_top = rect.top + 15;
	cell_right = cell_left + first_col*(rect.right-rect.left);
	cell_bottom = rect.bottom + yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	oldfont = pDC->SelectObject(&font_title);
	pDC->DrawText(_gszStationStoreTrainLabel[0],cellrect,DT_CENTER);

	cell_left = cell_right;
	//cell_top = rect.top + 15;
	cell_right = cell_left + second_col*(rect.right-rect.left);
	//cell_bottom = rect.bottom + yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);
	pDC->DrawText(_gszStationStoreTrainLabel[1],cellrect,DT_CENTER);

	cell_left = cell_right;
	//cell_top = rect.top + 15;
	cell_right = cell_left + third_col*(rect.right-rect.left);
	//cell_bottom = rect.bottom + yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);
	pDC->DrawText(_gszStationStoreTrainLabel[2],cellrect,DT_CENTER);


	pDC->SelectObject(oldfont);



	int first_col_xpos,second_col_xpos,third_col_xpos;

	//显示内容
	row_count = row_count - 1; //数据的行数,row_count中以前包括了标题栏这一行

	first_col_xpos = rect.left + 20;
	second_col_xpos = first_col_xpos + first_col*(rect.right - rect.left);
	third_col_xpos = second_col_xpos + second_col*(rect.right - rect.left);

	CString side_content,side_cars_num;

	for(i=1; i<=row_count; i++)  
	{

		side_name = m_ctrlStationStoreTrainList.GetItemText(i-1,0);
		side_content = m_ctrlStationStoreTrainList.GetItemText(i-1,1);
		side_cars_num = m_ctrlStationStoreTrainList.GetItemText(i-1,2);

		pDC->TextOut(first_col_xpos, rect.top+yStep*i+15,side_name);
		pDC->TextOut(second_col_xpos, rect.top+yStep*i+15,side_content);
		pDC->TextOut(third_col_xpos, rect.top+yStep*i+15,side_cars_num);

	}

	//判断下面统计数据的起始位置

	int static_rect_top; 
	//原定的起始位置在股道信息这些内容里面，我们必须把起始位置往下面挪
	if((row_count+1)*yStep > (rect.bottom-rect.top)/2)	
		static_rect_top = rect.top + (row_count+1)*yStep + 50 ;
	else
		static_rect_top = rect.top + (rect.bottom-rect.top)/2 +100;

	//重新建立一个RECT，从原来矩形的中间部分开始
	CRect static_rect(rect.left,static_rect_top,rect.right,rect.bottom);

	//画行，yStep同上面的一样
	yStep = (static_rect.top + yStep*8 > static_rect.bottom)?((static_rect.bottom-static_rect.top)/8):yStep;

	for(i=0; i<=6; i++)  //行数的绘制，共6行7条横线
	{
		pDC->MoveTo(static_rect.left,static_rect.top + yStep*i);
		pDC->LineTo(static_rect.right,static_rect.top + yStep*i);

	}

	int xStep = (static_rect.right - static_rect.left)/7;
	//画列，xStep自己重新计算
	for(i=0; i<7; i++)  //列数的绘制，共6列7条竖线，最后一条术线已经画过，这儿不重画
	{
		pDC->MoveTo(static_rect.left + xStep*i,static_rect.top);
		pDC->LineTo(static_rect.left + xStep*i,static_rect.top + 6*yStep);
	}



	//绘制标题


	/*
	|------|-------|-------|-------|-------|--------|------|	<----标题栏
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|	<----标题栏
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|

	第一列为重车空车信息
	^	
	|
	|
	|
	*/



	for(i=1; i<=6; i++)
	{
		cellrect.SetRect(static_rect.left + xStep*i,static_rect.top+15,static_rect.left + xStep*(i+1),static_rect.top+yStep);
		//tmp//pDC->DrawText(_gszCarriageType[i-1].lpszCarriageLongName ,cellrect,DT_CENTER);
	}

	for(i=1; i<=6; i++)
	{
		cellrect.SetRect(static_rect.left + xStep*i,static_rect.top+15+yStep*3,static_rect.left + xStep*(i+1),static_rect.top+yStep*4);
		//tmp//pDC->DrawText(_gszCarriageType[i-1+6].lpszCarriageLongName,cellrect,DT_CENTER);
	}

	//绘制重车空车

	cell_left = static_rect.left;
	cell_top = static_rect.top + yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("重车",cellrect,DT_CENTER);

	cell_top = cell_top + yStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("空车",cellrect,DT_CENTER);

	cell_top = cell_top + 2*yStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("重车",cellrect,DT_CENTER);

	cell_top = cell_top + yStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("空车",cellrect,DT_CENTER);

	//填空车辆数	

	cell_left = static_rect.left;
	cell_top = static_rect.top + yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;

	//第一排重车 	

	CString tempstr;
	for(i=1; i<=6; i++)
	{
		cell_left = cell_left + xStep;
		cell_right = cell_left + xStep;

		cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

		switch(i)
		{
		case 1://篷车，
			tempstr.Format("%d",m_pCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;;
		case 2://敞车
			tempstr.Format("%d",m_cCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 3://平板车
			tempstr.Format("%d",m_pbCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 4://油罐车
			tempstr.Format("%d",m_yCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 5://水泥车
			tempstr.Format("%d",m_sCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 6://保温车
			tempstr.Format("%d",m_bCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		default:
			break;

		}

	}


	//第一排空车
	//	m_pCount = m_cCount = m_pbCount = m_yCount = m_sCount = m_bCount = 0;
	cell_left = static_rect.left;
	cell_top = static_rect.top + 2*yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	for(i=1; i<=6; i++)
	{
		cell_left = cell_left + xStep;
		cell_right = cell_left + xStep;

		cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

		switch(i)
		{
		case 1://篷车，
			tempstr.Format("%d",m_pCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;;
		case 2://敞车
			tempstr.Format("%d",m_cCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 3://平板车
			tempstr.Format("%d",m_pbCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 4://油罐车
			tempstr.Format("%d",m_yCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 5://水泥车
			tempstr.Format("%d",m_sCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 6://保温车
			tempstr.Format("%d",m_bCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		default:
			break;

		}

	}

	cell_left = static_rect.left;
	cell_top = static_rect.top + 4*yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	for(i=1; i<=6; i++)
	{
		cell_left = cell_left + xStep;
		cell_right = cell_left + xStep;

		cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

		switch(i)
		{
		case 1://毒品车
			tempstr.Format("%d",m_dCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;;
		case 2://集装箱车
			tempstr.Format("%d",m_jzCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 3://石渣车
			tempstr.Format("%d",m_szCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 4://客车底
			tempstr.Format("%d",m_kCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 5://凹型平板
			tempstr.Format("%d",m_axCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 6://其他车种
			tempstr.Format("%d",m_qCount);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		default:
			break;

		}

	}


	//第二排空车 	
	cell_left = static_rect.left;
	cell_top = static_rect.top + 5*yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	for(i=1; i<=6; i++)
	{
		cell_left = cell_left + xStep;
		cell_right = cell_left + xStep;

		cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

		switch(i)
		{
		case 1://毒品车
			tempstr.Format("%d",m_dCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;;
		case 2://集装箱车
			tempstr.Format("%d",m_jzCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 3://石渣车
			tempstr.Format("%d",m_szCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 4://客车底
			tempstr.Format("%d",m_kCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 5://凹型平板
			tempstr.Format("%d",m_axCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		case 6://其他车种
			tempstr.Format("%d",m_qCountK);
			pDC->DrawText(tempstr,cellrect,DT_CENTER);
			break;
		default:
			break;

		}

	}
	//打印总数
	cell_left = static_rect.left + 15;
	cell_top = static_rect.top + yStep*7;
	cell_right = cell_left + 2*xStep;
	cell_bottom = cell_top + 2*yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	tempstr.Format("总数：%d",m_totalCount);

	pDC->DrawText(tempstr,cellrect,DT_LEFT);

	pDC->SelectObject(pOldPen);

	pen.DeleteObject();
	font.DeleteObject();
	font_title.DeleteObject();
	font_detail.DeleteObject();
}

void CQueryStationCurrentStoreTrain::OnButtonPrint() 
{
	Beep(0xFF,60);
	CString disp_msg,station_name;
	CWnd* cwnd_cur_std;
	cwnd_cur_std = GetDlgItem(IDC_SCST_STATIONNAME);
	cwnd_cur_std->GetWindowText(station_name);

	disp_msg = "打印" + station_name + "站现存车信息?";
	if(AfxMessageBox(disp_msg,MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES  )
		return; 


	PrintSingleStation();
}

CString CQueryStationCurrentStoreTrain::ConvertShiftIdToString(unsigned long id)
{

	CString date,banbie;

	int CurYear  = id/1000000;
	int CurMonth = id/10000-CurYear*100;
	int CurDay   = id/100-(id/10000)*100;
	date.Format("%4d年%2d月%2d日",CurYear,CurMonth,CurDay);
	int CurBanbie=id-(id/10)*10;
	banbie=(CurBanbie==1) ? "第一班" : "第二班";

	CString shift_id_string;
	shift_id_string = date + banbie;
	return shift_id_string;
}

//获取打印设备函数
extern BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);

BOOL CQueryStationCurrentStoreTrain::PrintSingleStation()
{
	CWinApp* cApp=AfxGetApp();
	ASSERT_VALID(cApp);

	HGLOBAL hDevMode;
	HGLOBAL hDevNames;

	CString csPlotterName;
	csPlotterName.Format("%s",gpcdef.GetPrinterName());
	if (!GetPrinterDevice((char *)(LPCTSTR)csPlotterName, &hDevNames, &hDevMode))
	{
		AfxMessageBox(_T("无法获取指定的打印机设备\n"));
		return FALSE;
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
	sPrintName.Format("%s",gpcdef.GetStationName(m_nCurrentSelectStation));
	sPrintName = sPrintName + "车站现存车打印";

	char sPrintDocName[32];
	memcpy(sPrintDocName,sPrintName,32);

	printDocumentName.cbSize = sizeof(DOCINFO);
	printDocumentName.lpszDocName = sPrintDocName;
	printDocumentName.lpszOutput = NULL;
	printDC.StartDoc(&printDocumentName);
	printDC.StartPage( );

	OnPrint(&printDC,&pInfo);

	printDC.EndPage( );

	printDC.EndDoc( );
	printDC.Detach( );
	DeleteDC(prnDc);

	GlobalFree(hDevNames);
	GlobalFree(hDevMode);
	return TRUE;
}

int CQueryStationCurrentStoreTrain::GetChineseSplitFlag(CString &chinesestr)
{

	int len = chinesestr.GetLength();
	char curChar;
	int chineseCount = 0;

	for(int i=0; i<len; i++)
	{
		curChar = chinesestr.GetAt(i);

		if(curChar<0)
		{
			chineseCount++;
		}

	}

	return chineseCount%2;
}

void CQueryStationCurrentStoreTrain::ShowTrainStatisticInfo()
{
	CString text, sTrainTypeName, strTitle;
	
	for(int i=0;i < m_pBasicPara->m_carKindType.size();i++)    
	{  
		for(int k=0; k<m_pBasicPara->m_carKindType[i].carKind.size(); k++)
		{   
			sTrainTypeName = m_pBasicPara->m_carKindType[i].carKind[k].name;

			for(int j=0; j<m_ctrlTrainStatisticList.GetItemCount();j++)
			{
				strTitle = m_ctrlTrainStatisticList.GetItemText(j, 0);
				if(strTitle == sTrainTypeName)
				{
					if(m_pBasicPara->m_carKindType[i].carKind[k].count>0)
						text.Format("%d",m_pBasicPara->m_carKindType[i].carKind[k].count);
					else 
						text.Empty();
					m_ctrlTrainStatisticList.SetItemText(j,1+i,text);
					
					break;
				}
			}
		}
	}
}
