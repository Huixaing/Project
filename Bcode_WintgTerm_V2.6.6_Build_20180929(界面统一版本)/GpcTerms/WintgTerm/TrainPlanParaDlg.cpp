// TrainPlanParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainPlanParaDlg.h"
#include "marcodefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrainPlanParaDlg dialog


CTrainPlanParaDlg::CTrainPlanParaDlg(CWintgDoc *pDoc,int schd_type,CWnd* pParent /*=NULL*/)
	: CDialog(CTrainPlanParaDlg::IDD, pParent)
{
	m_nSchdType= schd_type;
	//{{AFX_DATA_INIT(CTrainPlanParaDlg)
	m_check_adjust_start = FALSE;
	m_check_engine = FALSE;
	m_check_track = FALSE;
	m_next_abs_time = 0;
	m_check_stop_train = FALSE;
	m_check_pass_side = FALSE;
	m_check_permit_out = FALSE;
	m_station = _T("");
	m_train_id = _T("");
	m_stop_time = _T("");
	m_stop_time_min = _T("");
	m_check_abs_time = FALSE;
	m_sLoadEngine = _T("");
	m_sAbsTime = _T("");
	m_Plan_arrival_time = _T("");
	m_Plan_depart_time = _T("");
	m_arrival_time = _T("");
	m_depart_time = _T("");
	m_Collide_reason = _T("");
	m_stop_reason = _T("");
	m_exit = 0;
	m_entry = 0;
	m_Plan_arrival_date = _T("");
	m_Plan_depart_date = _T("");
	m_spermit_out_time = _T("");
	returnOK = FALSE;
	m_spermit_out_date = _T("");
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	isSendBehind = FALSE;//ADD BY CF
	m_Brush.CreateSolidBrush(CLR_LIGHTRED); //color initiate;
	
}

void CTrainPlanParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrainPlanParaDlg)
	DDX_Control(pDX, IDC_PERMIT_OUT_DATE, m_permit_out_date);
	DDX_Control(pDX, IDC_COMBO_PREVIOUSABS, m_combo_previous_abs);
	DDX_Control(pDX, IDC_PERMIT_OUT, m_ctrlPermitOutButton);
	DDX_Control(pDX, IDC_CHECK_ENGINE, m_ctrlEngineButton);
	DDX_Control(pDX, IDC_CHECK_ABS_TIME, m_ctrlAbsTimeButton);
	DDX_Control(pDX, IDC_CHECK_TRACK, m_ctrlTrackButton);
	DDX_Control(pDX, IDC_ABS_TIME, m_ctrlAbsTime);
	DDX_Control(pDX, IDC_LOAD_ENGINE, m_load_engine);
	DDX_Control(pDX, IDC_PERMIT_OUT_TIME, m_permit_out_time);
	DDX_Control(pDX, IDC_COMBO_TRACK, m_combo_track);
	DDX_Control(pDX, IDC_COMBO_NEXTABS, m_combo_next_abs);
	DDX_Check(pDX, IDC_ADJUST_START, m_check_adjust_start);
	DDX_Check(pDX, IDC_CHECK_ENGINE, m_check_engine);
	DDX_Check(pDX, IDC_CHECK_TRACK, m_check_track);
	DDX_Check(pDX, IDC_NO_STOP_TRAIN, m_check_stop_train);
	DDX_Check(pDX, IDC_PASS_SIDE, m_check_pass_side);
	DDX_Check(pDX, IDC_PERMIT_OUT, m_check_permit_out);
	DDX_Text(pDX, IDC_STATION, m_station);
	DDX_Text(pDX, IDC_TRAIN_ID, m_train_id);
	DDX_Text(pDX, IDC_STOP_TIME, m_stop_time);
	DDX_Text(pDX, IDC_STOP_TIME_MIN, m_stop_time_min);
	DDX_Check(pDX, IDC_CHECK_ABS_TIME, m_check_abs_time);
	DDX_Text(pDX, IDC_LOAD_ENGINE, m_sLoadEngine);
	DDX_Text(pDX, IDC_ABS_TIME, m_sAbsTime);
	DDX_Text(pDX, IDC_EDIT1, m_Plan_arrival_time);
	DDX_Text(pDX, IDC_EDIT2, m_Plan_depart_time);
	DDX_Text(pDX, IDC_EDIT7, m_arrival_time);
	DDX_Text(pDX, IDC_EDIT8, m_depart_time);
	DDX_Text(pDX, IDC_EDIT9, m_Collide_reason);
	DDX_Text(pDX, IDC_EDIT4, m_exit);
	DDX_Text(pDX, IDC_EDIT5, m_entry);
	DDX_Text(pDX, IDC_EDIT10, m_Plan_arrival_date);
	DDX_Text(pDX, IDC_EDIT11, m_Plan_depart_date);
	DDX_Text(pDX, IDC_PERMIT_OUT_TIME, m_spermit_out_time);
	DDX_Text(pDX, IDC_PERMIT_OUT_DATE, m_spermit_out_date);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrainPlanParaDlg, CDialog)
	//{{AFX_MSG_MAP(CTrainPlanParaDlg)
	ON_BN_CLICKED(IDC_CHECK_ENGINE, OnCheckEngine)
	ON_BN_CLICKED(IDC_CHECK_TRACK, OnCheckTrack)
	ON_BN_CLICKED(IDC_CHECK_ABS_TIME, OnCheckAbsTime)
	ON_BN_CLICKED(IDC_PERMIT_OUT, OnPermitOut)
	ON_EN_KILLFOCUS(IDC_STOP_TIME, OnKillfocusStopTime)
	ON_CBN_SELCHANGE(IDC_COMBO_NEXTABS, OnSelchangeComboNextabs)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainPlanParaDlg message handlers

