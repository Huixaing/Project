#if !defined(AFX_WINTGDOC_H__27E02CFA_7B29_11D1_9CEF_000021000CE6__INCLUDED_)
#define AFX_WINTGDOC_H__27E02CFA_7B29_11D1_9CEF_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WintgDoc.h : header file
//
#include "WinDef.h"
#include "Tg_TRAIN.h"
#include "TG_Manage.h"
#include ".\chezuhaomanger.h"

#include "tg_message_define.h"
#include "TGCtrl.h"

#include "tgtype.h"
#include "stationDef.h"
#include "traintypecfg.h"

#include <vector>  

#define ERR_NOT_FIND -1
#define FIND_ONCE	1
#define FIND_TWICE 2


#define MINIMUM_UNIT	25
#define MAX_STATION_PER_SECTION         128

const int SCHEDULE_TRAIN_TYPE = 4;
const int MAX_SECTION_NUM = 64;
const int CTC_STATION = 1;
const int TDCS_STATION = 0;
const int NONE_SYSTEM_STATION = -1;
#define MAX_SIDES	255

//#define DRAW_TRAIN_RECT
#define X_TIME_OFFSET		0
#define ABS_DESP_MARK		1
#define REVERSE_MARK		2
#define MAX_ENGINEER_NUM	8

const DWORD STAY_TO_PASS		= 0x00000001;
const DWORD PASS_TO_STAY		= 0x00000002;
const DWORD TRAIN_ATTRIB_CHANGED= 0x00000004;
const DWORD FUGUA_ENGINE		= 0x00000008;

const DWORD CHANGE_TRAINID = 0x00000010;
const DWORD HAT_TRAINID    = 0x00000020;
const DWORD ZHEFAN_TRAINID = 0x00000040;
const DWORD MODIFY_SIDE    = 0x00000080;

const DWORD SET_ABS_TOP = 0x00000100;
const DWORD CONTINUE_DRAW_TRAIN = 0x00000200;
const DWORD MUTI_REC_STAY_TO_PASS = 0x00000400;
const DWORD DEPART_BY_PLAN = 0x00000800;

const DWORD DEPART_BY_MEET = 0x00001000;
const DWORD TRAIN_PLAN_PARA = 0x00002000;
const DWORD HELP_TRAIN = 0x00004000;
const DWORD TURN_REALITY = 0x00008000;

const DWORD MOUSE_TURN_REALITY = 0x00010000;
const DWORD MODIFY_TRAIN_TIME = 0x00020000;
const DWORD JIE_JIAOLUT_RELATION= 0x00040000;

#define VERTICAL_SPACE                  80 

// 画线状态
#define NORMAL_STATUS			0
#define INSERT_TRAIN			1
#define MODIFY_TRAIN			2

#define INSERT_TRAIN_GOON		4
#define INSERT_TRAIN_RETURN		5
#define INSERT_TRAIN_ABS_A		6     //前行到区间
#define INSERT_TRAIN_ABS_B		7    //从区间返回

#define RECEIVE_POINT_STATUS	8
#define WAIT_STATUS				9

// 画标记状态
#define  DRAW_MARK_ITEM_STATUS  20
#define  MODIFY_MARK  21
#define  MOVE_MUTI_TRAINS   22
#define  MUTI_TRAINS_SEL_INIT    23
#define  MUTI_TRAINS_SEL_ING    24

#define  ABS_INHIBIT_MARK   1//区间禁用
#define  ABS_FIX_MARK       2//区间综合天窗修
#define  ABS_SLOW_MARK      3//区间慢行
#define  SIDE_SLOW_MARK     4//股道慢行
#define  SIDE_INHIBIT_MARK  5//股道禁用

#define  GRAPH_NOTE_MARK    7//图标注解
#define  TEXT_NOTE_MARK     8//图标注解-子类型
#define  SELF_CLOSE_MARK    9//变换闭塞
#define  SIDE_TIANCHUANG_MARK     10//股道天窗

#define	 ABS_ELE_FIX_MARK	11	//多区间电网检修
#define  SIDE_OCCUPY_MARK   12  //股道占用
#define  SIDE_LOST_ELE_MARK   13  //股道停电

#define  NEW_GRAPH_NOTE_MARK   15//新图标注解
#define	 NEW_ABS_ELE_FIX_MARK	16//兰州电网检修

using namespace std ; 

struct STATISTIC_FLAG
{
	enum{
		LATE=0,
		EARLY=1,
		RIGHT=2,
	};
};
struct POS_FLAG
{
	enum{
		UP	 = 1,
		DOWN = 2,
	};
};

struct MASTERROUTE
{
	int nRouteSetNo;
	int nRouteNo;
	int nAllRouteNo;
};

struct CSideInfo 
{
  int side_number;
  bool fhighlow;
  char sidename[15];
  BYTE  nPlatform; // 站台 0:无 1:有 2:高
  short side_property;
  short direct; // 1:上行; 2:下行;
  CSideInfo()
  {
	  fhighlow=false;
	  nPlatform=0;
	  side_number=0;
	  side_property=21;
	  direct=0;
	  memset(sidename,0, sizeof(sidename));
  }
};

struct DispDistance
{
	int   sub_section;
	int   station_number;
	char  station_name[MAX_STA_NAME_LEN];
	CString distance;
	float deltadistance;
	
	BYTE number_of_side;
	CSideInfo side[MAX_SIDES];

	bool fHigh;
	
	WORD  up_line_count;
    WORD  up_line_list[LINE_MAX_CNT];

	WORD  down_line_count;
    WORD  down_line_list[LINE_MAX_CNT];

	BYTE  show_sation_sep;
	WORD  show_sides_mode ;  
	BYTE  inhibit_sametime_dir;
	BYTE  isNeedThick; 
	BYTE  electric_arms; // 0:不绘制(默认) 1:上 2:下 3:上下
    
	int   nStaProperty; // 车站属性 1: CTC车站 0: TDCS车站 -1: 无设备
	bool  bShowADTime;  // 到发点显示
    
