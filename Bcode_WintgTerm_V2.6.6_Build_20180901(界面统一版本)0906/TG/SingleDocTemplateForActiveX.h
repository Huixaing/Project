// SingleDocTemplateForActiveX.h: interface for the CSingleDocTemplateForActiveX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEDOCTEMPLATEFORACTIVEX_H__B28F441F_84EF_4F87_AFFB_96A6B091AE24__INCLUDED_)
#define AFX_SINGLEDOCTEMPLATEFORACTIVEX_H__B28F441F_84EF_4F87_AFFB_96A6B091AE24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Stdafx.h"


class CSingleDocTemplateForActiveX : public CSingleDocTemplate  
{
public:
	CSingleDocTemplateForActiveX(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
	virtual ~CSingleDocTemplateForActiveX();

	CWnd* NewDVF(CWnd *pParent);
	CFrameWnd* CreateNewFrame(CDocument* pDoc, CWnd* pParent);
};

#endif // !defined(AFX_SINGLEDOCTEMPLATEFORACTIVEX_H__B28F441F_84EF_4F87_AFFB_96A6B091AE24__INCLUDED_)