BOOL CTrainPlanParaDlg::OnInitDialog() 
{
   	int i ;
	int index = 0;
  	int node_index_current,node_index_next; int node_index_another = 0;  //20070327 南昌，tg可能存在另外的序
	int side_no;
	CString str;

	CDialog::OnInitDialog();

	sameAsPasstoStay  = FALSE;
 	m_check_limit = FALSE; 
	switch(m_nSchdType){
	case WORK_SCHEDULE:
		m_pTrain = m_pDoc->m_sFocusTrain;
		pDispDistance = m_pDoc->m_sDispDistance;
		m_dispNum = m_pDoc->m_nDispDistanceNum;
		break;
	case UNCHANGE_HISTORY_SCHEDULE:
	case HISTORY_SCHEDULE:
		m_pTrain = m_pDoc->m_sFocusHistTrain;
		pDispDistance = m_pDoc->m_sDispHistDistance;
		m_dispNum = m_pDoc->m_nDispDistanceHistNum;
		break;
	case BASIC_SCHEDULE:
		m_pTrain = m_pDoc->m_sFocusBasicTrain;
		pDispDistance = m_pDoc->m_sDispBasicDistance;
		m_dispNum = m_pDoc->m_nDispDistanceBasicNum;
		break;
	case NEIGHBER_SCHEDULE:
		m_pTrain = m_pDoc->m_sFocusNeighberTrain;
		pDispDistance = m_pDoc->m_sDispNeighberDistance;
		m_dispNum = m_pDoc->m_nDispDistanceNeighberNum;
		break;
	default:
		break;
    }

    if(m_pTrain == NULL) 
		return FALSE;

	if(m_pTrain->m_nFocusRecordIndex == -1)	
		return FALSE;
    
	   
 	m_train_id = m_pTrain->m_strTrainId;
    rec = m_pTrain->GetIndexRecord(m_pTrain->m_nFocusRecordIndex);
    m_station =  gpcdef.GetStationName(rec->station);
	m_exit = short(rec->exit);
	m_entry = short(rec->entry);
    m_nCurrEntry = short(rec->entry);
	m_nCurrExit = short(rec->exit);

	if((rec->depart_time - rec->arrival_time + OsGetTwoDateOffset(PMTG.ThreadId,rec->depart_date,rec->arrival_date) * WHOLE_DAY_SECOND)<0)
	{
		m_stop_time = (CString)OsConvertTimetoString(rec->depart_time - rec->arrival_time 
			+ OsGetTwoDateOffset(PMTG.ThreadId,rec->depart_date,rec->arrival_date) * WHOLE_DAY_SECOND);
		orignStopTime = rec->depart_time - rec->arrival_time 
			+ OsGetTwoDateOffset(PMTG.ThreadId,rec->depart_date,rec->arrival_date) * WHOLE_DAY_SECOND;
		originalStopTime = m_stop_time;
		m_stop_time = (CString)OsConvertTimetoString(0);
	}
	else
	{
		m_stop_time = (CString)OsConvertTimetoString(rec->depart_time - rec->arrival_time 
			+ OsGetTwoDateOffset(PMTG.ThreadId,rec->depart_date,rec->arrival_date) * WHOLE_DAY_SECOND);
		orignStopTime = rec->depart_time - rec->arrival_time 
			+ OsGetTwoDateOffset(PMTG.ThreadId,rec->depart_date,rec->arrival_date) * WHOLE_DAY_SECOND;
		originalStopTime = m_stop_time;
	}

	if(rec->min_stop_time >0x8000)
	{
		m_stop_time_min = OsConvertTimetoString(0);
	}
	else
	{
		m_stop_time_min = OsConvertTimetoString(rec->min_stop_time);
	}

	//获得当前记录的节点号
	if(IsFlagStart(rec->flag))
	{
		if(m_pDoc->GetNodeIndex(m_pTrain,rec,node_index_current,EXIT_POSITION,m_pDoc->m_nCurrentSectionNo) < 0)
			return FALSE;
	}
	else
	{
		if(m_pDoc->GetNodeIndex(m_pTrain,rec,node_index_current,ENTRY_POSITION,m_pDoc->m_nCurrentSectionNo) < 0)
			return FALSE;

        if(m_pDoc->GetNodeIndex(m_pTrain,rec,node_index_another,EXIT_POSITION,m_pDoc->m_nCurrentSectionNo) < 0)
		{
			node_index_another = 0;
		}
	}
    
 
 	for(i = 2; i < (*pDispDistance)[node_index_current].distance.number_of_side; i++)
	{
       side_no = (*pDispDistance)[node_index_current].distance.side[i].side_number;
	   str.Format("%s",(*pDispDistance)[node_index_current].distance.side[i].sidename);
 	   m_combo_track.AddString(str);
	   m_combo_track.SetItemData(i-2,side_no);
	   if(rec->arrive_side == side_no){
		   index = i-2;
		   m_combo_track.SetCurSel(index);
	   }
	}
     
	if(IsForceSide(*rec)){
		m_check_track = TRUE;
	}else{
		m_check_track = FALSE;
		m_combo_track.EnableWindow(FALSE);
	}
    
	// load engine
	for(i = 0;i< m_pTrain->m_nTrainInfo.engine_count ;i++){
		if(rec->station == m_pTrain->m_nTrainInfo.attach_station[i]){
            m_sLoadEngine = m_pTrain->m_nTrainInfo.engine_id[i];
		    m_check_engine = TRUE;
		}
	} 
    m_load_engine.EnableWindow (m_check_engine);
     
	// for next abs time
	if(IsSpecifyAbsRunTime(rec))
	{
		m_check_abs_time = TRUE;
		m_ctrlAbsTimeButton.SetCheck(TRUE);
		m_ctrlAbsTime.EnableWindow (TRUE);
	}
	else
	{
		m_check_abs_time = FALSE;
		m_ctrlAbsTimeButton.SetCheck(FALSE);
		m_ctrlAbsTime.EnableWindow (FALSE);
	}

    Pstruct TRAINRECORD *next_rec;
	next_rec = m_pTrain->GetIndexRecord(m_pTrain->m_nFocusRecordIndex + 1);
	
	long lTimeValue,lDateValue;
    if(next_rec != NULL){
	    m_nNextEntry = next_rec->entry;

		lTimeValue = (rec->depart_time == NOTIME)?rec->arrival_time:rec->depart_time;
		lDateValue = (rec->depart_date == NOTIME)?rec->arrival_date:rec->depart_date;

		if(m_check_abs_time)
		{
			originalAbsTime = OsConvertTimetoString(rec->side_utility * 60);
			m_sAbsTime = OsConvertTimetoString(rec->side_utility * 60);
		}
		else
		{
			originalAbsTime = (CString)OsConvertTimetoString(next_rec->arrival_time - rec->depart_time \
				+ OsGetTwoDateOffset(PMTG.ThreadId,rec->depart_date,next_rec->arrival_date) * WHOLE_DAY_SECOND);
			m_sAbsTime = originalAbsTime;
		}
	}else
		m_sAbsTime = "";
#define UP_MARK		1
#define DOWN_MARK	2    
	//先判断上下行
	int DirectionFlag = UP_MARK;	//UP_MARK上行，DOWN_MARK下行,RET_FAILED

	DirectionFlag = m_pDoc->GetEntryExitDirection(m_pTrain,rec,EXIT_POSITION,m_pDoc->m_nCurrentSectionNo,m_nSchdType);
	
	
	// for curr abs line 
	Pstruct _node_info *node_info_current,*node_info_next,*node_info, *node_info_another;    
	Pstruct TRAINRECORD *previous_rec;

	int abs_line_count = 0;    
    do{
		node_info_current = &(*pDispDistance)[node_index_current].distance.node_info;
		if(next_rec != NULL)
		{
			if(m_pDoc->GetNodeIndex(m_pTrain,next_rec,node_index_next,ENTRY_POSITION,m_pDoc->m_nCurrentSectionNo) < 0)
			{
 				for(i=0; i<node_info_current->down_line_count; i++)
					abs_line[abs_line_count ++] = node_info_current->down_line_list [i];
				for(i=0; i<node_info_current->up_line_count; i++)
					abs_line[abs_line_count ++] = node_info_current->up_line_list [i];
         
				break;
			}
			//如果为区间作业折返车次，根据车次判断车方向，然后上行车显示全部上方连通线，下行车显示全部下方连通线
			else if(IsFlagStopAbs(rec->flag) && rec->station == next_rec->station)
			{
				if(IsFlagStopAbsUp(rec->flag))//上行
				{
					for(i=0; i<node_info_current->up_line_count; i++)
						abs_line[abs_line_count ++] = node_info_current->up_line_list [i];

				}
				else
				{
					for(i=0; i<node_info_current->down_line_count; i++)
						abs_line[abs_line_count ++] = node_info_current->down_line_list [i];

				}


				break;
			}
			else
				;
			node_info_next = &(*pDispDistance)[node_index_next].distance.node_info;
		
	//		if(DirectionFlag == DOWN_MARK)//下行
			{
				for(i=0;i<node_info_current->down_line_count;i++)
				{
					for(int j= 0;j<node_info_next->up_line_count ;j++)
					{
						if(node_info_current->down_line_list [i] == node_info_next->up_line_list [j])
							abs_line[abs_line_count ++] = node_info_current->down_line_list [i];
					}
				}
			}
	//		else if(DirectionFlag == UP_MARK)//上行
			{
				for(i=0;i<node_info_current->up_line_count;i++)
				{
					for(int j= 0;j<node_info_next->down_line_count ;j++)
					{
						if(node_info_current->up_line_list [i] == node_info_next->down_line_list [j])
							abs_line[abs_line_count ++] = node_info_current->up_line_list [i];
					}
				}   
			}
			////////////////////
			//new added 20070327 南昌
			if(node_index_another >0 )
			{
				node_info_another = &(*pDispDistance)[node_index_another].distance.node_info;
			    for(i=0;i<node_info_another->down_line_count;i++)
				{
					for(int j= 0;j<node_info_next->up_line_count ;j++)
					{
						if(node_info_another->down_line_list [i] == node_info_next->up_line_list [j])
						{
                            for(int n=0;n<abs_line_count;n++)
							{  if(abs_line[n] == node_info_another->down_line_list [i])
							   break;
								 
							}
							if(n >= abs_line_count )
							   abs_line[abs_line_count ++] = node_info_another->down_line_list [i];
						}
					}
				}
				for(i=0;i<node_info_another->up_line_count;i++)
				{
					for(int j= 0;j<node_info_next->down_line_count ;j++)
					{
						if(node_info_another->up_line_list [i] == node_info_next->down_line_list [j])
						{
                            for(int n=0;n<abs_line_count;n++)
							{  if(abs_line[n] == node_info_another->up_line_list [i])
							   break;
								 
							}
							if(n >= abs_line_count )
							   abs_line[abs_line_count ++] = node_info_another->up_line_list [i];

						}
							 
					}
				}   

			}
			////////////////////////
 
		}else{//下方无车站 ，该车站为终止车站，那么使用该车站的原始线号
			
			for(int m=0;m < m_dispNum;m++){
				node_info = &(*pDispDistance)[m].distance.node_info;
				if(node_info_current->station_number != node_info->station_number)
					continue;
				bool bFlag = FALSE;
				for(i=0; i<node_info->down_line_count; i++){
					bFlag = FALSE; // qiaoyt20081210
					for(int k=0;k<abs_line_count;k++){
						if(abs_line[k] == node_info->down_line_list [i]){
                            bFlag = TRUE;
							break;
						}
					}
					if(bFlag) continue;
					abs_line[abs_line_count ++] = node_info->down_line_list [i];
				}
				
				for(i=0; i<node_info->up_line_count; i++)
				{
					bFlag = FALSE;
					for(int k=0;k<abs_line_count;k++){
						if(abs_line[k] == node_info->up_line_list [i]){
                            bFlag = TRUE;
							break;
						}
					}
					if(bFlag) continue;
					abs_line[abs_line_count ++] = node_info->up_line_list [i];
				}
			}

		}
    }while(0);

	ushort nAbsNum,nDir;
    ConvertStationEntryToAbsnum(rec->station,rec->exit,&nAbsNum,&nDir);
	index = -1;  
	CString sStationStart,sStationEnd;
	if(abs_line_count >0)//在所有线号中查找当前找到的线号名称
	{
		for(i=0;i<abs_line_count;i++)
		{
			for(int j= 0;j< m_pDoc->m_nAbsLineCount ;j++)
			{
				if(m_pDoc->abs_line_info[j].nLineNo == abs_line[i])
				{
//					str = CString(m_pDoc->abs_line_info[j].sLineName) ;
                    //str.Format("%s(%d)",m_pDoc->abs_line_info[j].sLineName,abs_line[i]);
					if(m_pDoc->abs_line_info[j].nStartStation>0)
					   sStationStart.Format("%s",gpcdef.GetStationName(m_pDoc->abs_line_info[j].nStartStation ));
					else
						sStationStart ="管外";
					if(m_pDoc->abs_line_info[j].nEndStation>0)
					   sStationEnd.Format("%s",gpcdef.GetStationName(m_pDoc->abs_line_info[j].nEndStation ));
					else
						sStationEnd ="管外";
					if(m_pDoc->abs_line_info[j].nDirect !=3)
					   str.Format("%s(%d) %s-－>%s",m_pDoc->abs_line_info[j].sLineName,  abs_line[i],sStationStart,sStationEnd );
					else
					   str.Format("%s(%d) %s<-－>%s",m_pDoc->abs_line_info[j].sLineName,  abs_line[i],sStationStart,sStationEnd );
	                if(nAbsNum == abs_line[i])
						index = i;
			        break;
				}
			}
 		    m_combo_next_abs.AddString(str);
		    m_combo_next_abs.SetItemData (i,(DWORD)abs_line[i]);
		}
	}

	//出口为空,则显示空
	if(rec->exit != ushort(-1))
      m_combo_next_abs.SetCurSel (index);


//previous abs line info
//	abs_line_count = 0;    
	int abs_line_count_previous= 0;
    do{
		previous_rec = m_pTrain->GetIndexRecord(m_pTrain->m_nFocusRecordIndex - 1);

		int node_index_previous;
		
		Pstruct _node_info *node_info_previous;    

		if(previous_rec != NULL)
		{
		    m_nPrevExit = previous_rec->exit;

			if(m_pDoc->GetNodeIndex(m_pTrain,previous_rec,node_index_previous,EXIT_POSITION,m_pDoc->m_nCurrentSectionNo) < 0)
			{
 				for(i=0; i<node_info_current->down_line_count; i++)
					abs_line_previous[abs_line_count_previous ++] = node_info_current->down_line_list [i];
				for(i=0; i<node_info_current->up_line_count; i++)
					abs_line_previous[abs_line_count_previous ++] = node_info_current->up_line_list [i];
				break;
			}
			
			node_info_previous = &(*pDispDistance)[node_index_previous].distance.node_info;
					
	//		if(DirectionFlag == DOWN_MARK)//下行
			{
				for(i=0;i<node_info_current->up_line_count;i++)
				{
					for(int j= 0;j<node_info_previous->down_line_count ;j++)
					{
						if(node_info_current->up_line_list [i] == node_info_previous->down_line_list [j])
							abs_line_previous[abs_line_count_previous ++] = node_info_current->up_line_list [i];
					}
				}   
			}
	//		else if(DirectionFlag == UP_MARK)
			{ //上行
				for(i=0;i<node_info_current->down_line_count;i++)
				{
					for(int j= 0;j<node_info_previous->up_line_count ;j++)
					{
						if(node_info_current->down_line_list [i] == node_info_previous->up_line_list [j])
							abs_line_previous[abs_line_count_previous ++] = node_info_current->down_line_list [i];
					}
				}
			}
		}else{ //上方无车站 ，该车站为起始车站，那么使用该车站的原始线号
			for(int m=0;m < m_dispNum;m++){
				node_info = &(*pDispDistance)[m].distance.node_info;
				if(node_info_current->station_number != node_info->station_number)
					continue;
				bool bFlag = FALSE;
				for(i=0; i<node_info->down_line_count; i++){
					for(int k=0;k<abs_line_count_previous;k++){
						if(abs_line[k] == node_info->down_line_list [i]){
                            bFlag = TRUE;
							break;
						}
					}
					if(bFlag) continue;
					abs_line_previous[abs_line_count_previous ++] = node_info->down_line_list [i];
				}
				bFlag = FALSE;
				for(i=0; i<node_info->up_line_count; i++){
					for(int k=0;k<abs_line_count_previous;k++){
						if(abs_line[k] == node_info->up_line_list [i]){
                            bFlag = TRUE;
							break;
						}
					}
					if(bFlag) continue;
					abs_line_previous[abs_line_count_previous ++] = node_info->up_line_list [i];
				}
			}
 
		}
    }while(0);

//	ushort nAbsNum,nDir;
    ConvertStationEntryToAbsnum(rec->station,rec->entry,&nAbsNum,&nDir);
	
	index = -1;

    if(abs_line_count_previous >0)
	{
		for(i=0;i<abs_line_count_previous;i++)
		{
			for(int j= 0;j< m_pDoc->m_nAbsLineCount ;j++)
			{
				if(m_pDoc->abs_line_info[j].nLineNo == abs_line_previous[i])
				{
//					str = CString(m_pDoc->abs_line_info[j].sLineName) ;
					if(m_pDoc->abs_line_info[j].nStartStation>0)
					   sStationStart.Format("%s",gpcdef.GetStationName(m_pDoc->abs_line_info[j].nStartStation ));
					else
						sStationStart ="管外";
					if(m_pDoc->abs_line_info[j].nEndStation>0)
					   sStationEnd.Format("%s",gpcdef.GetStationName(m_pDoc->abs_line_info[j].nEndStation ));
					else
						sStationEnd ="管外";
					if(m_pDoc->abs_line_info[j].nDirect !=3)
					   str.Format("%s(%d) %s-－>%s",m_pDoc->abs_line_info[j].sLineName,  abs_line_previous[i], sStationStart, sStationEnd );
					else
                       str.Format("%s(%d) %s<-－>%s",m_pDoc->abs_line_info[j].sLineName,  abs_line_previous[i], sStationStart, sStationEnd );
                  //  str.Format("%s(%d)",m_pDoc->abs_line_info[j].sLineName,abs_line_previous[i]);
					if(nAbsNum == abs_line_previous[i])
						index = i;
			        break;
				}
			}
 		    m_combo_previous_abs.AddString(str);
		    m_combo_previous_abs.SetItemData (i,(DWORD)abs_line_previous[i]);
		}
	}
	//入口为空,则显示为空
	if(rec->entry != ushort(-1))
       m_combo_previous_abs.SetCurSel (index);
    
 
    //early depart time  
	if(rec->early_depart_time == (short)0x8000){  //0
 		m_uPermitoutTime = 0;  
		m_spermit_out_time.Empty();

		m_spermit_out_date =  OsConvertDatetoString(rec->depart_date);
		m_spermit_out_time =  OsConvertTimetoString(rec->depart_time);
        
		//if(m_spermit_out_time.GetLength()>5)
		//m_spermit_out_time = m_spermit_out_time.Left(5);

		m_permit_out_date.EnableWindow (FALSE);
 		m_permit_out_time.EnableWindow (FALSE);         
		m_check_permit_out = FALSE;
		 		
	}else{
        
		m_uPermitoutTime = (int)(rec->early_depart_time);
		time_t t_plan = long_time(rec->plan_depart_date, rec->plan_depart_time); //second
		//计算早晚点的时候，消除计划时间中的秒数，变为整分钟
		CTime t = CTime(t_plan/60*60 + m_uPermitoutTime*60);
		long  m_npermit_out_time = (t.GetHour( ) * 60 + t.GetMinute( ))*60 + t.GetSecond();  //minute
        long  m_npermit_out_date = (t.GetYear( ) - 1990) * 380 + (t.GetMonth( ) - 1)* 31 + t.GetDay( ) - 1; 
		
		m_spermit_out_date = OsConvertDatetoString(m_npermit_out_date);		
		m_spermit_out_time = OsConvertTimetoString(m_npermit_out_time);	
		
 	      m_permit_out_date.EnableWindow(TRUE);
		  m_permit_out_time.EnableWindow(TRUE);
		  m_check_permit_out = TRUE;
		  
	}

	//check flag
	if(IsForcePassByMainSide(*rec))
		m_check_pass_side = FALSE;
	else
		m_check_pass_side = TRUE; 
	///end of cf
    
	if((USHORT)(m_pTrain->m_nTrainInfo.adjust_begin_sta & ~0x0080) == (USHORT)rec->rec_index)
		m_check_adjust_start = TRUE;
	else
		m_check_adjust_start = FALSE;
	
    if(IsForcePass(*rec))//modify by cf after 7-18
		m_check_stop_train = TRUE;
	else
		m_check_stop_train = FALSE;

	//////////////////////////////
	//add by cf after 7-18
	///////////////////////////////
	if(m_nSchdType == BASIC_SCHEDULE){
		m_Plan_arrival_date = OsConvertDatetoString(rec->arrival_date);
		m_Plan_depart_date = OsConvertDatetoString(rec->depart_date);
		m_Plan_arrival_time = OsConvertTimetoString(rec->arrival_time);
		m_Plan_depart_time = OsConvertTimetoString(rec->depart_time);
		m_Plan_arrival_date="";
		m_Plan_depart_date="";
		CWnd* pWnd=(CWnd*)GetDlgItem(IDC_EDIT11);
		if(pWnd!=NULL)
			pWnd->EnableWindow(FALSE);
		pWnd=(CWnd*)GetDlgItem(IDC_EDIT10);
		if(pWnd!=NULL)
			pWnd->EnableWindow(FALSE);
	}else{
		m_Plan_arrival_date = OsConvertDatetoString(rec->plan_arrival_date);
		m_Plan_depart_date = OsConvertDatetoString(rec->plan_depart_date);
		m_Plan_arrival_time = OsConvertTimetoString(rec->plan_arrival_time);
		m_Plan_depart_time = OsConvertTimetoString(rec->plan_depart_time);		
	}

	m_arrival_time = OsConvertTimetoString(rec->arrival_time);
	m_depart_time = OsConvertTimetoString(rec->depart_time);
    
 	
	
	CString tempString;
	int temp = GetEntryCollide(*rec);
	//错误
	if(IsStatusError(*rec))
	{
		m_Collide_reason.Empty();
		tempString ="错误:";

	    if(IsNoErrorReason(*rec))
			tempString +="未知错误原因";
		if(IsErrorStationNumber(*rec))      
            tempString +="车站号错误";
		if(IsErrorEntryNumber(*rec) )     
			tempString +="入口号错误";
		if(IsErrorExitNumber(*rec))   
			tempString +="出口号错误";
	    if(IsErrorSideNumber(*rec))  
			tempString +="股道号错误";
		if(IsErrorArriveDepartTime(*rec))
			tempString +="时间错误";
		if(IsErrorRunFlag(*rec))
			tempString +="列车运行标志错误";
		if(IsArriveTimeTooSmall(*rec))
			tempString +="到达时间大于出发时间";
		if(IsAbsRunTimeTooSmall(*rec))
			tempString +="区间运行时间过小";
		if(IsEntrySideNotConnect(*rec))
			tempString +="入口和股道不相连";
		if(IsSideExitNotConnect(*rec) )
			tempString +="出口和股道不相连";
		if(IsPrevAbsEntryNotConnect(*rec))
			tempString +="区间(前方站出口)和本站入口不相连";
		if(IsExitNextAbsNotConnect(*rec))
			tempString +="本站出口和区间(后方站入口)不相连";
		if(IsWrongSideArriveDirection(*rec))
			tempString +="选择的股道不能接入该方向列车";
		if(IsWrongSideDepartDirection(*rec))
			tempString +="选择的股道不能发出该方向列车";		
		if(IsWrongSideArriveTrainType(*rec))
			tempString +="选择的股道不能接入该等级的列车";
		if(IsWrongSideDepartTrainType(*rec))
			tempString +="选择的股道不能发出该等级的列车";
		if(IsWrongSideLength(*rec))
			tempString +="股道长度不能容纳该列车";
		if(IsWrongSideSpace(*rec))
			tempString +="超限车不能接入该股道";
		if(IsWrongSideWithouElec(*rec))
			tempString +="不能接入无接触网股道";
		if(IsWrongSideWithoutPlatform(*rec))
			tempString +="需在设置客货运设备的股道停车作业";
		if(IsWrongSideStartDifficult(*rec))
			tempString +="不能停入启动困难股道";
		if(IsStopAbsEntryError(*rec))
			tempString +="区间返回出入口不一致";
		if(IsMustStopTrain(*rec))
			tempString +="该列车在本站必须停车";
		if(IsCannotDepartEarly(*rec))
			tempString +="该列车在本站不得提前发车";
		if(IsSideUtilityConflict(*rec))
			tempString +="该列车在本站股道使用冲突";
		if(IsAbsRunTimeConflict(*rec))
			tempString +="该列车在区间运行时间冲突";
		m_Collide_reason = tempString;
	}
	//无错误，无冲突
	else if(IsNoCollide(*rec))
	{
		m_Collide_reason = _T("没有错误和冲突"); 
	}
	//无错误，有冲突
	else
	{
		m_Collide_reason.Empty();
		tempString ="冲突:";
		
		GetEntryCollideString(*rec,tempString);
		m_Collide_reason+= tempString;
		GetSideCollideString(*rec,tempString);
		m_Collide_reason += tempString;
		GetExitCollideString(*rec,tempString);
		m_Collide_reason += tempString;
		GetAbsCollideString(*rec,tempString);
		m_Collide_reason += tempString;
	}
	//end of cf 


	UpdateData(FALSE);

	this->InitIdentify();

	InitDlgPosition(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTrainPlanParaDlg::OnCheckEngine() 
{
	m_check_engine = m_ctrlEngineButton.GetCheck();
    if(m_check_engine)
		m_load_engine.EnableWindow (TRUE);
	else{
		m_load_engine.EnableWindow (FALSE);	
		m_load_engine.SetWindowText ("");
	}
}

void CTrainPlanParaDlg::OnCheckTrack() 
{
	m_check_track = m_ctrlTrackButton.GetCheck();
    if(m_check_track)
		m_combo_track.EnableWindow (TRUE);
	else
		m_combo_track.EnableWindow (FALSE);
    	
}

void CTrainPlanParaDlg::OnCheckAbsTime() 
{
  	m_check_abs_time = m_ctrlAbsTimeButton.GetCheck();
    if(m_check_abs_time)
		m_ctrlAbsTime.EnableWindow (TRUE);
	else
		m_ctrlAbsTime.EnableWindow (FALSE);
 	
}

//允许check
void CTrainPlanParaDlg::OnPermitOut() 
{
	////modify by cf after 8-24
  	m_check_permit_out = m_ctrlPermitOutButton.GetCheck();
	UpdateData(TRUE);
	//允许
    if(m_check_permit_out)
	{
		m_permit_out_time.EnableWindow(TRUE);			
		m_permit_out_date.EnableWindow (TRUE);
	 
		long plan_depart_time = OsConvertStringtoTime((LPCTSTR)m_Plan_depart_time);
		
		long m_PermitoutDate = OsConvertStringtoDate((LPCTSTR)m_spermit_out_date);
		long m_PermitoutTime =OsConvertStringtoTime((LPCTSTR)m_spermit_out_time);

        if(plan_depart_time == (ushort)-1 )
		{
			AfxMessageBox("输入图定发时间格式不对!");
			return;
		}
		if(m_PermitoutTime == (ushort)-1 || m_PermitoutDate == (ushort)-1)
		{
           	AfxMessageBox("输入允许提前发车时间不对!");
			return;
		}
 
 
	}
	else
	{
		m_permit_out_time.EnableWindow (FALSE);		
		m_permit_out_date.EnableWindow (FALSE);
	 
	}
	UpdateData(FALSE);
}

void CTrainPlanParaDlg::OnOK() 
{
 
	UpdateData(TRUE);
	///////check time
	int ah,am,dh,dm;
	int index=m_Plan_arrival_time.Find(":");
	if(index==-1){
		AfxMessageBox("输入图定出发时间格式不对!");
		return;
	}
	ah=atoi((LPCSTR)m_Plan_arrival_time.Left(index));
	am=atoi((LPCSTR)m_Plan_arrival_time.Right(m_Plan_arrival_time.GetLength()-index-1));
	if(ah<0||ah>=24||am<0||am>=60){
		AfxMessageBox("输入图定出发时间格式不对!");
		return;
	}

	index=m_Plan_depart_time.Find(":");
	if(index==-1){
		AfxMessageBox("输入图定到达时间格式不对!");
		return;
	}
	dh=atoi((LPCSTR)m_Plan_depart_time.Left(index));
	dm=atoi((LPCSTR)m_Plan_depart_time.Right(m_Plan_depart_time.GetLength()-index-1));
	if(dh<0||dh>=24||dm<0||dm>=60){
		AfxMessageBox("输入图定到达时间格式不对!");
		return;
	}
	//////
	//////20070412
	///////////////////////////
	//更改计划到点，计划发点
    	///////check time
	int ah_p,am_p,dh_p,dm_p; ///p代表plan
	index=m_arrival_time.Find(":");
	if(index==-1){
		AfxMessageBox("输入计划出发时间格式不对!");
		return;
	}
	ah_p=atoi((LPCSTR)m_arrival_time.Left(index));
	am_p=atoi((LPCSTR)m_arrival_time.Right(m_arrival_time.GetLength()-index-1));
	if(ah_p<0||ah_p>=24||am_p<0||am_p>=60){
		AfxMessageBox("输入计划出发时间格式不对!");
		return;
	}

	index=m_depart_time.Find(":");
	if(index==-1){
		AfxMessageBox("输入计划到达时间格式不对!");
		return;
	}
	dh_p=atoi((LPCSTR)m_depart_time.Left(index));
	dm_p=atoi((LPCSTR)m_depart_time.Right(m_depart_time.GetLength()-index-1));
	if(dh_p<0||dh_p>=24||dm_p<0||dm_p>=60){
		AfxMessageBox("输入计划到达时间格式不对!");
		return;
	}

	///////////////////////////
	returnOK = TRUE;
	char *buffer; 
	if(m_nSchdType == BASIC_SCHEDULE){
		rec->arrival_time  = OsConvertStringtoTime((LPCTSTR)m_Plan_arrival_time);
 		if(rec->arrival_time == (ushort)-1)
		{
			AfxMessageBox("输入图定到时间格式不对!");
			return;
		}
		rec->depart_time	= OsConvertStringtoTime((LPCTSTR)m_Plan_depart_time);
 		if(rec->depart_time == (ushort)-1)
		{
			AfxMessageBox("输入图定发时间格式不对!");
			return;
		}
	}else{
    rec->plan_arrival_date  = OsConvertStringtoDate((LPCTSTR)m_Plan_arrival_date);
 	if(rec->plan_arrival_date == (ushort)-1)
	{
		AfxMessageBox("输入图定到日期格式不对!");
		return;
	}
	rec->plan_depart_date	= OsConvertStringtoDate((LPCTSTR)m_Plan_depart_date);
 	if(rec->plan_depart_date == (ushort)-1)
	{
		AfxMessageBox("输入图定发日期格式不对!");
		return;
	}
    rec->plan_arrival_time  = OsConvertStringtoTime((LPCTSTR)m_Plan_arrival_time);
 	if(rec->plan_arrival_time == (ushort)-1)
	{
		AfxMessageBox("输入图定到时间格式不对!");
		return;
	}
    rec->plan_depart_time	= OsConvertStringtoTime((LPCTSTR)m_Plan_depart_time);
 	if(rec->plan_depart_time == (ushort)-1)
	{
		AfxMessageBox("输入图定发时间格式不对!");
		return;
	}
    
		///////////////////////////
		////新增加的关于修改计划，到发时间20070412
        rec->arrival_time  = OsConvertStringtoTime((LPCTSTR)m_arrival_time);
		if(rec->arrival_time == (ushort)-1)
		{
			AfxMessageBox("输入计划到时间格式不对!");
			return;
		}
		rec->depart_time	= OsConvertStringtoTime((LPCTSTR)m_depart_time);
		if(rec->depart_time == (ushort)-1)
		{
			AfxMessageBox("输入计划发时间格式不对!");
			return;
		}
		///////////////////////////

	long m_PermitoutDate = OsConvertStringtoDate((LPCTSTR)m_spermit_out_date);
	long m_PermitoutTime = OsConvertStringtoTime((LPCTSTR)m_spermit_out_time);
	// m_PermitoutTime含义：0x8000，表示不限定发车；否则表示列车不得在这个时间前( rec->plan_depart_time+early_depart_time)发车
    if(m_ctrlPermitOutButton.GetCheck())
	{
		if(m_PermitoutTime == (ushort)-1 || m_PermitoutDate == (ushort)-1)
		{
			AfxMessageBox("输入允许提前发车时间格式不对!");
			return;
		}	
	  long t_plan =  long_time( rec->plan_depart_date,rec->plan_depart_time);
	  long t_tempTime = long_time( m_PermitoutDate,m_PermitoutTime);
	  //消除时间钟的秒数，变为整分钟
	  t_plan = t_plan/60*60;
	  t_tempTime = t_tempTime/60*60;
	  if ((t_tempTime -  t_plan)/60 > 0x7FFF)
	  {
		  AfxMessageBox("输入允许提前发车时间过晚，设为不限制提前发车");
		  rec->early_depart_time=0x8000;
	  }
	  else
	  {
		  rec->early_depart_time = (t_tempTime -  t_plan)/60; 
	  }
	}
    else
	{
       rec->early_depart_time = 0x8000;
	}
	}

	int nCurSel = 0;
	nCurSel = m_combo_track.GetCurSel();//add by cf after 8-24
	m_nTrackNo = m_combo_track.GetItemData(nCurSel);//add by cf after 8-24

    m_load_engine.GetWindowText(m_sLoadEngine);

	//next abs
    nCurSel = m_combo_next_abs.GetCurSel();
	int nNextAbsLine;
	if(nCurSel != -1 )
	{
		nNextAbsLine = abs_line[nCurSel];
		m_nCurrExit = ConvertAbsnumStationToEntry(rec->station,nNextAbsLine);
	}
	else
	{
		nNextAbsLine = -1;
		m_nCurrExit = ushort(m_exit);
	}

  	m_check_abs_time = m_ctrlAbsTimeButton.GetCheck();
    if(m_check_abs_time)
        m_ctrlAbsTime.GetWindowText (m_sAbsTime);
	else
		m_sAbsTime = originalAbsTime;

	Pstruct TRAINRECORD *next_rec = m_pTrain->GetIndexRecord(m_pTrain->m_nFocusRecordIndex + 1);
	if(next_rec != NULL)
	{
		if(nNextAbsLine == -1)
		{
			m_nNextEntry = next_rec->entry;
		}
		else
		{
			m_nNextEntry = ConvertAbsnumStationToEntry(next_rec->station,nNextAbsLine);
		}
	}
    
//previous abs
    nCurSel = m_combo_previous_abs.GetCurSel();
	int nPreviousAbsLine;
	if(nCurSel != -1 )
	{
		nPreviousAbsLine = abs_line_previous[nCurSel];
		m_nCurrEntry = ConvertAbsnumStationToEntry(rec->station,nPreviousAbsLine);
	}
	else
	{
		nPreviousAbsLine = -1;
		m_nCurrEntry = ushort(m_entry);
	}

	Pstruct TRAINRECORD *previous_rec = m_pTrain->GetIndexRecord(m_pTrain->m_nFocusRecordIndex -1);
	if(previous_rec != NULL)
	{
		if(nPreviousAbsLine == -1)
		{
			m_nPrevExit = previous_rec->exit;
		}
		else
		{
			m_nPrevExit = ConvertAbsnumStationToEntry(previous_rec->station,nPreviousAbsLine);
		}
	}

	//buffer = m_stop_time_min.GetBuffer(12);
	if(OsConvertStringtoTime(m_stop_time_min) < 0)
	{
		AfxMessageBox("请输入一个时间在最小停车时间");
		GetDlgItem(IDC_MIN_STOP_TIME)->SetFocus();
		//m_stop_time_min.ReleaseBuffer();
		return;
	}
//	m_stop_time_min.ReleaseBuffer();

	//buffer = m_stop_time.GetBuffer(12);
	long tempTime = OsConvertStringtoTime(m_stop_time);//modi by cf after 9-4
	if(tempTime < 0)
	{
		AfxMessageBox("请输入一个时间在停车时间");
		//m_stop_time.ReleaseBuffer();
		GetDlgItem(IDC_STOP_TIME)->SetFocus();
		return;
	}
	if((orignStopTime == 0 ) && (tempTime > 0))
	{
		sameAsPasstoStay = TRUE;
	}

	if((orignStopTime > 0) && (tempTime == 0))
	{
		sameAsPasstoStay = FALSE;
	}
	//m_stop_time.ReleaseBuffer();

	
 /*
	if(m_ctrlPermitOutButton.GetCheck())
	{
		m_check_limit = m_limit.GetCheck();//add by cf after 8-24

	}
	*/
	if(originalAbsTime.Compare(m_sAbsTime) != 0)
	{
		isSendBehind = TRUE;
	}

	if(originalStopTime.Compare(m_stop_time) != 0)
	{
		isSendBehind = TRUE;
	}

	

	CDialog::OnOK();
}


//add by cf after 7-18
BOOL CTrainPlanParaDlg::IsSendBehind()
{
	return isSendBehind;
}

//add by cf after 7-18
void CTrainPlanParaDlg::OnKillfocusStopTime() 
{
	// TODO: Add your control notification handler code here
	if(returnOK)
	{
		returnOK = FALSE;
		return;
	}
	char *buffer;
	long temp;
	CString temp_stop_time = m_stop_time;
	UpdateData();
	//buffer = m_stop_time.GetBuffer(12);
	if((temp = OsConvertStringtoTime(m_stop_time)) < 0)
	{
		AfxMessageBox("请输入一个时间在停车时间");
		//m_stop_time.ReleaseBuffer();
		m_stop_time = temp_stop_time;
		UpdateData(FALSE);
		GetDlgItem(IDC_STOP_TIME)->SetFocus();
		returnOK = FALSE;
		return;
	}
	//m_stop_time.ReleaseBuffer();
    long tempTime = rec->arrival_time + temp;
	m_depart_time = OsConvertTimetoString(tempTime);
	UpdateData(FALSE);
}
 

BOOL CTrainPlanParaDlg::IsSameAsPassToStay()
{
	return sameAsPasstoStay;

}

void CTrainPlanParaDlg::InitIdentify()
{
 
	if(!gpcdef.GetOperatorRight(WINTG_EDIT)||
		((m_nSchdType == HISTORY_SCHEDULE) && m_pDoc->m_bHistUnChange)
		|| !(m_pDoc->GetOperatorPriorByDistrict(m_pDoc->nCurrentMouseX,m_pDoc->focus_point)) )
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_STOP_TIME);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_STOP_TIME_MIN);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_LOAD_ENGINE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHECK_ENGINE);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_COMBO_TRACK);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_CHECK_TRACK);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_ABS_TIME);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_CHECK_ABS_TIME);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_COMBO_NEXTABS);
		pWnd->EnableWindow(FALSE);
	

		pWnd = GetDlgItem(IDC_PERMIT_OUT_TIME);
		pWnd->EnableWindow(FALSE);
		
	 
		
		pWnd = GetDlgItem(IDC_PERMIT_OUT);
		pWnd->EnableWindow(FALSE);

		

		pWnd = GetDlgItem(IDC_ADJUST_START);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_NO_STOP_TRAIN);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_PASS_SIDE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);
	}
}

void CTrainPlanParaDlg::OnSelchangeComboNextabs() 
{
	// TODO: Add your control notification handler code here
/*
	CString str;
	m_combo_next_abs.GetLBText(m_combo_next_abs.GetCurSel(),str);
	DWORD k= m_combo_next_abs.GetItemData(m_combo_next_abs.GetCurSel());
	TRACE(str);
	TRACE("%d\n",k);
	str.Format("%d",k);
	AfxMessageBox(str);
	*/
}

HBRUSH CTrainPlanParaDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if( GetDlgItem(IDC_PERMIT_OUT_DATE)->m_hWnd==pWnd->m_hWnd 
		|| GetDlgItem(IDC_PERMIT_OUT_TIME)->m_hWnd ==pWnd->m_hWnd )
	{     
		pDC->SetBkColor(CLR_LIGHTRED);
		//pDC->SetTextColor(RGB(0, 255, 0));
		return (HBRUSH) m_Brush;
	}  
	else if(GetDlgItem(IDC_COMBO_TRACK)->m_hWnd==pWnd->m_hWnd )
	{
		 pDC->SetBkColor(CLR_LIGHTGREEN);
	}
	
 
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
