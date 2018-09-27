// ColorHeaderList.cpp : implementation file
//

#include "stdafx.h"
#include "ColorHeaderList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorHeaderList

CColorHeaderList::CColorHeaderList()
{
	m_bFullRowSel = FALSE;
	m_bClientWidthSel = TRUE;

	m_nUserSetColorNum = 0;
	m_nAllocMemoryNum = 0;
	m_nAllocColorNum = 0;

	for(int i=0;i < sizeof(m_clrUserSet) / sizeof(m_clrUserSet[0]);i++)
	{
		m_clrUserSet[i] = NULL;
	}
	m_cxClient = 0;
	m_cxStateImageOffset = 0;

	m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrTextBk = ::GetSysColor(COLOR_WINDOW);
	m_clrBkgnd = ::GetSysColor(COLOR_WINDOW);
}

CColorHeaderList::~CColorHeaderList()
{
	if(m_nAllocMemoryNum)
	{
		for(int i=0;i < m_nAllocMemoryNum;i++)
			delete [ ] m_nAllocMemory[i];
	}
}


BEGIN_MESSAGE_MAP(CColorHeaderList, CListCtrl)
	//{{AFX_MSG_MAP(CColorHeaderList)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColorHeaderList::SetColumnColor(int nColumnNum,struct _tagUserSetColor *clrinfo)
{
	do {
		int i;
		int nStructSize = sizeof(struct _tagUserSetColor);
		char *ptr;

		if(clrinfo == NULL)
			break;
		if(nColumnNum >= GetItemCount( ))
			break;
		if(nColumnNum >= sizeof(m_clrUserSet) / sizeof(m_clrUserSet[0]) )
			break;

		if(m_nAllocMemoryNum == 0)
		{
			m_nAllocMemory[0] = new char[4096];
			if(m_nAllocMemory[0] == NULL)
			{//alloc failed
				break;
			}
			m_nAllocMemoryNum += 1;

			ptr = m_nAllocMemory[0];
			for(i=0;i < 4096 / nStructSize;i++)
			{
				m_clrUserSet[i] = (struct _tagUserSetColor *)ptr;
				m_clrUserSet[i]->m_clrUserSetText = m_clrText;
				m_clrUserSet[i]->m_clrUserSetTextBk = m_clrTextBk;
				ptr += nStructSize;
				m_nAllocColorNum += 1;
			}
		}
		while(nColumnNum >= m_nAllocColorNum)
		{
			if(m_nAllocMemoryNum >= sizeof(m_nAllocMemory) / sizeof(m_nAllocMemory[0]))
				return;
			m_nAllocMemory[m_nAllocMemoryNum] = new char[4096];
			if(m_nAllocMemory[m_nAllocMemoryNum] == NULL)
			{//alloc failed
				break;
			}
			
			ptr = m_nAllocMemory[m_nAllocMemoryNum];
			for(i=0;i < 4096 / nStructSize;i++)
			{
				m_clrUserSet[m_nAllocColorNum] = (struct _tagUserSetColor *)ptr;
				m_clrUserSet[m_nAllocColorNum]->m_clrUserSetText = m_clrText;
				m_clrUserSet[m_nAllocColorNum]->m_clrUserSetTextBk = m_clrTextBk;
				ptr += nStructSize;
				m_nAllocColorNum += 1;
			}
			m_nAllocMemoryNum += 1;
		}
		*(m_clrUserSet[nColumnNum]) = *clrinfo;
	
		CRect rcItem, rcLabel;

// invalidate focused item so it can repaint properly

		GetItemRect(nColumnNum, rcItem, LVIR_BOUNDS);
		GetItemRect(nColumnNum, rcLabel, LVIR_LABEL);
		rcItem.left = rcLabel.left;
		InvalidateRect(rcItem, FALSE);
		UpdateWindow( );
	}while(0);
}
/////////////////////////////////////////////////////////////////////////////
// CColorHeaderList message handlers
void CColorHeaderList::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	DrawListControl(lpDrawItemStruct);
}


