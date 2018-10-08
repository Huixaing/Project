#pragma once

#include "PropToolbar.h"
#include "PropTree.h"

// CToolbarContainer

class CToolbarContainer : public CWnd
{
	DECLARE_DYNAMIC(CToolbarContainer)

public:
	BOOL CreateContainer(const RECT &rect, CWnd *pParent, CPropTree *propTree, UINT toolbarID);

public:
	CToolbarContainer();
	virtual ~CToolbarContainer();

protected:
	CPropToolbar m_propToolbar;
	CPropTree    *m_pProp;
	BOOL         m_bSortByAlpha;
	
protected:
	afx_msg void OnSortByCategory();
	afx_msg void OnSortByAlpha();

	DECLARE_MESSAGE_MAP()
	
};


