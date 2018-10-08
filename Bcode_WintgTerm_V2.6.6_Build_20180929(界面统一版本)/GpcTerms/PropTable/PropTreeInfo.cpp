// PropTreeInfo.cpp : implementation file
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
#include "PropTree.h"
#include "Resource.h"
#include "PropTreeInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropTreeInfo

CPropTreeInfo::CPropTreeInfo() :
	m_pProp(NULL)
{
}

CPropTreeInfo::~CPropTreeInfo()
{
}


BEGIN_MESSAGE_MAP(CPropTreeInfo, CStatic)
	//{{AFX_MSG_MAP(CPropTreeInfo)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropTreeInfo message handlers
static int DrawTextInRect(CDC *pDC,const char *str,RECT *prect,int coltap,int rowtap,BYTE type);
void CPropTreeInfo::SetPropOwner(CPropTree* pProp)
{
	m_pProp = pProp;
}

void CPropTreeInfo::OnPaint() 
{
	CPaintDC dc(this);
	CRect rc;

	GetClientRect(rc);

	dc.SelectObject(GetSysColorBrush(COLOR_BTNFACE));
	dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);

	dc.DrawEdge(&rc, BDR_SUNKENOUTER, BF_RECT);
	rc.DeflateRect(4, 4);

	ASSERT(m_pProp!=NULL);

	CPropTreeItem* pItem = m_pProp->GetFocusedItem();

	if (!m_pProp->IsWindowEnabled())
		dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
	else
		dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));

	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(m_pProp->GetBoldFont());

	CString txt;
	CString content;
    if (pItem)
	{
		txt=pItem->GetLabelText();
		content=pItem->GetInfoText();
	}
	if(content.IsEmpty())
	{
		txt="说明信息";
		content=m_pProp->GetGeneralInfo();
	}
	
	CRect ir;
	ir = rc;
		
	// draw label
	dc.DrawText(txt, &ir, DT_SINGLELINE|DT_CALCRECT);
	dc.DrawText(txt, &ir, DT_SINGLELINE);

	ir.top = ir.bottom;
	ir.bottom = rc.bottom;
	ir.right = rc.right;

	dc.SelectObject(m_pProp->GetNormalFont());
	DrawTextInRect(&dc,content,&ir,2,2,0);
		
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
		pDC->TextOut(x,y,substr,strlen(substr));
		x+=size.cx+size.cx*coltap/(10*strlen(substr));
	}
	return 0;
}

