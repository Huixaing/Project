
// SelectScheduleTrain.cpp : implementation file
//

#include "stdafx.h"
#include "oscalls.h"
#include "sysshare.h"
#include "resource.h"
#include "SelectScheduleTrain.h"
#include "function.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include "tg_work_schdule_holder.h"
#include "tg_basicpara.h"
#include "gpcdef.h"
#include "tg_function_config.h"
#include "utility\mmi_function_config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern int QueryTrainType(char *train);
extern CMMIFunctionConfig *GetMMIFunctionConfig();
/////////////////////////////////////////////////////////////////////////////
// CSelectScheduleTrain dialog

#define SetFlagTrainExist(trn_flag)          (trn_flag |= 0x01)    //bit 0:   1 
#define SetFlagTrainNotExist(trn_flag)       (trn_flag &= ~0x01)   //bit 0:   0 
#define SetFalgTrainNormal(trn_flag)        (trn_flag &= 0xF9)     //bit 1,2: 0 
#define SetFlagTrainLongIncrease(trn_flag)  (trn_flag &= 0xF9, trn_flag |= 0x02)   //bit 1,2:  1 
#define SetFlagTrainLongStop(trn_flag)      (trn_flag &= 0xF9, trn_flag |= 0x04)   //bit 1,2:  2 
#define IsFlagTrainExist(trn_flag)           ((trn_flag & 0x01) == 0x01) 
#define IsFlagTrainNormal(trn_flag)         ((trn_flag & 0x06) == 0x00) 
#define IsFlagTrainLongIncrease(trn_flag)   ((trn_flag & 0x06) == 0x02) 
#define IsFlagTrainLongStop(trn_flag)       ((trn_flag & 0x06) == 0x04) 

#define SELECTSCHDTRAINCOLUMNS		6
_TCHAR *_gszSelectSchdTrainLabel[SELECTSCHDTRAINCOLUMNS] =
{
	_T("序号"),_T("车次号"),_T("开始时间"),_T("已存在"), _T("长期加开"), _T("长期停运")//,_T("运行标志"),_T("列车类型")
};


int _gnSelectSchdTrainFmt[SELECTSCHDTRAINCOLUMNS] = 
{
	LVCFMT_LEFT,LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT//, LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_CENTER
};

int _gnSelectSchdTrainWidth[SELECTSCHDTRAINCOLUMNS] = 
{
	40,70,100,60,100,100//,90,90
};

CSelectScheduleTrain::CSelectScheduleTrain(CWorkScheduleHolder *pHdr,CWnd* pParent/*=NULL*/)
: CDialog(CSelectScheduleTrain::IDD, pParent)
{
	m_check = FALSE;
	m_checksorttrainid = FALSE;

	m_pHolder = pHdr;
	m_nPermitOperationFlag = 0;
	m_nCount = 0;
	m_check = FALSE;
	intArray = NULL;//add by cf after 9-4
	m_listcount = 0 ;//add by cf after 9-9
	m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;
	m_bEnableChoiceSchedule=TRUE;
}


void CSelectScheduleTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectScheduleTrain)
	DDX_Check(pDX, IDC_CHECK1, m_check);
	DDX_Check(pDX, IDC_CHECK_SORTTRAINID, m_checksorttrainid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectScheduleTrain, CDialog)
	//{{AFX_MSG_MAP(CSelectScheduleTrain)
	ON_BN_CLICKED(IDC_SBT_ACK, OnSbtAck)
	ON_BN_CLICKED(IDC_SBT_CANCEL, OnSbtCancel)
	ON_BN_CLICKED(IDC_SBT_DELETE, OnSbtDelete)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SBT_FREIGHT, OnSbtFreight)
	ON_BN_CLICKED(IDC_SBT_KECHE, OnSbtKeche)
	ON_BN_CLICKED(IDC_SBT_RECEIVE, OnSbtReceive)
	ON_BN_CLICKED(IDC_SBT_START, OnSbtStart)
	ON_BN_CLICKED(IDC_SBT_DOWNDIR, OnSbtDowndir)
	ON_BN_CLICKED(IDC_SBT_UPDIR, OnSbtUpdir)
	ON_CBN_SELCHANGE(IDC_SBT_STATIONLIST, OnSelchangeSbtStationlist)
	ON_BN_CLICKED(IDC_SBT_ALLDIR, OnSbtAlldir)
	ON_CBN_SELCHANGE(IDC_SBT_ENDTIME, OnSelchangeSbtEndtime)
	ON_CBN_SELCHANGE(IDC_SBT_STARTTIME, OnSelchangeSbtStarttime)
	ON_BN_CLICKED(IDC_SBT_GET, OnSbtGet)
	ON_BN_CLICKED(IDC_CHECK_SORTTRAINID, OnCheckSorttrainid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectScheduleTrain message handlers

BOOL CSelectScheduleTrain::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CTG_ScheduleInfo schd;
	m_pHolder->GetWorkScheduleInfo(schd); 

	int i,j;
	m_ackButton.SubclassDlgItem(IDC_SBT_ACK,this);
	m_cancelButton.SubclassDlgItem(IDC_SBT_CANCEL,this);

	m_list.SubclassDlgItem(IDC_SBT_TRAINLIST,this);
	m_list.SetHeaderLabel(SELECTSCHDTRAINCOLUMNS,_gszSelectSchdTrainLabel,
		_gnSelectSchdTrainWidth,_gnSelectSchdTrainFmt);

	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_STARTTIME);
	CString text;
	CTime ctm=CTime::GetCurrentTime();
	int nCurrentHour = ctm.GetHour();

	if(schd.GetShiftNo()  == 2)
	{
		j=6;
	}
	else
	{
		j=18;
	}
	for(  i=0;i < 23;i++)
	{
		text.Format("%02d",(j+i)%24);
		pComboBox->AddString(text);
	}
	pComboBox->SetCurSel((nCurrentHour-j+24)%24);
	m_nStartTime = nCurrentHour;

	pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_ENDTIME);
	for(i= 0;i < 23;i++)
	{
		text.Format("%02d",(i+j)%24);
		pComboBox->AddString(text);
	}

	pComboBox->SetCurSel(((nCurrentHour+3)-j+24)%24);

	pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_STATIONLIST);

	int nStationIndexFlag[32];
	memset(nStationIndexFlag,0x00,sizeof(nStationIndexFlag));
	int nCount = 0;

	pComboBox->AddString("全部区段");
	pComboBox->SetItemData(nCount,0);
	nCount += 1;

	int nStationNum,nStationList[512];
	nStationNum = sizeof(nStationList)/sizeof(nStationList[0]);
	if(GetTGFunctionConfig()->GetGPCDEFx()->GetStationList(schd.m_nSectionID,nStationNum,nStationList)>0)
	{
		for(i=0;i<nStationNum;i++)
		{
			pComboBox->AddString(GetTGFunctionConfig()->GetGPCDEFx()->GetStationName(nStationList[i]));
			pComboBox->SetItemData(nCount,(DWORD)nStationList[i]);
			nCount += 1;
		}
	}

	//end of cf 

	pComboBox->SetCurSel(0);

	CButton *pButton = (CButton *)GetDlgItem(IDC_SBT_ALLDIR);
	pButton->SetCheck(1);

	pButton = (CButton *)GetDlgItem(IDC_SBT_KECHE);
	pButton->SetCheck(1);

	pButton = (CButton *)GetDlgItem(IDC_SBT_FREIGHT);
	if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_ADD_TRAIN_SEL_GOOD)>0)
		pButton->SetCheck(1); // 基本图加车默认加客车
	else
	    pButton->SetCheck(0); // 基本图加车默认加客车

	pButton = (CButton *)GetDlgItem(IDC_SBT_RECEIVE);
	if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_ADD_TRAIN_SEL_NONE_START)>0)
		pButton->SetCheck(1);
	else
		pButton->SetCheck(0);

	pButton = (CButton *)GetDlgItem(IDC_SBT_START);
	pButton->SetCheck(1);
	pButton = (CButton *)GetDlgItem(IDC_SBT_BASIC_TRAIN);
	pButton->SetCheck(1);

	if(m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		((CButton *)GetDlgItem(IDC_RADIO_SHIFT_SCHEDULE))->SetCheck(TRUE); 
	else
		((CButton *)GetDlgItem(IDC_RADIO_BASIC_SCHEDULE))->SetCheck(TRUE); 
	m_nCurrentSel = -1;


	if(m_bEnableChoiceSchedule)
	{
		GetDlgItem(IDC_RADIO_SHIFT_SCHEDULE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_BASIC_SCHEDULE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_RADIO_SHIFT_SCHEDULE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_BASIC_SCHEDULE)->EnableWindow(FALSE);
	}
	InitDlgPosition(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSelectScheduleTrain::PreTranslateMessage(MSG* lpMsg) 
{
	// 	m_ctrlTooltipWnd.RelayEvent(lpMsg);	
	if(lpMsg->message == WM_LBUTTONDOWN)  
	{
		do 
		{
			if(lpMsg->hwnd != m_list.GetSafeHwnd( ))
				break;
			CPoint point;
			point.x = (lpMsg->lParam) & 0x0000ffff;
			point.y = ((lpMsg->lParam) & 0xffff0000) >> 16;
			UINT uFlags = 0;
			int nHitItem = m_list.HitTest(point, &uFlags);

			BOOL bHit = FALSE;
			if ((uFlags & LVHT_ONITEM) && (m_list.GetStyle() & LVS_OWNERDRAWFIXED))
			{
				CRect rect;
				m_list.GetItemRect(nHitItem, rect, LVIR_BOUNDS);

				// check if hit was on a state icon
				if (point.x >= rect.left && point.x <= rect.right 
					&& point.y >= rect.top && point.y <= rect.bottom)
					bHit = TRUE;
			} 

			if (!bHit)
				break;
			if(nHitItem == m_nCurrentSel)
				break;

			m_nCurrentSel = nHitItem;
		}while(0);
	}

	return CDialog::PreTranslateMessage(lpMsg);
}

void CSelectScheduleTrain::OnSbtAck() 
{
	sysprint(0,PMTG.ThreadId,"调度员生成图定计划,确定生成, BEGIN");
	CDialog::OnOK( );	
}

void CSelectScheduleTrain::OnSbtCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel( );
}

void CSelectScheduleTrain::OnSbtDelete() 
{
	do 
	{
		if(m_nCurrentSel < 0 || m_nCurrentSel >= m_nCount)
			break;
		TRAIN_INDEX tindex=m_list.GetItemData(m_nCurrentSel);
		int i;
		for(i=0;i<m_nCount;i++)
		{
			if(info[i].train_index==tindex)
				break;
		}
		if(i>=m_nCount)
			return;
		for(;i<m_nCount-1;i++)
		{
			info[i]=info[i+1];
		}
		m_list.DeleteItem(m_nCurrentSel);
		m_nCount -= 1;

		if(m_nCurrentSel >= m_list.GetItemCount()-1)
			m_nCurrentSel = m_list.GetItemCount()-1;

		//add by cf after 9-4
		CString text;

		for(int i=0;i<m_nCount;i++)
		{
			text.Format("%d",i+1);

			m_list.SetItemText(i,0,text);

		}

		m_list.EnsureVisible(m_nCurrentSel,false);
		m_list.SetCurSel(m_nCurrentSel);
	}while(0);
}

void CSelectScheduleTrain::OnDestroy() 
{
	CDialog::OnDestroy();

	if(intArray)
		delete[] intArray;

}

void CSelectScheduleTrain::OnSbtFreight() 
{

}

void CSelectScheduleTrain::OnSbtKeche() 
{

}

void CSelectScheduleTrain::OnSbtReceive() 
{
}

void CSelectScheduleTrain::OnSbtStart() 
{
}

void CSelectScheduleTrain::OnSbtDowndir() 
{
}

void CSelectScheduleTrain::OnSbtUpdir() 
{
}

void CSelectScheduleTrain::OnSbtAlldir() 
{
}

void CSelectScheduleTrain::OnSelchangeSbtEndtime() 
{
}

void CSelectScheduleTrain::OnSelchangeSbtStarttime() 
{
}

void CSelectScheduleTrain::OnSelchangeSbtStationlist() 
{
}


void CSelectScheduleTrain::OnSbtGet() 
{
	//get selected tarin  conditions
	int nTrainClass=0,nStartType=0;
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_STATIONLIST);
	int nSel = pComboBox->GetCurSel( );
	if(nSel == CB_ERR)
		return;
	int nStationNo = pComboBox->GetItemData(nSel);

	//delete all old item
	m_list.DeleteAllItems ();
	m_nCount = 0;

	CButton *pButton = (CButton *)GetDlgItem(IDC_SBT_FREIGHT);
	int nFreightFlag = pButton->GetCheck( );
	pButton = (CButton *)GetDlgItem(IDC_SBT_KECHE);
	int nPassengerFlag = pButton->GetCheck( );
	if(nPassengerFlag == 1 && nFreightFlag == 1)
		nTrainClass = 2;
	else if(nPassengerFlag == 1 && nFreightFlag == 0)
		nTrainClass = 0;
	else if(nPassengerFlag == 0 && nFreightFlag == 1)
		nTrainClass = 1;
	else if(nPassengerFlag == 0 && nFreightFlag == 0)
	{
		AfxMessageBox("请注意, 你没有选择车种");
		return;
	}
	pButton = (CButton *)GetDlgItem(IDC_SBT_START);
	int nStartFlag = pButton->GetCheck( );
	pButton = (CButton *)GetDlgItem(IDC_SBT_RECEIVE);
	int nReceiveFlag = pButton->GetCheck( );
	pButton = (CButton *)GetDlgItem(IDC_SBT_BASIC_TRAIN);
	int nTrainFlag=0;
	if(pButton->GetCheck())
		nTrainFlag = 0;
	else
		nTrainFlag = 1;

	if(nStartFlag == 1 && nReceiveFlag == 1)
		nStartType = 2;
	else if(nStartFlag == 1 && nReceiveFlag == 0)
		nStartType = 0;
	else if(nStartFlag == 0 && nReceiveFlag == 1)
		nStartType = 1;
	else if(nStartFlag == 0 && nReceiveFlag == 0)
	{
		AfxMessageBox("请注意, 你没有选择运行类型");
		return;
	}

	pButton = (CButton *)GetDlgItem(IDC_SBT_UPDIR);
	int nDirFlag = pButton->GetCheck( );
	if(nDirFlag == 0)
	{//非上行
		pButton = (CButton *)GetDlgItem(IDC_SBT_DOWNDIR);
		nDirFlag = pButton->GetCheck( );
		if(nDirFlag == 1)
			nDirFlag = 1;//下行
		else
			nDirFlag = 2;//上下行
	}
	else
		nDirFlag = 0;// up direction

	pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_STARTTIME);
	CString time_str;
	pComboBox->GetWindowText (time_str);
	int nStartTime = atoi(time_str);
	m_nStartTime = nStartTime;

	pComboBox = (CComboBox *)GetDlgItem(IDC_SBT_ENDTIME);
	pComboBox->GetWindowText (time_str);
	int nEndTime = atoi(time_str);

	TGBasicTrainListRequest    mMsgBasicTrainListInit;	
	int count =0;

	mMsgBasicTrainListInit.station = nStationNo; //3

	if(((CButton *)GetDlgItem(IDC_RADIO_SHIFT_SCHEDULE))->GetCheck()==1)
		m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT;
	else
		m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;

	if(m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC) // 基本图没有日期
	{
		mMsgBasicTrainListInit.stime = nStartTime; //4 e.g; 8点
		mMsgBasicTrainListInit.etime = nEndTime; //5 e.g: 9点
		GpcLogTrainMessage(0, 0,"调度员基本图加车时间范围 %d %d",mMsgBasicTrainListInit.stime, mMsgBasicTrainListInit.etime);
	}
	else // 班计划有日期，计算小时
	{
		CTime curtm=CTime::GetCurrentTime();
		CTime todaytm=CTime(curtm.GetYear(),curtm.GetMonth(),curtm.GetDay(),0,0,0);
		long  todayhour=todaytm.GetTime()/3600;
		long  curhour=curtm.GetTime()/3600;
		nStartTime+=todayhour;
		nEndTime+=todayhour;
		if(nStartTime>curhour+12) nStartTime-=24;
		else if(nStartTime<curhour-6) nStartTime+=24;
		if(nStartTime>curhour+12 || nStartTime<curhour-6)
		{
			MessageBox("时间范围不合理,起始时间应该在当前时间附近!","提示");
			return;
		}
		if(nEndTime<nStartTime)
			nEndTime+=24;
		if(nEndTime<nStartTime || nEndTime>nStartTime+12)
		{
			MessageBox("时间范围不能超过12小时!","提示");
			return;
		}

		mMsgBasicTrainListInit.stime = nStartTime; //4 e.g; 8点
		mMsgBasicTrainListInit.etime = nEndTime; //5 e.g: 9点
		CTime tm1,tm2;
		tm1=CTime(nStartTime*3600);
		tm2=CTime(nEndTime*3600);

		GpcLogTrainMessage(0, 0,"调度员日班图加车时间范围 %s %s",tm1.Format("%Y-%m-%d %Hhr"), tm2.Format("%Y-%m-%d %Hhr"));
		// 此处时间有问题
	}


	mMsgBasicTrainListInit.train_type = 
		( TGBasicTrainListRequest::INIT_TRAINTYPE)nTrainClass; //6
	mMsgBasicTrainListInit.direction =	( TGBasicTrainListRequest::INIT_DIRECTION)nDirFlag; //7

	mMsgBasicTrainListInit.flag =	( TGBasicTrainListRequest::INIT_START_ACCEPT)nStartType; //8

	mMsgBasicTrainListInit.hat_flag =	( TGBasicTrainListRequest::INIT_HAT_FLAG)nTrainFlag; //9



	int train_count=sizeof(info)/sizeof(info[0]);
	BOOL bStartOnly = FALSE;
	if(m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		bStartOnly = FALSE; 
	else
		bStartOnly = TRUE;
	// 基本图或日班计划上图时，列车数目超出最大数目判断处理
	int nResult=m_pHolder->SelectSchdTrain(mMsgBasicTrainListInit,info,&train_count,(CTG_ScheduleInfo::SCHEDULE_TYPE)m_nSelectFromSchdType,bStartOnly);
	if(nResult==1)
	{
		m_nCount=train_count;
		GpcLogTrainMessage(PMTG.ThreadId,gpcdef.GetConsoleid(),"调度员生成图定计划/读取列表, 车种:%d,运行类型:%d,方向:%d,SCHD %d",nTrainClass,nStartType,nDirFlag,m_nSelectFromSchdType);
		ShowSelectScheTrains();
	}
	else if(nResult==-1)
	{
		MessageBox("选中列车超出了最大容量,请缩减时间范围,重新导入车次");
	}
	else
	{
		MessageBox("无法读取车次");
	}
}
void  CSelectScheduleTrain::ShowSelectScheTrains()
{
	m_list.DeleteAllItems( );

	this->placeAnOrder();
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT ;
	lvi.iSubItem = 0;
	CString text;
	char num_buffer[10];

	m_listcount = m_nCount;

	for(int i=0;i < m_nCount;i++)
	{
		lvi.iItem = i;
		itoa(i+1,num_buffer,10);

		lvi.pszText = (char *)(LPCTSTR)num_buffer;
		m_list.InsertItem(&lvi);
		m_list.SetItemData(i,info[intArray[i]].train_index);


		//add by cf after 9-4
		text.Format("%s",info[intArray[i]].train_id);
		m_list.SetItemText(i,1,text);


		if(info[intArray[i]].exist_train == CLBTH::MsgBasicTrainListReply::BT_EXIST)
		{
			text.Format("√");
		}
		else
		{
			text.Format(" ");
		}
		m_list.SetItemText(i,3,text);

		if(info[intArray[i]].add_train == (ushort)CLBTH::MsgBasicTrainListReply::BT_LONG_INCREASE)			
		{
			text.Format("√");
		}
		else
		{
			text.Format(" ");
		}
		m_list.SetItemText(i,4,text);

		if(info[intArray[i]].add_train == (ushort)CLBTH::MsgBasicTrainListReply::BT_LONG_STOP)
		{
			text.Format("√");
		}
		else
		{
			text.Format(" ");
		}
		m_list.SetItemText(i,5,text);

		//end of cf 
		text.Format("%s",OsConvertTimetoString(info[intArray[i]].start_time) );
		m_list.SetItemText(i,2,text);
	}

}

