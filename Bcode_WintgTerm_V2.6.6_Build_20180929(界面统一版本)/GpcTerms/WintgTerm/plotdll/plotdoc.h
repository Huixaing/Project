#if !defined(AFX_PLOTDOC_H__D47480D2_F5BD_11D1_9D7A_0000C05319CE__INCLUDED_)
#define AFX_PLOTDOC_H__D47480D2_F5BD_11D1_9D7A_0000C05319CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PlotDoc.h : header file
//
#include "tgmsg.h"  ////////////////////////////
#include "wintgdoc.h"
#include "plotinform.h"
#include "gpcdef.h"
#include ".\SingleTrainCfg.h"

#include "TgDocument.h"
#include "tg_work_schdule_holder.h"
#include "tg_schedule_base.h"

#define IsAbsDrawUp(stop_reason)                  (stop_reason & 0x04) 
#define IsAbsDrawBottom(stop_reason)              (stop_reason & 0x08)

#define IsStatArrival(stop_reason)           (((stop_reason)&0x1) == 0x1)
#define IsStatDepart(stop_reason)            (((stop_reason)&0x2) == 0x2)
#define IsStatBoth(stop_reason)              (((stop_reason)&0x3) == 0x3)
#define IsNotStat(stop_reason)               (((stop_reason)&0x3) == 0x0)

#define SetStatArrival(stop_reason)          (stop_reason &= ~0x3, stop_reason |= 0x1)
#define SetStatDepart(stop_reason)           (stop_reason &= ~0x3, stop_reason |= 0x2)
#define SetStatBoth(stop_reason)             (stop_reason |= 0x3)

#define ResetStatArrival(stop_reason)        (stop_reason &= ~0x1)
#define ResetStatDepart(stop_reason)         (stop_reason &= ~0x2)
#define ResetStatBoth(stop_reason)           (stop_reason &= ~0x3)

#define IsRunFast_Plot(rec)                 (!(((rec->m_data.run_fast_slow_time)>>7) & 0x01))
#define GetFastMinute_Plot(rec)             ((rec->m_data.run_fast_slow_time & 0x7F))
#define IsTrain_Plot(train_info)            (!(((train_info->m_data.type)>>7) &0x01))
#define IsTruck_Plot(train_info)            (((train_info->m_data.type)>>7) &0x01)
#define IsTechStopTrain_Plot(rec)				(((rec).m_data.adjust_status & 0x0010))
#define IsMakeDepartRouteByPlan_Plot(rec)      (((rec).m_data.stop_condition & 0x80) == 0x80)  //是否根据计划



#define IsFlagStopAbsUp(flag)       (flag & 0x2000)
#define IsFlagUnitOtherTrain(flag)  (flag & 0x4000)
#define IsFalgBeUnitedByOther(flag) (flag & 0x8000) 
#define IsFalgCombine(flag)		    ((flag & 0x0017) ==  0x0017)

#define IsFlagTrainWholeHelp(flag)  (flag & 0x01000000)
#define IsAbsAttainEngineReturn(adjust_flag)	  ((adjust_flag&0x8000)==0x8000)

//2004-02-11 JINAN
#define JINAN_MARGIN 50			//边界之间距离	
#define JINAN_STATION_WIDTH 100//车站的宽度
#define JINAN_PHASE_STORE_WIDTH 650//站存车的宽度
#define JINAN_MAIN_GRAPH_WIDTH 3010//运行图的宽度

#define JINAN_MAIN_GRAPH_HEIGHT 1092//运行图的宽度
#define JINAN_BUREAU_HEIGHT 200 //铁路局名称高度，从边缘到编组的顶端
#define JINAN_BIANZU_HEIGHT 300 //编组高度
#define JINAN_TIME_STR_HEIGHT 150 //时间轴的宽度，从上方编组的底部到运行图顶部

#define UP_MARK		1
#define DOWN_MARK	2

#define LEFT_MARK	3
#define RIGHT_MARK	4

#define SHIFT_FIRST 1
#define SHIFT_SECOND 0

