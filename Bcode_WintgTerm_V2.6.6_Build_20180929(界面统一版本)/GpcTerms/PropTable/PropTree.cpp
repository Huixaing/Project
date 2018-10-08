// PropTree.cpp : implementation file
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.

#include "stdafx.h"
#include "Resource.h"
#include "PropTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PROPTREEITEM_EXPANDCOLUMN		16			// width of the expand column
#define PROPTREEITEM_COLRNG				5			// width of splitter

static AFX_EXTENSION_MODULE PropTreeDLL = {NULL, NULL};
static const CString strOfficeFontName	= _T("Tahoma");
static const CString strDefaultFontName = _T("MS Sans Serif");

HINSTANCE ghInst;

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("PROPTREE.DLL Initializing!\n");
		
		if (!AfxInitExtensionModule(PropTreeDLL, hInstance))
			return 0;

		new CDynLinkLibrary(PropTreeDLL);

		ghInst = hInstance;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("PROPTREE.DLL Terminating!\n");
		AfxTermExtensionModule(PropTreeDLL);
	}

	return 1;
}

static int CALLBACK FontFamilyProcFonts(const LOGFONT FAR* lplf, const TEXTMETRIC FAR*, ULONG, LPARAM)
{
	ASSERT(lplf != NULL);
	CString strFont = lplf->lfFaceName;
	return strFont.CollateNoCase (strOfficeFontName) == 0 ? 0 : 1;
}

/////////////////////////////////////////////////////////////////////////////
// CPropTree

UINT CPropTree::s_nInstanceCount;
CFont* CPropTree::s_pNormalFont;
CFont* CPropTree::s_pBoldFont;
CPropTreeItem* CPropTree::s_pFound;

CPropTree::CPropTree() :
	m_bShowInfo(TRUE),
	m_nInfoHeight(50),
	m_pVisbleList(NULL),
	m_Origin(80,0),
	m_nLastUID(1),
	m_pFocus(NULL),
	m_bDisableInput(FALSE),
	m_itemNameHeight(24),
	m_toolbarHeight(24)

{
	m_Root.Expand();

	// init global resources only once
	if (!s_nInstanceCount)
		InitGlobalResources();
	s_nInstanceCount++;
}


CPropTree::~CPropTree()
{
	DeleteAllItems();

	s_nInstanceCount--;

	// free global resource when ALL CPropTrees are destroyed
	if (!s_nInstanceCount)
		FreeGlobalResources();
}


BEGIN_MESSAGE_MAP(CPropTree, CWnd)
	//{{AFX_MSG_MAP(CPropTree)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ENABLE()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropTree message handlers

const POINT& CPropTree::GetOrigin()
{
	return m_Origin;
}


BOOL CPropTree::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CWnd* pWnd = this;

	LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW));

	return pWnd->Create(pszCreateClass, _T(""), dwStyle, rect, pParentWnd, nID);
}


int CPropTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwStyle;
	CRect rc;

	GetClientRect(rc);

	//add skybird
	m_Origin.x = rc.Width() / 2 - 20;
	if(m_Origin.x<80) // && rc.Width()>80)
		m_Origin.x=80;
	// create CPropTreeList
	//

	dwStyle = WS_VISIBLE | WS_CHILD | WS_VSCROLL;

	if (!m_List.Create(dwStyle, rc, this, 100))
	{
		TRACE0("Failed to create CPropTreeList\n");
		return -1;
	}

	m_List.SetPropOwner(this);

	// create CPropTreeInfo
	//

	dwStyle &= ~WS_VSCROLL;

	//if (!m_Info.Create(_T(""), dwStyle, rc, this))
	if (!m_Info.Create(WS_BORDER|WS_CHILD|WS_VSCROLL|WS_VISIBLE|ES_WANTRETURN|ES_READONLY|ES_AUTOVSCROLL|ES_MULTILINE,
		rc,this,101))
	{
		TRACE0("Failed to create CPropTreeInfo\n");
		return -1;
	}
	m_Info.SetFont(s_pNormalFont);

	//m_Info.SetPropOwner(this);

	//add skybird cao, 2004.06.08
