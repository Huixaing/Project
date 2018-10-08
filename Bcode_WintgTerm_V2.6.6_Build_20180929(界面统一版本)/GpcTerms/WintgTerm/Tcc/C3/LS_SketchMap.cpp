// LS_SketchMap : implementation file
//

#include "stdafx.h"
//#include "../affair/resource.h"
#include "resource.h"
#include "messages.h"
#include "LS_SketchMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLS_SketchMap
extern void Draw3DRect(CDC * pCDC, CRect &rect, COLORREF clrTopLeft, COLORREF clrBottomRight,int nWidth);
extern void Draw3DRect(CDC * pCDC, CRect &rect, int nWidth, int nFlag);
void DrawAngleText(CDC* pDC,int x,int y,int angle,int size,const char *str); // 倾斜文字显示

namespace TCC_C3 {



CLS_SketchMap::CLS_SketchMap(ITccClientInfoProvider *pInfoProvider)
	: m_pInfoProvider(pInfoProvider)
	, m_pLine(NULL)
{
	m_nKmMark[0] = m_nKmMark[1] = 0;

	m_clrBkgnd = RGB(127, 127, 127);

	m_totalLog.cy = 0;
	m_totalLog.cx = 0;
	m_totalDev = m_totalLog;
	btnStatus=false;
	btnStatus=false;
	m_BtnEnV=false;
	BtnSLpos=false;
	BtnSRpos=false;
	BtnSpos=false;
	dir_limit=DIR_UNKNOWN;
}

CLS_SketchMap::~CLS_SketchMap()
{
//	if( IsWindow(GetSafeHwnd()) )
		DestroyWindow();
}


BEGIN_MESSAGE_MAP(CLS_SketchMap, CWnd)
	//{{AFX_MSG_MAP(CLS_SketchMap)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLS_SketchMap message handlers

void CLS_SketchMap::OnPaint() 
{
	double	PI = 3.1415926535897932384626433832795;
	XFORM	xForm;
	xForm.eM11 = 1.0 * m_totalDev.cx / m_totalLog.cx;//cos(-PI);
	xForm.eM12 = -1.0e-37;//sin(-PI);
	xForm.eM21 = 1.0 * m_totalDev.cy / m_totalLog.cy;//-sin(-PI);
	xForm.eM22 = 1.0e-37;//cos(-PI);
	xForm.eDx = 0;
	xForm.eDy = 0;

	xForm.eM11 =-1.0;//cos(-PI);
	xForm.eM12 = -1.0e-37;//sin(-PI);
	xForm.eM21 = 1.0e-37;//-sin(-PI);
	xForm.eM22 = -1.0;//cos(-PI);
	xForm.eDx = 20;
	xForm.eDy = 0;

	CPaintDC dc(this); // device context for painting
	OnPrepareDC(&dc);

	//SetGraphicsMode(dc.GetSafeHdc(), GM_ADVANCED);
	//SetWorldTransform(dc.GetSafeHdc(), &xForm);
	OnDraw(&dc);
}

void CLS_SketchMap::OnPrepareDC(CDC* pDC)
{
	ASSERT_VALID(pDC);
	ASSERT(m_totalLog.cx >= 0 && m_totalLog.cy >= 0);

	// Set the Mapping mode, and the window and viewport extents
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(m_totalLog);   // in logical coordinates
	pDC->SetViewportExt(m_totalDev); // in device  coordinates

	// by default shift viewport origin in negative direction of scroll
	ASSERT(pDC->GetWindowOrg() == CPoint(0,0));

	CRect rect;
	GetClientRect(&rect);
	CPoint ptVpOrg(0, 0);

	ptVpOrg.x = (rect.Width() - m_totalDev.cx) / 2;
	ptVpOrg.y = (rect.Height() - m_totalDev.cy) / 2;

	// Set the new origin
	pDC->SetViewportOrg(ptVpOrg);
}

void CLS_SketchMap::OnDraw(CDC *pDC)
{
	CRect rect;
	GetClientRect(&rect);
	if(NULL == m_pLine)
	{
		pDC->DrawText("侧线限速没有示意图", rect, DT_CENTER|DT_VCENTER);
		return;
	}
/*	if (m_nKmMark[0].Equal(m_nKmMark[1]))
	{
		pDC->DrawText("起始公里标与结束公里标相等，没有示意图", rect, DT_CENTER|DT_VCENTER);
		return;
	}*/
	int count_tcclist=0;
	tcclist.clear();
	if (!m_nKmMark[0].Equal(m_nKmMark[1]))
	{
		if(m_pInfoProvider->GetHitTccList(m_pLine->builtin_id,dir_limit,m_nKmMark[0],m_nKmMark[1],tcclist))
		{
			count_tcclist=tcclist.size();
			if( 0 < tcclist.size())
			{
				kmmark_t nBegin, nEnd;
				m_pInfoProvider->GetTccLineKmMarkerScope(tcclist[0].nID,tcclist[0].nTccLineID,dir_limit,&m_Tcclist_Begin,&m_Tcclist_End);
				for(int i=1; i<tcclist.size(); i++)
				{
					m_pInfoProvider->GetTccLineKmMarkerScope(tcclist[i].nID,tcclist[0].nTccLineID,dir_limit,&nBegin, &nEnd);
					if(nBegin.Less(m_Tcclist_Begin, m_pLine->builtin_id))
						m_Tcclist_Begin = nBegin;
					if(nEnd.Greater(m_Tcclist_End, m_pLine->builtin_id))
						m_Tcclist_End = nEnd;
				}
			}
		}
	}
	int  plat_height=32;
	//f = 1.0;
	int ptY = (rect.top + rect.bottom-5) /2;
	int ptX = 0,	ptX2 = 0;

	LOGBRUSH logBrush;
	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = RGB(127, 127, 127);

	CPen mPen, *pOldPen = NULL;
	mPen.CreatePen(PS_GEOMETRIC|PS_INSIDEFRAME, 5, &logBrush);
	
	//可能涉及到的站 的全部长度
	pOldPen = pDC->SelectObject(&mPen);
	pDC->MoveTo(0, ptY);
	pDC->LineTo(rect.right, ptY);
	pDC->SelectObject(pOldPen);
	int m=0;
	long nLength = 0;
	m_nKmMark0=m_nKmMark[0];
	m_nKmMark1=m_nKmMark[1];
	if(m_nKmMark[0].GreaterOrEqual(m_nKmMark[1],m_pLine->builtin_id))
	{
		m_nKmMark0=m_nKmMark[1];
		m_nKmMark1=m_nKmMark[0];
	}
	for(int i=0; i<m_pLine->gd_stas.size(); i++)
	{
		long nStaLength = 0;
		bool bIn = false;
		for(int j=0; j<m_pLine->gd_stas[i].secs.size(); j++)
		{
			const LINE_STRUCT::_STA_GD::_SEC_GD& sec = m_pLine->gd_stas[i].secs[j];
			if(m_nKmMark0.LessOrEqual(sec.nKmBegin, m_pLine->builtin_id)
				&& m_nKmMark1.GreaterOrEqual(sec.nKmEnd, m_pLine->builtin_id))
			{
				bIn = true;
			}
			nStaLength += (computef(i,j)+30);
		}
		if(1)
		{
			// 画站界和站名
			mPen.DeleteObject();
			logBrush.lbColor = RGB(0, 0, 255);
			mPen.CreatePen(PS_GEOMETRIC|PS_INSIDEFRAME, 3, &logBrush);

			pDC->SelectObject(&mPen);
			pDC->SetTextColor(RGB(0, 0, 255));

			ptX = nLength+xStart;
			ptX2 = (nLength + nStaLength)+xStart;
			
					//ptX = nLength*f+xStart;
					//ptX2 = (nLength + sec.nLength) * f+xStart;

			pDC->MoveTo(ptX2, ptY -5);
			pDC->LineTo(ptX2, ptY -30);
			pDC->SelectObject(pOldPen);

			pDC->SetTextColor(RGB(0,0,255));
			DrawAngleText(pDC,(ptX+ptX2)/2,ptY-80,0,36,m_pLine->gd_stas[i].szName);

			for(int j=0; j<m_pLine->gd_stas[i].secs.size(); j++)
			{
				
				const LINE_STRUCT::_STA_GD::_SEC_GD& sec = m_pLine->gd_stas[i].secs[j];
				if(m_nKmMark0.LessOrEqual(sec.nKmBegin, m_pLine->builtin_id)
					&& m_nKmMark1.GreaterOrEqual(sec.nKmEnd, m_pLine->builtin_id))
				{
					//画限速区段
					mPen.DeleteObject();
					logBrush.lbColor = RGB(255, 255, 0);
					mPen.CreatePen(PS_GEOMETRIC|PS_INSIDEFRAME, 3, &logBrush);

					pDC->SelectObject(&mPen);

					//ptX = nLength*f+xStart;
					//ptX2 = (nLength + sec.nLength) * f+xStart;
					ptX = nLength+xStart;
					ptX2 = (nLength + computef(i,j)+30)+xStart;
					nLength += computef(i,j)+30;

					pDC->MoveTo(ptX, ptY);
					pDC->LineTo(ptX2, ptY);
					pDC->MoveTo(ptX2, ptY -3);
					pDC->LineTo(ptX2, ptY +3);
					if(StaDis!=0)
					{
						if(m==0&&BtnS)
						{
							BtnLpos=ptX;
						}

						if(BtnS)
							m++;
						if(m==StaDis&&BtnS)
						{
							BtnRpos=ptX2;
							m=-1;
							BtnS=false;
						}
					}
					else
						BtnS=false;
					if(strstr(sec.szName,"XHOME"))
					{
						pDC->MoveTo((ptX+ptX2)/2, ptY);
						pDC->LineTo(ptX2,ptY-plat_height);
					}
					else if(strstr(sec.szName,"SIDE"))
   					{
						pDC->MoveTo(ptX, ptY-plat_height);
						pDC->LineTo(ptX2,ptY-plat_height);
					}
					else if(strstr(sec.szName,"XDEPA"))
   					{
						pDC->MoveTo(ptX, ptY-plat_height);
						pDC->LineTo((ptX+ptX2)/2,ptY);
					}
					else
					{
						pDC->SetTextColor(RGB(0,0, 0));
						DrawAngleText(pDC,(ptX+ptX2)/2,ptY,3150,15,sec.szName);
					}
					pDC->SelectObject(pOldPen);

					char  kmstr[32];
					sprintf(kmstr,"%s",sec.nKmBegin.toString(m_pInfoProvider, m_pLine->builtin_id));
					pDC->SetTextColor(RGB(0,0,255));
					DrawAngleText(pDC,ptX,ptY,3150,18,kmstr);
				}
				else
				{
					// 画不在限速范围内的区段
					if(m==1&&BtnS)
					{
						BtnS=false;
						BtnRpos=ptX2;
					}
					ptX = nLength+xStart;
					ptX2 = (nLength + computef(i,j)+30)+xStart;
					nLength += computef(i,j)+30;
					mPen.DeleteObject();
					logBrush.lbColor = RGB(127, 127, 127);
					if (m_Tcclist_Begin.LessOrEqual(sec.nKmBegin, m_pLine->builtin_id)
						&& m_Tcclist_End.GreaterOrEqual(sec.nKmEnd, m_pLine->builtin_id))
					{
						logBrush.lbColor = RGB(255, 0, 0);
					}
					mPen.CreatePen(PS_GEOMETRIC|PS_INSIDEFRAME, 3, &logBrush);

					pDC->SelectObject(&mPen);
					//pDC->SetBkMode(TRANSPARENT);

					//ptX = nLength * f+xStart;
					//ptX2 = (nLength + sec.nLength) * f+xStart;
					//nLength += sec.nLength;


					pDC->MoveTo(ptX, ptY);
					pDC->LineTo(ptX2, ptY);
					pDC->MoveTo(ptX2, ptY -3);
					pDC->LineTo(ptX2, ptY +3);
					

					if(strstr(sec.szName,"XHOME"))
					{
						pDC->MoveTo((ptX+ptX2)/2, ptY);
						pDC->LineTo(ptX2,ptY-plat_height);
					}
					else if(strstr(sec.szName,"SIDE"))
   					{
						pDC->MoveTo(ptX, ptY-plat_height);
						pDC->LineTo(ptX2,ptY-plat_height);
					}
					else if(strstr(sec.szName,"XDEPA"))
   					{
						pDC->MoveTo(ptX, ptY-plat_height);
						pDC->LineTo((ptX+ptX2)/2,ptY);
					}
					else
					{
						pDC->SetTextColor(RGB(127,127,127));
						DrawAngleText(pDC,(ptX+ptX2)/2,ptY,3150,15,sec.szName);
					}
					pDC->SelectObject(pOldPen);

					char  kmstr[32];
					sprintf(kmstr,"%s",sec.nKmBegin.toString(m_pInfoProvider, m_pLine->builtin_id));
					pDC->SetTextColor(RGB(0,0,255));
					DrawAngleText(pDC,ptX,ptY,3150,18,kmstr);
				}
			}
		}
	}
	CString str;
	str.Format("%s 至 %s", m_nKmMark[0].toString(m_pInfoProvider, m_pLine->builtin_id), m_nKmMark[1].toString(m_pInfoProvider, m_pLine->builtin_id));
	if(m_nKmMark[0].Equal(m_nKmMark[1]))
		str= "限速点不合法！请检查重置.";
	(this->GetOwner())->GetOwner()->GetDlgItem(IDC_TEXT)->SetWindowText(str);
	/*if(BtnS)
	{
		BtnS=false;
		BtnL.MoveWindow(BtnLpos-4,BtnTop-10,10,20);
		BtnR.MoveWindow(BtnRpos-4,BtnTop-10,10,20);
	}*/

}

void CLS_SketchMap::SetKmMark(int builtin_id, kmmark_t nKmBegin, kmmark_t nKmEnd)
{
	m_pLine = GetLineStruct(builtin_id);
	if(NULL == m_pLine)
		return;

	//if (nKmBegin.Equal(nKmEnd))
	//	return;
	
	//m_totalDev.cx = (m_nKmMark[1] - m_nKmMark[0])/5;

	m_nKmMark[0] = nKmBegin;
	m_nKmMark[1] = nKmEnd;

	if(GetSafeHwnd())
		Invalidate();
}

BOOL CLS_SketchMap::Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL BtnEn) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CString szClassName = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
		AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1),
		NULL);

	dwStyle = WS_VISIBLE | WS_CHILD;

	if(! CWnd::CreateEx(0,
		(LPCTSTR)szClassName, lpszWindowName,dwStyle,
		rect,pParentWnd,nID))
		return FALSE;
	if(NULL == m_pLine)
	{
		//pDC->DrawText("侧线限速没有示意图", rect, DT_CENTER|DT_VCENTER);
		return false;
	}
	//if (m_nKmMark[0].Equal(m_nKmMark[1]))
	//{
		//pDC->DrawText("起始公里标与结束公里标相等，没有示意图", rect, DT_CENTER|DT_VCENTER);
	//	return false;
	//}
	Init();
	m_BtnEnV=BtnEn;
	//CRect rect;
	//GetClientRect(&rect);
	//if((BtnLpos+20)>(AllLength-rect.Width()))
	//xStart=-(AllLength-rect.Width()+20);
	m_totalDev.cx = abs(rect.right - rect.left);
	m_totalDev.cy = abs(rect.bottom - rect.top);
	m_totalLog = m_totalDev;
	RedrawRT=rect;
	RedrawRT.bottom=rect.bottom-17;
	RT.left=rect.left;
	RT.bottom=rect.bottom;
	RT.right=rect.right;
	RT.top=rect.bottom-17;
    if(!scrollbar.Create(SBS_HORZ | SBS_TOPALIGN | WS_CHILD|WS_VISIBLE,RT,
		this, 100))//CRect(0,rect.bottom-20, rect.right, 15),
	{	
		return false;
    }
	xStart=-(int)BtnLpos+50;
	if(((int)BtnLpos-50)>=(AllLength-RT.Width()+2))
		xStart=-int(AllLength-RT.Width()+2);
	else 
		if(((int)BtnLpos-50)<0)
			xStart=0;
	scrollbar.SetScrollRange(0,100);
	scrollbar.EnableScrollBar();
	int k=0; 
	k=-(int)xStart*100/(AllLength-RT.Width()+2);
	if(k<100)
	{
		for(int m=0;m<50;m++)
		{
			if(k>=(2*m)&&k<=(2*m+2))
			{
				k=2*m;
				break;
			}
		}
		xStart=-double(AllLength-RT.Width()+2)*(double)k/100;
	}	
	//xStart=0;
	//k=0;
	DWORD BtnStyle=WS_DISABLED|WS_VISIBLE|WS_CHILD;
	if(!m_BtnEnV)
		BtnStyle&=!WS_VISIBLE;
	BtnLpos+=xStart;
	BtnRpos+=xStart;
	if (m_nKmMark[0].Equal(m_nKmMark[1]))
	{
		//pDC->DrawText("起始公里标与结束公里标相等，没有示意图", rect, DT_CENTER|DT_VCENTER);
		//return false;
		k=0;
		BtnRpos=BtnRpos-xStart+20;
		xStart=0;
		BtnLpos=-20;
		
		//BtnStyle&=!WS_VISIBLE;
	}
	scrollbar.SetScrollPos(k);
	
	BtnL.Create("",BtnStyle,CRect(BtnLpos-4,BtnTop-10,BtnLpos+6,BtnTop+10),this,102);
	BtnR.Create("",BtnStyle,CRect(BtnRpos-4,BtnTop-10,BtnRpos+6,BtnTop+10),this,103);

	return TRUE;
}