	int   nStationYPos;
 	int   nSideYPos[MAX_SIDES];
	int   nSecondStationYPos;
	
	WORD  usStationStatus;
 	WORD  usLircStatus;
    BYTE  nRouteCtrlMode;
    BYTE  nStationCtrlMode;

	BYTE  nStpcPlanStatus;
	BYTE  nLircPlanStatus;

	DWORD clStpcFlag;
	DWORD clLircFlag;
	long  clrStationLine;

	int  entry;
	int  exit;
	int  m_nCzhPrintFlag;
	DispDistance()
	{
		sub_section = 0;
		station_number = 0;
		electric_arms = 0;
		number_of_side = 0;
		
		nStpcPlanStatus = 255;
		nLircPlanStatus = 255;
        
	    clStpcFlag=255;
	    clLircFlag=255; 
		clrStationLine=-1;
		entry=0;
		exit=0;
		m_nCzhPrintFlag=0;
		show_sation_sep=0;
		show_sides_mode=0;
		inhibit_sametime_dir=0;
		nStaProperty=0;
		deltadistance=0.0;

		memset(station_name,0,sizeof(station_name));

		up_line_count=0;
		down_line_count=0;
		for(int i=0; i<LINE_MAX_CNT; i++)
		{  
			up_line_list[i]=0;
			down_line_list[i]=0;
		}
	}
};

struct VIEW_PORT
{
	enum
	{
		TIME_FLAG_VIEW_HEIGHT = 32,
		STATION_VIEW_WIDTH1 = 150,
		STATION_VIEW_WIDTH2 = 200,
	};
};

struct DRAW_LINE_STRUCT
{
	BYTE type;
	COLORREF clrLine;
	COLORREF plancolor;
	BYTE nPenWidth;
	bool bExitEngine;
	bool bAbsLine;
	int nEngine;
	bool bSchdLine;
};

enum RESULT
{
	SAME=0,
	NOT_SAME=1,
	UNKNOW=2,
};

struct Entry_Exit_Set
{
	int station;
	int nextstation;
	int line_num;
	Entry_Exit_Set()
	{
		station=0;
		nextstation=0;
		line_num=0;
	}
};

class  CTGForm;
class  CTGDoc : public CDocument
{
protected: 
	// protected constructor used by dynamic creation
	CTGDoc();          
	DECLARE_DYNCREATE(CTGDoc)

public:
	void SetTgCtrlPtr(CTGCtrl* ptgctrl);
	CTGCtrl* m_ptgctrl;
	CTG_Mange* myPDataManage;

public:
	COLORREF m_nManualRealColor; // 人工转实际 到发点显示颜色
	int m_nTrainLeapLineFlag;
   
	BOOL m_bShowAdjustErrorStatus;
	BOOL m_bShowErrorStatus;
	
	BOOL m_bShowConflict;

	BOOL m_bShowOPMSTrainsInfo;
		
	BOOL m_bStopStationWaveLine;
	
	bool m_bShowVirtualStation;  //显示虚拟车站
	bool m_bShowDepartFlag;		//显示发车时机星号
	BOOL m_nShowWorkTechFlag;		//显示作业三角
	bool m_nShowWorkSideFlag;		//显示作业股道
	bool m_nShowErrWorkSideFlag;	//始终显示错误股道
	BOOL m_nShowElecLocoFlag;   //显示电力牵引车
	bool m_bUseStringKiloMeter;  //是否使用字符串的公里标
	
	BOOL m_bShowRouteNotAutoTrigerFlag;
	BOOL m_bShowRouteTrigerOkFlag;
	bool m_bShowSelCZHLine;
	bool m_bShowAllCZHLine;
	bool m_bShowEngineCrewMenu; // 控制沈阳机务菜单
	bool m_bShowPlanNoteText; // 是否显示计划台下发的重点事项
	int m_nPageHeight;
	int m_nPageWidth;

    //限制实际运行区和临近计划区的功能
	BOOL m_bLimitDistrictFunction;
	//显示区间赶缓时间
	BOOL m_bShowFastOrSlowTime;
	//显示分界口，区段站早晚点时分 阶段计划
	bool m_bShowAcceptTruckLate;
	bool m_bShowAcceptTrainLate;
	//侧线通过
	BOOL m_bByPassFlag;
	//是否显示反向行车标志
	BOOL m_bShowReverse;
	
	//是否显示公里标
	BOOL m_bShowdistanceStone;
	// 禁止修改实际点
	BOOL m_bInhibitEditRealTime;
    //实际点是否可以修改
	BOOL m_bEditRealLine;
    //是否进行删除车的操作
	bool m_bDeleteMutiTrain;
	//是否进行删除车的操作
	bool m_bMoveMutiTrain;

    // 显示筛选标记
	BYTE m_nShowFlag;

	// 是否计算站序标志
	bool m_bCalcStationIndex;
	// 显示折返车次
	bool m_bShowReturnTrainid;
public:
	int GetShiftId();	
	long m_nShiftDate;
    int  m_nShiftId;
	int  m_nShiftNo;

	ULONG m_nBeginTm;
	WORD  m_nAutoMove;
	WORD  m_nHistoryLen;
	
	int m_fPixelPerTenMinute;
	int m_fPixelPerHalfHour;
	int m_fPixelPerMinute;
	int m_fPixelPerHour;
	
	int m_nViewFormat; 

	TIME m_TGGraphStartTime;
	int mGraphBeginPixel;
	int mGraphEndPixel;
 	
	int m_nAdjustStartX;
	int m_nAdjustEndX;

	int m_nCurrentSectionNo;
	
	BOOL m_bGoodTrainNotTechStopNeedPwd;
	int m_nCurrentStationList[MAX_STATION_PER_SECTION];
	int m_nStationNum;
		
	int m_nDispDistanceNum;
	DispDistance m_sDispDistance[512];

public:
	CTG_TRAIN  m_myFocusTrain;
	CTg_BLOCK  m_myFocusBlock;
	
