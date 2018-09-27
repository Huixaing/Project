#include "stdafx.h"
#include <io.h>
#include "propertypage_subao.h"
#include "cadstruct.h"
#include "tgdocument.h"
#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"
#include ".\propertypage_subao.h"


extern CEdit* g_curEdit;//当前常用词汇要输入的编辑框
/////////////
TRAIN_SUBAO_NEW TRAINS[512];   //列车编组信息
TRAIN_SUBAO_NEW TRAIN_T;       //修改缓冲区
/////////////
IMPLEMENT_DYNCREATE(CPageSUBAO, CPropertyPage)

#define ITEMNUM   12

#define OPER_SUBAO_INSERT 0x00
#define OPER_SUBAO_DELETE 0x01
#define OPER_SUBAO_CHANGE 0x02  //
#define OPER_SUBAO_CHANGE_BASIC_DATA 0x03 //修改基本数据
#define OPER_SUBAO_STPC  0x04   //No use for gpc;

static int GetTrainDirection(int TrainNum);
static int percolumn=0;
static int sortFlag=0;

CPageSUBAO::CPageSUBAO() : CPropertyPage(CPageSUBAO::IDD)
{
	//{{AFX_DATA_INIT(CPageSUBAO)
	m_train_leader_name = _T("");
	m_train_id = _T("");
	m_total_count = 0;
	m_tail_no = _T("");
	m_ck_count = 0;
	m_cover = _T("");
	m_cz_count = 0;
	m_dest = _T("");
	m_driver_name = _T("");
	m_gk_count = 0;
	m_gz_count = 0;
	m_k_count = 0;
	m_o_count = 0;
	m_pik_count = 0;
	m_piz_count = 0;
	m_pk_count = 0;
	m_pz_count = 0;
	m_s_count = 0;
	m_train_id_f = _T("");
	m_find_result = _T("");
	m_cal_len = 0.0f;
	m_total_weight = 0.0f;
	m_rowdatachanged = FALSE;	//没有数据发生改变
	m_previousrow	= -1;		//
	m_engine_id = _T("");
	m_traction_engine1 = _T("");
	m_traction_engine2 = _T("");
	m_attach_engine1 = _T("");
	m_attach_engine2 = _T("");
	m_nFixCols = 1;
	m_nFixRows = 1;
	m_nCols = ITEMNUM;
	m_nRows = sizeof(TRAINS)/sizeof(TRAINS[0]);
	m_pGridCtrl = NULL;
	m_nCurrentRow = 0; //指向控件中本来的行号，在查询速报的时候要定位到该行的下一行
	m_attach_engine3 = _T("");
	m_attach_engine4 = _T("");
	//}}AFX_DATA_INIT
	m_pDoc = NULL;
	schd_type = WORK_SCHEDULE;
	m_nSelectedSectionID=-1;
	m_nDefaultSectionID=-1;
	m_nCurrentRow=1;          
	m_nDirection=0;
	m_nUpCounts=0;           
	m_nDownCounts=0;         
	m_nTrainCounts=0;
	m_bReadOnly=FALSE;
	m_pNotifyWindow=NULL;
	g_curEdit=NULL;
}

CPageSUBAO::~CPageSUBAO()
{
	if(m_pGridCtrl != NULL)
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
}

void CPageSUBAO::OnDestroy()
{
	if(m_pGridCtrl != NULL)
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
	CPropertyPage::OnDestroy();
}

void CPageSUBAO::SetDocument(CTgDocument * pdoc)
{
	m_pDoc=pdoc;
	m_bReadOnly=pdoc->IsQueryMode();
	select_shift_id=pdoc->GetCurrentShiftID(); 
}

