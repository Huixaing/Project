// GuaJiCheDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GuaJiCheDlg.h"
#include "dbfunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGuaJiCheDlg dialog
//标题
#define MAX_COLUMN 5
#define MAX_SIZE_TYPE 3
TCHAR * _gzBiaoTi[MAX_COLUMN] ={
	_T("机车号"),_T("牵引方式"),_T("机车类型"),_T("起始车站"),_T("结束车站")
};

TCHAR * _gzJiCheType[MAX_SIZE_TYPE] = {
	_T("内燃机车"),_T("东风四"),_T("前进")
};

CGuaJiCheDlg::CGuaJiCheDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGuaJiCheDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGuaJiCheDlg)
	m_jiCheNo = _T("");
	//}}AFX_DATA_INIT
	m_curRecIndex = -1;
	m_GuaJiChe.engine_count = 0;
	m_pTrain = NULL;

}


void CGuaJiCheDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGuaJiCheDlg)
	DDX_Control(pDX, IDC_LIST_CONTENT, m_List);
	DDX_Text(pDX, IDC_EDIT2, m_jiCheNo);
	DDV_MaxChars(pDX, m_jiCheNo, 9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGuaJiCheDlg, CDialog)
	//{{AFX_MSG_MAP(CGuaJiCheDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_SHUAI, OnSelchangeComboShuai)
	ON_CBN_SELCHANGE(IDC_TRAIN_LIST, OnSelchangeTrainList)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGuaJiCheDlg message handlers

BOOL CGuaJiCheDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here


	do{
		this->InitialListCtrl();
		this->initLeftColumn();
	}while(0);
	
	if(!gpcdef.GetOperatorRight(WINTG_EDIT))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_SAVE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DELETE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);
		
	}

	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CGuaJiCheDlg::InitialListCtrl()
{
	ASSERT(m_pTrain != NULL);
	ASSERT_KINDOF(CSingleTrain,m_pTrain);

	Pstruct TRAINRECORD *train_rec;
	LV_COLUMN lvc;
	LV_ITEM lvi;
	CRect rect;
	int count = 0;
	CString text;
	text.Empty();

	//改变listctrl的风格
	//m_List.SetExtendedStyle(LVS_REPORT|LVS_EX_FULLROWSELECT|LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE);
	m_List.SetExtendedStyle(LVS_REPORT|LVS_EX_FULLROWSELECT| LVS_EX_ONECLICKACTIVATE);
	m_List.GetClientRect(&rect);


	//添加标题头
	for(count=0;count<MAX_COLUMN;count++)
	{
		lvc.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT;
		lvc.fmt = LVCFMT_CENTER;
		lvc.cx = rect.Width() / MAX_COLUMN;
		lvc.pszText = _gzBiaoTi[count];
		m_List.InsertColumn(count, &lvc);
	}

	//填充内容
	for(count = 0;count < (int)m_GuaJiChe.engine_count;count ++)
	{
//		int type_index;
		int rec_index;
		BYTE engine_type;

		lvi.mask  =  LVIF_TEXT;
		lvi.iItem = count;
		lvi.iSubItem = 0;
		text.Format("%s",m_GuaJiChe.engine_id[count]);
		lvi.pszText = const_cast<char *>((LPCSTR)text);
		m_List.InsertItem(&lvi);
		
		//modified by qianguorong 2003-08-14
		engine_type = m_GuaJiChe.engine_type[count];
		if(IsAttachEngineExt(engine_type))
		{
			text.Format("附挂机车");
		}
		else
		{
			text.Format("牵引机车");			
		}

		m_List.SetItemText(count,1,text);

		if(IsDieselEngineExt(engine_type))
		{
			text.Format("内燃机车");
		}
		else
		{
			text.Format("电力机车");			
		}
		m_List.SetItemText(count,2,text);



		//现在要根据attach_station 来取得station的名字
		rec_index  = (int)m_GuaJiChe.attach_station[count];
		train_rec = m_pTrain->GetIndexRecord(rec_index);
		if(train_rec != NULL)
		{
			text.Format("%s",gpcdef.GetStationName(train_rec->station));
		}
		else
		{
			text.Format("错误");
		}
		m_List.SetItemText(count,3,text);


		rec_index = (int)m_GuaJiChe.unattach_station[count];
		train_rec = m_pTrain->GetIndexRecord(rec_index);
		if(train_rec != NULL)
		{
			text.Format("%s",gpcdef.GetStationName(train_rec->station));
		}
		else
		{
			text.Format("无");
		}
		m_List.SetItemText(count,4,text);
	}
	return TRUE;
}

