// TestTgView.h : CTgView 类的接口
//


#pragma once

#include "tgx.h"
#include "TgDocument.h"
#include "loadgpcdef.h"
#include "schd_tgmmi.h"
#include "if.h"
#include "DlgDataXMLShow.h"
#include "ToolTipWndEx.h"

struct EngineInfoTag
{
	BYTE type;
	char engine_no[20];
};

class CTgView : public CView
{
public:
	enum{IDC_TGCTRL_VIEW=1000};
protected: // 仅从序列化创建
	CTgView();
	DECLARE_DYNCREATE(CTgView)

// 属性
public:
	CTgDocument* GetDocument() const;

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
// 实现
public:
	virtual ~CTgView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

protected:
	CTgX mTG;
	CWintgViewMMI m_tgmmi;
	CToolTipWndEx m_pCfWnd;  // 显示乘务信息
protected:
	CTG_ScheduleInfo m_schdinfo;
  
	bool  m_bSelfNextSTDState; // 调整标志
	int   m_nTrainLate;		   // 晚点标志
	
	void  GetTitle(CString &title);
	BOOL  m_bCanClose;
	int   m_timeCount;
	CTime nNosync_with_svr_LastTime;
	CTime nNosync_with_ocx_LastTime;
	int m_nTimeoutSecond;
public:
	void SetTitleUpdateTimer() {m_timeCount=0;SetTimer(990, 2000, NULL);}
	void BeginLoad() {mTG.BeginLoadData();}
	void EndLoad() {mTG.EndLoadData();}
	void SetCanClose(BOOL va) {m_bCanClose=va;}
	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, int cx, int cy);
	void OnDestroy();
	void OnDirectRangeChg(UINT nID);
	void OnUpdateDirectRange(CCmdUI* pCmdUI);
	void OnSelectTrainRange(UINT nID);
	void OnUpdateTrainRange(CCmdUI* pCmdUI);
	void OnBig();
	void OnSmall();
	
	void OnSelfNextSTD();
	void OnUpdateSelfNextSTD(CCmdUI* pCmdUI);
	
	void OnModifyAtcual();
	void OnUpdateModifyAtcual(CCmdUI* pCmdUI);

	void OnTrainLate(UINT nID);
	void OnUpdateTrainLate(CCmdUI* pCmdUI);

	void OnShowReverse(UINT nID);
	void OnUpdateShowReverse(CCmdUI *pCmdUI);

	afx_msg void OnZoomOutRange(UINT nID);
	afx_msg void OnZoomInRange(UINT nID);
	
	void OnSavePicture();
	void OnTimer(UINT idevent);
	void OnDCMD();
	LRESULT OnObjectChanged(WPARAM wp,LPARAM lp);
	void OnFindTrain();
		
	void OnQueBao();
	void OnJiaoBan();
	void OnSendPhaseNotePlan();
	void OnShigu();
	void OnNote();
	void OnStationCurrentTrains();
	void OnSUBAO();
	void OnShuaiGua();
	void GEN_BLOCK_DCMD(LPCTSTR content, long nIndex, long type);
	void PLOT_PARAM(LPCTSTR content, long nFlag);
	LRESULT OnRecievedDataEnd(WPARAM wp,LPARAM lp);
	LRESULT OnSetTrainFoucs(WPARAM wParam, LPARAM lParam);
	void OnShowDataXML();
	bool  m_bDyncView;
protected:
	void ShiftBanCi();
	void ShiftBanCiNew();
	void LogFile(CString filepath,CString log);
	BOOL m_bUserMemDC;
	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
public:
	int m_nPlotFlag;
	int mServer_nosync_count, mDatabase_nosync_count, mMmi_nosync_count;
public:
	virtual void OnInitialUpdate();
	DECLARE_EVENTSINK_MAP()
	void TG_LOG(LPCTSTR log);
	void SELECT_TRAIN(long train_index, long nFlag,short stano1, short stano2, long  date, long time);
	void SELECT_MARK(long mark_index, char type, char flag,short  stano1, short  stano2, long param);
	void TG_NOTICE(BYTE type, long nFlag, long nIndex,short station);
	void ShowCrewInfo(ULONG nTrainIndex, long x,long y);
public:
	virtual BOOL  InitScheduleView(CTG_ScheduleInfo::SCHEDULE_TYPE type,const char *datapath,BOOL query_mode);
	void   OnUpdateTitle();
	void   RefreshViewData();
	int    GetViewSectionNo() {return m_schdinfo.m_nSectionID;}
	CTG_ScheduleInfo::SCHEDULE_TYPE   GetViewScheduleType() {return m_schdinfo.m_nType;}
	int    GetViewShiftID() {return m_schdinfo.m_nShiftID;} 
	void   GetViewScheduleInfo(CTG_ScheduleInfo &info){info=m_schdinfo;}
	BOOL   CloseView();
	CWintgViewMMI *GetMMIInterface() {return &m_tgmmi;}
	const char *GetSyncStatusInfo();
	const char *GetCommStatusInfo();
public:
	void OnTrainEngineInfo(int rec_index);
	afx_msg void OnTrainSubao();
	afx_msg void OnTrainBianzudan(int rec_index);

	afx_msg void OnButtonLogin();
	afx_msg void OnChangePassword();
	afx_msg void OnPlot();
	afx_msg void OnPlot18_06();
	afx_msg void OnPlot06_18();
	

	afx_msg void OnDispLeapTrainNone();
	afx_msg void OnUpdateDispLeapTrainNone(CCmdUI* pCmdUI);
	afx_msg void OnDispLeapTrainJierujiaochu();
	afx_msg void OnUpdateDispLeapTrainJierujiaochu(CCmdUI* pCmdUI);
	afx_msg void OnDispLeapTrainLink();
	afx_msg void OnUpdateDispLeapTrainLink(CCmdUI* pCmdUI);
	
	afx_msg void OnDispBegin18();
	afx_msg void OnDispBegin12();

	afx_msg void OnActivateFrame(UINT nState,CFrameWnd *pwnd);
	afx_msg void OnRefreshTgData();
	afx_msg void OnUpdateWholeScreen(CCmdUI* pCmdUI);
	afx_msg void OnWholeScreen();

	afx_msg void OnBasicTrainMoveWhole();
	afx_msg void OnUpdateBasicTrainMoveWhole(CCmdUI* pCmdUI);
	
	afx_msg void OnJiaojiechestatistic();
	afx_msg void OnQueryDDYName();

	afx_msg void OnPredictHistroyView();

private:
	void CheckSyncAlarm();	
	char nosync_buf[512];
};
