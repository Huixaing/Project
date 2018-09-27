// ManualReportHistory.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "oscalls.h"
#include "sysshare.h"
#include "ManualReportHistory.h"
#include "function.h"
#include "tg_schedule_base.h"
#include "schd_tgmmi.h"
#include ".\manualreporthistory.h"
#include ".\tg_logic\tg_function_config.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define REPORTHISTORYCOLUMNS		5
_TCHAR *_gszReportHistoryLabel[REPORTHISTORYCOLUMNS] =
{
	_T("车次"), _T("计划到达"), _T("计划出发"),_T("实际到达"),_T("实际出发")
};

int _gnReportHistoryFmt[REPORTHISTORYCOLUMNS] = 
{
	LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_CENTER
};

int _gnReportHistoryWidth[REPORTHISTORYCOLUMNS] = 
{
	100,60,60,60,60
};

/////////////////////////////////////////////////////////////////////////////
// CManualReportHistory dialog


CManualReportHistory::CManualReportHistory(CWintgViewMMI *pMMI,int sectionno,int schetype, CWnd* pParent)
: CDialog(CManualReportHistory::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManualReportHistory)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pMMI=pMMI;	

	m_nCurrentStationNo = 0;
	m_nCurrentSectionNo=sectionno;
	m_nScheduleType=schetype;
	
	mrprec.clear();
}

void CManualReportHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManualReportHistory)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CManualReportHistory, CDialog)
	//{{AFX_MSG_MAP(CManualReportHistory)
	ON_BN_CLICKED(IDC_MRP_ACK, OnMrpAck)
	ON_BN_CLICKED(IDC_MRP_ALLDIR, OnMrpAlldir)
	ON_BN_CLICKED(IDC_MRP_DOWNDIR, OnMrpDowndir)
	ON_BN_CLICKED(IDC_MRP_UPDIR, OnMrpUpdir)
	ON_CBN_SELCHANGE(IDC_MRP_STATIONLIST, OnSelchangeMrpStationlist)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_REAL, OnBnClickedCheck1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManualReportHistory message handlers

void CManualReportHistory::OnMrpAck() 
{
	mrprec.clear();
	CDialog::OnOK();	
	delete this;
}

void CManualReportHistory::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mrprec.clear();
	CDialog::OnClose();
	delete this;
}

void CManualReportHistory::SetInfo(int station)
{
	mrprec.clear();

	CTG_TrainPtrGroup group;
	CTG_ScheduleInfo schd;
	schd.m_nSectionID=m_nCurrentSectionNo;
	schd.m_nType=(CTG_ScheduleInfo::SCHEDULE_TYPE)m_nScheduleType;
	CTG_TrainScheduleBase *pdata=m_pMMI->GetDataStore(schd);
	if(!pdata)
		return;
	pdata->FindTrains(group,station); 

	for(int i=0;i<group.GetTrainCount();i++)
	{
		const CTG_SingleTrain *ptrain=group.GetTrainPtr(i); 
		for(int i=0; i<ptrain->GetTrainRecordCount(); i++)
		{
			const CTG_TrainRecord *precord=ptrain->GetTrainRecordByRecIndex_Const(i);
			if(precord->GetStationNo()==station)
			{
				CButton* pBtn=(CButton*)GetDlgItem(IDC_CHECK_REAL);
				if(pBtn!=NULL && pBtn->GetCheck()==1)
				{
				}
				else
				{
					if(precord->IsStartRec())
					{
						if(precord->IsActualDepartTimeRecord())
							continue;
					}
					else
					{
						if(precord->IsTerminalRec())
						{
							if(precord->IsActualArriveTimeRecord())
								continue;
						}
						else
						{
							if(precord->IsActualArriveTimeRecord() && precord->IsActualDepartTimeRecord())
								continue;
						}
					}
				}
			

				tagManualReportRec rec;
				
				rec.uTrainIndex = ptrain->GetTrainIndex();
				rec.rec_index = i;
				rec.adjust_status = precord->m_data.adjust_status;
				strcpy(rec.lpszTrainID,ptrain->GetTrainID());
				rec.nPlanArrivalTime =minute_time(precord->m_data.plan_arrive,&rec.nPlanArrivalDate);  
				rec.nPlanDepartTime =minute_time(precord->m_data.plan_depart,&rec.nPlanDepartDate);   

				rec.nDirectFlag =ptrain->IsUpDirection()? UP_DIRECT:DOWN_DIRECT;

				rec.nActualArrivalTime =minute_time(precord->m_data.arrive,&rec.nActualArrivalDate);  
				rec.nActualDepartTime =minute_time(precord->m_data.depart,&rec.nActualDepartDate);   
				
				mrprec.push_back(rec);
			}
		}
	}
}

