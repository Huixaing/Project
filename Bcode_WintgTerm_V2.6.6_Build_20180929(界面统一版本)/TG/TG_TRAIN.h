//
#ifndef TG_TRAIN_H_FILE
#define TG_TRAIN_H_FILE

#include "BaseTrain.h"

#include "vector"
#include "map"

#define MINIMUMTIME	120
#define WHOLE_DAY_MINUTE	1440
#define WHOLE_DAY_SECOND	86400
#define NOTIME		0xffffffff
#define UP_MARK		1
#define DOWN_MARK	2


//adjust status
#define IsActualArrivalTime(flag)           ((flag & 0x0001) == 1)
#define IsActualDepartTime(flag)            ((flag & 0x0002) == 2)

#define SetActualArrivalTime(flag)           (flag = (flag | 0x0001) )
#define SetActualDepartTime(flag)            (flag = (flag | 0x0002) )
#define CancelActualArrivalTime(flag)           (flag = (flag & 0xFFFFfffe) )
#define CancelActualDepartTime(flag)            (flag = (flag & 0xFFFFfffd) )

#define CancelFlagStopSignal(flag)          (flag &= 0xFFFFff7f)
#define CancelFlagStopAbs(flag)             (flag &= 0xFFFFfeff)

#define CancelFlagRescue(flag)          (flag &= 0xFFFFF7FF)
#define CancelFlagRescued(flag)         (flag &= 0xFFFFefff)

#define CancelFlagUniteTrain(flag)          (flag &= 0xFFFFbfff)
#define CancelFlagTrainUnited(flag)         (flag &= 0xFFFF7fff)



//infer traintype
#define IsTrain(train_info)            (!(((train_info.type)>>7) &0x01))
#define IsTruck(train_info)            (((train_info.type)>>7) &0x01)
#define IsCrhTrain(train_info)         (((train_info.type)>=17 && (train_info.type)<=19) || (train_info.run_type)==5)

//add by cf after 11-7
#define IsRunFast(rec)                 (!(((rec->run_fast_slow_time)>>7) & 0x01))
#define GetFastMinute(rec)             ((rec->run_fast_slow_time & 0x7F))



#define   TRAIN_ARRIVE              1
#define   TRAIN_DEPART              2


//从区间开向车站


#define IsAbsDrawUp(stop_reason)                  (stop_reason & 0x04) 
#define IsAbsDrawBottom(stop_reason)              (stop_reason & 0x08)

#define SetAbsDrawUp(stop_reason)                 (stop_reason |= 0x04) 
#define SetAbsDrawBottom(stop_reason)             (stop_reason |= 0x08) 

#define ReSetAbsDraw(stop_reason)                 (stop_reason &= ~0x0C) 

#define SetDisplyTractionBottom(adjust_flag)  (adjust_flag |= 0x100)
#define SetDisplyTractionMiddle(adjust_flag)  (adjust_flag |= 0x200)
#define SetDisplyTractionTop(adjust_flag)     (adjust_flag |= 0x400)
#define SetDisplyTractionNone(adjust_flag)    (adjust_flag &= 0xfffff8ff)

#define IsDisplyTractionTop(adjust_flag)      (((adjust_flag)&0x700) == 0x400)
#define IsDisplyTractionMiddle(adjust_flag)   (((adjust_flag)&0x700) == 0x200)
#define IsDisplyTractionBottom(adjust_flag)   (((adjust_flag)&0x700) == 0x100)
#define IsDisplyTractionNone(adjust_flag)     (((adjust_flag)&0x700) == 0x000)

// 助调能够操作的运行线---区间作业车
#define SetTrainFlag(adjust_flag)    (adjust_flag |= 0x800)
#define IsSetTrainFlag(adjust_flag)  (((adjust_flag)&0x800) == 0x800)

#define SetAbsPartRunTrain(adjust_flag)  (adjust_flag |= 0x1000)
#define IsAbsPartRunTrain(adjust_flag)	  ((adjust_flag&0x1000)==0x1000)

#define ReSetAbsStartStop(adjust_flag)    (adjust_flag &= ~0x6000)
#define SetAbsStartStopUp(adjust_flag)	  (adjust_flag &= ~0x6000, adjust_flag |= 0x2000)
#define SetAbsStartStopDown(adjust_flag)  (adjust_flag &= ~0x6000, adjust_flag |= 0x4000)
#define IsAbsStartStopUp(adjust_flag)	  ((adjust_flag&0x6000)==0x2000)
#define IsAbsStartStopDown(adjust_flag)	  ((adjust_flag&0x6000)==0x4000)

#define SetAbsAttainEngineReturn(adjust_flag)  (adjust_flag &= ~0x8000, adjust_flag |= 0x8000)
#define IsAbsAttainEngineReturn(adjust_flag)	  ((adjust_flag&0x8000)==0x8000)

