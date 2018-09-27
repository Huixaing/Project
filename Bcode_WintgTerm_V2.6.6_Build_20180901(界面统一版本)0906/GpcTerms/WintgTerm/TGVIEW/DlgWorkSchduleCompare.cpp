
#include "stdafx.h"
#include "resource.h"
#include "tgpspdata.h"
#include "tg_basicpara.h"
#include "sysshare.h"
#include "function.h"
#include "tgdocument.h"
#include "DlgWorkSchduleCompare.h"

CDlgWorkSchduleCompare::CDlgWorkSchduleCompare(CWorkScheduleHolder* pHolder, CWnd* pParent) 
		: CDialog(CDlgWorkSchduleCompare::IDD,pParent)
{
	m_hViewWnd=pParent->m_hWnd;
	m_pHolder=pHolder;
}

CDlgWorkSchduleCompare::~CDlgWorkSchduleCompare()
{
}

void CDlgWorkSchduleCompare::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_PASSENGER, m_btnGaoTie);
	DDX_Control(pDX, IDC_CHK_PASSENGER2, m_btnPuKe);
	DDX_Control(pDX, IDC_CHK_FREIGHT, m_btnHuoche);
	
	DDX_Control(pDX, IDC_BUTTON_RE_COMPARE_WORK, m_btnReCheck);

	DDX_Control(pDX, IDC_EDIT1, m_edit1);
	
	DDX_Control(pDX, IDC_STATIC_CHECK1, m_static1);
	DDX_Control(pDX, IDC_STATIC_CHECK2, m_static2);
	DDX_Control(pDX, IDC_STATIC_CHECK3, m_static3);
	DDX_Control(pDX, IDC_STATIC_CHECK4, m_static4);

	DDX_Control(pDX, IDC_RADIO4, m_btnRadioThreeTime);
	DDX_Control(pDX, IDC_RADIO5, m_btnRadioAllViewTime);
}

BEGIN_MESSAGE_MAP(CDlgWorkSchduleCompare, CDialog)
	ON_WM_CLOSE()
	ON_COMMAND(IDC_BUTTON_RE_COMPARE_WORK,ReCompare)
END_MESSAGE_MAP()

BOOL CDlgWorkSchduleCompare::OnInitDialog()
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

void CDlgWorkSchduleCompare::InitGridStation()
{
	CheckStationsOfSection compare_stations;
	if(m_pHolder)
	{
		CTG_ScheduleInfo  schd_info;
		m_pHolder->GetWorkScheduleData()->GetScheduleInfo(schd_info);
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
		int w=compare_stations.station_list[row].setwcheck;
	
		m_staSelGridCtrl.AddRow(station,w);
	}
}

void CDlgWorkSchduleCompare::InitDlgCtrlItem()
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

void CDlgWorkSchduleCompare::SetCompareStation()
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

void CDlgWorkSchduleCompare::OnClose()
{
	this->ShowWindow(SW_HIDE);
}

void CDlgWorkSchduleCompare::OnCancel()
{
	OnClose();
}