void CManualReportHistory::OnSelchangeMrpStationlist() 
{
	// TODO: Add your control notification handler code here
	do 
	{
		CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_MRP_STATIONLIST);
		if(pComboBox == NULL)
			break;
		int nSel = pComboBox->GetCurSel();
		if(nSel == CB_ERR)
			break;
		m_nCurrentStationNo = pComboBox->GetItemData(nSel);
		// 提取报点记录
		SetInfo(m_nCurrentStationNo);

		int nDirectFlag = UP_DIRECT;
		do 
		{
			CButton *pButton = (CButton *)GetDlgItem(IDC_MRP_UPDIR);
			if(pButton->GetCheck())
				break;
			nDirectFlag = DOWN_DIRECT;
			pButton = (CButton *)GetDlgItem(IDC_MRP_DOWNDIR);
			if(pButton->GetCheck())
				break;
			nDirectFlag = DUAL_DIRECT;
			pButton = (CButton *)GetDlgItem(IDC_MRP_ALLDIR);
			if(pButton->GetCheck())
				break;
		}while(0);

		// 显示
		ShowStationMRPREC(nDirectFlag);

		m_nCurrentSel = 0;
		ShowRecord(0);

	}while(0);
}


BOOL CManualReportHistory::PreTranslateMessage(MSG* lpMsg) 
{
	CWnd* pWndFocus = GetFocus();

	if ((lpMsg->message == WM_KEYDOWN) && ((lpMsg->wParam == VK_RETURN) || (lpMsg->wParam == VK_ESCAPE)))
	{
		TCHAR szClass[10];
		if ((lpMsg->wParam == VK_RETURN) &&
			((pWndFocus = GetFocus()) != NULL) &&
			IsChild(pWndFocus)  &&
			GetClassName(pWndFocus->m_hWnd, szClass, 10) &&
			(_tcsicmp(szClass, _T("EDIT")) == 0)
			)
		{
			CWnd *pWnd;
			if(pWndFocus == GetDlgItem(IDC_MRP_ACTUAL_ARRIVE))
			{
				pWnd = GetDlgItem(IDC_MRP_ACTUAL_DEPART); 
				GotoDlgCtrl(pWnd); 
			} 
			else if(pWndFocus == GetDlgItem(IDC_MRP_ACTUAL_DEPART))
			{
				CString strtip;
				CString text;
				int nValue,theStatus = 0;
				char lpszInputValue[32];

				do 
				{
					int limitMin=gpcdef.GetManualConvertActualTimeLength()*60;
					if(gpcdef.IsInhibitManualConvertActualTime())
					{
						if(limitMin<=0) // 限制修改实际点，没有允许时分，所以完全禁止返回
						{
							MessageBox("限制修改实际点，不允许转实际!", "提示", MB_OK|MB_ICONHAND);
							break;
						}
					}
					else
					{
						limitMin=0;
					}

					if(m_nCurrentSel >= m_list.GetItemCount())
						break;

					int index = m_list.GetItemData(m_nCurrentSel);
					if(index>=mrprec.size())
						break;

					//从界面获取数据
					pWnd = GetDlgItem(IDC_MRP_ACTUAL_ARRIVE);
					pWnd->GetWindowText(text);

					if(text.GetLength( ) == 0 || IsAllSpace((char *)(LPCTSTR)text) )
						break;//用户没有输入列车到达时间

					if(text.Find('.') != -1 || text.Find(':') != -1 || text.Find(';') != -1 ||text.Find(',') != -1)
					{//用户输入小时和分钟
						strcpy(lpszInputValue,TrimSpace((char *)(LPCTSTR)text) );
						nValue = OsConvertStringtoTime(lpszInputValue);
						if(nValue == -1)
						{
							AfxMessageBox("实际到达 时间输入错误!");
							break;
						}
						
						long temp;
						temp = mrprec[index].nActualArrivalTime - nValue;
						if( abs(temp) > 12 * 3600)
						{
							if(temp > 0)
							{
								mrprec[index].nActualArrivalDate += 1;
							}
							else
							{
								mrprec[index].nActualArrivalDate -= 1;
							}
						}

						nValue=nValue-nValue%60;
						if(limitMin>0 && !GetTGFunctionConfig()->IsNoLimitStation(m_nCurrentSectionNo,m_nCurrentStationNo))
						{
							// 禁止修改实际点
							long arrive=long_time(mrprec[index].nActualArrivalDate, nValue);
							
							CTime tm=CTime::GetCurrentTime();
							long now=tm.GetTime();
							now = now-now%60;

							if(!TG_IsActualArrivalTime(mrprec[index].adjust_status))
							{
								if(abs(now-arrive)>limitMin)
								{
									strtip.Format("输入实际到达时间和当前时间相差不能超过 %d 分钟!", limitMin/60);
									MessageBox(strtip, "警告", MB_OK|MB_ICONHAND);
									break;
								}
								mrprec[index].nActualArrivalTime = nValue;
							}
							else
							{
								MessageBox("到达点是实际点，不能修改!", "警告", MB_OK|MB_ICONHAND);
							}
						}
						else
						{
							mrprec[index].nActualArrivalTime = nValue;
						}
					} 
					else 
					{
						AfxMessageBox("输入到点时间格式不对，正确格式 HH:MM");
						break;
					}

					pWnd = GetDlgItem(IDC_MRP_ACTUAL_DEPART);
					pWnd->GetWindowText(text);
					if(text.GetLength( ) == 0 || IsAllSpace((char *)(LPCTSTR)text) )
					{//用户没有输入列车出发时间
						mrprec[index].nActualDepartTime = mrprec[index].nActualArrivalTime;
					} 
					else 
					{
						if(text.Find('.') != -1 || text.Find(':') != -1 || text.Find(';') != -1 ||text.Find(',') != -1 )
						{//用户输入小时和分钟
							strcpy(lpszInputValue,TrimSpace((char *)(LPCTSTR)text) );
							nValue = OsConvertStringtoTime(lpszInputValue);
							if(nValue == -1)
							{
								AfxMessageBox("实际出发 时间输入错误!");
								break;
							}
							
							long temp;
							temp = mrprec[index].nActualDepartTime - nValue;
							if( abs(temp) > 12 * 3600)
							{
								if(temp > 0)
								{
									mrprec[index].nActualDepartDate += 1;
								}
								else
								{
									mrprec[index].nActualDepartDate -= 1;
								}
							}

							nValue=nValue-nValue%60;
							if(limitMin>0 && !GetTGFunctionConfig()->IsNoLimitStation(m_nCurrentSectionNo,m_nCurrentStationNo))
							{
								long depart=long_time(mrprec[index].nActualDepartDate, nValue);
									
								// 禁止修改实际点
								CTime tm=CTime::GetCurrentTime();
								long now=tm.GetTime();
								now = now-now%60;
								
								if(!TG_IsActualDepartTime(mrprec[index].adjust_status))
								{
									if(abs(now-depart)>limitMin)
									{
										strtip.Format("输入实际出发时间和当前时间相差不能超过 %d 分钟!", limitMin/60);
										MessageBox(strtip, "警告", MB_OK|MB_ICONHAND);
										break;
									}
									mrprec[index].nActualDepartTime = nValue;
								}
								else
								{
									MessageBox("出发点是实际点，不能修改!", "警告", MB_OK|MB_ICONHAND);
								}
							}
							else
							{
								mrprec[index].nActualDepartTime = nValue;
							}
						} 
						else 
						{
							AfxMessageBox("输入发点时间格式不对，正确格式 HH:MM");
							break;
						}
					}

					if(mrprec[index].nActualDepartTime<mrprec[index].nActualArrivalTime)
					{
						MessageBox("输入到达时间大于出发时间!", "警告", MB_OK|MB_ICONHAND);
						break;
					}

					//更新ListCtrl中的界面显示
					text.Format("%s",OsConvertTimetoStringIgnoreSecond(mrprec[index].nActualArrivalTime));//add by cf
					m_list.SetItemText(m_nCurrentSel,3,text);

					text.Format("%s",OsConvertTimetoStringIgnoreSecond(mrprec[index].nActualDepartTime));//add by cf
					m_list.SetItemText(m_nCurrentSel,4,text);

					SendMessageToCad(&mrprec[index]);

					int old=m_nCurrentSel;
					ShowStationMRPREC(m_nCurrentSelDirect);
					m_nCurrentSel=old;

					//显示下一条记录
					if(m_nCurrentSel == m_list.GetItemCount( ) - 1)
						m_nCurrentSel = 0;
					else
						m_nCurrentSel += 1;
					m_list.SetCurSel(m_nCurrentSel);
					m_list.EnsureVisible(m_nCurrentSel,false);
					ShowRecord(m_nCurrentSel);
					
					pWnd = GetDlgItem(IDC_MRP_ACTUAL_ARRIVE); 
					GotoDlgCtrl(pWnd); 
				}while(0);
			}
		}
		return TRUE;
	}
	else if(lpMsg->message == WM_LBUTTONDOWN)  
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

			// we need additional checking in owner-draw mode
			// because we only get LVHT_ONITEM
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
			ShowRecord(m_nCurrentSel);
		}while(0);
	}
	return CDialog::PreTranslateMessage(lpMsg);
}