CGuaJiCheDlg::CGuaJiCheDlg(CSingleTrain *pTrain,int rec_index,CWnd *pParent)
    : CDialog(CGuaJiCheDlg::IDD, pParent)
{
	Pstruct TRAIN_INFO *pInfo;
	m_curRecIndex = rec_index;
	m_pTrain = pTrain;
	pInfo = &pTrain->m_nTrainInfo;
	m_GuaJiChe.engine_count = pInfo->engine_count;
	memcpy(m_GuaJiChe.engine_type,pInfo->engine_type,sizeof(BYTE)*MAX_ENGINEER_NUM);
	memcpy(m_GuaJiChe.engine_id,pInfo->engine_id,sizeof(char)*MAX_ENGINEER_NUM*20);
	memcpy(m_GuaJiChe.attach_station,pInfo->attach_station,sizeof(USHORT)*MAX_ENGINEER_NUM);
	memcpy(m_GuaJiChe.unattach_station,pInfo->unattach_station,sizeof(USHORT)*MAX_ENGINEER_NUM);
}

BOOL CGuaJiCheDlg::initLeftColumn()
{
	ASSERT(m_pTrain != NULL );
	ASSERT_KINDOF(CSingleTrain,m_pTrain);

	Pstruct TRAINRECORD *train_rec = NULL;

	int count = 0;
	CString text;
	text.Empty();
	//用来初始化左边的编辑内容
	CEdit *pEdit;
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT2);
	pEdit->SetWindowText(_T("电"));


	//车次
	text.Format("%s",m_pTrain->m_strTrainId);
	pEdit = (CEdit *)GetDlgItem(IDC_TRAIN_ID);
	pEdit->SetWindowText(text);
	
	/*
	CComboBox *pCombo;
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO3);
	for(count = 0 ;count< MAX_SIZE_TYPE;count ++)
	{
		pCombo->InsertString(count,_gzJiCheType[count]);
		pCombo->SetItemData(count,count);
	}
	*/

	//added by qianguorong 2003-08-12
	CComboBox *pCombo;
	
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_TYPE);
	pCombo->InsertString(0,"附挂机车");
	pCombo->SetItemData(0,0);
	pCombo->InsertString(1,"牵引机车");
	pCombo->SetItemData(1,1);
	pCombo->SetCurSel(1); //默认牵引
	
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO3);
	pCombo->InsertString(0,"电力机车");
	pCombo->SetItemData(0,0);
	pCombo->InsertString(1,"内燃机车");
	pCombo->SetItemData(1,1);
	pCombo->SetCurSel(0);	
	
	//added by qianguorong 2003-08-12 ends here

	pCombo = (CComboBox *)GetDlgItem(IDC_TRAIN_LIST);
	int usRecordCount = m_pTrain->GetRecordNum();
	for(count = 0;count < usRecordCount;count ++)
	{
		train_rec = m_pTrain->GetIndexRecord(count);
		if(train_rec == NULL)
		{
			continue;
		}
		text.Format("%s",gpcdef.GetStationName(train_rec->station));
		pCombo->InsertString(count,text);
		pCombo->SetItemData(count,count);
	}
	
	pCombo->SetCurSel(0);

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SHUAI);
	for(count = 1;count < usRecordCount;count ++)
	{
		train_rec = m_pTrain->GetIndexRecord(count);
		if(train_rec == NULL)
		{
			continue;
		}
		text.Format("%s",gpcdef.GetStationName(train_rec->station));
		pCombo->InsertString(count - 1 ,text);
		pCombo->SetItemData(count -1,count);
	}
	pCombo->InsertString(usRecordCount - 1,"无");
	pCombo->SetItemData(usRecordCount-1,-1);

	pCombo->SetCurSel(usRecordCount-2);
	return TRUE;
}

void CGuaJiCheDlg::OnSelchangeComboShuai() 
{
	// TODO: Add your control notification handler code here
	int curSelect;
	curSelect = m_List.GetHotItem();
}

BOOL CGuaJiCheDlg::initialButtonStatus()
{
	return TRUE;

}

