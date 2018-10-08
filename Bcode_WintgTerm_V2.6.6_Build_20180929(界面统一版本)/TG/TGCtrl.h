#pragma once
#include "IF.h"
#include "TG_Manage.h"
#include ".\config\tgconfig.h"
// TGCtrl.h : CTGCtrl ActiveX 控件类的声明。

// CTGCtrl : 有关实现的信息，请参阅 TGCtrl.cpp。
// 运行图接收状态
struct SCHD_RECIEVE_STATUS
{
	enum{
	 END    = 0,
	 BEGIN	= 1,
	};
};

class CTGCtrl : public COleControl
{
	DECLARE_DYNCREATE(CTGCtrl)

// 构造函数
public:
	CTGCtrl();

//内部数据
private:
	DWORD m_timePress;
	bool mbFirstRun;
	CFrameWnd *m_pFrame;
	CSingleDocTemplate *m_pDocTemplate;
	

public:
	CTgConfig mConfigTg;
// 重写
public:
	ULONG GetShiftId() { return m_nShiftid; }
	void  GetShiftID(ULONG& nBeginTm, WORD& nAutoMove, WORD& nHistoryLen)
	{
		nBeginTm=m_nBeginTm;
		nAutoMove=m_nAutoMove;
		nHistoryLen=m_nHistoryLen;
	}
	bool IsNewMode() { return m_bNewMode;}
	void SetBeginHour(BYTE dlt){m_nBeginHour = dlt;}
	BYTE GetBeginHour(){return m_nBeginHour;}
	int  GetTimeLength(){return m_nTimeLength;}
	BYTE GetTGType(){return m_nTgSchdType;}
	CString GetTermTGTypeName();
	BYTE GetTgTermType() { return m_nTermSchdType;}
	bool IsWorkMode() { return m_bWorkMode;}
	bool IsFormatView() { return m_bFormatView;}
private:
	// 是否显示为总公司统一界面
	bool  m_bFormatView;
	// 工作编辑模式
	bool  m_bWorkMode;
	// 班次
	ULONG m_nShiftid;
	// 时间是否自动推进
	WORD  m_nAutoMove;
	// 开始时间
	ULONG m_nBeginTm;
	// 开始时间小时 对于基本图固定18,其它图由m_nBeginTm算出
	BYTE  m_nBeginHour;
	// 当前时间前长度小时
	WORD  m_nHistoryLen;
    // 模式 true:新模式 false:老模式
	bool  m_bNewMode; 
	// 时间长度
	int  m_nTimeLength;
	// 图的类型
	BYTE m_nTgSchdType;
	// term中图的类型
	BYTE m_nTermSchdType;
	// 车次号显示层数
	int m_nTrainNoLevelCount;
	// 能够解除基本图卡控的车站站号
	std::vector<int> m_bplanstation;
// 重写
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	bool IsBPlanStation(int station) const;
	int  GetTrainNoLevelCount() { return m_nTrainNoLevelCount; }
private:
	void SetTgCfg(CString dataPath);
	void SetIsDrawCurrentFlag(bool bDraw=false);
// 实现
protected:
	~CTGCtrl();

	DECLARE_OLECREATE_EX(CTGCtrl)    // 类工厂和 guid
	DECLARE_OLETYPELIB(CTGCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CTGCtrl)     // 属性页 ID
	DECLARE_OLECTLTYPE(CTGCtrl)		// 类型名称和杂项状态
																			
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_EVENT_MAP()
// 调度和事件 ID
public:
	enum 
	{					
		dispidTgStart = 1L,
		dispidSetTrainDisplayFlag = 2L,
		dispidSetBackGroud = 3L,	
		dispidSetTgType = 4L,	

		dispidSetBackgroudBeginHour = 5L,	
		dispidSetBackgroudTimeLength = 6L,		
		dispidSetStationInfo = 7L,	
		dispidSetCurrentSectionId = 8L,
		dispidSetShiftId = 9L,	

		dispidDeleteMutiTrain = 10L,
		dispidDrawSpecialTrain = 11L,	
		dispidFindTrain = 12L,
		dispidTrainsOperator = 13L,
		dispidSelectSection = 14L,

		dispidZoomIn = 15L,	
		dispidZoomOut = 16L,
		dispidGetZoomStatus = 17L,
		dispidSetStationConnectStatus = 18L,
		dispidSetCtrlMode = 19L,	

		dispidGetTrainDisplayFlag = 20L,
		dispidSaveToPicture = 21L,	
		dispidNewMark = 22L,
		dispidBeginNewSingleTrain = 23L,
		dispidEndNewSingleTrain = 24L,

		dispidDeleteTrain = 25L,
		dispidDeleteMark = 28L,
		