BOOL CLS_SketchMap::OnEraseBkgnd(CDC* pCDC) 
{
	// TODO: Add your message handler code here and/or call default
/*	CRect ClipRect;
	pCDC->GetClipBox(&ClipRect);

	pCDC->FillRect(&ClipRect,&m_brushNormal);
	
*/	
//	return TRUE;
	CWnd::OnEraseBkgnd(pCDC);
	CRect rectClient; 
	CBrush brushBkColor; 
	GetClientRect(rectClient); 

	brushBkColor.CreateSolidBrush(RGB(239,235,222)); 
	pCDC->DPtoLP(rectClient); 
	pCDC->FillRect(rectClient,&brushBkColor);
	return true;
}


void CLS_SketchMap::OnNcPaint() 
{
	Invalidate();
	
	// Do not call CWnd::OnNcPaint() for painting messages
}


void CLS_SketchMap::Init()
{

	CRect rect;
	GetClientRect(&rect);
	BtnTop= (rect.top + rect.bottom-5) /2;
	AllLength=0;  //dd
	btnStatus=false;
	btnStatusR=false;
	BtnS=false;
	int  plat_height=32;
	Tic.clear();
	AllLength=0;
	for(int i=0; i<m_pLine->gd_stas.size(); i++)
	{
		for(int j=0; j<m_pLine->gd_stas[i].secs.size(); j++)
		{
			const LINE_STRUCT::_STA_GD::_SEC_GD sec = m_pLine->gd_stas[i].secs[j];
			Tic.push_back(AllLength);
			AllLength+=computef(i,j)+30;   //dd
		}
	}
	BtnLpos=0;
	StaDis=0;
	//BtnlS=0;
	for(i=0; i<m_pLine->gd_stas.size(); i++)//左滑块位置
	{
		int nStaLength = 0;
		bool bIn = false;
		for(int j=0; j<m_pLine->gd_stas[i].secs.size(); j++)
		{
			const LINE_STRUCT::_STA_GD::_SEC_GD sec = m_pLine->gd_stas[i].secs[j];
			if(m_nKmMark[0].LessOrEqual(sec.nKmBegin, m_pLine->builtin_id)
				&& m_nKmMark[1].GreaterOrEqual(sec.nKmEnd, m_pLine->builtin_id))
			{
				bIn = true;
				break;
			}
			BtnLpos+=computef(i,j)+30;
		}
		if(bIn)
			break;
	}
	BtnRpos=0;
	for(i=0; i<m_pLine->gd_stas.size(); i++)//右滑块位置
	{
		int nStaLength = 0;
		bool bIn = false;
		for(int j=0; j<m_pLine->gd_stas[i].secs.size(); j++)
		{
			const LINE_STRUCT::_STA_GD::_SEC_GD sec = m_pLine->gd_stas[i].secs[j];
			if(m_nKmMark[0].LessOrEqual(sec.nKmBegin, m_pLine->builtin_id)
				&& m_nKmMark[1].GreaterOrEqual(sec.nKmEnd, m_pLine->builtin_id))
			{
				bIn = true;
				//break;
			}
			if(bIn)
			{
				bIn=false;
				BtnRpos+=computef(i,j)+30;
				StaDis++;
			}
		}
	}
	BtnRpos=BtnLpos+BtnRpos;
}

