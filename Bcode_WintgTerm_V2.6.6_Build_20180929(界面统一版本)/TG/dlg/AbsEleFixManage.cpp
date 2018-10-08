// AbsEleFixManage.cpp : implementation file
//

#include "stdafx.h"
#include "AbsEleFixManage.h"
#include ".\abselefixmanage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int send_message(USHORT usProcessId,USHORT usQueueHandle,
						USHORT usMsgSize,char *abBufferMsg);
/////////////////////////////////////////////////////////////////////////////
// CAbsEleFixManage dialog
#define TIANCHUANGTYPECOLUMNS           3 //
_TCHAR * TianChuangTypeLabel[]=  
{
	_T("编号"),_T("天窗名称"),_T("状态")
};
int TianChuangTypeFmt[]=
{
	    LVCFMT_LEFT,
		LVCFMT_LEFT
};
int TianChuangTypeWidth[]={50,300,100};


CAbsEleFixManage::CAbsEleFixManage(CTGDoc *pDoc,CWnd* pParent)
	: CDialog(CAbsEleFixManage::IDD, pParent)
{
	m_strTianChuangName = _T("");
	m_strTianChuangNo = _T("");

	m_pDoc = pDoc;
	m_nTianChuangFileNum = 0;
	m_nAbsNum = 0;

	nFenXiangPos = 0;
	m_FileFaceSelectBlockFlag = 0;
}


void CAbsEleFixManage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsEleFixManage)
	//DDX_Control(pDX, IDC_DTPICKER_ENDTIME, m_ctlEndTime);
	//DDX_Control(pDX, IDC_DTPICKER_BEGINTIME, m_ctlStartTime);
	//DDX_Control(pDX, IDC_COMBO_END_ABS, m_comboEndAbs);
	//DDX_Control(pDX, IDC_COMBO_BEGIN_ABS, m_comboBeginAbs);
	DDX_Text(pDX, IDC_EDIT_TIANCHUANG_NAME, m_strTianChuangName);
	DDX_Text(pDX, IDC_EDIT_TIANCHUANG_NO, m_strTianChuangNo);
	//DDV_MaxChars(pDX, m_strTianChuangNo, 4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbsEleFixManage, CDialog)
	//{{AFX_MSG_MAP(CAbsEleFixManage)
	ON_BN_CLICKED(IDC_BTN_ADD_SAVE, OnBtnAddSave)
	ON_BN_CLICKED(IDC_RADIO_SJXJ, OnRadioSjxj)
	ON_BN_CLICKED(IDC_RADIO_SJXY, OnRadioSjxy)
	ON_BN_CLICKED(IDC_RADIO_SYXJ, OnRadioSyxj)
	ON_BN_CLICKED(IDC_RADIO_SYXY, OnRadioSyxy)
	ON_BN_CLICKED(IDC_CHECK_INCLUDE_DOWN, OnCheckIncludeDown)
	ON_BN_CLICKED(IDC_CHECK_INCLUDE_UP, OnCheckIncludeUp)
	ON_BN_CLICKED(IDC_BTN_INSERT_TO_GRAPH, OnBtnInsertToGraph)
	ON_BN_CLICKED(IDC_BTN_MODIFY_SAVE, OnBtnModifySave)
	ON_BN_CLICKED(IDC_BTN_CANCEL_OPER, OnBtnCancelOper)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBtnDelete)
	ON_CBN_SELCHANGE(IDC_COMBO_BEGIN_ABS, OnSelchangeComboBeginAbs)
	ON_CBN_SELCHANGE(IDC_COMBO_END_ABS, OnSelchangeComboEndAbs)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
END_MESSAGE_MAP()


