// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__27E02CF4_7B29_11D1_9CEF_000021000CE6__INCLUDED_)
#define AFX_STDAFX_H__27E02CF4_7B29_11D1_9CEF_000021000CE6__INCLUDED_

//#if _MSC_VER >= 1000
//#pragma once
//#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include "icommtask.h" 

typedef void(*DRAWFUN)(CDC* pDC,CPrintInfo* pInfo,void* pVoid);
void InitDlgPosition(CWnd* dlg);
CString GetValidStr(CString str);


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__27E02CF4_7B29_11D1_9CEF_000021000CE6__INCLUDED_)