void CDlgWorkSchduleCompare::getCompareResultXml(CString& timexml,CString& sidexml,CString& routexml)
{
	timexml="<root>";
	sidexml="<root>";
	routexml="<root>";
	CString str,header;
	vector<DWORD> m_object_has;
	for(int n=0; n<station_count; n++)
	{
		if(m_result[n].list_trains.size()<=0)
			continue;

		int station = m_result[n].station;
		CString stationName=m_result[n].stationName;

		for(int m=0; m<m_result[n].list_trains.size(); m++)
		{
			ResultInfoForWork& a=m_result[n].list_trains[m];
			if(a.arrive_real==1 && a.depart_real==1)
				continue;
			if(a.flag==1) //1:only work has
			{
				if(m_object_has.end() == 
					   std::find(m_object_has.begin(), m_object_has.end(), a.work.train_index))
				{
					m_object_has.push_back(a.work.train_index);
				}
			}
			else if(a.flag==2) // 2:only work and basic has
			{
				NodeRecord w=a.work, b=a.basic;
				if(w.accept!=b.accept || w.giveout!=b.giveout || w.pass_stay!=b.pass_stay)
				{
					timexml+="<record>";

					str.Format("<train_index>%u</train_index>", w.train_index);
					timexml+=str;

					str.Format("<station>%s</station>", stationName);
					timexml+=str;
					str.Format("<trainid>%s</trainid>", a.trainid);
					timexml+=str;
					
					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					timexml+=str;
					
					str.Format("<plan>%s</plan>", GetStationPassStayInfo(w.accept, w.giveout, w.pass_stay));
					timexml+=str;

					str.Format("<basic>%s</basic>", GetStationPassStayInfo(b.accept, b.giveout, b.pass_stay));
					timexml+=str;	

					str.Format("<conclusion>与图定到开方式不同</conclusion>");
					timexml+=str;

					timexml+="</record>";
				}
				if(w.arrive_side!=b.arrive_side || w.depart_side!=b.depart_side)
				{
					sidexml+="<record>";

					str.Format("<train_index>%u</train_index>", w.train_index);
					sidexml+=str;

					str.Format("<station>%s</station>", stationName);
					sidexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					sidexml+=str;
					
					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					sidexml+=str;

		            str.Format("<side>%s</side>", GetSideName(station,w.arrive_side,w.depart_side));
					sidexml+=str;
					
					str.Format("<bside>%s</bside>", GetSideName(station,b.arrive_side,b.depart_side));
					sidexml+=str;
					
					str.Format("<other>%s</other>", GetStationNeedAlarm(a.side_valide));
					sidexml += str;
					
					str.Format("<conclusion>与图定股道不同,日班无车</conclusion>");
					sidexml+=str;

					sidexml+="</record>";
				}
				else if(!IS_VALID_SIDE(w.arrive_side) || !IS_VALID_SIDE(w.depart_side))
				{
					sidexml+="<record>";

					str.Format("<train_index>%u</train_index>", w.train_index);
					sidexml+=str;

					str.Format("<station>%s</station>", stationName);
					sidexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					sidexml+=str;

					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					sidexml+=str;

		            str.Format("<side>%s</side>", GetSideName(station,w.arrive_side,w.depart_side));
					sidexml+=str;
					
					str.Format("<bside>%s</bside>", GetSideName(station,b.arrive_side,b.depart_side));
					sidexml+=str;
					
					str.Format("<other>%s</other>", GetStationNeedAlarm(a.side_valide));
					sidexml += str;

					str.Format("<conclusion>无股道,日班无车</conclusion>");
					sidexml+=str;

					sidexml+="</record>";
				}
				if(w.entry!=b.entry || w.exit!=b.exit)
				{
					routexml+="<record>";
	    
					str.Format("<train_index>%u</train_index>", w.train_index);
					routexml+=str;

					str.Format("<station>%s</station>", stationName);
					routexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					routexml+=str;

					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					routexml+=str;

					if(w.accept==0)
						str.Format("<entry>始发</entry>");
					else
						str.Format("<entry>(%d口)%s</entry>", w.entry, GetTGBasicParam()->GetAbsNameByStationEntry(station, w.entry));
					routexml+=str;

					if(w.giveout==0)
						str.Format("<exit>终到</exit>");
					else
						str.Format("<exit>(%d口)%s</exit>", w.exit, GetTGBasicParam()->GetAbsNameByStationEntry(station, w.exit));
					routexml+=str;

					if(b.accept==0)
						str.Format("<entry1>始发</entry1>");
					else
						str.Format("<entry1>(%d口)%s</entry1>", b.entry, GetTGBasicParam()->GetAbsNameByStationEntry(station, b.entry));
					routexml+=str;

					if(b.giveout==0)
						str.Format("<exit1>终到</exit1>");
					else
						str.Format("<exit1>(%d口)%s</exit1>", b.exit, GetTGBasicParam()->GetAbsNameByStationEntry(station, b.exit));
					routexml+=str;
					
					if(w.entry!=b.entry && w.exit!=b.exit)
						str.Format("<conclusion>与图定出口入口不同,日班无车</conclusion>");
					else if(w.entry!=b.entry)
						str.Format("<conclusion>与图定入口不同,日班无车</conclusion>");
					else
						str.Format("<conclusion>与图定出口不同,日班无车</conclusion>");
					routexml+=str;

					routexml+="</record>";
				}
			}
			else if(a.flag==5) // 5:only work and shift have
			{
				NodeRecord w=a.work, s=a.shift;
				if(w.accept!=s.accept || w.giveout!=s.giveout || w.pass_stay!=s.pass_stay)
				{
					timexml+="<record>";

					str.Format("<train_index>%u</train_index>", w.train_index);
					timexml+=str;

					str.Format("<station>%s</station>", stationName);
					timexml+=str;
					str.Format("<trainid>%s</trainid>", a.trainid);
					timexml+=str;
					
					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					timexml+=str;
					
					str.Format("<plan>%s</plan>", GetStationPassStayInfo(w.accept, w.giveout, w.pass_stay));
					timexml+=str;
					
					str.Format("<shift>%s</shift>", GetStationPassStayInfo(s.accept, s.giveout, s.pass_stay));
					timexml+=str;

					str.Format("<conclusion>与日班到开方式不同</conclusion>");
					timexml+=str;

					timexml+="</record>";
				}
			
				if(w.arrive_side!=s.arrive_side || w.depart_side!=s.depart_side)
				{
					sidexml+="<record>";

					str.Format("<train_index>%u</train_index>", w.train_index);
					sidexml+=str;

					str.Format("<station>%s</station>", stationName);
					sidexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					sidexml+=str;

					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					sidexml+=str;

		            str.Format("<side>%s</side>", GetSideName(station,w.arrive_side,w.depart_side));
					sidexml+=str;
					
					str.Format("<sside>%s</sside>", GetSideName(station,s.arrive_side,s.depart_side));
					sidexml+=str;
					
					str.Format("<other>%s</other>", GetStationNeedAlarm(a.side_valide));
					sidexml += str;

					str.Format("<conclusion>与日班股道不同,基本图无车</conclusion>");
					sidexml+=str;

					sidexml+="</record>";
				}
				else if(!IS_VALID_SIDE(w.arrive_side) || !IS_VALID_SIDE(w.depart_side))
				{
					sidexml+="<record>";

					str.Format("<train_index>%u</train_index>", w.train_index);
					sidexml+=str;

					str.Format("<station>%s</station>", stationName);
					sidexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					sidexml+=str;

					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					sidexml+=str;

		            str.Format("<side>%s</side>", GetSideName(station,w.arrive_side,w.depart_side));
					sidexml+=str;
					
					str.Format("<sside>%s</sside>", GetSideName(station,s.arrive_side,s.depart_side));
					sidexml+=str;
					
					str.Format("<other>%s</other>", GetStationNeedAlarm(a.side_valide));
					sidexml += str;

					str.Format("<conclusion>没有分配股道,基本图无车</conclusion>");
					sidexml+=str;

					sidexml+="</record>";
				}
				if(w.entry!=s.entry || w.exit!=s.exit)
				{
					routexml+="<record>";
	    
					str.Format("<train_index>%u</train_index>", w.train_index);
					routexml+=str;

					str.Format("<station>%s</station>", stationName);
					routexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					routexml+=str;

					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					routexml+=str;

					if(w.accept==0)
						str.Format("<entry>始发</entry>");
					else
						str.Format("<entry>(%d口)%s</entry>", w.entry, GetTGBasicParam()->GetAbsNameByStationEntry(station, w.entry));
					routexml+=str;

					if(w.giveout==0)
						str.Format("<exit>终到</exit>");
					else
						str.Format("<exit>(%d口)%s</exit>", w.exit, GetTGBasicParam()->GetAbsNameByStationEntry(station, w.exit));
					routexml+=str;

					if(s.accept==0)
						str.Format("<entry2>始发</entry2>");
					else
						str.Format("<entry2>(%d口)%s</entry2>", s.entry, GetTGBasicParam()->GetAbsNameByStationEntry(station, s.entry));
					routexml+=str;

					if(s.giveout==0)
						str.Format("<exit2>终到</exit2>");
					else
						str.Format("<exit2>(%d口)%s</exit2>", s.exit, GetTGBasicParam()->GetAbsNameByStationEntry(station, s.exit));
					routexml+=str;

					if(w.entry!=s.entry && w.exit!=s.exit)
						str.Format("<conclusion>与日班出口入口不同,基本图无车</conclusion>");
					else if(w.entry!=s.entry)
						str.Format("<conclusion>与日班入口不同,基本图无车</conclusion>");
					else
						str.Format("<conclusion>与日班出口不同,基本图无车</conclusion>");
					routexml+=str;

					routexml+="</record>";
				}
			}
			else if(a.flag==6) // 6:目标,基本图和日班图有
			{
				NodeRecord w=a.work, b=a.basic, s=a.shift;
				if((w.accept!=b.accept || w.giveout!=b.giveout || w.pass_stay!=b.pass_stay) ||
				   (w.accept!=s.accept || w.giveout!=s.giveout || w.pass_stay!=s.pass_stay))
				{
					timexml+="<record>";

					str.Format("<train_index>%u</train_index>", w.train_index);
					timexml+=str;

					str.Format("<station>%s</station>", stationName);
					timexml+=str;
					str.Format("<trainid>%s</trainid>", a.trainid);
					timexml+=str;
					
					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					timexml+=str;
					
					str.Format("<plan>%s</plan>", GetStationPassStayInfo(w.accept, w.giveout, w.pass_stay));
					timexml+=str;
					
					str.Format("<basic>%s</basic>", GetStationPassStayInfo(b.accept, b.giveout, b.pass_stay));
					timexml+=str;

					str.Format("<shift>%s</shift>", GetStationPassStayInfo(s.accept, s.giveout, s.pass_stay));
					timexml+=str;

					str="";
					if(w.accept!=b.accept || w.giveout!=b.giveout || w.pass_stay!=b.pass_stay)
					{
						if(w.accept!=s.accept || w.giveout!=s.giveout || w.pass_stay!=s.pass_stay)
						{
							str.Format("<conclusion>与图定和日班到开方式均不同</conclusion>");
						}
						else
						{
							str.Format("<conclusion>与图定到开方式不同</conclusion>");
						}
					}
					else
					{
						if(w.accept!=s.accept || w.giveout!=s.giveout || w.pass_stay!=s.pass_stay)
						{
							str.Format("<conclusion>与日班到开方式不同</conclusion>");
						}
					}
					timexml+=str;

					timexml+="</record>";
				}
				if((w.arrive_side!=b.arrive_side || w.depart_side!=b.depart_side) ||
					(w.arrive_side!=s.arrive_side || w.depart_side!=s.depart_side))
				{
					sidexml+="<record>";

					str.Format("<train_index>%u</train_index>", w.train_index);
					sidexml+=str;

					str.Format("<station>%s</station>", stationName);
					sidexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					sidexml+=str;

					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					sidexml+=str;

		            str.Format("<side>%s</side>", GetSideName(station,w.arrive_side,w.depart_side));
					sidexml+=str;
					
					str.Format("<bside>%s</bside>", GetSideName(station,b.arrive_side,b.depart_side));
					sidexml+=str;

					str.Format("<sside>%s</sside>", GetSideName(station,s.arrive_side,s.depart_side));
					sidexml+=str;
					
					str.Format("<other>%s</other>", GetStationNeedAlarm(a.side_valide));
					sidexml += str;

					if(w.arrive_side!=b.arrive_side || w.depart_side!=b.depart_side)
					{
						if(w.arrive_side!=s.arrive_side || w.depart_side!=s.depart_side)
						{
							str.Format("<conclusion>与图定和日班股道不同</conclusion>");
						}
						else
						{
							str.Format("<conclusion>与图定股道不同</conclusion>");
						}
					}
					else if(w.arrive_side!=s.arrive_side || w.depart_side!=s.depart_side)
					{
						str.Format("<conclusion>与日班股道不同</conclusion>");
					}
					else
					{
						str.Format("<conclusion>股道相同</conclusion>");
					}
					sidexml+=str;

					sidexml+="</record>";
				}
				else if(!IS_VALID_SIDE(w.arrive_side) || !IS_VALID_SIDE(w.depart_side))
				{
					sidexml+="<record>";

					str.Format("<train_index>%u</train_index>", w.train_index);
					sidexml+=str;

					str.Format("<station>%s</station>", stationName);
					sidexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					sidexml+=str;

					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					sidexml+=str;

		            str.Format("<side>%s</side>", GetSideName(station,w.arrive_side,w.depart_side));
					sidexml+=str;
					
					str.Format("<bside>%s</bside>", GetSideName(station,b.arrive_side,b.depart_side));
					sidexml+=str;

					str.Format("<sside>%s</sside>", GetSideName(station,s.arrive_side,s.depart_side));
					sidexml+=str;
					
					str.Format("<other>%s</other>", GetStationNeedAlarm(a.side_valide));
					sidexml += str;

					str.Format("<conclusion>没有分配股道</conclusion>");
					sidexml+=str;

					sidexml+="</record>";
				}
				if((w.entry!=b.entry || w.exit!=b.exit) ||
				   (w.entry!=s.entry || w.exit!=s.exit))
				{
					routexml+="<record>";
	    
					str.Format("<train_index>%u</train_index>", w.train_index);
					routexml+=str;

					str.Format("<station>%s</station>", stationName);
					routexml+=str;

					str.Format("<trainid>%s</trainid>", a.trainid);
					routexml+=str;

					if(w.accept==1)
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.arrive));
					else
						str.Format("<dtime>%s</dtime>", GetDateTimeStr(w.depart));
					routexml+=str;

					if(w.accept==0)
						str.Format("<entry>始发</entry>");
					else
						str.Format("<entry>(%d口)%s</entry>", w.entry, GetTGBasicParam()->GetAbsNameByStationEntry(station, w.entry));
					routexml+=str;

					if(w.giveout==0)
						str.Format("<exit>终到</exit>");
					else
						str.Format("<exit>(%d口)%s</exit>", w.exit, GetTGBasicParam()->GetAbsNameByStationEntry(station, w.exit));
					routexml+=str;

					if(b.accept==0)
						str.Format("<entry1>始发</entry1>");
					else
						str.Format("<entry1>(%d口)%s</entry1>", b.entry, GetTGBasicParam()->GetAbsNameByStationEntry(station, b.entry));
					routexml+=str;

					if(b.giveout==0)
						str.Format("<exit1>终到</exit1>");
					else
						str.Format("<exit1>(%d口)%s</exit1>", b.exit, GetTGBasicParam()->GetAbsNameByStationEntry(station, b.exit));
					routexml+=str;

					if(s.accept==0)
						str.Format("<entry2>始发</entry2>");
					else
						str.Format("<entry2>(%d口)%s</entry2>", s.entry, GetTGBasicParam()->GetAbsNameByStationEntry(station, s.entry));
					routexml+=str;

					if(s.giveout==0)
						str.Format("<exit2>终到</exit2>");
					else
						str.Format("<exit2>(%d口)%s</exit2>", s.exit, GetTGBasicParam()->GetAbsNameByStationEntry(station, s.exit));
					routexml+=str;

					CString str1,str2;
					if(w.entry!=b.entry && w.exit!=b.exit)
					{
						if(w.entry!=s.entry && w.exit!=s.exit)
						{
							str.Format("<conclusion>与图定,日班进出站径路不同</conclusion>");
						}
						else if(w.entry!=s.entry)
						{
							str.Format("<conclusion>与图定进出站,日班进站径路不同</conclusion>");
						}
						else if(w.exit!=s.exit)
						{
							str.Format("<conclusion>与图定进出站,日班出站径路不同</conclusion>");
						}
						else
						{
							str.Format("<conclusion>与图定进出站径路不同</conclusion>");
						}
					}
					else if(w.entry!=b.entry)
					{
						if(w.entry!=s.entry && w.exit!=s.exit)
						{
							str.Format("<conclusion>与图定进站,日班进出站径路不同</conclusion>");
						}
						else if(w.entry!=s.entry)
						{
							str.Format("<conclusion>与图定,日班进站径路不同</conclusion>");
						}
						else if(w.exit!=s.exit)
						{
							str.Format("<conclusion>与图定进站,日班出站径路不同</conclusion>");
						}
						else
						{
							str.Format("<conclusion>与图定进站径路不同</conclusion>");
						}
					}
					else if(w.exit!=b.exit)
					{
						if(w.entry!=s.entry && w.exit!=s.exit)
						{
							str.Format("<conclusion>与图定出站,日班进出站径路不同</conclusion>");
						}
						else if(w.entry!=s.entry)
						{
							str.Format("<conclusion>与图定出站,日班进站径路不同</conclusion>");
						}
						else if(w.exit!=s.exit)
						{
							str.Format("<conclusion>与图定,日班出站径路不同</conclusion>");
						}
						else
						{
							str.Format("<conclusion>与图定出站径路不同</conclusion>");
						}
					}
					else
					{
						if(w.entry!=s.entry && w.exit!=s.exit)
						{
							str.Format("<conclusion>与日班进出站径路不同</conclusion>");
						}
						else if(w.entry!=s.entry)
						{
							str.Format("<conclusion>与日班进站径路不同</conclusion>");
						}
						else if(w.exit!=s.exit)
						{
							str.Format("<conclusion>与日班出站径路不同</conclusion>");
						}
						else
						{
							str.Format("<conclusion>径路相同</conclusion>");
						}
					}
					routexml+=str;

					routexml+="</record>";
				}
			}
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
		CTG_SingleTrain *ptrain=m_pHolder->GetWorkScheduleData()->GetSingleTrain(m_object_has[i]);
		if(ptrain)
		{
			strText += ptrain->GetTrainID();
			strText += "\r\n";
		}
	}
	m_edit1.SetWindowText(strText);
}