#define  XOFFSETPRINT    180 
#define  YOFFSETPRINT     300
#define  YOFFSETPRINT_BASIC 180
#define  TRAIN_MARK_HEIGHT  15



/****************end of add **************************/
#define STATION_AXIS_WIDTH	200               //车站轴宽度
#define HEADER_TITLE_HEIGHT	100               //标题宽度
#define DRAW_MARK_HEIGHT	80
#define DISTANCE_TEXT_WIDTH	160
#define FOOTER_TITLE_HEIGHT	50               //脚注高度
     
#define TRAIN_INFORM_HEIGHT	350              

#define EXCEPTION_MARK 0
#define BAD_MARK       1
#define REVERSE_MARK   2
#define STOPREASTON_MARK 3

#define SINGLE_TRAIN_INFORM_WIDTH	900//970//1020     // 编组宽度        
#define SINGLE_TRAIN_HEIGHT          23      // 编组高度
#define TRAIN_COUNTS                 8         //列车编组单向个数

#define TRAIN_FORM_COLUMNS           8         //列数



#define STEP_OF_SIDES   8

const int max_czh_count=2048;

struct TRAIN_ADDITIONAL_INFO 
{                                                   
	TRAIN_INDEX m_nTrainIndexforCompare;
	int m_nNumberOfMarkedRect;
	RECT  m_nRect[64];
	int ptoftrainid_count;
	int m_nIncreaseLayer;//初始值为1
	struct PTofTrainID stPttoTrainid[8];
}; 

struct TGVirtualStation
{
	int perStationIndex;
	int nextStationIndex;
	int nStation;
	int nPercent;
	char strStatinName[64];
	
	TGVirtualStation()
	{
		perStationIndex=0;
		nextStationIndex=0;
		nStation=0;
		nPercent=0;
		memset(strStatinName, 0, sizeof(strStatinName));
	}
};

class CPlotDoc;

class CPlotAbsMark: public CObject
{
public:
	CPlotAbsMark();
	CPlotAbsMark(CPlotDoc *pDoc){ m_pDoc = pDoc;}
 	 	
protected :
	int uThickness;//general line width
	DECLARE_DYNCREATE(CPlotAbsMark)
public:
	CPlotDoc *m_pDoc;
	ulong m_nMarkIndex;
   	Pstruct _tagMarkItem rec;
    
 	CRect cRect,lRect,rRect,tRect,bRect,m_drawRect;

	BOOL bMoveCenter,bMoveRight,bMoveLeft,bMoveTop,bMoveBottom;
	BOOL m_bFocus;
	int m_nDrawFlag,m_nOldDrawFlag;
	COLORREF mColor,mOldColor;

	void RestoreDrawFlag();
 	void SetDrawFlag(int nDrawFlag,BOOL isChangeOldDrawFlag =  FALSE);
	void SetColor(COLORREF  newColor);
	void Draw(CDC *pCDC,int nPageWidth);
	void DrawBlock(CDC *pCDC,int nPageWidth);
	
	void DrawAbsInhitBlock(CDC *pCDC,int nPageWidth);
	void DrawAbsSlowBlock(CDC *pCDC,int nPageWidth);
	void DrawAbsSelfCloseBlock(CDC *pCDC,int nPageWidth);

	void DrawAbsEleFix(CDC *pCDC,int nPageWidth);
	void DrawAbsZhFix(CDC *pCDC,int nPageWidth);
	void DrawLanzhouAbsEleFix(CDC *pCDC,int nPageWidth);

	void DrawSationBlock(CDC *pCDC,int nPageWidth);

	void DrawNoteTextBlock(CDC *pCDC,int nPageWidth);
	void DrawNewNoteTextBlock(CDC *pCDC,int nPageWidth);

	void FrameRectExtend(CDC *pDC, CRect &rc);
	CString GetKiloMDesp();
};

class CMultiMark
{
public:
	CMultiMark();
	virtual ~ CMultiMark();
protected:
	CObArray mObArray;
public:
	void AddFigure(CPlotAbsMark *pFigure);
	virtual void DeleteContents( );
	CPlotAbsMark *GetFigure(int uIndex);
	int GetNumFigures( );

};