BOOL CAbsEleFixManage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	if(!m_pDoc)
		return FALSE;
	memset(&m_absInfo,0,sizeof(m_absInfo));
	CListCtrl *pList=(CListCtrl*)GetDlgItem(IDC_LIST_CONTENT_ELE_FIX);
	DWORD dwStyle =pList->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	pList->SetExtendedStyle(dwStyle);

	LV_COLUMN lvc;
	int j;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	//insert header
	for(j = 0; j < TIANCHUANGTYPECOLUMNS; j++)
	{
		lvc.iSubItem = j;
		lvc.pszText =  _T(TianChuangTypeLabel[j]);
		lvc.cx = TianChuangTypeWidth[j];
		lvc.fmt = TianChuangTypeFmt[j];
		pList->InsertColumn(j,&lvc);
	}
	InitAbsInfo();
    //起始区间下拉列表
	int i = 0;
	CString str;
	CComboBox * pBeginAbs=(CComboBox*)GetDlgItem(IDC_COMBO_BEGIN_ABS);
	if (pBeginAbs==NULL)
	    return FALSE;
	CComboBox * pEndAbs=(CComboBox*)GetDlgItem(IDC_COMBO_END_ABS);
	if (pEndAbs==NULL)
		return FALSE;
	for (i = 0; i < m_nAbsNum; i++)
	{
		str.Format("%s",m_absInfo[i].szAbsName);
		pBeginAbs->AddString(str);
		pEndAbs->AddString(str);
	}
	UpdateData(FALSE);
	
	SetControlEnable(FALSE,FALSE);
	
    CDateTimeCtrl *pStartTime=(CDateTimeCtrl*)GetDlgItem(IDC_DTPICKER_BEGINTIME);
	pStartTime->SetFormat("HH':'mm yyy'-'MM'-'dd");
    CDateTimeCtrl *pEndTime=(CDateTimeCtrl*)GetDlgItem(IDC_DTPICKER_ENDTIME);
	pEndTime->SetFormat("HH':'mm yyy'-'MM'-'dd");

	InitListInfo();

	CButton *pCheckUp = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_UP);
	CButton *pCheckDown = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_DOWN);
	if (!pCheckUp || !pCheckDown)
		return FALSE;
	pCheckUp->EnableWindow(FALSE);
	pCheckUp->SetCheck(0);
	pCheckDown->EnableWindow(FALSE);
	pCheckDown->SetCheck(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CAbsEleFixManage message handlers

BOOL CAbsEleFixManage::InitAbsInfo()
{
	int i = 0;
	m_nAbsNum = 0;
	
	for (int i=0; i<m_pDoc->m_nDispDistanceNum-1; i++)
	{
		//同一个区段
		if(m_pDoc->m_sDispDistance[i].sub_section == m_pDoc->m_sDispDistance[i+1].sub_section)  
		{
			m_absInfo[m_nAbsNum].nAbsIndex = m_nAbsNum;
			m_absInfo[m_nAbsNum].nNode1 = i;
			m_absInfo[m_nAbsNum].nNode1StationNo = m_pDoc->m_sDispDistance[i].station_number;//站号
			strcpy(m_absInfo[m_nAbsNum].szNode1Name,m_pDoc->m_sDispDistance[i].station_name);
			m_absInfo[m_nAbsNum].nNode2 = i+1;
			m_absInfo[m_nAbsNum].nNode2StationNo = m_pDoc->m_sDispDistance[i+1].station_number;//站号
			strcpy(m_absInfo[m_nAbsNum].szNode2Name,m_pDoc->m_sDispDistance[i+1].station_name);
            sprintf(m_absInfo[m_nAbsNum].szAbsName,"%s-%s",m_absInfo[m_nAbsNum].szNode1Name,m_absInfo[m_nAbsNum].szNode2Name);
			m_nAbsNum++;
		}
	}

	return TRUE;
}

void CAbsEleFixManage::OnBtnAddSave() 
{
	UpdateData(TRUE);
	CListCtrl *pList=(CListCtrl*)GetDlgItem(IDC_LIST_CONTENT_ELE_FIX);

	CEdit *pEditTianchuangName = (CEdit *)GetDlgItem(IDC_EDIT_TIANCHUANG_NAME);
	if(!pEditTianchuangName)
		return;
	CEdit *pEditTianchuangNo = (CEdit *)GetDlgItem(IDC_EDIT_TIANCHUANG_NO);
	if(!pEditTianchuangNo)
		return;


	CString strButtonAddText,strButtonModifyText;
	CButton *pButtonModify = (CButton *)GetDlgItem(IDC_BTN_MODIFY_SAVE);
	if(!pButtonModify)
		return;
	CButton *pButtonAdd = (CButton *)GetDlgItem(IDC_BTN_ADD_SAVE);
	if(!pButtonAdd)
		return;

	pButtonModify->GetWindowText(strButtonModifyText);
	pButtonAdd->GetWindowText(strButtonAddText);



	if(strButtonAddText == "新建")
	{
		InitControl();
		pButtonAdd->SetWindowText("新建保存");
		pButtonModify->EnableWindow(FALSE);
	}
	else if (strButtonAddText == "新建保存")
	{
		if(!SaveTianChuangInfoToFile(1))
			return;

		pList->DeleteAllItems();
		char szTemp[256];

		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT;
		int row = 0;
		int col = 0;
		int insertindex = 0;
		for(row = 0; row < m_AbsEleFixModule.tianchuang_num; row ++)
		{
			for(col = 0;col < 3; col ++)
			{
				lvi.iItem = row;
				if(col == 0)
				{
					lvi.iSubItem = col;
					sprintf(szTemp,"%d",m_AbsEleFixModule.tianchuang_details[row].nTianChuangNo);
					lvi.pszText = szTemp;
					if(m_AbsEleFixModule.tianchuang_details[row].nTianChuangNo == atoi((LPCSTR)m_strTianChuangNo))
						insertindex = row;
					pList->InsertItem(&lvi);
				}
				if(col == 1)
				{
					lvi.iSubItem = col;
					lvi.pszText = m_AbsEleFixModule.tianchuang_details[row].tianchuang_title_name;
					pList->SetItem(&lvi);
				}
				if(col == 2)
				{
					lvi.iSubItem = col;
					lvi.pszText = CheckResult(row);
					pList->SetItem(&lvi);
				}
			}
		}
		//nItemCount = m_typelist.GetItemCount();
		m_nCurrentSel = insertindex;
		pList->SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
		ShowRecord(m_nCurrentSel);

		pButtonAdd->SetWindowText("新建");
		SetControlEnable(FALSE,FALSE);

		pButtonModify->EnableWindow(TRUE);
	}

	
	
}

void CAbsEleFixManage::OnRadioSjxj() 
{
	nFenXiangPos = 1;
	
}

void CAbsEleFixManage::OnRadioSjxy() 
{
	nFenXiangPos = 2;
	
}

void CAbsEleFixManage::OnRadioSyxj() 
{
	nFenXiangPos = 3;
	
}

void CAbsEleFixManage::OnRadioSyxy() 
{
	nFenXiangPos = 4;
	
}

void CAbsEleFixManage::OnCheckIncludeDown() 
{
	CButton *pCheckDown = (CButton *)GetDlgItem(IDC_CHECK_INCLUDE_DOWN);
	if (!pCheckDown)
		return;
	CButton *pCheckUp = (CButton *)GetDlgItem(IDC_CHECK_INCLUDE_UP);
	if (!pCheckUp)
		return;

	if (pCheckDown->GetCheck() == 1 && pCheckUp->GetCheck() == 1)
		nCheckUpAndDownFlag = 3;
	else if (pCheckDown->GetCheck() == 1 && pCheckUp->GetCheck() == 0)
		nCheckUpAndDownFlag = 1;
	else if (pCheckDown->GetCheck() == 0 && pCheckUp->GetCheck() == 1)
		nCheckUpAndDownFlag = 2;
	else
		nCheckUpAndDownFlag = 0;
	
}

void CAbsEleFixManage::OnCheckIncludeUp() 
{
	CButton *pCheckDown = (CButton *)GetDlgItem(IDC_CHECK_INCLUDE_DOWN);
	if (!pCheckDown)
		return;
	CButton *pCheckUp = (CButton *)GetDlgItem(IDC_CHECK_INCLUDE_UP);
	if (!pCheckUp)
		return;

	if (pCheckDown->GetCheck() == 1 && pCheckUp->GetCheck() == 1)
		nCheckUpAndDownFlag = 3;
	else if (pCheckDown->GetCheck() == 1 && pCheckUp->GetCheck() == 0)
		nCheckUpAndDownFlag = 1;
	else if (pCheckDown->GetCheck() == 0 && pCheckUp->GetCheck() == 1)
		nCheckUpAndDownFlag = 2;
	else
		nCheckUpAndDownFlag = 0;
}


BOOL CAbsEleFixManage::InitListInfo()
{
	memset(&m_AbsEleFixModule,0x00,sizeof(m_AbsEleFixModule));
	char path[256];
    //CGpcdef gpcdef;
	sprintf(path,"%s\\general_tianchuang_file\\%02d\\*.cfg", m_pDoc->GetDataPath(), m_pDoc->m_nCurrentSectionNo);
	ReadTianChuangModuleFile(path,&m_AbsEleFixModule);
	SortTianChuangModuleFile(&m_AbsEleFixModule);
 
    CListCtrl *pList=(CListCtrl*)GetDlgItem(IDC_LIST_CONTENT_ELE_FIX);
	pList->DeleteAllItems();
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT;
	int row = 0, col = 0;
	for(row = 0; row < m_AbsEleFixModule.tianchuang_num ; row ++)
	{
		for(col = 0;col < 3; col ++)
		{
			lvi.iItem = row;
			if(col == 0)
			{
				char szTemp[64];
				lvi.iSubItem = col;
				sprintf(szTemp,"%d",m_AbsEleFixModule.tianchuang_details[row].nTianChuangNo);
				lvi.pszText = szTemp;
				pList->InsertItem(&lvi);
			}
			if(col == 1)
			{
				lvi.iSubItem = col;
				lvi.pszText = m_AbsEleFixModule.tianchuang_details[row].tianchuang_title_name;
				pList->SetItem(&lvi);
			}
			if(col == 2)
			{
				lvi.iSubItem = col;
				lvi.pszText = CheckResult(row);
				pList->SetItem(&lvi);
			}
		}
	}
	//nItemCount = m_typelist.GetItemCount();
	m_nCurrentSel = 0;
	pList->SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
	UpdateData(FALSE);
	ShowRecord(m_nCurrentSel);

	return TRUE;
}

void CAbsEleFixManage::ShowRecord(int nCurrentSel)
{//显示当前选中的条目的命令内容
    do 
    {
		m_strTianChuangNo.Format("%d",m_AbsEleFixModule.tianchuang_details[nCurrentSel].nTianChuangNo);
		m_strTianChuangName.Format("%s",m_AbsEleFixModule.tianchuang_details[nCurrentSel].tianchuang_title_name);
		CComboBox * pBeginAbs=(CComboBox*)GetDlgItem(IDC_COMBO_BEGIN_ABS);
		if (pBeginAbs==NULL)
			break;
		CComboBox * pEndAbs=(CComboBox*)GetDlgItem(IDC_COMBO_END_ABS);
		if (pEndAbs==NULL)
			break;
		pBeginAbs->SetCurSel(m_AbsEleFixModule.tianchuang_details[nCurrentSel].nSelectBeginAbsIndex);
		pEndAbs->SetCurSel(m_AbsEleFixModule.tianchuang_details[nCurrentSel].nSelectEndAbsIndex);

		//1 上近下近，2上近下远，3 上远下近，4 上远下远
		CButton *pRadio1 = (CButton *)GetDlgItem(IDC_RADIO_SJXJ) ;
		CButton *pRadio2 = (CButton *)GetDlgItem(IDC_RADIO_SJXY) ;
		CButton *pRadio3 = (CButton *)GetDlgItem(IDC_RADIO_SYXJ) ;
		CButton *pRadio4 = (CButton *)GetDlgItem(IDC_RADIO_SYXY) ;
		if (!pRadio1 || !pRadio2 || !pRadio3 || !pRadio4)
			return;

		if (m_AbsEleFixModule.tianchuang_details[nCurrentSel].nSelectFenXiangFlag == 1)
		{
			pRadio1->SetCheck(1);
			pRadio2->SetCheck(0);
			pRadio3->SetCheck(0);
			pRadio4->SetCheck(0);
			nFenXiangPos = 1;
		}
		else if (m_AbsEleFixModule.tianchuang_details[nCurrentSel].nSelectFenXiangFlag == 2)
		{
			pRadio2->SetCheck(1);
			pRadio1->SetCheck(0);
			pRadio3->SetCheck(0);
			pRadio4->SetCheck(0);
			nFenXiangPos = 2;
		}
		else if (m_AbsEleFixModule.tianchuang_details[nCurrentSel].nSelectFenXiangFlag == 3)
		{
			pRadio3->SetCheck(1);
			pRadio2->SetCheck(0);
			pRadio1->SetCheck(0);
			pRadio4->SetCheck(0);
			nFenXiangPos = 3;
		}
		else if (m_AbsEleFixModule.tianchuang_details[nCurrentSel].nSelectFenXiangFlag == 4)
		{
			pRadio4->SetCheck(1);
			pRadio2->SetCheck(0);
			pRadio3->SetCheck(0);
			pRadio1->SetCheck(0);
			nFenXiangPos = 4;
		}

		CButton *pCheckUp = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_UP);
		CButton *pCheckDown = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_DOWN);
		if (!pCheckUp || !pCheckDown)
			return;
		if(m_AbsEleFixModule.tianchuang_details[nCurrentSel].nFaceSelectBlockFlag & 0x01)
			pCheckUp->SetCheck(1);
		else
			pCheckUp->SetCheck(0);

		if(m_AbsEleFixModule.tianchuang_details[nCurrentSel].nFaceSelectBlockFlag & 0x02)
			pCheckDown->SetCheck(1);
		else
			pCheckDown->SetCheck(0);
		break;
    } while (TRUE);


	UpdateData(FALSE);

}