//	LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW), 
//					GetSysColorBrush(COLOR_BTNFACE));

	/*
	if (! m_toolbarContainer.CreateContainer(rc, this, this, IDR_PROP_TOOLBAR))
	{
		TRACE0("Failed to create toolbar container\n");
		return -1;
	}

	

	dwStyle = CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_BORDER;

	if (! m_comboBoxItemName.Create(dwStyle, CRect(0, 0, 0, 54), this, 431))
	{
		TRACE0("Failed to create combobox for item name\n");
		return -1;
	}

	m_comboBoxItemName.SetFont(GetBoldFont());
	CRect rect;
	m_comboBoxItemName.GetWindowRect(&rect);
	*/
	//m_itemNameHeight = rect.Height();
	m_itemNameHeight=3;
	m_toolbarHeight=0;
	return 0;
}


CWnd* CPropTree::GetCtrlParent()
{
	return &m_List;
}


void CPropTree::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	ResizeChildWindows(cx, cy);
}


void CPropTree::ResizeChildWindows(int cx, int cy)
{
	if (m_bShowInfo)
	{
		m_nInfoHeight=cy/4;
			//add skybird, 2004.06.08
		ASSERT(IsWindow(m_List.m_hWnd) && IsWindow(m_Info.m_hWnd)); //m_comboBoxItemName.m_hWnd) &&IsWindow(m_toolbarContainer.m_hWnd) && 
// CUIHU: 暂时不要这些杂物
		// m_comboBoxItemName.MoveWindow(0, 0, cx, m_itemNameHeight);
		
		// m_toolbarContainer.MoveWindow(0, m_itemNameHeight - 2, cx, m_toolbarHeight + 2);
	
		m_List.MoveWindow(0, m_itemNameHeight + m_toolbarHeight, cx, 
				cy - m_nInfoHeight - m_itemNameHeight - m_toolbarHeight);

		
	
		m_Info.MoveWindow(0, cy - m_nInfoHeight, cx, m_nInfoHeight);
	}
	else
	{
		if (IsWindow(m_List.m_hWnd))
			m_List.MoveWindow(0, 0, cx, cy);
	}
}


void CPropTree::InitGlobalResources()
{
	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);

	LOGFONT lf;
	memset(&lf, 0, sizeof (LOGFONT));

	CWindowDC dc(NULL);
	lf.lfCharSet = (BYTE)GetTextCharsetInfo(dc.GetSafeHdc(), NULL, 0);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	// check if we should use system font
	_tcscpy(lf.lfFaceName, info.lfMenuFont.lfFaceName);

	BOOL fUseSystemFont = (info.lfMenuFont.lfCharSet > SYMBOL_CHARSET);
	if (!fUseSystemFont)
	{
		// check for "Tahoma" font existance:
		if (::EnumFontFamilies(dc.GetSafeHdc(), NULL, FontFamilyProcFonts, 0)==0)
		{
			// Found! Use MS Office font!
			_tcscpy(lf.lfFaceName, strOfficeFontName);
		}
		else
		{
			// Not found. Use default font:
			_tcscpy(lf.lfFaceName, strDefaultFontName);
		}
	}

	s_pNormalFont = new CFont;
	s_pNormalFont->CreateFontIndirect(&lf);

	lf.lfWeight = FW_BOLD;
	s_pBoldFont = new CFont;
	s_pBoldFont->CreateFontIndirect(&lf);
}


void CPropTree::FreeGlobalResources()
{
	if (s_pNormalFont)
	{
		delete s_pNormalFont;
		s_pNormalFont = NULL;
	}

	if (s_pBoldFont)
	{
		delete s_pBoldFont;
		s_pBoldFont = NULL;
	}
}


