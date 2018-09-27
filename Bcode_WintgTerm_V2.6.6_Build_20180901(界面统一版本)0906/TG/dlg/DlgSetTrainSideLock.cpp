// DlgSelMutiMark.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "DlgSetTrainSideLock.h"
#include ".\DlgSetTrainSideLock.h"

#define COLUMN_COUNT  3

_TCHAR *_gszTrainTitle[COLUMN_COUNT] =
{
	_T("索引"),_T("车次号"),_T("运行类型")
};
// CDlgSetTrainSideLock 对话框

IMPLEMENT_DYNAMIC(CDlgSetTrainSideLock, CDialog)
CDlgSetTrainSideLock::CDlgSetTrainSideLock(CTGDoc *pDoc, CWnd* pParent)
: CDialog(CDlgSetTrainSideLock::IDD, pParent)
{
	m_pDoc = pDoc;
	memset(mSelTrainIndex, 0, sizeof(mSelTrainIndex));
	mSelTrainCount=0;
	m_pGridCtrl=NULL;
}

CDlgSetTrainSideLock::~CDlgSetTrainSideLock()
{
	if (m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl=NULL;
	}
}

void CDlgSetTrainSideLock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMER1, m_btm);
	DDX_Control(pDX, IDC_DATETIMER2, m_etm);
	DDX_Control(pDX, IDC_SEL_ALL, m_selall);
	DDX_Control(pDX, IDC_SEL_NONE, m_selnone);
}


BEGIN_MESSAGE_MAP(CDlgSetTrainSideLock, CDialog)
	ON_BN_CLICKED(IDC_FIND_TRAIN, OnBnClickedBtnFind)
	ON_BN_CLICKED(IDC_SET_SIDE_LOCK, OnBnClickedLock)
	ON_BN_CLICKED(IDC_SET_SIDE_UNLOCK, OnBnClickedUnLock)
	ON_BN_CLICKED(IDC_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_SEL_ALL, OnBtnSelAll)
	ON_BN_CLICKED(IDC_SEL_NONE, OnBtnSelNone)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgSetTrainSideLock 消息处理程序
