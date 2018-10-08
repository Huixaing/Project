// BaseTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BaseTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBaseTabCtrl

CBaseTabCtrl::CBaseTabCtrl()
{
	m_nCurPage = 0;
	m_nPageCount = 0;
	m_pImagelist = NULL;
	m_bkColor = RGB(231, 236, 247);
	m_textColor = RGB(0, 0, 0);
	m_curItemColor = RGB(211, 208, 200);
	m_itemColor = RGB(211, 208, 200);
	m_nXOffset = 3;
	m_nYOffset = 3;
	m_textFont.CreateFont(   
        12,					//   nHeight   
        0,					//   nWidth   
        0,					//   nEscapement   
        0,					//   nOrientation   
        FW_NORMAL,			//   nWeight   
        FALSE,				//   bItalic   
        FALSE,				//   bUnderline   
        0,					//   cStrikeOut   
        ANSI_CHARSET,		//   nCharSet   
        OUT_DEFAULT_PRECIS,	//   nOutPrecision   
        CLIP_DEFAULT_PRECIS,//   nClipPrecision   
        DEFAULT_QUALITY,	//   nQuality   
        DEFAULT_PITCH   |   FF_SWISS,	//   nPitchAndFamily   
        "宋体");
}

CBaseTabCtrl::~CBaseTabCtrl()
{
	m_textFont.DeleteObject();
}


BEGIN_MESSAGE_MAP(CBaseTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CBaseTabCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseTabCtrl message handlers

BOOL CBaseTabCtrl::AddPage(LPCTSTR lpTitle, CDialog *pDialog, UINT ID, INT nIcon, UINT nBkBitmapID)
{
	//达到最大包含页数
	if ( MAX_PAGE_COUNT == m_nPageCount ) return FALSE;
	
	ASSERT(pDialog);
	
	InsertItem( m_nPageCount, lpTitle, nIcon );
	m_pPages[m_nPageCount] = pDialog;
	m_PageIDDs[m_nPageCount] = ID;
	m_BitmapIDDs[m_nPageCount] = nBkBitmapID;
	m_pPages[m_nPageCount]->DestroyWindow();
	m_pPages[m_nPageCount]->Create( m_PageIDDs[m_nPageCount], this );
	m_pPages[m_nPageCount]->ShowWindow( SW_HIDE );
	SetRect( m_nPageCount );
	
	m_nPageCount++;
	return TRUE;	
}


void CBaseTabCtrl::SetRect(int nIndex)
{
	CRect tabRect, itemRect,winRect;
	int nX, nY, nXc, nYc;
	
	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);
	
	DWORD dwStyle = GetStyle();
	if ( dwStyle & TCS_BOTTOM && dwStyle & TCS_VERTICAL )//在右边框显示页标签
	{
		nX=itemRect.left;
		nXc=tabRect.right-itemRect.left-2;
		nY=itemRect.bottom+1;
		nYc=tabRect.bottom-nY-2;
		nX = nXc = nY = nYc = 0;
	}
	else if ( dwStyle & TCS_VERTICAL )//在左边框显示页标签
	{
		nX=itemRect.left;
		nXc=tabRect.right-itemRect.left-2;
		nY=itemRect.bottom+1;
		nYc=tabRect.bottom-nY-2;
		nX = nXc = nY = nYc = 0;
	}
	else if ( dwStyle & TCS_BOTTOM )//在下边框显示页标签
	{
		nX=itemRect.left;
		nXc=tabRect.right-itemRect.left-2;
		nY=1;
		nYc=tabRect.bottom-nY -2 - itemRect.bottom + itemRect.top - 2;
	}
	else//在上边框显示页标签
	{
		nX=itemRect.left;
		nXc=tabRect.right-itemRect.left-2;
		nY=itemRect.bottom+1;
		nYc=tabRect.bottom-nY-2;
	}
	
	m_pPages[nIndex]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
}

void CBaseTabCtrl::PreSubclassWindow() 
{
	CTabCtrl::PreSubclassWindow();
	ModifyStyle(0, TCS_OWNERDRAWFIXED);
}