CFont* CPropTree::GetNormalFont()
{
	return s_pNormalFont;
}


CFont* CPropTree::GetBoldFont()
{
	return s_pBoldFont;
}


CPropTreeItem* CPropTree::GetFocusedItem()
{
	return m_pFocus;
}


CPropTreeItem* CPropTree::GetRootItem()
{
	return &m_Root;
}


void CPropTree::ClearVisibleList()
{
	m_pVisbleList = NULL;
}


CPropTreeItem* CPropTree::GetVisibleList()
{
	return m_pVisbleList;
}


void CPropTree::AddToVisibleList(CPropTreeItem* pItem)
{
	if (!pItem)
		return;

	// check for an empty visible list
	if (!m_pVisbleList)
		m_pVisbleList = pItem;
	else
	{
		// Add the new item to the end of the list
		CPropTreeItem* pNext;

		pNext = m_pVisbleList;
		while (pNext->GetNextVisible())
			pNext = pNext->GetNextVisible();

		pNext->SetNextVisible(pItem);
	}

	pItem->SetNextVisible(NULL);
}


BOOL CPropTree::EnumItems(CPropTreeItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam)
{
	if (!pItem || !proc)
		return FALSE;

	CPropTreeItem* pNext;

	// don't count the root item in any enumerations
	if (pItem!=&m_Root && !proc(this, pItem, lParam))
		return FALSE;

	// recurse thru all child items
	pNext = pItem->GetChild();

	while (pNext)
	{
		if (!EnumItems(pNext, proc, lParam))
			return FALSE;

		pNext = pNext->GetSibling();
	}

	return TRUE;
}


void CPropTree::SetOriginOffset(LONG nOffset)
{
	m_Origin.y = nOffset;
}

	
void CPropTree::UpdatedItems()
{
	if (!IsWindow(m_hWnd))
		return;

	Invalidate();

	m_List.UpdateResize();
	m_List.Invalidate();
}


void CPropTree::DeleteAllItems()
{
	//add skybird, 2004.06.25
	m_propArray.RemoveAll();

	Delete(NULL);
	ClearVisibleList();

	UpdatedItems();
	m_nLastUID = 1; // reset uid counter
}


void CPropTree::DeleteItem(CPropTreeItem* pItem)
{
	Delete(pItem);
	UpdatedItems(); 
}


LONG CPropTree::GetColumn()
{
	return m_Origin.x;
}


void CPropTree::SetColumn(LONG nColumn)
{
	

//modified by skybird 2004.06.10

	CRect rc;
	GetClientRect(rc);
	
#if 0	
	if (rc.IsRectEmpty())
	{
		nColumn = __max(PROPTREEITEM_EXPANDCOLUMN, nColumn);
	}
	else
	{
		//nColumn = __min(__max(PROPTREEITEM_EXPANDCOLUMN, nColumn), rc.Width() - PROPTREEITEM_EXPANDCOLUMN);
		nColumn = __max(__max(PROPTREEITEM_EXPANDCOLUMN, nColumn), (rc.Width() - PROPTREEITEM_EXPANDCOLUMN) / 2);
	}
#endif

	if (PROPTREEITEM_EXPANDCOLUMN * 2 > nColumn)
	{
		nColumn = PROPTREEITEM_EXPANDCOLUMN * 2;
	}
	else if ((rc.Width() - PROPTREEITEM_EXPANDCOLUMN ) < nColumn)
	{
		nColumn = 	rc.Width() - PROPTREEITEM_EXPANDCOLUMN;
	}

	m_Origin.x = nColumn;
	if(m_Origin.x<100)
		m_Origin.x=100;

	Invalidate();
}