void CPageSUBAO::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageSUBAO)
	DDX_Control(pDX, IDC_CMB_SHIFTID, m_cmbShiftId);
	DDX_Control(pDX, IDC_STATIC1, m_Frame1);
	DDX_Control(pDX, IDC_COMBO_DIRECTIONS, m_CboDirectionsCtrl);
	DDX_Control(pDX, IDC_COMBO_DDTNAME, m_CboDdtCtrl);
	DDX_Control(pDX, IDC_COMBO_DIRECTION, m_CboDirectionCtrl);
	
	DDX_Text(pDX, IDC_TRAIN_ID, m_train_id);
	DDV_MaxChars(pDX, m_train_id, 9);
	DDX_Text(pDX, IDC_TOTAL_COUNT, m_total_count);
	DDX_Text(pDX, IDC_TAIL_NO, m_tail_no);
	DDV_MaxChars(pDX, m_tail_no, 9);
	DDX_Text(pDX, IDC_CK_COUNT, m_ck_count);
	DDX_Text(pDX, IDC_COVER, m_cover);
	DDV_MaxChars(pDX, m_cover, 9);
	DDX_Text(pDX, IDC_CZ_COUNT, m_cz_count);
	DDX_Text(pDX, IDC_DEST, m_dest);
	DDV_MaxChars(pDX, m_dest, 19);
	
	DDX_Text(pDX, IDC_GK_COUNT, m_gk_count);
	DDX_Text(pDX, IDC_GZ_COUNT, m_gz_count);
	DDX_Text(pDX, IDC_K_COUNT, m_k_count);
	DDX_Text(pDX, IDC_O_COUNT, m_o_count);
	DDX_Text(pDX, IDC_PIK_COUNT, m_pik_count);
	DDX_Text(pDX, IDC_PIZ_COUNT, m_piz_count);
	DDX_Text(pDX, IDC_PK_COUNT, m_pk_count);
	DDX_Text(pDX, IDC_PZ_COUNT, m_pz_count);
	DDX_Text(pDX, IDC_S_COUNT, m_s_count);
	DDX_Text(pDX, IDC_CAL_LEN, m_cal_len);
	DDV_MinMaxFloat(pDX, m_cal_len, 0.f, 1.e+007f);
	DDX_Text(pDX, IDC_TOTAL_WEIGHT, m_total_weight);
	DDV_MinMaxFloat(pDX, m_total_weight, 0.f, 1.e+007f);
	DDX_Text(pDX, IDC_ENGINE_ID, m_engine_id);
	DDV_MaxChars(pDX, m_engine_id, 9);

	DDX_Text(pDX, IDC_DRIVER_NAME, m_driver_name);
	DDV_MaxChars(pDX, m_driver_name, 12);
	DDX_Text(pDX, IDC_TRAIN_LEADER_NAME, m_train_leader_name);
	DDV_MaxChars(pDX, m_train_leader_name, 12);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPageSUBAO, CPropertyPage)
	//{{AFX_MSG_MAP(CPageSUBAO)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_DIRECTION_UP, OnDirectionUp)
	ON_BN_CLICKED(IDC_DIRECTION_DOWN, OnDirectionDown)
	ON_BN_CLICKED(IDC_DIRECTION_ALL, OnDirectionAll)

    ON_WM_TIMER()
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_FIND_TRAIN, OnFindTrain)
	ON_EN_KILLFOCUS(IDC_K_COUNT, OnKillfocusKCount)
	ON_EN_KILLFOCUS(IDC_PK_COUNT, OnKillfocusPkCount)
	ON_EN_KILLFOCUS(IDC_PZ_COUNT, OnKillfocusPzCount)
	ON_EN_KILLFOCUS(IDC_CK_COUNT, OnKillfocusCkCount)
	ON_EN_KILLFOCUS(IDC_CZ_COUNT, OnKillfocusCzCount)
	ON_EN_KILLFOCUS(IDC_PIK_COUNT, OnKillfocusPikCount)
	ON_EN_KILLFOCUS(IDC_PIZ_COUNT, OnKillfocusPizCount)
	ON_EN_KILLFOCUS(IDC_GK_COUNT, OnKillfocusGkCount)
	ON_EN_KILLFOCUS(IDC_GZ_COUNT, OnKillfocusGzCount)
	ON_EN_KILLFOCUS(IDC_O_COUNT, OnKillfocusOCount)
	ON_EN_KILLFOCUS(IDC_S_COUNT, OnKillfocusSCount)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)

	ON_CBN_SELENDOK(IDC_COMBO_DDTNAME, OnSelendokComboDdtname)
	ON_WM_CHAR()
	ON_CBN_SELENDOK(IDC_COMBO_DIRECTIONS, OnSelendokComboDirections)
	ON_CBN_SELCHANGE(IDC_CMB_SHIFTID, OnSelchangeCmbShiftid)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GRID, OnGridItemChanged)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID, OnGridStartEdit)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit) //LVN_ITEMCHANGING
	ON_EN_SETFOCUS(IDC_TRAIN_ID, OnSetfocusTrainId)
	ON_EN_SETFOCUS(IDC_TRAIN_ID_F, OnSetfocusTrainIdF)
	ON_EN_SETFOCUS(IDC_ENGINE_TYPE, OnSetfocusEngineType)
	ON_EN_SETFOCUS(IDC_DRIVER_NAME, OnSetfocusDriverName)
	ON_EN_SETFOCUS(IDC_TRAIN_LEADER_NAME, OnSetfocusTrainLeaderName)
	
	ON_BN_CLICKED(IDC_BTN_IMPORT, OnBtnImport)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PRINT_GROUP, OnPrintGroup)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_MESSAGE(UM_TG_TRAIN_SELECTED,OnSchdTrainSelected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageSUBAO message handlers
/////////////////////////////////////////////////////////////////////////////
// CPropertySheetNotepad
LRESULT CPageSUBAO::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;

	if(pMsgStruct->msg_id==MSGID_MsgSubaoFormat)
	{
		ProcessMessage(( CLBTH::MsgSubaoFormat *)pMsgStruct->pmsg);
	}

	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

void CPageSUBAO::OnGridItemChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	m_nCurrentRow=pItem->iRow;                              
	ToFocusRow(m_nCurrentRow);

	CGridCellBase* pCell=m_pGridCtrl->GetCell(m_nCurrentRow,0);
	if(pCell==NULL)
	{
		m_nCurrentRow=0;
		return;
	}

	TRAIN_INDEX tidx = m_pGridCtrl->GetCell(m_nCurrentRow,0)->GetData();
	if(m_pNotifyWindow)
		m_pNotifyWindow->PostMessage(UM_TG_TRAIN_SELECTED,(WPARAM)tidx,0);
	  
	int j = GetSubaoArrayIndexByRowNo(m_nCurrentRow);
	if(j<0)
		return;   
	ShowTrainInEdits(j);
	*pResult = (1)? -1 : 0;
}
void CPageSUBAO::OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	TRACE(_T("Start Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	*pResult = (1)? -1 : 0;  

}

void CPageSUBAO::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{   
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	TRACE("End Edit on row %d, col %d\n", pItem->iRow, pItem->iColumn);

	*pResult = (1)? -1 : 0;      
}


BOOL CPageSUBAO::OnInitDialog() 
{   
	CPropertyPage::OnInitDialog();

	m_edittemp1.SubclassDlgItem( IDC_TRAIN_ID, this );
	m_edittemp2.SubclassDlgItem( IDC_ENGINE_TYPE, this );
	m_edittemp3.SubclassDlgItem( IDC_DRIVER_NAME, this );
	m_edittemp4.SubclassDlgItem( IDC_TRAIN_LEADER_NAME, this );

	InitIdentify();
	m_nSelectedSectionID=m_pDoc->GetCurrentSectionNo();
	m_nDefaultSectionID=m_nSelectedSectionID; 

	int myYear  = select_shift_id/1000000;
	int myMonth = select_shift_id/10000 - myYear*100;
	int myDay   = select_shift_id/100 - (select_shift_id/10000)*100;
	int myBanbie = select_shift_id - (select_shift_id/10)*10;

	CTime t = CTime::GetCurrentTime( );
	long m_myCurrentTime;
	if(myBanbie == 1)
		m_myCurrentTime = (18 * 60 + 0) * 60 + 0;
	else
		m_myCurrentTime = (6 * 60 + 0) * 60 + 0;

	long m_myCurrentDate = (myYear - 1990) * 380 + (myMonth - 1) * 31
		+ myDay - 1;

	long next_date = OsGetNextDate(PMTG.ThreadId,m_myCurrentDate);
	long prev_date = OsGetPrevDate(PMTG.ThreadId,m_myCurrentDate);
	if(myBanbie == 1){
		CTime t = CTime(long_time(prev_date,m_myCurrentTime));
		prev_shift_id = t.GetYear ()*1000000 + t.GetMonth ()*10000
			+ t.GetDay()*100 + 2;
		next_shift_id = select_shift_id + 1;
	}else{
		CTime t = CTime(long_time(next_date,m_myCurrentTime));
		prev_shift_id = select_shift_id - 1;
		next_shift_id = t.GetYear ()*1000000 + t.GetMonth ()*10000
			+ t.GetDay()*100 + 1;
	}
	curr_shift_id = select_shift_id;

	m_cmbShiftId.InsertString(0,ConvertShiftIdToString(prev_shift_id));
	m_cmbShiftId.InsertString(1,ConvertShiftIdToString(select_shift_id));
	m_cmbShiftId.InsertString(2,ConvertShiftIdToString(next_shift_id));
	m_cmbShiftId.SetCurSel(1);

	m_nCurrentRow=-2;           
	m_nDirection=2;            
	char *  ddt[64];         
	int nSectionNum,nSectionList[64];
	nSectionNum = sizeof(nSectionList) / sizeof(nSectionList[0]);
	gpcdef.GetSectionList(nSectionNum,nSectionList);	
	for (int i = 0; i < nSectionNum; i++)    
	{
		ddt[i] = gpcdef.GetSectionName(nSectionList[i]);
		m_CboDdtCtrl.InsertString (i,_T(ddt[i]) );   
		if(m_nDefaultSectionID==nSectionList[i])               
			m_CboDdtCtrl.SetCurSel(i);
	}
	m_CboDdtCtrl.EnableWindow(FALSE);

	int sectionNo=m_nDefaultSectionID;
	memset(nPlotItemList, 0, sizeof(nPlotItemList));
	nPlotItemNum=gpcdef.GetSectionPlotItemList(sectionNo, nPlotItemList, sizeof(nPlotItemList)/sizeof(nPlotItemList[0]));  
	if(nPlotItemNum>=10)
		nPlotItemNum=9;
	
	nPlotItemNum+=1;
	nPlotItemList[nPlotItemNum-1]=nPlotItemNum-1;

	for( i=0;i<nPlotItemNum-1;i++)
	{ 	 
		m_CboDirectionsCtrl.InsertString( i,_T(gpcdef.GetPlotItemName(nPlotItemList[i])) );
	}  
	m_CboDirectionsCtrl.InsertString(i,"全部");
	m_CboDirectionsCtrl.SetCurSel(i);

	CButton  *pWnd;
	pWnd = (CButton *)GetDlgItem(IDC_DIRECTION_ALL);
	if(pWnd==NULL) 
		return FALSE;
	pWnd->SetCheck(TRUE);

	//print flag
	CComboBox *pComboBox;
	pComboBox= (CComboBox *)GetDlgItem(IDC_PRINT_FLAG);
	if(pComboBox == NULL)
		;
	else
		pComboBox->SetCurSel(0);

	TCHAR title[ITEMNUM][8]={_T("序号"), _T("车次"),_T("机车1"),_T("机车2"),_T("附挂1"),_T("司机"),_T("车长"),
		_T("辆数"),_T("总重"),_T("计长"),_T("上报站"),_T("打印")};


	if(m_pGridCtrl!=NULL)
		return FALSE;
	m_pGridCtrl=new CGridCtrl;
	if(!m_pGridCtrl) return FALSE;


	CRect rc;
	m_Frame1.GetWindowRect(rc);
	ScreenToClient(rc);
	
	m_pGridCtrl->Create (rc,this,IDC_GRID,WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

	m_pGridCtrl->SetEditable(FALSE);

	try{
		m_pGridCtrl->SetRowCount(m_nRows);              
		m_pGridCtrl->SetColumnCount(m_nCols);            
		m_pGridCtrl->SetFixedRowCount(m_nFixRows);        
		m_pGridCtrl->SetFixedColumnCount(m_nFixCols);     


		m_pGridCtrl->SetFixedTextColor(RGB(0,0,0));       
		//m_pGridCtrl->SetFixedBkColor(RGB(192,192,192));    
		m_pGridCtrl->SetGridLineColor(RGB(166,202,240));       
		m_pGridCtrl->SetFixedBkColor(RGB(255,242,240));
		//RGB(166,202,240
	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	DWORD dwTextStyle = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
#ifndef _WIN32_WCE
	dwTextStyle |= DT_END_ELLIPSIS;
#endif

	int col;
	for ( col = 0; col < ITEMNUM; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = col;  
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;  //DT_LEFT
		Item.strText.Format(title[col]);
		m_pGridCtrl->SetItem(&Item);
	}
	m_pGridCtrl->ExpandColumnsToFit();
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgSubaoFormat,0);
	GetDlgItem(IDC_TRAIN_ID)->EnableWindow(FALSE);

	CRect  locorect;
	GetDlgItem(IDC_TRACTION_ENGINE1)->GetWindowRect(&locorect);
	ScreenToClient(&locorect);
	locorect.DeflateRect(1,1); 
	m_locoInputEngine1.CreateMe(this,locorect,m_nDefaultSectionID);

	GetDlgItem(IDC_TRACTION_ENGINE2)->GetWindowRect(&locorect);
	ScreenToClient(&locorect);
	locorect.DeflateRect(1,1);
	m_locoInputEngine2.CreateMe(this,locorect,m_nDefaultSectionID);

	GetDlgItem(IDC_ATTACH_ENGINE1)->GetWindowRect(&locorect);
	ScreenToClient(&locorect);
	locorect.DeflateRect(1,1);
	m_locoInputAttachEngine1.CreateMe(this,locorect,m_nDefaultSectionID);

	GetDlgItem(IDC_ATTACH_ENGINE2)->GetWindowRect(&locorect);
	ScreenToClient(&locorect);
	locorect.DeflateRect(1,1);
	m_locoInputAttachEngine2.CreateMe(this,locorect,m_nDefaultSectionID);

	GetDlgItem(IDC_ATTACH_ENGINE3)->GetWindowRect(&locorect);
	ScreenToClient(&locorect);
	locorect.DeflateRect(1,1);
	m_locoInputAttachEngine3.CreateMe(this,locorect,m_nDefaultSectionID);

	GetDlgItem(IDC_ATTACH_ENGINE4)->GetWindowRect(&locorect);
	ScreenToClient(&locorect);
	locorect.DeflateRect(1,1);
	m_locoInputAttachEngine4.CreateMe(this,locorect,m_nDefaultSectionID);

	Query_Subao(select_shift_id);

	percolumn = -1;
	sortFlag = -1;
	
	return TRUE;
}

void CPageSUBAO::OnOK() 
{   
}
BOOL CPageSUBAO::OnApply() 
{
	return CPropertyPage::OnApply();
}

void CPageSUBAO::OnAdd() 
{
	if(m_bReadOnly)
		return;
	if(m_nDefaultSectionID!=m_nSelectedSectionID)
	{
		AfxMessageBox("您没有修改权限,请更换调度台！");
		return;
	}

	Calculate();
	m_train_id = ChangeAlpha(m_train_id);
	if(m_train_id.GetLength()==0)// || m_nCurrentTrainIndex==0)  
	{
		AfxMessageBox("请从计划信息列表中选择车次号!");
		GetDlgItem(IDC_TRAIN_ID)->SetFocus();
		return;
	}

	//2004-05-19
	memset(&TRAIN_T,0,sizeof(TRAIN_T));

	TRAIN_T.input_time=GetTimeidx();    //time idx
	//TRAIN_T.train_idx=0;
	TRAIN_T.Con_id=m_nSelectedSectionID;            //ddtid idx
	TRAIN_T.Subao_type=nPlotItemList[m_CboDirectionCtrl.GetCurSel()];

	m_train_id = ChangeAlpha(m_train_id);
	sprintf(TRAIN_T.Train_id,"%s",m_train_id);

	//print flag
	CComboBox *pComboBox;
	pComboBox= (CComboBox *)GetDlgItem(IDC_PRINT_FLAG);
	if(pComboBox == NULL)
		TRAIN_T.Print_flag = 0;
	else
		TRAIN_T.Print_flag = pComboBox->GetCurSel( );

	sprintf(TRAIN_T.Dest,"%s",m_dest);

	/////////////////// 20090703 //////////////////
	TRAIN_T.train_idx=m_nCurrentTrainIndex;
	const char* ptr=m_locoInputEngine1.GetLocoNo();
	if(ptr==NULL)
		return;
	m_traction_engine1=ptr;

	ptr=m_locoInputEngine2.GetLocoNo();
	if(ptr==NULL)
		return;
	m_traction_engine2=ptr;

	ptr=m_locoInputAttachEngine1.GetLocoNo();
	if(ptr==NULL)
		return;
	m_attach_engine1=ptr;

	ptr=m_locoInputAttachEngine2.GetLocoNo();
	if(ptr==NULL)
		return;
	m_attach_engine2=ptr;

	ptr=m_locoInputAttachEngine3.GetLocoNo();
	if(ptr==NULL)
		return;
	m_attach_engine3=ptr;

	ptr=m_locoInputAttachEngine4.GetLocoNo();
	if(ptr==NULL)
		return;
	m_attach_engine4=ptr;


	///////////////////////////////////////////////

	//2004-05-19 如果没有输入机车号，使用牵引机车号
	if(m_engine_id.IsEmpty())
		m_engine_id = m_traction_engine1;
	sprintf(TRAIN_T.Engine_id[0],"%s",m_engine_id);

	sprintf(TRAIN_T.Driver_name,"%s",m_driver_name);
	sprintf(TRAIN_T.Train_leader_name,"%s",m_train_leader_name);

	TRAIN_T.K_count=m_k_count;
	TRAIN_T.PK_count=m_pk_count;
	TRAIN_T.PZ_count=m_pz_count;
	TRAIN_T.CK_count=m_pk_count;
	TRAIN_T.CZ_count=m_pz_count;
	TRAIN_T.PIK_count=m_pik_count;
	TRAIN_T.PIZ_count=m_piz_count;
	TRAIN_T.GK_count=m_gk_count;
	TRAIN_T.GZ_count=m_gz_count;
	TRAIN_T.O_count=m_o_count;
	TRAIN_T.S_count=m_s_count;
	TRAIN_T.Total_count=m_total_count;
	int tmp;
	tmp = m_cal_len*100.0;
	if(tmp%10 != 0)
		tmp++;
	TRAIN_T.Cal_len= (int)tmp/10;

	tmp = m_total_weight*100.0;
	if(tmp%10 != 0)
		tmp++;
	TRAIN_T.Total_weight=(int)tmp/10;
	sprintf(TRAIN_T.Cover,"%s",m_cover);
	sprintf(TRAIN_T.Tail_no,"%s",m_tail_no);


	//2004-05-11 增加牵引，附挂机车

	ushort enginecount = 0;
	ushort engineindex = 0;

	//两个牵引机车
	if(!m_traction_engine1.IsEmpty())
		enginecount++;
	if(!m_traction_engine2.IsEmpty())
		enginecount++;

	TRAIN_T.tract_engine_count =  enginecount; //牵引机车个数

	if(!m_traction_engine1.IsEmpty())
	{
		strcpy((char*)TRAIN_T.tract_engine_id[engineindex],(char*)(LPCTSTR)m_traction_engine1); 
		SetTractionEngineExt(TRAIN_T.tract_engine_type[engineindex]);
		engineindex++;		
	}
	/*	else
	{
	TRAIN_T.tract_engine_id[engineindex][0] = '\0';
	SetTractionEngineExt(TRAIN_T.tract_engine_type[engineindex]);
	}
	*/
	if(!m_traction_engine2.IsEmpty())
	{
		strcpy((char*)TRAIN_T.tract_engine_id[engineindex],(char*)(LPCTSTR)m_traction_engine2); 
		SetTractionEngineExt(TRAIN_T.tract_engine_type[engineindex]);
		engineindex++;		
	}
	/*	else
	{
	TRAIN_T.tract_engine_id[engineindex][0] = '\0';
	SetTractionEngineExt(TRAIN_T.tract_engine_type[engineindex]);
	}
	*/
	//附挂机车的个数
	enginecount = 0;
	engineindex = 0;
	if(!m_attach_engine1.IsEmpty())
		enginecount++;
	if(!m_attach_engine2.IsEmpty())
		enginecount++;
	if(!m_attach_engine3.IsEmpty())
		enginecount++;
	if(!m_attach_engine4.IsEmpty())
		enginecount++;

	TRAIN_T.attach_engine_count =  enginecount; ////附挂机车的个数

	TRAIN_T.engine_count = TRAIN_T.tract_engine_count + TRAIN_T.attach_engine_count ;//总机车个数= 牵引机车+附挂机车

	if(!m_attach_engine1.IsEmpty())
	{
		strcpy((char*)TRAIN_T.attach_engine_id[engineindex],(char*)(LPCTSTR)m_attach_engine1); 
		SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
		engineindex++;		
	}
	/*	else
	{
	TRAIN_T.attach_engine_id[engineindex][0] = '\0';
	SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
	}
	*/
	if(!m_attach_engine2.IsEmpty())
	{
		strcpy((char*)TRAIN_T.attach_engine_id[engineindex],(char*)(LPCTSTR)m_attach_engine2); 
		SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
		engineindex++;		
	}
	/*	else
	{
	TRAIN_T.attach_engine_id[engineindex][0] = '\0';
	SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
	}
	*/
	if(!m_attach_engine3.IsEmpty())
	{
		strcpy((char*)TRAIN_T.attach_engine_id[engineindex],(char*)(LPCTSTR)m_attach_engine3); 
		SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
		engineindex++;		
	}
	/*	else
	{
	TRAIN_T.attach_engine_id[engineindex][0] = '\0';
	SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
	}
	*/	
	if(!m_attach_engine4.IsEmpty())
	{
		strcpy((char*)TRAIN_T.attach_engine_id[engineindex],(char*)(LPCTSTR)m_attach_engine4); 
		SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
		engineindex++;		
	}
	/*	else
	{
	TRAIN_T.attach_engine_id[engineindex][0] = '\0';
	SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
	}
	*/	
	TRAIN_T.flag = 0;
	TRAIN_T.note[0]='\0';


	//insert a new subao
	Set_Subao(GetTimeidx(), CLBTH::MsgSubaoFormat::SUBAO_GPC_INSERT);//OPER_SUBAO_INSERT);	   

	//get updated subao
	Sleep(200);
	Query_Subao(select_shift_id);   

}

void CPageSUBAO::OnDelete() 
{  
	if(m_bReadOnly)
		return;

	if(m_nDefaultSectionID!=m_nSelectedSectionID)
	{	AfxMessageBox("您没有删除权限，请更换调度台！");
	return ;
	}

	int nRow = m_pGridCtrl->GetFocusCell().row;   

	int i = GetSubaoArrayIndexByRowNo(m_nCurrentRow);

	if(i<0)
		return ;   
	if(AfxMessageBox("要删除此条?",MB_YESNO | MB_DEFBUTTON2 |MB_ICONQUESTION) != IDYES  )
		return; 
	memcpy(&TRAIN_T,&TRAINS[i],sizeof(TRAIN_T));
	Set_Subao(GetTimeidx_SelectedRow(m_nCurrentRow), CLBTH::MsgSubaoFormat::SUBAO_GPC_DELETE);//OPER_SUBAO_DELETE);

	m_nTrainCounts = 0;
	OnSelendokComboDirections();

	Query_Subao(select_shift_id);   

}

BOOL CPageSUBAO::Show_Subao(int num,int row)
{   
	TCHAR buffer[50];

	for (int  col = 0; col < ITEMNUM; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
		Item.row = row;  
		Item.col = col;  
		Item.lParam =TRAINS[num].train_idx; 
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS; //DT_LEFT
		switch(col)
		{
		case 0:
			sprintf(buffer,"%d",row);
			break;
		case 1:
			sprintf(buffer,"%s",TRAINS[num].Train_id);
			break;
		case 2:
			sprintf(buffer,"%s", gpcdef.GetEngineDesp(TRAINS[num].tract_engine_id[0]));
			break;
		case 3:
			sprintf(buffer,"%s", gpcdef.GetEngineDesp(TRAINS[num].tract_engine_id[1]));
			break;
		case 4:
			sprintf(buffer,"%s", gpcdef.GetEngineDesp(TRAINS[num].attach_engine_id[0]));
			break;
		case 5:
			sprintf(buffer,"%s",TRAINS[num].Driver_name);
			break;
		case 6:
			sprintf(buffer,"%s",TRAINS[num].Train_leader_name);
			break;
		case 7:
			sprintf(buffer,"%d",TRAINS[num].K_count);
			break;
		case 8:
			sprintf(buffer,"%.1f",(float)TRAINS[num].Total_weight/10);
			break;
		case 9:
			sprintf(buffer,"%.1f",(float)TRAINS[num].Cal_len/10);
			break;
		case 10:
			{
				CString stName = gpcdef.GetStationName(TRAINS[num].report_station);
				if(stName != "管外")
					sprintf(buffer,"%s",gpcdef.GetStationName(TRAINS[num].report_station));
				else
					buffer[0] = '\0';
			}
			break;
		case 11:
			if(TRAINS[num].Print_flag == 0)
				sprintf(buffer,"是");
			else
				sprintf(buffer,"否");
			break;
		default:
			break;
		}

		Item.strText.Format(buffer);
		ASSERT_VALID(m_pGridCtrl);
		m_pGridCtrl->SetItem(&Item);
	} 

	return TRUE;
}

BOOL CPageSUBAO::Query_Subao(unsigned long shift_id)
{   
	CLBTH::MsgSubaoQuery mMsgSubaoQuery;
	CTG_ScheduleInfo info;
	m_pDoc->GetScheduleInfo(info);
	mMsgSubaoQuery.shift_id = shift_id;
	mMsgSubaoQuery.con_id =  info.m_nSectionID;
	mMsgSubaoQuery.schd_type = CTG_ScheduleInfo::SCHEDULE_TYPE_WORK;//info.m_nType;
	mMsgSubaoQuery.train_index = -1;
	mMsgSubaoQuery.query_filter = CLBTH::MsgSubaoQuery::SUBAO_QUERY_BY_SHIFTID;

	m_nUpCounts=0;
	m_nDownCounts=0;
	//	m_nFlag=1;                    //ok to receive subao
	m_nTrainCounts=0;          
	m_pGridCtrl->DeleteNonFixedRows();
	////////// 本地数据处理 CUIHU 2008.11
	//if(DB_QueryTrainSubaoInfo(&mMsgSubaoQuery,GetParent()->GetSafeHwnd()))
	//	return TRUE;
	long eid=GetSpecialEntityID(SERVER_TYPE_TG,mMsgSubaoQuery.con_id);
	SendTGBMsg(mMsgSubaoQuery.con_id,&mMsgSubaoQuery,eid);	

	GpcLogTrainMessage(0, 0, "[按照班次请求速报]: svr_entity %d con_id %d shift_id %d schd_type %d query_filter %d",
		eid, 
		mMsgSubaoQuery.con_id, 
		mMsgSubaoQuery.shift_id,
		mMsgSubaoQuery.schd_type,
		mMsgSubaoQuery.query_filter);

	return TRUE;
}

void CPageSUBAO::ClearEdit()
{
	m_nCurrentTrainIndex=0;
	m_train_id = _T("");
	m_dest = _T("");
	m_engine_id = _T("");
	m_driver_name = _T("");
	m_train_leader_name = _T("");
	m_k_count = 0;
	m_o_count = 0;
	m_total_count = 0;
	m_s_count = 0;
	m_cal_len = 0.0f;
	//	m_cal_len = 0;
	m_total_weight = 0.0f;
	m_cover = _T("");
	m_tail_no = _T("");
	m_pk_count = 0;
	m_pz_count = 0;
	m_ck_count = 0;
	m_cz_count = 0;
	m_pik_count = 0;
	m_piz_count = 0;
	m_gk_count = 0;
	m_gz_count = 0;

	m_traction_engine1 = _T("");
	m_traction_engine2 = _T("");

	m_attach_engine1 = _T("");
	m_attach_engine2 = _T("");
	m_attach_engine3 = _T("");
	m_attach_engine4 = _T("");

	UpdateData(FALSE);
}



void CPageSUBAO::OnDirectionUp() 
{
	m_nDirection=0;             
	m_nUpCounts=0;
	ClearEdit();

	m_pGridCtrl->DeleteNonFixedRows();

	for(int i=0;i<m_nTrainCounts;i++)   
	{
		if(!GetTrainDirection(i))
		{  
			m_nUpCounts++;
		}
	}
	m_pGridCtrl->SetRowCount(m_nUpCounts+2);

	m_pGridCtrl->SetRedraw (FALSE);   
	m_nUpCounts=0;
	for(int i=0;i<m_nTrainCounts;i++)   
	{
		if(!GetTrainDirection(i))
		{  
			m_nUpCounts++;
			Show_Subao(i,m_nUpCounts); 
		}
	}

	CRect rect(4,43,700,420);
	SetBlankCells(m_nUpCounts);
	m_pGridCtrl->SetRedraw (TRUE);   

	InvalidateRect(&rect);

	return;
}

//根据车次的最后一位数字来判断车的上下行，上行车为偶数次，下行车是奇数次
int GetTrainDirection(int TrainNum)
{
	CString id,last;
	int result;
	id=TRAINS[TrainNum].Train_id;

	if(id.GetLength()==0)  
		return -1;

	last=id.Right(1);
	//右边第一位不是整数，则去掉继续判断
	while(id.GetLength()>=1  )
	{	
		if( !(last=="0" || last=="1" ||last=="2"|| last=="3" ||last=="4"
			||last=="5" ||last=="6"  ||last=="7"||last=="8"||last=="9") )
		{  	 
			id = id.Left(id.GetLength()-1);
			last=id.Right(1);
		}
		else
			break;
	}

	result=(atoi(last) )%2 ? 1:0;
	
	return result;
}

void CPageSUBAO::OnDirectionDown() 
{
	m_nDirection=1;                   
	m_nDownCounts=0;
	ClearEdit();

	m_pGridCtrl->DeleteNonFixedRows();

	for(int i=0;i<m_nTrainCounts;i++)   
	{
		if(GetTrainDirection(i))  
		{  
			m_nDownCounts++;
		}
	}

	m_pGridCtrl->SetRowCount(m_nDownCounts+2);
	m_pGridCtrl->SetRedraw (FALSE);   
	m_nDownCounts=0;
	for(int i=0;i<m_nTrainCounts;i++)   
	{
		if(GetTrainDirection(i))  
		{  
			m_nDownCounts++;
			Show_Subao(i,m_nDownCounts);
		}
	}

	CRect rect(4,43,700,420);		
	SetBlankCells(m_nDownCounts);
	m_pGridCtrl->SetRedraw (TRUE);    
	InvalidateRect(&rect);

	return;
}


//速报的修改
BOOL CPageSUBAO::Set_Subao(int timeidx,ushort oper)
{
	CTG_ScheduleInfo info;
	m_pDoc->GetScheduleInfo(info);
	if(m_nDefaultSectionID!=m_nSelectedSectionID)
	{	
		MessageBox("您没有修改列车编组的权限！");
		return FALSE;
	}
	CLBTH::MsgSubaoFormat mMsgSubaoFormat;
	mMsgSubaoFormat.con_id =  info.m_nSectionID;
	mMsgSubaoFormat.schd_type = info.m_nType;
	mMsgSubaoFormat.oper =( CLBTH::MsgSubaoFormat::SUBAO_OPER)oper;
	unsigned long shift_id;

	shift_id = select_shift_id;//gpcdef.GetLoginShiftid();


	if(oper==CLBTH::MsgSubaoFormat::SUBAO_GPC_CHANGE_KEY)
	{ 
		int i=GetFocusTrain(GetTimeidx_SelectedRow(m_nCurrentRow));       //获得当前的记录
		if(i<0)  	return FALSE ;   		
		mMsgSubaoFormat.old_train_index =  (TRAINS[i].train_idx);    // train_index     
		mMsgSubaoFormat.old_shift_id =  (shift_id);       //shift_id 		 
		strncpy(mMsgSubaoFormat.old_train_id,TRAINS[i].Train_id,sizeof(mMsgSubaoFormat.old_train_id));//,sizeof(TRAINS[i].Train_id));     //train id ////

		mMsgSubaoFormat.train_index =  (TRAIN_T.train_idx);      
		mMsgSubaoFormat.shift_id = (shift_id);   //shift_id 
		strncpy(mMsgSubaoFormat.train_id,TRAIN_T.Train_id,sizeof(mMsgSubaoFormat.train_id));//,sizeof(TRAIN_T.Train_id));
	}
	else
	{
		mMsgSubaoFormat.old_train_index =  0;    // old train_index     
		mMsgSubaoFormat.old_shift_id =  0;       //old shift_id 		 
		strcpy(mMsgSubaoFormat.old_train_id,""); //oldtrain id 

		mMsgSubaoFormat.train_index =  (TRAIN_T.train_idx);      
		mMsgSubaoFormat.shift_id = (shift_id);   //shift_id 
		strncpy(mMsgSubaoFormat.train_id,TRAIN_T.Train_id,sizeof(mMsgSubaoFormat.train_id));//,sizeof(TRAIN_T.Train_id));

	}


	mMsgSubaoFormat.input_time = (timeidx);    //input_time;

	strncpy(mMsgSubaoFormat.car_des,TRAIN_T.Train_describe,sizeof(TRAIN_T.Train_describe));      // train describe
	strncpy(mMsgSubaoFormat.orig,TRAIN_T.Orig,sizeof(TRAIN_T.Orig));   // train orig
	strncpy(mMsgSubaoFormat.dest,TRAIN_T.Dest,sizeof(TRAIN_T.Dest));// tain destin

	//总个数
	BYTE   engine_count =  TRAIN_T.tract_engine_count + (TRAIN_T.attach_engine_count<<4) ; 

	mMsgSubaoFormat.engine_count =engine_count ;//总个数，已编码过

	//牵引机车数
	int i;
	for( i=0; i<TRAIN_T.tract_engine_count;i++ )
	{		

		mMsgSubaoFormat.subao_engine[i].engine_type  = TRAIN_T.tract_engine_type[i];										//0 engine type char
		memcpy(mMsgSubaoFormat.subao_engine[i].engine_id,	 TRAIN_T.tract_engine_id[i], sizeof(mMsgSubaoFormat.subao_engine[i].engine_id));     // engine id 	
	}

	//附挂机车个数
	for(int j=0; j<TRAIN_T.attach_engine_count;j++ )
	{		

		mMsgSubaoFormat.subao_engine[i+j].engine_type  = TRAIN_T.attach_engine_type[j];										//0 engine type char
		memcpy(mMsgSubaoFormat.subao_engine[i+j].engine_id,	 TRAIN_T.attach_engine_id[j], sizeof(mMsgSubaoFormat.subao_engine[i+j].engine_id));     // engine id 	
	} 


	strncpy(mMsgSubaoFormat.driver_name,TRAIN_T.Driver_name,sizeof(TRAIN_T.Driver_name)-1);//driver name
	mMsgSubaoFormat.driver_name[sizeof(mMsgSubaoFormat.driver_name)]=0;
	strncpy(mMsgSubaoFormat.train_leader_name,TRAIN_T.Train_leader_name, sizeof(TRAIN_T.Train_leader_name)-1);// leader name 
	mMsgSubaoFormat.train_leader_name[sizeof(mMsgSubaoFormat.train_leader_name)]=0;
	strncpy(mMsgSubaoFormat.cover,TRAIN_T.Cover,sizeof(TRAIN_T.Cover)-1);  //m_cover
	mMsgSubaoFormat.cover[sizeof(mMsgSubaoFormat.cover)]=0;
	strncpy(mMsgSubaoFormat.tail_no,TRAIN_T.Tail_no,sizeof(TRAIN_T.Tail_no));  // tail no
	mMsgSubaoFormat.tail_no[sizeof(mMsgSubaoFormat.tail_no)]=0;

	mMsgSubaoFormat.space_rank= TRAIN_T.Space_rank;     //space rank;

	mMsgSubaoFormat.length_rank= TRAIN_T.Length_rank;    //length rank;

	mMsgSubaoFormat.super_rank= TRAIN_T.Super_rank;     //super rank;

	mMsgSubaoFormat.military_rank= TRAIN_T.Military_rank;  //military;

	mMsgSubaoFormat.speed= TRAIN_T.Speed;       //speed limit;

	mMsgSubaoFormat.con_id =  (byte)m_nSelectedSectionID;      //con id;

	mMsgSubaoFormat.e_engine= TRAIN_T.E_engine;    //engine type ;

	mMsgSubaoFormat.print_flag= TRAIN_T.Print_flag;  //print flag;

	mMsgSubaoFormat.pass_cnt = (TRAIN_T.K_count); 

	mMsgSubaoFormat.load_cnt= (TRAIN_T.Load_count); 

	mMsgSubaoFormat.pk_count = (TRAIN_T.PK_count); 

	mMsgSubaoFormat.pz_count = (TRAIN_T.PZ_count); 

	mMsgSubaoFormat.ck_count= (TRAIN_T.CK_count); 

	mMsgSubaoFormat.cz_count= (TRAIN_T.CZ_count); 

	mMsgSubaoFormat.pik_count= (TRAIN_T.PIK_count); 

	mMsgSubaoFormat.piz_count= (TRAIN_T.PIZ_count); 

	mMsgSubaoFormat.gk_count= (TRAIN_T.GK_count); 

	mMsgSubaoFormat.gz_count= (TRAIN_T.GZ_count); 

	mMsgSubaoFormat.o_count= (TRAIN_T.O_count); 

	mMsgSubaoFormat.s_count= (TRAIN_T.S_count); 

	mMsgSubaoFormat.total_count= (TRAIN_T.Total_count); 

	mMsgSubaoFormat.van_count= (TRAIN_T.Van_count); 

	mMsgSubaoFormat.cal_len= (TRAIN_T.Cal_len); 

	mMsgSubaoFormat.total_weight= (TRAIN_T.Total_weight); 

	mMsgSubaoFormat.subao_type= (TRAIN_T.Subao_type); 

	mMsgSubaoFormat.speed= (TRAIN_T.Speed); 

	mMsgSubaoFormat.noutil_cnt= (TRAIN_T.Noutil_cnt); 

	////////// 本地数据处理 CUIHU 2008.11

	//if(DB_UpdateTrainSubaoInfo(&mMsgSubaoFormat,GetParent()->GetSafeHwnd()))
	//	return TRUE;
	long eid=GetSpecialEntityID(SERVER_TYPE_TG,mMsgSubaoFormat.con_id);
	SendTGBMsg(mMsgSubaoFormat.con_id,&mMsgSubaoFormat,eid);	

	GpcLogTrainMessage(0, 0, "[修改速报]: svr_entity %d con_id %d shift_id:%d schd_type %d oper %d",
		eid, 
		mMsgSubaoFormat.con_id, 
		mMsgSubaoFormat.shift_id,
		mMsgSubaoFormat.schd_type,
		mMsgSubaoFormat.oper);
	return TRUE;

}

int CPageSUBAO::GetFocusTrain(int timeidx)
{   
	for(int i=0;i<m_nTrainCounts;i++) {
		if(TRAINS[i].input_time==timeidx)
			return i;
	}
	return -1;
}

void CPageSUBAO::OnNew() 
{
	if(m_nDefaultSectionID!=m_nSelectedSectionID)
	{	AfxMessageBox("您没有修改权限,请更换调度台！");
	return;
	}
	ClearEdit();
	GetDlgItem(IDC_TRAIN_ID)->SetFocus();

}

void CPageSUBAO::SetBlankCells(int Start)
{
	TCHAR buffer[10];
	for (int row = Start+1; row < 100; row++)
		for (int  col = 0; col < ITEMNUM; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT|GVIF_PARAM;
			Item.row = row;  
			Item.col = col;  
			Item.lParam =0;
			Item.nFormat = DT_LEFT|DT_WORDBREAK|DT_NOPREFIX;
			strcpy(buffer,"");
			Item.strText.Format(buffer);
			ASSERT_VALID(m_pGridCtrl);
			m_pGridCtrl->SetItem(&Item);

		}  //for
		// m_pGridCtrl->AutoSizeColumns();
}


unsigned long CPageSUBAO::GetTimeidx()
{
	CTime t=CTime::GetCurrentTime();
	CString result;
	result.Format("%d",t);
	return  atoi(result);
}

void CPageSUBAO::OnFindTrain() 
{
	UpdateData(TRUE);

	m_train_id_f = ChangeAlpha(m_train_id_f);
	if(m_train_id_f.GetLength()==0)  
	{ AfxMessageBox("请输入车次号!");
	GetDlgItem(IDC_TRAIN_ID_F)->SetFocus();
	return;
	}

	int i,j;
	for(i=0;i<m_nTrainCounts;i++)   
	{   
		if(TRAINS[i].Train_id==m_train_id_f)
		{  
			m_CboDirectionsCtrl.SetCurSel(nPlotItemNum);
			for(j=0;j<m_nTrainCounts;j++)   		 
				Show_Subao(j,j+1); 
			//SetBlankCells(m_nTrainCounts);
			ToFocusRow(i+1);            
			ShowTrainInEdits(i);       
			m_find_result="发现列车";
			UpdateData(FALSE);
			return ; 
		}
	}
	m_find_result="没有此列车 ";  
	UpdateData(FALSE);
}

void CPageSUBAO::ToFocusRow(int rowNum)
{ 

	ClearEdit();
	int maxColspan=ITEMNUM-1;

	//commented by Qianguorong 2003-4-10

	//m_pGridCtrl->SetFocusCell(rowNum, 0);   
	m_pGridCtrl->SetSelectedRange(rowNum,0,rowNum,maxColspan, FALSE);
	//m_pGridCtrl->setrow

	//commented by Qianguorong 2003-4-10 ends here

}


unsigned long CPageSUBAO::GetTimeidx_SelectedRow(int rownum)
{
	int sel = m_CboDirectionsCtrl.GetCurSel();
	int ndir=nPlotItemList[sel];   
	int i,num=0;
	if(ndir == nPlotItemNum - 1)
	{
		for(i=0;i<m_nTrainCounts;i++)   
		{ 
			num ++;
			if(num == rownum)
				return TRAINS[i].input_time;
		}
	}
	else
	{
		for(i=0;i<m_nTrainCounts;i++)   
		{
			if(TRAINS[i].Subao_type==ndir)      
			{
				num ++;
				if(num == rownum)
					return TRAINS[i].input_time;
			}  
		}
	}

	return -1;
}

void CPageSUBAO::OnKillfocusKCount() 
{
	Calculate();
}

void CPageSUBAO::Calculate()
{
	g_curEdit=NULL;
	UpdateData(TRUE);
	m_total_count=m_k_count +m_pk_count+m_pz_count+m_pk_count+m_pz_count+m_pik_count+
		m_piz_count+m_gk_count+m_gz_count+m_o_count+m_s_count;
	UpdateData(FALSE);
}

void CPageSUBAO::OnKillfocusPkCount() 
{
	Calculate();	
}

void CPageSUBAO::OnKillfocusPzCount() 
{
	Calculate();	
}

void CPageSUBAO::OnKillfocusCkCount() 
{
	Calculate();	
}

void CPageSUBAO::OnKillfocusCzCount() 
{
	Calculate();	
}

void CPageSUBAO::OnKillfocusPikCount() 
{
	Calculate();	
}

void CPageSUBAO::OnKillfocusPizCount() 
{
	Calculate();	
}

void CPageSUBAO::OnKillfocusGkCount() 
{
	Calculate();	
}

void CPageSUBAO::OnKillfocusGzCount() 
{
	Calculate();

}

void CPageSUBAO::OnKillfocusOCount() 
{
	Calculate();
}

void CPageSUBAO::OnKillfocusSCount() 
{
	Calculate();	
}

//nRow  是在TRAINS数组中的实际位置
void CPageSUBAO::ShowTrainInEdits(int nRow)
{  
	ClearEdit();
	if(nRow<0)
		return;

	int  train_array_index = nRow;
	for(int i=0;i<nPlotItemNum;i++)
	{ 
		if(nPlotItemList[i]==TRAINS[train_array_index].Subao_type)
		{    
			m_CboDirectionCtrl.SetCurSel(i);
			break;
		}
	}
	if(i>= nPlotItemNum)
		m_CboDirectionCtrl.SetCurSel(nPlotItemNum-1);

	m_nCurrentTrainIndex=TRAINS[train_array_index].train_idx; 
	m_train_id = ChangeAlpha(CString(TRAINS[train_array_index].Train_id));

	m_train_id=TRAINS[train_array_index].Train_id;

	m_dest=TRAINS[train_array_index].Dest;
	m_engine_id=TRAINS[train_array_index].Engine_id[0];
	m_driver_name=TRAINS[train_array_index].Driver_name;
	m_train_leader_name=TRAINS[train_array_index].Train_leader_name;
	m_k_count=TRAINS[train_array_index].K_count;
	m_pk_count=TRAINS[train_array_index].PK_count;
	m_pz_count=TRAINS[train_array_index].PZ_count;
	m_ck_count=TRAINS[train_array_index].CK_count;
	m_cz_count=TRAINS[train_array_index].CZ_count;
	m_pik_count=TRAINS[train_array_index].PIK_count;
	m_piz_count=TRAINS[train_array_index].PIZ_count;
	m_gk_count=TRAINS[train_array_index].GK_count;
	m_gz_count=TRAINS[train_array_index].GZ_count;
	m_o_count=TRAINS[train_array_index].O_count;
	m_s_count=TRAINS[train_array_index].S_count;
	m_total_count=TRAINS[train_array_index].Total_count;
	m_cal_len=(float)TRAINS[train_array_index].Cal_len/10;
	m_total_weight=(float)TRAINS[train_array_index].Total_weight/10;
	m_cover=TRAINS[train_array_index].Cover;
	m_tail_no=TRAINS[train_array_index].Tail_no;

	m_traction_engine1=TRAINS[train_array_index].tract_engine_id[0];
	m_traction_engine2=TRAINS[train_array_index].tract_engine_id[1];

	m_attach_engine1=TRAINS[train_array_index].attach_engine_id[0];
	m_attach_engine2=TRAINS[train_array_index].attach_engine_id[1];
	m_attach_engine3=TRAINS[train_array_index].attach_engine_id[2];
	m_attach_engine4=TRAINS[train_array_index].attach_engine_id[3];

	Timeidx_Selected=TRAINS[train_array_index].input_time;   //时间



	//print flag
	CComboBox *pComboBox;
	pComboBox= (CComboBox *)GetDlgItem(IDC_PRINT_FLAG);
	if(pComboBox == NULL)
		;
	else
		pComboBox->SetCurSel(TRAINS[train_array_index].Print_flag);

	UpdateData(FALSE);

	m_locoInputEngine1.SetLocoNo(m_traction_engine1);
	m_locoInputEngine2.SetLocoNo(m_traction_engine2);

	m_locoInputAttachEngine1.SetLocoNo(m_attach_engine1);
	m_locoInputAttachEngine2.SetLocoNo(m_attach_engine2);
	m_locoInputAttachEngine3.SetLocoNo(m_attach_engine3);
	m_locoInputAttachEngine4.SetLocoNo(m_attach_engine4);

	GetDlgItem(IDC_TRAIN_ID)->SetFocus();

}

void CPageSUBAO::OnChange() 
{
	if(m_bReadOnly)
		return;

	//重新设置行号，针对用户按下了回车键的情况
	CCellRange cur_cell = m_pGridCtrl->GetSelectedCellRange();
	if(cur_cell.GetMaxRow() >0)
	{
		m_nCurrentRow = cur_cell.GetMaxRow();
	}
	if(m_nDefaultSectionID!=m_nSelectedSectionID)
	{
		AfxMessageBox("您没有修改列车编组的权限！");
		return ;
	}

	Calculate();
	m_train_id = ChangeAlpha(m_train_id);
	if(m_train_id.GetLength()==0)// || m_nCurrentTrainIndex==0)  
	{
		AfxMessageBox("请从计划信息列表中选择车次号!");
		GetDlgItem(IDC_TRAIN_ID)->SetFocus();
		return;
	}

	bool flag_change_basic = false;
	bool flag_different_train = false;
	unsigned long old_trainindex = 0;

	int i = GetSubaoArrayIndexByRowNo(m_nCurrentRow);			
	if(i<0) 
	{
		OnAdd();
		return ;
	}

	old_trainindex = TRAINS[i].train_idx;


	memcpy(&TRAIN_T,&TRAINS[i],sizeof(TRAIN_T));

	/////////////////// 20090703 //////////////////
	TRAIN_T.train_idx=m_nCurrentTrainIndex;

	const char* ptr=m_locoInputEngine1.GetLocoNo();
	if(ptr==NULL)
		return;
	m_traction_engine1=ptr;

	ptr=m_locoInputEngine2.GetLocoNo();
	if(ptr==NULL)
		return;
	m_traction_engine2=ptr;

	ptr=m_locoInputAttachEngine1.GetLocoNo();
	if(ptr==NULL)
		return;
	m_attach_engine1=ptr;

	ptr=m_locoInputAttachEngine2.GetLocoNo();
	if(ptr==NULL)
		return;
	m_attach_engine2=ptr;

	ptr=m_locoInputAttachEngine3.GetLocoNo();
	if(ptr==NULL)
		return;
	m_attach_engine3=ptr;

	ptr=m_locoInputAttachEngine4.GetLocoNo();
	if(ptr==NULL)
		return;
	m_attach_engine4=ptr;

	///////////////////////////////////////////////
	//2004-05-19
	memset(&TRAIN_T.tract_engine_type[0],0,sizeof(TRAIN_T.tract_engine_type));//牵引机车类型
	memset(&TRAIN_T.tract_engine_id[0],0,sizeof(TRAIN_T.tract_engine_id));//牵引机车号
	memset(&TRAIN_T.attach_engine_type[0],0,sizeof(TRAIN_T.attach_engine_type));//附挂机车类型
	memset(&TRAIN_T.attach_engine_id[0],0,sizeof(TRAIN_T.attach_engine_id));//附挂机车号

	//TRAIN_T.train_idx=0;
	TRAIN_T.Con_id=m_nSelectedSectionID;          

	TRAIN_T.Subao_type = nPlotItemList[m_CboDirectionCtrl.GetCurSel()];   
	sprintf(TRAIN_T.Train_id,"%s",m_train_id);


	if(TRAIN_T.train_idx != old_trainindex)
		flag_change_basic = true;


	//2004-05-12 ends here
	//print flag
	CComboBox *pComboBox;
	pComboBox= (CComboBox *)GetDlgItem(IDC_PRINT_FLAG);
	if(pComboBox == NULL)
		TRAIN_T.Print_flag = 0;
	else
		TRAIN_T.Print_flag = pComboBox->GetCurSel( );

	//TRAIN_T.train_idx=TRAINS[i].train_idx;

	//2004-05-19 如果没有输入机车号，使用牵引机车号
	if(m_engine_id.IsEmpty())
		m_engine_id = m_traction_engine1;

	sprintf(TRAIN_T.Dest,"%s",m_dest);
	sprintf(TRAIN_T.Engine_id[0],"%s",m_engine_id);
	sprintf(TRAIN_T.Driver_name,"%s",m_driver_name);
	sprintf(TRAIN_T.Train_leader_name,"%s",m_train_leader_name);

	TRAIN_T.K_count=m_k_count;
	TRAIN_T.PK_count=m_pk_count;
	TRAIN_T.PZ_count=m_pz_count;
	TRAIN_T.CK_count=m_ck_count;   ////////////  2002.09.06
	TRAIN_T.CZ_count=m_cz_count;   ////////////  2002.09.06
	TRAIN_T.PIK_count=m_pik_count;
	TRAIN_T.PIZ_count=m_piz_count;
	TRAIN_T.GK_count=m_gk_count;
	TRAIN_T.GZ_count=m_gz_count;
	TRAIN_T.O_count=m_o_count;
	TRAIN_T.S_count=m_s_count;
	TRAIN_T.Total_count=m_total_count;
	int tmp = m_cal_len*100;
	if(tmp%10 != 0)
		tmp++;
	TRAIN_T.Cal_len=tmp/10;

	tmp = m_total_weight*100;
	if(tmp%10 != 0)
		tmp++;
	TRAIN_T.Total_weight=tmp/10;

	sprintf(TRAIN_T.Cover,"%s",m_cover);
	sprintf(TRAIN_T.Tail_no,"%s",m_tail_no);

	//2004-05-11 增加牵引，附挂机车

	ushort enginecount = 0;
	ushort engineindex = 0;

	if(!m_traction_engine1.IsEmpty())
		enginecount++;
	if(!m_traction_engine2.IsEmpty())
		enginecount++;

	TRAIN_T.tract_engine_count =  enginecount;

	if(!m_traction_engine1.IsEmpty())
	{
		strcpy((char*)TRAIN_T.tract_engine_id[engineindex],(char*)(LPCTSTR)m_traction_engine1); 
		SetTractionEngineExt(TRAIN_T.tract_engine_type[engineindex]);
		engineindex++;		
	}

	if(!m_traction_engine2.IsEmpty())
	{
		strcpy((char*)TRAIN_T.tract_engine_id[engineindex],(char*)(LPCTSTR)m_traction_engine2); 
		SetTractionEngineExt(TRAIN_T.tract_engine_type[engineindex]);
		engineindex++;		
	}	

	enginecount = 0;
	engineindex = 0;
	if(!m_attach_engine1.IsEmpty())
		enginecount++;
	if(!m_attach_engine2.IsEmpty())
		enginecount++;
	if(!m_attach_engine3.IsEmpty())
		enginecount++;
	if(!m_attach_engine4.IsEmpty())
		enginecount++;

	TRAIN_T.attach_engine_count =  enginecount;

	if(!m_attach_engine1.IsEmpty())
	{
		strcpy((char*)TRAIN_T.attach_engine_id[engineindex],(char*)(LPCTSTR)m_attach_engine1); 
		SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
		engineindex++;		
	}
	/*	else
	{
	TRAIN_T.attach_engine_id[engineindex][0] = '\0';
	SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
	}
	*/
	if(!m_attach_engine2.IsEmpty())
	{
		strcpy((char*)TRAIN_T.attach_engine_id[engineindex],(char*)(LPCTSTR)m_attach_engine2); 
		SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
		engineindex++;		
	}


	if(!m_attach_engine3.IsEmpty())
	{
		strcpy((char*)TRAIN_T.attach_engine_id[engineindex],(char*)(LPCTSTR)m_attach_engine3); 
		SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
		engineindex++;		
	}

	if(!m_attach_engine4.IsEmpty())
	{
		strcpy((char*)TRAIN_T.attach_engine_id[engineindex],(char*)(LPCTSTR)m_attach_engine4); 
		SetAttachEngineExt(TRAIN_T.attach_engine_type[engineindex]);
		engineindex++;		
	}

	TRAIN_T.flag = 0;
	TRAIN_T.note[0]='\0';


	if(m_nCurrentRow == (m_pGridCtrl->GetRowCount() - m_pGridCtrl->GetFixedRowCount()))//行号为末尾行号，新增一行数据
		Set_Subao(GetTimeidx(), CLBTH::MsgSubaoFormat::SUBAO_GPC_INSERT);
	else if(GetTimeidx_SelectedRow(m_nCurrentRow) <= 0)
	{
		m_nCurrentRow = m_pGridCtrl->GetRowCount() - m_pGridCtrl->GetFixedRowCount();
		if(flag_change_basic)
			Set_Subao(GetTimeidx(),CLBTH::MsgSubaoFormat::SUBAO_GPC_CHANGE_KEY);
		else
			Set_Subao(GetTimeidx(),CLBTH::MsgSubaoFormat::SUBAO_GPC_CHANGE_MISC);
	}
	else
	{ 
		if(flag_change_basic)
			Set_Subao(GetTimeidx_SelectedRow(m_nCurrentRow), CLBTH::MsgSubaoFormat::SUBAO_GPC_CHANGE_KEY);
		else
			Set_Subao(GetTimeidx(),CLBTH::MsgSubaoFormat::SUBAO_GPC_CHANGE_MISC);
	}

	ClearEdit();
	Query_Subao(select_shift_id);


}

void CPageSUBAO::OnDirectionAll() 
{
	m_nDirection=2;             
	ClearEdit();

	m_pGridCtrl->DeleteNonFixedRows();
	m_pGridCtrl->SetRowCount(m_nTrainCounts+2);
	m_pGridCtrl->SetRedraw (FALSE);    
	for(int i=0;i<m_nTrainCounts;i++)   
		Show_Subao(i,i+1); 

	CRect rect(4,43,700,420);
	SetBlankCells(m_nTrainCounts);
	m_pGridCtrl->SetRedraw (TRUE);   
	InvalidateRect(&rect);

	return;
}

void CPageSUBAO::OnSelendokComboDdtname() 
{  
	CString sName;                 
	int num=m_CboDdtCtrl.GetCurSel();
	int n = m_CboDdtCtrl.GetLBTextLen(num);
	m_CboDdtCtrl.GetLBText( num, sName.GetBuffer(n) );

	char *  ddt[64];         
	int nSectionNum,nSectionList[64];
	nSectionNum = sizeof(nSectionList) / sizeof(nSectionList[0]);
	gpcdef.GetSectionList(nSectionNum,nSectionList);
	for (int i = 0; i < nSectionNum; i++)   
	{
		ddt[i] = gpcdef.GetSectionName(nSectionList[i]);
		if(ddt[i]==sName)
			m_nSelectedSectionID=nSectionList[i];   
	}
	//重新初始化，避免上个台站的内容传到这个台站

	m_previousrow = -1;
	m_nCurrentRow = 0;

	//ends here 

	Query_Subao(select_shift_id);
	// ClearEdit();
	if(m_bReadOnly)
	{
		//您没有修改下列列车编组的权限！");
		GetDlgItem(IDC_NEW)->EnableWindow(FALSE);
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHANGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);

		//设置GRID控件内容不可修改
		if(m_pGridCtrl)
			m_pGridCtrl->SetEditable(FALSE);

	}
	else
	{
		GetDlgItem(IDC_NEW)->EnableWindow(TRUE);
		GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHANGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);

		//设置GRID控件内容可修改
		if(m_pGridCtrl)
			m_pGridCtrl->SetEditable(TRUE);
	}
	SetBlankCells(m_nTrainCounts);  
	m_pGridCtrl->Invalidate();

}

