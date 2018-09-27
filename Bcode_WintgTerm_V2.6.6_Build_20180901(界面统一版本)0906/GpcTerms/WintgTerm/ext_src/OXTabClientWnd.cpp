// ==========================================================================
//						Class Implementation : 
//		COXTabClientWnd & COXTabWorkspace & COXTabWorkspaceDropTarget
// ==========================================================================

// Source file : OXTabClientWnd.cpp

// Copyright ?Dundas Software Ltd. 1997 - 1998, All Rights Reserved                      

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXTabClientWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UM_GET_FRAME_TYPE       (WM_USER + 1000)
#define UM_SET_FRAME_ACTIVE       (WM_USER + 1001)

// Change tab on drag over handler
DROPEFFECT COXTabWorkspaceDropTarget::OnDragOver(CWnd* pWnd, 
												 COleDataObject* pDataObject,
												 DWORD dwKeyState, 
												 CPoint point)
{
	UNREFERENCED_PARAMETER(pDataObject);
	UNREFERENCED_PARAMETER(dwKeyState);

	// get the pointer to tab control
    COXTabWorkspace* pTabWorkspace=(COXTabWorkspace*)pWnd;
    TC_HITTESTINFO hitTest;
    hitTest.pt=point;
    
	// find the drop target item 
    int nItem=pTabWorkspace->HitTest(&hitTest);

	// if found ...
    if(nItem>=0)
    {
		// get the pointer to corresponding MDIChild 
		CWnd* pChildWnd=pTabWorkspace->m_arrTab[nItem].pWnd;

		if(::IsWindow(pChildWnd->GetSafeHwnd()))
		{
			// activate it
			pTabWorkspace->GetParentFrame()->MDIActivate(pChildWnd);
		}
    }

    return DROPEFFECT_NONE;
}


COXTabWorkspace::COXTabWorkspace()
{
	m_pTabClientWnd=NULL;
	m_dwOffset=ID_TABOFFSET;
	m_bAcceptDraggedObject=TRUE;

	m_textColor = RGB(0, 0, 0);
	m_curItemColor = RGB(0, 255,255); //231, 236, 247
	m_itemColor = RGB(224, 224, 192);
}


COXTabWorkspace::~COXTabWorkspace()
{
}


BEGIN_MESSAGE_MAP(COXTabWorkspace, CTabCtrl)
	//{{AFX_MSG_MAP(COXTabWorkspace)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchange)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COXTabWorkspace::PreSubclassWindow() 
{
	ModifyStyle(0, TCS_OWNERDRAWFIXED);
	CTabCtrl::PreSubclassWindow();
	//ModifyStyle(BS_TYPEMASK, TCS_OWNERDRAWFIXED, SWP_FRAMECHANGED); 
}

int COXTabWorkspace::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CTabCtrl::OnCreate(lpCreateStruct)==-1)
		return -1;
	
    // set status timer
    if(SetTimer(IDT_MDI_STATUS_TIMER,500,NULL)==0)
	{
		TRACE(_T("COXTabWorkspace::OnCreate: SetTimer() failed\n"));
		return -1;
	}
    
	// create image list 
    if(!m_imageList.Create(GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),ILC_COLOR8|ILC_MASK,4,4))
	{
		TRACE(_T("COXTabWorkspace::OnCreate: Create() image list failed\n"));
		return -1;
	}
    
	// set default font
	SendMessage(WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT));

	// set image list that will be in future populated with MDIChild
	// window icons
    SetImageList(&m_imageList);

	// register drop-target
	if(!m_dropTarget.Register(this))
	{
		TRACE(_T("COXTabWorkspace::OnCreate: failed to register the control with COleDropTarget\n"));
		TRACE(_T("you've probably forgot to initialize OLE libraries using AfxOleInit function\n"));
	}

	return 0;
}


void COXTabWorkspace::OnNcCalcSize(BOOL bCalcValidRects, 
								   NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// TODO: Add your message handler code here and/or call default
	
	CTabCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);

	// add an offset
	if(lpncsp->rgrc[0].bottom-lpncsp->rgrc[0].top>2*(int)m_dwOffset)
	{
		lpncsp->rgrc[0].top+=m_dwOffset;
		lpncsp->rgrc[0].bottom-=m_dwOffset;
	}
	else
	{
		lpncsp->rgrc[0].top=lpncsp->rgrc[0].bottom;
	}

	if(lpncsp->rgrc[0].right-lpncsp->rgrc[0].left>2*(int)m_dwOffset)
	{
		lpncsp->rgrc[0].left+=m_dwOffset;
		lpncsp->rgrc[0].right-=m_dwOffset;
	}
	else
	{
		lpncsp->rgrc[0].left=lpncsp->rgrc[0].right;
	}
}