struct INHIBIT_TRAIN
{
	BYTE nTypeCount;
	WORD trainType[100];
};

struct STA_NODE
{
	int sta_no;
	int sta_sep;
	int show_side;
	int side_num;
	struct SIDE
	{
		int side_no;
		char side_name[10];
		int side_property;
	}SIDES[128];
	STA_NODE()
	{
		memset(this, 0, sizeof(STA_NODE));
	}
};

struct AREA_TYPE
{
	enum
	{
		NAME,
		TOP_TIME,
		BOTTOM_TIME,
		TRAIN,
		PHASE,
		NOTE,
		CAR,
		QB,
		TITLE,
		FOOT,
		QB_LEFT,
		QB_RIGHT,
	};
};

struct AreaDefine
{
	double x_offset;
	double y_offset;
	double height;
	double width;
};

struct PlotStytle
{
	int id;
	int left, right, top, bottom;
	int timedlt;
	AreaDefine name_area;
	AreaDefine train_area;
	AreaDefine phase_area;
	
	AreaDefine note_area;
	AreaDefine car_area;

	AreaDefine qb_area;
	AreaDefine qb_area_left;
	AreaDefine qb_area_right;

	int side_sep;
	int con_num;
	int con_id[64];
	int train_id;
};

struct BlockCfg
{
	BYTE draw;
	BYTE init;
	BYTE time_pos;
	COLORREF color1;
	COLORREF color2;
	COLORREF color3;
	BYTE dlt;

	BlockCfg()
	{
		draw = 1;
		init = 0;
		time_pos = 1;
		color1 = RGB(255,0,0);
		color2 = RGB(0,0,255);
		color3 = RGB(255,0,0);
		dlt=0;
	}
};

struct PrintFont
{
	BYTE time_char;
	BYTE train_id;
	BYTE engine_id;
	BYTE late_time;
	BYTE station_text;
	PrintFont()
	{
		time_char=8;
		train_id=8;
		engine_id=6;
		late_time=8;
		station_text=8;
	}
};

struct HeightSpace
{
	BYTE vertical_route_space;
	BYTE train_mark_height;
	BYTE step_of_side;
	HeightSpace()
	{
		vertical_route_space=20;
		train_mark_height=15;
		step_of_side=8;
	}
};

class CZH_Relation
{
public:
	CZH_Relation()
	{
		Init();
	}
	void Init()
	{
		memset(cCzh, 0, sizeof(cCzh));
		nTrainCount=0;
		for(int i=0; i<sizeof(Trains)/sizeof(Trains[0]); i++)
		{
			memset(Trains[i].strTrainId, 0, sizeof(Trains[i].strTrainId));
			Trains[i].nTrainIndex=0;
		}
	}

public:
	char cCzh[32];
	int  nTrainCount;
	struct Train_Info
	{
		char strTrainId[10];
		long nTrainIndex;
	}Trains[24];
};

class CPlotDoc : public CDocument
{
public:
	// protected constructor used by dynamic creation
	CPlotDoc();           
	DECLARE_DYNCREATE(CPlotDoc)

// Attributes
public:
	CMultiMark m_nAbsList;
	std::vector<CTG_SingleTrain> m_vecSchdTrains;
	TRAIN_ADDITIONAL_INFO train_additional_info[1024];

public:
	// 跳跃模式
	int m_nTrainLeapLineFlag;
	int m_nShowElecLocoFlag;

public:
	//显示虚拟车站
	bool m_bShowVirtualStationOnPlot;
	std::vector<TGVirtualStation> m_virtualStation;

	double fWidthMulti,fHeightMulti;
	CString parsecontent[19];

	void  DrawBadMark(CDC *pCDC, CPoint &ptlStart, CPoint &ptlEnd,int line_type,CString strTrainID);
	void parseNotes();
	void DrawMarkOfStart(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId,CPoint ptlStart,int nLength,int nDirect,COLORREF Color, bool bActual, int schd_type);
	void DrawMarkOfArrive(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId,CPoint ptlStart,int nLength,int nDirect,COLORREF Color, bool bActual, int schd_type, int rec_index);

