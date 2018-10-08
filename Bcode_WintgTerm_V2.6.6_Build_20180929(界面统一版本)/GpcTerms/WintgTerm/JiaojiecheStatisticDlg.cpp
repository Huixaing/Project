// JiaojiecheStatisticDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "JiaojiecheStatisticDlg.h"
#include <afxdtctl.h>
#include "SuperGridCtrl.h"
#include "tgmsg.h"
#include "messages.h"
#include "function.h" 
#include "widgets.h"
#include ".\jiaojiechestatisticdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IsActualArrivalTime(flag)           ((flag & 0x0001) == 1)
#define IsActualDepartTime(flag)            ((flag & 0x0002) == 2)

#import  <msxml.dll> named_guids
using namespace MSXML;

CJiaojiecheStatisticDlg::CJiaojiecheStatisticDlg(CTgDocument *pTgDoc, int ntSectionNo, CWnd* pParent)
: CDialog(CJiaojiecheStatisticDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJiaojiecheStatisticDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pTgDoc = pTgDoc;
	m_nCurrentSectionNo = ntSectionNo;
}

void CJiaojiecheStatisticDlg::SetTrains()
{
	CWorkScheduleHolder *pholder=m_pTgDoc->GetTGDataHolder();
	CTG_TrainScheduleBase *pdata = pholder->GetWorkScheduleData();

	CTG_ScheduleInfo schd;
	pdata->GetScheduleInfo(schd); 
	long nBeginTm, nEndTm;
	int id = schd.m_nShiftID;
	long m_nShiftDate = (id/1000000 - 1990) * 380 + (id/10000-(id/1000000)*100 - 1) * 31 + id/100 - (id/10000)*100  - 1 ;
	if(schd.GetShiftNo() == 2)//6:00-18:00
	{
		nBeginTm = long_time(m_nShiftDate,6*3600);  
		nEndTm = long_time(m_nShiftDate,18*3600);				
	}
	else//18:00-6:00
	{
		nBeginTm = long_time(OsGetPrevDate(PMTG.ThreadId,m_nShiftDate),18*3600);
		nEndTm = long_time(m_nShiftDate,6*3600);					
	}

	CTG_TrainPtrGroup group;
	pdata->FindTrains(group, NULL_STATION, nBeginTm, nEndTm);

	m_vecSchdTrains.clear();
	int GroupCount = group.GetTrainCount();
	for (int i=0;i< GroupCount ;i++)
	{
		CTG_SingleTrain * train = group.GetTrainPtr(i);
		if(NULL == train)
		{
			continue;
		}

		CTG_SingleTrain single = *train;
		single.GetRecordList()->PurgeRecords();
		int nRealRecNum = train->GetRecordList()->GetRecordCount();
		int nRecNum = min(train->GetTrainInfo()->m_data.delete_flag, nRealRecNum);
		for(int idx = 0; idx < nRecNum; idx++)
		{
			CTG_TrainRecord rd = train->GetRecordList()->m_listRecords[idx];
			single.GetRecordList()->AddRecord(rd);
		}
		if(nRecNum <= 0)
			continue;

		m_vecSchdTrains.push_back(single);
	}
}
void CJiaojiecheStatisticDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJiaojiecheStatisticDlg)
	DDX_Control(pDX, IDC_COMBO_DERECTION, m_DirectionCtrl);
	DDX_Control(pDX, IDC_STATIC_FRAME, m_Frame);
	DDX_Control(pDX, IDC_COMBO_STATION, m_StationCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJiaojiecheStatisticDlg, CDialog)
	//{{AFX_MSG_MAP(CJiaojiecheStatisticDlg)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_CBN_SELENDOK(IDC_COMBO_STATION, OnSelComboStation)
	ON_BN_CLICKED(IDC_STATISTIC, OnStatistic)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedUpTrain)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedDownTrain)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJiaojiecheStatisticDlg message handlers
const char * sRawClass[6]    = {_T("客车"),_T("行包专列"),_T("货物列车"), _T("单机和路用列车"),_T("未知列车类型"),_T("合计")};
const char * sPersonClass[8] = {_T("动车组"),_T("直达旅客列车"),_T("特快旅客快车"),_T("快速旅客快车"),_T("普通旅客列车"),_T("临时旅客列车"),_T("回送空车底列车"),_T("因故折返旅客列车")};