void COXTabWorkspace::OnNcPaint() 
{
	// TODO: Add your message handler code here
	CWindowDC dc(this);
	CRect rectClient;
	GetClientRect(rectClient);
	CRect rectWindow;
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	if(rectClient.top<rectClient.bottom && rectClient.top<rectClient.bottom)
		dc.ExcludeClipRect(rectClient);

	// erase parts not drawn
	SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);

	// draw borders in non-client area
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.Draw3dRect(rectWindow,::GetSysColor(COLOR_BTNSHADOW),
		::GetSysColor(COLOR_BTNHILIGHT));

	// Do not call CTabCtrl::OnNcPaint() for painting messages
}

void COXTabWorkspace::OnDestroy() 
{
	CTabCtrl::OnDestroy();
	
	// TODO: Add your message handler code here

	// delete image list
	VERIFY(m_imageList.DeleteImageList());
	// kill update timer
	VERIFY(KillTimer(IDT_MDI_STATUS_TIMER));

	// clean up internal arrays
	m_arrImage.RemoveAll();
	m_arrTab.RemoveAll();
}

static int DrawTextInRect(CDC *pDC,const char *str,RECT *prect,int coltap,int rowtap,BYTE type)
{
	char substr[4];
	int x,y;
	CSize size;
        size.cx=16;size.cy=16;
	x=prect->left;
	y=prect->top;
	pDC->SetTextAlign(0); 
	while(*str){
		if(*str==10){  // ignore linefeed
     		str++; 
			continue;
        }else if(*str==13){ // return
			if(type == 0){
				x=prect->left;
				y+=size.cy+coltap*size.cy/10;
			}
			str++;
            continue;
        }
		if((BYTE)*str>0x80){
			substr[0]=*str++;
			substr[1]=*str++;
			substr[2]=0;
		}else{
			substr[0]=*str++;
			substr[1]=0;
		}
		size=pDC->GetTextExtent(substr,strlen(substr));
		if((x+size.cx)>prect->right){
			x=prect->left;
			y+=size.cy+coltap*size.cy/10;
		}
		if(y+size.cy>prect->bottom)  // CUIHU,
			break;
		pDC->TextOut(x,y,substr,strlen(substr));
		x+=size.cx+size.cx*coltap/(10*strlen(substr));
	}
	return 0;
}

BOOL COXTabWorkspace::DrawTabText(CDC *pDC, const char *lpszText, const COLORREF rgb, CRect *pRect)
{
	ASSERT(pDC);
	ASSERT(pRect);
	ASSERT(lpszText);

	pDC->SetBkMode(TRANSPARENT);	
	pDC->SetTextColor(rgb);
	CSize r=pDC->GetTextExtent(lpszText); 
	pDC->TextOut(pRect->left+(pRect->Width()-r.cy)/2 ,pRect->bottom-(pRect->Height()-r.cx)/2  ,lpszText,strlen(lpszText));
	//pDC->DrawText(lpszText, pRect, DT_BOTTOM);
	//DrawTextInRect(pDC,lpszText,pRect,2,2,0);

	return TRUE;
}
BOOL COXTabWorkspace::DrawTabIcon(CDC *pDC, const TC_ITEM tci, CRect *pRect)
{
	//画ICON
	ASSERT(pDC);
	ASSERT(pRect);
	CImageList* pImageList = GetImageList();
	//HIMAGELIST hilTabs = (HIMAGELIST)TabCtrl_GetImageList(GetSafeHwnd());
	if (pImageList && tci.iImage >= 0) 
	{
		//pRect->left += pDC->GetTextExtent(_T(" ")).cx;
		//CSize txtsize=pDC->GetTextExtent(tci.pszText);  
		//得到ICON的高度
		IMAGEINFO info;
		pImageList->GetImageInfo(tci.iImage, &info);
		CRect ImageRect(info.rcImage);
		int nYpos=pRect->top+(pRect->Height()-ImageRect.Height())/2;
		int nXpos=pRect->left+(pRect->Width()-ImageRect.Width())/2;
		//画ICON
		pImageList->Draw(pDC, tci.iImage, CPoint(nXpos, nYpos), ILD_TRANSPARENT);
		//ImageList_Draw(hilTabs, tci.iImage, dc.m_hDC, rcPage.left, rcPage.top, ILD_TRANSPARENT);

	}

	return (BOOL)pImageList;
}