int CColorHeaderList::SetCurSel(int nFocusRecordNo)
{
	char szBuff[128];
	int theStatus = 1;
	do {
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT | LVIF_STATE;
		lvi.iItem = nFocusRecordNo;
		lvi.iSubItem = 0;
		lvi.pszText = szBuff;
		lvi.cchTextMax = sizeof(szBuff);
		lvi.stateMask = 0xFFFF;		// get all state flags
		if(!GetItem(&lvi))
			break;


		lvi.state |= LVIS_SELECTED;
		if(!SetItemState(nFocusRecordNo,&lvi))
			break;
		if(!EnsureVisible(nFocusRecordNo,TRUE))
			break;
		theStatus = 0;
	}while(0);
	if(theStatus)
		return -1;
	return 0;
}

void CColorHeaderList::DrawListControl(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	UINT uiFlags = ILD_TRANSPARENT;
	CImageList* pImageList;
	int nItem = lpDrawItemStruct->itemID;
	BOOL bFocus = (GetFocus( ) == this);
	COLORREF clrTextSave, clrBkSave;
	COLORREF clrImage = m_clrBkgnd;
	static _TCHAR szBuff[MAX_PATH];
	LPCTSTR pszText;

// get item data

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szBuff;
	lvi.cchTextMax = sizeof(szBuff);
	lvi.stateMask = 0xFFFF;		// get all state flags
	GetItem(&lvi);

	BOOL bSelected = (bFocus || (GetStyle() & LVS_SINGLESEL) || (GetStyle( ) & LVS_SHOWSELALWAYS)) 
		&& (lvi.state & LVIS_SELECTED);
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

// set colors if item is selected

	CRect rcAllLabels;
	GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);

	CRect rcLabel;
	GetItemRect(nItem, rcLabel, LVIR_LABEL);

	rcAllLabels.left = rcLabel.left;
	if (m_bClientWidthSel && rcAllLabels.right < m_cxClient)
		rcAllLabels.right = m_cxClient;

	if (bSelected)
	{
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else {
		if(m_clrUserSet[nItem])
		{
			clrTextSave = pDC->SetTextColor(m_clrUserSet[nItem]->m_clrUserSetText);
			clrBkSave = pDC->SetBkColor(m_clrUserSet[nItem]->m_clrUserSetTextBk);

			pDC->FillRect(rcAllLabels, &CBrush(m_clrUserSet[nItem]->m_clrUserSetTextBk));
		} else
			pDC->FillRect(rcAllLabels, &CBrush(m_clrTextBk));
	}

// set color and mask for the icon

	if (lvi.state & LVIS_CUT)
	{
		clrImage = m_clrBkgnd;
		uiFlags |= ILD_BLEND50;
	}
	else if (bSelected)
	{
		clrImage = ::GetSysColor(COLOR_HIGHLIGHT);
		uiFlags |= ILD_BLEND50;
	}

// draw state icon

	UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
	if (nStateImageMask)
	{
		int nImage = (nStateImageMask >> 12) - 1;
		pImageList = GetImageList(LVSIL_STATE);
		if (pImageList)
		{
			pImageList->Draw(pDC, nImage,
				CPoint(rcItem.left, rcItem.top), ILD_TRANSPARENT);
		}
	}

// draw normal and overlay icon

	CRect rcIcon;
	GetItemRect(nItem, rcIcon, LVIR_ICON);

	pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		UINT nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;
		if (rcItem.left < rcItem.right-1)
		{
			ImageList_DrawEx(pImageList->m_hImageList, lvi.iImage,
					pDC->m_hDC,rcIcon.left,rcIcon.top, 16, 16,
					m_clrBkgnd, clrImage, uiFlags | nOvlImageMask);
		}
	}