	int nCurrentX; //当前时间x坐标
    int nCurrentMouseX; //当前鼠标所在位置

	int m_nCurrentNodeIndex;			 //鼠标所在的nodeIndex
	int m_nCurrentStationNoOfMouse;     //鼠标所在的车站号

	CFont fontText;
	CFont fontTech;
	CFont fontSide;
	CFont fontSecondTime;
    CFont fontADTime;
	CFont fontADTimeBold;
	CFont fontAbsTime;
	CFont fontEarlyLastTime;
	CFont fontAnalyTime;
	CFont fontPressure;
	CFont mBlockFont;
	CFont mBlockFontTime;
		
	CBrush m_brushBlack;
	CBrush m_brushWhite;
	CBrush m_brushBlue;

	CFont fontReturnTrain;
	CFont fontNomorlTrain;
	CFont fontElecTrain;
	CFont fontSpaceTrain;
	CFont fontLengthTrain;
	CFont fontMilitaryTrain;
	CFont fontSuperTrain;
	CFont fontEngine;

	CPen  mLeftPen;
	CPen  mRightPen;

	int m_nVerticalPos;
	int m_nHorizontalPos;

	int m_nOperatorStatus;
	
	enum TRAIN_DIRECT
	{
		UP      = 1,
		DOWN	= 2,
		DUAL	= 3,
	};

	enum FOCUS_POINT
	{
		POINT_PLAN =0,       //计划点
		POINT_REAL,          //实际点
        POINT_ROUTE_OK,      //进路已触发
		POINT_ROUTE_NOT_OK,  //进路未触发好
        POINT_INIT           //初始化
	}focus_point;
	
	int m_nADTimeSystle;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTGDoc)
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	void InitTGState();
	int GetTrainIds(CStringArray& trainidary, CTG_TRAIN *train);
	int GetTgType();
	int GetTgTermType();
	int GetDispDistanceIndex(int Pointy);
	const DispDistance* GetDispDistance(int staid);
	DispDistance* GetDispDistanceByIdx(UINT index);
	
	int GetHistoryRegionWidth();
	void SetGraphBeginEndPixel();
	void DrawChordLine(CDC* pDC,POINT pt1,POINT pt2,int dir);
	
	void DrawBasicStationStopLine(CDC* pDC,POINT ptlStart,POINT ptlEnd);
	void DrawSchdLine(CDC* pCDC,const CPoint& ptlStart,const CPoint& ptlEnd,const TgTRAIN_RECORD* prec,bool bAbsLine);
	void DrawBasicSchdLine(CDC* pCDC,const CPoint& ptlStart,const CPoint& ptlEnd,const TgTRAIN_RECORD* prec,bool bAbsLine);
 	void DrawActualLine(CDC* pCDC,const CPoint& ptlStart,const CPoint& ptlEnd,const TgTRAIN_RECORD* prec,bool bAbsLine);
	void DrawLine(CDC* pCDC, const DRAW_LINE_STRUCT& param,const CPoint& ptlStart,const CPoint& ptlEnd);
  
    WORD GetLineColor(const int factor, const BYTE nFlag, COLORREF& schd_color, COLORREF& real_color, COLORREF& vert_color);
	void ResetLineWidth();
	
 	
 	long ConvertXPostoTime(int xPos);
	int  ConvertYPostoStationNo(int yPos,int& node_index,TgTRAIN_RECORD *last=NULL);
    int  GetStationSideNo(int yPos,int node_index);

	void SetStaticTimeByCfg();
	void PointBetweenMidOfTwoStation(int yPos, int& sta1, int& sta2);
    int  PointBetweenMidOfTwoStation(int yPos, TgTRAIN_RECORD *rec, int& direct);
    
	int  ConvertYPostoPrimarySide(int yPos,int &nRouteSetNo);
	
	CString QueryCurrentTrainStationAndside(CPoint point,ULONG index);
	ULONG QueryCurrentBlock(const CPoint& point);

	void DrawSingleTrain(CDC *pCDC,CTG_TRAIN *single,int nDrawMode,bool bShowSecond=false);
	void UpdateBackgrond(CDC* pCDC, CRect& rect);
	void DrawSeperatorBar(CDC *pCDC,long lColor);
	bool IsCanModifyTrainStation(const TgTRAIN_MORE_INFO& train_info);
	bool InitStationVerticalPos(bool bInitStaDistance, bool bWholeScreen);
	bool IsGoodTrainNotTechStopNeedPwd();
	void ReadStationSide();
	
	bool EntrySideConnect(int station, int side, int exit);
	CStationDef *m_pStationDef[MAX_STATION_PER_SECTION];//2007.04.18 增加动车组股道限制
	BYTE GetLateTime(long plan_time, long real_time, CString& strDeltm);

	bool IsOnlyShowMark(void) const;
	bool IsOnlyShowTrain(void) const;
	bool IsShowTrainAndMark(void) const;

	void SetOnlyShowMark(void)		{ m_nShowFlag=1;}
	void SetOnlyShowTrain(void)		{ m_nShowFlag=2;}
	void SetShowTrainAndMark(void)	{ m_nShowFlag=3;}
public:
	CString QueryCurrentTrain(CPoint *point,ULONG& index);
	void InitEarlyLastTime(CTG_TRAIN* single);
	void CreateGDIInfo(CDC* pDC);