void CPropTree::Delete(CPropTreeItem* pItem)
{
	if (pItem && pItem!=&m_Root && SendNotify(PTN_DELETEITEM, pItem))
		return;

	// passing in a NULL item is the same as calling DeleteAllItems
	if (!pItem)
		pItem = &m_Root;

	// delete children

	CPropTreeItem* pIter;
	CPropTreeItem* pNext;

	pIter = pItem->GetChild();
	while (pIter)
	{
		pNext = pIter->GetSibling();
		DeleteItem(pIter);
		pIter = pNext;
	}

	// unlink from tree
	if (pItem->GetParent())
	{
		if (pItem->GetParent()->GetChild()==pItem)
			pItem->GetParent()->SetChild(pItem->GetSibling());
		else
		{
			pIter = pItem->GetParent()->GetChild();
			while (pIter->GetSibling() && pIter->GetSibling()!=pItem)
				pIter = pIter->GetSibling();

			if (pIter->GetSibling())
				pIter->SetSibling(pItem->GetSibling());
		}
	}

	if (pItem!=&m_Root)
	{
		if (pItem==GetFocusedItem())
			SetFocusedItem(NULL);
		delete pItem;
	}
}


void CPropTree::SetFocusedItem(CPropTreeItem* pItem)
{
	m_pFocus = pItem;
	EnsureVisible(m_pFocus);

	if (!IsWindow(m_hWnd))
		return;

	Invalidate();
}


void CPropTree::ShowInfoText(BOOL bShow)
{
	m_bShowInfo = bShow;

	CRect rc;
	if(IsWindow(m_Info.m_hWnd))
	{
		m_Info.ShowWindow(bShow?SW_SHOW:SW_HIDE);
	}
		


	GetClientRect(rc);
	ResizeChildWindows(rc.Width(), rc.Height());

}


BOOL CPropTree::IsItemVisible(CPropTreeItem* pItem)
{
	if (!pItem)
		return FALSE;

	for (CPropTreeItem* pNext = m_pVisbleList; pNext; pNext = pNext->GetNextVisible())
	{
		if (pNext==pItem)
			return TRUE;
	}

	return FALSE;
}


void CPropTree::EnsureVisible(CPropTreeItem* pItem)
{
	if (!pItem)
		return;

	// item is not scroll visible (expand all parents)
	if (!IsItemVisible(pItem))
	{
		CPropTreeItem* pParent;

		pParent = pItem->GetParent();
		while (pParent)
		{
			pParent->Expand();
			pParent = pParent->GetParent();
		}

		UpdatedItems();
		UpdateWindow();
	}

	ASSERT(IsItemVisible(pItem));

	CRect rc;

	m_List.GetClientRect(rc);
	rc.OffsetRect(0, m_Origin.y);
	rc.bottom -= pItem->GetHeight();

	CPoint pt;

	pt = pItem->GetLocation();

	if (!rc.PtInRect(pt))
	{
		LONG oy;

		if (pt.y < rc.top)
			oy = pt.y;
		else
			oy = pt.y - rc.Height() + pItem->GetHeight();

		m_List.OnVScroll(SB_THUMBTRACK, oy, NULL);
	}
}


CPropTreeItem* CPropTree::InsertItem(CPropTreeItem* pItem, CPropTreeItem* pParent)
{
	//modified by skybird, 2004.06.25
	if (pItem == NULL)
	{
		return NULL;
	}

	if (pParent == NULL)
	{
		pParent = &m_Root;
	}
	else //save element, not root
	{
		m_propArray.Add(pItem);
	}

	if (!pParent->GetChild())
		pParent->SetChild(pItem);
	else
	{
		// add to end of the sibling list
		CPropTreeItem* pNext;

		pNext = pParent->GetChild();
		while (pNext->GetSibling())
			pNext = pNext->GetSibling();

		pNext->SetSibling(pItem);
	}

	pItem->SetParent(pParent);
	pItem->SetPropOwner(this);

	// auto generate a default ID
	pItem->SetCtrlID(m_nLastUID++);

	SendNotify(PTN_INSERTITEM, pItem);

	UpdatedItems();

	return pItem;
}



