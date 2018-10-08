// TccCommandSketchMapDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "tcc_dlg.h"
#include "TccCommandSketchMapDlg.h"

namespace TCC_C3
{

// CTccCommandSketchMapDlg 对话框

IMPLEMENT_DYNAMIC(CTccCommandSketchMapDlg, CDialog)
CTccCommandSketchMapDlg::CTccCommandSketchMapDlg(
		ITccClientInfoProvider *pInfoProvider,
		CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
	, m_SketchMap(pInfoProvider)
{
	m_BtnEn=false;
	ASSERT(pInfoProvider);
	m_pInfoProvider = pInfoProvider;
	m_pInfoProvider->AddRef();
}

CTccCommandSketchMapDlg::~CTccCommandSketchMapDlg()
{
	if(m_pInfoProvider)
	{
		m_pInfoProvider->Release();
		m_pInfoProvider = NULL;
	}
}

void CTccCommandSketchMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTccCommandSketchMapDlg, CDialog)
	//{{AFX_MSG_MAP(CTccCommandSketchMapDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BeginLimit,OnClickedBtnBeginLimit)
	ON_BN_CLICKED(IDC_EndLimit,OnClickedBtnEndLimit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CTccCommandSketchMapDlg 消息处理程序
BOOL CTccCommandSketchMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CClientDC   dc(this);   
	CEdit*   pEdit   =   (CEdit*)GetDlgItem(IDC_TEXT);   
	CFont*   font   =   new   CFont;   
	font->CreatePointFont(120,   _T("宋体"),   &dc);   
	if(pEdit)   
	{   
		pEdit->SetFont(font,   TRUE);
	}   
	else   
		AfxMessageBox("Failed");  
//	str.Format("%s 至 %s", nKmBegin.toString(m_pInfoProvider, m_nLineID), nKmEnd.toString(m_pInfoProvider, m_nLineID));
	CRect rect;
	GetDlgItem(IDC_SKETCH_MAP)->GetClientRect(&rect);
	if(!m_BtnEn)
	{
		GetDlgItem(IDC_BeginLimit)->EnableWindow(false);
		GetDlgItem(IDC_EndLimit)->EnableWindow(false);
		GetDlgItem(IDC_BeginLimit)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EndLimit)->ShowWindow(SW_HIDE);
	}
//	rect.left=rect.left+10;
//	rect.right=rect.right-10;
	m_SketchMap.Create("XXX", WS_CHILD|WS_VISIBLE , rect, GetDlgItem(IDC_SKETCH_MAP), 1,m_BtnEn);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CTccCommandSketchMapDlg::OnOK()
{
	if(m_SketchMap.m_nKmMark[0].Equal(m_SketchMap.m_nKmMark[1]))
	{
		MessageBox("限速起始点与结束点相同，不合法！","提示",MB_OK|MB_ICONEXCLAMATION|MB_ICONWARNING);
		return;
	}
	CDialog::OnOK();
}
HBRUSH CTccCommandSketchMapDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{

	// TODO: Change any attributes of the DC here
	if(pWnd->GetDlgCtrlID()==IDC_TEXT)
	{
		pDC->SetTextColor(RGB(255,0,0));
	}
	else
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO: Return a different brush if the default is not desired
}

void CTccCommandSketchMapDlg::OnClickedBtnBeginLimit()
{
	//MessageBox("nihao");
	m_SketchMap.BtnSLpos=true;
	m_SketchMap.BtnSRpos=false;
}
void CTccCommandSketchMapDlg::OnClickedBtnEndLimit()
{
	//MessageBox("women");
	m_SketchMap.BtnSLpos=false;
	m_SketchMap.BtnSRpos=true;
}

void CTccCommandSketchMapDlg::SetKmMark(int builtin_id, kmmark_t nKmBegin, kmmark_t nKmEnd)
{
	m_SketchMap.SetKmMark(builtin_id, nKmBegin, nKmEnd);
}

}