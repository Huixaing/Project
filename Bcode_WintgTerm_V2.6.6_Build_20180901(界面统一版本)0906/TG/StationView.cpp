// StationForm.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "messages.h"
#include "Stationview.h"
#include "tgform.h"


#include ".\dlg\DlgRouteCtrlMode.h"
#include ".\dlg\SuperPassDlg.h"
#include ".\dlg\DlgMessageBox.h"
#include ".\stationview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationView

IMPLEMENT_DYNCREATE(CStationView, CView)

CStationView::CStationView()
{
	m_pDoc = NULL;
	m_pOldBitmap = NULL;
	m_nCurSelDistanceNo = 0;
}

CStationView::~CStationView()
{
	
}


BEGIN_MESSAGE_MAP(CStationView, CView)
	//{{AFX_MSG_MAP(CStationView)
	ON_COMMAND(ID_SET_ERROR_SIDE, OnSetStationErrorSide)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationView drawing

void CStationView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_fontSide.CreatePointFont(70,"Arial");
	m_fontKilo.CreatePointFont(90,"Arial");
	m_fontStatus.CreateFont(
				12,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_NORMAL,                 // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				"宋体");                 // lpszFacename

	m_fontStation.CreateFont(
				16,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_NORMAL,                 // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				"宋体");                 // lpszFacename
	
	ASSERT(m_pDoc != NULL);

	int wide=0;
	if (m_pctrl->mConfigTg.m_context.IsWideStationView())
		wide=VIEW_PORT::STATION_VIEW_WIDTH2;
	else
		wide=VIEW_PORT::STATION_VIEW_WIDTH1;

	X[0] = wide-24; // ☆
	X[1] = wide-36;// ◇
	X[2] = 0;// 站名
	X[3] = wide;// 股道
	X[4] = 0;  // 进路
	X[5] = 14; // 分散
	X[6] = wide;
	
	ReLayoutBmpPageSize(wide);
	ShowStationString(&m_dcMemoryDC,CLR_GRAY);
}

/////////////////////////////////////////////////////////////////////////////
// CStationView diagnostics

#ifdef _DEBUG
void CStationView::AssertValid() const
{
	CView::AssertValid();
}

void CStationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStationView message handlers
void CStationView::OnDraw(CDC* pDC)
{
	CRect rectClient;
	GetClientRect(&rectClient);
	pDC->BitBlt(rectClient.left,rectClient.top,
		rectClient.Width(),GetSystemMetrics(SM_CYSCREEN),
		&m_dcMemoryDC, 0, m_pDoc->m_nVerticalPos, SRCCOPY);
}

void CStationView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	int wide=0;
	if (m_pctrl->mConfigTg.m_context.IsWideStationView())
		wide=VIEW_PORT::STATION_VIEW_WIDTH2;
	else
		wide=VIEW_PORT::STATION_VIEW_WIDTH1;
	
	if(lHint == MSG_RE_LAYOUT_BMP_SIZE)
	{
		ReLayoutBmpPageSize(wide);
		ShowStationString(&m_dcMemoryDC,CLR_GRAY);
	}

	if(lHint == VERTICAL_POSITION_CHANGE_WORK )
	{
		Invalidate(TRUE);
	}
	else if (lHint == FOCUS_SECTION_CHANGE) 
	{
		CRect rect(0, 0, wide,	m_pDoc->m_nPageHeight);
		m_dcMemoryDC.FillRect(&rect, &m_pDoc->m_brushWhite);
		m_dcMemoryDC.SetBkMode(TRANSPARENT);

		m_pDoc->InitStationVerticalPos(true);  
		ShowStationString(&m_dcMemoryDC,CLR_GRAY);//CLR_GREEN);
		Invalidate(TRUE);
	} 
	else if(lHint == UM_CHANGE_STATION_STATUS)
	{
		CRect rect(0, 0, wide,	m_pDoc->m_nPageHeight);
		m_dcMemoryDC.FillRect(&rect, &m_pDoc->m_brushWhite);
		m_dcMemoryDC.SetBkMode(TRANSPARENT);

		ShowStationString(&m_dcMemoryDC,CLR_GRAY);
		Invalidate(TRUE);
	}
	else if(lHint == GRAPH_ROOM_CHANGE)
	{
		CRect rect(0, 0, wide,	m_pDoc->m_nPageHeight);
		m_dcMemoryDC.FillRect(&rect, &m_pDoc->m_brushWhite);
		m_dcMemoryDC.SetBkMode(TRANSPARENT);

		ShowStationString(&m_dcMemoryDC,CLR_GRAY);
		Invalidate(true);
	}
}