double CLS_SketchMap::computef(int stas,int secs)
{
	long leastLength = m_pLine->gd_stas[0].secs[0].nLength;
	
	for(int i=0; i<m_pLine->gd_stas.size(); i++)
	{

		for(int j=0; j<m_pLine->gd_stas[i].secs.size(); j++)
		{
			const LINE_STRUCT::_STA_GD::_SEC_GD sec = m_pLine->gd_stas[i].secs[j];

			if(sec.nLength<leastLength)
				leastLength=sec.nLength;
	
		}
	
	}
	double disf=0;
	disf=double(m_pLine->gd_stas[stas].secs[secs].nLength-leastLength)/(double)m_pLine->gd_stas[stas].secs[secs].nLength;
	return disf*30;
}

void CLS_SketchMap:: OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect123;
	GetClientRect(&rect123);
//	static int k=2;
	if(m_BtnEnV)
	{
		if(BtnSLpos)
		{

			if((point.y>((rect123.top + rect123.bottom) /2-6))&&(point.y<((rect123.top + rect123.bottom) /2+2)))
			{

				BtnSLpos=false;
				if((point.x>(BtnRpos-3))&&(point.x<(BtnRpos+3)))
					return;
				if((point.x>(BtnLpos-3))&&(point.x<(BtnLpos+3)))
					return;
				if(point.x>BtnRpos)
				{				
					//MessageBox("起始点不能在终止点的右侧","提示",MB_OK|MB_ICONEXCLAMATION|MB_ICONWARNING);
					//return;
					BtnLpos= BtnRpos;
					BtnRpos= point.x;
				}
				else
				{
					BtnLpos=point.x;
									
				}
				btnStatusR=true;
				btnStatus=true;
				OnLButtonUp(nFlags,point);
				return;
			}
		}
		if(BtnSRpos)
		{
			if((point.y>((rect123.top + rect123.bottom) /2-6))&&(point.y<((rect123.top + rect123.bottom) /2+2)))
			{
				BtnSRpos=false;
				/*if(point.x<=BtnLpos)
				{				
					//MessageBox("起始点不能在终止点的左侧","提示",MB_OK|MB_ICONEXCLAMATION|MB_ICONWARNING);
					return;
				}*/
				if((point.x>(BtnLpos-3))&&(point.x<(BtnLpos+3)))
					return;
				if((point.x>(BtnRpos-3))&&(point.x<(BtnRpos+3)))
					return;
				if(point.x<BtnLpos)
				{				
					//MessageBox("起始点不能在终止点的右侧","提示",MB_OK|MB_ICONEXCLAMATION|MB_ICONWARNING);
					//return;
					BtnRpos= BtnLpos;
					BtnLpos= point.x;
					//btnStatus=true;
				}
				else
					BtnRpos=point.x;
				
				btnStatusR=true;
				btnStatus=true;
				OnLButtonUp(nFlags,point);
				return;
			}
		}
		BtnS=false;
		if(btnStatus)
			btnStatus=false;
		if(btnStatusR)
			btnStatusR=false;
		CRect rt1;
		BtnL.GetWindowRect(&rt1);
		ClientToScreen(&point);
		if(point.x>(rt1.left-2)&&point.x<(rt1.right+2)&&point.y>(rt1.top-2)&&point.y<(rt1.bottom+2))
			btnStatus=1;
		BtnR.GetWindowRect(&rt1);
		if(point.x>(rt1.left-2)&&point.x<(rt1.right+2)&&point.y>(rt1.top-2)&&point.y<(rt1.bottom+2))
			btnStatusR=1;
		
	}
}
void CLS_SketchMap::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_BtnEnV)
	{
		BtnS=false;
		if(btnStatus)
		{
			btnStatus=false;
			int i;
			for(i=0; i<Tic.size();)
			{
				if((BtnLpos+2)>(Tic[i]+xStart))
				{
					long a=Tic[i];
					i++;
					//continue;
				}

				if((BtnLpos+2)<(Tic[i]+xStart))
				{
					long a=Tic[i];
					if(i==0)
						BtnLpos=Tic[i];
					else
					{
						BtnLpos=Tic[i-1];
						i--;
					}
					break;
				}
				if((BtnLpos+2)==(Tic[i]+xStart))
				{
					BtnLpos=Tic[i];
					break;
				}
			}
			int L=0;
			BOOL nSt=false;
			for(int k=0; k<m_pLine->gd_stas.size(); k++)//改变m_nKmMark[0]
			{

				for(int j=0; j<m_pLine->gd_stas[k].secs.size(); j++)
				{
					if(L<i)
						L++;
					else
					{
	//					m_nKmMark[0]=m_pLine->gd_stas[k].secs[j].nKmBegin;
						SetKmMark(m_pLine->builtin_id,m_pLine->gd_stas[k].secs[j].nKmBegin,m_nKmMark[1]);
						nSt=true;
					}
					if(nSt)
						break;
				}
				if(nSt)
					break;
			}
			if(!btnStatusR)
			{
				//int dis=StaDis;
				StaDis=0;
				for(i=0; i<m_pLine->gd_stas.size(); i++)//计算间隔
				{
					bool bIn = false;
					for(int j=0; j<m_pLine->gd_stas[i].secs.size(); j++)
					{
						const LINE_STRUCT::_STA_GD::_SEC_GD sec = m_pLine->gd_stas[i].secs[j];
						if(m_nKmMark[0].LessOrEqual(sec.nKmBegin, m_pLine->builtin_id)
							&& m_nKmMark[1].GreaterOrEqual(sec.nKmEnd, m_pLine->builtin_id))
						{
							bIn = true;
						}
						if(bIn)
						{
							bIn=false;
							StaDis++;
						}
					}
				}
				//if(dis==StaDis)
				//	return;
				BtnS=true;
				InvalidateRect(&RedrawRT);	
				UpdateWindow();
				BtnL.MoveWindow(BtnLpos-4,BtnTop-10,10,20);
			}
		}
		if(btnStatusR)
		{
			btnStatusR=false;
			int i;
			/*if(BtnSpos)
			{
				BtnRpos=BtnLpos;
				m_nKmMark[1]=m_nKmMark[0];
				BtnSpos=false;
				BtnS=true;
				InvalidateRect(&RedrawRT);
				UpdateWindow();
				BtnR.MoveWindow(BtnRpos-4,BtnTop-10,10,20);
				return;
			}*/
			for(i=0; i<Tic.size();)
			{
				if((BtnRpos-2)>(Tic[i]+xStart))
				{
					i++;
					//continue;
				}

				if((BtnRpos-2)<(Tic[i]+xStart))
				{
					BtnRpos=Tic[i];
					break;
				}
				if((BtnRpos-2)==(Tic[i]+xStart))
				{
					BtnRpos=Tic[i];
					break;
				}
			}
			int L=0;
			BOOL nSt2=false;
			for(int k=0; k<m_pLine->gd_stas.size(); k++)//改变m_nKmMark[1]
			{

				for(int j=0; j<m_pLine->gd_stas[k].secs.size(); j++)
				{
					if(L<i)
						L++;
					if(L==i)
					{
	//					m_nKmMark[0]=m_pLine->gd_stas[k].secs[j].nKmBegin;
						SetKmMark(m_pLine->builtin_id,m_nKmMark[0],m_pLine->gd_stas[k].secs[j].nKmEnd);
						nSt2=true;
					}
					if(nSt2)
						break;
				}
				if(nSt2)
					break;
			}
			//int dis=StaDis;
			StaDis=0;
			for(i=0; i<m_pLine->gd_stas.size(); i++)//计算间隔
			{
				bool bIn = false;
				for(int j=0; j<m_pLine->gd_stas[i].secs.size(); j++)
				{
					const LINE_STRUCT::_STA_GD::_SEC_GD sec = m_pLine->gd_stas[i].secs[j];
					if(m_nKmMark[0].LessOrEqual(sec.nKmBegin, m_pLine->builtin_id)
						&& m_nKmMark[1].GreaterOrEqual(sec.nKmEnd, m_pLine->builtin_id))
					{
						bIn = true;
					}
					if(bIn)
					{
						bIn=false;
						StaDis++;
					}
				}
			}
			//if(dis==StaDis)
			//	return;
			BtnS=true;
			InvalidateRect(&RedrawRT);
			UpdateWindow();
			BtnR.MoveWindow(BtnRpos-4,BtnTop-10,10,20);
			BtnL.MoveWindow(BtnLpos-4,BtnTop-10,10,20);
		}


		//BtnR.MoveWindow(BtnRpos-4,BtnTop-10,10,20);
	}
}
void CLS_SketchMap::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_BtnEnV)
	{
		BtnS=false;
		CRect rt2,rt3;
		BtnL.GetWindowRect(&rt2);
		BtnR.GetWindowRect(&rt3);
		//ClientToScreen(&point);
		ScreenToClient(&rt2);
		ScreenToClient(&rt3);
		if(btnStatus)
		{
			if(point.x>rt3.left-10)
				BtnLpos=rt3.left-10;
			else
			{
				BtnLpos=point.x;
			}
			BtnL.MoveWindow(BtnLpos-4,BtnTop-10,10,20);
		}
		if(btnStatusR)
		{
			if(point.x<rt2.right+10)
				BtnRpos=rt2.right+10;
			else
			{
				BtnRpos=point.x;
			}
			BtnR.MoveWindow(BtnRpos-4,BtnTop-10,10,20);
		}
	}
	CWnd::OnMouseMove(nFlags,point);

}
void CLS_SketchMap::OnLButtonDblClk(UINT nFlags, CPoint point)
{
/*	if(btnStatus)
		btnStatus=false;
	if(btnStatusR)
		btnStatusR=false;
	CRect rt1;
	BtnL.GetWindowRect(&rt1);
	ClientToScreen(&point);
	if(point.x>(rt1.left-2)&&point.x<(rt1.right+2)&&point.y>(rt1.top-2)&&point.y<(rt1.bottom+2))
		btnStatus=1;
	BtnR.GetWindowRect(&rt1);
	if(point.x>(rt1.left-2)&&point.x<(rt1.right+2)&&point.y>(rt1.top-2)&&point.y<(rt1.bottom+2))
		btnStatusR=1;*/
	BtnS=false;
	CWnd::OnLButtonDblClk(nFlags,point);

}