BOOL CAbsEleFixManage::PreTranslateMessage(MSG* pMsg) 
{
	 //TODO: Add your specialized code here and/or call the base class
	CListCtrl *pList=(CListCtrl*)GetDlgItem(IDC_LIST_CONTENT_ELE_FIX);
	UpdateData(TRUE);
	if ((pMsg->message == WM_KEYDOWN) &&
		((pMsg->wParam == VK_ESCAPE)))
	{
		CWnd *pWndFocus = NULL;
		TCHAR szClass[10];
		if ((pMsg->wParam == VK_RETURN) &&
			((pWndFocus = GetFocus()) != NULL) &&
			IsChild(pWndFocus)  &&
			GetClassName(pWndFocus->m_hWnd, szClass, 10) &&
			(_tcsicmp(szClass, _T("EDIT")) == 0)
		){
			return TRUE;
		} else if (pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}	
	
	if(pMsg->message == WM_LBUTTONDOWN)  {
		do {
			if(pMsg->hwnd != pList->GetSafeHwnd( ))
				break;
			CPoint point;
			point.x = (pMsg->lParam) & 0x0000ffff;
			point.y = ((pMsg->lParam) & 0xffff0000) >> 16;
			UINT uFlags = 0;
			int nHitItem = pList->HitTest(point, &uFlags);

			// we need additional checking in owner-draw mode
			// because we only get LVHT_ONITEM
			BOOL bHit = FALSE;
			if ((uFlags & LVHT_ONITEM) && (pList->GetStyle() & LVS_OWNERDRAWFIXED))
			{
				CRect rect;
				pList->GetItemRect(nHitItem, rect, LVIR_BOUNDS);

				// check if hit was on a state icon
				if (point.x >= rect.left && point.x <= rect.right 
					&& point.y >= rect.top && point.y <= rect.bottom)
					bHit = TRUE;
			} 

			m_nCurrentSel = nHitItem;
			if (m_nCurrentSel<0)
				break;
			ShowRecord(m_nCurrentSel);

            UpdateData(FALSE);

			if (!bHit)
				break;
			if(nHitItem == m_nCurrentSel)
				break;


		}while(0);
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

//读天窗文件
bool CAbsEleFixManage::ReadTianChuangModuleFile(char *lpszFileName,struct _tagAbsEleFixModule *t)
{//-1 is fail,0 is success
	t->tianchuang_num = 0;//init
	memset(t,0x00,sizeof(t));
	BOOL flag = TRUE;
	CFileFind m_Findfile;
	if(m_Findfile.FindFile(lpszFileName) == FALSE)
		return false;
	do{
		flag = m_Findfile.FindNextFile();
		CString filetitle,filename;
		filetitle = m_Findfile.GetFilePath();
		filename = m_Findfile.GetFileName();

		char buffer[2048];
		memset(buffer,0x00,sizeof(buffer));
		char *ptr,*substr;
		int len=0;
		
		FILE *fp;
		fp = fopen((char *)(LPCTSTR)filetitle,"r");
		int i = 0,ch;
		if(fp == NULL)
		{
			return false;
		}
		//将文件所有内容拷贝至str中
		ch = fgetc(fp);
		while(!feof(fp))
		{
			buffer[i] = (char)ch;
			ch = fgetc(fp);
			i++;
		}
		buffer[i] = '\0';
		fclose(fp);

		sscanf((LPCSTR)filename,"%d.cfg",&t->tianchuang_details[t->tianchuang_num].nTianChuangNo);

		/*int m_FileNode1Index;
	int m_FileNode2Index;
	int m_FileNode1StationNo;
	int m_FileNode2StationNo;
	int m_FileBlockFlag;//封锁上下方站
	int m_FileSelectBeginAbsIndex;
	int m_FileSelectEndAbsIndex;
	int m_FileRealBeginAbsIndex;
	int m_FileRealEndAbsIndex;*/
		char szTemp[64];
		//将buffer给结构赋值
		ptr = buffer;
		if(substr = strstr(ptr,"@"))//天窗的类型 11;
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nType = atoi(szTemp);
		}
		ptr = ptr + len + 1;
		if(substr = strstr(ptr,"@"))//天窗名称
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(t->tianchuang_details[t->tianchuang_num].tianchuang_title_name,ptr,len);
		}
		ptr += len + 1;
		if(substr = strstr(ptr,"@"))//节点序号1
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nNode1Index = atoi(szTemp);
		}
		ptr += len + 1;
		if(substr = strstr(ptr,"@"))//节点序号2
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nNode2Index = atoi(szTemp);
		}
		ptr += len + 1;
		if(substr = strstr(ptr,"@"))//节点序号1对应站号
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nNode1StationNo = atoi(szTemp);
		}
		ptr += len + 1;
		if(substr = strstr(ptr,"@"))//节点序号2对应站号
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nNode2StationNo = atoi(szTemp);
		}
		ptr += len + 1;
		if(substr = strstr(ptr,"@"))//用户分相标志
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nSelectFenXiangFlag = atoi(szTemp);
		}
		ptr += len + 1;
		if(substr = strstr(ptr,"@"))//实际封锁标志
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nRealBlockFlag = atoi(szTemp);
		}
		ptr += len + 1;
		if(substr = strstr(ptr,"@"))//调度员选择的开始区间序号
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nSelectBeginAbsIndex = atoi(szTemp);
		}

		ptr += len + 1;
		if(substr = strstr(ptr,"@"))//调度员选择的结束区间序号
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nSelectEndAbsIndex = atoi(szTemp);
		}

		ptr += len + 1;
		if(substr = strstr(ptr,"@"))//实际的开始区间序号
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nRealBeginAbsIndex = atoi(szTemp);
		}
		ptr += len + 1;
		if(substr = strstr(ptr,"@"))//实际的结束区间序号
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nRealEndAbsIndex = atoi(szTemp);
		}

		ptr += len + 1;
		if(substr = strstr(ptr,"@"))//界面选择的复选框标志 "含上方站"，"含下方站"
		{
			memset(szTemp,0,sizeof(szTemp));
			len = substr - ptr;
			strncpy(szTemp,ptr,len);
			t->tianchuang_details[t->tianchuang_num].nFaceSelectBlockFlag = atoi(szTemp);
		}

		ptr += len + 1;
		t->tianchuang_num += 1;
	}while(flag == TRUE);
	m_Findfile.Close();
	return true;
}
bool CAbsEleFixManage::WriteTianChuangModuleFile(char *lpszFileName)
{
	char buffer[256];
	memset(buffer,0x00,sizeof(buffer));
	FILE *fp;
	fp = fopen(lpszFileName,"w");
	if(fp == NULL)
	{
		return false;
	}
	/*
	int m_FileNode1Index;
	int m_FileNode2Index;
	int m_FileNode1StationNo;
	int m_FileNode2StationNo;
	int m_FileBlockFlag;//封锁上下方站
	int m_FileSelectBeginAbsIndex;
	int m_FileSelectEndAbsIndex;
	int m_FileRealBeginAbsIndex;
	int m_FileRealEndAbsIndex;
	*/
	CString str;
	strcpy(buffer,"11");//天窗的类型
	strcat(buffer,"@");

	strcat(buffer,m_strTianChuangName);
	strcat(buffer,"@");
	

	str.Format("%d",m_FileNode1Index);
	strcat(buffer,str);
	strcat(buffer,"@");

	str.Format("%d",m_FileNode2Index);
	strcat(buffer,str);
	strcat(buffer,"@");

	str.Format("%d",m_FileNode1StationNo);
	strcat(buffer,str);
	strcat(buffer,"@");

	str.Format("%d",m_FileNode2StationNo);
	strcat(buffer,str);
	strcat(buffer,"@");

	str.Format("%d",m_FileSelectFenXiangFlag);
	strcat(buffer,str);
	strcat(buffer,"@");

	str.Format("%d",m_FileRealBlockFlag);
	strcat(buffer,str);
	strcat(buffer,"@");

	str.Format("%d",m_FileSelectBeginAbsIndex);
	strcat(buffer,str);
	strcat(buffer,"@");

	str.Format("%d",m_FileSelectEndAbsIndex);
	strcat(buffer,str);
	strcat(buffer,"@");

	str.Format("%d",m_FileRealBeginAbsIndex);
	strcat(buffer,str);
	strcat(buffer,"@");

	str.Format("%d",m_FileRealEndAbsIndex);
	strcat(buffer,str);
	strcat(buffer,"@");

	str.Format("%d",m_FileFaceSelectBlockFlag);
	strcat(buffer,str);
	strcat(buffer,"@");

	fprintf(fp,"%s",buffer);
	fclose(fp);
	return true;
}

