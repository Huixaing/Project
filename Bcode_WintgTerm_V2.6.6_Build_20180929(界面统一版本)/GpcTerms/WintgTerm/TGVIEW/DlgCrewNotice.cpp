// DlgCrewNotice.cpp : implementation file
//

#include "stdafx.h"
#include "td/td_data_def.h"
#include "DlgCrewNotice.h"
#include "TgDocument.h"
#include ".\dlgcrewnotice.h"
#include "sysshare.h"
#include "mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COLUMN_COUNT  4

_TCHAR *_gCrewNoticeTitle[COLUMN_COUNT] =
{
	_T("序号"),_T("GTID"),_T("选中"),_T("index")
};

/////////////////////////////////////////////////////////////////////////////
CDlgCrewNotice::CDlgCrewNotice(CTgDocument* pDoc, CWnd* pParent)
	: CDialog(CDlgCrewNotice::IDD, pParent)
{
	m_pDoc=pDoc;
	m_pGridCtrl = NULL;
}

CDlgCrewNotice::~CDlgCrewNotice()
{
	if(m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
}

void CDlgCrewNotice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCrewNotice)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCrewNotice, CDialog)
	//{{AFX_MSG_MAP(CDlgCrewNotice)
	ON_BN_CLICKED(IDC_BUTTON_SELALL, OnButtonSelall)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCrewNotice message handlers
void CDlgCrewNotice::SetXML(const char* token, const char* xml)
{
	CTD_CrewNotice notice;
	notice.LoadCrewNoticeFromTDXML(xml, token);
	int num=notice.m_count;
	if(notice.m_sendcount<num)
		num=notice.m_sendcount;

	for(int i=0; i<num&&i<sizeof(notice.m_gtid)/sizeof(notice.m_gtid[0]); i++)
	{
		if(notice.m_gtid[i]>0)
		{
			if(mVecGTID.end() == find(mVecGTID.begin(), mVecGTID.end(), notice.m_gtid[i]))
				mVecGTID.push_back(notice.m_gtid[i]);
		}
	}

	if(mVecGTID.size()>0)
	{
		m_nSoundCout=0;
		SetTimer(1010, 1000, NULL);
	}

	fillGrid();
}

void CDlgCrewNotice::fillGrid()
{
	int count = 1;
	
	ASSERT_VALID(m_pGridCtrl);
	m_pGridCtrl->DeleteNonFixedRows();

	m_pGridCtrl->SetRedraw(FALSE);
	m_pGridCtrl->SetRowCount(mVecGTID.size() + 1);
	
	CWorkScheduleHolder *pHolder=m_pDoc->GetTGDataHolder();
	CTG_TrainScheduleBase *pShiftData=NULL, *pWorkData=NULL;
	if(pHolder)
	{
		pShiftData=pHolder->GetShiftScheduleData(); 
		pWorkData=pHolder->GetWorkScheduleData(); 
	}

	for(int i=0; i<mVecGTID.size(); i++)
	{
		ULONG nShiftIndex = mVecGTID[i];
		CTG_SingleTrain* train=NULL;
		if(pShiftData)
		{
			train=pShiftData->GetSingleTrainByShiftIndex(nShiftIndex,NULL_STATION);
		}
		if(train==NULL && pWorkData)
		{
			train=pWorkData->GetSingleTrainByShiftIndex(nShiftIndex,NULL_STATION);
		}
		CString trainId = "";
		if(train)
			trainId = train->GetTrainID();
	
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
		Item.strText.Format("%d",nShiftIndex);
		m_pGridCtrl->SetItem(&Item);            

		m_pGridCtrl->SetItemData(count,0,(DWORD)nShiftIndex);
		count ++;
		
	}
	m_pGridCtrl->SetRedraw(TRUE);
}

void CDlgCrewNotice::initialGridCtrl()
{
	if(m_pGridCtrl == NULL)
		m_pGridCtrl= new CGridCtrl;
	
	
	int col;

	CRect rect2;
	CRect rect;
	CWnd *pWnd;
	pWnd= GetDlgItem(IDC_GRID_CREW);
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
		Item.strText.Format(_gCrewNoticeTitle[col]);
		m_pGridCtrl->SetItem(&Item);
	}
}

BOOL CDlgCrewNotice::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	initialGridCtrl();
	fillGrid();
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgCrewNotice::OnClose()
{
	OnCancel();
}

void CDlgCrewNotice::OnCancel() 
{
	if(IDYES==MessageBox("你确实要忽略 乘务信息变化通知 吗?", "提示", MB_YESNO|MB_ICONQUESTION))
	{
		mVecGTID.clear();
		this->ShowWindow(SW_HIDE);
		return;
	}
}

void CDlgCrewNotice::OnOK() 
{
    CGridCellCheck *pCell=NULL;
	std::vector<ULONG> selgid;
	for(int i=0; i<mVecGTID.size(); i++)
	{
		int nSelRow=i+1;
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(nSelRow,2);
		if(pCell)
		{	
			if(pCell->GetCheck())
			{
				DWORD key = (DWORD)m_pGridCtrl->GetItemData(nSelRow,0);
				selgid.push_back(key);
			}
		}
	}
	if(selgid.size()<=0)
	{
		MessageBox("请选择要更新乘务信息的列车!", "提示", MB_OK|MB_ICONQUESTION);
		return;
	}
	m_pDoc->RequestCrewInform(selgid);
	mVecGTID.clear();
	this->ShowWindow(SW_HIDE);
}

void CDlgCrewNotice::OnButtonSelall() 
{
    CGridCellCheck *pCell;

	for(int i=0; i<mVecGTID.size(); i++)
	{
		pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(i+1,2);
		if(pCell)
		{
			pCell->SetCheck(TRUE);
		}
	}
}

void CDlgCrewNotice::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(1010==nIDEvent)
	{
		if(GetTGFunctionConfig()->IsCrewPlaySound() && mVecGTID.size()>0)
		{
			if(m_nSoundCout%4==0)
			{
				CString path;
				path.Format("%s\\Sound\\alarm\\乘务信息更新.wav", gpcdef.GetDataPathName()); 
				PlaySound(path, NULL, SND_FILENAME|SND_ASYNC);
				m_nSoundCout=1;
			}
			else
			{
				m_nSoundCout++;
			}
		}
		else
		{
			KillTimer(1010);
		}
	}
	CDialog::OnTimer(nIDEvent);
}