private:
	void GetRrcArlRect(CPoint pt, CRect& rect, int nFlagX, int nFlagY);
    void GetRrcRect(CPoint pt, double dltX, double dltY, CRect& rect, int nFlag);
	virtual ~CTGDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	void SetAbsReturnFlag(CTG_TRAIN* train);
	void SetBasicTrainDate(CTG_TRAIN* train);
	void ResetBasicTrainDate(CTG_TRAIN* train);
	void SendAppendMark(CTg_BLOCK* block);
	void SendUpdateMark(CTg_BLOCK* block);
	void SendAppendMark(CTg_BLOCK& block);
	void SendUpdateMark(CTg_BLOCK& block);

	void SendAppendTrain(CTG_TRAIN* train, CString strChangeTrainId, BYTE nRunType);
	void SendUpdateTrain(CTG_TRAIN* train, BYTE Flag = SCHD_UPDATE, DWORD nFlagEx = 0, WORD sta = 0, WORD rec_index = 0, CString trainid = "");
	void SendUpdateTrain(CTG_TRAIN& train, BYTE Flag = SCHD_UPDATE, DWORD nFlagEx = 0, WORD sta = 0, WORD rec_index = 0, CString trainid = "");
	void SendDeleteTrain(CTG_TRAIN& train);
	CTG_TRAIN* GetTrainByIndex(long trainIndex);
	CTG_TRAIN* GetTrainByTrainId(CString trainid, UINT& nFindNum);

	void SendDeleteTrainXml(ULONG train_index);
	void SendDeleteTrainXml(std::vector<ULONG>& selectedTrains);
	void SendDeleteBlockXml(ULONG block_index, int flag=1);  // flag:0给出提示对话框 1直接删除
	
	BOOL GetOperatorPriorByDistrict(int xMouse,WORD focus_pointInfo = POINT_PLAN);
	int  GetNodeIndexByAbs(int curNodeIndex, int abs_num,int dir);
	int  GetNodeSubSectionByIndex(int index);
	
	void DrawLanzhouMarkOfTractionEngine(CDC* pCDC,CTG_TRAIN* single,int nDirect);
	void DrawMarkOfTractionEngine(CDC* pCDC,CPoint pBasic,CSize trainidSize,CTG_TRAIN* single,int nDirect);
	int  GenerateRecordsBetweenNodes(int sectionNo,int preNodeIndex,int nextNodeIndex,CTG_TRAIN* single,long curTime);
	
	int  GetIncreaseLayer(CTG_TRAIN* single,const CPoint& ptlOrig,int nDirect,int statinno, const CSize& trainidSize);
	
	int  GetMultiNodeJudgeIndex(CTG_TRAIN * single, TgTRAIN_RECORD* rec,int& index,int forwardindex,int reverseindex,WORD usFlag,int section,BYTE bJudgeFlag=3);
	int  JudgeMultiNodeIndex(CTG_TRAIN * single,TgTRAIN_RECORD* rec,int& index,WORD usFlag,int section);
	int  JudgeMultiNodeIndexbySeq(CTG_TRAIN * single,TgTRAIN_RECORD* rec,CList<int,int>& list,TgTRAIN_RECORD* _start_rec,TgTRAIN_RECORD* _end_rec);
	int  GetNodeIndexBySubSection(int stationNo,int subsection);
	
	BOOL AdjustRectSizeByBasicPoint(CRect &orignRect, CString drawText,CPoint basicpt,int workThread=0);
	bool JudgeNeedWaveLine(TgTRAIN_RECORD *rec, int nDirectFlag = DOWN_DIRECT);
 	void DrawByPassFlag(CDC * pCDC,CTG_TRAIN *single);
	void DrawTrainFocusRect(CDC*pCDC, CTG_TRAIN* single);
	void DrawFastOrSlowTime(CDC *pCDC, TgTRAIN_RECORD *rec, POINT &ptlStart, POINT &ptlEnd,BOOL isFast,int minute,int dir = 2);

	int  ConvertYPostoStationNos(int yPos, int &node_index, int &nStationNum);
	BOOL PtInRects(RECT rects, POINT pt);   

	WORD GetEntryFromNodeIndex(int node_index, int dir, int abs_line[]);
	void GetEntryExitFromNodeIndex_abs(int node_index,WORD& entry,WORD *exitlist, WORD& exitnum,CString *sExplain,WORD* entry_next,WORD* station_next,int dir,int next_station=0);
	void GetEntryExitFromNodeIndex(int node_index,int& entry,int& exit,int dir = DOWN_DIRECT);
 	void GetExitEntryFromTwoNodeIndex(int prev_node_index,int node_index,int& exit,int& entry,int dir = DOWN_DIRECT);
    BOOL JudgeNodesConnectedThroughThirdParty(WORD cur_node_index,WORD pre_node_index,int &replace_node_index);
	// 获取区间开回列车,备用入口区间线别
	int GetEntryByNodeIndexForFrom_abs(int node_index, int station, int dir, int abs_line_list[32]);
	// 获取开向区间列车,备用出口区间线别
	int GetExitByNodeIndexForTo_abs(int node_index, int nstation, int dir, int abs_line_list[32]);
	int GetExitByNodeIndex12ForTo_abs(int node_index1, int node_index2, int abs_line_list[32]);
    // 判断一个车站是否在本台上
	bool IsThisSectionStation(int station);
	BOOL JudeNodesConnected(WORD node_index1,WORD node_index2);
   
	int  GetNodeExitIndex(CTG_TRAIN * single, TgTRAIN_RECORD* rec, int section);
	int  GetNodeEntryIndex(CTG_TRAIN * single, TgTRAIN_RECORD* rec, int section);

    int  GetEntryExitDirection(CTG_TRAIN * single,TgTRAIN_RECORD* rec,WORD usFlag,int section);

	
	CFont* GetDrawTrainNoFont(const TgTRAIN_MORE_INFO& nTrainInfo, CString& strTrainId, bool& bDrawRect);
	
	void DrawMarkOfStart(CDC *pCDC,CTG_TRAIN * single, CString strTrainId,CPoint ptlStart,int nDirect,COLORREF line_color,int line_width, TgTRAIN_RECORD* rec);
	void DrawMarkOfArrive(CDC *pCDC,CTG_TRAIN * single,CString strTrainId,CPoint ptlStart,int nDirect,COLORREF line_color,int line_width, TgTRAIN_RECORD* rec);
 
	void DrawStopOutofSignalMark(CDC *pCDC, CPoint ptlStart, CPoint ptlEnd, int direct);
	void DrawMarkOfDepart(CDC *pCDC,CTG_TRAIN * single,CPoint ptlStart,int nDirect,COLORREF line_color,int line_width,const TgTRAIN_RECORD* rec);	
	void DrawMarkOfEnd(CDC *pCDC,CTG_TRAIN * single,CPoint ptlStart,int nDirect, COLORREF line_color,USHORT flag, int line_width);
	void DrawMarkOfEndLittle(CDC *pCDC,CTG_TRAIN * single,CPoint ptlStart,int nDirect,COLORREF Color, USHORT flag, int line_width);
    void DrawMarkOfZheFanTrainID(CDC *pCDC, CPoint ptlStart, int nDirect,COLORREF Color, CString& trainid,CTG_TRAIN * single);
	
	long  GetXPositionConsiderSecond(long nTime);

	long  GetOffsetMinuteFromGraphBegin(long nTime);
	long  GetOffsetPixelFromGraphBegin(long nTime);
	
	void SetGraphStartTime(ULONG nShiftId);
	void SetGraphStartTime_New(ULONG nShiftId);
 	
	void DrawArrivalTime(CDC* pCDC,  TgTRAIN_RECORD* rec,  CPoint point,  int lColor, int lActualColor,int nDrawMode, int flag=0);
	void DrawDepartTime(CDC* pCDC,  TgTRAIN_RECORD* rec,  CPoint point,  int lColor, int lActualColor,int nDrawMode, int flag=0);

	
	void DrawTrainWorkSide(CDC * pCDC,CTG_TRAIN * single,int nDrawMode);
	void DrawTrainWorkText(CDC * pCDC,CTG_TRAIN * single);
	void DrawTrainNoteText(CDC * pCDC,CTG_TRAIN * single);
	void DrawTrainGiveoutDirection(CDC * pCDC,TgTRAIN_RECORD * prec,int direct_flag, const CPoint& ptlStart,CTG_TRAIN * single);
	void DrawTrainAcceptDirection(CDC * pCDC,TgTRAIN_RECORD * prec,int direct_flag, const CPoint& ptlStart,CTG_TRAIN * single);

	void DrawTrainTechDepartMark(CDC * pCDC,CTG_TRAIN * single);
	void DrawTrainTrigerMark(CDC * pCDC,CTG_TRAIN * single);
	void DrawTrainRouteOKMark(CDC * pCDC,CTG_TRAIN * single);
    
    int  GetStationVPos(CTG_TRAIN * single, TgTRAIN_RECORD* rec,int usSideFlag,int usFlag = EXIT_POSITION);
	int  GetStationVPos(int dist_index, int usSideFlag);
	int  GetStationSideVPos(const TgTRAIN_RECORD* rec, const int index, const int side_no);
	int  GetAbsYPosition(CTG_TRAIN * single, const int rec_index, WORD position);

	void GetSepState(int index, bool& bIndexSep);
	//{{AFX_MSG(CTGDoc)
 	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void DrawTrainBenqianTime(CDC *pDC, CTG_TRAIN * single, TgTRAIN_RECORD* first, const CPoint& ptlEnd, CRect& tempRect);
	void DrawBadMark(CDC *pCDC, CTG_TRAIN * single, const CPoint &ptlStart, const CPoint &ptlEnd, const TgTRAIN_RECORD* first);
    void DrawNoMeetMark(CDC* pCDC, const CPoint& ptlStart, const CPoint& ptlEnd, long no_meet_flag, COLORREF clrText, CString strTrainID);

	void DrawTrainDecorateGreenLine(CDC* pCDC, const CPoint& ptlStart, const CPoint& ptlEnd);
	void DrawUnknowTrainFlag(CDC *pCDC, CTG_TRAIN * single);
	
