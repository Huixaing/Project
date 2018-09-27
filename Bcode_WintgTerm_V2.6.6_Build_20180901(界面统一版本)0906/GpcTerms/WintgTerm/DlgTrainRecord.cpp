// DlgTrainRecord.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgTrainRecord.h"
#include "tgpspdata.h"
#include "tg_basicpara.h"
#include "tg_schedule_base.h"
#include "sysshare.h"
#include "function.h"
#include "GridCtrl_src/GridCellCheck.h"
#include "gridctrl_src/GridCellCombo.h"
#include "tgdocument.h"
#include <vector>
#include ".\dlgtrainrecord.h"
#define   UPDATE_SIDE WM_USER+1
#define   IDC_TRAIN_GRID_ID  54321

BOOL  CTrainListGridView::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid, bool bShiftOrPreBasic)
{
	if(bShiftOrPreBasic)
		m_bCheckShiftTrainAddTime=GetTGFunctionConfig()->IsShiftCompareWithBasicAddTime();
	else
		m_bCheckShiftTrainAddTime=false;
	m_pTrainDataBase=NULL;
	m_record_col_count=0;
	strcpy(m_train_record_def[m_record_col_count].caption,"序号");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"no");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"车次号");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"trainid");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"车站名称");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"station");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"到达时刻");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"arrive_tm");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"出发时刻");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"depart_tm");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	strcpy(m_train_record_def[m_record_col_count].caption,"股道");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"side_name");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=30;
	m_record_col_count++;
	
	strcpy(m_train_record_def[m_record_col_count].caption,"股道报警");
	sprintf(m_train_record_def[m_record_col_count].attr_name,"side_valid");
	m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
	m_train_record_def[m_record_col_count].flag=0;
	m_record_col_count++;

	if(m_bCheckShiftTrainAddTime) // 增加一列时刻比较
	{
		strcpy(m_train_record_def[m_record_col_count].caption,"时刻报警");
		sprintf(m_train_record_def[m_record_col_count].attr_name,"time_valid");
		m_train_record_def[m_record_col_count].data_type=CTC_DATA_STR;
		m_train_record_def[m_record_col_count].flag=0;
		m_record_col_count++;
	}

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_train_record_def,m_record_col_count);

	SetColumnWidth(0, 50);
	SetColumnWidth(1, 80);
	SetColumnWidth(2, 100);
	SetColumnWidth(3, 60);
	SetColumnWidth(4, 60);
	SetColumnWidth(5, 60);

	if(m_bCheckShiftTrainAddTime)
	{
		SetColumnWidth(6, 250);
		SetColumnWidth(7, 250);
	}
	else
	{
		SetColumnWidth(6, 350);
	}
	return TRUE;
}

void  CTrainListGridView::SetScheduleBase(CTG_TrainScheduleBase *pTrainDataBase,CTG_TrainScheduleBase *pBasicTrainDataBase)
{
	m_pTrainDataBase=pTrainDataBase;
	m_pBasicTrainDataBase=pBasicTrainDataBase;
}

void  CTrainListGridView::BeginEditCell(int row,int col)
{
	int station=GetCell(row,1)->GetData();
	if(m_FiledInfo[col].flag==30) // 股道
	{
		CStringArray options;
		int value;
		for(int i=0;i<MAX_SIDE_COUNT;i++)
		{
			const char *ptmpstr=GetTGBasicParam()->GetSideNameByIndex(station,i,&value);
			if(ptmpstr)
				options.Add(ptmpstr);
			else
				break;
		}
		SetCellType(row ,col, RUNTIME_CLASS(CGridCellCombo));
		CGridCellCombo *pCell=(CGridCellCombo *)GetCell(row,col);
		pCell->SetOptions(options);
		pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST);
	}
	else
		CXMLDataGrid::BeginEditCell(row,col);
}