void CAbsEleFixManage::SortTianChuangModuleFile(struct _tagAbsEleFixModule *t)
{//这个函数的作用就是将t中的所有内容按照模板号码的顺序排列整齐
	for(int i = 0;i < t->tianchuang_num;i ++)
		for(int k = 0; k < t->tianchuang_num - i;k++)
		{
			if (k >= t->tianchuang_num-1)
				continue;
			if(t->tianchuang_details[k].nTianChuangNo > t->tianchuang_details[k+1].nTianChuangNo)
			{
				struct _tagAbsEleFixDetails temp;
				memcpy(&temp,&(t->tianchuang_details[k]),sizeof(t->tianchuang_details[k]));
				memcpy(&(t->tianchuang_details[k]),&(t->tianchuang_details[k+1]),sizeof(t->tianchuang_details[i]));
				memcpy(&(t->tianchuang_details[k+1]),&temp,sizeof(temp));
			}
		}
}
void CAbsEleFixManage::OnBtnInsertToGraph() 
{
	if (m_nCurrentSel < 0)
	{
		AfxMessageBox("请选择一个天窗");
		return;
	}

	CTime ctStart,ctEnd;
	CDateTimeCtrl *pStartTime=(CDateTimeCtrl*)GetDlgItem(IDC_DTPICKER_BEGINTIME);
	pStartTime->GetTime(ctStart);
	CDateTimeCtrl *pEndtTime=(CDateTimeCtrl*)GetDlgItem(IDC_DTPICKER_ENDTIME);
	pEndtTime->GetTime(ctEnd);
	
	CTime tm = CTime::GetCurrentTime();
	ULONG nShift = m_pDoc->GetShiftId();
	// 第2班
	long nTime;
	if(nShift%2==0)
	{
		CTime tm1;
		tm1 = tm - CTimeSpan(0,tm.GetHour()-6,tm.GetMinute(),tm.GetSecond());
		nTime = tm1.GetTime();
	}
	else
	{
		CTime tm1;
		if(tm.GetHour() <= 23 && tm.GetHour() >=18 )
		    tm1 = tm - CTimeSpan(0,tm.GetHour()-18,tm.GetMinute(),tm.GetSecond());
		else if (tm.GetHour() <= 6 && tm.GetHour() >=0 )
			tm1 = tm - CTimeSpan(0,tm.GetHour()+6,tm.GetMinute(),tm.GetSecond());

		nTime = tm1.GetTime();
	}

	usStartTimeValue = ctStart.GetTime();
	usEndTimeValue = ctEnd.GetTime();

	if(usStartTimeValue >= usEndTimeValue)
	{
		AfxMessageBox("失败! 开始时间应比结束时间早");
		return;
	}

	if(nTime >= usStartTimeValue)
	{
		if (MessageBox("设定时间不在本班范围内，确定么？","电网检修绘图注意",MB_OKCANCEL)!=IDOK)
		{
			return;
		}
	}

	//////////////////////////////////////////////////////////////////////////
    station = m_AbsEleFixModule.tianchuang_details[m_nCurrentSel].nNode1Index;
    index = m_AbsEleFixModule.tianchuang_details[m_nCurrentSel].nNode2Index;
	nFenXiangPos = m_AbsEleFixModule.tianchuang_details[m_nCurrentSel].nSelectFenXiangFlag;
	nCheckUpAndDownFlag = m_AbsEleFixModule.tianchuang_details[m_nCurrentSel].nFaceSelectBlockFlag;
	station_no1 = m_AbsEleFixModule.tianchuang_details[m_nCurrentSel].nNode1StationNo;
	station_no2 = m_AbsEleFixModule.tianchuang_details[m_nCurrentSel].nNode2StationNo;
	//////////////////////////////////////////////////////////////////////////
    OnOK();
}