public:
	bool FindTrainByTrainIndex(long& position, DWORD& train_index, const CString& trainId);
	int  FindTrainByTrainId(const CString& trainId, long postion[], long indexlist[]);
	int	 FindTrainByTime(DWORD btm, DWORD etm, ULONG train_index[]);
	BOOL IfRecordInTimeScope(TgTRAIN_RECORD *record, DWORD begin_time, DWORD end_time);
	BOOL IfTrainInTimeScope(CTG_TRAIN *ptrain, DWORD begin_time, DWORD end_time, BOOL IsStart);

	//批量修改多车的区间运缓时间
	BOOL IfTrainInStationScope(CTG_TRAIN *ptrain, int begin_sta_index, int end_sta_index, DWORD begin_time, DWORD end_time);

	bool GetOperatorRight(BYTE oper);	
	CString GetDataPath() { return m_ptgctrl->mConfigTg.m_context.GetDataPath();}
	CString GetStationName(int stano);
	CString GetSideName(int stano, int sideno);
	int  GetStationProperty(int stano);
	
	CString QueryTrainFromAndToStation(CTG_TRAIN* single);
  
public:
	int  GetAbsByStationNoAndEntry(WORD station, WORD entry);
	int  ConvertAbsnumStationToEntry(WORD station,WORD abs_num);
	bool ConvertStationEntryToAbsnum(WORD station, WORD entry_num,WORD *abs_num,WORD *dir);
	// 根据给定站和口子，获取对应该口子区间的另外一个站的站号
	int  GetAbsLineOtherStation(WORD station, WORD entry_num, WORD& line_no);
	// 根据给定站和口子，获取对应该口子区间的另外一个站的站序
	int  GetAbsPreStationIndexByNextStationAndEntry(WORD station, WORD entry_num, bool bStopAbsUp, int in_station_index);
    

	bool IsCanEditTrain();	
	TIME GetBeginTime() {return m_TGGraphStartTime;}
	void SetSchdBeginTime(void);

	int  GetLineNoByStations(int from_st, int to_st);
	void GetViewScope(TIME& begin, TIME& end);
	void GetAcceptDataScope(TIME& begin, TIME& end);