void CStationView::OnShowStationSides() 
{
	int wide=0;
	if (m_pctrl->mConfigTg.m_context.IsWideStationView())
		wide=VIEW_PORT::STATION_VIEW_WIDTH2;
	else
		wide=VIEW_PORT::STATION_VIEW_WIDTH1;

	CRect rect(0, 0, wide,	m_pDoc->m_nPageHeight);
	m_dcMemoryDC.FillRect(&rect, &m_pDoc->m_brushWhite);
	
	m_dcMemoryDC.SetBkMode(TRANSPARENT);

	m_pDoc->InitStationVerticalPos(false);
	ShowStationString(&m_dcMemoryDC,CLR_GRAY);
	m_pDoc->UpdateAllViews(this, MSG_DOUBLE_CLICK_STATION, 0);
	Invalidate(TRUE);
}

void CStationView::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	if(m_pDoc == NULL)
		return;

	CPoint mouse_pos = point;
	ClientToScreen(&mouse_pos);
	point.y += m_pDoc->m_nVerticalPos;

	m_nCurSelDistanceNo = m_pDoc->GetDispDistanceIndex(point.y);
	if(m_nCurSelDistanceNo >= 0) 
	{
		const DispDistance* pdis = m_pDoc->GetDispDistanceByIdx(m_nCurSelDistanceNo);
		if(point.x <= 20 && pdis)
		{
			CDlgRouteCtrlMode dlg(m_pDoc, pdis->station_number);
			dlg.DoModal();
		}
		else if (point.x >= 30 && pdis && TG_TYPE::BASIC_TG == m_pDoc->GetTgType())
		{
			if (!m_pDoc->m_ptgctrl->mConfigTg.m_context.GetBasicSchdCanModify())
				return;

			if (!m_pDoc->m_ptgctrl->mConfigTg.m_context.IsErrorSideStation(pdis->station_number))
				return;

			CMenu hResourceMenu;
			CRect r;

			do 
			{
				if(m_pDoc == NULL)
					break;
				if(hResourceMenu.LoadMenu(IDR_RBUTTON_POPMENU) == 0)
					break;

				CMenu *hPopupMenu= hResourceMenu.GetSubMenu(0);
				if(hPopupMenu == NULL)	
					break;
				hPopupMenu->DeleteMenu(0,MF_BYPOSITION);

				hPopupMenu->AppendMenu(MF_STRING | MF_ENABLED,	ID_SET_ERROR_SIDE, "设置?股");
			
				hPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN |TPM_LEFTBUTTON,mouse_pos.x,mouse_pos.y,this,NULL);

				hPopupMenu = NULL;
				hResourceMenu.DestroyMenu( );
			}while(0);
		}
	}
}

void CStationView::OnMouseMove(UINT nFlags, CPoint point) 
{	
	m_pDoc->UpdateAllViews(NULL, MSG_MOUSE_MOVE_OUTSIDE, 0);
	CView::OnMouseMove(nFlags, point);
}