LONG CPropTree::HitTest(const POINT& pt)
{
	POINT p = pt;

	CPropTreeItem* pItem;

	// convert screen to tree coordinates
	p.y += m_Origin.y;

	if ((pItem = FindItem(pt))!=NULL)
	{
		if (!pItem->IsRootLevel() && pt.x >= m_Origin.x - PROPTREEITEM_COLRNG && pt.x <= m_Origin.x + PROPTREEITEM_COLRNG)
			return HTCOLUMN;

		if (pt.x > m_Origin.x + PROPTREEITEM_COLRNG)
			return HTATTRIBUTE;

		if (pItem->HitExpand(p))
			return HTEXPAND;

		if (pItem->HitCheckBox(p))
			return HTCHECKBOX;

		return HTLABEL;
	}

	return HTCLIENT;
}


CPropTreeItem* CPropTree::FindItem(const POINT& pt)
{
	CPropTreeItem* pItem;

	CPoint p = pt;

	// convert screen to tree coordinates
	p.y += m_Origin.y;

	// search the visible list for the item
	for (pItem = m_pVisbleList; pItem; pItem = pItem->GetNextVisible())
	{
		CPoint ipt = pItem->GetLocation();
		if (p.y>=ipt.y && p.y<ipt.y + pItem->GetHeight())
			return pItem;
	}

	return NULL;
}


CPropTreeItem* CPropTree::FindItem(UINT nCtrlID)
{
	s_pFound = NULL;

	EnumItems(&m_Root, EnumFindItem, nCtrlID);

	return s_pFound;
}


BOOL CALLBACK CPropTree::EnumFindItem(CPropTree*, CPropTreeItem* pItem, LPARAM lParam)
{
	ASSERT(pItem!=NULL);

	if (pItem->GetCtrlID()==(UINT)lParam)
	{
		s_pFound = pItem;
		return FALSE;
	}

	return TRUE;
}


BOOL CPropTree::IsDisableInput()
{
	return m_bDisableInput;
}


void CPropTree::DisableInput(BOOL bDisable)
{
	m_bDisableInput = bDisable;

	CWnd* pWnd;

	if ((pWnd = GetParent())!=NULL)
		pWnd->EnableWindow(!bDisable);
}


void CPropTree::SelectItems(CPropTreeItem* pItem, BOOL bSelect)
{
	if (!pItem)
		pItem = &m_Root;

	EnumItems(pItem, EnumSelectAll, (LPARAM)bSelect);
}


CPropTreeItem* CPropTree::FocusFirst()
{
	CPropTreeItem *pold;

	pold = m_pFocus;

	SetFocusedItem(m_pVisbleList);

	if (m_pFocus)
	{
		SelectItems(NULL, FALSE);
		m_pFocus->Select();
	}

	if (pold!=m_pFocus)
		SendNotify(PTN_SELCHANGE, m_pFocus);

	return m_pFocus;
}


CPropTreeItem* CPropTree::FocusLast()
{
	CPropTreeItem* pNext;
	CPropTreeItem* pChange;

	pChange = m_pFocus;

	pNext = m_pVisbleList;

	if (pNext)
	{
		while (pNext->GetNextVisible())
			pNext = pNext->GetNextVisible();

		SetFocusedItem(pNext);

		if (m_pFocus)
		{
			SelectItems(NULL, FALSE);
			m_pFocus->Select();
		}
	}

	if (pChange!=m_pFocus)
		SendNotify(PTN_SELCHANGE, m_pFocus);

	return pNext;
}


CPropTreeItem* CPropTree::FocusPrev()
{
	CPropTreeItem* pNext;
	CPropTreeItem* pChange;

	pChange = m_pFocus;

	if (m_pFocus==NULL)
	{
		// get the last visible item
		pNext = m_pVisbleList;
		while (pNext && pNext->GetNextVisible())
			pNext = pNext->GetNextVisible();
	}
	else
	{
		pNext = m_pVisbleList;
		while (pNext && pNext->GetNextVisible()!=m_pFocus)
			pNext = pNext->GetNextVisible();
	}

	if (pNext)
		SetFocusedItem(pNext);
	
	if (m_pFocus)
	{
		SelectItems(NULL, FALSE);
		m_pFocus->Select();
	}

	if (pChange!=m_pFocus)
		SendNotify(PTN_SELCHANGE, m_pFocus);

	return pNext;
}