	void DrawMarkOfDepart(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId,CPoint ptlStart,int nLength,int nDirect,COLORREF Color, bool bActual);

	void DrawMarkOfDepartFixedSize(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId, CPoint ptlStart, int nLength, int nDirect, COLORREF Color, bool bActual);
	void DrawMarkOfEnd(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId,CPoint ptlStart,int nLength,int nDirect,COLORREF Color,USHORT flag, bool bActual);
 
 	void DrawActualLine(CDC* pCDC, CPoint& ptlStart, CPoint& ptlEnd, int factor = 0,bool bAbsLine = FALSE,int nEngine=0,int nCarriageCount=0, bool bIsActual = TRUE);
	void DrawMarkOfCritical(CDC *pCDC,CTG_SingleTrain * single,CString strTrainId,CPoint ptlStart,int nLength,int nDirect,COLORREF Color);

 	void DrawTrainText(CDC * pCDC,CTG_SingleTrain * single,COLORREF color = NULL,int schd_type = WORK_SCHEDULE);
 	void DrawTrainEarlyLastTime(CDC * pCDC,CTG_SingleTrain * single,COLORREF color = NULL,bool isPrint = FALSE,int schd_type = WORK_SCHEDULE);

	void InitEarlyLastTime(CTG_SingleTrain* single);
	UINT GetTrainRecDirect(CTG_SingleTrain* single, int index);
    void DrawLanzhouMarkOfTractionEngine(CDC *pCDC, CPoint pBasic, CSize trainidSize, CTG_SingleTrain *single,int nDirect,int schd_type,int workthread);//added by zhaoliang for lanzou Engine attach_unattach style

	NOTEBOOK NOTES;
	NOTEBOOK NOTES2;//两班的三小时阶段数据，呼和需要
	long m_shiftid_Notes2; //notes2对应的班别

	int m_nActualPenWidth,m_nPenWidth;
	BYTE m_nLineWidthRate;
	std::vector<TRAIN_SUBAO_NEW> m_VectTrainsSubao;
   
	int m_nPartialHeight,m_nPartialWidth;// only include graphic area 

	int m_nSectionBreakHeight;
	int m_nMarkHeight;
	int m_nHeaderTitleHeight;
	PRINTINFORM *m_sPrintInform;
	PRINTINFORM  m_LocalSavedPrintInform;

	int m_nVerticalOffsetHeight;

	TERMINATEPOINT m_sTerminalPoint[8];
	int m_nTerminalPointNum;
    int m_nCurrentSectionNo;

	int station_axis_width,
		header_title_height,
		draw_mark_height,
		distance_text_width,
		footer_title_height,
		train_inform_height,
		time_axis_height,
		single_train_inform_width;
	int x_offset_print,
		y_offset_print,
		step_of_side,
		
		vertical_route_space,
		train_mark_height;
   
    int xMultiple,yMultiple;

    int m_nSubaoWidth[8];
	int nColumnWidth[8];

    int m_nShowTrainBasicFlag;   //打印基本图的时候可以选择客货车
 
	// 站号, 站存车内容
	std::map<int, CString> m_car_phase_0;
	std::map<int, CString> m_car_phase_6;
	std::map<int, CString> m_car_phase_12;
	std::map<int, CString> m_car_phase_18;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	//}}AFX_VIRTUAL

// Implementation
public:
	bool IsPrintPhaseCar(int con_id);
	int  ConvertTimetoXPos(long usTime,long usDate,int schd_type = WORK_SCHEDULE);
	void ConvertXPostoTime(int xPos,long *pusDate,long *pusTime);
	int  ConvertYPostoStationNo(int yPos,int &nRouteSetNo);
 	void DrawSingleTrain(CDC *pCDC,CTG_SingleTrain *single,int nDrawMode,int nStartNo = 0,int nPointNum = -1,BOOL isPrint = FALSE,int schd_type = WORK_SCHEDULE);
	void DrawSingleTrain_New(CDC *pCDC,CTG_SingleTrain *single,int nDrawMode,int nStartNo,int nPointNum,BOOL isPrint,int schd_type);