void CStationView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	point.y += m_pDoc->m_nVerticalPos;

	m_nCurSelDistanceNo = m_pDoc->GetDispDistanceIndex(point.y);
	if(m_nCurSelDistanceNo >= 0) 
	{
		m_pDoc->ReLayoutBmpPageSize(m_nCurSelDistanceNo);
		OnShowStationSides();
	}
}
void CStationView::ShowStationString(CDC * pDC, long lColor)
{
	int i;
	UINT nOldAlign,nOldBackMode,nOldColor;
	int textOffset = 502;
	
	CFont *pOldFont = NULL;
	CPen mThickPen,*pOldPen;
	UINT textAlign;


	mThickPen.CreatePen(PS_INSIDEFRAME,10,CLR_GRAY);
	nOldAlign = pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);
	nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	nOldColor = pDC->SetTextColor(lColor);
	pOldFont = pDC->SelectObject(&m_fontStation);
	pOldPen = pDC->SelectObject(&mThickPen);
	CSize sz = pDC->GetTextExtent("A");
	CString str;
	int nStationNo = 0;
   	
	X[2] = 30;// 站名
	
	for(i=0;i < m_pDoc->m_nDispDistanceNum;i++)
	{
		if (m_pctrl->mConfigTg.m_context.IsWideStationView())
		{
			X[0] = VIEW_PORT::STATION_VIEW_WIDTH2; // ☆
			X[1] = VIEW_PORT::STATION_VIEW_WIDTH2-12;// ◇
		}
		else
		{
			X[0] = VIEW_PORT::STATION_VIEW_WIDTH1; // ☆
			X[1] = VIEW_PORT::STATION_VIEW_WIDTH1-12;// ◇
		}

		
		// 计划状态
		pDC->SelectObject(&m_fontStatus);
		sz = pDC->GetTextExtent("☆");
		int StatusY = m_pDoc->m_sDispDistance[i].nStationYPos + sz.cy/2;
		pDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
		// STPC		
		pDC->SetTextColor(m_pDoc->m_sDispDistance[i].clStpcFlag);
		pDC->TextOut(X[0], StatusY,	"☆");
		// LIRC
		pDC->SetTextColor(m_pDoc->m_sDispDistance[i].clLircFlag);
		pDC->TextOut(X[1], StatusY,	"◇");
		
		pDC->SelectObject(&m_fontStation);
		CString strInhibitDir;
		if(m_pDoc->m_sDispDistance[i].inhibit_sametime_dir == 0)
			strInhibitDir = "▼";
		else if(m_pDoc->m_sDispDistance[i].inhibit_sametime_dir == 1)
			strInhibitDir = "▲";
		else if(m_pDoc->m_sDispDistance[i].inhibit_sametime_dir == 2)
			strInhibitDir = "■";
		else
			strInhibitDir = "";
		nStationNo = m_pDoc->m_sDispDistance[i].station_number;

		if(TG_TYPE::BASIC_TG == m_pDoc->GetTgType())
		{
			str.Format("%s%s(%d)",m_pDoc->m_sDispDistance[i].station_name, strInhibitDir, 
									m_pDoc->m_sDispDistance[i].station_number);
			X[2] = 5;
		}
		else
			str.Format("%s%s",m_pDoc->m_sDispDistance[i].station_name, strInhibitDir);

		if(TG_TYPE::WORK_TG != m_pDoc->GetTgType())
		{
			nOldColor = pDC->SetTextColor(RGB(0,0,0)); // RGB(0,200,50
		}
		else
		{
			if(m_pDoc->m_sDispDistance[i].usStationStatus ==  STATION_STATUS::ACTIVE)
				nOldColor = pDC->SetTextColor(RGB(0,0,0)); //RGB(0,200,50
			else
				nOldColor = pDC->SetTextColor(CLR_RED);
		}
		
		pDC->SelectObject(&m_fontStation);
		pDC->SetTextAlign(TA_LEFT|TA_BOTTOM);

		sz = pDC->GetTextExtent("车站名");
		int StNameY = m_pDoc->m_sDispDistance[i].nStationYPos + sz.cy/2;
		pDC->TextOut(X[2], StNameY,	str);
		
		if(m_pDoc->m_sDispDistance[i].show_sation_sep)
		{  
			pDC->TextOut(X[2]+10, m_pDoc->m_sDispDistance[i].nSecondStationYPos + sz.cy/2,str);
		}
		
		if(m_pDoc->m_bShowdistanceStone)
		{
			CSize sz = pDC->GetTextExtent(str);
			int kiloy = m_pDoc->m_sDispDistance[i].nStationYPos + sz.cy/2;
			CString dist_stone;
		
			//dist_stone.Format("K%.3f",m_pDoc->m_sDispDistance[i].distance);
			if (m_pDoc->m_bUseStringKiloMeter)
				dist_stone.Format("%s",m_pDoc->m_sDispDistance[i].distance);
			else
				dist_stone.Format("K%s",m_pDoc->m_sDispDistance[i].distance);
			dist_stone.Replace(".","+");
			int fontHeight = pDC->GetTextExtent(dist_stone).cy;
		   
			pDC->SelectObject(&m_fontKilo);
			pDC->SetTextColor(RGB(0,0,255));
			pDC->SetTextAlign(TA_LEFT|TA_TOP);
			pDC->TextOut(X[4], kiloy-3, dist_stone);
		}

		//显示自律机按图排路情况
		CString strLircStatus;
		pDC->SelectObject(&m_fontStatus);
		if(m_pDoc->m_sDispDistance[i].usLircStatus == STATION_STATUS::ACTIVE)
		{
			CSize sz = pDC->GetTextExtent("图");
			int LircY = m_pDoc->m_sDispDistance[i].nStationYPos + sz.cy/2;
			if(m_pDoc->m_sDispDistance[i].nRouteCtrlMode == ROUTE::PLAN_CTRL)//0)     //按图排路
			{
				nOldColor = pDC->SetTextColor(CLR_LAWNGREEN);
				strLircStatus = "图";

			}
			else if(m_pDoc->m_sDispDistance[i].nRouteCtrlMode == ROUTE::SAVE_ROUTE)//1) //存储控制
			{
				nOldColor = pDC->SetTextColor(CLR_DARKGRAY);
				strLircStatus = "存";

			}
			else if(m_pDoc->m_sDispDistance[i].nRouteCtrlMode == ROUTE::FORBIT_AUTO)//2) //人工排路
			{
				nOldColor = pDC->SetTextColor(CLR_GRAY);
				strLircStatus = "人";

			}
			else
			{ 
				nOldColor = pDC->SetTextColor(CLR_RED);
				strLircStatus = "";
			}
		
			nOldAlign = pDC->SetTextAlign(TA_LEFT |TA_BOTTOM);
			if(CTC_STATION != m_pDoc->GetStationProperty(nStationNo)) 
				strLircStatus.Empty();
			if(!strLircStatus.IsEmpty())
				pDC->TextOut(X[4],  LircY, strLircStatus);

			if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == STATION::DISTRIBUTED)//0)       //分散自律 
			{ 
				nOldColor = pDC->SetTextColor(CLR_LAWNGREEN);
				strLircStatus = "分";
			}
			else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == STATION::CENTER_CTRL)//1)  //中心控制 
			{ 
				nOldColor = pDC->SetTextColor(CLR_BLUE);
				strLircStatus = "中";
			}
			else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == STATION::STATION_CTRL)//2)  //车站控制
			{
				nOldColor = pDC->SetTextColor(RGB(255,128,0)); 
				strLircStatus = "站";
			}
			else if(m_pDoc->m_sDispDistance[i].nStationCtrlMode == STATION::EMERGENCY)//3)  //非常站控
			{ 
				nOldColor = pDC->SetTextColor(CLR_GRAY);
				strLircStatus = "非";
			}
			else
			{
				nOldColor = pDC->SetTextColor(CLR_RED);
				strLircStatus = "";
			}

			nOldAlign = pDC->SetTextAlign(TA_LEFT |TA_BOTTOM);
			if(CTC_STATION != m_pDoc->GetStationProperty(nStationNo)) 
				strLircStatus.Empty();
			if(!strLircStatus.IsEmpty())
				pDC->TextOut(X[5],  LircY, strLircStatus);
		}
		else
		{
			CSize sz = pDC->GetTextExtent("断");
			int LircY = m_pDoc->m_sDispDistance[i].nStationYPos + sz.cy/2;
			nOldColor = pDC->SetTextColor(CLR_RED);
			strLircStatus = "断";
            if(TG_TYPE::WORK_TG != m_pDoc->GetTgType())
				strLircStatus.Empty();
			
			nOldAlign = pDC->SetTextAlign(TA_LEFT |TA_BOTTOM);
			if(CTC_STATION != m_pDoc->GetStationProperty(nStationNo)) 
				strLircStatus.Empty();
			if(!strLircStatus.IsEmpty())
			{	
				pDC->TextOut(X[4],  LircY, strLircStatus);
			}
		}
		
		nOldAlign = pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);
		pDC->SetTextColor(nOldColor);

		if(m_pDoc->m_sDispDistance[i].show_sation_sep
			&& m_pDoc->m_sDispDistance[i].show_sides_mode ==0)
		{  
			pDC->SelectObject(&m_fontSide);
			CSize sz = pDC->GetTextExtent("IG");
			textAlign = pDC->GetTextAlign();
			pDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
			int sideno,position;
			CString str;
			for(int ii = 0; ii <  sizeof(m_pDoc->m_sDispDistance[i].side)/sizeof(m_pDoc->m_sDispDistance[i].side[0]); ii++)
			{ 
				sideno = m_pDoc->m_sDispDistance[i].side[ii].side_number;
				if(sideno == -1)
					continue;
				
				str.Format("%s",m_pDoc->m_sDispDistance[i].side[ii].sidename);
				position =  m_pDoc->m_sDispDistance[i].nSideYPos[sideno];
		
				pDC->TextOut(X[3],	position + sz.cy/2, _T(str));
			}
			pDC->SelectObject(pOldFont);
			pDC->SetTextAlign(textAlign);
		}
	}

	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldPen);
	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SetTextColor(nOldColor);

	BOOL ret = mThickPen.DeleteObject();

	if (m_pDoc->m_bShowVirtualStation)
		ShowVirtualStationString(pDC, lColor);

}