		dispidEndLoadData = 31L,
		dispidBeginLoadData = 32L,	
		dispidTurnReality = 33L,
		dispidReceivePoint = 34L,

		dispidSetAppCtrlWord = 35L,
		dispidSetLineCtrlWord = 36L,	
		dispidSetMarkCtrlWord = 37L,
		dispidGetAppCtrlWord = 38L,		
		dispidGetLineCtrlWord = 39L,

		dispidGetMarkCtrlWord = 40L,
		dispidSetOperateRight = 41L,	
		dispidGetOperateRight = 42L,
		dispidSetLeapLineFlag = 43L,	
		dispidSetDrawTenicalStopMark = 44L,	

		dispidSetUseBmp = 45L,	
		dispidSetAllStationActive = 46L,	
		dispidSetPlanRecieveFlag = 47L,		
		dispidSetLeapTrainShow = 48L,
		dispidSetRouteOkMarkFlag = 49L,	

		dispidAliveWindow = 50L,
		dispidGetCurrentSectionID = 51L,	
		dispidSetTrainDefaultInfo = 52L,	
		dispidGetTgVersion = 53L,			
		dispidGenTrainExitEntryByStationIndex = 54L,		
		dispidShowWholeScreen = 55L,

		dispidSetContrlRight = 56L,
		dispidGetPlotParam = 57L,
		dispidGetTgData = 58L,
		dispidSetBasicTrainWholeMove = 59L,
		dispidRefreshTG = 60L,

		dispidBeginNewSingleTrainXml = 61L,
		dispidTGGenTrainNodeIndex = 62L,
		dispidAppendEarlyLateInfo=63L,
		dispidSetWinTGOperateRight = 64L,
		dispidSetUseVirtualMemory = 65L,
		dispidSetWidePixel = 66L,

		dispidOnStopDraw = 67L,		
		dispidOnBatchDraw = 68L,		
		dispidOnBatchSchduleData = 69L,	
		dispidIsBatchStatus = 70L,
		dispidSetBackgrandInfo = 71L,
		dispidNewShiftBanci = 72L,

		dispidGetLoginShiftId = 73L,
		dispidGetBeginTime = 74L,
		dispidGetHistoryLen = 75L,
		dispidGetAutoMove = 76L,
		dispidDataIsExistInOCX = 77L,
		dispidSetSectionInfo=78L,

		dispidAddBStplanStation=79L,
		dispidUsingWorkGoodMode=80L,
		dispidMoveTGOCXOneStep=81L,
		dispidSetUserRight=82L,
		dispidSetNotMeetInfo=83L,
		dispidFindTrainByTrainIndex=84L,

		dispidSetFormatView=85L,
		dispidCheckSideByRelation=86L,
		dispidGetTrainJiaoRelation=87L,
	};
protected:
	void  TgStart(LPCSTR filename);
	void  OnSetTrainDisplayFlag(SHORT type, SHORT val);
	void  OnBeginNewSingleTrain(LPCTSTR strTrainId, LPCTSTR strChangeTrainId, BYTE inMode, BYTE outMode);
	SHORT OnSelectSection(SHORT nSection,LONG nFlag);
	SHORT OnDrawSpecialTrain(SHORT trainType);
			
	void  OnEndNewSingleTrain(void);
	void  OnFindTrain(void);		
	void  OnFindTrainByTrainIndex(ULONG train_index);
	void  OnTrainsOperator(SHORT oper, SHORT rst);
		
	void  OnNewMark(SHORT mark_type);
	void  OnSetBackGroud(USHORT flag);
	void  OnSetTgType(BYTE type);
	VARIANT_BOOL GetTrainDisplayFlag(SHORT type, SHORT val);
	SHORT OnZoomIn(SHORT param);

	SHORT OnZoomOut(SHORT param);
	void OnSetStationConnectStatus(SHORT nStaId, BYTE active);
	void OnSetCtrlMode(SHORT nStaId, SHORT sta_ctrl_mode, SHORT route_ctrl_mode);
	void OnSetBackgroudBeginHour(BYTE begin_hour);
	void OnSetBackgroudTimeLength(BYTE nHours);

	SHORT OnGetZoomStatus(void);
	void  OnDeleteMutiTrain(void);
	
	void OnSetStationInfo(LPCTSTR xml);
	void OnSetCurrentSectionId(SHORT flag);
	void SetShiftId(ULONG nshiftid, SHORT nCurValue);


	void OnSaveToPicture(LPCTSTR pathname);
	void OnBeginLoadData(void);
	void OnEndLoadData(void);
	
	void OnDeleteMark(LONG nIndex);
	void OnDeleteTrain(ULONG nIndex, ULONG dwLogicFlag); 

	void OnTurnReality(void);
	void OnReceivePoint(void);