	void DrawSchdTrain(CDC *pCDC,int nFlag = 0);
	void DrawBackground(CDC *pDC,LONG lColor,ushort usStationNo,int usStartDate,int usStartTime,int usEndDate,int usEndTime, int nFlag);
	
	void HideStationString(CDC *pCDC);
		
	int Resize(int nDirect,int nFlag);

	virtual ~CPlotDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
public:
	void ShowPhaseStoreTrain(CDC *pCDC, COLORREF clr,int nStationNo,int phase,CString& stext);
	void DrawPhaseStoreTrainFrame(CDC *pCDC,COLORREF clr); ////车站阶段暂存车
	
	bool JudgeNeedWaveLine(CTG_TrainRecord *rec,int schd_type = WORK_SCHEDULE, int nDirectFlag = DOWN_DIRECT);
	
	void DrawActualStopStationLine(CDC* pCDC, CPoint& ptlStart, CPoint& ptlEnd,int factor,int dir);
 
	void DrawFastOrSlowTime(CDC *pCDC, CPoint &ptlStart, CPoint &ptlEnd,BOOL isFast,int minute,int dir = 2);
	int GetAbsVPos(CTG_SingleTrain *single,CTG_TrainRecord* rec,ushort position,int usFlag,ushort dir,int schd_type );
	int GetMinuteWidthToPlotStartTime(long nTime,long nDate,BOOL isPrint,int schd_type = WORK_SCHEDULE);
	int GetStationVPos(CTG_SingleTrain *single,CTG_TrainRecord* rec,int usSideFlag,int usFlag,int schd_type = WORK_SCHEDULE);

	int GetStationVPosByIndex(CTG_SingleTrain *single,CTG_TrainRecord* rec,int usSideFlag,int usFlag);
	virtual void GetGraphStartTime(ulong nShiftId,int schd_type);


	int single_train_height;
	
	void DrawFrameBorder(CDC *pCDC,COLORREF clrColor);
	
	void DrawTrainMark(CDC *pCDC);
	void DrawStopStationLine(CDC* pCDC, CPoint& ptlStart, CPoint& ptlEnd,int dir);

	int m_nPageWidth;
	int m_nPageHeight;
	double m_fPixelPerHour;
	int m_nViewTimeRange;
	int m_nShiftId;
	double m_fPixelPerMinute;
	int m_nSchdStatus;
	int m_EnvironInitFlag;
	CBrush m_brushBackground;
	int m_nCurrentRouteNum;
	int m_nAllRouteNum;

	int m_nDispDistanceNum;
	DISPDISTANCE m_sDispDistance[128];
	
	int m_nDisplayMode;
	int m_nViewFormat;
	int m_nHorizontalPos;
	int m_nVerticalPos;
	int m_nHistoryGraphWidth;
	long m_usCurrentTime;
	long m_usCurrentDate;
	long m_nShiftDate;
	int m_nShiftNo;
	
	int m_nSectionNum;
	int m_nCurrentSectionList [32];
	
	int m_nAllStationNum;
	int m_nAllStationList [256];
	
	int m_nStationNum;
	int m_nCurrentStationList [256];
	int m_nCurrentStationNo;
	int m_nFirstStationNo;
	
	
	double m_fPixelPerTenMinute;
	double m_fPixelPerTwoMinute ;
	double m_fMaxPixelPerMinute;

	struct _section_node_info   section_node_info;
	
	int m_nStationYPos [3 * MAX_STATION_PER_SECTION];
	double m_fPixelPerHalfHour;
	int m_usThreeHourMaxValue;
	int m_usOneHourMaxValue;
	int m_usHalfHourMaxValue;
	int m_usTenMinuteMaxValue;
	int m_usTwoMinuteMaxValue;
	int m_usMemoryPageWidth;
	
	int m_nShowDirectFlag;
	int m_nShowTrainFlag;
	

	BOOL m_bShowReverse;
	
	long m_usHistGraphStartTime;
	TGTIME m_sTGGraphStartTime,m_sTGBasicGraphStartTime;
	int m_nShiftType;
			