void CPageSUBAO::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CPropertyPage::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CPageSUBAO::PreTranslateMessage(MSG* pMsg) 
{    	
	CWnd* pWndFocus = GetFocus();

	if ((pMsg->message == WM_KEYDOWN) &&
		((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE)))
	{
		// Special case: if control with focus is an edit control with
		// ES_WANTRETURN style, let it handle the Return key.

		TCHAR szClass[10];
		if ((pMsg->wParam == VK_RETURN) &&
			((pWndFocus = GetFocus()) != NULL) &&
			IsChild(pWndFocus)  &&
			GetClassName(pWndFocus->m_hWnd, szClass, 10) &&
			(_tcsicmp(szClass, _T("EDIT")) == 0)
			)
		{

			CWnd *pWnd;			
			if(pWndFocus == GetDlgItem(IDC_TRAIN_ID))
			{
				pWnd = GetDlgItem(IDC_TRACTION_ENGINE1); 
				GotoDlgCtrl(pWnd); 				
			}else if(pWndFocus == GetDlgItem(IDC_TRACTION_ENGINE1)){
				pWnd = GetDlgItem(IDC_TRACTION_ENGINE2); 
				GotoDlgCtrl(pWnd); 				
			}else if(pWndFocus == GetDlgItem(IDC_TRACTION_ENGINE2)){
				pWnd = GetDlgItem(IDC_DRIVER_NAME); 
				GotoDlgCtrl(pWnd); 				
			} else if(pWndFocus == GetDlgItem(IDC_DRIVER_NAME)){
				pWnd = GetDlgItem(IDC_TRAIN_LEADER_NAME); 
				GotoDlgCtrl(pWnd); 
			} else if(pWndFocus == GetDlgItem(IDC_TRAIN_LEADER_NAME)){
				pWnd = GetDlgItem(IDC_K_COUNT); 
				GotoDlgCtrl(pWnd); 
			} else if(pWndFocus == GetDlgItem(IDC_K_COUNT)){
				pWnd = GetDlgItem(IDC_TOTAL_WEIGHT); 
				GotoDlgCtrl(pWnd); 
			} else if(pWndFocus == GetDlgItem(IDC_TOTAL_WEIGHT)){
				pWnd = GetDlgItem(IDC_CAL_LEN); 
				GotoDlgCtrl(pWnd); 
			} else if(pWndFocus == GetDlgItem(IDC_CAL_LEN)){
				pWnd = GetDlgItem(IDC_ATTACH_ENGINE1); 
				GotoDlgCtrl(pWnd); 				
			}else if(pWndFocus == GetDlgItem(IDC_ATTACH_ENGINE1)){
				pWnd = GetDlgItem(IDC_ATTACH_ENGINE2); 
				GotoDlgCtrl(pWnd); 				
			}else if(pWndFocus == GetDlgItem(IDC_ATTACH_ENGINE2)){
				pWnd = GetDlgItem(IDC_ATTACH_ENGINE3); 
				GotoDlgCtrl(pWnd); 				
			}else if(pWndFocus == GetDlgItem(IDC_ATTACH_ENGINE3)){
				pWnd = GetDlgItem(IDC_ATTACH_ENGINE4); 
				GotoDlgCtrl(pWnd); 				
			}else if(pWndFocus == GetDlgItem(IDC_ATTACH_ENGINE4)){
				pWnd = GetDlgItem(IDC_TRAIN_ID); 
				GotoDlgCtrl(pWnd); 				
				if(!m_bReadOnly) //if(gpcdef.GetOperatorPrior())
					OnChange();
				return TRUE;
			}


		}

	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}
BOOL CPageSUBAO::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	SetModified(FALSE);	
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CPageSUBAO::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{ 
	//需要明确，是修改了基本数据(train_id)还是其他数据
	int   idCtrl = (int) wParam;
	if(idCtrl!=IDC_GRID) 
		return CPropertyPage::OnNotify(wParam, lParam, pResult);

	NM_GRIDVIEW *nmgv=(NM_GRIDVIEW*)lParam;;
	int column=nmgv->iColumn;
	m_nCurrentRow=nmgv->iRow;  //m_nCurrentRow

	if(NM_CLICK==nmgv->hdr.code)
	{
		if(column != percolumn)	sortFlag = -1;
		if(m_pGridCtrl->GetFixedRowCount()>0  && m_nCurrentRow == 0 && column > 0)
		{
			SortSubaoByColIndex(column);
			if(sortFlag>=0)
			{
				sortFlag = -1;
				for(int i=m_nTrainCounts-1;i>=0;i--)   
				{ 			
					Show_Subao(i,m_nTrainCounts-i);
				}
				m_pGridCtrl->Invalidate();	
			}
			else
			{
				sortFlag = 1;
				for(int i=0;i<m_nTrainCounts;i++)   
				{ 			
					Show_Subao(i,i+1);
 				}
				m_pGridCtrl->Invalidate();
			}
			
		}
		percolumn = column;
	}

	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

void CPageSUBAO::OnSelendokComboDirections() 
{
	m_nSelectedSel = m_CboDirectionsCtrl.GetCurSel();
	int ndir=nPlotItemList[m_nSelectedSel];     

	ClearEdit();

	//判断要显示的记录数，重新生成行数
	int subao_count = 0;

	//显示全部内容
	if(ndir == nPlotItemNum - 1)
		subao_count = m_nTrainCounts;
	else
	{
		for(int i=0;i<m_nTrainCounts;i++)   
		{
			if(TRAINS[i].Subao_type == ndir)      
				subao_count++;
		}
	}

	m_pGridCtrl->SetRowCount(subao_count+2);
	m_pGridCtrl->SetRedraw (FALSE);

	// ---------在控件中显示内容---------
	//显示全部内容
	if(ndir == nPlotItemNum - 1)
	{
		for(int i=0;i<m_nTrainCounts;i++)   
			Show_Subao(i,i+1); 
	}
	else//显示对应编组的内容
	{
		subao_count=0;
		for(int i=0;i<m_nTrainCounts;i++)   
		{
			if(TRAINS[i].Subao_type==ndir)      
			{  
				subao_count+=1;
				Show_Subao(i, subao_count); 
			}
		}
	}

	SetBlankCells(subao_count);

	m_pGridCtrl->ExpandColumnsToFit();
	m_pGridCtrl->SetRedraw (TRUE);

	return;
}
void CPageSUBAO::InitIdentify()
{
	if(m_bReadOnly)
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_NEW);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_ADD);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHANGE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DELETE);
		pWnd->EnableWindow(FALSE);
	}

	if(m_bReadOnly)
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_NEW);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_ADD);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHANGE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DELETE);
		pWnd->EnableWindow(FALSE);
	}
}