void CDlgWorkSchduleCompare::ReCompare()
{
	SetCompareStation();
	if(station_count<=0)
	{
		MessageBox("调整图比较时,发现你没有选择比较车站", "提醒");
		return;
	}

	generateObjInfo();
	
	CString timexml, sidexml, routexml;
	getCompareResultXml(timexml, sidexml, routexml);

	m_trainTimeGrid.ShowTrainData(timexml);
	m_trainSideGrid.ShowTrainData(sidexml);
	m_trainRouteGrid.ShowTrainData(routexml);
}

bool CDlgWorkSchduleCompare::IsCompareThisTrain(CTG_SingleTrain* ptrain)
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

void CDlgWorkSchduleCompare::GetCompareTimeRange(TIME& bTm, TIME& eTm, TIME& sTm)
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
		sTm = bTm-12*3600;
	}
	else
	{
		bTm = schd_info.m_tStartTime+12*3600;
		eTm = bTm+12*3600;
		sTm = bTm-12*3600;
	}
}

void CDlgWorkSchduleCompare::generateObjInfo()
{
	if(m_pHolder==NULL)
		return;

	TIME bTm=0, eTm=0, sTm=0;
	GetCompareTimeRange(bTm, eTm, sTm);
	
	CTime sTime(bTm), eTime(eTm);
	CString str;
	str.Format("调整图检查---时间范围[%s-%s]", sTime.Format("%Y-%m-%d %H:%M"), eTime.Format("%Y-%m-%d %H:%M"));
	this->SetWindowText(str);

	CTG_TrainPtrGroup groupObject;
	m_pHolder->GetWorkScheduleData()->FindTrains(groupObject,0,bTm,eTm);
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

			ResultInfoForWork result;
			strncpy(result.trainid, prec->m_data.arrive_train_id, 9);
			result.trainid[9]=0;
			result.flag=1; // 1:目标有
			if(prec->IsActualArriveTimeRecord())
				result.arrive_real=1;
			if(prec->IsActualDepartTimeRecord())
				result.depart_real=1;
			result.side_valide=CheckSideInfo(ptrain, prec, m);

			NodeRecord work;
			GetNodeRecord(work, prec->m_data, m, ptrain->GetTrainRecordCount());
			work.train_index=train_index;
			result.work=work;
			
			m_result[idx].list_trains.push_back(result);
		}
	}

	compareWithBasic();
	compareWithShift(bTm, eTm);
}