void COXTabWorkspace::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    
	//CTabCtrl::DrawItem(lpDrawItemStruct); 
	/*
	if(lpDrawItemStruct->itemID==GetCurSel())
	{
		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC); 
		CRect rc(lpDrawItemStruct->rcItem);
		rc.DeflateRect(2,2);
  	    CPen pen;
	    pen.CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	    CPen* pOldPen = dc.SelectObject(&pen);
	    dc.Rectangle(rc);
	    dc.SelectObject(pOldPen);
		dc.Detach();
	}*/
    char     szTabText[100];      
     TC_ITEM  tci;      
    
//  TRACE("item:%d\n",lpDrawItemStruct->itemID);       

    memset(szTabText,   '\0',   sizeof(szTabText));      
    tci.mask                 =   TCIF_TEXT|TCIF_IMAGE;      
    tci.pszText         =   szTabText;      
    tci.cchTextMax     =   sizeof(szTabText)-1;      
     
    GetItem(lpDrawItemStruct->itemID,   &tci);      
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC); 
	if(lpDrawItemStruct->itemID==GetCurSel())
	{
		CBrush brush(m_curItemColor);
		dc.FillRect(&lpDrawItemStruct->rcItem,&brush);    
	    dc.SetBkColor(m_curItemColor);    
	}
	else
	{
		CBrush brush(m_itemColor);
		dc.FillRect(&lpDrawItemStruct->rcItem,&brush);    
		dc.SetBkColor(m_itemColor);    
	}
	CRect rc(lpDrawItemStruct->rcItem);
	if(strlen(tci.pszText)==0)
		DrawTabIcon(&dc,tci,&rc);
	else
	{
		CRect textrect=lpDrawItemStruct->rcItem;
		DrawTabText(&dc,tci.pszText,RGB(0,0,0),&textrect);
	   //dc.SetTextColor(RGB(0,0,0)); 
       //dc.TextOut(lpDrawItemStruct->rcItem.left+5,      
        //lpDrawItemStruct->rcItem.top+5,      
        //tci.pszText,      
        //lstrlen(tci.pszText));      
	}
	dc.Detach();
}

/*
void COXTabWorkspace::OnPaint()
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
		return;
	//循环画同每个页
	while (iTab--)
	{
		//得到指定页面的信息
		tci.mask = TCIF_TEXT|TCIF_IMAGE;
		tci.pszText = szLable;
		tci.cchTextMax = 63;
		if (!GetItem(iTab, &tci )) 
			return;
		//绘非当前页
		if (iTab != iSel)
		{
			lpDrawItemStruct->itemID = iTab;
			lpDrawItemStruct->itemState = 0;
			
			VERIFY(GetItemRect(iTab, &lpDrawItemStruct->rcItem));
			rc=lpDrawItemStruct->rcItem;

			int iSave=dc.SaveDC();		//存储DC状态

			
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
			
			//显示标签文本,可以设置字体
			//rc.top+=m_nYOffset;
			//rc.left+=m_nXOffset;

			//CFont *oldFont = (CFont*) dc.SelectObject(&m_textFont);
			DrawTabText(&dc, szLable, m_textColor, &rc);
			
			//dc.SelectObject(oldFont);
			dc.RestoreDC(iSave);		//恢复DC状态

			//oldFont->DeleteObject();

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
			
			//显示标签文本,可以设置字体
			//rc.top+=m_nYOffset;
			//rc.left+=m_nXOffset;
			//CFont *oldFont = (CFont*) dc.SelectObject(&m_textFont);
			DrawTabText(&dc, szLable, m_textColor, &rc);
			//dc.SelectObject(oldFont);
			dc.RestoreDC(iSave);
			//oldFont->DeleteObject();
			
		}
		//画ICON
		DrawTabIcon(&dc, tci, &rc);
	}
	
	delete lpDrawItemStruct;
}
*/
//////////////////////////////////////////////////////////////////////////////
// Description: Timer handler - periodically refreshes tabs, determines
//				active MDIChild and updates window names
//////////////////////////////////////////////////////////////////////////////
void COXTabWorkspace::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent!=IDT_MDI_STATUS_TIMER)
	{
		CTabCtrl::OnTimer(nIDEvent);
		return;
	}

	// update active MDIChild and window text
	UpdateContents(TRUE);
}