void  CTrainListGridView::EndEditCell(int row,int col)
{
	SetCellType(row ,col,RUNTIME_CLASS(CGridCell));
	if(m_train_record_def[col].flag==30) // 股道
	{
		int train_index=GetCell(row,0)->GetData();
		int station=GetCell(row,1)->GetData();
		int rec_index=GetCell(row,2)->GetData();

		CGridCellCombo *pCell=(CGridCellCombo *)GetCell(row,col);
		int sideid=GetTGBasicParam()->GetSideIDByName(station,pCell->GetText());
		if(IS_VALID_SIDE(sideid))
		{
			CTG_SingleTrain* ptrain=m_pTrainDataBase->GetSingleTrain(train_index);
			if(ptrain)
			{
				CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(rec_index);
				if(prec)
				{
					if(prec->m_data.station==station)
					{
						GetCell(row,col)->SetData(sideid);
						prec->m_data.arrive_side=sideid;
						prec->m_data.depart_side=sideid;
						Refresh();
						ptrain->GetRecordList()->SetSyncStatusX(SYCFLAG_NULL,TRUE);
						return;
					}
				}
			}
			AfxMessageBox("股道修改失败!");
		}
	}
	else
		CXMLDataGrid::EndEditCell(row,col);
}

void CTrainListGridView::ShowNotAckStationSide()
{
	for(int i=0;i<mlistRD.size();i++)
	{
		GetCell(i+1,0)->SetData(mlistRD[i].train_index);
		GetCell(i+1,1)->SetData(mlistRD[i].station);
		GetCell(i+1,2)->SetData(mlistRD[i].rec_index);
		int arrive_side=mlistRD[i].arrive_side;
		int depart_side=mlistRD[i].depart_side;
		CGridCell *pCellSide=(CGridCell *)GetCell(i+1,5);
		
		if(mlistRD[i].status==1)
		{
			for(int j=0; j<stationNum; j++)
			{
				if(stationNo[j] == mlistRD[i].station)
				{
					break;
				}
			}
			if(j>=stationNum && j<128)
			{
				stationNo[j]=mlistRD[i].station;
				stationNum++;
			}
		}

		// 显示车站未确认股道
		if(mlistRD[i].status==1)
			pCellSide->SetBackClr(RGB(239,228,176));
			
		if(arrive_side==255 || depart_side==255)
			pCellSide->SetBackClr(RGB(255,100,255));
						
	}

	return;
}