public:
	void  ReGeneratetStationInOutIndex(CTG_TRAIN& train);
	bool  CheckStationInOutIndex(CTG_TRAIN& train);
	void  GenTrainExitEntryByStationIndex(CTG_TRAIN& train);
	void  SetStationInOutIndex(CTG_TRAIN& train);
	
	int  SetTrainExitByCfgForUI(CTG_TRAIN* train, int rec_index,int abslineno[]);
	int  SetTrainEntryByCfgForUI(CTG_TRAIN* train, int rec_index, int abslineno[]);
	int  SetTrainExitByCfg(CTG_TRAIN* train, int rec_index);
	int  SetTrainEntryByCfg(CTG_TRAIN* train, int rec_index);

	void  CheckABC(CTG_TRAIN& train);
	void  CheckFirstLast(CTG_TRAIN& train);
	void  SetTrainFirstRecIndex(CTG_TRAIN& train, bool bFlag);
	void  SetTrainMidRecIndex(CTG_TRAIN& train, int rec_index);
	void  SetTrainLastRecIndex(CTG_TRAIN& train, bool bFlag);
	
	int   GetTrainDirectionByTrainId(CString strTrainId);
	void  SetTrainDirect(CTG_TRAIN& train);
	void  SetTrainSideAndSchdReal(CTG_TRAIN& train);
	void  SetTrainWidthColor(CTG_TRAIN* ptrain);
	
	void  SetTrainRect(CTG_TRAIN* ptrain);
	void  ShowRect(CDC*pCDC, const CRect& rect);
	void  SetWorkTrainRect(CTG_TRAIN* ptrain);
	void  SetBasicTrainRect(CTG_TRAIN* ptrain);
	BOOL  SetABSStart(CTG_TRAIN* train, CString m_strTrainId, long usTimeValue, int ypos, std::vector<int>& abs_line_list);
private:
	COLORREF GetSecondBkColor(LONG x); // 按照到发点位置,获取底图背景颜色
	CString GetTimeString(long time, CString& strSecond, bool bShowSecond);
	bool GetFlagDrawTrainTime(int stano);

	bool TwoStationSubSectionSame(int index1, int index2);
	RESULT StationInTwoSubSection(int index1, int index2);

	bool GetNodeIndexOfDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, int exitEntryFlag, int section, int &index);
	bool GetNodeIndexOfAllDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, int exitEntryFlag, int section, int &index, section_path_define& route_define);
	bool GetNodeIndexOfGoodDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, int exitEntryFlag, int section, int &index, section_path_define& route_define);
	bool GetNodeIndexOfPessagerDefine(CTG_TRAIN *single, TgTRAIN_RECORD* rec, int exitEntryFlag, int section, int &index, section_path_define& route_define);

	void DrawAbsInhibitMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawAbsSlowMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawAbsEleFixMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawAbsZhFixMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawAbsSelfCloseMark(CTg_BLOCK& block, CDC *pDC, int nDirect);

	void DrawSideInhibitMark(CTg_BLOCK& block, CDC *pDC);
	void DrawSideSlowMark(CTg_BLOCK& block, CDC *pDC);
	void DrawSideOccupyMark(CTg_BLOCK& block, CDC *pDC);
	void DrawSideTianchuangMark(CTg_BLOCK& block, CDC *pDC);

	void DrawNoteTextMark(CTg_BLOCK& block, CDC *pDC);
	void DrawNoteTextMark2(CTg_BLOCK& block, CDC *pDC);

	// 兰州画法 G21
	void DrawLanzhouAbsInhibitMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawLanzhouAbsSlowMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	void DrawAbsLanzhouEleFixMark(CTg_BLOCK& block, CDC *pDC, int nDirect);
	
public:
	int   GetNoteChineseChar(CString strNotes, int len);
	void  SetGraphPosition(CTg_BLOCK* block, CRect rect);
	void ParseMarkReasonText(const CString& text, CString& dcmdno, CString& reason);
	CString CombineReasonText(const CString& dcmdno, const CString& reason);
public:
	bool GetABSLine(int lineNo, ABS_LINE& lineInfo);
	bool GenDefaultBlock(CTg_BLOCK& block);
	
	void SetBlockDriect(CTg_BLOCK& block);
	bool GetABSLineNoByIndex(int index1, int index2);
	bool GetIndexByStation(WORD stano1, WORD stano2, int& index1, int& index2);
	void SetDocContext();
	
	void sysprint(short msg_level,unsigned short process_id,char *format, ... );

	int  GetVPosition(int idx, int flag);
	int  GetSubSection(int idx);
	int  GetEntry(int node_index);
	int  GetExit(int node_index);
public:
	CString GetTrainShowId(CTG_TRAIN& train);	
	CString GetTrainShowId(CTG_TRAIN* train);
	CString GetTrainShowId(const int station, const TgTRAIN_RECORD& rec, CTG_TRAIN* train, bool bStar=false);

	CString GetABSStrDirectDesp(int direct, USHORT nAbsNo, USHORT nAbsNum);
	void    GetPlotParamXml(ULONG nFlag);

	CStationDef* GetStationSideDef(int station);
	void CheckTrainSideInfo(CTG_TRAIN *ptrain, int rec_index, CStringArray& ary);

	bool  IsEnableChangeSide(HWND hWnd, TgTRAIN_RECORD *rec, CTG_TRAIN *pTrain);
	bool  SideSetEntry(HWND hWnd, int arrive_side, int depart_side, int rec_index, CTG_TRAIN *pTrain);
	bool  ArriveSideSetEntry(HWND hWnd, int arrive_side, int rec_index, CTG_TRAIN *pTrain);
	bool  DepartSideSetEntry(HWND hWnd, int depart_side, int rec_index, CTG_TRAIN *pTrain);

	bool  IsEntrySideConnect(int station, int side, int entry);
	bool  IsSideExitConnect(int station, int side, int exit);

	bool  CheckSideEntry(int arl_side_no, int dep_side_no, const TgTRAIN_RECORD* rec, int& flag);
	// 下面函数不能直接调用
	int   OffsetWorkScheduleTimeByRouteOk(CTG_TRAIN* train, int nRecordNo, int nTimeOffset, int nFlag);