void CDlgWorkSchduleCompare::compareWithBasic()
{
	CTG_TrainPtrGroup groupBasic;
	m_pHolder->GetBasicScheduleData()->FindAllTrains(groupBasic);
	for(int n=0; n<groupBasic.GetTrainCount(); n++)
	{
		CTG_SingleTrain* ptrain=groupBasic.GetTrainPtr(n);
		if(ptrain==NULL)
			continue;
		if(!IsCompareThisTrain(ptrain))
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
			
			ResultInfoForWork basicinfo;
			strncpy(basicinfo.trainid, prec->m_data.arrive_train_id, 9);
			basicinfo.trainid[9]=0;
			basicinfo.flag=3; //3:基本图有
			
			NodeRecord basic;
			GetNodeRecord(basic, prec->m_data, m, ptrain->GetTrainRecordCount());
			basic.train_index=train_index1;
			basicinfo.basic=basic;

			compareBasicWithWork(basicinfo, idx);
		}
	}
}

void CDlgWorkSchduleCompare::compareWithShift(TIME bTm, TIME eTm)
{
	CTG_TrainPtrGroup groupShift;
	m_pHolder->GetShiftScheduleData()->FindTrains(groupShift,0,bTm,eTm);
	for(int n=0; n<groupShift.GetTrainCount(); n++)
	{
		CTG_SingleTrain* ptrain=groupShift.GetTrainPtr(n);
		if(ptrain==NULL)
			continue;

		if(!IsCompareThisTrain(ptrain))
			continue;

		DWORD train_index2=ptrain->GetTrainIndex();
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
			
			ResultInfoForWork shiftinfo;
			strncpy(shiftinfo.trainid, prec->m_data.arrive_train_id, 9);
			shiftinfo.trainid[9]=0;
			shiftinfo.flag=4; //4:日班图有
			
			NodeRecord shift;
			GetNodeRecord(shift, prec->m_data, m, ptrain->GetTrainRecordCount());
			shift.train_index=train_index2;
			shiftinfo.shift=shift;

			compareShiftWithWork(shiftinfo, idx);
		}
	}
}