void CSelectScheduleTrain::placeAnOrder()
{
	struct TGSelectedBasicTrain *first;
	struct TGSelectedBasicTrain *second;

	if(m_nCount == 0)
		return;

	if(intArray)
	{
		delete[] intArray;
		intArray = NULL;
	}
	intArray = new int[m_nCount];

	int i,j;
	for(i=0;i<m_nCount;i++)
	{
		intArray[i] = i;
	}
	int temp;
	for(i=0;i<m_nCount-1;i++)
	{
		first = &info[intArray[i]];
		for(j=i+1;j<m_nCount;j++)
		{
			second = &info[intArray[j]];

			//车次排序还是时间排序
			if(m_checksorttrainid)//车次排序
			{
				CString str1,str2;
				str1.Format("%s",first->train_id);
				str2.Format("%s",second->train_id);

				if(str1 > str2)
				{
					temp = intArray[i];
					intArray[i] = intArray[j];
					intArray[j] = temp;
					first = &info[intArray[i]];
				}

			}
			else
			{
				if(first->start_date > second->start_date)
				{
					temp = intArray[i];
					intArray[i] = intArray[j];
					intArray[j] = temp;
					first = &info[intArray[i]];
				}
				else
				{
					if((first->start_date == second->start_date)&&(first->start_time> second->start_time))
					{
						temp = intArray[i];
						intArray[i] = intArray[j];
						intArray[j] = temp;
						first = &info[intArray[i]];
					}
				}
			}
		}
	}
}