void CGuaJiCheDlg::fillLeftContent(int curSel)
{
	ASSERT(m_pTrain != NULL );
	ASSERT_KINDOF(CSingleTrain,m_pTrain);
	ASSERT(curSel < (int)m_GuaJiChe.engine_count);

	BOOL isWrong = FALSE;
	Pstruct TRAINRECORD *train_rec = NULL;
	int total_count_rec = m_pTrain->GetRecordNum();
	int index = 0;

	int count = 0;
	CString text;
	text.Empty();
	//用来初始化左边的编辑内容
	CEdit *pEdit;
	text.Format("%s",m_GuaJiChe.engine_id[curSel]);
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT2);
	pEdit->SetWindowText(text);

	
		
	/*commented by qianguorong 2003-08-14
	if(index >= 0 && index < MAX_SIZE_TYPE)
	{
		pCombo->SetCurSel(index);
	}
	else
	{
		pCombo->SetWindowText("未知类型");
	}
	commented by qianguorong 2003-08-14 ends here*/

	//added by qianguorong 2003-08-14
	CComboBox *pCombo;
	index = m_GuaJiChe.engine_type[curSel];

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_TYPE);
	
	if(IsAttachEngineExt(index))//附挂机车
	{
		pCombo->SetCurSel(0);
	}
	else
	{
		pCombo->SetCurSel(1);
	}

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO3);
	
	if(IsDieselEngineExt(index))//电力
	{
		pCombo->SetCurSel(1);
	}
	else   //柴油机
	{
		pCombo->SetCurSel(0);
	}	
	
	//added by qianguorong 2003-08-14 ends here

	pCombo = (CComboBox *)GetDlgItem(IDC_TRAIN_LIST);
	index = (int)m_GuaJiChe.attach_station[curSel];
	if(index >= 0 && index < total_count_rec)
	{
		pCombo->SetCurSel(index);
	}
	else
	{
		isWrong = TRUE;
		pCombo->SetWindowText("数据出错，请删除");
	}
	

	int last_index;
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SHUAI);
	pCombo->EnableWindow(TRUE);
	pCombo->ResetContent();
	if(isWrong)
	{
		pCombo->SetWindowText("数据出错，请删除");
	}
	else
	{
		for(count = index + 1;count < total_count_rec;count++)
		{
			train_rec = m_pTrain->GetIndexRecord(count);
			if(train_rec == NULL)
			{
				continue;
			}
			text.Format("%s",gpcdef.GetStationName(train_rec->station));
			pCombo->InsertString(count - index - 1,text);
			pCombo->SetItemData(count - index - 1,count);
		}
		pCombo->InsertString(total_count_rec - index -1,"无");
		pCombo->SetItemData(total_count_rec-index-1,-1);

		last_index = (int)m_GuaJiChe.unattach_station[curSel];
		if(last_index > index && last_index >= 0 && last_index < total_count_rec)
		{
			pCombo->SetCurSel(last_index - index -1);
		}
		else
		{
			/*pCombo->SetWindowText(_T(""));*/
			m_GuaJiChe.unattach_station[curSel] = -1;
			//m_List.SetItemText(curSel,4,"未甩车");
			pCombo->SetCurSel(pCombo->GetCount()-1);
		}
	}
}

void CGuaJiCheDlg::OnSelchangeTrainList() 
{
	// TODO: Add your control notification handler code here
	ASSERT(m_pTrain!= NULL);
	ASSERT_KINDOF(CSingleTrain,m_pTrain);

	int index = 0;
	int count = 0;
	CComboBox *pCombo; 
	Pstruct TRAINRECORD *train_rec;
	CString text;

	int total_count_rec = m_pTrain->GetRecordNum();
	pCombo = (CComboBox *)GetDlgItem(IDC_TRAIN_LIST);
	index = pCombo->GetCurSel();
	

//	int last_index;
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SHUAI);
	pCombo->EnableWindow(TRUE);
	pCombo->ResetContent();
	for(count = index + 1;count < total_count_rec;count++)
	{
		train_rec = m_pTrain->GetIndexRecord(count);
		if(train_rec == NULL)
		{
			continue;
		}
		text.Format("%s",gpcdef.GetStationName(train_rec->station));
		pCombo->InsertString(count - index - 1,text);
		pCombo->SetItemData(count - index - 1,count);
	}
	pCombo->InsertString(total_count_rec - m_curRecIndex -1,"无");
	pCombo->SetItemData(total_count_rec-m_curRecIndex-1,-1);

	pCombo->SetCurSel(count-index-1);
}