void CDlgWorkSchduleCompare::compareBasicWithWork(const ResultInfoForWork& basicinfo, int idx)
{
	CString strTrainId=basicinfo.trainid;
	int n=0;
	int station = m_result[idx].station;

	for(n=0; n<m_result[idx].list_trains.size(); n++)
	{
		ResultInfoForWork result = m_result[idx].list_trains[n];
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

void CDlgWorkSchduleCompare::compareShiftWithWork(const ResultInfoForWork& shiftinfo, int idx)
{
	CString strTrainId=shiftinfo.trainid;
	int n=0;
	int station = m_result[idx].station;

	for(n=0; n<m_result[idx].list_trains.size(); n++)
	{
		ResultInfoForWork result = m_result[idx].list_trains[n];
		if((strTrainId==result.trainid) && (result.flag==1 || result.flag==2 || result.flag==3))
		{
			if(result.flag==1) // 目标有
				result.flag=5; // 目标和日班有
			if(result.flag==2) // 目标和基本图有
				result.flag=6; // 目标,基本图和日班有
			if(result.flag==3) // 基本图有
				result.flag=7; // 基本图和日班有

			result.shift=shiftinfo.shift;
			
			m_result[idx].list_trains[n]=result;
			break;
		}
	}

	//if(n>=m_result[idx].list_trains.size())
	//{
	//	//日班图独有
	//	m_result[idx].list_trains.push_back(shiftinfo);
	//}
}

////////////////////////////////////////////////////////////////////////////
BOOL CStationTimeListGridForWork::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
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

	strcpy(m_train_def[m_train_col_count].caption,"计划");
	sprintf(m_train_def[m_train_col_count].attr_name,"plan");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;
	
	strcpy(m_train_def[m_train_col_count].caption,"图定");
	sprintf(m_train_def[m_train_col_count].attr_name,"basic");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"日班");
	sprintf(m_train_def[m_train_col_count].attr_name,"shift");
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
	SetColumnWidth(5, 80);
	SetColumnWidth(6, 160);
				
	return TRUE;
}