void CAbsEleFixManage::SetControlEnable(BOOL bFlag, BOOL bTianChuangNoFlag)
{
	CComboBox * pBeginAbs=(CComboBox*)GetDlgItem(IDC_COMBO_BEGIN_ABS);
	CComboBox * pEndAbs=(CComboBox*)GetDlgItem(IDC_COMBO_END_ABS);
	if(bFlag == FALSE)
	{
        pBeginAbs->SetCurSel(-1);
		pEndAbs->SetCurSel(-1);
	}
	pBeginAbs->EnableWindow(bFlag);
	pEndAbs->EnableWindow(bFlag);

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TIANCHUANG_NO);
	if(!pEdit)
		return;
	pEdit->EnableWindow(bTianChuangNoFlag);
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TIANCHUANG_NAME);
	if(!pEdit)
		return;
	pEdit->EnableWindow(bFlag);

	CButton *pRadio1 = (CButton *)GetDlgItem(IDC_RADIO_SJXJ) ;
	CButton *pRadio2 = (CButton *)GetDlgItem(IDC_RADIO_SJXY) ;
	CButton *pRadio3 = (CButton *)GetDlgItem(IDC_RADIO_SYXJ) ;
	CButton *pRadio4 = (CButton *)GetDlgItem(IDC_RADIO_SYXY) ;
	if (!pRadio1 || !pRadio2 || !pRadio3 || !pRadio4)
		return;
	pRadio1->EnableWindow(bFlag);
	pRadio2->EnableWindow(bFlag);
	pRadio3->EnableWindow(bFlag);
	pRadio4->EnableWindow(bFlag);
}

void CAbsEleFixManage::InitControl()
{
	CComboBox * pBeginAbs=(CComboBox*)GetDlgItem(IDC_COMBO_BEGIN_ABS);
	CComboBox * pEndAbs=(CComboBox*)GetDlgItem(IDC_COMBO_END_ABS);
	pBeginAbs->SetCurSel(-1);
	pEndAbs->SetCurSel(-1);
	pBeginAbs->EnableWindow(TRUE);
	pEndAbs->EnableWindow(TRUE);

	m_strTianChuangName.Empty();
	m_strTianChuangNo.Empty();
	UpdateData(FALSE);

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TIANCHUANG_NO);
	if(!pEdit)
		return;
	pEdit->EnableWindow(TRUE);
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TIANCHUANG_NAME);
	if(!pEdit)
		return;
	pEdit->EnableWindow(TRUE);

	CButton *pRadio1 = (CButton *)GetDlgItem(IDC_RADIO_SJXJ) ;
	CButton *pRadio2 = (CButton *)GetDlgItem(IDC_RADIO_SJXY) ;
	CButton *pRadio3 = (CButton *)GetDlgItem(IDC_RADIO_SYXJ) ;
	CButton *pRadio4 = (CButton *)GetDlgItem(IDC_RADIO_SYXY) ;
	if (!pRadio1 || !pRadio2 || !pRadio3 || !pRadio4)
		return;
	pRadio1->EnableWindow(TRUE);
	pRadio2->EnableWindow(TRUE);
	pRadio3->EnableWindow(TRUE);
	pRadio4->EnableWindow(TRUE);
	pRadio1->SetCheck(0);
	pRadio2->SetCheck(0);
	pRadio3->SetCheck(0);
	pRadio4->SetCheck(0);
	nFenXiangPos = 0;
}