CString CTrainListGridView::GetStationTrainSide(CTG_TrainRecord *prec, int rec_index, long train_index, DWORD side_valide, CStringArray& list)
{
	if(prec==NULL)
		return "";

	CString stName=gpcdef.GetStationName(prec->m_data.station);

	CString tmp;
	CString trainstr="<RECORD>";

	tmp.Format("<no>%d</no>", mlistRD.size()+1);
	trainstr +=tmp;
	if(prec->IsTerminalRec())
		tmp.Format("<trainid>%s</trainid>", GetValidStr(prec->GetArriveTID()));
	else
		tmp.Format("<trainid>%s</trainid>", GetValidStr(prec->GetDepartTID()));
	trainstr +=tmp;
	tmp.Format("<station>%s</station>", GetValidStr(stName));
	trainstr +=tmp;

	CTime tm1(2000,1,1,0,0,0);
	if(prec->IsStartRec())
	{
		tmp.Format("<arrive_tm>始发</arrive_tm>");
		trainstr +=tmp;
	}
	else
	{		
		CTime tm(prec->GetArriveTime());
		if (m_pBasicTrainDataBase==NULL)
			tm += tm1.GetTime();
		tmp.Format("<arrive_tm>%s</arrive_tm>", tm.Format("%H:%M"));
		trainstr +=tmp;
	}
	
	if(prec->IsTerminalRec())
	{
		tmp.Format("<depart_tm>终到</depart_tm>");
		trainstr +=tmp;
	}
	else
	{
		CTime tm(prec->GetDepartTime());
		if (m_pBasicTrainDataBase==NULL)
			tm += tm1.GetTime();
		tmp.Format("<depart_tm>%s</depart_tm>", tm.Format("%H:%M"));
		trainstr +=tmp;
	}
	
	CString arrive_side="", depart_side="";
	const char* psidename=GetTGBasicParam()->GetSideNameByID(prec->m_data.station,prec->m_data.arrive_side);
	if(psidename)
		arrive_side=GetValidStr(psidename);
	
	psidename=GetTGBasicParam()->GetSideNameByID(prec->m_data.station,prec->m_data.depart_side);
	if(psidename)
		depart_side=GetValidStr(psidename);
	if(arrive_side==depart_side)
		tmp.Format("<side_name>%s</side_name>", arrive_side);
	else
		tmp.Format("<side_name>%s,%s</side_name>", arrive_side, depart_side);
	trainstr +=tmp;
	
	CString strBasic;
	if(m_bCheckShiftTrainAddTime)
	{
		if(list.GetCount()>0)
		{
			for(int n=0; n<list.GetCount(); n++)
			{
				CString str=list.GetAt(n);
				if(str.Find("股道")>=0)
				{
					strBasic+=str;
					strBasic+=";";
				}
			}
		}
	}
	else
	{
		if(list.GetCount()>0)
		{
			for(int n=0; n<list.GetCount(); n++)
			{
				strBasic+=list.GetAt(n);
				strBasic+=";";
			}
		}
	}
	//side_valid
	if(side_valide>0)
	{	
		CString ss;
		if(IsPassNotMainSide(side_valide))
			ss+=",侧线通过";
		if(IsSideDisconnectEntry(side_valide))
			ss+=",股道和出入口不通";
		if(IsSideNotSame(side_valide))
			ss+=",接发股道不同";
		if(IsNotHighPlatform(side_valide))
			ss+=",非高站台";
		if(IsNoPlatform(side_valide))
			ss+=",无站台";
		if(IsNotStopThisClassTrain(side_valide))
			ss+=",不能接发此种车";
		if(IsNotStopCRHTrain(side_valide))
			ss+=",不能接动车组";
		if(IsNotGiveSide(side_valide))
			ss+=",无股道";
		if(IsNotSpaceTrain(side_valide))
			ss+=",不能接超限车";
		if(IsNotCheckSide(side_valide))
			ss+=",站细获取失败,无法检查股道";

		tmp.Format("<side_valid>%s;%s</side_valid>", ss, strBasic);
		trainstr +=tmp;
	}
	else if(strBasic!="")
	{
		tmp.Format("<side_valid>%s</side_valid>", strBasic);
		trainstr +=tmp;
	}
	
	if(m_bCheckShiftTrainAddTime)
	{
		strBasic="";
		if(list.GetCount()>0)
		{
			for(int n=0; n<list.GetCount(); n++)
			{
				CString str=list.GetAt(n);
				if(str.Find("时刻")>=0)
				{
					strBasic+=str;
					strBasic+=";";
				}
			}
		}
		if(strBasic!="")
		{
			tmp.Format("<time_valid>%s</time_valid>", strBasic);
			trainstr +=tmp;
		}
	}

	trainstr+="</RECORD>";

	NodeRD rd;
	rd.train_index=train_index;
	rd.station=prec->GetStationNo();
	rd.rec_index=rec_index;
	rd.arrive_side=prec->m_data.arrive_side;
	rd.depart_side=prec->m_data.depart_side;
	rd.side_valide=side_valide;
	
	if(!prec->IsSideConfirmed())
		rd.status=1;
	else
		rd.status=0;

	mlistRD.push_back(rd);

	return trainstr;
}