	void SetAppCtrlWord(ULONG word);
	void SetLineCtrlWord(ULONG word);
	void SetMarkCtrlWord(ULONG word);
	ULONG GetAppCtrlWord(void);
	ULONG GetLineCtrlWord(void);
	ULONG GetMarkCtrlWord(void);

	void SetOperateRight(BYTE right);
	BYTE GetOperateRight(void);
	void SetLeapLineFlag(BYTE flag);
	void SetDrawTenicalStopMark(SHORT flag);
	void SetUseBmp(BYTE flag);

	void SetAllStationActive(void);
	void OnSetPlanRecieveFlag(BYTE nType, ULONG id, ULONG color);
	void SetLeapTrainShow(BYTE nFlag);
	void SetRouteOkMarkFlag(BYTE flag);

	void AliveWindow(ULONG nFlag);
	ULONG GetCurrentSectionId(void);
	BSTR OnSetTrainDefaultInfo(const VARIANT& msg);
	BSTR OnGetTgVersion(void);
	BSTR GenTrainExitEntryByStationIndex(const VARIANT& msg);
	void OnShowWholeScreen(BYTE nFlag);
	void SetContrlRight(unsigned long nLFlag);

	void OnGetPlotParam(ULONG nFlag);
	void OnGetTgData(ULONG flag);
	void OnSetBasicTrainWholeMove(BYTE nFlag);
	void OnRefreshTG(ULONG nFlag);
	void OnBeginNewSingleTrainXml(LPCTSTR xml);

	void OnGenTrainNodeIndex(LPCTSTR xml);
	void OnAppendEarlyLateInfo(LPCTSTR xml, BYTE val, LONG nIndex);
	void SetWinTGOperateRight(DWORD right);
	void SetUseVirtualMemory(short val);
	void SetWidePixel(short val);

	void OnSetBatchDrawEnv(LONG nFlag);
	void OnBatchDraw(LONG nFlag);
	LONG OnBatchSchduleData(const VARIANT& msg);
	BYTE IsBatchStatus(void);
	void SetBackGroudInfo(ULONG nshiftid, ULONG nBeginTm, SHORT nAutoMove, SHORT nTotalLen, SHORT nHistoryLen);
	void NewShiftBanci();
	ULONG GetLoginShiftId(void);
	ULONG GetShiftBeginTime(void);
	ULONG GetHistoryLen(void);
	ULONG GetAutoMove(void);
	BYTE DataIsExistInOCX(ULONG nTrainIndex, ULONG data_type);
	void OnSetSectionInfo(LPCTSTR xml);
	void AddBStplanStation(ULONG station);
	void UsingWorkGoodMode(void);
	void MoveTGOCXOneStep(void);
	void SetUserRight(LONG nRight);
	void AddNotMeetInfo(WORD no_meet_flag, LPCTSTR name, LPCTSTR short_name);
	void SetFormatView(void);
	BSTR CheckSideByRelation(const VARIANT& msg);
	BSTR GetTrainJiaoRelation(ULONG nTrainIndex);
public:
	enum 
	{			
		eventidTG_APPEND_MARK = 1L,
		eventidTG_UPDATE_MARK = 2L,
		eventidTG_DELETE_MARK = 3L,	

		eventidTG_APPEND_TRAIN = 4L,	
		eventidTG_UPDATE_TRAIN = 5L,	
		eventidTG_DELETE_TRAIN = 6L,

		eventidTG_LOG = 7L,
		eventidTG_SELECT_TRAIN = 8L,	
		eventidTG_SELECT_MARK = 9L,
		eventidTG_NOTICE = 10L,
		eventidTG_SET_TRAIN_DEFAULT_INFO = 11L,
		eventidTG_GEN_BLOCK_DCMD = 12L,		
		eventidTG_SET_PLOT_PARAM = 13L,
		eventidTG_FIRE_DELETE = 14L,
		eventidFIRE_REAL_TIME = 15L,
		eventidAddEarlyLateInfo = 16L,		
		eventidFireShowCrew = 17L,
		eventidFireMutiTrainOper = 18L,
	};

	// 1
	void FIRE_TG_APPEND_MARK(LONG nIndex, LPCTSTR xml)
	{
		FireEvent(eventidTG_APPEND_MARK, EVENT_PARAM(VTS_I4 VTS_BSTR), nIndex, xml);
	}
    // 2
	void FIRE_TG_UPDATE_MARK(LONG nIndex, LPCTSTR xml)
	{
		FireEvent(eventidTG_UPDATE_MARK, EVENT_PARAM(VTS_I4 VTS_BSTR), nIndex, xml);
	}
    // 3
	void FIRE_TG_DELETE_MARK(LONG nIndex, LPCTSTR xml)
	{
		FireEvent(eventidTG_DELETE_MARK, EVENT_PARAM(VTS_I4 VTS_BSTR), nIndex, xml);
	}
	