private:
	bool IsMutilStation(int station);
	
	CString GetStationSidesName(const int nStaNo, const USHORT nSidesNum, const USHORT nSidesNo[]);
	bool TrainHaveActualPoint(CTG_TRAIN* train);

	int  GetEngineCount(const CTG_TRAIN *single, UINT rec_idx);

	void DrawStopSidePoint(CDC*pCDC, const CPoint& pt_side, COLORREF lColor);
	
	void DrawStopLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, int nDrawMode);
	void DrawStopLine3LeapLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, const int lColor, int lActualColor, int nDrawMode, int in_index, int out_index);
	void DrawStopLine1(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, COLORREF lColor, COLORREF lActualColor, int nDrawMode);
	void DrawStopLine2(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, COLORREF lColor, COLORREF lActualColor, int nDrawMode);
	void DrawStopLine3(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, COLORREF lColor, COLORREF lActualColor, int nDrawMode);
	
	bool DrawAbsLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, TgTRAIN_RECORD* second, CPoint& ptStart, CPoint& ptEnd);
	void DrawAbsStopLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, TgTRAIN_RECORD* second);
	void DrawTrainStartOrAcceptMark(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, const TgTRAIN_MORE_INFO& train_info);
	void DrawTrainEndOrDepartMark(CDC*pCDC, CTG_TRAIN* single,  TgTRAIN_RECORD* second);
    
	void DrawTrainRescuePart(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first);
	BOOL DrawUniteTrainMark(CDC*pCDC, TgTRAIN_RECORD *first);
	void DrawDivideOrCombineTrainMark(CDC*pCDC, TgTRAIN_RECORD *first);
   
	bool IsShowSidePos(int nSideNo, int index);

	void DrawMutiTrainSeparate(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first);
	WORD GetStationAbsPosition(CTG_TRAIN* single, TgTRAIN_RECORD* rec);
	void DrawAbsText(CDC*pCDC, CTG_TRAIN*single, TgTRAIN_RECORD *first, const CPoint& ptlStart, const CPoint& ptlEnd, bool bStopAbs);
	void DrawUniteTrainAbsLine(CDC*pCDC, TgTRAIN_RECORD *first, const CPoint& ptlStart, const CPoint& ptlEnd);

	void DrawStatStationArrival(CDC * pCDC,CTG_TRAIN *single, TgTRAIN_RECORD* first, CPoint point, int nDrawMode);
	void DrawStatStationDepart2(CDC * pCDC,CTG_TRAIN *single, TgTRAIN_RECORD* first, CPoint point,int nDrawMode);

	void DrawStatStationDepart(CDC * pCDC,CTG_TRAIN *single, TgTRAIN_RECORD* first, CPoint point,int nDrawMode);
	
	void PressureStopLine(CDC * pCDC,TgTRAIN_RECORD* first, BYTE in_index, BYTE out_index, const CPoint& ptSide, const CPoint& ptDepSide);
	void DrawEngineLine(CDC*pCDC, CTG_TRAIN*single, TgTRAIN_RECORD* first, const CPoint& ptlStart, const CPoint& ptlEnd);
	void DrawAbsPartRunLine(CDC*pCDC, CTG_TRAIN*single, TgTRAIN_RECORD* first, const CPoint& ptlStart);
public:
	int GetCascoIdByLocoId(int id);
	CString GetLocoNameByLocoId(int id);
    void SetBasicNoStatLateTime(CTG_TRAIN* train);
	int  GenExitEntryByInitABS(int conid, int station1, int station2, int index1, int index2, int& exit, int& entry);

	int  GetSideUseByYPos(int yPos, int index);
	int  ConvertYPostoSideUseStationNo(int yPos, int& node_index);
	void EraseTrainIndex(ULONG train_index) { m_nEraseTrainIndex=train_index;}
private:	   
	COLORREF mjcolor, mfcolor;
	TRAIN_INDEX  m_nEraseTrainIndex;
	int m_nTrainNoMaxLevel;
	CWordArray   locoId;
	CWordArray   cascoId;
	CStringArray locoPref;
	CStringArray locoName;

	int m_linecount;
	struct ABS_LINE_INFO
	{
		int  lineNo;
		int  dir;
	}m_line_list[128]; 
	long m_nFirstArlMinuteOffset, m_nSecondArlMinuteOffset;
	long m_nFirstDepMinuteOffset, m_nSecondDepMinuteOffset;
	// 计算位置考虑秒,以下相同
	int  m_nFirstArlSecond, m_nSecondArlSecond;
	int  m_nFirstDepSecond, m_nSecondDepSecond;
	//X.Y轴的偏移量
	int x_point_offset;
	int y_point_offset;

	CString time_char;

	bool first_a_time_status,second_a_time_status;
	bool first_d_time_status,second_d_time_status;

public:
	CPoint m_selmarkpoint;
public:
	void SetDirectAtStation(CTG_TRAIN* train);
	void DrawSingleMark(CTg_BLOCK& block, CDC *pCDC);
	void FrameRectExtend(CDC *pDC,CRect &rc);
	void ClearTrainRelation(TgTRAIN_RECORD* rec);
	
	bool SetBlockDirect(CTg_BLOCK* block);
	int  GetNeibAbsLines(int index1, int index2, std::vector<int>& abs_line_list);
	void SetTimeValid(CTG_TRAIN* train);
	int  GetContextLineWidth();
	int  GetEarlyLateTime();
	bool IsShowLocoNo();
	int  GetShowTrainType();
	int  GetShowTrainDirect();
	bool IsShowStationMark(CTg_BLOCK& block, UINT nIndex, int& y1, int& y2);
    int  GetMutiNeibAbsLines(int index1, int index2, std::vector<int>& abs_line_list);
	const CTG_TRAIN* GetTrainByPoint(CPoint& pt);

	int  GetTrainDirectByTrainId(CString trainid);
	CTG_TRAIN* GetPrevTrainByIndex(long index);
public:
	struct ContinueDrawInfo
	{
		long m_nTrainIndex;
		int   m_nRecIndex;
		bool  m_bContinueDraw;
	}m_ContinueDrawInfo;
	void ResetContinueDraw();
	void SetContinueDraw(int rec_index, long trainIndex);
	bool IsContinueDrawTrain(int& rec_index, long& trainIndex);