//////////////////////////////////////////////////////////////////////////////
// Description: Selection change handler
//////////////////////////////////////////////////////////////////////////////
BOOL COXTabWorkspace::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UNREFERENCED_PARAMETER(pNMHDR);

    if(GetCurSel()>=0)
    {
		// get the pointer to the MDIChild that will be activated
		CWnd* pChildWnd=m_arrTab[GetCurSel()].pWnd;

		if(::IsWindow(pChildWnd->GetSafeHwnd()))
		{
			GetParentFrame()->MDIActivate(pChildWnd);
			pChildWnd->SendMessage(UM_SET_FRAME_ACTIVE);
		}
    }

	// update the size and position of the tab control and MDIClient window
	ASSERT(::IsWindow(GetParentFrame()->GetSafeHwnd()));
	GetParentFrame()->RecalcLayout();

	*pResult = 0;

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////////
// Description: Double click handler - maximizes / restores.
//////////////////////////////////////////////////////////////////////////////
void COXTabWorkspace::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);

    if(GetCurSel()>=0)
    {
		// get the pointer to the MDIChild that will be maximized/restored
		CWnd* pChildWnd=m_arrTab[GetCurSel()].pWnd;

		if(::IsWindow(pChildWnd->GetSafeHwnd()))
		{
		    BOOL bMaximize=FALSE;
			CWnd* pActiveWnd=GetParentFrame()->MDIGetActive(&bMaximize);

			// maximize or restore MDIChild window based on its current state
			if(pActiveWnd==pChildWnd && bMaximize)
				GetParentFrame()->MDIRestore(pChildWnd);
			else
				GetParentFrame()->MDIMaximize(pChildWnd);
		}
    }
}

// scan through all MDIChild windows and refreshes window names and 
// current active window. If bAddNewWindows is set to TRUE then if any
// new MDIChild is found it will be added to the tab control
void COXTabWorkspace::UpdateContents(BOOL bAddNewWindows/*=FALSE*/)
{
	ASSERT(m_pTabClientWnd!=NULL);

    // check MDI windows
    CMDIFrameWnd* pFrameWnd=GetParentFrame();
    if(pFrameWnd==NULL)
		return;

	BOOL bRecalc=m_pTabClientWnd->m_bForceToRecalc;

	// get pointer to currently active MDIChild
    CWnd* pActiveChildWnd=pFrameWnd->MDIGetActive(NULL);

    CMDIChildWnd* pChildWnd=NULL;
    int nActiveIndex=-1;

	// start enumerating from currently active MDIChild
    if(pActiveChildWnd!=NULL)
		pChildWnd=(CMDIChildWnd*)pActiveChildWnd->GetWindow(GW_HWNDFIRST);

    // flag all current tabs as unfound (for debug purposes in order to check
	// the integrity of the framework)
#ifdef _DEBUG
    for(int nIndex=0; nIndex<m_arrTab.GetSize(); nIndex++)
		m_arrTab[nIndex].bFound=FALSE;
#endif

	// enumerate all child windows
    while(pChildWnd!=NULL)
    {
		// window text
		CString sWindowText=GetTextForTabItem(pChildWnd);

		// see if can find
		int nFoundItem=FindTabItem(pChildWnd->GetSafeHwnd());

		if(nFoundItem!=-1)
		{
			if((pChildWnd->GetStyle()&WS_VISIBLE)==WS_VISIBLE)
			{
				if(pChildWnd==pActiveChildWnd)
					// found currently active MDIChild
					nActiveIndex=nFoundItem;

#ifdef _DEBUG
				m_arrTab[nFoundItem].bFound=TRUE;
#endif

				// update text if necessary
				if(m_arrTab[nFoundItem].sText!=sWindowText)
				{
					m_arrTab[nFoundItem].sText=sWindowText;

					TC_ITEM tci;
					tci.mask=TCIF_TEXT;
					tci.pszText=(LPTSTR)(LPCTSTR)sWindowText;
					SetItem(nFoundItem,&tci);

					bRecalc=TRUE;
				}
			}
			else
			{
				RemoveTabItem(pChildWnd,FALSE);
				bRecalc=TRUE;
			}
		}
		else if(bAddNewWindows)
		{
			// add item
			AddTabItem(pChildWnd,FALSE);
			bRecalc=TRUE;
		}

		// get next MDIChild
		pChildWnd=(CMDIChildWnd*)pChildWnd->GetWindow(GW_HWNDNEXT);
    }

#ifdef _DEBUG
    for(nIndex=0; nIndex<m_arrTab.GetSize(); nIndex++)
		ASSERT(m_arrTab[nIndex].bFound);
#endif

	// set the active item
    if(nActiveIndex>=0 && GetCurSel()!=nActiveIndex)
	{
		SetCurSel(nActiveIndex);
		bRecalc=TRUE;
	}

	if(bRecalc)
	{
		// update the size and position of the tab control and MDIClient window
		if(::IsWindow(GetParentFrame()->GetSafeHwnd()))
			GetParentFrame()->RecalcLayout();
		m_pTabClientWnd->m_bForceToRecalc=FALSE;
	}
}