CDialog* CBaseTabCtrl::GetPage(int nIndex)
{
	return m_pPages[nIndex];
}

BOOL CBaseTabCtrl::SetImageList( int nX, int nY, UINT uStartID, UINT uEndID )
{
	if ( m_pImagelist != NULL )
	{
		SetLastError( ERROR_OBJECT_ALREADY_EXISTS );
		return FALSE;
	}
	CBitmap bitmap;
	m_pImagelist = new CImageList();
	m_pImagelist->Create( nX, nY, ILC_MASK, 10, 10 );
	UINT nID;
	for( nID = uStartID; nID <= uEndID; nID++ )
	{
		HICON hIco = AfxGetApp()->LoadIcon( nID);
		if ( hIco == NULL ) return FALSE;
		if ( m_pImagelist->Add( hIco ) == -1 ) return FALSE;
	}
	((CTabCtrl*)this)->SetImageList( m_pImagelist );
	
	return TRUE;
}

BOOL CBaseTabCtrl::SetCurPage( int nIndex )
{
	//提示页标签在左右两边风格不支持
	DWORD dwStyle = GetStyle();
	if ( dwStyle & TCS_VERTICAL )
	{
		AfxMessageBox( "CBaseTabCtrl属性页不支持纵向排列页标，请去掉TCS_VERTICAL属性设置以及相关风格设置代码代码" );
	}
	
	//页码超出合法范围
	if ( nIndex < 0  || nIndex >= m_nPageCount ) return FALSE;
	
	//如果设置过当前页则隐藏旧的当前页
	if ( -1 != m_nCurPage )
	{
		m_pPages[m_nCurPage]->ShowWindow( SW_HIDE );
		m_pPages[m_nCurPage]->UpdateData();
	}
	m_nCurPage = nIndex;//记录新的当前页码
	SetCurFocus( m_nCurPage );
	m_pPages[m_nCurPage]->UpdateData( FALSE );
	m_pPages[m_nCurPage]->ShowWindow( SW_SHOW );//显示新当前页
	
	return TRUE;
}

void CBaseTabCtrl::OnPaint() 
{
	DWORD dwStyle = GetStyle();
	if(dwStyle&TCS_BUTTONS)
		OnButStylePaint();
	else
		OnCommPaint();
}
/*摈除背景*/
BOOL CBaseTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
	//return CTabCtrl::OnEraseBkgnd(pDC);
	DWORD dwStyle = GetStyle();
	if(!(dwStyle&TCS_BUTTONS))
	{
		int iSaveDC;
		CRect rc, rcItem;
		CBrush *hBrush=new CBrush;
		CPen *hPen=new CPen;

		hBrush->CreateSolidBrush( m_bkColor );			//211,208,200
		hPen->CreatePen(PS_SOLID, 1, m_bkColor );
		
		GetClientRect(&rc);
		GetItemRect(GetItemCount()-1, &rcItem);

		iSaveDC=pDC->SaveDC();

		pDC->SelectObject(hBrush);
		pDC->SelectObject(hPen);
		
		pDC->Rectangle(&rc);
		
		pDC->RestoreDC(iSaveDC);
		
		hPen->DeleteObject();

		iSaveDC=pDC->SaveDC();

		hPen->CreatePen(PS_SOLID, 1, RGB(255,255,255));	
		pDC->SelectObject(hPen);
		pDC->MoveTo(CPoint(rcItem.right, rcItem.bottom));
		pDC->LineTo(CPoint(rc.right-1, rcItem.bottom));
		pDC->LineTo(CPoint(rc.right-1, rc.bottom-1));
		pDC->LineTo(CPoint(rc.left+1, rc.bottom-1));
		pDC->LineTo(CPoint(rc.left+1, rcItem.bottom));

		pDC->RestoreDC(iSaveDC);

		hPen->DeleteObject();	
		hBrush->DeleteObject();

		delete hPen;
		delete hBrush;
	}
	return TRUE;
}

