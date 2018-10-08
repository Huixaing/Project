
#include "stdafx.h"
#include "resource.h"
#include "tgpspdata.h"
#include "tg_basicpara.h"
#include "sysshare.h"
#include "function.h"
#include "tgdocument.h"
#include "DlgShiftSchduleCompare.h"
#include ".\dlgshiftschdulecompare.h"

CDlgShiftSchduleCompare::CDlgShiftSchduleCompare(CWorkScheduleHolder* pHolder, CWnd* pParent) 
	: CDialog(CDlgShiftSchduleCompare::IDD,pParent)
{
	m_hViewWnd=pParent->m_hWnd;
	m_pHolder=pHolder;
}

CDlgShiftSchduleCompare::~CDlgShiftSchduleCompare()
{
}

void CDlgShiftSchduleCompare::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_PASSENGER, m_btnGaoTie);
	DDX_Control(pDX, IDC_CHK_PASSENGER2, m_btnPuKe);
	DDX_Control(pDX, IDC_CHK_FREIGHT, m_btnHuoche);
	
	DDX_Control(pDX, IDC_RADIO4, m_btnRadioThreeTime);
	DDX_Control(pDX, IDC_RADIO5, m_btnRadioAllViewTime);

	DDX_Control(pDX, IDC_BUTTON_RE_COMPARE_SHIFT, m_btnReCheck);

	DDX_Control(pDX, IDC_EDIT2, m_edit2);

	DDX_Control(pDX, IDC_STATIC_CHECK1, m_static1);
	DDX_Control(pDX, IDC_STATIC_CHECK2, m_static2);
	DDX_Control(pDX, IDC_STATIC_CHECK3, m_static3);
	DDX_Control(pDX, IDC_STATIC_CHECK4, m_static4);
}

BEGIN_MESSAGE_MAP(CDlgShiftSchduleCompare, CDialog)
	ON_WM_CLOSE()
	ON_COMMAND(IDC_BUTTON_RE_COMPARE_SHIFT,ReCompare)
END_MESSAGE_MAP()

BOOL CDlgShiftSchduleCompare::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect(0,0,0,0);
	m_static2.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_trainTimeGrid.CreateGrid(this,rect,IDC_STATIC_CHECK2);
	m_trainTimeGrid.SetSingleRowSelection(TRUE);
	m_trainTimeGrid.SetFindTrainViewHWnd(m_hViewWnd);

	m_static3.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_trainSideGrid.CreateGrid(this,rect,IDC_STATIC_CHECK3);
	m_trainSideGrid.SetSingleRowSelection(TRUE);
	m_trainSideGrid.SetFindTrainViewHWnd(m_hViewWnd);

	m_static4.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_trainRouteGrid.CreateGrid(this,rect,IDC_STATIC_CHECK4);
	m_trainRouteGrid.SetSingleRowSelection(TRUE);
	m_trainRouteGrid.SetFindTrainViewHWnd(m_hViewWnd);

	m_static1.GetWindowRect(rect);
	ScreenToClient(&rect);
	m_staSelGridCtrl.CreateGrid(this,rect,IDC_STATIC_CHECK1);
	m_staSelGridCtrl.SetSingleRowSelection(TRUE);
	m_staSelGridCtrl.SetEditable(TRUE);
	InitGridStation();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDlgShiftSchduleCompare::InitGridStation()
{
	CheckStationsOfSection compare_stations;
	if(m_pHolder)
	{
		CTG_ScheduleInfo  schd_info;
		m_pHolder->GetShiftScheduleData()->GetScheduleInfo(schd_info);
		GetTGFunctionConfig()->LoadCompareStationList(schd_info.m_nSectionID, compare_stations);
	}
	else
	{
		compare_stations.Init();
	}

	m_staSelGridCtrl.DeleteNonFixedRows();
	for (int row = 0; row < compare_stations.count; row++)
	{
		int station=compare_stations.station_list[row].station;
		int s=compare_stations.station_list[row].setscheck;
	
		m_staSelGridCtrl.AddRow(station,s);
	}
}