const char * sPersonPutong[2] = {_T("普通旅客快车"),_T("普通旅客慢车")};
const char * sWuding[2] =      {_T("五定班列"),_T("其他货物列车")};
const char * sSingle[2] =      {_T("单机"),_T("其他")};


void CJiaojiecheStatisticDlg::OnOK() 
{	
	CDialog::OnOK();
}

int CJiaojiecheStatisticDlg::GetTrainTypeDesp(MSXML::IXMLDOMNodePtr pRoot, int layer)
{
	MSXML::IXMLDOMNodePtr pChildNode,pNode;

	MSXML::IXMLDOMNodeListPtr pListType = pRoot->selectNodes( "TYPE");
	if(0 == pListType->Getlength())
	{
		CString text;
		pChildNode = pRoot->selectSingleNode("@name");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			text = CString(v.bstrVal);
			SysFreeString(v.bstrVal);
			v.Clear();
		}

		BYTE nId=255;
		pChildNode = pRoot->selectSingleNode("@id");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			nId = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		else
		{
			CString msg;
			msg.Format("请注意train_statistic.xml中[%s]没有配置id",text);
			AfxMessageBox(msg);
			return 0;
		}

		TYPE_STATISTIC_INFO info;
		info.train_type = nId;
		strncpy(info.train_text, text.GetBuffer(), 64);
		info.train_text[63]=0;

		mTmpMapTrainDetail.insert(make_pair(nId, info));

		aryPlotTrainText[aryPlotTrainText.size()-1].childId.push_back(nId);
		return 0;
	}
	CString parentName="未知";
	pChildNode = pRoot->selectSingleNode("@name");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		parentName.Format("%s_%d", CString(v.bstrVal), layer-1);
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	for(int idx = 0; idx < pListType->Getlength(); idx++)
	{
		pNode = pListType->Getitem(idx);
		if(NULL == pNode)
			continue;

		CString name;
		pChildNode = pNode->selectSingleNode("@name");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			name = CString(v.bstrVal);
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		DirNode node;
		
		CString tmpName,plotName;
		plotName = name;
		tmpName.Format("%s_%d", name, layer);
		strncpy(node.dirName, tmpName, sizeof(node.dirName));
		node.dirName[sizeof(node.dirName)-1]=0;
		
		
		for(int i=0; i<layer; i++)
			plotName = "--" + plotName;
		strncpy(node.plotName, plotName, sizeof(node.plotName));
		node.plotName[sizeof(node.plotName)-1]=0;
		
		aryPlotTrainText.push_back(node);
			
		GetTrainTypeDesp(pNode, layer+1);
	}


	for(int p=0; p<aryPlotTrainText.size(); p++)
	{
		if(parentName == aryPlotTrainText[p].dirName)
		{
			aryPlotTrainText[p].childId.clear();

			int pos = parentName.Find('_');
			CString sLayer = parentName.Right(parentName.GetLength()-pos-1);
			int nLayer = atoi(sLayer);
			for(int c = p+1; c < aryPlotTrainText.size(); c++)
			{
				CString childName = aryPlotTrainText[c].dirName;
				int cpos = childName.Find('_');
				CString csLayer = childName.Right(childName.GetLength()-cpos-1);
				int ncLayer = atoi(csLayer);

				if(nLayer == ncLayer-1)
				{
					for(int n=0; n<aryPlotTrainText[c].childId.size(); n++)
					{
						aryPlotTrainText[p].childId.push_back(aryPlotTrainText[c].childId[n]);
					}
				}
			}
		}
	}
	return 0;
}

BOOL CJiaojiecheStatisticDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	MSXML::IXMLDOMNodePtr pRootNode;

	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return 0;
	}
	
	CString pathname = gpcdef.GetDataDir();
	pathname += "\\train_statistic.xml";
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		CString  prompt;
		prompt.Format("配置%s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", pathname);
		return 0;
	}
	
	pRootNode = pXMLDoc->selectSingleNode( "TRAIN");
	if(pRootNode == NULL)
		return 0;

	GetTrainTypeDesp(pRootNode, 0);
	
	nStationNum=gpcdef.GetInterfaceStationList(m_nCurrentSectionNo,StationList,sizeof(StationList)/sizeof(StationList[0]));
	for(int i=0;i<nStationNum;i++)
		m_StationCtrl.InsertString(i,_T(gpcdef.GetStationName(StationList[i])) );
	m_StationCtrl.SetCurSel(0);

	//to get direction num;
	int nDirectNum=gpcdef.GetEntryStationDirectNum(StationList[0]);
	for(i=0;i<nDirectNum;i++)
	{
		char* name = gpcdef.GetEntryStationDirectName(StationList[0],i);
		if(name != NULL)
			m_DirectionCtrl.InsertString(i,_T(name));
	}
	m_DirectionCtrl.SetCurSel(0);

	flag_up = 1;
	flag_down= 0;

	m_brGrnBrush.CreateSolidBrush(RGB(255,242,240));
	
	CRect rc;
	m_Frame.GetWindowRect(rc);
	ScreenToClient(rc);
	m_Frame.ShowWindow(SW_HIDE);
	if(m_List.GetSafeHwnd())
		m_List.MoveWindow(rc);

	std::map<int, TYPE_STATISTIC_INFO>::iterator it = mTmpMapTrainDetail.begin();
	for(; it!=mTmpMapTrainDetail.end(); it++)
	{
		it->second.real_in_count=0;
		it->second.plan_in_count=0;
		it->second.real_out_count=0;
		it->second.plan_out_count=0;
	}

	return true;
}

HBRUSH CJiaojiecheStatisticDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	//if(m_stationCtrl.m_hWnd==pWnd->m_hWnd)

	switch(nCtlColor)
	{
	case CTLCOLOR_SCROLLBAR:
		pDC->SetBkColor(RGB(213,253,224));
		return	(HBRUSH) m_brGrnBrush;
	case CTLCOLOR_EDIT:
		break;
		pDC->SetBkColor(RGB(213,253,224));
		return	(HBRUSH) m_brGrnBrush;
	case CTLCOLOR_LISTBOX:
		pDC->SetBkColor(RGB(213,253,224));
		pDC->SetBkMode(TRANSPARENT);
		return	(HBRUSH) m_brGrnBrush;

	}	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

int CJiaojiecheStatisticDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//to create the tree list in dialog
	m_List.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0,0,0,0),this,0x1001);	
	//to init the grid;
	m_List.InitializeGrid();
	return 0;
}

void CJiaojiecheStatisticDlg::OnSelComboStation() 
{
	CComboBox * pComboBox;
	pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_STATION);    
	if(pComboBox == NULL)
		return;
	int nSel = pComboBox->GetCurSel();
	if(nSel == CB_ERR || nSel < 0)
		return;

	int stationNo = StationList[nSel];

	m_DirectionCtrl.ResetContent();
	int nDirectNum=gpcdef.GetEntryStationDirectNum(stationNo);
	for(int i=0;i<nDirectNum;i++)
	{
		char*name = gpcdef.GetEntryStationDirectName(stationNo,i);
		if(NULL != name)
			m_DirectionCtrl.InsertString(i,_T(name));
	}
	m_DirectionCtrl.SetCurSel(0);

	OnStatistic();
}