void CStationView::ShowVirtualStationString(CDC * pDC, long lColor)
{
	std::map<int , SectionVirtualStation>::iterator it = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.m_TGVirtualStation.find(m_pDoc->m_nCurrentSectionNo);
	if(it == m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.m_TGVirtualStation.end())
		return;

	CString str;

	UINT nOldAlign = pDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
	UINT nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF nOldColor = pDC->SetTextColor(RGB(255,0,0));
	CFont* pOldFont=pDC->SelectObject(&m_fontStatus);

	if(TG_TYPE::BASIC_TG == m_pDoc->GetTgType())
		pDC->SetTextColor(RGB(0,0,0));
    
	CSize sz = pDC->GetTextExtent("A");
	for(int i=0; i<it->second.count; i++)
	{
		int perStationIndex=it->second.m_virtualStation[i].perStationIndex;
		int nextStationIndex=it->second.m_virtualStation[i].nextStationIndex;
		
		if(nextStationIndex<=perStationIndex)
			continue;

		if(perStationIndex<0)
			continue;

		if(nextStationIndex>=m_pDoc->m_nDispDistanceNum)
			continue;

		int delta = m_pDoc->m_sDispDistance[nextStationIndex].nStationYPos - 
				            m_pDoc->m_sDispDistance[perStationIndex].nSecondStationYPos;

		CString strStaName=it->second.m_virtualStation[i].strStatinName;
		int nStation=it->second.m_virtualStation[i].nStation;
		int nPercent=it->second.m_virtualStation[i].nPercent;

		if(strStaName=="")
			strStaName="未知虚拟站";

		if(TG_TYPE::BASIC_TG == m_pDoc->GetTgType())
		{
			str.Format("%s(%d)", strStaName,  nStation);
			X[2] = 5;
		}
		else
			str.Format("%s", strStaName);

		sz = pDC->GetTextExtent(strStaName);
		int StNameY = m_pDoc->m_sDispDistance[perStationIndex].nSecondStationYPos + (delta*nPercent)/100 +sz.cy/2;
		pDC->TextOut(X[2], StNameY,	str);
	}
	
	pDC->SelectObject(pOldFont);
	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SetTextColor(nOldColor);

	return;
}