BOOL CDlgSetTrainSideLock::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int col;
	if (NULL==m_pGridCtrl)
		m_pGridCtrl= new CGridCtrl;
	if(!m_pGridCtrl) 
		return FALSE;

	CRect rect2;
	CRect rect;
	CWnd *pWnd;
	pWnd= GetDlgItem(IDC_GRID_TRAIN);
	if(pWnd == NULL) 
		return FALSE;

	m_btm.SetFormat("yyyy-MM-dd HH:mm:ss");
	m_etm.SetFormat("yyyy-MM-dd HH:mm:ss");

	m_selall.EnableWindow(FALSE);
	m_selnone.EnableWindow(FALSE);

	CTime tm = CTime::GetCurrentTime();

	CTime btm = tm;
	btm-=CTimeSpan(0, 0, 0, 0);
	m_btm.SetTime(&btm);

	CTime etm = tm;
	etm+=CTimeSpan(0, 6, 0, 0);
	m_etm.SetTime(&etm);

	pWnd->GetClientRect(rect);
	pWnd->MapWindowPoints(this,rect);

	m_pGridCtrl->Create(rect,this,IDC_GRID_TRAIN);
	try{
		m_pGridCtrl->SetFixedRowCount(1);
		m_pGridCtrl->SetFixedColumnCount(0);
		m_pGridCtrl->SetColumnCount(COLUMN_COUNT);           
		m_pGridCtrl->SetRowCount(1);
		m_pGridCtrl->SetFixedTextColor(CLR_BLUE);      //文本颜色
		m_pGridCtrl->SetGridLineColor(RGB(166,202,240));       //线的颜色
		m_pGridCtrl->SetFixedBkColor(GetSysColor(COLOR_3DFACE));
		m_pGridCtrl->SetFixedColumnSelection(TRUE);
		m_pGridCtrl->SetFixedRowSelection(TRUE);

	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	for (col = 0; col < COLUMN_COUNT; col++)
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
		Item.strText.Format(_gszTrainTitle[col]);
		m_pGridCtrl->SetItem(&Item);

	}
	m_pGridCtrl->AutoSizeColumns();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSetTrainSideLock::OnBnClickedBtnFind()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pGridCtrl==NULL)
		return;

	m_pGridCtrl->DeleteNonFixedRows();
	m_pGridCtrl->Invalidate();

	for (int n=1;n<m_pGridCtrl->GetRowCount();n++)
	{
		m_pGridCtrl->DeleteRow(n);
	}

	if(NULL == m_pDoc)
		return;

	if(NULL == m_pDoc->myPDataManage)
		return;

	CTime btm, etm;
	m_btm.GetTime(btm);
	m_etm.GetTime(etm);

	if(btm >= etm)
	{
		AfxMessageBox("查找结束时间必须大于开始时间");
		return;
	}

	ULONG train_index[2048], index_cnt=0;
	memset(train_index, 0, sizeof(train_index));
	index_cnt=m_pDoc->FindTrainByTime(btm.GetTime(), etm.GetTime(), train_index);

	m_pGridCtrl->SetRowCount(index_cnt + 1);  

	CGridCellCheck *pCell;
	int row_index =0;
	for(int m = 0; m < index_cnt; m++)
	{		 
		CTG_TRAIN* train = m_pDoc->myPDataManage->GetTrain(train_index[m]);
		if(NULL == train)
			continue;
		if(train->IsExistActualTime())
			continue;

		//0 序号
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = row_index+1;  //行
		Item.col = 0;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%d", train->GetTrainIndex());
		m_pGridCtrl->SetItem(&Item);

		//1 车次号
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = row_index+1;  //行
		Item.col = 1;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%s", m_pDoc->GetTrainShowId(train));
		m_pGridCtrl->SetItem(&Item);

		//2 列车类型
		CString strType;
		int type_index = -1;
		std::map<int, CString>::iterator it1 = m_pDoc->mTrainType.mRunTypeNameMap.begin();
		for(int i = 0 ;it1 != m_pDoc->mTrainType.mRunTypeNameMap.end(); it1++, i++)
		{
			if(train->myTrainInfo.run_type == (*it1).first)
			{
				type_index = i;
				break;
			}
		}

		if (type_index>=0)
		{
			strType.Format("%s", it1->second);
		}
		else
			strType.Format("类型 %d", train->myTrainInfo.run_type);

		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = row_index+1;  //行
		Item.col = 2;  //列
		Item.crBkClr = CLR_RED;        
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%s", strType);
		m_pGridCtrl->SetItem(&Item);

		m_pGridCtrl->SetCellType(row_index+1,0,RUNTIME_CLASS(CGridCellCheck));
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(row_index+1,0);
		if(pCell)
		{
			pCell->SetCheck(FALSE);
		}

		row_index++;
	}

	m_pGridCtrl->SetRowCount(row_index + 1);  

	m_pGridCtrl->AutoSizeColumns();

	if(0 == index_cnt)
	{
		AfxMessageBox("请注意:在给定时间范围内, 查找不到列车运行线");
	}
	else
	{
		m_selall.EnableWindow(TRUE);
		m_selnone.EnableWindow(TRUE);
	}
	return;
}
void CDlgSetTrainSideLock::OnBnClickedLock()
{
	// TODO: 在此添加控件通知处理程序代码
	memset(mSelTrainIndex, 0, sizeof(mSelTrainIndex));
	mSelTrainCount=0;

    CString xml="<root>", str;
	for(int i=1; i<m_pGridCtrl->GetRowCount(); i++)
	{
		CGridCellCheck* pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,0);
		if(NULL==pCell)
			continue;
		if(pCell->GetCheck())
		{
			str.Format("<TRAIN_INDEX>%s</TRAIN_INDEX>", m_pGridCtrl->GetItemText(i, 0));
			xml+=str;
		}
	}
	xml+="</root>";
	m_pDoc->m_ptgctrl->FIRE_MULTI_TRAIN_OPER(xml, 0x00000001);
	OnBtnClose();
}

void CDlgSetTrainSideLock::OnBnClickedUnLock()
{
	// TODO: 在此添加控件通知处理程序代码
	memset(mSelTrainIndex, 0, sizeof(mSelTrainIndex));
	mSelTrainCount=0;

	CString xml="<root>", str;
	for(int i=1; i<m_pGridCtrl->GetRowCount(); i++)
	{
		CGridCellCheck* pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,0);
		if(NULL==pCell)
			continue;
		if(pCell->GetCheck())
		{
			str.Format("<TRAIN_INDEX>%s</TRAIN_INDEX>", m_pGridCtrl->GetItemText(i, 0));
			xml+=str;
		}
	}
	xml+="</root>";
	m_pDoc->m_ptgctrl->FIRE_MULTI_TRAIN_OPER(xml, 0x00000002);
	OnBtnClose();
}


void CDlgSetTrainSideLock::OnBtnClose()
{
	CDialog::OnOK();
}

void CDlgSetTrainSideLock::OnBtnSelAll()
{
	if (m_pGridCtrl->GetRowCount()<=1)
		return;

	for (int i=1;i<m_pGridCtrl->GetRowCount();i++)
	{
		CGridCellCheck *pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,0);
		if(NULL==pCell)
			continue;
		pCell->SetCheck(TRUE);
	}
}

void CDlgSetTrainSideLock::OnBtnSelNone()
{
	if (m_pGridCtrl->GetRowCount()<=1)
		return;

	for (int i=1;i<m_pGridCtrl->GetRowCount();i++)
	{
		CGridCellCheck *pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,0);
		if(NULL==pCell)
			continue;
		pCell->SetCheck(FALSE);
	}
}