void CAbsEleFixManage::OnBtnModifySave() 
{
	if (m_nCurrentSel < 0)
	{
		AfxMessageBox("请选择一个天窗");
		return;
	}
	
	
	UpdateData(TRUE);
	CListCtrl *pList=(CListCtrl*)GetDlgItem(IDC_LIST_CONTENT_ELE_FIX);
	CString strButtonAddText,strButtonModifyText;
	CButton *pButtonModify = (CButton *)GetDlgItem(IDC_BTN_MODIFY_SAVE);
	if(!pButtonModify)
		return;
	CButton *pButtonAdd = (CButton *)GetDlgItem(IDC_BTN_ADD_SAVE);
	if(!pButtonAdd)
		return;

	CButton *pCheckUp = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_UP);
	CButton *pCheckDown = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_DOWN);
	if (!pCheckUp || !pCheckDown)
		return ;

	pButtonModify->GetWindowText(strButtonModifyText);
	pButtonAdd->GetWindowText(strButtonAddText);

	if(strButtonModifyText == "修改")
	{
		SetControlEnable(TRUE,FALSE);
		pButtonModify->SetWindowText("修改保存");
		pButtonAdd->EnableWindow(FALSE);

			pCheckUp->EnableWindow(TRUE);
			pCheckDown->EnableWindow(TRUE);
	}
	else if (strButtonModifyText == "修改保存")
	{
		if(!SaveTianChuangInfoToFile(2))
			return;

		pList->DeleteAllItems();
		char szTemp[256];

		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT;
		int row = 0;
		int col = 0;
		int insertindex = 0;
		for(row = 0; row < m_AbsEleFixModule.tianchuang_num; row ++)
		{
			for(col = 0;col < 3; col ++)
			{
				lvi.iItem = row;
				if(col == 0)
				{
					lvi.iSubItem = col;
					sprintf(szTemp,"%d",m_AbsEleFixModule.tianchuang_details[row].nTianChuangNo);
					lvi.pszText = szTemp;
					if(m_AbsEleFixModule.tianchuang_details[row].nTianChuangNo == atoi((LPCSTR)m_strTianChuangNo))
						insertindex = row;
					pList->InsertItem(&lvi);
				}
				if(col == 1)
				{
					lvi.iSubItem = col;
					lvi.pszText = m_AbsEleFixModule.tianchuang_details[row].tianchuang_title_name;
					pList->SetItem(&lvi);
				}
				if(col == 2)
				{
					lvi.iSubItem = col;
					lvi.pszText = CheckResult(row);
					pList->SetItem(&lvi);
				}
			}
		}
		//nItemCount = m_typelist.GetItemCount();
		m_nCurrentSel = insertindex;
		pList->SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
		ShowRecord(m_nCurrentSel);

		pButtonModify->SetWindowText("修改");
		SetControlEnable(FALSE,FALSE);

		pCheckUp->EnableWindow(FALSE);
		pCheckDown->EnableWindow(FALSE);

		pButtonAdd->EnableWindow(TRUE);
	}
}

BOOL CAbsEleFixManage::SaveTianChuangInfoToFile(int nAddOrModify)//1 新建保存。2 修改保存
{
	//1 对输入信息进行判断
	if(m_strTianChuangNo.IsEmpty())
	{
		AfxMessageBox("天窗编号不能为空");
		return FALSE;
	}

	if(m_strTianChuangName.IsEmpty())
	{
		AfxMessageBox("天窗名称不能为空");
		return FALSE;
	}
	CComboBox * pBeginAbs=(CComboBox*)GetDlgItem(IDC_COMBO_BEGIN_ABS);
	CComboBox * pEndAbs=(CComboBox*)GetDlgItem(IDC_COMBO_END_ABS);
	int nSelectFirstAbsIndex = pBeginAbs->GetCurSel();
	int nSelectSecondAbsIndex = pEndAbs->GetCurSel();
	if (nSelectFirstAbsIndex == -1)
	{
		AfxMessageBox("请选择开始区间");
		return FALSE;
	}
	if (nSelectSecondAbsIndex == -1)
	{
		AfxMessageBox("请选择结束区间");
		return FALSE;
	}
	if (nSelectFirstAbsIndex > nSelectSecondAbsIndex)
	{
		AfxMessageBox("结束区间不能在开始区间上方");
		return FALSE;
	}

	if (nFenXiangPos < 1)
	{
		AfxMessageBox("请选择分相位置");
		return FALSE;
	}

	//2 判断保存文件的路径是否存在,命令号是否重复
	CFileFind m_Findfile;
	BOOL flag = TRUE;
	char szFileName[128];
	sprintf(szFileName,"%s\\general_tianchuang_file\\%02d\\*.cfg", m_pDoc->GetDataPath(), m_pDoc->m_nCurrentSectionNo);
	flag = m_Findfile.FindFile(szFileName);
	if(!flag)
	{
		int errcode = GetLastError();
		switch(errcode)
		{
		case 2://文件没有找到，目录为空
			break;
		case 3:
			AfxMessageBox("天窗模板保存路径不存在");
			return FALSE;
		default:
			return FALSE;
		}

		
	}
	
	if(nAddOrModify != 2)//当点击"修改保存"按钮后，该参数为2.不需要判断天窗模板号了
	{
		while(flag)
		{
			flag = m_Findfile.FindNextFile();
			CString filename,str;
			filename = m_Findfile.GetFileName();
			str.Format("%s.cfg",m_strTianChuangNo);
			if(filename == str)
			{	
				m_Findfile.Close();
				AfxMessageBox("此号码的天窗模板已经存在");
				return FALSE;
			}
		}
	}
	m_Findfile.Close();


	//3 取得要往文件中写入的字段信息 并写入文件
	m_FileRealBeginAbsIndex = m_FileSelectBeginAbsIndex = m_absInfo[nSelectFirstAbsIndex].nAbsIndex;
	m_FileRealEndAbsIndex = m_FileSelectEndAbsIndex = m_absInfo[nSelectSecondAbsIndex].nAbsIndex;

	CButton *pCheckUp = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_UP);
	CButton *pCheckDown = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_DOWN);
	if (!pCheckUp || !pCheckDown)
		return FALSE;
	BOOL bCheckFirstStation = TRUE;
	BOOL bCheckSecondStation = TRUE;

	m_FileFaceSelectBlockFlag = 0;
	m_FileRealBlockFlag = 0;
	m_FileFaceSelectBlockFlag = 0;

	if (/*m_FileSelectBeginAbsIndex == 0 && */pCheckUp->GetCheck() == 1)//开始区间选中第一个区间,并且复选框的“含上方站”也选中
	{
		bCheckFirstStation = FALSE;
		m_FileFaceSelectBlockFlag |= 0x01;
	}
	if (/*m_FileSelectEndAbsIndex == m_nAbsNum-1 && */pCheckDown->GetCheck() == 1)//结束区间选中最后一个区间,并且复选框的“含下方站”也选中
	{
		bCheckSecondStation = FALSE;
		m_FileFaceSelectBlockFlag |= 0x02;
	}
	

	m_FileNode1Index = m_absInfo[nSelectFirstAbsIndex].nNode1;
	m_FileNode1StationNo = m_absInfo[nSelectFirstAbsIndex].nNode1StationNo;
	m_FileNode2Index = m_absInfo[nSelectSecondAbsIndex].nNode2;
	m_FileNode2StationNo = m_absInfo[nSelectSecondAbsIndex].nNode2StationNo;
	/*当前兰州电网检修需要重新绘制，不进行转化
     m_FileRealBlockFlag 真实的封锁标记 现在不用
     m_FileSelectFenXiangFlag 0 上近下近 1 上近下远 2 上远下近 3 上远下远
	 站号不再进行转化
    */
	if (nFenXiangPos==1)//上近下近
		m_FileSelectFenXiangFlag = 1;

	else if(nFenXiangPos==2)//上近下远
		m_FileSelectFenXiangFlag = 2;

	else if (nFenXiangPos==3)//上远下近
		m_FileSelectFenXiangFlag = 3 ;

	else if (nFenXiangPos==4)//上远下远
		m_FileSelectFenXiangFlag = 4; 
	else
		m_FileSelectFenXiangFlag = 4;
	

	char szSavedFilePath[256];
	sprintf(szSavedFilePath,"%s\\general_tianchuang_file\\%02d\\%s.cfg", m_pDoc->GetDataPath(), m_pDoc->m_nCurrentSectionNo,m_strTianChuangNo);
	WriteTianChuangModuleFile(szSavedFilePath);

	//重新读取文件夹下的所有文件并显示
	memset(&m_AbsEleFixModule,0x00,sizeof(m_AbsEleFixModule));
	char path[128];
	sprintf(path,"%s\\general_tianchuang_file\\%02d\\*.cfg", m_pDoc->GetDataPath(), m_pDoc->m_nCurrentSectionNo);
	ReadTianChuangModuleFile(path,&m_AbsEleFixModule);
	SortTianChuangModuleFile(&m_AbsEleFixModule);

	return TRUE;
}