void CSelectScheduleTrain::OnCheckSorttrainid() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	//排序
	placeAnOrder();
	ShowTrainList();
}

void CSelectScheduleTrain::ShowTrainList()
{
	m_list.DeleteAllItems( );
	//插入项
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT ;
	lvi.iSubItem = 0;
	CString text;
	char num_buffer[10];

	m_listcount = m_nCount;

	int i;
	for(i=0;i < m_nCount;i++)
	{
		lvi.mask=LVIF_TEXT|LVIF_PARAM;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.lParam = intArray[i];

		itoa(i+1,num_buffer,10);
		lvi.pszText = (char *)(LPCTSTR)num_buffer;
		int ipos = m_list.InsertItem(&lvi);

		m_list.SetItemData(i,info[intArray[i]].train_index);

		text.Format("%s",info[intArray[i]].train_id);

		strcpy(lvi.pszText,text);
		lvi.iItem = ipos;
		lvi.iSubItem = 1;
		m_list.SetItem(&lvi);

		m_list.SetItemText(i,1,text);

		if(info[intArray[i]].exist_train == CLBTH::MsgBasicTrainListReply::BT_EXIST)
		{
			text.Format("√");
		}
		else
		{
			text.Format(" ");
		}

		lvi.iItem = ipos;
		strcpy(lvi.pszText,text);
		lvi.iSubItem = 3;
		m_list.SetItem(&lvi);

		m_list.SetItemText(i,3,text);

		if(info[intArray[i]].add_train == (ushort)CLBTH::MsgBasicTrainListReply::BT_LONG_INCREASE)
		{ 
			text.Format("√");
		}
		else 
		{
			text.Format(" ");
		}
		m_list.SetItemText(i,4,text);

		if(info[intArray[i]].add_train == (ushort)CLBTH::MsgBasicTrainListReply::BT_LONG_STOP)
		{
			text.Format("√");
		}
		else
		{
			text.Format(" ");
		}
		m_list.SetItemText(i,5,text);

		text.Format("%s",OsConvertTimetoString(info[intArray[i]].start_time) );

		lvi.iItem = ipos;
		strcpy(lvi.pszText,text);
		lvi.iSubItem = 2;
		m_list.SetItem(&lvi);


		m_list.SetItemText(i,2,text);
	}
}