void CPageSUBAO::OnSelchangeCmbShiftid() 
{
	int currSel = m_cmbShiftId.GetCurSel();
	if(currSel == 0)
		select_shift_id = prev_shift_id;
	else if(currSel == 2) 
		select_shift_id = next_shift_id;
	else 
		select_shift_id = curr_shift_id;

	SetBlankCells(0);
	m_pGridCtrl->DeleteNonFixedRows();

	Query_Subao(select_shift_id);

	ClearEdit();
	Invalidate(FALSE);

	InformParent();
}

CString CPageSUBAO::ConvertShiftIdToString(unsigned long id)
{
	CString date,banbie;
	int CurYear  = id/1000000;
	int CurMonth = id/10000-CurYear*100;
	int CurDay   = id/100-(id/10000)*100;
	date.Format("%4d年%2d月%2d日",CurYear,CurMonth,CurDay);
	int CurBanbie=id-(id/10)*10;
	banbie=(CurBanbie==1) ? "第一班" : "第二班";

	CString shift_id_string;
	shift_id_string = date + banbie;
	return shift_id_string;

}


BOOL CPageSUBAO::OnRowDataUpdate()
{
	if(m_previousrow == -1) //没有数据被改变
		return FALSE;
	if(m_nDefaultSectionID!=m_nSelectedSectionID)
	{
		AfxMessageBox("您没有修改列车编组的权限！");
		return FALSE;
	}

	Calculate();

	CGridCellCheck * pCell;
	pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(m_previousrow,1);//列车ID在第1列，基数为0
	m_train_id = pCell->GetText();

	m_train_id = ChangeAlpha(m_train_id);
	if(m_train_id.GetLength()==0 )  
	{
		AfxMessageBox("请输入车次号!");
		m_pGridCtrl->SetFocusCell(m_previousrow,1);
		return FALSE;
	}

	/////////////////////////////////////////////////////////////////////////////////
	bool flag_change_basic = false;
	long old_trainindex;
	//2004-05-26 subao
	//int i=GetFocusTrain(GetTimeidx_SelectedRow(m_nCurrentRow));       //获得当前的记录
	int i = GetSubaoArrayIndexByRowNo(m_nCurrentRow);	
	if(i<0)  
		return false;   



	memcpy(&TRAIN_T,&TRAINS[i],sizeof(TRAIN_T));
	old_trainindex = TRAIN_T.train_idx;

	//TRAIN_T.train_idx=0;
	TRAIN_T.Con_id=m_nSelectedSectionID;          
	TRAIN_T.Subao_type = nPlotItemList[m_CboDirectionCtrl.GetCurSel()];   
	sprintf(TRAIN_T.Train_id,"%s",m_train_id);

	//寻找列车库中是否有此车次号，如有一个，则找出此车的 train_index,
	//如有二，则弹出对话框，从中选择之；
	/*-----------------------------------------------------------------*/
	//  NA???
	//    	TRAIN_T.train_idx = tempTrain[Trainresult[0]]->m_nTrainIndex;

	if(TRAIN_T.train_idx != old_trainindex)
		flag_change_basic = true;
	/*-----------------------------------------------------------------*/
	/////////////////////////////////////////////////////////////////////////////////

	pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(m_previousrow,2);//机车，基数为0
	sprintf(TRAIN_T.Engine_type[0],"%s",pCell->GetText());

	pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(m_previousrow,3);//司机，基数为0
	sprintf(TRAIN_T.Driver_name,"%s",pCell->GetText());

	pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(m_previousrow,4);//车长，基数为0
	sprintf(TRAIN_T.Train_leader_name,"%s",pCell->GetText());

	pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(m_previousrow,5);//辆数，基数为0
	TRAIN_T.K_count = atoi(pCell->GetText());

	pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(m_previousrow,6);//吨位，基数为0
	TRAIN_T.Total_weight = atof(pCell->GetText())*10;

	pCell = (CGridCellCheck *) m_pGridCtrl->GetCell(m_previousrow,7);//长度，基数为0
	TRAIN_T.Cal_len = atof(pCell->GetText())*10;

	//TRAIN_T.K_count=m_k_count;
	TRAIN_T.PK_count=m_pk_count;
	TRAIN_T.PZ_count=m_pz_count;
	TRAIN_T.CK_count=m_ck_count;   ////////////  2002.09.06
	TRAIN_T.CZ_count=m_cz_count;   ////////////  2002.09.06
	TRAIN_T.PIK_count=m_pik_count;
	TRAIN_T.PIZ_count=m_piz_count;
	TRAIN_T.GK_count=m_gk_count;
	TRAIN_T.GZ_count=m_gz_count;
	TRAIN_T.O_count=m_o_count;
	TRAIN_T.S_count=m_s_count;
	TRAIN_T.Total_count=m_total_count;
	//TRAIN_T.Cal_len=m_cal_len;
	//TRAIN_T.Total_weight=m_total_weight;

	sprintf(TRAIN_T.Cover,"%s",m_cover);
	sprintf(TRAIN_T.Tail_no,"%s",m_tail_no);

	if(m_previousrow == (m_pGridCtrl->GetRowCount() - m_pGridCtrl->GetFixedRowCount()))//行号为末尾行号，新增一行数据
		Set_Subao(GetTimeidx(),CLBTH::MsgSubaoFormat::SUBAO_GPC_INSERT);//OPER_SUBAO_INSERT);
	else
	{	  
		if(flag_change_basic)
			Set_Subao(GetTimeidx_SelectedRow(m_previousrow),CLBTH::MsgSubaoFormat::SUBAO_GPC_CHANGE_KEY);//;OPER_SUBAO_CHANGE_BASIC_DATA);
		else
			Set_Subao(GetTimeidx_SelectedRow(m_previousrow),CLBTH::MsgSubaoFormat::SUBAO_GPC_CHANGE_MISC);//OPER_SUBAO_CHANGE);

	}
	//ClearEdit();     
	Query_Subao(select_shift_id);    
	m_previousrow = -1;
	return TRUE;

}
void CPageSUBAO::OnSetfocusTrainId() 
{
	// TODO: Add your control notification handler code here
	g_curEdit =(CEdit*)GetDlgItem(IDC_TRAIN_ID);
}

