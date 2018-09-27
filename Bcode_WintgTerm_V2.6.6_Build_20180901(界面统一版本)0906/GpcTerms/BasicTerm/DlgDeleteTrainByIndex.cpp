// UserPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DlgDeleteTrainByIndex.h"
#include ".\dlgdeletetrainbyindex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDeleteTrainByIndex dialog
CDlgDeleteTrainByIndex::CDlgDeleteTrainByIndex(CWnd* pParent /*=NULL*/)
: CDialog(CDlgDeleteTrainByIndex::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDeleteTrainByIndex)
	m_nTrainIndex = 0;
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CDlgDeleteTrainByIndex, CDialog)
END_MESSAGE_MAP()
void CDlgDeleteTrainByIndex::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChangeHatTrainId)
	DDX_Text(pDX, IDC_TRAIN_INDEX, m_nTrainIndex);
	//}}AFX_DATA_MAP
}