void CStationView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_pOldBitmap != NULL)
	{
		m_dcMemoryDC.SelectObject(m_pOldBitmap);
		m_bmpMemBitmap.DeleteObject();
		m_dcMemoryDC.DeleteDC();
		m_pOldBitmap = NULL;
	}	
	m_fontSide.DeleteObject();
	m_fontKilo.DeleteObject();
	m_fontStatus.DeleteObject();
	m_fontStation.DeleteObject();
}

void CStationView::OnSetStationErrorSide()
{
	if(m_nCurSelDistanceNo>=0)
	{
		const DispDistance* pdis = m_pDoc->GetDispDistanceByIdx(m_nCurSelDistanceNo);

		if (pdis)
		{
			CString text;
			text.Format("设置所有列车股道为?股,请输入密码", pdis->station_name);
			CSuperPassDlg pdlg(text);
			if (pdlg.DoModal() == IDOK)
			{
				if (pdlg.m_super_pass.Compare("1234") != 0)
				{
					CString str;
					str.Format("%s","密码错误!修改股道失败!");
					CDlgMessageBox dlg(str);
					dlg.DoModal();
					return;
				}
				else
				{
					m_pDoc->SetAllTrainsErrorSide(m_nCurSelDistanceNo,pdis->station_number);
				}
			}
		}
	}
}