void CPageSUBAO::OnSetfocusTrainIdF() 
{
	// TODO: Add your control notification handler code here
	g_curEdit =(CEdit*)GetDlgItem(IDC_TRAIN_ID_F);
}

void CPageSUBAO::OnSetfocusEngineType() 
{
	// TODO: Add your control notification handler code here
	g_curEdit =(CEdit*)GetDlgItem(IDC_ENGINE_TYPE);
}

void CPageSUBAO::OnSetfocusDriverName() 
{
	// TODO: Add your control notification handler code here
	g_curEdit =(CEdit*)GetDlgItem(IDC_DRIVER_NAME);
}

void CPageSUBAO::OnSetfocusTrainLeaderName() 
{
	// TODO: Add your control notification handler code here
	g_curEdit =(CEdit*)GetDlgItem(IDC_TRAIN_LEADER_NAME);
}

extern bool CreateRecursiveDir(const char* filename);
void CPageSUBAO::OnBtnImport() 
{
	//导入机调编组数据
	//获得调度台名字，选择的班次，合成我们需要的文件名
	CString fileNameMerged;
	CString strDispName,strBanBie;
	strDispName = CString(gpcdef.GetSectionName(m_pDoc->m_nCurrentSectionNo));

	int currSel = m_cmbShiftId.GetCurSel();
	ulong shift_id;
	if(currSel == 0)
		shift_id = prev_shift_id;
	else if(currSel == 2) 
		shift_id = next_shift_id;
	else 
		shift_id = curr_shift_id;

	char str_shift_id[20];
	ltoa(shift_id,str_shift_id,10);

	fileNameMerged = CString(str_shift_id) + strDispName;

	CString fileNameData,fileNameFlag;
	fileNameData = fileNameMerged + ".txt";
	fileNameFlag = fileNameMerged + "flag.txt";

	char localDriver[10];
	char computerName[MAX_PATH+1];
	strcpy(localDriver,"J:");
	gpcdef.GetJDServerAddr(computerName);

	DWORD ret = WNetCancelConnection2(localDriver, 0,TRUE);
	if (ret != NO_ERROR)
	{
		ret = GetLastError();
		//	continue;
	}

	NETRESOURCE NetResource;
	NetResource.dwType = RESOURCETYPE_DISK ;
	NetResource.lpLocalName = localDriver;
	NetResource.lpRemoteName = computerName;
	NetResource.lpProvider = NULL;
	DWORD dwFlags = 0;//no re-connect when next login.

	int rv = WNetAddConnection2(&NetResource,
		gpcdef.GetJDServerPWD(NULL),
		gpcdef.GetJDServerUser(NULL),
		dwFlags);

	if (rv != NO_ERROR)
	{
		rv = GetLastError();
		AfxMessageBox("连接机调服务器出错" + CString(computerName));
		return;
	}

	//如果机调服务器上面存在当前选中的文件，那么拷贝到本地，然后导入
	//否则提示没有当前班的编组数据

	CString remoteFilePath,localFilePath,remoteFlag;
	remoteFilePath = CString(localDriver) + "\\SUBAO\\" + fileNameData;
	remoteFlag = CString(localDriver) + "\\SUBAO\\" + fileNameFlag;

	if(_access((char*)(LPCTSTR)remoteFlag,0) == -1 || _access((char*)(LPCTSTR)remoteFilePath,0) == -1) //不存在
	{
		AfxMessageBox("没有当前班次的司机信息");
		return;
	}

	localFilePath = CString(gpcdef.GetDataDir()) + "\\Export\\SUBAO\\" + fileNameData;

	if(!CreateRecursiveDir((char*)(LPCTSTR)localFilePath))
	{
		return;
	}

	if(CopyFile((char*)(LPCTSTR)remoteFilePath,(char*)(LPCTSTR)localFilePath,FALSE))
	{
		//调入编组信息，重新查询速报
		if(ReadSuBaoFromFile(localFilePath))
		{
			AfxMessageBox("导入成功");
			Query_Subao(shift_id);
		}
		else
		{
			AfxMessageBox("导入失败");
			return;

		}
		//删除一个月前的文件
		DeleteFiles(CString(gpcdef.GetDataDir()) + "\\Export\\SUBAO\\",31*24*3600,"*.txt");
	}
}
BOOL CPageSUBAO::ReadSuBaoFromFile(const CString &localFilePath)
{

	if(m_bReadOnly) //if(!gpcdef.GetOperatorPrior())
		return FALSE;
	if(m_nDefaultSectionID!=m_nSelectedSectionID)
	{
		AfxMessageBox("您没有修改权限,请更换调度台！");
		return FALSE;
	}

	CStdioFile cf;
	if(!cf.Open(localFilePath,CFile::modeRead))
		return FALSE;

	char lineText[256];
	char trainid[32],jicheNo[20],driverName[32];
	CString line;
	int pos = 0;
	char* token;
	char seps[]   = "$";
	ulong timeindex = GetTimeidx();
	int i;
	while(cf.ReadString(lineText,sizeof(lineText)-1) != NULL)
	{	

		token = strtok(lineText,seps);		
		strncpy(trainid,token,sizeof(trainid)-1);
		strupr(trainid);
		token = strtok(NULL,seps);		
		strncpy(jicheNo,token,sizeof(jicheNo)-1);
		token = strtok(NULL,seps);		
		strncpy(driverName,token,sizeof(driverName)-1);

		if(strlen(trainid)<=0)
			continue;


		for(i=0;i<m_nTrainCounts;i++)   
		{   
			if(strcmp(TRAINS[i].Train_id,trainid) == 0)//find the train
			{  
				memcpy(&TRAIN_T,&TRAINS[i],sizeof(TRAIN_SUBAO_NEW));
				break;
			}
		}

		strcpy(TRAIN_T.Train_id,trainid);
		strcpy(TRAIN_T.Engine_type[0],jicheNo);
		strncpy(TRAIN_T.Driver_name,driverName, sizeof(TRAIN_T.Driver_name)-1);

		if(i>=m_nTrainCounts)
		{
			TRAIN_T.train_idx=0;
			TRAIN_T.Train_leader_name[0] = '\0';
			TRAIN_T.Dest[0] = '\0';

			TRAIN_T.K_count=0;		
			TRAIN_T.PK_count=0;
			TRAIN_T.PZ_count=0;
			TRAIN_T.CK_count=0;
			TRAIN_T.CZ_count=0;
			TRAIN_T.PIK_count=0;
			TRAIN_T.PIZ_count=0;
			TRAIN_T.GK_count=0;
			TRAIN_T.GZ_count=0;
			TRAIN_T.O_count=0;
			TRAIN_T.S_count=0;
			TRAIN_T.Total_count=0;
			TRAIN_T.Cal_len=0;
			TRAIN_T.Total_weight=0;		
			TRAIN_T.Cover[0] = '\0';
			TRAIN_T.Tail_no[0] = '\0';
			timeindex++;			
			Set_Subao(GetTimeidx(),OPER_SUBAO_INSERT);	   
		}
		else
		{
			Set_Subao(GetTimeidx(),OPER_SUBAO_CHANGE);

		}

		//Set_Subao(TRAIN_T.time_idx);



		Sleep(10);
	}

	cf.Close();
	return TRUE;
}