void CDlgShiftSchduleCompare::InitDlgCtrlItem()
{
	m_btnGaoTie.SetCheck(1);
	m_btnPuKe.SetCheck(1);
	m_btnHuoche.SetCheck(0);
	
	m_trainTimeGrid.DeleteNonFixedRows();
	m_trainSideGrid.DeleteNonFixedRows();
	m_trainRouteGrid.DeleteNonFixedRows();
	
	InitGridStation();
	
	m_btnRadioThreeTime.SetCheck(0);
	m_btnRadioAllViewTime.SetCheck(1);

	ReCompare();
}

void CDlgShiftSchduleCompare::SetCompareStation()
{
	int station_list[128];
	int count = m_staSelGridCtrl.GetSelectStation(station_list);

	station_count=0;
	for(int n=0; n<128; n++)
	{
		m_result[n].clear();
		if(n<count)
		{
			CString strName=gpcdef.GetStationName(station_list[n]);;

			m_result[n].station=station_list[n];
			m_result[n].stationName=strName;
			
			station_count++;
		}
	}
}

void CDlgShiftSchduleCompare::OnClose()
{
	this->ShowWindow(SW_HIDE);
}

void CDlgShiftSchduleCompare::OnCancel()
{
	OnClose();
}

void CDlgShiftSchduleCompare::getCompareResultXml(CString& timexml,CString& sidexml,CString& routexml)
{
	timexml="<root>";
	sidexml="<root>";
	routexml="<root>";
	CString str,header;
	vector<DWORD> m_basic_has, m_object_has;
	for(int n=0; n<station_count; n++)
	{
		if(m_result[n].list_trains.size()<=0)
			continue;

		int station = m_result[n].station;
		CString stationName=m_result[n].stationName;

		for(int m=0; m<m_result[n].list_trains.size(); m++)
		{
			ResultInfo& a=m_result[n].list_trains[m];
			
			if(a.flag==1) //1:目标有
			{
				if(m_object_has.end() == 
					   std::find(m_object_has.begin(), m_object_has.end(), a.obj.train_index))
				{
					m_object_has.push_back(a.obj.train_index);
				}
			}
			else if(a.flag==2) // 2:都有
			{
				NodeRecord o=a.obj, b=a.basic;
				CTime tm1(o.arrive), tm2(o.depart);
				int arrive=tm1.GetHour()*3600+tm1.GetMinute()*60;
				int depart=tm2.GetHour()*3600+tm2.GetMinute()*60;
				if(arrive!=b.arrive || depart!=b.depart || o.accept!=b.accept || o.giveout!=b.giveout)
				{
					timexml+="<record>";

					str.Format("<train_index>%u</train_index>", o.train_index);
					timexml+=str;

					str.Format("<station>%s</station>", stationName);
					timexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					timexml+=str;
					
					if(o.accept==0)
						str.Format("<arrive>始发</arrive>");
					else
						str.Format("<arrive>%s</arrive>", GetDateTimeStr(o.arrive));
					timexml+=str;

					if(o.giveout==0)
						str.Format("<depart>终到</depart>");
					else
						str.Format("<depart>%s</depart>", GetDateTimeStr(o.depart));
					timexml+=str;

					if(b.accept==0)
						str.Format("<arrive1>始发</arrive1>");
					else
						str.Format("<arrive1>%s</arrive1>", GetTimeStr(b.arrive));
					timexml+=str;

					if(b.giveout==0)
						str.Format("<depart1>终到</depart1>");
					else
						str.Format("<depart1>%s</depart1>", GetTimeStr(b.depart));
					timexml+=str;

					if(arrive!=b.arrive && depart!=b.depart)
						str.Format("<conclusion>到发点不同</conclusion>");
					else if(arrive!=b.arrive)
						str.Format("<conclusion>到点不同</conclusion>");
					else if(depart!=b.depart)
						str.Format("<conclusion>发点不同</conclusion>");
					else
						str.Format("<conclusion>时刻相同</conclusion>");
					timexml+=str;

					CString str1=GetStationPassStayInfo(o.accept, o.giveout, o.pass_stay);
					CString str2=GetStationPassStayInfo(b.accept, b.giveout, b.pass_stay);
					if(str1!=str2)
						str.Format("<conclusion2>日班:%s 图定:%s</conclusion2>", str1, str2);
					else
						str.Format("<conclusion2>到开方式相同</conclusion2>", str1, str2);
					timexml+=str;

					timexml+="</record>";
				}
				if(o.arrive_side!=b.arrive_side || o.depart_side!=b.depart_side)
				{
					sidexml+="<record>";

					str.Format("<train_index>%u</train_index>", o.train_index);
					sidexml+=str;

					str.Format("<station>%s</station>", stationName);
					sidexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					sidexml+=str;
					
					if(o.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(o.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(o.depart));
					sidexml+=str;

		            str.Format("<side>%s</side>", GetSideName(station,o.arrive_side,o.depart_side));
					sidexml+=str;
					
					str.Format("<bside>%s</bside>", GetSideName(station,b.arrive_side,b.depart_side));
					sidexml+=str;
					
					str.Format("<other>%s</other>", GetStationNeedAlarm(a.side_valide));
					sidexml += str;

					str.Format("<conclusion>股道不同</conclusion>");
					sidexml+=str;

					sidexml+="</record>";
				}
				else if(!IS_VALID_SIDE(o.arrive_side) || !IS_VALID_SIDE(o.depart_side))
				{
					sidexml+="<record>";

					str.Format("<train_index>%u</train_index>", o.train_index);
					sidexml+=str;

					str.Format("<station>%s</station>", stationName);
					sidexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					sidexml+=str;

					if(o.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(o.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(o.depart));
					sidexml+=str;

		            str.Format("<side>%s</side>", GetSideName(station,o.arrive_side,o.depart_side));
					sidexml+=str;
					
					str.Format("<bside>%s</bside>", GetSideName(station,b.arrive_side,b.depart_side));
					sidexml+=str;
					
					str.Format("<other>%s</other>", GetStationNeedAlarm(a.side_valide));
					sidexml += str;

					str.Format("<conclusion>没有股道</conclusion>");
					sidexml+=str;

					sidexml+="</record>";
				}

				if(o.entry!=b.entry || o.exit!=b.exit)
				{
					routexml+="<record>";
	    
					str.Format("<train_index>%u</train_index>", o.train_index);
					routexml+=str;

					str.Format("<station>%s</station>", stationName);
					routexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					routexml+=str;

					if(o.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(o.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(o.depart));
					routexml+=str;

					if(o.accept==0)
						str.Format("<entry>始发</entry>");
					else
						str.Format("<entry>(%d口)%s</entry>", o.entry,GetTGBasicParam()->GetAbsNameByStationEntry(station, o.entry));
					routexml+=str;

					if(o.giveout==0)
						str.Format("<exit>终到</exit>");
					else
						str.Format("<exit>(%d口)%s</exit>", o.exit,GetTGBasicParam()->GetAbsNameByStationEntry(station, o.exit));
					routexml+=str;

					if(b.accept==0)
						str.Format("<entry1>始发</entry1>");
					else
						str.Format("<entry1>(%d口)%s</entry1>", b.entry,GetTGBasicParam()->GetAbsNameByStationEntry(station, b.entry));
					routexml+=str;

					if(b.giveout==0)
						str.Format("<exit1>终到</exit1>");
					else
						str.Format("<exit1>(%d口)%s</exit1>", b.exit,GetTGBasicParam()->GetAbsNameByStationEntry(station, b.exit));
					routexml+=str;

					if(o.entry!=b.entry && o.exit!=b.exit)
						str.Format("<conclusion>进出站径路不同</conclusion>");
					else if(o.entry!=b.entry)
						str.Format("<conclusion>进站径路不同</conclusion>");
					else if(o.exit!=b.exit)
						str.Format("<conclusion>出站径路不同</conclusion>");
					else
						str.Format("<conclusion>径路相同</conclusion>");
					routexml+=str;

					routexml+="</record>";
				}
			}			
			//else if(a.flag==3) //3:基本图有
			//{
			//	if(m_basic_has.end() == 
			//		   std::find(m_basic_has.begin(), m_basic_has.end(), a.basic.train_index))
			//	{
			//		m_basic_has.push_back(a.basic.train_index);
			//	}
			//}
		}
	}

	timexml+="</root>";
	sidexml+="</root>";
	routexml+="</root>";

	if(m_pHolder==NULL)
		return;

	CString strText="";
	for(int i=0; i<m_object_has.size(); i++)
	{
		CTG_SingleTrain *ptrain=m_pHolder->GetShiftScheduleData()->GetSingleTrain(m_object_has[i]);
		if(ptrain)
		{
			strText += ptrain->GetTrainID();
			strText += "\r\n";
		}
	}
	m_edit2.SetWindowText(strText);
}

void CDlgShiftSchduleCompare::ReCompare()
{
	SetCompareStation();
	if(station_count<=0)
	{
		MessageBox("日班图比较时,发现你没有选择比较车站", "提醒");
		return;
	}

	generateObjInfo();
	
	CString timexml, sidexml, routexml;
	getCompareResultXml(timexml, sidexml, routexml);

	m_trainTimeGrid.ShowTrainData(timexml);
	m_trainSideGrid.ShowTrainData(sidexml);
	m_trainRouteGrid.ShowTrainData(routexml);
}

bool CDlgShiftSchduleCompare::IsCompareThisTrain(CTG_SingleTrain* ptrain)
{
	bool bCompareIt=true;
	if(ptrain)
	{
		if(ptrain->IsPassengerTrain())
		{
			if(ptrain->IsCRHTrain())
			{
				if(m_btnGaoTie.GetCheck()==0)
				{
					bCompareIt=false;
				}
			}
			else
			{
				if(m_btnPuKe.GetCheck()==0)
				{
					bCompareIt=false;
				}
			}
		}
		else
		{
			if(m_btnHuoche.GetCheck()==0)
			{
				bCompareIt=false;
			}
		}
	}

	return bCompareIt;
}

void CDlgShiftSchduleCompare::GetCompareTimeRange(TIME& bTm, TIME& eTm)
{
	CTG_ScheduleInfo schd_info;
	m_pHolder->GetWorkScheduleData()->GetScheduleInfo(schd_info);
	
	if(m_btnRadioThreeTime.GetCheck() || m_btnRadioAllViewTime.GetCheck())
	{
		CTime ctm=CTime::GetCurrentTime();
		bTm=ctm.GetTime();
		bTm=bTm-bTm%60;
		
		if(m_btnRadioThreeTime.GetCheck())
		{	
			eTm=bTm+3*3600;
		}
		else
		{
			eTm=schd_info.m_tStartTime+30*3600;
		}
	}
	else
	{
		bTm=schd_info.m_tStartTime+12*3600;
		eTm=bTm+12*3600;
	}
}

void CDlgShiftSchduleCompare::generateObjInfo()
{
	if(m_pHolder==NULL)
		return;

	TIME bTm=0, eTm=0;
	GetCompareTimeRange(bTm, eTm);
	CTime sTime(bTm), eTime(eTm);
	CString str;
	str.Format("日班图检查----时间范围[%s-%s]", sTime.Format("%Y-%m-%d %H:%M"), eTime.Format("%Y-%m-%d %H:%M"));
	this->SetWindowText(str);

	CTG_TrainPtrGroup groupObject;
	m_pHolder->GetShiftScheduleData()->FindTrains(groupObject,0,bTm,eTm);
	for(int n=0; n<groupObject.GetTrainCount(); n++)
	{
		CTG_SingleTrain* ptrain=groupObject.GetTrainPtr(n);
		if(ptrain==NULL)
			continue;

		if(!IsCompareThisTrain(ptrain))
			continue;

		DWORD train_index=ptrain->GetTrainIndex();
		for(int m=0; m<ptrain->GetTrainRecordCount(); m++)
		{
			CTG_TrainRecord *prec=ptrain->GetTrainRecordNoDelete(m);
			if(prec==NULL)
				continue;
			
			int idx=-1;
			for(int a=0; a<station_count; a++)
			{
				if(m_result[a].station==prec->m_data.station)
				{
					idx=a;
					break;
				}
			}
			if(idx==-1)
				continue;

			ResultInfo result;
			strncpy(result.trainid, prec->m_data.arrive_train_id, 9);
			result.trainid[9]=0;
			result.flag=1; // 1:目标有
			result.side_valide=CheckSideInfo(ptrain, prec, m);

			NodeRecord obj;
			GetNodeRecord(obj, prec->m_data, m, ptrain->GetTrainRecordCount());
			obj.train_index=train_index;
			result.obj=obj;

			m_result[idx].list_trains.push_back(result);
		}
	}

	compareWithBasic();
}

void CDlgShiftSchduleCompare::compareWithBasic()
{
	CTG_TrainPtrGroup groupBasic;
	m_pHolder->GetBasicScheduleData()->FindAllTrains(groupBasic);
	for(int n=0; n<groupBasic.GetTrainCount(); n++)
	{
		CTG_SingleTrain* ptrain=groupBasic.GetTrainPtr(n);
		if(ptrain==NULL)
			continue;
		DWORD train_index1=ptrain->GetTrainIndex();
		for(int m=0; m<ptrain->GetTrainRecordCount(); m++)
		{
			const CTG_TrainRecord *prec=ptrain->GetTrainRecord_Const(m);
			if(prec==NULL)
				continue;
			
			int idx=-1;int a=0;
			for(a=0; a<station_count; a++)
			{
				if(m_result[a].station==prec->m_data.station)
				{
					idx=a;
					break;
				}
			}
			if(idx==-1)
				continue;
			
			//if(!prec->BasicTrainIfInTimeScope(banbie))
			//	continue;

			ResultInfo basicinfo;
			strncpy(basicinfo.trainid, prec->m_data.arrive_train_id, 9);
			basicinfo.trainid[9]=0;
			basicinfo.flag=3; //3:基本图有

			NodeRecord basic;
			GetNodeRecord(basic, prec->m_data, m, ptrain->GetTrainRecordCount());
			basic.train_index=train_index1;
			basicinfo.basic=basic;

			compareObjWithBasic(basicinfo, idx);
		}
	}
}

void CDlgShiftSchduleCompare::compareObjWithBasic(const ResultInfo& basicinfo, int idx)
{
	CString strTrainId=basicinfo.trainid;
	int n=0;
	int station = m_result[idx].station;

	for(n=0; n<m_result[idx].list_trains.size(); n++)
	{
		ResultInfo result = m_result[idx].list_trains[n];
		if(strTrainId==result.trainid && result.flag==1)
		{
			result.flag=2; // 两者都有
			result.basic=basicinfo.basic;
						
			m_result[idx].list_trains[n]=result;
			break;
		}
	}

	//if(n>=m_result[idx].list_trains.size())
	//{
	//	//基本图独有
	//	m_result[idx].list_trains.push_back(basicinfo);
	//}
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CStationTimeListGridForShift::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	m_train_col_count=0;
	strcpy(m_train_def[m_train_col_count].caption,"车站");
	sprintf(m_train_def[m_train_col_count].attr_name,"station");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"车次");
	sprintf(m_train_def[m_train_col_count].attr_name,"trainid");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"日班到点");
	sprintf(m_train_def[m_train_col_count].attr_name,"arrive");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;
	
	strcpy(m_train_def[m_train_col_count].caption,"图定到点");
	sprintf(m_train_def[m_train_col_count].attr_name,"arrive1");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"日班发点");
	sprintf(m_train_def[m_train_col_count].attr_name,"depart");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"图定发点");
	sprintf(m_train_def[m_train_col_count].attr_name,"depart1");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"结论1");
	sprintf(m_train_def[m_train_col_count].attr_name,"conclusion");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"结论2");
	sprintf(m_train_def[m_train_col_count].attr_name,"conclusion2");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_train_def,m_train_col_count);

	SetColumnWidth(0, 80);
	SetColumnWidth(1, 60);
	SetColumnWidth(2, 80);
	SetColumnWidth(3, 90);
	SetColumnWidth(4, 90);
	SetColumnWidth(5, 90);
	SetColumnWidth(6, 150);
	SetColumnWidth(7, 150);
			
	return TRUE;
}