BOOL CManualReportHistory::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_nCurrentSectionNo==0)
		return FALSE;
	// TODO: Add extra initialization here
	m_ctrlActualArriveTime.SubclassDlgItem(IDC_MRP_ACTUAL_ARRIVE,this);
	m_ctrlActualDepartTime.SubclassDlgItem(IDC_MRP_ACTUAL_DEPART,this);
	m_list.SubclassDlgItem(IDC_MRP_TRAINLIST,this);

	m_list.SetHeaderLabel(REPORTHISTORYCOLUMNS,_gszReportHistoryLabel,
		_gnReportHistoryWidth,_gnReportHistoryFmt);

	CComboBox *pComboBox;
	//  下拉列表框中加入车站信息
	pComboBox = (CComboBox *)GetDlgItem(IDC_MRP_STATIONLIST);

	int nCurSel = 0;
	int stationlist[128];
	int stationnum=128;
	gpcdef.GetStationList(m_nCurrentSectionNo,stationnum,stationlist);
	for(int i=0;i < stationnum;i++)//add by cf after 8-24
	{
		pComboBox->AddString(gpcdef.GetStationName(stationlist[i]));
		pComboBox->SetItemData(i,(DWORD)stationlist[i]);
		if(m_nCurrentStationNo == stationlist[i])
			nCurSel = i;
	}

	pComboBox->SetCurSel(nCurSel);
	m_nCurrentStationNo = pComboBox->GetItemData(nCurSel);
	
	CButton *pButton = (CButton *)GetDlgItem(IDC_MRP_ALLDIR);
	pButton->SetCheck(1);
	m_nCurrentSelDirect=DUAL_DIRECT;

	OnSelchangeMrpStationlist();

    
	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CManualReportHistory::SendMessageToCad(tagManualReportRec *rec)
{
	CTG_ScheduleInfo schd;
	schd.m_nSectionID=m_nCurrentSectionNo;
	schd.m_nType=(CTG_ScheduleInfo::SCHEDULE_TYPE)m_nScheduleType;
	CTG_TrainScheduleBase *pdata=m_pMMI->GetDataStore(schd); 
	if(!pdata)
		return;
	CTG_SingleTrain *ptrain=pdata->GetSingleTrain(rec->uTrainIndex);
	if(!ptrain)
		return;
	CTG_SingleTrain newtrain=*ptrain;
	CTG_TrainRecord *precord=newtrain.GetTrainRecordByRecIndex(rec->rec_index);
	if(!precord)
		return;
	
	bool bUpdate=false;
	if(gpcdef.IsInhibitManualConvertActualTime()&& !GetTGFunctionConfig()->IsNoLimitStation(m_nCurrentSectionNo,m_nCurrentStationNo))
	{
		if(TG_IsActualArrivalTime(precord->m_data.adjust_status) && TG_IsActualDepartTime(precord->m_data.adjust_status))
			return;

		if(!TG_IsActualArrivalTime(rec->adjust_status))
		{
			TG_SetActualArrivalTime(rec->adjust_status);
			TG_SetActualArrivalTime(precord->m_data.adjust_status);
			precord->m_data.arrive=long_time(rec->nActualArrivalDate,rec->nActualArrivalTime);

			TG_SetRecTimeFromDispatcherTurnActual(precord->m_data, TRAIN_ARRIVE);
			bUpdate=true;

			GpcLogTrainMessage(0, 0, "[调度员操作] 通过 人工报点窗口 进行操作, 设置列车 %s(%d) 在 %d 站的实际到达时间为 %d",
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),precord->GetStationNo(), precord->m_data.arrive);
		}
		if(!TG_IsActualDepartTime(rec->adjust_status))
		{
			TG_SetActualDepartTime(rec->adjust_status);
			TG_SetActualDepartTime(precord->m_data.adjust_status);
			precord->m_data.depart=long_time(rec->nActualDepartDate,rec->nActualDepartTime); 

			TG_SetRecTimeFromDispatcherTurnActual(precord->m_data, TRAIN_DEPART);
			bUpdate=true;

			GpcLogTrainMessage(0, 0, "[调度员操作] 通过 人工报点窗口 进行操作, 设置列车 %s(%d) 在 %d 站的实际出发时间为 %d",
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),precord->GetStationNo(), precord->m_data.depart);
		}
	}
	else
	{
		TG_SetActualArrivalTime(rec->adjust_status);
		TG_SetActualArrivalTime(precord->m_data.adjust_status);
		precord->m_data.arrive=long_time(rec->nActualArrivalDate,rec->nActualArrivalTime);
		TG_SetRecTimeFromDispatcherTurnActual(precord->m_data, TRAIN_ARRIVE);

		TG_SetActualDepartTime(rec->adjust_status);
		TG_SetActualDepartTime(precord->m_data.adjust_status);
		precord->m_data.depart=long_time(rec->nActualDepartDate,rec->nActualDepartTime);
		TG_SetRecTimeFromDispatcherTurnActual(precord->m_data, TRAIN_DEPART);

		bUpdate=true;

		GpcLogTrainMessage(0, 0, "[调度员操作] 通过 人工报点窗口 进行操作, 设置列车 %s(%d) 在 %d 站的实际到达时间为 %d,实际出发时间为 %d",
			ptrain->GetTrainID(),ptrain->GetTrainIndex(),precord->GetStationNo(), precord->m_data.arrive,precord->m_data.depart);
	}

	if(bUpdate==true)
	{
		CMMIOperFlag oper_flag;
		m_pMMI->ProcessMMIOperation(IScheduleComm::TG_OPER_UPDATE,newtrain,oper_flag); 
	}
}