//#define SetTrainUpdateByOtherGPC(train) ( ((train)->dwFlag) |= 0x1)
#define SetTrainDeleteByOtherGPC(train) ( ((train)->dwFlag) |= 0x2)
#define SetTrainShow(train)             ( ((train)->dwFlag) |= 0x4)
//#define IsTrainUpdateByOtherGPC(train)  ( (((train)->dwFlag)&0x1) == 0x1)
#define IsTraintDeleteByOtherGPC(train) ( (((train)->dwFlag)&0x2) == 0x2)
#define IsTrainShow(train)				( (((train)->dwFlag)&0x4) == 0x4)


#define SetAbsStopTime(abs_stop_time)  ((abs_stop_time)|0x8000)

#define TG_IsSpaceRank(rank)               ((rank) & 0x01) //超限标志---超限等级保存在bit16-bit21
#define TG_IsSpecialGaurdRank(rank)   ((rank) & 0x02)
#define TG_IsGoustRank(rank)               ((rank) & 0x04)
#define TG_IsMilitaryRank(rank)            ((rank) & 0x08)
#define TG_IsSpace(rank)							( ((rank) & 0x3F0000)>>16)

#define TG_GetArriveSpace(rank)              ( ((rank) & 0x070000)>>16) // 接车超限等级值
#define TG_GetDepartSpace(rank)            ( ((rank) & 0x380000)>>19)  // 发车超限等级值

// bit 4: 1-- 直接从日班计划加车 0-- 其他
using namespace std;
struct ABS_STOP
{
	long flag;
	long position;
	long start_time;
	long stop_time;
	ABS_STOP()
	{
		flag = 0;
		position = 0;
		stop_time = 0;
		start_time = 0;
	}

	bool IsAbsStopCoreSame(const ABS_STOP& rhs);
};

class TgTRAIN_RECORD
{
public:
	int        rec_index; 
	
	int        opmsGiveStationText; // 调度员给的值 0 OPMS给的值 1
	CString    szOpmsStationText;
	CString    szStationText;
	
	CString    szAbsText;
	CString    szAlarmText;
	CString    szPlanNote;
    	
	CString    arrive_train_id;
	CString    original_arrive_train_id;
	CString    depart_train_id;
	CString    original_depart_train_id;

    DWORD      special_train_index;
	
	STATION_NO station;      
	ENTRY_ID   entry;       
	ENTRY_ID   exit;       

	BYTE    arrive_side;         
	BYTE    plan_arrive_side;    
	BYTE    depart_side;       
	BYTE    plan_depart_side;   

	long	arrive; 
	long	depart;
	
	long	plan_arrive;
	long	plan_depart;
	
	long	route_flag;
	long    stop_reason;
							
	long    late_reason;
						// bit 0 是否统计到达点
						// bit 1 是否统计出发点
	long    early_depart_time;   
	long    run_fast_slow_time;  

	long    speed_limit;
						// 0 无限速, else: 最大限速值
	long    stop_condition;
	long    adjust_status;  
						// bit 0:  0 : 到达时间为计划
						//         1 : 到达时间为实际
						// bit 1:  0 : 出发时间为计划
						//         1 : 出发时间为实际
						// bit 2:  0 : 正常
						//         1 : 存在错误信息
						// bit 3:  0 : 调整成功
						//         1 : 调整失败
						// bit 4:  1 : 技术停车
						// bit 5:  1 : 
						// bit 6:  0 : arrivetime is same with the nextcon's arr
						//         1 : not same(only for inter_sta)
						// bit 7:  0 : departtime is same with the nextcon's dep
						//         1 : not same(only for inter_sta)
						// bit 8 :   到达时间锁
						// bit 9 :   出发时间锁
						// bit 10:  0  实际到达时间为人工报点
						//          1  实际到达时间为自动报点
						// bit 11:  0  实际出发时间为人工报点
						//          1  实际出发时间为自动报点
						// bit 12:  
						//          
						// bit 13,14:  0  未发送到邻台（台间站）
						//                 1  邻台拒绝
						//                 2  邻台接受
						//       
						// bit 15:  0  无电力牵引
						//          1  电力牵引
						// bit 16: 1:分界口车站
	                    // bit 17: 1:台间站
	                    // bit 18: 是否参与进路触发调整

	short	min_stop_time;
	short   spec_run_time; // side_utility        
	long     super_interval_ltime;  
	short   stop_abs_position;// positon --- 区间折返 
	WORD    stop_abs_s_time;//special_time1 
	WORD    stop_abs_e_time;//special_time2

	long    data_source;
	long    flag;
	long    no_meet_reason;
     