BOOL CBaseTabCtrl::UpdateData( BOOL flag )
{
	BOOL bSuccess = CTabCtrl::UpdateData( flag );
	int i = 0;
	
	for ( i = 0; i < m_nPageCount; i++ )
	{
		bSuccess &= m_pPages[i]->UpdateData( flag );
	}
	
	return bSuccess;
}
/*设置图标*/
BOOL CBaseTabCtrl::SetImageList(CImageList *pImageList)
{
	((CTabCtrl*)this)->SetImageList( pImageList );
	return TRUE;
}

/*绘制底图*/
BOOL CBaseTabCtrl::DrawBkBitmap( CDC * pDC, const int iTab, const CRect *pRect)
{
	CBitmap bkBitmap;
	HBITMAP hOldBitmap;
	BITMAP Bitmap;
	CDC MemDC;
	
	ASSERT(pDC);
	ASSERT(pRect);
	//如果位图ID不为0，则绘制底图
	if (m_BitmapIDDs[iTab]!=0)
	{
		MemDC.CreateCompatibleDC(pDC);
		bkBitmap.LoadBitmap(m_BitmapIDDs[iTab]);
		hOldBitmap = (HBITMAP)MemDC.SelectObject(bkBitmap);
		
		bkBitmap.GetBitmap(&Bitmap);
		//pDC->StretchBlt(pRect->left, pRect->top, Bitmap.bmWidth, Bitmap.bmHeight, &MemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, SRCCOPY);
		pDC->StretchBlt(pRect->left, pRect->top, pRect->Width(), pRect->Height(), &MemDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, SRCCOPY);

		MemDC.SelectObject(hOldBitmap);
		DeleteObject(hOldBitmap);
		bkBitmap.DeleteObject();
		MemDC.DeleteDC();
	}

	return m_BitmapIDDs[iTab];
}

/*显示标签文本,可以设置字体,未完成*/
BOOL CBaseTabCtrl::DrawTabText(CDC *pDC, const char *lpszText, const COLORREF rgb, CRect *pRect)
{
	ASSERT(pDC);
	ASSERT(pRect);
	ASSERT(lpszText);

	pDC->SetBkMode(TRANSPARENT);	
	pDC->SetTextColor(rgb);
	pDC->DrawText(lpszText, pRect, DT_SINGLELINE|DT_BOTTOM|DT_CENTER|DT_VCENTER);

	return TRUE;
}
/*画ICON*/
BOOL CBaseTabCtrl::DrawTabIcon(CDC *pDC, const TC_ITEM tci, CRect *pRect)
{
	//画ICON
	ASSERT(pDC);
	ASSERT(pRect);
	CImageList* pImageList = GetImageList();
	//HIMAGELIST hilTabs = (HIMAGELIST)TabCtrl_GetImageList(GetSafeHwnd());
	if (pImageList && tci.iImage >= 0) 
	{
		pRect->left += pDC->GetTextExtent(_T(" ")).cx;
		
		//得到ICON的高度
		IMAGEINFO info;
		pImageList->GetImageInfo(tci.iImage, &info);
		CRect ImageRect(info.rcImage);
		int nYpos = pRect->top;
		//画ICON
		pImageList->Draw(pDC, tci.iImage, CPoint(pRect->left, nYpos), ILD_TRANSPARENT);
		//ImageList_Draw(hilTabs, tci.iImage, dc.m_hDC, rcPage.left, rcPage.top, ILD_TRANSPARENT);
		pRect->left += ImageRect.Width();
	}

	return pImageList?TRUE:FALSE;
}