	// 4
	void FIRE_TG_APPEND_TRAIN(LONG nIndex, LPCTSTR xml)
	{
		FireEvent(eventidTG_APPEND_TRAIN, EVENT_PARAM(VTS_I4 VTS_BSTR), nIndex, xml);
	}
    // 5
	void FIRE_TG_UPDATE_TRAIN(LONG nIndex, LPCTSTR xml)
	{
		FireEvent(eventidTG_UPDATE_TRAIN, EVENT_PARAM(VTS_I4 VTS_BSTR), nIndex, xml);
	}
    // 6
	void FIRE_TG_DELETE_TRAIN(LONG nIndex, LPCTSTR xml)
	{
		FireEvent(eventidTG_DELETE_TRAIN, EVENT_PARAM(VTS_I4 VTS_BSTR), nIndex, xml);
	}
	
	// 7
	void FIRE_TG_LOG(LPCTSTR log)
	{
		FireEvent(eventidTG_LOG, EVENT_PARAM(VTS_BSTR), log);
	}
    
	// 8
	void FIRE_TG_SELECT_TRAIN(ULONG train_index, ULONG nFlag, USHORT stano1, USHORT stano2, ULONG date, ULONG time)
	{
		FireEvent(eventidTG_SELECT_TRAIN, EVENT_PARAM(VTS_UI4 VTS_UI4 VTS_UI2 VTS_UI2 VTS_UI4 VTS_UI4), train_index, nFlag, stano1, stano2, date, time);
	}

	// 9
	void FIRE_TG_SELECT_MARK(ULONG index, BYTE type, BYTE flag, USHORT stano1, USHORT stano2, ULONG param)
	{
		FireEvent(eventidTG_SELECT_MARK, EVENT_PARAM(VTS_UI4 VTS_UI1 VTS_UI1 VTS_UI2 VTS_UI2 VTS_UI4), index, type, flag, stano1, stano2, param);
	}

	// 10
	void TG_NOTICE(BYTE nType, ULONG nFlag, ULONG nIndex, USHORT nStaNo)
	{
		FireEvent(eventidTG_NOTICE, EVENT_PARAM(VTS_UI1 VTS_UI4 VTS_UI4 VTS_UI4), nType, nFlag, nIndex, nStaNo);
	}

	// 11
	//void FIRE_SET_TRAIN_DEFAULT_INFO(LPCTSTR train_xml, LONG nFlag)
	//{
	//	FireEvent(eventidTG_SET_TRAIN_DEFAULT_INFO, EVENT_PARAM(VTS_BSTR VTS_I4), train_xml, nFlag);
	//}

	// 12
	void TG_GEN_BLOCK_DCMD(LPCTSTR xml, ULONG index, ULONG type)
	{
		FireEvent(eventidTG_GEN_BLOCK_DCMD, EVENT_PARAM(VTS_BSTR VTS_UI4 VTS_UI4), xml, index, type);
	}

	// 13
	void FIRE_SET_PLOT_PARAM(LPCTSTR param_xml, LONG nFlag)
	{
		FireEvent(eventidTG_SET_PLOT_PARAM, EVENT_PARAM(VTS_BSTR VTS_I4), param_xml, nFlag);
	}

	// 14
	void FIRE_DELETE(LPCTSTR xml)
	{
		FireEvent(eventidTG_FIRE_DELETE, EVENT_PARAM(VTS_BSTR), xml);
	}
	
    // 15
	void FIRE_REAL_TIME(ULONG train_index, SHORT rec_index, ULONG adjust_flag, ULONG arrive, ULONG depart, USHORT station)
	{
		FireEvent(eventidFIRE_REAL_TIME, EVENT_PARAM(VTS_UI4 VTS_I2 VTS_UI4 VTS_UI4 VTS_UI4 VTS_UI2), train_index, rec_index, adjust_flag, arrive, depart, station);
	}

	//16
	void AddEarlyLateInfo(LPCTSTR key, BYTE val, LONG shiftid)
	{
		FireEvent(eventidAddEarlyLateInfo, EVENT_PARAM(VTS_BSTR VTS_UI1 VTS_I4), key, val, shiftid);
	}

	//17
	void FIRE_SHOW_CREW(ULONG train_index, ULONG x, ULONG y)
	{
		FireEvent(eventidFireShowCrew, EVENT_PARAM(VTS_UI4 VTS_UI4 VTS_I4), train_index, x, y);
	}

	// 18
	void FIRE_MULTI_TRAIN_OPER(LPCTSTR param_xml, LONG nFlag)
	{
		FireEvent(eventidFireMutiTrainOper, EVENT_PARAM(VTS_BSTR VTS_I4), param_xml, nFlag);
	}
	
};