// added by cf
char * CManualReportHistory::OsConvertTimetoStringIgnoreSecond(long uValue)
{
	static char buff1[12],buff2[12];
	short uHour,uMinute;
	char *string;
	static int nFlag = 0;

	if(nFlag == 0)
	{
		string = buff1;
		nFlag = 1;
	}else {
		string = buff2;
		nFlag = 0;
	}
	memset(string,0x20,12);
	if(uValue != -1)
	{
		uHour = (uValue / 60) / 60;
		if(uHour < 10)
		{
			string[0] = 0x30;
			string[1] = 0x30 + uHour;
		}else {
			string[0] = 0x30 + uHour/10;
			string[1] = 0x30 + uHour%10;
		}
		string[2] = ':';

		uMinute = (uValue / 60) % 60;
		if(uMinute < 10)
		{
			string[3] = 0x30;
			string[4] = 0x30 + uMinute;
		}else {
			string[3] = 0x30 + uMinute/10;
			string[4] = 0x30 + uMinute%10;
		}
		string[5] = 0x00;
	} else 
		strcpy(string,"--:--");
	return string;
}
//above add by cf

void CManualReportHistory::OnMrpUpdir() 
{
	m_nCurrentSelDirect=UP_DIRECT;
	ShowStationMRPREC(m_nCurrentSelDirect);
}