void CJiaojiecheStatisticDlg::ShowStatisticInfo()
{   
	// 1 selected station no 
	int nStaSel = m_StationCtrl.GetCurSel();
	if(nStaSel >= nStationNum)
		return;  
	
	int iStationNo = StationList[nStaSel];
	if(m_staStatisticInfo.nStationNo != iStationNo)
		return;

	// 2 selected station direction 
	int nDirecSel = m_DirectionCtrl.GetCurSel();
	if(m_staStatisticInfo.nDirectNo != nDirecSel)
		return;

	for(int i=0; i<aryPlotTrainText.size();i++)
	{
		memset(aryPlotTrainText[i].num, 0, sizeof(aryPlotTrainText[i].num));
		for(int j=0; j<aryPlotTrainText[i].childId.size(); j++)
		{
			int type = aryPlotTrainText[i].childId[j];
			std::map<int, TYPE_STATISTIC_INFO>::iterator it = 
						m_staStatisticInfo.mapTrainDetail.find(type);
			if(it != m_staStatisticInfo.mapTrainDetail.end())
			{
				aryPlotTrainText[i].num[0] += it->second.real_in_count;
				aryPlotTrainText[i].num[1] += it->second.plan_in_count;
				
				aryPlotTrainText[i].num[2] += it->second.real_out_count;
				aryPlotTrainText[i].num[3] += it->second.plan_out_count;
			}
		}
		CString text = aryPlotTrainText[i].dirName;
		text = text.Left(text.Find('_'));

		int nCount[2];
		nCount[0] = aryPlotTrainText[i].num[2]*flag_down;
		nCount[1] = aryPlotTrainText[i].num[3]*flag_down;
		nCount[0] += aryPlotTrainText[i].num[0]*flag_up;
		nCount[1] += aryPlotTrainText[i].num[1]*flag_up;
		
		m_List.ToSearchAndUpdateItems(text.GetBuffer(), nCount);
	}

	int nCount[2];
	nCount[0]=0;nCount[1]=0;
	std::map<int, TYPE_STATISTIC_INFO>::iterator it = 
		m_staStatisticInfo.mapTrainDetail.begin();
	while(it != m_staStatisticInfo.mapTrainDetail.end())
	{
		nCount[0] += it->second.real_in_count*flag_up;
		nCount[1] += it->second.plan_in_count*flag_up;
				
		nCount[0] += it->second.real_out_count*flag_down;
		nCount[1] += it->second.plan_out_count*flag_down;
		it++;
	}
	m_List.ToSearchAndUpdateItems("合计", nCount);

	m_List.Invalidate(FALSE);
}
void CJiaojiecheStatisticDlg::DrawBottomExplain(CDC *pDC, CRect &rect, int xStep, int yStep)
{
	CString str,temp;
	str ="  调度台:";
	str += gpcdef.GetCurUserConsoleName();

	str +="  调度员姓名:";
	str += gpcdef.GetCurrentUsername();

	str += "          打印时间：";
	CTime t = CTime::GetCurrentTime();
	temp.Format("%d年%d月%d日 %d时%d分%d秒",
		t.GetYear(),t.GetMonth(),t.GetDay(),
		t.GetHour(),t.GetMinute(),t.GetSecond());
	str += temp;
	pDC->TextOut(rect.left+1*xStep,  rect.bottom+yStep*0.2,str);
}