// retrieves pointer to the MDIFrame window
CMDIFrameWnd* COXTabWorkspace::GetParentFrame() const 
{ 
	ASSERT(m_pTabClientWnd!=NULL);
	ASSERT(m_pTabClientWnd->IsAttached());

	// request MDIClient for the parent frame
	CMDIFrameWnd* pParentFrame=m_pTabClientWnd->GetParentFrame();
	ASSERT(pParentFrame!=NULL);

	return pParentFrame; 
}

// finds the tab item for specified window. Returns -1 if not found
int COXTabWorkspace::FindTabItem(const HWND hWnd) const
{
	int nFoundItem=-1;
	// loop through all tab items
	for(int nIndex=0; nIndex<m_arrTab.GetSize(); nIndex++)
	{
		// check for window handle
		if(m_arrTab[nIndex].pWnd->GetSafeHwnd()==hWnd)
		{
			// double check for window class name
			TCHAR sWndClass[512];
			GetClassName(hWnd,sWndClass,sizeof(sWndClass)/sizeof(TCHAR));
			if(m_arrTab[nIndex].sWndClass==sWndClass)
			{
				nFoundItem=nIndex;
				break;
			}
		}
	}

	return nFoundItem;
}

// add new item into the tab control for specified MDIChild. If bRedraw is
// set to TRUE then framework will be redrawn in order to show new item.
BOOL COXTabWorkspace::AddTabItem(const CWnd* pChildWnd, BOOL bRedraw/*=TRUE*/,
								 BOOL bOnlyVisible/*=TRUE*/)
{
	ASSERT(pChildWnd!=NULL);
	ASSERT(::IsWindow(pChildWnd->GetSafeHwnd()));

	// make sure we add MDIChild window
	if((pChildWnd->GetExStyle()&WS_EX_MDICHILD)==0)
		return FALSE;

	// make sure it is visible at the moment
	if(bOnlyVisible && (pChildWnd->GetStyle()&WS_VISIBLE)==0)
		return FALSE;

	// work out icon...
	TCHAR sWndClass[512];
	WNDCLASSEX classInfo={ sizeof (WNDCLASSEX) };
	GetClassName(pChildWnd->m_hWnd,sWndClass,sizeof(sWndClass)/sizeof(TCHAR));
	GetClassInfoEx(AfxGetInstanceHandle(),sWndClass,&classInfo);
    
	HICON hIcon=(HICON)::GetClassLong(pChildWnd->m_hWnd,GCL_HICONSM);
	if(hIcon==NULL)
	{
		CWnd* pWnd=(CWnd*)pChildWnd;
		hIcon=(HICON)pWnd->SendMessage(WM_GETICON,ICON_SMALL);
		if(hIcon==NULL)
		{
			hIcon=(HICON)::GetClassLong(pChildWnd->m_hWnd,GCL_HICON);
			if(hIcon==NULL)
				hIcon=(HICON)pWnd->SendMessage(WM_GETICON,ICON_BIG);
		}
	}

	int nImageIndex=-1;
	if(hIcon!=NULL)
	{
		// check if we already included the specifyed item into the tab control
		// image list
		for(nImageIndex=0; nImageIndex<m_arrImage.GetSize(); nImageIndex++)
			if(m_arrImage[nImageIndex]==hIcon)
				// found
				break;

		if(nImageIndex==m_arrImage.GetSize())
		{
			// add if not found
			m_imageList.Add(hIcon);
			m_arrImage.Add(hIcon);
		}
	}

	// set item text to window text
	CString sWindowText=GetTextForTabItem(pChildWnd);

	TC_ITEM tci;
	tci.mask=TCIF_TEXT|TCIF_IMAGE;
	tci.pszText=(TCHAR*)(void*)(const TCHAR*)sWindowText;
	tci.iImage=nImageIndex;

	// insert new tab control item
	VERIFY(InsertItem(m_arrTab.GetSize(),&tci)!=-1);

	// redraw the tab control
	if(!m_arrTab.GetSize() && bRedraw)
		InvalidateRect(NULL);

	// save information about new entry
	TAB_ITEM_ENTRY newTabItemEntry;
	newTabItemEntry.sText=sWindowText;
	newTabItemEntry.pWnd=(CWnd*)pChildWnd;
	newTabItemEntry.bFound=TRUE;
	newTabItemEntry.sWndClass=sWndClass;

	m_arrTab.Add(newTabItemEntry);

	// update the size and position of the tab control and MDIClient window
	if(bRedraw)
	{
		ASSERT(::IsWindow(GetParentFrame()->GetSafeHwnd()));
		GetParentFrame()->RecalcLayout();
	}

	return TRUE;
}