/*按钮风格绘制*/
BOOL CBaseTabCtrl::OnButStylePaint()
{
	CPaintDC dc(this); // device context for painting
	
	int iSave;
	char szLable[64];
	TC_ITEM tci;
	CRect rc;
	
	//DRAWITEMSTRUCT信息
	LPDRAWITEMSTRUCT lpDrawItemStruct=new DRAWITEMSTRUCT;
	//填充DRAWITEMSTRUCT信息
	lpDrawItemStruct->CtlID=GetDlgCtrlID();
	lpDrawItemStruct->CtlType=ODT_TAB;
	lpDrawItemStruct->hwndItem=GetSafeHwnd();
	lpDrawItemStruct->hDC=dc.GetSafeHdc();
	lpDrawItemStruct->itemAction=ODA_DRAWENTIRE;
	
	//得到总页数和当前页码
	int iTab=GetItemCount();
	int iSel=GetCurSel();
	
	if (!iTab)
		return FALSE;
	//循环画同每个页
	while (iTab--)
	{
		//得到指定页面的信息
		tci.mask = TCIF_TEXT|TCIF_IMAGE;
		tci.pszText = szLable;
		tci.cchTextMax = 63;
		if (!GetItem(iTab, &tci )) 
			return FALSE;
		//绘非当前页
		if (iTab != iSel)
		{
			lpDrawItemStruct->itemID = iTab;
			lpDrawItemStruct->itemState = 0;
			
			VERIFY(GetItemRect(iTab, &lpDrawItemStruct->rcItem));
			rc=lpDrawItemStruct->rcItem;
			
			int iSave=dc.SaveDC();
			
			if (m_BitmapIDDs[iTab]!=0)	//图片作为底图
			{
				DrawBkBitmap(&dc, iTab, &rc);
			}
			else						//GDI自绘
			{
				CPen pen,*oldPen;
				//left line
				pen.CreatePen(PS_SOLID, 1, RGB(255,255,255));
				oldPen=dc.SelectObject(&pen);
				dc.MoveTo(CPoint(rc.left,rc.top));
				dc.LineTo(CPoint(rc.left,rc.bottom));
				dc.SelectObject(oldPen);
				pen.DeleteObject();
				oldPen->DeleteObject();
				//top line
				pen.CreatePen(PS_SOLID, 1, RGB(255,255,255));
				oldPen=dc.SelectObject(&pen);
				dc.MoveTo(CPoint(rc.left,rc.top));
				dc.LineTo(CPoint(rc.right-1,rc.top));
				dc.SelectObject(oldPen);
				pen.DeleteObject();
				oldPen->DeleteObject();
				//right lines
				pen.CreatePen(PS_SOLID, 1, RGB(128,128,128));	
				oldPen=dc.SelectObject(&pen);
				dc.MoveTo(CPoint(rc.right-2, rc.top+1));
				dc.LineTo(CPoint(rc.right-2, rc.bottom-1));
				dc.SelectObject(oldPen);
				pen.DeleteObject();
				oldPen->DeleteObject();

				//bottom lines
				pen.CreatePen(PS_SOLID, 1, RGB(128,128,128));	
				oldPen=dc.SelectObject(&pen);
				dc.MoveTo(CPoint(rc.left+1, rc.bottom-1));
				dc.LineTo(CPoint(rc.right-1, rc.bottom-1));
				dc.SelectObject(oldPen);
				pen.DeleteObject();
				oldPen->DeleteObject();
				//填充标签
				CRect brc = rc;
				
				brc.DeflateRect(1,2,2,0);
				dc.FillSolidRect(&brc, m_itemColor);
			}
			//显示标签文本,可以设置字体
			rc.top+=m_nYOffset;
			rc.left+=m_nXOffset;
			CFont *oldFont = (CFont*)dc.SelectObject(&m_textFont);
			DrawTabText(&dc, szLable, m_textColor, &rc);
			dc.SelectObject(oldFont);
			oldFont->DeleteObject();
			dc.RestoreDC(iSave);
			
			m_pPages[iTab]->ShowWindow(SW_HIDE);
		}
		else
		{
			lpDrawItemStruct->itemID = iSel;
			lpDrawItemStruct->itemState = ODS_SELECTED;
			VERIFY(GetItemRect(iSel, &lpDrawItemStruct->rcItem));

			rc=lpDrawItemStruct->rcItem;
			
			iSave=dc.SaveDC();

			if (m_BitmapIDDs[iTab]!=0)	//图片作为底图
			{
				DrawBkBitmap(&dc, iTab, &rc);
			}
			else						//GDI自绘
			{
				CPen pen, *oldPen;
				//left lines
				pen.CreatePen(PS_SOLID, 1, RGB(64,64,64));
				oldPen=dc.SelectObject(&pen);
				dc.MoveTo(CPoint(rc.left,rc.top));
				dc.LineTo(CPoint(rc.left,rc.bottom));
				dc.SelectObject(oldPen);
				pen.DeleteObject();
				oldPen->DeleteObject();

				pen.CreatePen(PS_SOLID, 1, RGB(128,128,128));
				oldPen=dc.SelectObject(&pen);
				dc.MoveTo(CPoint(rc.left+1,rc.top+1));
				dc.LineTo(CPoint(rc.left+1,rc.bottom-1));
				dc.SelectObject(oldPen);
				pen.DeleteObject();
				oldPen->DeleteObject();
				//top lines
				pen.CreatePen(PS_SOLID, 1, RGB(64,64,64));
				oldPen=dc.SelectObject(&pen);
				dc.MoveTo(CPoint(rc.left,rc.top));
				dc.LineTo(CPoint(rc.right-1,rc.top));
				dc.SelectObject(oldPen);
				pen.DeleteObject();
				oldPen->DeleteObject();

				pen.CreatePen(PS_SOLID, 1, RGB(128,128,128));
				oldPen=dc.SelectObject(&pen);
				dc.MoveTo(CPoint(rc.left+1,rc.top+1));
				dc.LineTo(CPoint(rc.right-1,rc.top+1));
				dc.SelectObject(oldPen);
				pen.DeleteObject();
				oldPen->DeleteObject();
				//right lines
				pen.CreatePen(PS_SOLID, 1, RGB(128,128,128));	
				oldPen=dc.SelectObject(&pen);
				dc.MoveTo(CPoint(rc.right-2, rc.top+1));
				dc.LineTo(CPoint(rc.right-2, rc.bottom-1));
				dc.SelectObject(oldPen);
				pen.DeleteObject();
				oldPen->DeleteObject();

				//bottom lines
				pen.CreatePen(PS_SOLID, 1, RGB(128,128,128));	
				oldPen=dc.SelectObject(&pen);
				dc.MoveTo(CPoint(rc.left+1, rc.bottom-1));
				dc.LineTo(CPoint(rc.right-1, rc.bottom-1));
				dc.SelectObject(oldPen);
				pen.DeleteObject();
				oldPen->DeleteObject();
				
				//填充标签
				CRect brc = rc;
			
				brc.DeflateRect(1,2,2,0);
				dc.FillSolidRect(&brc, m_curItemColor);
			}
			//显示标签文本,可以设置字体
			rc.top+=m_nYOffset;
			rc.left+=m_nXOffset;
			CFont *oldFont = (CFont *)dc.SelectObject(&m_textFont);
			DrawTabText(&dc, szLable, m_textColor, &rc);
			dc.SelectObject(oldFont);
			oldFont->DeleteObject();
			dc.RestoreDC(iSave);
			
			m_pPages[iSel]->ShowWindow(SW_SHOW);
		}
		
		//画ICON
		DrawTabIcon(&dc, tci, &rc);
	}

	delete lpDrawItemStruct;
	return TRUE;
}
/*普通风格绘制*/
BOOL CBaseTabCtrl::OnCommPaint()
{
	CPaintDC dc(this); // device context for painting
	
	int iSave;
	char szLable[64];
	TC_ITEM tci;
	CRect rc;
	
	//DRAWITEMSTRUCT信息
	LPDRAWITEMSTRUCT lpDrawItemStruct=new DRAWITEMSTRUCT;
	//填充DRAWITEMSTRUCT信息
	lpDrawItemStruct->CtlID=GetDlgCtrlID();
	lpDrawItemStruct->CtlType=ODT_TAB;
	lpDrawItemStruct->hwndItem=GetSafeHwnd();
	lpDrawItemStruct->hDC=dc.GetSafeHdc();
	lpDrawItemStruct->itemAction=ODA_DRAWENTIRE;
	
	//得到总页数和当前页码
	int iTab=GetItemCount();
	int iSel=GetCurSel();
	
	if (!iTab)
		return FALSE;
	//循环画同每个页
	while (iTab--)
	{
		//得到指定页面的信息
		tci.mask = TCIF_TEXT|TCIF_IMAGE;
		tci.pszText = szLable;
		tci.cchTextMax = 63;
		if (!GetItem(iTab, &tci )) 
			return FALSE;
		//绘非当前页
		if (iTab != iSel)
		{
			lpDrawItemStruct->itemID = iTab;
			lpDrawItemStruct->itemState = 0;
			
			VERIFY(GetItemRect(iTab, &lpDrawItemStruct->rcItem));
			rc=lpDrawItemStruct->rcItem;

			int iSave=dc.SaveDC();		//存储DC状态

			if (m_BitmapIDDs[iTab]!=0)	//图片作为底图
			{
				DrawBkBitmap(&dc, iTab, &rc);
			}
			else						//GDI自绘
			{
				CPen *pen=new CPen;
				CPen *oldPen;
				//left pixel
				dc.SetPixel(rc.left+1, rc.top+1, RGB(255,255,255));
				//left line
				pen->CreatePen(PS_SOLID, 1, RGB(255,255,255));
				oldPen=dc.SelectObject(pen);

				dc.MoveTo(CPoint(rc.left,rc.top+2));
				dc.LineTo(CPoint(rc.left,rc.bottom));

				dc.SelectObject(oldPen);
				pen->DeleteObject();
				oldPen->DeleteObject();
				//top line
				pen->CreatePen(PS_SOLID, 1, RGB(255,255,255));
				oldPen=dc.SelectObject(pen);

				dc.MoveTo(CPoint(rc.left+2,rc.top));
				dc.LineTo(CPoint(rc.right-2,rc.top));

				dc.SelectObject(oldPen);
				pen->DeleteObject();
				oldPen->DeleteObject();
				//right pixel
				dc.SetPixel(rc.right-2, rc.top+1, RGB(64,64,64));
				//right lines
				pen->CreatePen(PS_SOLID, 1, RGB(128,128,128));	
				oldPen=dc.SelectObject(pen);

				dc.MoveTo(CPoint(rc.right-2, rc.top+2));
				dc.LineTo(CPoint(rc.right-2, rc.bottom));

				dc.SelectObject(oldPen);
				pen->DeleteObject();
				oldPen->DeleteObject();
				
				pen->CreatePen(PS_SOLID, 1, RGB(64,64,64));		
				oldPen=dc.SelectObject(pen);

				dc.MoveTo(CPoint(rc.right-1, rc.top+2));
				dc.LineTo(CPoint(rc.right-1, rc.bottom));

				dc.SelectObject(oldPen);
				pen->DeleteObject();
				oldPen->DeleteObject();
				//bottom line
				pen->CreatePen(PS_SOLID, 1, RGB(255,255,255));	
				oldPen=dc.SelectObject(pen);

				dc.MoveTo(CPoint(rc.left, rc.bottom));
				dc.LineTo(CPoint(rc.right, rc.bottom));

				dc.SelectObject(oldPen);
				pen->DeleteObject();
				oldPen->DeleteObject();
				//填充标签
				CRect brc = rc;

				brc.DeflateRect(1,2,2,0);
				dc.FillSolidRect(&brc, m_itemColor);

				delete pen;
			}
			//显示标签文本,可以设置字体
			rc.top+=m_nYOffset;
			rc.left+=m_nXOffset;

			CFont *oldFont = (CFont*) dc.SelectObject(&m_textFont);
			DrawTabText(&dc, szLable, m_textColor, &rc);
			
			dc.SelectObject(oldFont);
			dc.RestoreDC(iSave);		//恢复DC状态

			oldFont->DeleteObject();

			m_pPages[iTab]->ShowWindow(SW_HIDE);
		}
		else//选中项
		{
			lpDrawItemStruct->itemID = iSel;
			lpDrawItemStruct->itemState = ODS_SELECTED;
			VERIFY(GetItemRect(iSel, &lpDrawItemStruct->rcItem));
			
			//突出显示
			lpDrawItemStruct->rcItem.bottom += 2;
			lpDrawItemStruct->rcItem.top -= 2;
			
			rc=lpDrawItemStruct->rcItem;
			
			iSave=dc.SaveDC();

			if (m_BitmapIDDs[iTab]!=0)	//图片作为底图
			{
				DrawBkBitmap(&dc, iTab, &rc);
			}
			else						//GDI自绘
			{
				CPen *pen=new CPen;
				CPen *oldPen;
				//left pixel
				dc.SetPixel(rc.left+1, rc.top+1, RGB(255,255,255));
				//left line
				pen->CreatePen(PS_SOLID, 1, RGB(255,255,255));
				oldPen = dc.SelectObject(pen);
				
				dc.MoveTo(CPoint(rc.left,rc.top+2));
				dc.LineTo(CPoint(rc.left,rc.bottom));
				
				dc.SelectObject(oldPen);
				pen->DeleteObject();
				oldPen->DeleteObject();
				//top line
				pen->CreatePen(PS_SOLID, 1, RGB(255,255,255));
				oldPen = dc.SelectObject(pen);

				dc.MoveTo(CPoint(rc.left+2,rc.top));
				dc.LineTo(CPoint(rc.right-2,rc.top));
				
				dc.SelectObject(oldPen);
				pen->DeleteObject();
				oldPen->DeleteObject();
				//right pixel
				dc.SetPixel(rc.right-2, rc.top+1, RGB(64,64,64));
				//right lines
				pen->CreatePen(PS_SOLID, 1, RGB(128,128,128));	//RGB(128,128,128)
				oldPen = dc.SelectObject(pen);

				dc.MoveTo(CPoint(rc.right-2, rc.top+2));
				dc.LineTo(CPoint(rc.right-2, rc.bottom));
				
				dc.SelectObject(oldPen);
				pen->DeleteObject();
				oldPen->DeleteObject();
				
				pen->CreatePen(PS_SOLID, 1, RGB(64,64,64));		//RGB(128,128,128)
				oldPen = dc.SelectObject(pen);

				dc.MoveTo(CPoint(rc.right-1, rc.top+2));
				dc.LineTo(CPoint(rc.right-1, rc.bottom));

				dc.SelectObject(oldPen);
				pen->DeleteObject();
				oldPen->DeleteObject();
				//填充标签
				CRect brc = rc;
				
				brc.DeflateRect(1,2,2,0);
				dc.FillSolidRect(&brc, m_curItemColor);

				delete pen;
			}
			//显示标签文本,可以设置字体
			rc.top+=m_nYOffset;
			rc.left+=m_nXOffset;
			CFont *oldFont = (CFont*) dc.SelectObject(&m_textFont);
			DrawTabText(&dc, szLable, m_textColor, &rc);
			dc.SelectObject(oldFont);
			dc.RestoreDC(iSave);
			oldFont->DeleteObject();
			
			m_pPages[iSel]->ShowWindow(SW_SHOW);
			
		}
		//画ICON
		DrawTabIcon(&dc, tci, &rc);
	}
	
	ReleaseDC(&dc);
	delete lpDrawItemStruct;
	return TRUE;
}

void CBaseTabCtrl::SetBkColor( COLORREF clr )
{
	m_bkColor = clr;
}

void CBaseTabCtrl::SetTextColor(COLORREF rgb)
{
	m_textColor = rgb;
}

void CBaseTabCtrl::SetTextFont(HFONT hFont)
{
	m_textFont.Attach(hFont);
}

void CBaseTabCtrl::SetTextFont(LOGFONT *logfont)
{
	m_textFont.DeleteObject();
	m_textFont.CreateFontIndirect(logfont);
}

void CBaseTabCtrl::SetTextXOffset(int x)
{
	m_nXOffset = x;
}

void CBaseTabCtrl::SetTextYOffset(int y)
{
	m_nYOffset = y;
}

void CBaseTabCtrl::SetCurItemColor(COLORREF rgb)
{
	m_curItemColor = rgb;
}

void CBaseTabCtrl::SetItemColor(COLORREF rgb)
{
	m_itemColor = rgb;
}