void CTrainListGridView::CheckSideValid(CTG_SingleTrain *ptrain, int n, CTG_TrainRecord *prec, DWORD& side_valide)
{
	side_valide=0;
	if(IS_VALID_SIDE(prec->m_data.arrive_side) && IS_VALID_SIDE(prec->m_data.depart_side))
	{
		const CStationDef* pSideDef = GetTGBasicParam()->GetStationDef(prec->m_data.station);
		if(pSideDef==NULL)
		{
			SetNotCheckSide(side_valide);
			return;
		}
		if (GetTGBasicParam()->IsTrainPassNotUseMainSide(prec->m_data))
		{
			SetPassNotMainSide(side_valide);
		}
		
		BYTE nTrainType = ptrain->IsPassengerTrain() ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
		bool bCheck=true;
		if(n==0 && prec->IsStartRec())
		{
			if(!pSideDef->isReceiveTrain(prec->m_data.depart_side, nTrainType) )
			{
				SetNotStopThisClassTrain(side_valide);
			}
			if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.depart_side, prec->m_data.exit))
			{
				SetSideDisconnectEntry(side_valide);
			}
			if(!pSideDef->isReceiveExceedTrain(prec->m_data.depart_side, ptrain->GetTrainInfo()->m_data.space_rank))
			{
				SetNotSpaceTrain(side_valide);
			}
			bCheck=false;
		}
		if(n==ptrain->GetTrainRecordCount()-1 && prec->IsTerminalRec())
		{
			if(!pSideDef->isReceiveTrain(prec->m_data.arrive_side, nTrainType) )
			{
				SetNotStopThisClassTrain(side_valide);
			}
			if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.arrive_side, prec->m_data.entry))
			{
				SetSideDisconnectEntry(side_valide);
			}
			if(!pSideDef->isReceiveExceedTrain(prec->m_data.arrive_side, ptrain->GetTrainInfo()->m_data.space_rank))
			{
				SetNotSpaceTrain(side_valide);
			}
			bCheck=false;
		}

		if(bCheck)
		{
			if(prec->m_data.depart_side==prec->m_data.arrive_side)
			{
				if(!pSideDef->isReceiveTrain(prec->m_data.arrive_side, nTrainType))
				{
					SetNotStopThisClassTrain(side_valide);
				}
				if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.arrive_side, prec->m_data.entry))
				{
					SetSideDisconnectEntry(side_valide);
				}
				if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.depart_side, prec->m_data.exit))
				{
					SetSideDisconnectEntry(side_valide);
				}
				if(!pSideDef->isReceiveExceedTrain(prec->m_data.arrive_side, ptrain->GetTrainInfo()->m_data.space_rank))
				{
					SetNotSpaceTrain(side_valide);
				}
			}
			else
			{
				SetSideNotSame(side_valide);

				if(!pSideDef->isReceiveTrain(prec->m_data.arrive_side, nTrainType))
				{
					SetNotStopThisClassTrain(side_valide);
				}
				if(!pSideDef->isReceiveTrain(prec->m_data.depart_side, nTrainType) )
				{
					SetNotStopThisClassTrain(side_valide);
				}
				if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.arrive_side, prec->m_data.entry))
				{
					SetSideDisconnectEntry(side_valide);
				}
				if (GetTGBasicParam()->IsTrainSideNotConnectWithEntry(prec->m_data.station, prec->m_data.depart_side, prec->m_data.exit))
				{
					SetSideDisconnectEntry(side_valide);
				}

				if(!pSideDef->isReceiveExceedTrain(prec->m_data.arrive_side, ptrain->GetTrainInfo()->m_data.space_rank))
				{
					SetNotSpaceTrain(side_valide);
				}
				if(!pSideDef->isReceiveExceedTrain(prec->m_data.depart_side, ptrain->GetTrainInfo()->m_data.space_rank))
				{
					SetNotSpaceTrain(side_valide);
				}
			}
		}

		

		if(ptrain->IsPassengerTrain())
		{
			if ( prec->IsStartRec() || prec->IsTerminalRec() || !prec->IsPassRec())
			{				
				// 无站台,不能办理客运作业
				if (!pSideDef->isPlatform(prec->m_data.arrive_side) )
					SetNoPlatform(side_valide);
				else
				{
					// 动车组接入非动车股道，非高站台股道
					if (ptrain->IsCRHTrain())
					{
						if(!GetTGBasicParam()->VerifyNotCRHStopSide(prec->m_data.arrive_side, prec->m_data.station, ptrain->GetTrainInfo()->m_data))
							SetNotStopCRHTrain(side_valide);
						else if(!pSideDef->isHighPlatform(prec->m_data.arrive_side))
							SetNotHighPlatform(side_valide);
					}
				}
			}
		}
	}
	else
	{
		SetNotGiveSide(side_valide);
	}

	return;
}

