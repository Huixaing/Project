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
#include "DlgRestoreTrainList.h"

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
	
protected:
	CTG_ScheduleInfo m_schdinfo;
  
	void  GetTitle(CString &title);
	BOOL  m_bCanClose;
	int   m_timeCount;
	bool  m_bSelTDMSTrainGenBasic; // 挑选TDMS基本图生成预览图
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
	
	void OnTrainLate(UINT nID);
	void OnUpdateTrainLate(CCmdUI* pCmdUI);

	void OnShowReverse(UINT nID);
	void OnUpdateShowReverse(CCmdUI *pCmdUI);

	afx_msg void OnZoomOutRange(UINT nID);
	afx_msg void OnZoomInRange(UINT nID);
	
	void OnSavePicture();
	void OnTimer(UINT idevent);
	
	LRESULT OnObjectChanged(WPARAM wp,LPARAM lp);
	void OnFindTrain();
	
	void PLOT_PARAM(LPCTSTR content, long nFlag);
	LRESULT OnRecievedDataEnd(WPARAM wp,LPARAM lp);
	LRESULT OnSetTrainFoucs(WPARAM wParam, LPARAM lParam);
	void  OnShowDataXML();
	
protected:
	void LogFile(CString filepath,CString log);
	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	BOOL CheckUserTGRight();

public:
	int m_nPlotFlag;
	int mServer_nosync_count, mMmi_nosync_count;
public:
	virtual void OnInitialUpdate();
	DECLARE_EVENTSINK_MAP()
	void TG_LOG(LPCTSTR log);
	void SELECT_TRAIN(long train_index, long nFlag,short stano1, short stano2, long  date, long time);
public:
	virtual BOOL  InitScheduleView(CTG_ScheduleInfo::SCHEDULE_TYPE type,const char *datapath,BOOL query_mode);
	void   OnUpdateTitle();
	void   RefreshViewData();
	int    GetViewSectionNo() {return m_schdinfo.m_nSectionID;}
	CTG_ScheduleInfo::SCHEDULE_TYPE   GetViewScheduleType() {return m_schdinfo.m_nType;}
	
	void   GetViewScheduleInfo(CTG_ScheduleInfo &info){info=m_schdinfo;}
	BOOL   CloseView();
	CWintgViewMMI *GetMMIInterface() {return &m_tgmmi;}
	const char *GetSyncStatusInfo();
	const char *GetCommStatusInfo();
	CString GetSchdName();

public:
	afx_msg void OnSaveToFiles();
	afx_msg void OnLoadFromFiles();

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

	afx_msg void OnRefreshTgData();
	afx_msg void OnUpdateWholeScreen(CCmdUI* pCmdUI);
	afx_msg void OnWholeScreen();

	afx_msg void OnBasicTrainMoveWhole();
	afx_msg void OnUpdateBasicTrainMoveWhole(CCmdUI* pCmdUI);
	
	afx_msg void OnUpdateOperTGTrain(CCmdUI *pCmdUI);
	afx_msg void OnBasicGraphsavetotext(); 

	afx_msg void OnUpdateSaveBasicSideWorkInfo(CCmdUI *pCmdUI);
	afx_msg void OnSaveBasicSideWorkInfo();

	afx_msg void OnPlanCompareStationSet();
	afx_msg void OnSetSideCalTrainType();
	afx_msg void OnSetStplanNoWarningStation();

	afx_msg void OnUpdateEditAllCzh(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGenAllReturnTrainid(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSelTdmsTrainsGenBasic(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOperAddWorkTrain(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDeleteBasic(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicGraphchange(CCmdUI* pCmdUI);
	afx_msg void OnUpdateModifyBschd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBasicNameing(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBasicDeleteName(CCmdUI* pCmdUI);
};