void CLS_SketchMap::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int curpos =pScrollBar->GetScrollPos();
	int max=pScrollBar->GetScrollLimit();
	int oldpos=curpos;
	int CtrlID=pScrollBar->GetDlgCtrlID();
	if(CtrlID==100)
	// Determine the new position of scroll box.
	{
		switch (nSBCode)
		{
		case SB_LEFT:    // Scroll to far left.
			curpos -=2;
			break;

		case SB_RIGHT:    // Scroll to far right.
			curpos +=2; //maxpos;
			break;

		case SB_ENDSCROLL:  // End scroll.
			break;

		case SB_LINELEFT:    // Scroll left.
		//    if (curpos > minpos)
			curpos-=2;
			break;

		case SB_LINERIGHT:  // Scroll right.
		//    if (curpos < maxpos)
			curpos+=2;
			break;

		case SB_PAGELEFT:   // Scroll one page left.
		{
			// Get the page size.
			curpos-=2;
	//		xStart=-(AllLength-RT.Width())/max*curpos;
		}
			break;

		case SB_PAGERIGHT:    // Scroll one page right.
		{
			// Get the page size.
			curpos+=2;
	
		}
			break;
		case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
			curpos =nPos;      // of the scroll box at the end of the drag// operation.
			break;
		case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is                        // the
			curpos =nPos;     // position that the scroll box has been dragged                      // to.
			break;
		}

		if (curpos<0) 
		{
			curpos=0;
//			return;
		}
		if (curpos>100) 
		{
			curpos=100;
//			return;
		}
		pScrollBar->SetScrollPos(curpos);
		CRect rm;
		GetClientRect(&rm);
		long l=xStart;
		xStart=-double(AllLength-rm.Width()+2)*(double)curpos/100;
		pScrollBar->MoveWindow(&RT);
		
		if(curpos!=oldpos)
		{
			BtnS=true;	
			InvalidateRect(&RedrawRT);
			UpdateWindow();
			if(StaDis==0)
			{
				BtnLpos+=xStart-l;
				BtnRpos+=xStart-l;
				BtnL.MoveWindow(BtnLpos-4,BtnTop-10,10,20);
				BtnR.MoveWindow(BtnRpos-4,BtnTop-10,10,20);
			}
			else
			{
				BtnL.MoveWindow(BtnLpos-4,BtnTop-10,10,20);
				BtnR.MoveWindow(BtnRpos-4,BtnTop-10,10,20);
			}
		}


	}
}



}//end of namespace TCC_C3