// remove item from the tab control that corresponds to specified MDIChild
BOOL COXTabWorkspace::RemoveTabItem(const CWnd* pChildWnd, BOOL bRedraw/*=TRUE*/)
{
	ASSERT(pChildWnd!=NULL);
	ASSERT(::IsWindow(pChildWnd->GetSafeHwnd()));
	ASSERT((pChildWnd->GetExStyle()&WS_EX_MDICHILD)!=0);

	// find the item
	int nTabItem=FindTabItem(pChildWnd);

	if(nTabItem==-1)
		return FALSE;

	// delete item
	DeleteItem(nTabItem);
	// remove entry from the internal array of created items
	m_arrTab.RemoveAt(nTabItem);

	// update the size and position of the tab control and MDIClient window
	if(bRedraw)
	{
		if(::IsWindow(GetParentFrame()->GetSafeHwnd()))
			GetParentFrame()->RecalcLayout();
	}

	return TRUE;
}


CString COXTabWorkspace::GetTextForTabItem(const CWnd* pChildWnd) const
{
	ASSERT(pChildWnd!=NULL);
	ASSERT(pChildWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));

	CString sWindowText=(LPCTSTR)((CWnd*)pChildWnd)->
		SendMessage(OXWM_MTI_GETWINDOWTEXT);

	if(sWindowText.IsEmpty())
	{
		CDocument* pDoc=((CMDIChildWnd*)pChildWnd)->GetActiveDocument();
		if(pDoc!=NULL)
			sWindowText=pDoc->GetTitle();

		if(sWindowText.IsEmpty())
			pChildWnd->GetWindowText(sWindowText);
	}
	int frametp=((CWnd*)pChildWnd)->SendMessage(UM_GET_FRAME_TYPE);
	if(frametp==0)
		sWindowText.Empty();  // TMP!!!
	return sWindowText;

}


/////////////////////////////////////////////////////////////////////////////
// COXTabClientWnd

COXTabClientWnd::COXTabClientWnd()
{
	m_pParentFrame=NULL;
	m_bForceToRecalc=FALSE;
}

COXTabClientWnd::~COXTabClientWnd()
{
}


BEGIN_MESSAGE_MAP(COXTabClientWnd, CWnd)
	//{{AFX_MSG_MAP(COXTabClientWnd)
	ON_MESSAGE(WM_MDIACTIVATE,OnMDIActivate)
	ON_MESSAGE(WM_MDICREATE,OnMDICreate)
	ON_MESSAGE(WM_MDIDESTROY,OnMDIDestroy)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// subclasses MDIClient window of the specified MDIFrame window and 