void CStationTimeListGridForWork::ShowTrainData(CString& xml)
{
	DeleteNonFixedRows();
	InsertRows(xml,"root/record");
}

BOOL CStationSideListGridForWork::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
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

	strcpy(m_train_def[m_train_col_count].caption,"股道");
	sprintf(m_train_def[m_train_col_count].attr_name,"side");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"图定股道");
	sprintf(m_train_def[m_train_col_count].attr_name,"bside");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;
	
	strcpy(m_train_def[m_train_col_count].caption,"日班股道");
	sprintf(m_train_def[m_train_col_count].attr_name,"sside");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;
	
	strcpy(m_train_def[m_train_col_count].caption,"结论");
	sprintf(m_train_def[m_train_col_count].attr_name,"conclusion");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	strcpy(m_train_def[m_train_col_count].caption,"站细报警");
	sprintf(m_train_def[m_train_col_count].attr_name,"other");
	m_train_def[m_train_col_count].data_type=CTC_DATA_STR;
	m_train_def[m_train_col_count].flag=0;
	m_train_col_count++;

	CXMLDataGrid::CreateGrid(parent,rect,ctrlid,m_train_def,m_train_col_count);

	SetColumnWidth(0, 80);
	SetColumnWidth(1, 60);
	SetColumnWidth(2, 80);
	SetColumnWidth(3, 50);
	SetColumnWidth(4, 70);
	SetColumnWidth(5, 70);
	SetColumnWidth(6, 180);
	SetColumnWidth(7, 200);
	return TRUE;
}