void CGuaJiCheDlg::OnSave() 
{
	// TODO: Add your control notification handler code here
	//if old item
	CString text;
	CString itemText;
	CComboBox * pCombo;
	BOOL isExist = FALSE;

	int pos = 0;
	int index = -1;
	int count = 0;

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT2);
	pEdit->GetWindowText(text);
	if(text.IsEmpty())
	{
		AfxMessageBox("无效的名称");
		return;
	}
	
	for(count = 0 ;count < m_List.GetItemCount();count ++)
	{
		itemText = m_List.GetItemText(count,0);
		if(text.Compare(itemText) == 0)
		{
			isExist = TRUE;
			break;
		}
	}
	pos = count;


	if(!isExist)
	{
		if( REAL_MAX_ENGIN_NUM <=  m_GuaJiChe.engine_count)
		{
			CString tempStr;
			tempStr.Format("最多只能挂%d个机车",REAL_MAX_ENGIN_NUM);
			AfxMessageBox(tempStr);	
			//AfxMessageBox("最多只能挂4个机车");
			return;
		}
		m_GuaJiChe.engine_count ++;
	}

	pCombo = (CComboBox *)GetDlgItem(IDC_TRAIN_LIST);
	index = pCombo->GetCurSel();
	if(index == -1)
	{
		AfxMessageBox("请选择起始车站");
		return;//do nothing
	}	

		
	memset(m_GuaJiChe.engine_id[pos],0x00,20);
	strncpy(m_GuaJiChe.engine_id[pos],text,10);//机车号
	
	

	//added by qianguorong 2003-08-14
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_TYPE);//牵引方式
	index = pCombo->GetCurSel();
	switch(index)
	{
	case 0://附挂
		SetAttachEngineExt(m_GuaJiChe.engine_type[pos]);
		break;
	case 1://牵引
		SetTractionEngineExt(m_GuaJiChe.engine_type[pos]);
		break;
	default:
		SetAttachEngineExt(m_GuaJiChe.engine_type[pos]);
		break;
	}

	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO3);//机车类型
	index = pCombo->GetCurSel();
	switch(index)
	{
	case 1://柴油机
		SetDieselEngineExt(m_GuaJiChe.engine_type[pos]);
		break;
	case 0://电力
		SetElecEngineExt(m_GuaJiChe.engine_type[pos]);
		break;
	default:
		SetDieselEngineExt(m_GuaJiChe.engine_type[pos]);
		break;
	}


	//added by qianguorong 2003-08-14 ends here
	pCombo = (CComboBox *)GetDlgItem(IDC_TRAIN_LIST);//挂车车站
	index = pCombo->GetCurSel();
	m_GuaJiChe.attach_station[pos] = index;
	
	pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SHUAI);//甩车车站
	index = pCombo->GetCurSel();
	if(index != -1)
	{
		index = pCombo->GetItemData(index);
	}
	m_GuaJiChe.unattach_station[pos] = index;

    //填充界面
	LV_ITEM lvi;
//	int type_index;
	int rec_index;
	Pstruct TRAINRECORD *train_rec;
	count = pos;
	
	if(!isExist)
	{
		lvi.mask  =  LVIF_TEXT;
		lvi.iItem = count;
		lvi.iSubItem = 0;
		text.Format("%s",m_GuaJiChe.engine_id[count]);
		lvi.pszText = const_cast<char *>((LPCSTR)text);
		m_List.InsertItem(&lvi);
	}
	else
	{
		m_List.SetItemText(count,0,text);
	}
	
	
	
	
	/*commented by qianguorong 2003-08-14
	type_index = (int)m_GuaJiChe.engine_type[count];
	if(type_index >= 0 && type_index < MAX_SIZE_TYPE)
	{
		text.Format("%s",_gzJiCheType[type_index]);
	}
	else
	{
		text.Format("未知类型");
	}
	commented by qianguorong 2003-08-14 ends here*/
	//added by qianguorong 2003-08-14
	if(IsAttachEngineExt(m_GuaJiChe.engine_type[count]))
	{
		text.Format("附挂机车");
	}
	else
	{
		text.Format("牵引机车");			
	}
	//added by qianguorong 2003-08-14 ends here
	m_List.SetItemText(count,1,text);
	
	
	
	if(IsDieselEngineExt(m_GuaJiChe.engine_type[count]))
	{
		text.Format("内燃机车");
	}
	else
	{
		text.Format("电力机车");			
	}
	//added by qianguorong 2003-08-14 ends here
	m_List.SetItemText(count,2,text);

	//现在要根据attach_station 来取得station的名字
	rec_index  = (int)m_GuaJiChe.attach_station[count];
	train_rec = m_pTrain->GetIndexRecord(rec_index);
	if(train_rec != NULL)
	{
		text.Format("%s",gpcdef.GetStationName(train_rec->station));
	}
	else
	{
		text.Format("错误数据");
	}
	m_List.SetItemText(count,3,text);
	
	
	rec_index = (int)m_GuaJiChe.unattach_station[count];
	train_rec = m_pTrain->GetIndexRecord(rec_index);
	if(train_rec != NULL)
	{
		text.Format("%s",gpcdef.GetStationName(train_rec->station));
	}
	else
	{
		text.Format("无");
	}
	m_List.SetItemText(count,4,text);
}