public:
	CDWordArray  m_aryStatus;
	CStringArray m_aryEngine;
	std::map<ULONG, ULONG> m_noearlylatetime;
	bool m_bAcceptCadData;

public:
	TrainTypeCfg mTrainType;
	SPECIAL_LINE_STYLE line_style;
	bool mIsSchdLineSameToActualLine;
	bool mIsChangeVertColor;
	int  mMouseMoveMode;  // 0:整分移动，显示分 1：默认值，秒级移动，显示分 2：秒级移动，显示秒
	int  mDisplayUnknownText;
	int  m_nBlockNum;
	bool m_bIsVerifyTrainRect;
	int  m_nTrainZoomRangeX;
	int  m_nTrainZoomRangeY;
	SPECIAL_TRAIN_ID_SHOW trainid_style;
	DECORATE_STYLE decorateStyle;
	bool m_bElecTrainDrawGreenDecorate;
public:
	void DrawBasicTrainConnectLine(std::map<ULONG, TRAIN_CZH_INFO>::iterator& it, std::map<ULONG, TRAIN_CZH_INFO>::iterator& next_it, TRAIN_CHEZUHAO* pTrainsCZH, CDC* pCDC);
	void DrawBasicTrainConnectLine(TRAIN_CHEZUHAO* pTrainsCZH, CDC* pCDC);
	void DrawTrainConnectLine(TRAIN_CHEZUHAO* pTrainsCZH, CDC* pCDC);
	int  GetStationSideVPos(const int index, const int side_no, int direct);
	
public:
	void ModifyTrainCZH(CString czh, CString newczh);
	void DeleteTrainCZH(CString czh);
	void AddNewTrainCZH(CString czh);
	void AddTrainToCZHGroup(CString czh, CString trainid);
	bool DelTrainFromCZHGroup(CString czh, CString trainindex);

	void AddTrainToCZHGroup(DWORD prev_train_index, CString trainid);
	void SetFormView(CTGForm* pView);
	bool GenTrainReturnTrainID(CString czh, CStringArray& text);
	void ReGenTrainReturnTrainID(CTG_TRAIN& train);
	void ReGenTrainCZHRelation(CString czh); 
	bool IsCanSetInterCityReturn(CTG_TRAIN& train, CString& text, CStringArray& list_trainno, CDWordArray& list_trainindex);
	bool IsCanSetInterCityStartReturn(CTG_TRAIN& train, CString& text, CStringArray& list_trainno, CDWordArray& list_trainindex);
	
	void ReGenTrainCZHRelation();
	void ReGenTrainJiaoluRelation();
	
	bool IsAbsBySide(HWND m_hWnd, BYTE sideno, int rec_index, CTG_TRAIN *pTrain, int param);
	void SetLayer(CDC* pLayer1, CDC* pLayer2, CDC* pLayer3) 
	{
		m_pLayer1=pLayer1; 
		m_pLayer2=pLayer2;
		m_pLayer3=pLayer3;
	}  // 优化效率使用
public:
	void DrawAllCheZuHaoLine(CDC * pCDC);
	TRAIN_CHEZUHAO* GetCurrentSelTrainCZH();
    void SetCurrentSelTrainCZH(CString czh);
	CCheZuHaoManger myChezuhao;

private:
	CString m_CurSelCZH;
	CDC    * m_pLayer1;   // 优化效率使用
	CDC    * m_pLayer2;   // 优化效率使用 
	CDC    * m_pLayer3;   // 优化效率使用 
	CTGForm* m_pView;

// 沈阳局自动平移慢性施工
public:
	void AddMarkMoveIndex(ULONG index);
	void DelMarkMoveIndex(ULONG index);
	bool IsMarkMove(ULONG index);
	
	void ReadFromFile();
	void WriteToFile();
	CString mfilePath;
	std::map<ULONG,ULONG> mark_move_index;

public:
	CString GetThisTrainReturnTrainID(CTG_TRAIN& train, int rec_index);
	void GetJiaoluDlgName(CString& title1, CString& title2);
	void GetChezuhaoDlgName(CString& title1, CString& title2);
	BYTE GetEarlyLateTimeRd(long train_index, int station, int rec_index, CString& trainid);
	void SetLateEarlyTimeRd(long train_index, int station, int rec_index, BYTE flag, CString& trainid);

// 济南局设置?股
	void SetAllTrainsErrorSide(int dis_index, int station_no);
    void TrytoFindAbsEngineReturnTrain(CTG_TRAIN* ptrain, std::vector<CTG_TRAIN>& train_list);
    void TrytoFindAbsPartRunTrain(CTG_TRAIN* ptrain, std::vector<CTG_TRAIN>& train_list);
	bool TrytoMoveOtherTrain(CTG_TRAIN* ptrain, CTG_TRAIN& othertrain);

	void CheckSideRelation(CTG_TRAIN *ptrain, int rec_index, CStringArray& ary);
	void ReLayoutBmpPageSize(int nSelDistanceNo, HWND hWnd);
	void SetTrainRelationSideModify(CTG_TRAIN* ptrain);
	
private:
	std::vector<Entry_Exit_Set> mSetLineNoAry;
	std::vector<Station_Direction_Tip> mDirectionTipAry;
	std::vector<StationGiveoutTip> mGiveoutTipAry;
	std::vector<StationAcceptTip> mAcceptTipAry;
public:
	// 单趟车显示查找
	bool FindTrainByTrainIndexForSingleTrain(long& position, DWORD& train_index, const CString& trainId);
	int  FindTrainByTrainIdForSingleTrain(const CString& trainId, long postion[], long indexlist[]);
	// 单趟车模式
	bool m_bShowSingleTrain;
	bool  m_bLineBold;
	bool m_bShowWholeScreen; // 是否全屏显示
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINTGDOC_H__27E02CFA_7B29_11D1_9CEF_000021000CE6__INCLUDED_)