void CStationView::ReLayoutBmpPageSize(int width)
{
	if(m_pOldBitmap != NULL)
	{
		m_dcMemoryDC.SelectObject(m_pOldBitmap);
		m_bmpMemBitmap.DeleteObject();
		m_dcMemoryDC.DeleteDC();
		m_pOldBitmap = NULL;
	}
	
	CClientDC dc(this);
	dc.SetMapMode(MM_TEXT);

	BITMAPINFOHEADER bmih;
	memset(&bmih,0,sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	DEVMODE md;
	md.dmSize = sizeof(md);
	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &md))
		bmih.biBitCount = md.dmBitsPerPel;
	else
		bmih.biBitCount =16;
	bmih.biCompression=BI_RGB;
	bmih.biPlanes=1;
	bmih.biWidth=width;
	bmih.biHeight=m_pDoc->m_nPageHeight;
	BITMAPINFO bmi;
	memset(&bmi,0,sizeof(BITMAPINFO));
	bmi.bmiHeader = bmih;
	void *p;
	HBITMAP bitMap = ::CreateDIBSection(dc.m_hDC, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
	if(!bitMap)
	{
		m_pDoc->sysprint(0,0,"[TG]: create memory bitmap error");				
		AfxMessageBox("创建车站视图位图失败，请重启计算机后登陆！");
		return;
	}
	m_bmpMemBitmap.Attach(bitMap);
	if(m_dcMemoryDC.CreateCompatibleDC(&dc) == 0)
	{
		m_bmpMemBitmap.DeleteObject();
		AfxMessageBox("创建车站兼容内存dc失败，请重启计算机后登陆！");
		m_pDoc->sysprint(0,0,"[TG]: create  memory DC error");
		return;
	}

	m_pOldBitmap = m_dcMemoryDC.SelectObject(&m_bmpMemBitmap);

	m_dcMemoryDC.SetBkMode(TRANSPARENT);

	CRect rect(0, 0, width,	m_pDoc->m_nPageHeight);
	m_dcMemoryDC.FillRect(&rect, &m_pDoc->m_brushWhite);
}