	int m_nOperatorStatus;
	int m_nMaxPageHeight,m_nMinPageHeight;
	BOOL m_bShowTractionEngine;
	vector<int>  fenjie_station_list;
	vector<int>  train_type_inhibit_array[MAX_SECTION_NUM];
	vector<int>  bianzhu_station_list;
	TGTIME m_sTGHistGraphStartTime;
	int m_nCfgUnknownTrainColor;
	CFont m_fontStation,m_fontTimeString;
	
	int m_nCfgUpAbsEleColor,m_nCfgDownAbsEleColor;
	int m_nTgErrorline;
	
	CTgDocument *m_pTgDoc;

	//manul
	
	int GetNodeIndex(CTG_SingleTrain * single,CTG_TrainRecord* rec,int& index,ushort usFlag = EXIT_POSITION);
	
	void DrawChordLine (CDC* pDC,CPoint pt1,CPoint pt2,int dir,int rate = 1);
	int GetEntryExitDirection (CTG_SingleTrain * single,CTG_TrainRecord* rec,ushort usFlag = EXIT_POSITION,int section=-1,int schd_type = WORK_SCHEDULE,short dFlag = -1);//yuzhu 20110529 处理西安局包西一台张桥打印问题
	int GetMinuteWidthToGraphStartTime (long nTime,long nDate,int schd_type = WORK_SCHEDULE);
	BOOL LoadTGProfile();
	
	int GetDispIndex(ushort nStationNo,ushort nRouteSetNo,ushort *rcCode = NULL);
	BOOL PtInRects(RECT rects, POINT pt);  
	BOOL IsElectricTrain( CTG_SingleTrain *single);
	void DrawMarkOfTractionEngine(CDC* pCDC,CPoint pBasic,CSize trainidSize,CTG_SingleTrain* single,int nDirect,int schd_type,int rec_index);
	int JudeTrainDirectByRecord(CTG_SingleTrain *single, CTG_TrainRecord *rec, int flag = ENTRY_POSITION);
	int GetModeOfStationShowSide(int DistanceIndex, int schd_type = WORK_SCHEDULE );
	BOOL JudeNodesConnected(ushort node_index1,ushort node_index2,int schd_type = WORK_SCHEDULE);
	int GetNodeIndexByAbs(int curNodeIndex, int abs_num,int dir,int schd_type);
	COLORREF  GetDefaultUnknownTrainColor()     const  { return  m_nCfgUnknownTrainColor;     }
	
	bool IsSameStation(CTG_SingleTrain * single,int rec_index,int station);
	int JudgeMultiNodeIndex(CTG_SingleTrain * single,CTG_TrainRecord* rec,int& index,ushort usFlag,int section,int shcd_type);
	int GetNodeSubSectionByIndex(int index,int schd_type=WORK_SCHEDULE);
	int GetNodeIndexBySubSection(int stationNo,int subsection,int schd_type);
	int JudgeMultiNodeIndexbySeq(CTG_SingleTrain * single,CTG_TrainRecord* rec,CList<int,int>& list,CTG_TrainRecord* _start_rec,CTG_TrainRecord* _end_rec,int shcd_type);
	
	int GetAdditionalTrainInfoIndex(TRAIN_INDEX trainindex);
	int GetIncreaseLayer(CTG_SingleTrain* single,CDC* pCDC, int schd_type=WORK_SCHEDULE);
	int GetIncreaseLayer(CTG_SingleTrain* single,CPoint ptlOrig,int nDirect,int statinno,CDC* pCDC, int schd_type=WORK_SCHEDULE);
	CSize GetNodesExtent(CTG_TrainRecord* rec1, CTG_TrainRecord* rec2,CTG_SingleTrain* train1,CTG_SingleTrain* train2,CDC* pCDC);
	//{{AFX_MSG(CPlotDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	INHIBIT_TRAIN m_InhibitStatTrain;
    int m_nDrawTrainConsiderStationSep; // 打印运行图是否考虑车站展开 0：默认值，不考虑 1：考虑
public:
	bool IsInhibitStatTrain(int con_id, int type);
	_node_info* GetNodeByIndex(int con_id, int node_index);
public:
	bool IsBlockAllSide(int stano, int sideNum);
	CString GetSideName(int stano, int sideno);
	void ParseMarkReasonText(const CString& text, CString& dcmdno, CString& reason);
	void TimeToDataAndMinute(TIME long_time, long *date, long *minute);

public:
	int GetLineType(int factor);
	COLORREF GetActualLineColor(int factor);
	COLORREF GetSchdColor(int factor);
	void LoadMarkCfgXML(CString pathname);

