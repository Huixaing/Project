// DlgMessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox dialog


CDlgMessageBox::CDlgMessageBox(CString sMessage,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMessageBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMessageBox)
	m_text = _T("");
	//}}AFX_DATA_INIT

	m_text = sMessage;
}


void CDlgMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMessageBox)
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMessageBox, CDialog)
	//{{AFX_MSG_MAP(CDlgMessageBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox message handlers

BOOL CDlgMessageBox::OnInitDialog() 
{
	CDialog::OnInitDialog();

    UpdateData(FALSE);
	InitDlgPosition(this);

	COLORREF bgcolor=(COLORREF)::GetSysColor(COLOR_3DFACE);
	COLORMAP cMap1 = {RGB(251, 254, 252), bgcolor/*CLR_PALEGRAY*/ };
	//if(mBmp.LoadMappedBitmap(IDB_CASCO,0,&cMap1,1)){
	//	CStatic* pWnd=(CStatic*)GetDlgItem(IDC_STATIC);
	//	if(pWnd!=NULL)
	//		pWnd->SetBitmap((HBITMAP)mBmp);
	//}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