void CGuaJiCheDlg::OnDelete() 
{
	// TODO: Add your control notification handler code here
	int count;
	int index;
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		return;
	}
	index = m_List.GetNextSelectedItem(pos);

	{
		for(count = index;count <(int)m_GuaJiChe.engine_count - 1;count++)
		{
			memset(m_GuaJiChe.engine_id[count],0x00,20);
			strncpy(m_GuaJiChe.engine_id[count],m_GuaJiChe.engine_id[count+1],20);
			
			m_GuaJiChe.engine_type[count] = m_GuaJiChe.engine_type[count+1];
			m_GuaJiChe.attach_station[count] = m_GuaJiChe.attach_station[count+1];
			m_GuaJiChe.unattach_station[count] = m_GuaJiChe.unattach_station[count+1];
		}
		m_GuaJiChe.engine_count --;
	}

	m_List.DeleteAllItems();

	LV_ITEM lvi;
	CString text;
	Pstruct TRAINRECORD *train_rec;
	for(count = 0;count < (int)m_GuaJiChe.engine_count;count ++)
	{
		int type_index;
		int rec_index;
		lvi.mask  =  LVIF_TEXT;
		lvi.iItem = count;
		lvi.iSubItem = 0;
		text.Format("%s",m_GuaJiChe.engine_id[count]);
		lvi.pszText = const_cast<char *>((LPCSTR)text);

		m_List.InsertItem(&lvi);


		type_index = (int)m_GuaJiChe.engine_type[count];
/*
		if(type_index >= 0 && type_index < MAX_SIZE_TYPE)
		{
			text.Format("%s",_gzJiCheType[type_index]);
		}
		else
		{
			text.Format("未知类型");
		}
		m_List.SetItemText(count,1,text);

*/
		if(IsAttachEngineExt(m_GuaJiChe.engine_type[count]))
		{
			text.Format("附挂机车");
		}
		else
		{
			text.Format("牵引机车");			
		}
		m_List.SetItemText(count,1,text);
		
		
		
		if(IsDieselEngineExt(m_GuaJiChe.engine_type[count]))
		{
			text.Format("内燃机车");
		}
		else
		{
			text.Format("电力机车");			
		}
		m_List.SetItemText(count,2,text);

		//现在要根据attach_station 来取得station的名字
		rec_index  = (int)m_GuaJiChe.attach_station[count];
		train_rec = m_pTrain->GetIndexRecord(rec_index);
		if(train_rec != NULL)
		{
			text.Format("%s",gpcdef.GetStationName(train_rec->station));
		}
		else
		{
			text.Format("错误数据");
		}
		m_List.SetItemText(count,3,text);

		rec_index = (int)m_GuaJiChe.unattach_station[count];
		train_rec = m_pTrain->GetIndexRecord(rec_index);
		if(train_rec != NULL)
		{
			text.Format("%s",gpcdef.GetStationName(train_rec->station));
		}
		else
		{
			text.Format("无");
		}
		m_List.SetItemText(count,4,text);
	}
}




BOOL CGuaJiCheDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	NMHDR *pNMHDR = (NMHDR *)lParam;
	//modified by qiangurong 2003-08-12
	if(pNMHDR->idFrom == IDC_LIST_CONTENT && pNMHDR->code == NM_CLICK)
	//modified by qiangurong 2003-08-12 ends here
	{
		int index;

		POSITION pos = m_List.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			index = m_List.GetNextSelectedItem(pos);
			fillLeftContent(index);
			return TRUE;
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}
