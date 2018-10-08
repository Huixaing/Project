// ToolbarContainer.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "PropTree.h"
#include "ToolbarContainer.h"


// CToolbarContainer

IMPLEMENT_DYNAMIC(CToolbarContainer, CWnd)
CToolbarContainer::CToolbarContainer()
{
	m_pProp = NULL;
	m_bSortByAlpha = FALSE;
}

CToolbarContainer::~CToolbarContainer()
{
}


BEGIN_MESSAGE_MAP(CToolbarContainer, CWnd)

	ON_COMMAND(ID_PROP_ALPHABETIC,  OnSortByAlpha)
	ON_COMMAND(ID_PROP_CATEGORIZE,  OnSortByCategory)

END_MESSAGE_MAP()


BOOL CToolbarContainer::CreateContainer(const RECT &rect, CWnd *pParent, CPropTree *propTree, UINT toolbarID)
{
	ASSERT(propTree != NULL);

	LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW), 
					GetSysColorBrush(COLOR_BTNFACE));

	UINT dwStyle = WS_VISIBLE | WS_CHILD;

	if (! CWnd::Create(pszCreateClass, _T(""), dwStyle, rect, pParent, 222))
	{
		TRACE0("Failed to create toolbar container\n");
		return FALSE;
	}

	dwStyle = TBSTYLE_TRANSPARENT | WS_VISIBLE | WS_CHILD | TBSTYLE_FLAT 
			 | CCS_TOP | TBSTYLE_TOOLTIPS; //| CCS_NORESIZE CCS_ADJUSTABLE |  

	if (! m_propToolbar.Create(dwStyle, CRect(0, 0, 0, 0), this, toolbarID))//&m_toolbarContainer
	{
		TRACE0("Failed to create prop tool bar\n");
		return FALSE;
	}
	
	m_propToolbar.SetButtonSize(CSize(16, 16));
	m_propToolbar.CheckButton(ID_PROP_CATEGORIZE);

	m_pProp = propTree;

	return TRUE;
}

// CToolbarContainer message handlers
void  CToolbarContainer::OnSortByCategory()
{
	if (m_propToolbar.IsButtonChecked(ID_PROP_ALPHABETIC))
	{
		m_propToolbar.CheckButton(ID_PROP_ALPHABETIC, FALSE);
	}
	
	if (! m_propToolbar.IsButtonChecked(ID_PROP_CATEGORIZE))
	{
		m_propToolbar.CheckButton(ID_PROP_CATEGORIZE);
		return;
	}

	m_pProp->SortByCategory();

}

void  CToolbarContainer::OnSortByAlpha()
{
	if (m_propToolbar.IsButtonChecked(ID_PROP_CATEGORIZE))
	{
		m_propToolbar.CheckButton(ID_PROP_CATEGORIZE, FALSE);
	}

	if (! m_propToolbar.IsButtonChecked(ID_PROP_ALPHABETIC))
	{
		m_propToolbar.CheckButton(ID_PROP_ALPHABETIC);
		return;
	}

	
	m_pProp->SortByAlpha();
}
