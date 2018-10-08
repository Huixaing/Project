
#include "StdAfx.h"
#include "TabSheet.h"

IMPLEMENT_DYNAMIC(CTabSheet, CTabCtrl);

CTabSheet::CTabSheet()
{
	m_nNumOfPages = 0;
	m_nCurrentPage = 0;
	memset(m_pPages, 0, sizeof(m_pPages));
	memset(m_IDD, 0, sizeof(m_IDD));
}

CTabSheet::~CTabSheet()
{
}

BOOL CTabSheet::AddPage(LPCTSTR title, CDialog *pDialog,UINT ID)
{
	if( MAXPAGE == m_nNumOfPages )
		return FALSE;

	//保存目前总的子对话框数
	m_nNumOfPages++;

	//记录子对话框的指针、资源ID、要在标签上显示的文字
	m_pPages[m_nNumOfPages-1] = pDialog;
	m_IDD[m_nNumOfPages-1] = ID;
	m_Title[m_nNumOfPages-1] = title;

	return TRUE;
}

void CTabSheet::SelectPage(int nSel)
{
	SetCurSel(nSel);

	//判断是否单击了其他标签
	if(m_nCurrentPage != GetCurFocus())
	{
		//将原先的子对话框隐藏
		m_pPages[m_nCurrentPage]->ShowWindow(SW_HIDE);
		m_nCurrentPage=GetCurFocus();
		//显示当前标签所对应的子对话框
		m_pPages[m_nCurrentPage]->ShowWindow(SW_SHOW);
	}
}

void CTabSheet::Show()
{
	//利用CDialog::Create来创建子对话框，并且使用CTabCtrl::InsertItem来加上相应的标签
	for( int i=0; i < m_nNumOfPages; i++ )
	{
		m_pPages[i]->Create( m_IDD[i], this );
		InsertItem( i, m_Title[i] );
	}

	//由于对话框显示时默认的是第一个标签被选中，所以应该让第一个子对话框显示，其他子对话框隐藏
	m_pPages[0]->ShowWindow(SW_SHOW);
	for( i=1; i < m_nNumOfPages; i++)
		m_pPages[i]->ShowWindow(SW_HIDE);

	SetRect();
}

void CTabSheet::SetRect()
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	//得到Tab Control的大小
	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	//计算出各子对话框的相对于Tab Control的位置和大小
	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=tabRect.right-itemRect.left-2;
	nYc=tabRect.bottom-nY-2;

	//利用计算出的数据对各子对话框进行调整
	m_pPages[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for( int nCount=1; nCount < m_nNumOfPages; nCount++ )
		m_pPages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
}

BEGIN_MESSAGE_MAP(CTabSheet, CTabCtrl)
	//{{AFX_MSG_MAP(CTabSheet)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTabSheet::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	//判断是否单击了其他标签
	if(m_nCurrentPage != GetCurFocus())
	{
		//将原先的子对话框隐藏
		m_pPages[m_nCurrentPage]->ShowWindow(SW_HIDE);
		m_nCurrentPage=GetCurFocus();
		//显示当前标签所对应的子对话框
		m_pPages[m_nCurrentPage]->ShowWindow(SW_SHOW);
	}
}