void CAbsEleFixManage::OnBtnCancelOper() 
{
	CString strButtonAddText,strButtonModifyText;
	CButton *pButtonModify = (CButton *)GetDlgItem(IDC_BTN_MODIFY_SAVE);
	if(!pButtonModify)
		return;
	CButton *pButtonAdd = (CButton *)GetDlgItem(IDC_BTN_ADD_SAVE);
	if(!pButtonAdd)
		return;

	pButtonModify->GetWindowText(strButtonModifyText);
	pButtonAdd->GetWindowText(strButtonAddText);

	if (strButtonModifyText == "修改保存")//说明此时已经点击过“修改”按钮，用户向取消这个操作
	{
		ShowRecord(m_nCurrentSel);

		pButtonModify->SetWindowText("修改");
		SetControlEnable(FALSE,FALSE);

		pButtonModify->EnableWindow(TRUE);
		pButtonAdd->EnableWindow(TRUE);
	}
	else if (strButtonAddText == "新建保存")//说明此时已经点击过“修改”按钮，用户向取消这个操作
	{
		ShowRecord(m_nCurrentSel);

		pButtonAdd->SetWindowText("新建");
		SetControlEnable(FALSE,FALSE);

		pButtonModify->EnableWindow(TRUE);
		pButtonAdd->EnableWindow(TRUE);
	}
	CButton *pCheckUp = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_UP);
	CButton *pCheckDown = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_DOWN);
	//pCheckUp->EnableWindow(FALSE);
	//pCheckDown->EnableWindow(FALSE);
	
}

void CAbsEleFixManage::OnBtnDelete() 
{
	CListCtrl *pList=(CListCtrl*)GetDlgItem(IDC_LIST_CONTENT_ELE_FIX);
	if(m_nCurrentSel == -1)
		AfxMessageBox("请选中要删除的选项");
	else
	{
		if (pList->GetItemCount() <= 0)
			return;

		CString strButtonAddText,strButtonModifyText;
		CButton *pButtonModify = (CButton *)GetDlgItem(IDC_BTN_MODIFY_SAVE);
		if(!pButtonModify)
			return;
		CButton *pButtonAdd = (CButton *)GetDlgItem(IDC_BTN_ADD_SAVE);
		if(!pButtonAdd)
			return;

		pButtonModify->GetWindowText(strButtonModifyText);
		pButtonAdd->GetWindowText(strButtonAddText);

		if (strButtonModifyText == "修改保存" || strButtonAddText == "新建保存")
		{
			AfxMessageBox("正在 新建 或 修改,不能删除!");
			return;
		}
		
		CString str;
		str.Format("您确实要删除编号为 %d 的天窗吗?",m_AbsEleFixModule.tianchuang_details[m_nCurrentSel].nTianChuangNo);
		if(IDOK != MessageBox(str, "删除天窗模板", MB_OKCANCEL))
			return;

		CString no,name;
		no = pList->GetItemText(m_nCurrentSel,0);
		name = pList->GetItemText(m_nCurrentSel,1);
		pList->DeleteItem(m_nCurrentSel);
	//	m_CommandContent.Empty();

		char szFileName[128];
		memset(szFileName,0,sizeof(szFileName));
		sprintf(szFileName,"%s\\general_tianchuang_file\\%02d\\%d.cfg", m_pDoc->GetDataPath(), m_pDoc->m_nCurrentSectionNo,
			m_AbsEleFixModule.tianchuang_details[m_nCurrentSel].nTianChuangNo);

		if(_unlink(szFileName))
			;
		else
			;

		m_nCurrentSel -= 1;
		if(m_nCurrentSel < 0)
		{
			m_nCurrentSel = 0;
			ShowRecord(0);
		}
		else
			ShowRecord(m_nCurrentSel);
		UpdateData(FALSE);

		//重新生成数组
		memset(&m_AbsEleFixModule,0x00,sizeof(m_AbsEleFixModule));
		char path[128];
		memset(path,0,sizeof(path));
		sprintf(path,"%s\\general_tianchuang_file\\%02d\\*.cfg", m_pDoc->GetDataPath(), m_pDoc->m_nCurrentSectionNo);
		ReadTianChuangModuleFile(path,&m_AbsEleFixModule);
		SortTianChuangModuleFile(&m_AbsEleFixModule);
	}
	
}