void CManualReportHistory::OnMrpDowndir() 
{
	m_nCurrentSelDirect=DOWN_DIRECT;
	ShowStationMRPREC(m_nCurrentSelDirect);
}

void CManualReportHistory::OnMrpAlldir() 
{
	m_nCurrentSelDirect=DUAL_DIRECT;
	ShowStationMRPREC(m_nCurrentSelDirect);
}

void CManualReportHistory::ShowStationMRPREC(int flag) 
{
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT ;
	lvi.iSubItem = 0;
	CString text;
	int nRow=0;

	m_list.DeleteAllItems( );
	
	for(int j=0;j < mrprec.size();j++)
	{
		if(flag!=DUAL_DIRECT)
		{
			if(mrprec[j].nDirectFlag != flag)
			{
				continue;
			}
		}

		lvi.iItem = nRow;
		lvi.pszText = (char *)(LPCTSTR)mrprec[j].lpszTrainID;
		m_list.InsertItem(&lvi);

		text.Format("%s",OsConvertTimetoStringIgnoreSecond(mrprec[j].nActualArrivalTime) );
		m_list.SetItemText(nRow,1,text);

		text.Format("%s",OsConvertTimetoStringIgnoreSecond(mrprec[j].nActualDepartTime) );
		m_list.SetItemText(nRow,2,text);

		if(!TG_IsActualArrivalTime(mrprec[j].adjust_status))
			text.Format("--:--");
		else
			text.Format("%s",OsConvertTimetoStringIgnoreSecond(mrprec[j].nActualArrivalTime) );
		m_list.SetItemText(nRow,3,text);

		if(!TG_IsActualDepartTime(mrprec[j].adjust_status))
			text.Format("--:--");
		else
			text.Format("%s",OsConvertTimetoStringIgnoreSecond(mrprec[j].nActualDepartTime) );
		m_list.SetItemText(nRow,4,text);
	
		m_list.SetItemData(nRow,j);
		nRow += 1;
	}

	m_nCurrentSel = 0;
	ShowRecord(0);
}