void CStationTimeListGridForShift::ShowTrainData(CString& xml)
{
	DeleteNonFixedRows();
	InsertRows(xml,"root/record");
}

BOOL CStationSideListGridForShift::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	m_train_col_count=0;
	strcpy(m_train_def[m_train_col_count].caption,"车站");
	sprintf(m_train_def[m_train_col_count].attr_name,"station");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"车次");
	sprintf(m_train_def[m_train_col_count].attr_name,"trainid");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"时刻");
	sprintf(m_train_def[m_train_col_count].attr_name,"dtime");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"日班股道");
	sprintf(m_train_def[m_train_col_count].attr_name,"side");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"图定股道");
	sprintf(m_train_def[m_train_col_count].attr_name,"bside");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;
	
	strcpy(m_train_def[m_train_col_count].caption,"站细报警");
	sprintf(m_train_def[m_train_col_count].attr_name,"other");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"结论");
	sprintf(m_train_def[m_train_col_count].attr_name,"conclusion");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_train_def,m_train_col_count);

	SetColumnWidth(0, 80);
	SetColumnWidth(1, 60);
	SetColumnWidth(2, 80);
	SetColumnWidth(3, 80);
	SetColumnWidth(4, 80);
	SetColumnWidth(5, 140);
	SetColumnWidth(6, 180);
	
	return TRUE;
}