// draw item label

	GetItemRect(nItem, rcItem, LVIR_LABEL);
	rcItem.right -= m_cxStateImageOffset;

	pszText = MakeShortString(pDC, szBuff,
				rcItem.right - rcItem.left, 2*OFFSET_FIRST);

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	pDC->DrawText(pszText,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

// draw labels for extra columns

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
				   
	for(int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)
	{
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

		int nRetLen = GetItemText(nItem, nColumn,
						szBuff, sizeof(szBuff));
		if (nRetLen == 0)
			continue;

		pszText = MakeShortString(pDC, szBuff,
			rcItem.right - rcItem.left, 2*OFFSET_OTHER);

		UINT nJustify = DT_LEFT;

		if(pszText == szBuff)
		{
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
			{
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;
				break;
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
			default:
				break;
			}
		}

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText, -1, rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
	}

// draw focus rectangle if item has focus

	if (lvi.state & LVIS_FOCUSED && bFocus)
		pDC->DrawFocusRect(rcAllLabels);

// set original colors if item was selected

	if (bSelected || m_clrUserSet[nItem] != NULL)
	{
        pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

LPCTSTR CColorHeaderList::MakeShortString(CDC * pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{

	static const _TCHAR szThreeDots[] = _T("...");

	int nStringLen = lstrlen(lpszLong);

	if(nStringLen == 0 ||
		(pDC->GetTextExtent(lpszLong, nStringLen).cx + nOffset) <= nColumnLen)
	{
		return(lpszLong);
	}

	static _TCHAR szShort[MAX_PATH];

	lstrcpy(szShort,lpszLong);
	int nAddLen = pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;

	for(int i = nStringLen-1; i > 0; i--)
	{
		szShort[i] = 0;
		if((pDC->GetTextExtent(szShort, i).cx + nOffset + nAddLen)
			<= nColumnLen)
		{
			break;
		}
	}

	lstrcat(szShort, szThreeDots);
	return(szShort);
}

void CColorHeaderList::RepaintSelectedItems()
{

	CRect rcItem, rcLabel;

// invalidate focused item so it can repaint properly

	int nItem = GetNextItem(-1, LVNI_FOCUSED);

	if(nItem != -1)
	{
		GetItemRect(nItem, rcItem, LVIR_BOUNDS);
		GetItemRect(nItem, rcLabel, LVIR_LABEL);
		rcItem.left = rcLabel.left;

		InvalidateRect(rcItem, FALSE);
	}

// if selected items should not be preserved, invalidate them

	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem = GetNextItem(-1, LVNI_SELECTED);
			nItem != -1; nItem = GetNextItem(nItem, LVNI_SELECTED))
		{
			GetItemRect(nItem, rcItem, LVIR_BOUNDS);
			GetItemRect(nItem, rcLabel, LVIR_LABEL);
			rcItem.left = rcLabel.left;

			InvalidateRect(rcItem, FALSE);
		}
	}

// update changes 

	UpdateWindow();
}

void CColorHeaderList::SetHeaderLabel(int nCount, TCHAR *label[ ], int * width, int * fmt)
{
 
	LV_COLUMN lvc;
	int j;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
//insert header
	for(j = 0; j < nCount; j++)
	{
		lvc.iSubItem = j;
		lvc.pszText =  _T(label[j]);
		lvc.cx = width[j];
		lvc.fmt = fmt[j];
		InsertColumn(j,&lvc);
	}
}

int CColorHeaderList::GetCurSel()
{
	int nItem = GetNextItem(-1, LVNI_SELECTED);
	return nItem;
}


void CColorHeaderList::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CWnd *pWnd;
	pWnd = GetParent();
	CString str;
	pWnd->GetWindowText(str);
	if(str.Find("查询调度命令历史记录")>=0)
	////if(str == "查询调度命令历史记录")

	{
		pWnd->SendMessage(UM_SORT_MANAGE_LIST,(WPARAM)pNMListView->iSubItem);
	}
	*pResult = 0;
}