int  CTrainListGridView::ShowTrainData(CTG_TrainPtrGroup *pGroup, int station, int flag)
{
	stationNum=0;
	memset(stationNo, 0, sizeof(stationNo));
	memset(exitNullSide, 0, sizeof(exitNullSide));
	
	mlistRD.clear();

	CString xml="<TRAINLIST>";
	for(int i=0;i<pGroup->GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=pGroup->GetTrainPtr(i);
		if(!ptrain)
			continue;

		CTG_TrainRecord * pfirst=ptrain->GetTrainFirstRecord();
		if(pfirst==NULL)
			continue;
	
		CTG_SingleTrain *pbasetrain=NULL;
		if(m_pBasicTrainDataBase!=NULL)
			pbasetrain=m_pBasicTrainDataBase->FindTrainsByStationAndTrainNo(pfirst->GetStationNo(), ptrain->GetTrainID());

		for(int n=0; n<ptrain->GetTrainRecordCount(); n++)
		{
			CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(n);
			if(prec==NULL) continue;

			if(!((CDlgTrainRecord *)GetParent())->IsNeedCheckStation(prec->m_data.station))
			{
				continue;
			}
			// 找到基本图车
			CString trainid;
			if(n==0 && prec->IsStartRec())
				trainid=prec->GetDepartTID();
			else
				trainid=prec->GetArriveTID();
			
			DWORD side_valide=0;
			
			// 检查车站值班员是否确认股道
			if(CHECH_ST_MAN_CONFIRM_SIDE == flag)
			{
				if(prec->IsSideConfirmed())
					continue;
			}
			
			if(CHECH_SIDE_VALID == flag)
			{
				if(station>0)
				{
					if(prec->GetStationNo()!=station)
						continue;
				}

				CheckSideValid(ptrain, n, prec, side_valide);
			}

			CStringArray list;
			if(pbasetrain)
			{
				for (int index = 0; index < pbasetrain->GetTrainRecordCount(); index++)
				{
					CTG_TrainRecord *pbasicrec = pbasetrain->GetTrainRecordByRecIndex(index);
					if(NULL == pbasicrec)
						continue;

					if(prec->GetStationNo()==pbasicrec->GetStationNo())
					{
						if(prec->m_data.arrive_side!=pbasicrec->m_data.arrive_side 
							|| prec->m_data.depart_side!=pbasicrec->m_data.depart_side)
						{
							CString strside1="?G", strside2="?G";
							const char* p=GetTGBasicParam()->GetSideNameByID(pbasicrec->m_data.station, pbasicrec->m_data.arrive_side);
							if(NULL!=p)
								strside1=p;
							p=GetTGBasicParam()->GetSideNameByID(pbasicrec->m_data.station, pbasicrec->m_data.depart_side);
							if(NULL!=p)
								strside2=p;

							CString str;
							if(strside1==strside2)
								str.Format("股道不同,基本图:%s", strside2);
							else
								str.Format("股道不同,基本图:%s接%s发", strside1, strside2);

							list.Add(str);
						}

						if(m_bCheckShiftTrainAddTime)
						{
							long date;
							long ar_tm=minute_time(prec->m_data.arrive, &date);
							long b_ar_tm=minute_time(pbasicrec->m_data.arrive, &date);
							
							long dp_tm=minute_time(prec->m_data.depart, &date);
							long b_dp_tm=minute_time(pbasicrec->m_data.depart, &date);

							if(ar_tm!=b_ar_tm || dp_tm!=b_dp_tm)
							{ 
								CString str,str1=BasicStringTime(b_ar_tm);
								if(b_ar_tm==b_dp_tm)
									str.Format("时刻不同,基本图:%s 通过", str1);
								else
								{
									
									CString str2=BasicStringTime(b_dp_tm);
									str.Format("时刻不同,基本图:%s到%s发", str1, str2);
								}
								list.Add(str);
							}
						}
					}
				}
			}

			if(CHECH_SIDE_VALID == flag && side_valide==0 && list.GetCount()==0)
			{
				continue;
			}

			if(SHOW_ST_SIDE == flag)
			{
				if(station>0)
				{
					if(prec->GetStationNo()!=station)
						continue;
				}

				CheckSideValid(ptrain, n, prec, side_valide);
			}
			
			xml+=GetStationTrainSide(prec, n, ptrain->GetTrainIndex(), side_valide, list);
		}
	}
	if(xml=="<TRAINLIST>")
	{
		AfxMessageBox("没有可显示列车股道信息");
	}
	xml+="</TRAINLIST>";	
	InsertRows(xml,"TRAINLIST/RECORD");
	
	ShowNotAckStationSide();

	Refresh();

	return mlistRD.size();
}