BOOL CPageSUBAO::DeleteFiles(CString fileDir, time_t elapsedtime, CString postfix)
{
	CFileFind cf;
	BOOL findFlag = TRUE;
	findFlag = cf.FindFile(fileDir+ postfix);

	if(!findFlag)
		return FALSE;

	time_t curTime,fileTime;
	time(&curTime);
	curTime -= elapsedtime;

	struct tm realFileTime;
	long file_year,file_month,file_day;

	CString fileName;
	char temp[10];
	memset(temp,0,sizeof(temp));
	while(findFlag)
	{
		findFlag = cf.FindNextFile();
		//根据文件名前面的8位数字确定文件时间
		fileName = cf.GetFileName();
		strncpy(temp,fileName,4);
		temp[4] = '\0';
		file_year = atol(temp);
		fileName = fileName.Mid(4);
		strncpy(temp,fileName,2);
		temp[2] = '\0';
		file_month = atol(temp);
		fileName = fileName.Mid(2);
		strncpy(temp,fileName,2);
		temp[2] = '\0';
		file_day = atol(temp);

		realFileTime.tm_year = file_year-1900;
		realFileTime.tm_mon = file_month-1;//从0开始
		realFileTime.tm_mday = file_day;
		realFileTime.tm_hour = 0;
		realFileTime.tm_min = 0;
		realFileTime.tm_sec = 0;
		realFileTime.tm_isdst = -1;

		fileTime = mktime(&realFileTime);

		if(fileTime < curTime)
		{
			fileName = cf.GetFilePath();
			DeleteFile(fileName);
		}


	}

	return TRUE;

}

