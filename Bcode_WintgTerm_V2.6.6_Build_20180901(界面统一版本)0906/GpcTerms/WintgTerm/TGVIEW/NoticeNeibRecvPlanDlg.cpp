// NoticeNeibRecvPlanDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "NoticeNeibRecvPlanDlg.h"
#include ".\noticeneibrecvplandlg.h"

#define COLUMN_COUNT_NEIB  3

_TCHAR *_gTitle[COLUMN_COUNT_NEIB] =
{
	_T("序号"),_T("邻台名称"),_T("选中")
};

// CNoticeNeibRecvPlanDlg 对话框

IMPLEMENT_DYNAMIC(CNoticeNeibRecvPlanDlg, CDialog)
CNoticeNeibRecvPlanDlg::CNoticeNeibRecvPlanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNoticeNeibRecvPlanDlg::IDD, pParent)
{
	m_pGridCtrl = NULL;
	bSelAll = false;
}

CNoticeNeibRecvPlanDlg::~CNoticeNeibRecvPlanDlg()
{
	if(m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
}

void CNoticeNeibRecvPlanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNoticeNeibRecvPlanDlg, CDialog)
	ON_BN_CLICKED(ID_SELALLBTN, OnBnClickedSelallbtn)
END_MESSAGE_MAP()


// CNoticeNeibRecvPlanDlg 消息处理程序
void CNoticeNeibRecvPlanDlg::fillGrid()
{
	int count = 1;
	
	ASSERT_VALID(m_pGridCtrl);
	m_pGridCtrl->DeleteNonFixedRows();

	m_pGridCtrl->SetRedraw(FALSE);
	m_pGridCtrl->SetRowCount(m_strSectionName.GetCount() + 1);
	

	for(int i=0; i<m_strSectionName.GetCount(); i++)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = count;  //行
		Item.col = 0;  //列
		Item.crBkClr = RGB(255,0,0);        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%d",count);
		m_pGridCtrl->SetItem(&Item);            


		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = count;  //行
		Item.col = 1;  //列
		Item.crBkClr = RGB(255,0,0);         
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%s",  m_strSectionName[i]);
		m_pGridCtrl->SetCellType(count,1,RUNTIME_CLASS(CUnEditAbleGridCell));
		m_pGridCtrl->SetItem(&Item);            

		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = count;  //行
		Item.col = 2;  //列
		Item.crBkClr = RGB(255,0,0);          
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("选中");
		m_pGridCtrl->SetCellType(count,2,RUNTIME_CLASS(CGridCellCheck));
		m_pGridCtrl->SetItem(&Item);            

		m_pGridCtrl->SetItemData(count,0, m_nSectionEntityId[i]);
		count ++;
	}
	m_pGridCtrl->AutoSizeColumns();
	m_pGridCtrl->SetRedraw(TRUE);
}

void CNoticeNeibRecvPlanDlg::initialGridCtrl()
{
	if(m_pGridCtrl == NULL)
		m_pGridCtrl= new CGridCtrl;
	
	int col;

	CRect rect2;
	CRect rect;
	CWnd *pWnd;
	pWnd= GetDlgItem(IDC_GRID);
	pWnd->GetClientRect(rect);
	pWnd->MapWindowPoints(this,rect);

	m_pGridCtrl->Create(rect,this,IDC_GRID);
	try
	{
		m_pGridCtrl->SetFixedRowCount(1);
		m_pGridCtrl->SetFixedColumnCount(1);
		m_pGridCtrl->SetColumnCount(COLUMN_COUNT_NEIB);           
		m_pGridCtrl->SetRowCount(1);
		m_pGridCtrl->SetFixedTextColor(RGB(0,0,255));      //文本颜色
		m_pGridCtrl->SetGridLineColor(RGB(166,202,240));       //线的颜色
       	m_pGridCtrl->SetFixedBkColor(GetSysColor(COLOR_3DFACE));
		m_pGridCtrl->SetFixedColumnSelection(TRUE);
		m_pGridCtrl->SetFixedRowSelection(TRUE);
	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return ;
	}
	
    for (col = 0; col < COLUMN_COUNT_NEIB; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		if(col == 0)
		{
			Item.nState = GVIS_READONLY|GVIS_FIXEDROW|GVIS_FIXEDCOL;
		}
		else
		{
			Item.nState = GVIS_READONLY|GVIS_FIXEDROW;
		}
		Item.row = 0;
		Item.col = col;
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format(_gTitle[col]);
		m_pGridCtrl->SetItem(&Item);
	}
}

BOOL CNoticeNeibRecvPlanDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	initialGridCtrl();
	fillGrid();
    
	m_nSelHour = 3;
	CWnd* pWnd = GetDlgItem(IDC_EDIT2);
	if(pWnd)
	{
		pWnd->SetWindowText("3");
	}
	
	CGridCellCheck* pCell;
	for(int i=0;i< m_strSectionName.GetCount();i++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,2);
		pCell->SetCheck(bSelAll);
	}
	m_pGridCtrl->ResetScrollBars();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNoticeNeibRecvPlanDlg::OnBnClickedSelallbtn()
{
	bSelAll = !bSelAll;
	CGridCellCheck* pCell;
	for(int i=0;i< m_strSectionName.GetCount();i++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,2);
		pCell->SetCheck(bSelAll);
	}
}

void CNoticeNeibRecvPlanDlg::OnOK()
{
	bool bSelSection = false;
    CGridCellCheck *pCell;
	
	CWnd* pWnd = GetDlgItem(IDC_EDIT2);
	if(pWnd)
	{
		CString strTime;
		pWnd->GetWindowText(strTime);
		m_nSelHour = atoi(strTime);
		if(m_nSelHour < 1 || m_nSelHour > 6)
		{
			AfxMessageBox("请注意, 邻台交互时间段为1-6小时, 请重新修改");
			return;
		}
	}

	for(int i=0;i< m_strSectionName.GetCount();i++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,2);
		if(pCell->GetCheck())
		{
			bSelSection = true;
		}
		else
		{
			m_nSectionEntityId[i] = 0; // 设置不选中标志
		}
	}

	if(!bSelSection)
	{
		AfxMessageBox("请注意, 你没有选择要发送的邻台");
		return;
	}
	else
	{
		CDialog::OnOK();
	}
}

