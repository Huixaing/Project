// DlgMultiSelOperate.cpp : implementation file
//

#include "stdafx.h"

#include "DlgMultiSelOperate.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COLUMN_COUNT  4

_TCHAR *_gMultiOperateTitle[COLUMN_COUNT] =
{
	_T("序号"),_T("车次号"),_T("选中"),_T("列车索引")
};

/////////////////////////////////////////////////////////////////////////////
CDlgMultiSelOperate::CDlgMultiSelOperate(std::vector<DWORD>& list, CWnd* pParent)
	: CDialog(CDlgMultiSelOperate::IDD, pParent)
{
	m_pGridCtrl = NULL;
		
	all.assign(list.begin(), list.end());
    sel.assign(list.begin(), list.end());
}

CDlgMultiSelOperate::~CDlgMultiSelOperate()
{
	if(m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
}

void CDlgMultiSelOperate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMultiSelOperate)
	DDX_Control(pDX, IDC_COMBO_TRAINTYPE, m_ctlTrainType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMultiSelOperate, CDialog)
	//{{AFX_MSG_MAP(CDlgMultiSelOperate)
	ON_BN_CLICKED(IDC_BUTTON_SELALL, OnButtonSelall)
	ON_BN_CLICKED(IDC_BUTTON_NOTSELALL, OnButtonNotSelall)
	ON_CBN_SELCHANGE(IDC_COMBO_TRAINTYPE, OnSelchangeComboTraintype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMultiSelOperate message handlers

void CDlgMultiSelOperate::fillGrid()
{
	int count = 1;
	
	ASSERT_VALID(m_pGridCtrl);
	m_pGridCtrl->DeleteNonFixedRows();

	m_pGridCtrl->SetRedraw(FALSE);
	m_pGridCtrl->SetRowCount(sel.size() + 1);
	

	for(int i=0; i<sel.size(); i++)
	{
		ULONG trainIndex = sel[i];
		CTG_SingleTrain* train = m_pSchdBase->GetSingleTrain(trainIndex);
		if(train == NULL)
			continue;
		CString trainId = train->GetTrainID();

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
		Item.strText.Format("%s",  trainId);
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

		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nState = GVIS_READONLY;
		Item.row = count;  //行
		Item.col = 3;  //列
		Item.crBkClr = RGB(255,0,0);         
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		Item.strText.Format("%d",trainIndex);
		m_pGridCtrl->SetItem(&Item);            

		m_pGridCtrl->SetItemData(count,0,(DWORD)trainIndex);
		count ++;
		
	}
	m_pGridCtrl->SetRedraw(TRUE);
	m_pGridCtrl->AutoSizeColumns();
}


void CDlgMultiSelOperate::initialGridCtrl()
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
		m_pGridCtrl->SetColumnCount(COLUMN_COUNT);           
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
		Item.strText.Format(_gMultiOperateTitle[col]);
		m_pGridCtrl->SetItem(&Item);
	}
}

BOOL CDlgMultiSelOperate::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	initialGridCtrl();
	fillGrid();
    
	//m_ctlDirect.InsertString(-1,"上行");
	//m_ctlDirect.InsertString(-1,"下行");
	//m_ctlDirect.InsertString(-1,"上下行");
	//m_ctlDirect.SetCurSel(2);

	m_ctlTrainType.InsertString(-1,"客车");
	m_ctlTrainType.InsertString(-1,"货车");
	m_ctlTrainType.InsertString(-1,"客货车");
	m_ctlTrainType.SetCurSel(2);
	
	

	m_pGridCtrl->AutoSizeColumns();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgMultiSelOperate::OnOK() 
{
    CGridCellCheck *pCell;
	
	sel.clear();
	for(int i=1;i<= all.size();i++)
	{
		BOOL m_bChecked=FALSE;
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,2);
		if(pCell)
		{
			m_bChecked = pCell->GetCheck();
		}
		else
		{
			m_bChecked=FALSE;
		}
		
		if(m_bChecked)
		{
			DWORD key = (DWORD)m_pGridCtrl->GetItemData(i,0);
			sel.push_back(key);
		}
	}

	if(sel.size() ==0)
	{
		AfxMessageBox("请选中要删除的列车？");
		return;
	}
	else
	{
		if(AfxMessageBox("确定要删除列车？", MB_YESNO) == IDYES)
		{
			CDialog::OnOK();
		}
	}
	SendMessage(WM_CLOSE);
}

void CDlgMultiSelOperate::OnButtonSelall() 
{
    CGridCellCheck *pCell;

	for(int i= 1; i<= all.size();i++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,2);
		if(pCell)
		{
			pCell->SetCheck(TRUE);
		}
	}
}

void CDlgMultiSelOperate::OnButtonNotSelall()
{
    CGridCellCheck *pCell;

	for(int i= 1; i<= all.size();i++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i,2);
		if(pCell)
		{
			pCell->SetCheck(FALSE);
		}
	}
}
//void CDlgMultiSelOperate::OnSelchangeComboDirection() 
//{
//	ResetTrainSelected();
//	fillGrid();
//}

void CDlgMultiSelOperate::OnSelchangeComboTraintype() 
{
	ResetTrainSelected();
	fillGrid();
}

BOOL CDlgMultiSelOperate::ResetTrainSelected()
{
	int typeFlag = 0;

	typeFlag = m_ctlTrainType.GetCurSel();
	sel.clear();

	for(int i=0; i<all.size(); i++)
	{
		CTG_SingleTrain* single = m_pSchdBase->GetSingleTrain(all[i]);
		if(single == NULL)
			continue;

		//switch(dirFlag)
		//{
		//case 0://上行
		//	if(single->m_nDirectFlag != UP_DIRECT)
		//		continue;
		//	break;
		//case 1://下行
		//	if(single->m_nDirectFlag != DOWN_DIRECT)
		//		continue;
		//	break;
		//case 2://上下行
		//	break;
		//}

		switch(typeFlag)
		{
		case 0://客车
			if(!single->IsPassengerTrain())
				continue;
			break;
		case 1://货车
			if(single->IsPassengerTrain())
				continue;
			break;
		case 2://客货车
			break;
		}

		sel.push_back(all[i]);
	}
	return TRUE;
}