	int     modify_abs_runtime;
	int   in_station_index;
	int   out_station_index;
	int     rank;
						// bit 0:  超限---0：不超限 1：超限 超限等级保存在bit16-bit21
						// bit 1:  重点警卫
						// bit 2:  毒品
						// bit 3:  军运
					    // bit 0-bit15该字段和TMIS对应, 不可扩充使用

						// bit16-bit18: 到达超限 0:正常 1:超级超限  2:一级超限 3:二级超限 4:三级超限
						// bit19-bit21: 出发超限 0:正常 1:超级超限  2:一级超限 3:二级超限 4:三级超限
	DWORD   display_status;
	short    in_sta_direct;
	short    out_sta_direct;
	BYTE    static_arrive;
	BYTE    static_depart;
	BYTE    static_manual; // 人工设置统计

	CString str_arrive_side;
	CString str_depart_side;
	
	vector<ABS_STOP> abs_stop;
	bool    bShowSecond;   // 是否显示秒
	WORD    line_type;
	BYTE    factor;
	DWORD   schd_color, real_color, schd_vert_color, real_vert_color;
	BYTE    schd_width, real_width;
	BYTE    nEngineCnt;
public:
	TgTRAIN_RECORD();
	TgTRAIN_RECORD(const TgTRAIN_RECORD& rhs);
	TgTRAIN_RECORD& operator=(const TgTRAIN_RECORD& rhs);
	~TgTRAIN_RECORD();

public:
	bool  IfInTimeScope(TIME stime, TIME etime) const;
	bool  IsRecordCoreSame(const TgTRAIN_RECORD& rhs);
	CString GetStaAlarmText();
	void  InitRecordBasic();
	CString toXML();
	
	BYTE GetArriveSpace() const;
	BYTE GetDepartSpace() const ;
public:
	void Clear();
	
private:
	void Copy(const TgTRAIN_RECORD& rhs);
};

class TgTRAIN_MORE_INFO
{
public: 
	// 基础信息
	CString  train_name;  //车次号组合

	TRAIN_KEY   train_key;
	TRAIN_INDEX basic_train_index;    
    CString shift_index_string;
	BYTE     type;   
	BYTE     run_type;

	long start_actual_time; 
	long start_plan_time;

	long sborder_plan_time;  /* converted from hh:mm:ss */
	long sborder_actual_time;  /* converted from hh:mm:ss */

	long eborder_plan_time;  /* converted from hh:mm:ss */
	long eborder_actual_time;  /* converted from hh:mm:ss */

	STATION_NO sborder_sta;
	STATION_NO eborder_sta;
	CString start_sta_name;
	CString end_sta_name;

	CONID from_conid;
	CONID to_conid;

	BYTE   priority;
	long   train_length;
	long   train_weight;
	long   status;  
			// 0x08: 秘密列车
			// 0x10: 日班计划列车
	        // 0x20: 强制正点

	// 特殊列车信息
	BYTE    space_rank;    
	BYTE    length_rank; 
	BYTE    super_rank;  
	BYTE    military_rank;  

	// 列车运行信息
	long     engine_utility;    
	long     side_utility;  
	long     no_meet_flag;
	


	// 自动调整信息
	short  adjust_begin_sta;
	short  adjust_end_sta;        
	long   adjust_flag;  
							// bit 0: 0 - 参于自动调整   1 - 不参于自动调整 
							// bit 1: 
							// bit 2: 0 - generate time_link    1 - not
							// bit 3: 0 - not in save before adjust   1 - do no use
							// bit 7-6:  0 - 分界口接入时间未设置
							//           1 - 分界口接入时间手工未设置
							//           2 - 分界口接入时间由系统自动设置
							// bit 10-8:  机车号显示位置  0-不显示   1-下 2-中  4-上
							// bit 11:   助调能够操纵的列车标志---区间作业车
	                        // bit 12:   区间分部运行 1:是  0：否
							// bit 13-14: 区间始发列车 停上区间 01 下区间 10
							// bit 15: 补机途中折返 1：是 0:否
	                        // bit 16-31: 区间始发列车 区间运行时分
	
	long   train_source;
		
	// add your code here
	BYTE whole_static;
	WORD stop_abs_time; 
	bool b_elec_rank;
	bool b_self_update;
public:
	TgTRAIN_MORE_INFO();
	TgTRAIN_MORE_INFO(const TgTRAIN_MORE_INFO& rhs);
	TgTRAIN_MORE_INFO& operator=(const TgTRAIN_MORE_INFO& rhs);
	~TgTRAIN_MORE_INFO();

public:
	bool  IsInfoCoreSame(const TgTRAIN_MORE_INFO& rhs);
	CString toXML();

public:
	void Clear();
private:
	void Copy(const TgTRAIN_MORE_INFO& rhs);
};