// create COXTabWorkspace control to manage MDIChild window
BOOL COXTabClientWnd::Attach(const CMDIFrameWnd* pParentFrame, 
							 DWORD dwTabCtrlStyle/*=DEFAULT_TABCTRLSTYLE*/)
{
	ASSERT(pParentFrame!=NULL);
	ASSERT(::IsWindow(pParentFrame->GetSafeHwnd()));

	// check if already attached
	if(IsAttached())
	{
		TRACE(_T("COXTabClientWnd::Attach: window has already been attached. Call Detach() function before!\n"));
		return FALSE;
	}

	// make sure the specified window is/derived from CMDIFrameWnd class
	if(!pParentFrame->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
	{
		TRACE(_T("COXTabClientWnd::Attach: specified frame window is not of CMDIFrameWnd class (or derived)!\n"));
		return FALSE;
	}

	// try to sublass MDIClient window
	if(!SubclassWindow(pParentFrame->m_hWndMDIClient))
	{
		TRACE(_T("COXTabClientWnd::Attach: failed to subclass MDI Client window\n"));
		return FALSE;
	}

	// save the pointer to parent MDIFrame
	m_pParentFrame=(CMDIFrameWnd*)pParentFrame;


	// create tab control
	//
	
	ASSERT(!::IsWindow(m_tab.GetSafeHwnd()));
	CRect rect(0,0,0,0);

	// make sure WS_POPUP style is not used
	ASSERT((dwTabCtrlStyle&WS_POPUP)==0);
	// make sure the following styles are used 
	dwTabCtrlStyle|=(TCS_FOCUSNEVER|WS_VISIBLE|WS_CHILD|TCS_OWNERDRAWFIXED);
	m_tab.m_pTabClientWnd=this;
	// try to create the tab control
    if(!m_tab.Create(dwTabCtrlStyle,rect,m_pParentFrame,IDC_TABWORKSPACE))
	{
		TRACE(_T("COXTabClientWnd::Attach: failed to create tab control\n"));
		// if failed we detach the parent frame
		Detach();
		return FALSE;
	}

	// update the size and position of the tab control and MDIClient window
	m_pParentFrame->RecalcLayout();

	// populate tab control with MDIChild windows if any exist at the moment
	m_tab.UpdateContents(TRUE);

	return TRUE;
}


// unsubclasses MDIClient window and destroy the COXTabWorkspace control 
// that was used to manage MDIChild window
BOOL COXTabClientWnd::Detach()
{
	// check if any attached
	if(!IsAttached())
	{
		TRACE(_T("COXTabClientWnd::Attach: there is nothing to detach! Window hasn't been attached!\n"));
		return FALSE;
	}

	// destroy tab control
	if(::IsWindow(m_tab.GetSafeHwnd()))
		m_tab.DestroyWindow();

	m_tab.m_pTabClientWnd=NULL;

	// unsubclass MDIClient window
	UnsubclassWindow();

	// update the size and position of the MDIClient window
	if(::IsWindow(m_pParentFrame->GetSafeHwnd()))
		m_pParentFrame->RecalcLayout();

	m_pParentFrame=NULL;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// COXTabClientWnd message handlers


// crucial function that calculate the size of MDIClient window. Called
// by parent MDIFrame window
void COXTabClientWnd::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	// TODO: Add your specialized code here and/or call the base class

    // now do the laying out
    HDWP dwh=BeginDeferWindowPos(2);

    // move tab window
    if(::IsWindow(m_tab.m_hWnd) && (m_tab.GetStyle()&WS_VISIBLE)==WS_VISIBLE)
    {
		// get the size of MDIClient the way it fits into the client area of
		// the tab control
		DWORD dwTabStyle=m_tab.GetStyle();
		if((dwTabStyle&TCS_BUTTONS)==TCS_BUTTONS && 
			(dwTabStyle&TCS_VERTICAL)==TCS_VERTICAL)
		{
			CRect rectTab=lpClientRect;

			CRect rect(lpClientRect->left,lpClientRect->top,
				lpClientRect->left+lpClientRect->bottom-lpClientRect->top,
				lpClientRect->top+lpClientRect->right-lpClientRect->left);

			// move tab control
			::SetWindowPos(m_tab.m_hWnd,NULL,
				rect.left,rect.top,rect.Width(),
				rect.Height(),SWP_NOZORDER|SWP_NOREDRAW);

			rect.DeflateRect(m_tab.GetOffset(),m_tab.GetOffset());
			CRect rectCopy=rect;

			// adjust the size of tab control
			lpClientRect->left+=m_tab.GetOffset();       
			lpClientRect->top+=m_tab.GetOffset();       
			lpClientRect->right-=m_tab.GetOffset();       
			lpClientRect->bottom-=m_tab.GetOffset();       

			if((dwTabStyle&TCS_RIGHT)==TCS_RIGHT)
			{
				m_tab.ModifyStyle(TCS_RIGHT|TCS_VERTICAL,0,SWP_NOREDRAW);
				m_tab.AdjustRect(FALSE,rect);
				m_tab.ModifyStyle(0,TCS_RIGHT|TCS_VERTICAL,SWP_NOREDRAW);
			}
			else
			{
				m_tab.ModifyStyle(TCS_VERTICAL,0,SWP_NOREDRAW);
				m_tab.AdjustRect(FALSE,rect);
				m_tab.ModifyStyle(0,TCS_VERTICAL,SWP_NOREDRAW);
				lpClientRect->left+=rect.top-rectCopy.top;
			}

			lpClientRect->top+=rect.left-rectCopy.left;
			lpClientRect->bottom=lpClientRect->top+rect.Width();
			lpClientRect->right=lpClientRect->left+rect.Height();

			// move tab control
			dwh=::DeferWindowPos(dwh,m_tab.m_hWnd,NULL,rectTab.left,
				rectTab.top,rectTab.Width(),
				rectTab.Height(),SWP_NOZORDER);
		}
		else
		{
			CRect rectTab=lpClientRect;

			// move tab control
			dwh=::DeferWindowPos(dwh,m_tab.m_hWnd,NULL,lpClientRect->left,
				lpClientRect->top,
				lpClientRect->right-lpClientRect->left,
				lpClientRect->bottom-lpClientRect->top, SWP_NOZORDER);

			// adjust the size of tab control
			if(lpClientRect->bottom-lpClientRect->top>2*(int)m_tab.GetOffset())
			{
				lpClientRect->top+=m_tab.GetOffset();
				lpClientRect->bottom-=m_tab.GetOffset();
			}
			else
			{
				lpClientRect->top=lpClientRect->bottom;
			}

			if(lpClientRect->right-lpClientRect->left>2*(int)m_tab.GetOffset())
			{
				lpClientRect->left+=m_tab.GetOffset();
				lpClientRect->right-=m_tab.GetOffset();
			}
			else
			{
				lpClientRect->left=lpClientRect->right;
			}

			if((dwTabStyle&TCS_BUTTONS)==TCS_BUTTONS && 
				(dwTabStyle&TCS_VERTICAL)!=TCS_VERTICAL && 
					(dwTabStyle&TCS_BOTTOM)==TCS_BOTTOM)
			{
				int nTop=lpClientRect->top;

				m_tab.ModifyStyle(TCS_BOTTOM,0,SWP_NOREDRAW);
				m_tab.AdjustRect(FALSE,lpClientRect);
				m_tab.ModifyStyle(0,TCS_BOTTOM,SWP_NOREDRAW);

				lpClientRect->bottom-=lpClientRect->top-nTop;
				lpClientRect->top-=lpClientRect->top-nTop;

			}
			else
			{
				m_tab.AdjustRect(FALSE,lpClientRect);
			}
		}

		if(lpClientRect->bottom<lpClientRect->top || 
			lpClientRect->right<lpClientRect->left)
			::memset(lpClientRect,0,sizeof(RECT));

		::ShowWindow(m_tab.m_hWnd,SW_SHOWNA);
    }

	// move MDIClient window
    dwh=::DeferWindowPos(dwh,m_hWnd,NULL,lpClientRect->left,lpClientRect->top,
		lpClientRect->right-lpClientRect->left,
		lpClientRect->bottom-lpClientRect->top,
		SWP_NOZORDER);

    EndDeferWindowPos(dwh);

	CWnd::CalcWindowRect(lpClientRect, nAdjustType);
}

// handler for WM_MDIACTIVATE message. Will select corresponding 
// tab control item
LONG COXTabClientWnd::OnMDIActivate(UINT wParam, LONG lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	LONG lResult=Default();
	
	HWND hActiveWnd=(HWND)wParam;
	// find corresponding tab control item 
	int nTabItem=m_tab.FindTabItem(hActiveWnd);
	if(nTabItem!=-1)
	{
		ASSERT(nTabItem>=0 && nTabItem<m_tab.GetItemCount());
		m_tab.SetCurSel(nTabItem);
	}

	// update the contents of the tab control afterwards
	m_tab.PostMessage(WM_TIMER,IDT_MDI_STATUS_TIMER);

	return lResult;
}


// handler for WM_MDICREATE message. Will add new item into the 
// tab control
LONG COXTabClientWnd::OnMDICreate(UINT wParam, LONG lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	LONG lResult=Default();

	// lResult is handle to the newd MDIChild window if it was
	// successfully created
	if(lResult!=NULL)
	{
		// there shouldn't be such item in the tab control
		ASSERT(m_tab.FindTabItem((HWND)lResult)==-1);

		// update the tab control contents
		m_tab.SetCurSel(-1);
	    m_tab.PostMessage(WM_TIMER,IDT_MDI_STATUS_TIMER);
	}

	return lResult;
}


// handler for WM_MDIDESTROY message. Will remove the correspondent item 
// from the tab control
LONG COXTabClientWnd::OnMDIDestroy(UINT wParam, LONG lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	// remove the item from the tab control
	CWnd* pChildWnd=CWnd::FromHandle((HWND)wParam);
	m_tab.RemoveTabItem(pChildWnd);

	LONG lResult=Default();

	// update the contents of the tab control afterwards
	m_tab.PostMessage(WM_TIMER,IDT_MDI_STATUS_TIMER);

	return lResult;
}

// update the size of the tab control and the MDIClient window
void COXTabClientWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
	// update the contents of the tab control afterwards
	m_bForceToRecalc=TRUE;
	m_tab.PostMessage(WM_TIMER,IDT_MDI_STATUS_TIMER);
}


// show/hide tab control if MDIClient is shown/hidden
void COXTabClientWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow,nStatus);
	
	// TODO: Add your message handler code here

	if(nStatus==0 && ::IsWindow(m_tab.GetSafeHwnd()))
	{
		if(bShow)
			m_tab.ModifyStyle(NULL,WS_VISIBLE);
		else
			m_tab.ModifyStyle(WS_VISIBLE,NULL);
	}
}