extern BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);
void CJiaojiecheStatisticDlg::OnPrint() 
{
	if(AfxMessageBox("打印交接车数统计?",MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES  )
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
	sPrintName = "分界口车站统计";
	
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

void CJiaojiecheStatisticDlg::PrintEveryPageInfo(CDC *pDC, CPrintInfo *pInfo)
{
	if(this->GetSafeHwnd() == NULL || !::IsWindow(this->GetSafeHwnd()))
		return;
	
	CPen pen;
	pen.CreatePen(PS_SOLID,2,CLR_BLACK);
	CPen *pOldPen=pDC->SelectObject(&pen);

	CFont font,*oldfont;
	CPoint point;
	//标题1
	CString ss=_T("分界口车站交接车数统计");
	int nWidth=pDC->GetDeviceCaps(HORZRES);
	int nHeight=pDC->GetDeviceCaps(VERTRES);
	int ghdf=pDC->GetDeviceCaps(LOGPIXELSY);
	CRect DrawRect(0,0,nWidth,nHeight),rect,tempRect;		
	DrawRect.InflateRect(-100,-50,-100,-20);

	font.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
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

	int nStaSel = m_StationCtrl.GetCurSel();
	if(nStaSel > nStationNum)
		return ;  

	CString strStation="  统计车站:";
	strStation +=_T(gpcdef.GetStationName(StationList[nStaSel]));

	int select_shift_id=gpcdef.GetLoginShiftid();
	int myYear  = select_shift_id/1000000;
	int myMonth = select_shift_id/10000 - myYear*100;
	int myDay   = select_shift_id/100 - (select_shift_id/10000)*100;
	int myBanbie = select_shift_id - (select_shift_id/10)*10;


	CString strDate ="统计日期:";
	CString temp;

	temp.Format("%d年%d月%d日第%d班",myYear,myMonth,myDay,myBanbie);
	strDate += temp;

	CString strDirec = "统计方向:";
	m_DirectionCtrl.GetWindowText(temp);
	strDirec +=temp;

	CString strType = "统计类型:";
	if(flag_up==1)
		strType += "上行";
	else if(flag_down==1)
		strType += "下行";
	
	CFont font_detail;
	font_detail.CreateFont(-MulDiv(7,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));

	pDC->SelectObject(&font_detail);
	pDC->TextOut(rect.left+100, rect.top+250,strDate+strStation);
	pDC->TextOut(rect.left +rect.Width()*0.7,rect.top + 250,strDirec+strType);
	
	rect.InflateRect(-100,-480,-100,-200);
	pDC->Rectangle(&rect);	     

	int xStep = (rect.right-rect.left)/5;  //列数的绘制
	int yStep =((rect.bottom-rect.top)/(aryPlotTrainText.size()+1))-2;   
	for(int i=0;i<aryPlotTrainText.size()+1;i++)  //行数的绘制
	{
		pDC->MoveTo(rect.left,rect.top + yStep*(i+1));
		pDC->LineTo(rect.right,rect.top + yStep*(i+1));
	}
	for(int j=1;j<4;j++)
	{  
		pDC->MoveTo(rect.left + (j+1)*xStep, rect.top);
		pDC->LineTo(rect.left + (j+1)*xStep, rect.bottom);
	}

	pDC->SelectObject(oldfont);
	pDC->SelectObject(&font_detail);


	pDC->TextOut(rect.left+10,  rect.top+12 ,"列车分类");   
	pDC->TextOut(rect.left+2.2*xStep ,  rect.top+12 ,"实际");
	pDC->TextOut(rect.left+3.2*xStep ,  rect.top+12 ,"计划");
	pDC->TextOut(rect.left+4.2*xStep ,  rect.top+12 ,"合计");
	
	CString text = "";
	int nCount[2];
	for(i=0;i<aryPlotTrainText.size();i++)
	{
		text = aryPlotTrainText[i].plotName;
		
		nCount[0] = aryPlotTrainText[i].num[2]*flag_down;
		nCount[1] = aryPlotTrainText[i].num[3]*flag_down;
		nCount[0] += aryPlotTrainText[i].num[0]*flag_up;
		nCount[1] += aryPlotTrainText[i].num[1]*flag_up;

		pDC->TextOut(rect.left+10,                  rect.top+yStep*(i+1)+16, text);
		temp.Format("%d", nCount[0]);
		pDC->TextOut(rect.left+(2.5)*xStep,  rect.top+yStep*(1+i)+16 ,temp);
		temp.Format("%d", nCount[1]-nCount[0]);
		pDC->TextOut(rect.left+(3.5)*xStep,  rect.top+yStep*(1+i)+16 ,temp);
		temp.Format("%d", nCount[1]);
		pDC->TextOut(rect.left+(4.5)*xStep,  rect.top+yStep*(1+i)+16 ,temp);
	}

	nCount[2];
	nCount[0]=0;nCount[1]=0;
	std::map<int, TYPE_STATISTIC_INFO>::iterator it = 
		m_staStatisticInfo.mapTrainDetail.begin();
	while(it != m_staStatisticInfo.mapTrainDetail.end())
	{
		nCount[0] += it->second.real_in_count*flag_up;
		nCount[1] += it->second.plan_in_count*flag_up;
				
		nCount[0] += it->second.real_out_count*flag_down;
		nCount[1] += it->second.plan_out_count*flag_down;
		it++;
	}
	i=aryPlotTrainText.size();
	pDC->TextOut(rect.left+10,               rect.top+yStep*(i+1)+16, "合计");
	temp.Format("%d", nCount[0]);
	pDC->TextOut(rect.left+(2.5)*xStep,  rect.top+yStep*(1+i)+16 ,temp);
	temp.Format("%d", nCount[1]-nCount[0]);
	pDC->TextOut(rect.left+(3.5)*xStep,  rect.top+yStep*(1+i)+16 ,temp);
	temp.Format("%d", nCount[1]);
	pDC->TextOut(rect.left+(4.5)*xStep,  rect.top+yStep*(1+i)+16 ,temp);

	//加尾注
	DrawBottomExplain(pDC,rect, xStep,  yStep);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(oldfont);

	pen.DeleteObject();
		
	font_detail.DeleteObject();
}

void CJiaojiecheStatisticDlg::OnBnClickedUpTrain()
{
	// TODO: 在此添加控件通知处理程序代码
	flag_up  = 1;
	flag_down = 0;

	OnStatistic();
}

void CJiaojiecheStatisticDlg::OnBnClickedDownTrain()
{
	// TODO: 在此添加控件通知处理程序代码
	flag_up  = 0;
	flag_down = 1;

	OnStatistic();
}

// 上行:0 下行:1 上下行:2
int GetTrainDirectionByTrainId(CString strTrainId)
{
	// 去掉非数字
	CString strLastNumTrainId="0";
	int len = strTrainId.GetLength();
	for(int i= len-1;i>=0;i--)
	{
		char lastchar = strTrainId.GetAt(i);
		if(0x30 <= lastchar && lastchar <= 0x39)
		{
			strLastNumTrainId = lastchar;
			break;
		}
	}

	len = atoi(strLastNumTrainId);
	if(len%2 == 0)
		return UP_DIRECT;
	else
		return DOWN_DIRECT;
}

void CJiaojiecheStatisticDlg::OnStatistic() 
{
	SetTrains();

	std::map<int, TYPE_STATISTIC_INFO>::iterator it = mTmpMapTrainDetail.begin();
	for(; it!=mTmpMapTrainDetail.end(); it++)
	{
		it->second.real_in_count=0;
		it->second.plan_in_count=0;
		it->second.real_out_count=0;
		it->second.plan_out_count=0;
	}
	
	CButton* pWnd = (CButton*)GetDlgItem(IDC_RADIO1);
	if(flag_up>0)
	{
		if(NULL !=pWnd)
			pWnd->SetCheck(1);
	}
	else
	{
		if(NULL !=pWnd)
			pWnd->SetCheck(0);
	}

	pWnd = (CButton*)GetDlgItem(IDC_RADIO2);
	if(flag_down>0)
	{
		if(NULL !=pWnd)
			pWnd->SetCheck(1);
	}
	else
	{
		if(NULL !=pWnd)
			pWnd->SetCheck(0);
	}

	CTG_ScheduleInfo schd;
	CWorkScheduleHolder *pholder=m_pTgDoc->GetTGDataHolder();
	CTG_TrainScheduleBase *pdata = pholder->GetWorkScheduleData();
	pdata->GetScheduleInfo(schd); 

	long nBeginTm, nEndTm;
	int id = schd.m_nShiftID;
	long m_nShiftDate = (id/1000000 - 1990) * 380 + (id/10000-(id/1000000)*100 - 1) * 31 + id/100 - (id/10000)*100  - 1 ;
	if(schd.GetShiftNo() == 2)//6:00-18:00
	{
		nBeginTm = long_time(m_nShiftDate,6*3600);  
		nEndTm = long_time(m_nShiftDate,18*3600);				
	}
	else//18:00-6:00
	{
		nBeginTm = long_time(OsGetPrevDate(PMTG.ThreadId,m_nShiftDate),18*3600);
		nEndTm = long_time(m_nShiftDate,6*3600);					
	}

	int nStaSel = m_StationCtrl.GetCurSel();
	if(nStaSel > nStationNum)
		return;  

	int iStationNo = StationList[nStaSel];
	int nDirecSel = m_DirectionCtrl.GetCurSel();

	//Get In and Out entry exit;
	int InList[20],OutList[20];
	int nInNum  = gpcdef.GetEntryStationInListDirec(iStationNo,nDirecSel, InList);
	int nOutNum = gpcdef.GetEntryStationOutListDirec(iStationNo,nDirecSel,OutList);
	if(nInNum <0 || nOutNum<0)
		return;

	std::map<int, TYPE_STATISTIC_INFO> Info;
	Info.insert(mTmpMapTrainDetail.begin(), mTmpMapTrainDetail.end());
	
	//statistic for arrived trains
	for(int k = 0;k < nInNum;k++)
	{
		int entry_exit_no = InList[k]; 
		
		for(int i=0;i< m_vecSchdTrains.size();i++)
		{
			int nTrainType = m_vecSchdTrains[i].GetTrainInfo()->m_data.type;
			std::map<int, TYPE_STATISTIC_INFO>::iterator it = Info.find(nTrainType);
			if(it == Info.end())
			{
				sysprint(0,PMTG.ThreadId,"[接口车站统计]: no statistic train type:%d!", nTrainType);
				continue;
			}
			
			for(int j=0;j<m_vecSchdTrains[i].GetTrainRecordCount();j++)
			{
				CTG_TrainRecord *rec = m_vecSchdTrains[i].GetTrainRecord(j);
				if(rec == NULL)
					continue;

				if(rec->GetStationNo() != iStationNo)
					continue;
				if(rec->GetEntry() != entry_exit_no)
					continue;

				if(rec->GetArriveTime() < nBeginTm || rec->GetArriveTime() >= nEndTm)
					continue;

				// 判断方向
				CString inTrainId = rec->GetArriveTID();
				int nDirect = GetTrainDirectionByTrainId(inTrainId);
				if(flag_up>0)
				{	
					if(UP_DIRECT == nDirect) // 上行车
					{
						if(IsActualArrivalTime(rec->m_data.adjust_status))
						{
							it->second.real_in_count ++;
						}
						else
						{
							it->second.plan_in_count ++;
						}
					}
				}

				if(flag_down>0)
				{
					if(DOWN_DIRECT == nDirect) // 下行车
					{
						if(IsActualArrivalTime(rec->m_data.adjust_status))
						{
							it->second.real_out_count ++;
						}
						else
						{
							it->second.plan_out_count ++;
						}
					}
				}
			}
		}
	}

	//statistic of departed trains
	for(int k=0;k<nOutNum;k++)
	{
		int entry_exit_no = OutList[k];
		
		for(int i=0;i<m_vecSchdTrains.size();i++)
		{
			int nTrainType = m_vecSchdTrains[i].GetTrainInfo()->m_data.type;
			std::map<int, TYPE_STATISTIC_INFO>::iterator it = Info.find(nTrainType);
			if(it == Info.end())
			{
				sysprint(0,PMTG.ThreadId,"[接口车站统计]: no statistic train type:%d!", nTrainType);
				continue;
			}

			for(int j=0;j<m_vecSchdTrains[i].GetTrainRecordCount();j++)
			{
				CTG_TrainRecord *rec = m_vecSchdTrains[i].GetTrainRecord(j);
				if(rec == NULL)
					continue;

				if(rec->GetStationNo() != iStationNo)
					continue;
				if(rec->GetExit() != entry_exit_no)
					continue;

				if(rec->GetDepartTime() < nBeginTm || rec->GetDepartTime() >= nEndTm)
					continue;

				// 判断方向
				CString outTrainId = rec->GetDepartTID();
				int nDirect = GetTrainDirectionByTrainId(outTrainId);
				if(flag_up>0)
				{
					if(UP_DIRECT == nDirect) // 上行车
					{
						if(IsActualDepartTime(rec->m_data.adjust_status))
						{
							it->second.real_in_count ++;
						}
						else
						{
							it->second.plan_in_count ++;
						}
					}
				}

				if(flag_down>0)
				{
					if(DOWN_DIRECT == nDirect) // 下行车
					{
						if(IsActualDepartTime(rec->m_data.adjust_status))
						{
							it->second.real_out_count ++;
						}
						else
						{
							it->second.plan_out_count ++;
						}
					}
				}
			}
		}
	}

	m_staStatisticInfo.nDirectNo=nDirecSel;
	m_staStatisticInfo.nStationNo=iStationNo;
	m_staStatisticInfo.mapTrainDetail.clear();
	m_staStatisticInfo.mapTrainDetail.insert(Info.begin(), Info.end());
	
	ShowStatisticInfo();
	return;
}
void CJiaojiecheStatisticDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	OnOK();
}