CPropTreeItem* CPropTree::FocusNext()
{
	CPropTreeItem* pNext;
	CPropTreeItem* pChange;

	pChange = m_pFocus;

	if (m_pFocus==NULL)
		pNext = m_pVisbleList;
	else
	if (m_pFocus->GetNextVisible())
		pNext = m_pFocus->GetNextVisible();
	else
		pNext = NULL;

	if (pNext)
		SetFocusedItem(pNext);

	if (m_pFocus)
	{
		SelectItems(NULL, FALSE);
		m_pFocus->Select();
	}

	if (pChange!=m_pFocus)
		SendNotify(PTN_SELCHANGE, m_pFocus);

	return pNext;
}


void CPropTree::UpdateMoveAllItems()
{
	EnumItems(&m_Root, EnumMoveAll);
}


void CPropTree::RefreshItems(CPropTreeItem* pItem)
{
	if (!pItem)
		pItem = &m_Root;

	EnumItems(pItem, EnumRefreshAll);

	UpdatedItems();
}


BOOL CALLBACK CPropTree::EnumSelectAll(CPropTree*, CPropTreeItem* pItem, LPARAM lParam)
{
	if (!pItem)
		return FALSE;

	pItem->Select((BOOL)lParam);

	return TRUE;
}


BOOL CALLBACK CPropTree::EnumRefreshAll(CPropTree*, CPropTreeItem* pItem, LPARAM)
{
	if (!pItem)
		return FALSE;

	pItem->OnRefresh();

	return TRUE;
}


BOOL CALLBACK CPropTree::EnumMoveAll(CPropTree*, CPropTreeItem* pItem, LPARAM)
{
	if (!pItem)
		return FALSE;

	pItem->OnMove();

	return TRUE;
}


LRESULT CPropTree::SendNotify(UINT nNotifyCode, CPropTreeItem* pItem)
{
	if (!IsWindow(m_hWnd))
		return 0L;

	if (!(GetStyle() & PTS_NOTIFY))
		return 0L;

	NMPROPTREE nmmp;
	LPNMHDR lpnm;

	lpnm = NULL;

	switch (nNotifyCode)
	{
		case PTN_INSERTITEM:
		case PTN_DELETEITEM:
		case PTN_DELETEALLITEMS:
		case PTN_ITEMCHANGED:
		case PTN_ITEMBUTTONCLICK:
		case PTN_SELCHANGE:
		case PTN_ITEMEXPANDING:
		case PTN_COLUMNCLICK:
		case PTN_PROPCLICK:
		case PTN_CHECKCLICK:
			lpnm = (LPNMHDR)&nmmp;
			nmmp.pItem = pItem;
			break;
	}

	if (lpnm)
	{
		UINT id = (UINT)::GetMenu(m_hWnd);
		lpnm->code = nNotifyCode;
		lpnm->hwndFrom = m_hWnd;
		lpnm->idFrom = id;
	
		return GetParent()->SendMessage(WM_NOTIFY, (WPARAM)id, (LPARAM)lpnm);
	}

	return 0L;
}


void CPropTree::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	Invalidate();
}


void CPropTree::OnSysColorChange() 
{
	CWnd::OnSysColorChange();
	
	Invalidate();	
}


BOOL CPropTree::IsSingleSelection()
{
	// right now only support single selection
	return TRUE;
}

 //used to sort by category
void CPropTree::SetGroupInfo(const CStringArray & groupInfo)
{
	m_itemGroupInfo.RemoveAll();

	for (int i = 0; i < (int) groupInfo.GetCount(); i++)
	{
		m_itemGroupInfo.Add(groupInfo[i]);
	}
}