int CPageSUBAO::GetRowByTrainIndex(DWORD index)
{
	for(int i=1;i<m_pGridCtrl->GetRowCount();i++)
	{
		if(m_pGridCtrl->GetCell(i,0)->GetData()==index)
			return i;
	}
	return -1;
}

int CPageSUBAO::GetSubaoArrayIndexByRowNo(int rowno)
{
	int i,num=0;

	if(m_nDirection == 0)//上行
	{
		for(i=0;i<m_nTrainCounts;i++)   
		{ 
			if(!GetTrainDirection(i))
				num ++;
			if(num == rowno)
				return i;
		}

	}
	else if(m_nDirection == 1)//下行
	{
		for(i=0;i<m_nTrainCounts;i++)   
		{ 
			if(GetTrainDirection(i))
				num ++;

			if(num == rowno)
				return i;
		}

	}
	else//上下行
	{
		;
	}



	int sel = m_CboDirectionsCtrl.GetCurSel();
	int ndir=nPlotItemList[sel];  
	i = num = 0;

	if(ndir == nPlotItemNum - 1)
	{
		for(i=0;i<m_nTrainCounts;i++)   
		{ 
			num ++;
			if(num == rowno)
				return i;
		}
	}
	else
	{
		for(i=0;i<m_nTrainCounts;i++)   
		{
			if(TRAINS[i].Subao_type==ndir)      
			{
				num ++;
				if(num == rowno)
					return i;
			}  
		}
	}
	return -1;

}
//获取打印设备函数
extern BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);
void CPageSUBAO::OnPrintGroup() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(AfxMessageBox("打印列车编组信息?",MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES  )
		return; 

	CWinApp* cApp=AfxGetApp();
	ASSERT_VALID(cApp);

	HGLOBAL hDevMode;
	HGLOBAL hDevNames;

	CString csPlotterName;
	csPlotterName.Format("%s",gpcdef.GetPrinterName());
	if (!GetPrinterDevice((char *)(LPCTSTR)csPlotterName, &hDevNames, &hDevMode))
	{
		AfxMessageBox(_T("无法获取指定的打印机设备\n"));
		return;
	}

	m_RecordsPerPrintPage = 100;

	CPrintInfo pInfo;
	HDC prnDc;

	pInfo.m_pPD->m_pd.hDevMode = hDevMode;
	pInfo.m_pPD->m_pd.hDevNames = hDevNames;
	prnDc = pInfo.m_pPD->CreatePrinterDC();

	CDC printDC;
	printDC.Attach(prnDc);

	DOCINFO printDocumentName;
	CString sPrintName;
	sPrintName = "列车编组";

	char sPrintDocName[32];
	memcpy(sPrintDocName,sPrintName,32);

	printDocumentName.cbSize = sizeof(DOCINFO);
	printDocumentName.lpszDocName = sPrintDocName;
	printDocumentName.lpszOutput = NULL;
	printDC.StartDoc(&printDocumentName);


	int pageNo = 1;

	if((m_pGridCtrl->GetRowCount() - 1) % m_RecordsPerPrintPage == 0)
		pageNo = (m_pGridCtrl->GetRowCount() -1) / m_RecordsPerPrintPage;
	else
		pageNo = (m_pGridCtrl->GetRowCount() -1) / m_RecordsPerPrintPage + 1;
	pInfo.SetMaxPage(pageNo);
	pInfo.SetMinPage(1);

	for(int i=1; i<=pageNo; i++)
	{
		printDC.StartPage( );
		pInfo.m_nCurPage = i;
		PrintGroupInfo(&printDC,&pInfo);
		printDC.EndPage( );
	}


	printDC.EndDoc( );
	printDC.Detach( );
	DeleteDC(prnDc);

	GlobalFree(hDevNames);
	GlobalFree(hDevMode);
}