void CStationSideListGridForShift::ShowTrainData(CString& xml)
{
	DeleteNonFixedRows();
	InsertRows(xml,"root/record");
}

BOOL CStationRouteListGridForShift::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	m_result_col_count=0;
	strcpy(m_result_def[m_result_col_count].caption,"车站");
	sprintf(m_result_def[m_result_col_count].attr_name,"station");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"车次");
	sprintf(m_result_def[m_result_col_count].attr_name,"trainid");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"时刻");
	sprintf(m_result_def[m_result_col_count].attr_name,"dtime");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"日班入口");
	sprintf(m_result_def[m_result_col_count].attr_name,"entry");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"图定入口");
	sprintf(m_result_def[m_result_col_count].attr_name,"entry1");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"日班出口");
	sprintf(m_result_def[m_result_col_count].attr_name,"exit");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"图定出口");
	sprintf(m_result_def[m_result_col_count].attr_name,"exit1");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"结论");
	sprintf(m_result_def[m_result_col_count].attr_name,"conclusion");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_result_def,m_result_col_count);

	SetColumnWidth(0, 80);
	SetColumnWidth(1, 60);
	SetColumnWidth(2, 80);
	SetColumnWidth(3, 200);
	SetColumnWidth(4, 200);
	SetColumnWidth(5, 200);
	SetColumnWidth(6, 200);
	SetColumnWidth(7, 200);
	
	return TRUE;
}

void CStationRouteListGridForShift::ShowTrainData(CString& xml)
{
	DeleteNonFixedRows();
	InsertRows(xml,"root/record");
}