void CAbsEleFixManage::OnSelchangeComboBeginAbs() 
{
	UpdateData();
	CString strBeginAbs,strEndAbs;

	CComboBox * pBeginAbs=(CComboBox*)GetDlgItem(IDC_COMBO_BEGIN_ABS);
	CComboBox * pEndAbs=(CComboBox*)GetDlgItem(IDC_COMBO_END_ABS);
	int nBeginIndex = pBeginAbs->GetCurSel();
	int nEndIndex = pEndAbs->GetCurSel();
	
	m_strTianChuangName.Empty();
	if (nEndIndex != -1)
		m_strTianChuangName.Format("%s—%s供电臂",m_absInfo[nBeginIndex].szAbsName,m_absInfo[nEndIndex].szAbsName);
	else
		m_strTianChuangName.Format("%s—供电臂",m_absInfo[nBeginIndex].szAbsName);

	UpdateData(FALSE);

	CButton *pCheckUp = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_UP);
	CButton *pCheckDown = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_DOWN);
	if (!pCheckUp || !pCheckDown)
		return;
	if (nBeginIndex == -1 && nEndIndex == -1)
		return;

	/*if (nBeginIndex == nEndIndex)
	{
		pCheckUp->EnableWindow(TRUE);
		pCheckUp->SetCheck(0);
		pCheckDown->EnableWindow(TRUE);
		pCheckDown->SetCheck(0);
	}
	else */
	if(nBeginIndex == 0 && nEndIndex == m_nAbsNum-1)
	{
		pCheckUp->EnableWindow(TRUE);
		pCheckUp->SetCheck(0);
		pCheckDown->EnableWindow(TRUE);
		pCheckDown->SetCheck(0);
	}
	else if(nBeginIndex == 0 && nEndIndex != m_nAbsNum-1)
	{
		pCheckUp->EnableWindow(TRUE);
		pCheckUp->SetCheck(0);
		pCheckDown->EnableWindow(FALSE);
		pCheckDown->SetCheck(0);
	}
	else if(nEndIndex == m_nAbsNum-1 && nBeginIndex != 0)
	{
		pCheckUp->EnableWindow(FALSE);
		pCheckUp->SetCheck(0);
		pCheckDown->EnableWindow(TRUE);
		pCheckDown->SetCheck(0);
	}
	else
	{
		pCheckUp->EnableWindow(FALSE);
		pCheckUp->SetCheck(0);
		pCheckDown->EnableWindow(FALSE);
		pCheckDown->SetCheck(0);
	}
	
}

void CAbsEleFixManage::OnSelchangeComboEndAbs() 
{
	UpdateData();
	CString strBeginAbs,strEndAbs;
	CComboBox * pBeginAbs=(CComboBox*)GetDlgItem(IDC_COMBO_BEGIN_ABS);
	CComboBox * pEndAbs=(CComboBox*)GetDlgItem(IDC_COMBO_END_ABS);
	int nBeginIndex = pBeginAbs->GetCurSel();
	int nEndIndex = pEndAbs->GetCurSel();
	
	m_strTianChuangName.Empty();
	if (nBeginIndex != -1)
		m_strTianChuangName.Format("%s—%s供电臂",m_absInfo[nBeginIndex].szAbsName,m_absInfo[nEndIndex].szAbsName);
	else
		m_strTianChuangName.Format("—%s供电臂",m_absInfo[nEndIndex].szAbsName);

	UpdateData(FALSE);
	
	CButton *pCheckUp = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_UP);
	CButton *pCheckDown = (CButton *)GetDlgItem( IDC_CHECK_INCLUDE_DOWN);
	if (!pCheckUp || !pCheckDown)
		return;
	if (nBeginIndex == -1 && nEndIndex == -1)
		return;

	/*if (nBeginIndex == nEndIndex)
	{
		pCheckUp->EnableWindow(TRUE);
		pCheckUp->SetCheck(0);
		pCheckDown->EnableWindow(TRUE);
		pCheckDown->SetCheck(0);
	}
	else */
	if(nBeginIndex == 0 && nEndIndex == m_nAbsNum-1)
	{
		pCheckUp->EnableWindow(TRUE);
		pCheckUp->SetCheck(0);
		pCheckDown->EnableWindow(TRUE);
		pCheckDown->SetCheck(0);
	}
	else if(nEndIndex == m_nAbsNum-1 && nBeginIndex != 0)
	{
		pCheckUp->EnableWindow(FALSE);
		pCheckUp->SetCheck(0);
		pCheckDown->EnableWindow(TRUE);
		pCheckDown->SetCheck(0);
	}
	else if(nBeginIndex == 0 && nEndIndex != m_nAbsNum-1)
	{
		pCheckUp->EnableWindow(TRUE);
		pCheckUp->SetCheck(0);
		pCheckDown->EnableWindow(FALSE);
		pCheckDown->SetCheck(0);
	}
	else
	{
		pCheckUp->EnableWindow(FALSE);
		pCheckUp->SetCheck(0);
		pCheckDown->EnableWindow(FALSE);
		pCheckDown->SetCheck(0);
	}
}

void CAbsEleFixManage::OnBnClickedBtnClose()//关闭按钮
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

// 检查配置数据和当前底图是否匹配
char* CAbsEleFixManage::CheckResult(int row)
{
	static char buf[256];
	memset(buf, 0, sizeof(buf));
	do
	{	
		int index1 = m_AbsEleFixModule.tianchuang_details[row].nNode1Index;
		int index2 = m_AbsEleFixModule.tianchuang_details[row].nNode2Index;
		int station1 = m_AbsEleFixModule.tianchuang_details[row].nNode1StationNo;
		int station2 = m_AbsEleFixModule.tianchuang_details[row].nNode2StationNo;
		
		int flag=0;
		if(index1<0 || index1>=m_pDoc->m_nDispDistanceNum)
		{
			sprintf(buf, "站序1不在底图范围");
			flag++;
		}
		if(index2<0 || index2>=m_pDoc->m_nDispDistanceNum)
		{
			sprintf(buf, "站序2不在底图范围");
			flag++;
		}

		if(flag==2)
		{
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "站序1,2均不在底图范围");
			break;
		}
		else if(flag==1)
		{
			break;
		}

		flag=0;
		if(m_pDoc->m_sDispDistance[index1].station_number!=station1)
		{
			sprintf(buf, "站序1和站号不一致");
			flag++;
		}
		if(m_pDoc->m_sDispDistance[index2].station_number!=station2)
		{
			sprintf(buf, "站序2和站号不一致");
			flag++;
		}

		if(flag==2)
		{
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "站序1,2均和站号不一致");
			break;
		}
		else if(flag==1)
		{
			break;
		}

		sprintf(buf, "正常");
	}while(0);

	return buf;
}