void CPageSUBAO::PrintGroupInfo(CDC *pDC, CPrintInfo *pInfo)
{
	if(this->GetSafeHwnd() == NULL || !::IsWindow(this->GetSafeHwnd()))
		return;

	CPen pen,*pOldPen;
	CFont font,*poldfont;

	int nWidth=pDC->GetDeviceCaps(HORZRES);
	int nHeight=pDC->GetDeviceCaps(VERTRES);

	CString title=_T("列车编组");
	CRect DrawRect(100,100,nWidth,nHeight),rect,tempRect;


	pen.CreatePen(PS_SOLID,2,CLR_BLACK);
	font.CreateFont(-MulDiv(20,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));

	pOldPen = pDC->SelectObject(&pen);
	poldfont = pDC->SelectObject(&font);

	//绘制标题栏
	tempRect=DrawRect;
	pDC->DrawText(title,&tempRect,DT_CENTER|DT_TOP|DT_SINGLELINE);
	pDC->SelectObject(poldfont);
	font.DeleteObject();

	//绘制外面的框架
	DrawRect.top  = DrawRect.top - MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72) ;
	//DrawRect.top += 50;
	DrawRect.InflateRect(-50,0,-150,-150);
	pDC->Rectangle(DrawRect);

	//绘制选项信息	
	font.CreateFont(-MulDiv(8,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));
	poldfont = pDC->SelectObject(&font);

	CString tempString;
	CSize tempSize;
	//打印车站名称和查询起始范围
	CString sectionname,date_start,date_end;
	//sectionname = m_CurStatisticsSection->sSectionName;
	//	CTime tempTime(long_time(m_shiftdate,18*3600)-WHOLE_DAY_SECOND);
	//	CTime tempTime2(long_time(m_shiftdate,18*3600));


	tempRect=DrawRect;
	tempRect.left+=5;
	tempRect.right+=5;
	tempRect.top = tempRect.top + 10;
	tempRect.bottom = tempRect.top + 150;
	tempRect.InflateRect(-5,0,0,0);

	m_CboDdtCtrl.GetLBText(m_CboDdtCtrl.GetCurSel(),tempString);

	CString tempString2;

	m_cmbShiftId.GetLBText(m_cmbShiftId.GetCurSel(),tempString2);

	tempString += "   ";
	tempString += tempString2;

	pDC->DrawText(tempString,tempRect,DT_LEFT|DT_SINGLELINE|DT_VCENTER);

	DrawRect.top = tempRect.bottom;


	pDC->SelectObject(poldfont);
	pDC->SelectObject(pOldPen);

	font.DeleteObject();
	pen.DeleteObject();


	font.CreateFont(-MulDiv(6,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	poldfont = pDC->SelectObject(&font);

	DrawRect.top = tempRect.bottom + 10;
	//绘制表格
	int rowscount = 0; //要绘制的行数
	if(pInfo->m_nCurPage < pInfo->GetMaxPage())
		rowscount = m_RecordsPerPrintPage; //min( - m_RecordsPerPrintPage*pInfo.m_nCurPage + (pInfo.m_nCurPage - 1),);		
	else
	{
		if((m_pGridCtrl->GetRowCount() - 1) % m_RecordsPerPrintPage == 0)
			rowscount = m_RecordsPerPrintPage;
		else
			rowscount = (m_pGridCtrl->GetRowCount() - 1)% m_RecordsPerPrintPage;
	}

	rowscount++; //加上标题栏这一行

	int rowHeight = 130;
	int i;

	//画横线
	CPoint pstart,pend;
	pstart.x = DrawRect.left;
	pend.x = DrawRect.right;

	for(i=0; i<=rowscount; i++)
	{		
		pstart.y = pend.y = DrawRect.top + i*rowHeight;
		pDC->MoveTo(pstart);
		pDC->LineTo(pend);
	}

	//每列所占的比例
	double colrate[] = {0.05,0.1,0.1,0.1,0.1,0.1,0.1,0.05,0.1,0.08,0.12};
	//画竖线
	pstart.y = DrawRect.top;
	pend.y = DrawRect.top + rowHeight*rowscount;

	pstart.x = pend.x = DrawRect.left;

	//是否打印这一列不打印
	for(i=1; i<ITEMNUM-1;i++)
	{
		pstart.x += colrate[i-1]*DrawRect.Width();
		pend.x = pstart.x;

		pDC->MoveTo(pstart);
		pDC->LineTo(pend);

	}

	int j=0;
	//COLORREF bkColor;

	int nOldAlign = pDC->SetTextAlign(TA_CENTER);
	tempRect.left = DrawRect.left;
	tempRect.top = DrawRect.top;
	tempRect.right = tempRect.left + colrate[0]*DrawRect.Width();
	tempRect.bottom = tempRect.top + rowHeight;
	for(j=0; j<ITEMNUM-1; j++)
	{		
		for(i=0; i<rowscount;i++)
		{
			//bkColor = m_pGridCtrl->GetItemBkColour(i,j);
			//if(bkColor>RGB(255,255,255))
			//{
			//	bkColor = CLR_WHITE;
			//}
			//CBrush bkBrush(bkColor);
			//pDC->FillRect(tempRect,&bkBrush);
			if(i == 0) //标题栏
				tempString = m_pGridCtrl->GetItemText(i,j);
			else
				tempString = m_pGridCtrl->GetItemText(i+(pInfo->m_nCurPage-1) * m_RecordsPerPrintPage,j);

			tempSize = pDC->GetTextExtent(tempString);
			pDC->TextOut (tempRect.CenterPoint().x, tempRect.CenterPoint().y - tempSize.cy/2,tempString);
			tempRect.OffsetRect(0,rowHeight);
		}

		tempRect.top = DrawRect.top;
		tempRect.bottom = tempRect.top + rowHeight;
		tempRect.left = tempRect.left + colrate[j]*DrawRect.Width();
		tempRect.right = tempRect.left + colrate[j+1]*DrawRect.Width();		
	}

	pDC->SetTextAlign(nOldAlign);

	tempRect = DrawRect;
	tempRect.top = tempRect.bottom - 100;
	tempString.Format("第%d页 共%d页",pInfo->m_nCurPage,pInfo->GetMaxPage());
	pDC->DrawText(tempString,tempRect,DT_CENTER|DT_SINGLELINE);	

	pDC->SelectObject(poldfont);
	font.DeleteObject();


}

//根据方向和所选择的行号返回实际的index
int CPageSUBAO::GetActualRowNo(int curRowNo, int dir)
{
	return 0;
}

//colNum index from 0
void CPageSUBAO::SortSubaoByColIndex(int colNum)
{
	if(m_nTrainCounts <= 0)
		return;

	TRAIN_SUBAO_NEW *cmp1, *cmp2;
	char buf1[128],buf2[128];
	int i1, i2;

	TRAIN_SUBAO_NEW temp;
	int cmpResult;
	for(int i=0;i<m_nTrainCounts-1;i++)
	{
		cmp1 = &TRAINS[i];
		for(int j=i+1; j<m_nTrainCounts; j++)
		{
			cmp2 = &TRAINS[j];

			i1 = i2 = 0;
			buf1[0] = '\0';
			buf2[0] = '\0';

			switch(colNum)
			{	
			case 1:
				sprintf(buf1,"%s",cmp1->Train_id);
				sprintf(buf2,"%s",cmp2->Train_id);
				break;
			case 2:
				sprintf(buf1,"%s",cmp1->tract_engine_id[0]);
				sprintf(buf2,"%s",cmp2->tract_engine_id[0]);
				break;
			case 3:
				sprintf(buf1,"%s",cmp1->tract_engine_id[1]);
				sprintf(buf2,"%s",cmp2->tract_engine_id[1]);
				break;
			case 4:
				sprintf(buf1,"%s",cmp1->attach_engine_id[0]);
				sprintf(buf2,"%s",cmp2->attach_engine_id[0]);
				break;
			case 5:
				sprintf(buf1,"%s",cmp1->Driver_name);
				sprintf(buf2,"%s",cmp2->Driver_name);
				break;
			case 6:
				sprintf(buf1,"%s",cmp1->Train_leader_name);
				sprintf(buf2,"%s",cmp2->Train_leader_name);
				break;
			case 7:
				i1 = cmp1->K_count;
				i2 = cmp2->K_count;
				break;
			case 8:				
				i1 = cmp1->Total_weight;
				i2 = cmp2->Total_weight;
				break;
			case 9:
				i1 = cmp1->Cal_len;
				i2 = cmp2->Cal_len;
				break;
			case 10:
				{
					CString stName = gpcdef.GetStationName(cmp1->report_station);
					if(stName != "管外")
						sprintf(buf1,"%s",gpcdef.GetStationName(cmp1->report_station));
					else
						buf1[0] = '\0';
					
					stName = gpcdef.GetStationName(cmp2->report_station);
					if(stName != "管外")
						sprintf(buf2,"%s",gpcdef.GetStationName(cmp2->report_station));
					else
						buf2[0] = '\0';
				}
				break;
			case 11:	

				if(cmp1->Print_flag == 0)
					sprintf(buf1,"是");
				else
					sprintf(buf1,"否");

				if(cmp2->Print_flag == 0)
					sprintf(buf2,"是");
				else
					sprintf(buf2,"否");

				break;


			}

			if(colNum >= 7 && colNum <=9)
			{
				cmpResult = i1 - i2;

			}
			else
			{
				cmpResult = strcmp(buf1,buf2);
			}


			if(cmpResult>0)//greater
			{
				temp = *cmp1;
				*cmp1 = *cmp2;
				*cmp2 = temp;
			}			
		}		
	}
}

int oldTrainCounts=0;
void CPageSUBAO::OnTimer(UINT_PTR IDEvent)
{
	if(201105 == IDEvent)
	{
		if(m_nTrainCounts!=oldTrainCounts)
			oldTrainCounts=m_nTrainCounts;
		else
		{
			if(m_nTrainCounts>=0)
			{
				KillTimer(201105);

				CButton* pBtnDown=(CButton*)GetDlgItem(IDC_DIRECTION_DOWN);
				CButton* pBtnUp=(CButton*)GetDlgItem(IDC_DIRECTION_UP);
				
				OnDirectionAll();
				if(pBtnDown)
				{
					if(pBtnDown->GetCheck())
					{
						OnDirectionDown();
					}
				}
				if(pBtnUp)
				{
					if(pBtnUp->GetCheck())
					{
						OnDirectionUp();
					}
				}
			}
		}
	}
}

BOOL  CPageSUBAO::ProcessMessage(CLBTH::MsgSubaoFormat *pBuf)
{  
	if(pBuf->oper != CLBTH::MsgSubaoFormat::SUBAO_SERVER_RELAY)
		return  TRUE;
    
	int i=m_nTrainCounts; 
	if (i>=sizeof(TRAINS)/sizeof(TRAINS[0]))
	{
		GpcLogTrainMessage(0,0,"速报: 列车 %s,总个数过多丢弃", pBuf->train_id);
		return  TRUE;
	}

	if(m_nTrainCounts==0)
	{
		SetTimer(201105, 500, NULL);
	}
	m_nTrainCounts +=1;
	
	memset(&TRAINS[i],0x00,sizeof(TRAIN_SUBAO_NEW));
	TRAINS[i].train_idx = pBuf->train_index;
	TRAINS[i].input_time = pBuf->input_time;
	memset(TRAINS[i].Train_id, 0,sizeof(TRAINS[i].Train_id));
	strncpy(TRAINS[i].Train_id, pBuf->train_id,sizeof(pBuf->train_id));	 
	memset(TRAINS[i].Train_describe,0, sizeof(TRAINS[i].Train_describe));
	strncpy(TRAINS[i].Train_describe,pBuf->car_des,sizeof(pBuf->car_des));   //describe  	
	memset(TRAINS[i].Orig,0, sizeof(TRAINS[i].Orig));
	strncpy(TRAINS[i].Orig,pBuf->orig,sizeof(pBuf->orig));   //start station    
	memset(TRAINS[i].Dest,0, sizeof(TRAINS[i].Dest));
	strncpy(TRAINS[i].Dest,pBuf->dest,sizeof(pBuf->dest));   //end station

	////BYTE          engine_count;                   //bit 7-4 附挂机车个数  3-0 牵引机车个数
	///2005.12.21修改

	TRAINS[i].tract_engine_count = 0x0F &(pBuf->engine_count) ;  //牵引机车个数
	TRAINS[i].attach_engine_count = ( 0xF0 &(pBuf->engine_count) )>>4 ; //附挂机车个数

	TRAINS[i].engine_count =  TRAINS[i].tract_engine_count  +   TRAINS[i].attach_engine_count ; //附挂机车个数 + 牵引机车个数

	int m,n;
	for( m=0;m < TRAINS[i].tract_engine_count ; m++)
	{
		TRAINS[i].tract_engine_type[m] = pBuf->subao_engine[m].engine_type ;		//engine type 
		memcpy(TRAINS[i].tract_engine_id[m],pBuf->subao_engine[m].engine_id,sizeof(pBuf->subao_engine[m].engine_id));     //engine id  

	}
	for( n=0;n < TRAINS[i].attach_engine_count ; n++)
	{
		TRAINS[i].attach_engine_type[n] = pBuf->subao_engine[m+n].engine_type ;		//engine type 
		memcpy(TRAINS[i].attach_engine_id[n],pBuf->subao_engine[m+n].engine_id,sizeof(pBuf->subao_engine[m+n].engine_id) );     //engine id  
	}

	memset(TRAINS[i].Driver_name,0, sizeof(TRAINS[i].Driver_name));
	strncpy(TRAINS[i].Driver_name,pBuf->driver_name,sizeof(pBuf->driver_name));  //drive name
	memset(TRAINS[i].Train_leader_name,0, sizeof(TRAINS[i].Train_leader_name));
	strncpy(TRAINS[i].Train_leader_name,pBuf->train_leader_name,sizeof(pBuf->train_leader_name)); 
	memset(TRAINS[i].Cover,0, sizeof(TRAINS[i].Cover));
	strncpy(TRAINS[i].Cover,pBuf->cover,sizeof(pBuf->cover));  //cover
	memset(TRAINS[i].Tail_no,0, sizeof(TRAINS[i].Tail_no));
	strncpy(TRAINS[i].Tail_no,pBuf->tail_no,sizeof(pBuf->tail_no));  //tail_no

	TRAINS[i].Space_rank= pBuf->space_rank;  //space rank   ******
	TRAINS[i].Length_rank= pBuf->length_rank;  //length rank ****** 
	TRAINS[i].Super_rank= pBuf->super_rank;  //super rank  ******
	TRAINS[i].Military_rank= pBuf->military_rank;  //military rank ******
	TRAINS[i].Speed_limit= pBuf->speed_limit;  //speed limit ******
	TRAINS[i].Con_id= pBuf->con_id;    //con id  ******
	TRAINS[i].E_engine= pBuf->e_engine;  //e_engine ******
	TRAINS[i].Print_flag= pBuf->print_flag;  //print flag  ******
	TRAINS[i].K_count=pBuf->pass_cnt;    //k count

	TRAINS[i].Load_count= pBuf->load_cnt;  //load count  

	TRAINS[i].PK_count=  pBuf->pk_count;  //pk count 

	TRAINS[i].PZ_count= pBuf->pz_count;    //pz count  

	TRAINS[i].CK_count= pBuf->ck_count;  //ck count  

	TRAINS[i].CZ_count=  pBuf->cz_count;   // cz count   

	TRAINS[i].PIK_count=  pBuf->pik_count;  // pik count   

	TRAINS[i].PIZ_count=  pBuf->piz_count;   // piz count  

	TRAINS[i].GK_count=   pBuf->gk_count; // gk count 

	TRAINS[i].GZ_count=  pBuf->gz_count;   //gz count  

	TRAINS[i].O_count=   pBuf->o_count; // o count  

	TRAINS[i].S_count=    pBuf->s_count;  // s count

	TRAINS[i].Total_count= pBuf->total_count;    //total count  

	TRAINS[i].Van_count= pBuf->van_count;  //Van count  ******


	//	ushort cal_len= pBuf->cal_len; //cal len     modified by zzg in ty 2008.05.14
	TRAINS[i].Cal_len=pBuf->cal_len; //cal len

	//	long total_weight= pBuf->total_weight;   //total weight 
	TRAINS[i].Total_weight= pBuf->total_weight;   //total weight  

	TRAINS[i].Subao_type= pBuf->subao_type;  //subao type  	 
	TRAINS[i].Speed= pBuf->speed ;//speed     ******	 
	TRAINS[i].Noutil_cnt= pBuf->noutil_cnt; //tail  ****** 
	TRAINS[i].report_station= pBuf->report_sta;
 
	//获得编组栏目
	m_nSelectedSel = m_CboDirectionsCtrl.GetCurSel();
	int ndir=nPlotItemList[m_nSelectedSel]; 

	int subao_num = 0;
	if(ndir == nPlotItemNum-1)
	{
		subao_num = m_nTrainCounts;
	}
	else
	{
		for(i=0;i<m_nTrainCounts;i++)   
		{
			if(TRAINS[i].Subao_type==ndir)      
				subao_num++;
		}
	}

	//设置新的行数

	m_pGridCtrl->SetRowCount(subao_num+2);

	//注意，一定要在设置行以后才可以设置不重画的标志，否则滚动条的信息还是老的信息
	m_pGridCtrl->SetRedraw (FALSE);


	//显示记录
	if(ndir == nPlotItemNum - 1)//显示全部内容
	{
		for(i=0;i<m_nTrainCounts;i++)   
			Show_Subao(i,i+1); 
	}
	else//显示对应编组的内容
	{
		subao_num=0; 
		for(i=0;i<m_nTrainCounts;i++)   
		{
			if(TRAINS[i].Subao_type==ndir)      
			{  
				subao_num++;
				Show_Subao(i, subao_num); 
			}
		}
	}

	SetBlankCells(subao_num);

	//定位上次的行 ，注意还要区分当前要显示的编组栏目
	//开始定位记录，记录自动往后面挪一行

	if(subao_num>=m_nCurrentRow)
	{
		SCROLLINFO sinfo;
		m_pGridCtrl->GetScrollInfo(SB_VERT,&sinfo);

		int pos;
		pos = sinfo.nMin+(sinfo.nMax - sinfo.nMin)*(m_nCurrentRow-2)/(subao_num+2);

		if(pos<sinfo.nMin)
			pos = sinfo.nMin;
		else if(pos>sinfo.nMax)
			pos = sinfo.nMax;
		else
			;


		m_pGridCtrl->SetScrollPos(SB_VERT,pos,FALSE);
		ToFocusRow(m_nCurrentRow+1);//自动移动到下一行
		m_pGridCtrl->SetFocusCell(m_nCurrentRow+1,1);
		int train_index = 0;
		//获得数组的索引号
		if(subao_num == m_nCurrentRow)
		{
			ClearEdit();
		}
		else
		{
			int train_index = GetSubaoArrayIndexByRowNo(m_nCurrentRow+1);
			ShowTrainInEdits(train_index);
		}
	}
	m_pGridCtrl->ExpandColumnsToFit(FALSE);
	m_pGridCtrl->SetRedraw (TRUE);  
	m_pGridCtrl->Invalidate(); 

	return TRUE;
}

LRESULT CPageSUBAO::OnSchdTrainSelected(WPARAM wp,LPARAM lp)
{
	int row=GetRowByTrainIndex(wp);
	if(row>=0)
		ToFocusRow(row);
	ClearEdit();
	m_nCurrentTrainIndex=(DWORD)wp;
	CWorkScheduleHolder *pholder=m_pDoc->GetTGDataHolder();
	CTG_TrainScheduleBase *pdata=pholder->GetWorkScheduleData();
	CTG_SingleTrain *ptrain=pdata->GetSingleTrain(m_nCurrentTrainIndex);
	if(!ptrain)
		return 0;
	m_train_id=ptrain->GetTrainID(); 
	UpdateData(FALSE);
	return 0;
}

void CPageSUBAO::InformParent()
{
	TRAIN_SELECT_PARA para;
	CTG_ScheduleInfo schd(m_pDoc->GetScheduleType(),m_pDoc->GetCurrentSectionNo(),select_shift_id);
	TIME shift_start,shift_end;
	schd.GetShiftManagerRange(shift_start,shift_end); 
	para.starttime= shift_start;
	para.endtime= shift_end;
	para.list_type=0; 
	if(m_pNotifyWindow)
		m_pNotifyWindow->SendMessage(UM_TG_TRAIN_SELECTED,(WPARAM)0,(LPARAM)&para);
}