void CStationSideListGridForWork::ShowTrainData(CString& xml)
{
	DeleteNonFixedRows();
	InsertRows(xml,"root/record");
}

BOOL CStationRouteListGridForWork::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
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

	strcpy(m_result_def[m_result_col_count].caption,"入口");
	sprintf(m_result_def[m_result_col_count].attr_name,"entry");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"图定入口");
	sprintf(m_result_def[m_result_col_count].attr_name,"entry1");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"日班入口");
	sprintf(m_result_def[m_result_col_count].attr_name,"entry2");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"出口");
	sprintf(m_result_def[m_result_col_count].attr_name,"exit");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"图定出口");
	sprintf(m_result_def[m_result_col_count].attr_name,"exit1");
	m_result_def[m_result_col_count].data_type=CTC_DATA_STR;
	m_result_def[m_result_col_count].flag=0;
	m_result_col_count++;

	strcpy(m_result_def[m_result_col_count].caption,"日班出口");
	sprintf(m_result_def[m_result_col_count].attr_name,"exit2");
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
	SetColumnWidth(3, 160);
	SetColumnWidth(4, 160);
	SetColumnWidth(5, 160);
	SetColumnWidth(6, 160);
	SetColumnWidth(7, 160);
	SetColumnWidth(8, 160);
	SetColumnWidth(9, 200);
		
	return TRUE;
}

void CStationRouteListGridForWork::ShowTrainData(CString& xml)
{
	DeleteNonFixedRows();
	InsertRows(xml,"root/record");
}