	void DrawOutScopeConnectLine(CDC* pCDC, CTG_SingleTrain* train, const CPoint& ptlStart, const int lColor, int nDirect, const CRect& rangeRect);
	void DrawTrainConnectLine(CDC* pCDC, CTG_SingleTrain* single, CTG_TrainRecord* first, CTG_TrainRecord* prec, const CPoint& plStart, const int lColor, BOOL isPrint, CString trainid);

	void DrawBasicTrainOutScopeConnectLine(CDC* pCDC, CTG_SingleTrain* single, CTG_TrainRecord* first, CTG_TrainRecord* prec, const CPoint& plStart, const int lColor, const CRect& rangeRect);
	void DrawBasicTrainConnectLine(CDC* pCDC, CTG_SingleTrain* single, CTG_TrainRecord* first, CTG_TrainRecord* prec, const CPoint& plStart, const int lColor, BOOL isPrint, CString trainid);

	void LoadTrainCZHRelationXML();
	void SetTrainBeforeAfterIndex();
	CTG_SingleTrain* GetTrain(TRAIN_INDEX index);

	bool IsCRHStopStation(int station);
	bool IsCRHReturnStation(int station, int index);
public:
	int      m_tgNodeNum;
	STA_NODE m_tgNode[128];
	BYTE m_hrbLateTimeMode;
	BOOL m_bStopStationWaveLine;
	int m_nShowTrainEarlyLateFlag;
	COLORREF manualRealColor;

	void ParsePlotParam(const CString& plot_param);
	PlotStytle mPlotStyle;
	std::map<ULONG, ULONG> m_noearlylatetime;
	void LoadPlotXML(CString fileName, int printType, int nShiftNo);
	
	int  GetEngineCount(CTG_SingleTrain *single, UINT rec_idx);
	bool GetOffsetAndSize(int type, int& x_offset, int& y_offset, int& height, int& width);
	void DrawMutiTrainSeparate(CDC*pCDC, CTG_SingleTrain* single, CTG_TrainRecord* first, const int lColor, const int lActualColor);

	int  m_nStaticMethod;
	bool m_bStaticsTimeCompleteByTGDef;
	bool m_bDrawBlackArc;
	BOOL m_bShowdistanceStone;
	BOOL m_bShowFastOrSlowTime;
	bool m_bShowCzhConnectLineOnPlot;
	bool m_bShowNotTechStopFlag;
	int m_nBasicPlotFlag;

	SingleTrainCfg m_lineCfg;

	BlockCfg m_SelfCloseMarkCfg;
	BlockCfg m_AbsInhibitMarkCfg;
	BlockCfg m_AbsSlowMarkCfg;
	BlockCfg m_AbsEleFixMarkCfg;
	BlockCfg m_AbsZhFixMarkCfg;

	BlockCfg m_SideInhibitMarkCfg;
	BlockCfg m_SideSlowMarkCfg;
	BlockCfg m_SideTianchuangMarkCfg;
	BlockCfg m_SideOccupyMarkCfg;

	PrintFont m_font;
	HeightSpace m_space;
	int m_nSubaoRowNum;

protected:
	int m_nCRHStopStation[5];// 动车段或动车所
	CPoint m_nCRHReturnStation[5];// 动车组折返车站
	
	int m_nCZCount;
	CZH_Relation m_sCZRelation[max_czh_count];

	int m_nDrawSidePoint;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINTGDOC_H__27E02CFA_7B29_11D1_9CEF_000021000CE6__INCLUDED_)