void  CPropTree::SortByCategory()
{
	//TRACE("property sort by category\n");
	
	CPropTreeItem *pTreeItem = m_Root.GetChild();
	if (pTreeItem == NULL)
	{
		return;
	}

	m_Root.SetChild(NULL);
	DeleteAllItems();

	CTreeItemArray  itemArray;
	GetAndResetAllItems(pTreeItem, itemArray);
	
	for (int i = 0; i < (int) m_itemGroupInfo.GetCount(); i++)
	{
		CString str(m_itemGroupInfo[i]);
		CString token;
		int curPos= 0;

		token= str.Tokenize("%",curPos);
		token.Trim();
		if (token.IsEmpty())
		{
			continue;
		}

		CPropTreeItem *pRoot = InsertItem(new CPropTreeItem());
		pRoot->SetLabelText(token);
		pRoot->SetInfoText(token);
		pRoot->Expand(); // have this item expanded by default

		while (token != "")
		{
			token = str.Tokenize("%",curPos);
			token.Trim();
			
			if (token.IsEmpty())
			{
				break;
			}
			
			int i = FindItemByString(itemArray, token);
			if (i >= 0)
			{
				InsertItem(itemArray[i], pRoot);
				itemArray.RemoveAt(i);
			}
		}
	}

	ASSERT(itemArray.GetCount() == 0);
}

void  CPropTree::SortByAlpha()
{
	//TRACE("property sort by alpha\n");

	CPropTreeItem *pTreeItem = m_Root.GetChild();
	if (pTreeItem == NULL)
	{
		return;
	}
	
	if ((m_pFocus != NULL) && (m_pFocus->GetChild() != NULL))
	{
		m_pFocus = NULL;
	}

	m_Root.SetChild(NULL);
	DeleteAllItems();

	CTreeItemArray  itemArray;

	GetAndResetAllItems(pTreeItem, itemArray);
	SortItemByAlpha(itemArray);
	
	for (int i = 0; i < (int) itemArray.GetCount(); i++)
	{
		InsertItem(itemArray[i]);
	}

}

CPropTreeItem * CPropTree::GetItemByTextLabel(const CString &textLabel)
{
	CPropTreeItem *pTreeItem = m_Root.GetChild();
	if (pTreeItem == NULL)
	{
		return NULL;
	}

	for (int i = 0; i < (int) m_propArray.GetCount(); i++)
	{
		pTreeItem = m_propArray[i];
		if(textLabel.Compare(pTreeItem->GetLabelText()) == 0)
		{
			return pTreeItem;
		}
	}

	return NULL;
}

void CPropTree::SetPropertyValues(const CStringArray & valueArray, const CStringArray &itemName)
{
	CPropTreeItem *pTreeItem = m_Root.GetChild();
	if (pTreeItem == NULL)
	{
		return ;
	}
	
	for (int i = 0; i < (int) m_propArray.GetCount(); i++)
	{
		pTreeItem = m_propArray[i];

		for (int j = 0; j < (int)itemName.GetCount(); j++)
		{
			CString textLabel = itemName.GetAt(j);
			if(textLabel.Compare(pTreeItem->GetLabelText()) == 0)
			{
				pTreeItem->SetItemValue((LPARAM)(LPCSTR)valueArray.GetAt(j));
				break;
			}
		}
	}
}

void CPropTree::GetPropertyValues(CStringArray & valueArray, const CStringArray &itemName)
{
	CPropTreeItem *pTreeItem = m_Root.GetChild();
	if (pTreeItem == NULL)
	{
		return;
	}

	for (int i = 0; i < (int) m_propArray.GetCount(); i++)
	{
		pTreeItem = m_propArray[i];

		for (int j = 0; j < (int)itemName.GetCount(); j++)
		{
			CString textLabel = itemName.GetAt(j);
			if(textLabel.Compare(pTreeItem->GetLabelText()) == 0)
			{
				valueArray.SetAt(j, (LPCSTR) pTreeItem->GetItemValue());
				break;
			}
		}
	}
}