void DrawAngleText(CDC* pDC,int x,int y,int angle,int size,const char *str) // 倾斜文字显示
{
CFont font;
LOGFONT stFont;//定义字体结构；
memset(&stFont,0,sizeof(stFont));//设置字体结构的属性；
stFont.lfHeight=size;
stFont.lfWeight=FW_NORMAL;
stFont.lfClipPrecision=CLIP_LH_ANGLES;
strcpy(stFont.lfFaceName,"Arial Black");
//下面的代码以视图中心为圆点，在半径100的圆周上每隔30度显示字符串；
stFont.lfEscapement=angle;//设置字体的倾斜角度；
font.CreateFontIndirect(&stFont);//根据字体结构创建字体对象；
CFont *oldFont;
oldFont=pDC->SelectObject(&font);//将创建的字体选入设备上下文；
pDC->TextOut(x,y,str);
 /*
if(angle<900)
　　　pDC->TextOut(rect.left+rect.Width )/2+100*cos(i/1800*3.14),rect.top+rect.Height ()/2-100*sin(i/1800*3.14),str);
　　if(i>=900&&i<1800)
　　　pDC->TextOut(rect.left+rect.Width ()/2+100*cos(i/1800*3.14),
rect.top+rect.Height ()/2-100*sin(i/1800*3.14),str);
　　if(i>=1800&&i<2700)
　　　pDC->TextOut(rect.left+rect.Width ()/2+100*cos(i/1800*3.14),
rect.top+rect.Height ()/2-100*sin(i/1800*3.14),str);
　　if(i>=2700&&i<=3600)
　　　pDC->TextOut(rect.left+rect.Width ()/2+100*cos(i/1800*3.14),
 rect.top+rect.Height ()/2-100*sin(i/1800*3.14),str);
*/
pDC->SelectObject(oldFont);//恢复设备上下文的字体；
font.DeleteObject ();//删除创建的字体；
}
 
//////////////////////////////////////////
 