// CDlgTrainRecord 对话框
CDlgTrainRecord::CDlgTrainRecord(CTG_ScheduleInfo::SCHEDULE_TYPE schd_type, CWnd* pParent) : CDialog(CDlgTrainRecord::IDD,pParent)
{
	m_schd_type=schd_type;
	m_nFlag=1;
	m_pBasicDataBase=NULL;
	station_count=0;
	memset(station_list, 0, sizeof(station_list));
}

CDlgTrainRecord::~CDlgTrainRecord()
{
}

void CDlgTrainRecord::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_STTM2, mStationCombo);
}

BEGIN_MESSAGE_MAP(CDlgTrainRecord, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_RELOAD, OnBnClickedReload)
	ON_BN_CLICKED(ID_CHECK_STATION_ACK_SIDE3, OnBnClickedCheckTrainSide)
	ON_CBN_SELCHANGE(IDC_COMBO_STTM2, OnCbnSelchangeComboStation)
END_MESSAGE_MAP()

void CDlgTrainRecord::SetDocument(CTgDocument *pdoc, BOOL flag)
{
	m_pDoc=pdoc;
	m_pTrainDataBase=m_pDoc->GetScheduleData(m_schd_type);
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==m_schd_type)
		m_pBasicDataBase=m_pDoc->GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);
	else
		m_pBasicDataBase=NULL;

	m_nFlag=flag;
	m_grid.SetScheduleBase(m_pTrainDataBase, m_pBasicDataBase);
}
int  CDlgTrainRecord::OnCreate(LPCREATESTRUCT lpCreat)
{
	if(CDialog::OnCreate(lpCreat))
		return -1;
	CRect rect(0,0,0,0);

	if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT == m_schd_type)
		m_grid.CreateGrid(this,rect,IDC_TRAIN_GRID_ID,true);
	else
		m_grid.CreateGrid(this,rect,IDC_TRAIN_GRID_ID,false);
		
	return 0;
}