void CPropTree::SetComboTitle(CString &title)
{
	/*
	m_comboBoxItemName.DeleteString(0);
	m_comboBoxItemName.AddString(title);
	m_comboBoxItemName.SetCurSel(0);*/
}

void CPropTree::SetAllItemsReadOnly(BOOL va) // add by cuihu
{
	CPropTreeItem *pTreeItem = m_Root.GetChild();
	if (pTreeItem == NULL)
	{
		return ;
	}
	
	for (int i = 0; i < (int) m_propArray.GetCount(); i++)
	{
		pTreeItem = m_propArray[i];
		pTreeItem->ReadOnly(va);
	}
}

void CPropTree::SetPropItemReadOnly(const CStringArray &itemName, const BOOL bReadOnly[])
{
	CPropTreeItem *pTreeItem = m_Root.GetChild();
	if (pTreeItem == NULL)
	{
		return ;
	}
	
	for (int i = 0; i < (int) m_propArray.GetCount(); i++)
	{
		pTreeItem = m_propArray[i];

		for (int j = 0; j < (int)itemName.GetCount(); j++)
		{
			CString textLabel = itemName.GetAt(j);
			if(textLabel.Compare(pTreeItem->GetLabelText()) == 0)
			{
				pTreeItem->ReadOnly(bReadOnly[j]);
			
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////
//internal interfaces, add skybird, 2004.06.10
void CPropTree::GetAndResetAllItems(CPropTreeItem *pItem, CTreeItemArray & itemArray)
{
	if (pItem == NULL)
	{
		return;
	}

	CPropTreeItem *pChild = pItem->GetChild();
	CPropTreeItem *pOldParent = NULL;
	CPropTreeItem *pSibling = pItem->GetSibling();

	if (pChild != NULL)
	{
		pOldParent = pItem;
		GetAndResetAllItems(pChild, itemArray);
	}
	else
	{
		pItem->SetSibling(NULL);
		itemArray.Add(pItem);
	}

	if (pOldParent != NULL)
	{
		pOldParent->SetChild(NULL);
		pOldParent->SetSibling(NULL);

		delete pOldParent;
	}

	GetAndResetAllItems(pSibling, itemArray);	
}


void CPropTree::SortItemByAlpha(CTreeItemArray & itemArray)
{
	int count = (int) itemArray.GetCount();

	for (int i = 0; i < count; i++)
	{
		int selected = i;
		for (int j = i + 1; j < count; j++)
		{
			CString str1 = itemArray[selected]->GetLabelText();
			CString str2 = itemArray[j]->GetLabelText();
			if (str1.Compare(str2) > 0)
			{
				selected = j;
			}
		}

		if (selected != i)
		{
			CPropTreeItem *pTmp = itemArray[selected];
			itemArray[selected] = itemArray[i];
			itemArray[i] = pTmp;
		}
	}
}

int CPropTree::FindItemByString(CTreeItemArray & itemArray, const CString & itemName)
{
	int count = (int) itemArray.GetCount();

	for (int i = 0; i < count; i++)
	{
		CString str1 = itemArray[i]->GetLabelText();
		str1.Trim();

		if (str1.Compare(itemName) == 0)
		{
			return i;
		}
	}

	return -1;
}
CString Change0ATo0A0D(const char *s)
{
	CString tmp=s;
	tmp.Replace("\n","\r\n");
	return tmp;
}
void  CPropTree::SetGeneralInfo(const char *p)
{
	if(m_Info.GetSafeHwnd())
	{
		if(p)
			m_Info.SetWindowText(Change0ATo0A0D(p)); 
		else
			m_Info.SetWindowText(""); 
	}
	m_szGeneralInfo=p;
}

const char * CPropTree::GetGeneralInfo()
{
	return (const char *)m_szGeneralInfo;
}