struct TgTRAIN_ENGINE_INFO
{
	CString     engine_id;            // 机车号
	CString     engine_name;          // 机车名称
	long        engine_status;        // bit 7-6:  from dispatcher/subao
	// bit 5-4:  0 - traction  1 - attach
	short       attach_sta;           // index of trainrecord
	short       unattach_sta;

public:
	TgTRAIN_ENGINE_INFO();
	TgTRAIN_ENGINE_INFO(const TgTRAIN_ENGINE_INFO& rhs);
	TgTRAIN_ENGINE_INFO& operator=(const TgTRAIN_ENGINE_INFO& rhs);
	~TgTRAIN_ENGINE_INFO();

public:
	bool IsEngineCoreSame(const TgTRAIN_ENGINE_INFO& rhs);
	void Copy(const TgTRAIN_ENGINE_INFO& rhs);
	void Clear();
	
public:
	CString toXML();

};

class CTG_TRAIN: public CBaseTrain
{
public:
	CTG_TRAIN();
	~CTG_TRAIN();
	CTG_TRAIN(const CTG_TRAIN& rhs);
	CTG_TRAIN& operator=(const CTG_TRAIN& rhs);
public:
	bool  IfInTimeScope(TIME stime, TIME etime) const;
	bool  IsTrainIDChange(const CTG_TRAIN& rhs);
	bool  IsTrainCoreSame(const CTG_TRAIN& rhs);
	ULONG GetTrainIndex();
	CString toXML(DWORD flag, WORD station, WORD rec_index, CString trainid, BYTE nRunType);
	
	bool GetLogicInfo(CString xml);
    CString toString();
	CString toPassway();
	CString toXMLForExitEntry();
	void AddPerPointInfo(WORD nRecIndex, WORD nFlag, const CPoint& point, int nSection);
	bool IsExistActualTime();
public:
	TRAIN_INDEX m_nTrainIndex;
	TRAIN_INDEX prev_train_index;
	TgTRAIN_MORE_INFO myTrainInfo;
	vector<TgTRAIN_RECORD> myRecord;
	vector<TgTRAIN_ENGINE_INFO> myEngine;

public:
	int AddRecord(TgTRAIN_RECORD& curr, int nType);
	TgTRAIN_RECORD* GetLastRecord();
	TgTRAIN_RECORD* GetFirstRecord();
	TgTRAIN_RECORD* GetRecordByPosition(int pos);
	TgTRAIN_RECORD* GetRecordByRecIndex(int uIndex);
	TgTRAIN_RECORD* GetStationRecord(int nStationNo);
	
	TgTRAIN_RECORD* GetPrevStationRecord(const TgTRAIN_RECORD* currec);
	TgTRAIN_RECORD* GetNextStationRecord(const TgTRAIN_RECORD* currec);

public:
	void ClearEngine();
	void AppendTailEngine(const TgTRAIN_ENGINE_INFO& eng);
	
	void DeleteHeadRecord();
	void DeleteTailRecord();
	void SetTrainType();
	void SetEleLocoTrain();
	int  GetTrainType(){ return m_nTrainType;}
	bool IsCHRTrain();
public:	
	bool bChangeTrainID;
	
	DWORD mOtherTrainIndex;
	CString GetTrainId();
	CString GetTractionIds();

	UINT GetRecordNum() const;
	UINT GetIndexOfRecord(int nStationNo);
	
	int  OffsetWorkScheduleTime(int nTimeOffset);
	int  OffsetWorkScheduleTime(int nRecordNo, int nTimeOffset, int nFlag);
	int  OffsetBasicScheduleTime(int nRecordNo,int nTimeOffset,bool bMoveWhole,int nFlag); // nFlag: 0 到点变 1 发点变 2 到发点变
	void UnionRect(CRect &rect) 
	{ 
		if(!rect.IsRectEmpty())
			m_train_rect.UnionRect(&m_train_rect, &rect);
	}
	void EmptyRect() { m_train_rect.SetRectEmpty();}
	CRect GetTrainRect(){ return m_train_rect;}
public:
	DWORD dwFlag;		// bit0: 该列车已经被其它GPC更新 1:是 0:否
					    // bit1: 该列车已经被其它GPC删除 1:是 0:否
						// bit2: 该列车总是显示,用于自由画线，无法判定方向和客货时
	
	bool  mbDoubleDirect; // true: 双向 false: 单向
	DWORD dwLogicFlag;  // 逻辑标志
	                    // bit0: 1:不刷新界面

public:
	void InitBasicTM();
	bool IsFlash(int& times);
	bool IsWholeTrainSideLock();
private:
	CRect m_train_rect; // 列车所有文字矩形的合并矩形
	int m_flashTimes;
	bool bWholeTrainSideLock;
};

#endif