void CManualReportHistory::ShowRecord(int row)
{
	if(row >= m_list.GetItemCount())
		return;
	
	int index = m_list.GetItemData(row);
	if(index >= mrprec.size() || index<0)
		return;

	CString text;

	CWnd *pWnd = GetDlgItem(IDC_MRP_PLAN_ARRIVE);
	text.Format("%s",OsConvertTimetoStringIgnoreSecond(mrprec[index].nActualArrivalTime));
	pWnd->SetWindowText(text);

	pWnd = GetDlgItem(IDC_MRP_PLAN_DEPART);
	text.Format("%s",OsConvertTimetoStringIgnoreSecond(mrprec[index].nActualDepartTime));
	pWnd->SetWindowText(text);

	pWnd = GetDlgItem(IDC_MRP_ACTUAL_ARRIVE);
	if(!TG_IsActualArrivalTime(mrprec[index].adjust_status))
		text.Format("");
	else
		text.Format("%s",OsConvertTimetoStringIgnoreSecond(mrprec[index].nActualArrivalTime));
	pWnd->SetWindowText(text);

	pWnd = GetDlgItem(IDC_MRP_ACTUAL_DEPART);
	if(!TG_IsActualDepartTime(mrprec[index].adjust_status))
		text.Format("");
	else
		text.Format("%s",OsConvertTimetoStringIgnoreSecond(mrprec[index].nActualDepartTime));
	pWnd->SetWindowText(text);

	return;
}

void CManualReportHistory::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	OnSelchangeMrpStationlist();
}