BOOL CDlgTrainRecord::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDlgTrainRecord::InitDlgCtrlItem()
{
	((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->SetCheck(1);
	((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->SetCheck(1);
	m_grid.SetEditable(TRUE);

	CTG_ScheduleInfo schd;
	m_pTrainDataBase->GetScheduleInfo(schd);
	memset(station_list, 0, sizeof(station_list));
	int nRet=gpcdef.GetTGStationListForCheck(schd.m_nSectionID,sizeof(station_list)/sizeof(station_list[0]),station_list);
	if(nRet<0)
	{
		station_count=0;
		AfxMessageBox("获取车站时,车站数组容量太小,不能获取完整车站!");
	}
	else
	{
		station_count=nRet;
		mStationCombo.ResetContent(); // 重复增加
		int idx=mStationCombo.AddString("全部车站");
		mStationCombo.SetItemData(idx, 0);
		for(int i=0; i<station_count; i++)
		{
			CString str=gpcdef.GetStationName(station_list[i]);
			int idx=mStationCombo.AddString(str);
			mStationCombo.SetItemData(idx, station_list[i]);
		}
		m_selStation=0;
		mStationCombo.SetCurSel(0);
	}

	m_grid.DeleteNonFixedRows();

}

void CDlgTrainRecord::OnBnCheckStationSideAck()
{
	((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->SetCheck(1);
	((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->SetCheck(1);
	mStationCombo.SetCurSel(0);
	m_grid.DeleteNonFixedRows();

	CTG_TrainPtrGroup group;
	m_pTrainDataBase->FindTrains(group);
	int count=m_grid.ShowTrainData(&group, m_selStation, CTrainListGridView::CHECH_ST_MAN_CONFIRM_SIDE);

	CString text="所有车站完成股道确认";
	if(m_grid.stationNum>0)
	{
		text="如下车站没有完成股道确认:\n";
		for(int k=0; k<m_grid.stationNum; k++)
		{
			CString str = gpcdef.GetStationName(m_grid.stationNo[k]);
			text += "\n"+str;
		}
	}
	MessageBox(text, "提示", MB_OK|MB_ICONINFORMATION);

	return;
}

void CDlgTrainRecord::OnCbnSelchangeComboStation()
{
	int nSel=mStationCombo.GetCurSel();
	if(nSel<0)
	{
		MessageBox("请选择要查看的车站!","提示",MB_OK);
		return;
	}

	m_selStation=mStationCombo.GetItemData(nSel);
}

void  CDlgTrainRecord::OnSize(UINT nType,int cx,int cy)
{
	if(m_grid.GetSafeHwnd())
		m_grid.MoveWindow(0,50,cx,cy-55);
	CDialog::OnSize(nType,cx,cy);
}

void  CDlgTrainRecord::OnClose()
{
	this->ShowWindow(SW_HIDE);
}
void  CDlgTrainRecord::OnCancel()
{
	OnClose();
}

CTG_TrainRecord  *CDlgTrainRecord::GetRowTrainRecord(int row)
{
	if(row<=0 || row>=m_grid.GetRowCount())
		return NULL;

	long train_index=m_grid.GetCell(row,0)->GetData();
	long station	=m_grid.GetCell(row,1)->GetData();
	long rec_index	=m_grid.GetCell(row,2)->GetData();

	CTG_SingleTrain *ptrain=m_pTrainDataBase->GetSingleTrain(train_index);
	if(ptrain==NULL)
		return NULL;
	CTG_TrainRecord  *prec=ptrain->GetTrainRecordByRecIndex(rec_index);
	if(prec==NULL)
		return NULL;
	if(prec->GetStationNo()!=station)
		return NULL;
	return prec;
}

bool CDlgTrainRecord::GetSelTrains(CTG_TrainPtrGroup& group, CString& str)
{
	CString ss;
	if(m_selStation==0)
		ss="所有车站";
	else
	{
		CString tmp;
		tmp=gpcdef.GetStationName(m_selStation);
		ss=tmp;
	}

	bool bSel=false;
	str="请注意, 你确实要查看:"+ss;
	if(((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->GetCheck()==1)
	{
		str+=" 客车";
		bSel=true;
	}
	if(((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->GetCheck()==1)
	{
		str+=" 货车";
		bSel=true;
	}
	
	if(!bSel)
	{
		AfxMessageBox("请选择, 要查看的列车类型!");
		return false;
	}
	

	group.Clear();
	CTG_TrainPtrGroup group2;
	m_pTrainDataBase->FindTrains(group2);
	for(int i=0;i<group2.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptr=group2.GetTrainPtr(i);
		
		if(((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->GetCheck()==0)
		{
			if(ptr->IsPassengerTrain())
				continue;
		}
		if(((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->GetCheck()==0)
		{
			if(!ptr->IsPassengerTrain())
				continue;
		}

		group.AddTrainPtr(ptr);
	}

	group.SetTrainsPtr();
	if(group.GetTrainCount()<=0)
	{
		MessageBox("没有要显示的列车!", "提示", MB_OK|MB_ICONWARNING);
	}
	return true;
}

void CDlgTrainRecord::OnBnClickedCheckTrainSide()
{
	CString str;
	CTG_TrainPtrGroup group;
	if(!GetSelTrains(group, str))
		return;
	
	str+=" 股道警告 车次!";
		
	if(IDNO==MessageBox(str, "提示", MB_YESNO | MB_ICONQUESTION))
		return;

	m_grid.DeleteNonFixedRows();
	int count=m_grid.ShowTrainData(&group, m_selStation, CTrainListGridView::CHECH_SIDE_VALID);
}

void CDlgTrainRecord::OnBnClickedReload()
{
	CString str;
	CTG_TrainPtrGroup group;
	if(!GetSelTrains(group, str))
		return;
	str+=" 车次!";
	if(IDNO==MessageBox(str, "提示", MB_YESNO | MB_ICONQUESTION))
		return;

	m_grid.DeleteNonFixedRows();
	int count=m_grid.ShowTrainData(&group, m_selStation, CTrainListGridView::SHOW_ST_SIDE);
}

bool CDlgTrainRecord::IsNeedCheckStation(int station) const
{
	bool bReturn=false;
	for(int i=0; i<station_count; i++)
	{
		if(station_list[i] == station)
		{
			bReturn=true;
			break;
		}
	}

	return bReturn;
}
