// TestTgView.h : CTgView 类的接口
//


#pragma once

#include "tgview.h"
#include "DlgElecArms.h"
#include "DlgMultiSelOperate.h"
#include "TempAddTrains.h"
#include "DlgTrainRecord.h"
#include "DlgTrainRouteCheck.h"
#include "DlgBasicSchduleCompare.h"
#include "DlgShiftSchduleCompare.h"
#include "DlgWorkSchduleCompare.h"
#include ".\predict\DepartPredictDlg.h"
#include ".\predict\TrainDepartPredictHolder.h"

class CSendStationPlanDlgEx;
class CReceiveNeighberPlanSheet;
class CPropertySheetNotepad;
class CPageNotepadPlan;
class CTgEditView : public CTgView
{
	CDlgTrainRecord *m_pTrainSideDlg;
	CDlgTrainRouteCheck *m_pTrainRouteDlg;
	CDlgWorkSchduleCompare* m_pWorkSchdCompareDlg;
	CDlgShiftSchduleCompare *m_pShiftSchdCompareDlg;
	CDlgBasicSchduleCompare *m_pBasicSchdCompareDlg;
	CSendStationPlanDlgEx *m_pSendPlanDlg;
	CPropertySheetNotepad  *m_pNotepadDlg;
	CDepartPredictDlg* m_predictDlg;
	CTrainDepartPredictHolder *m_pPredictServer;
	CDlgElecArms           m_DlgArms;
	enum{
		OPER_STATUS_IDLE=0,
		OPER_STATUS_PASSWAY=1
	};
	struct PasswayDrawInfo
	{
		int        nOperStatus;
		TRAIN_INDEX nTrainIndex;
		TRAIN_NO   szArriveTrainNo;
		TRAIN_NO   szTrainNo;
		int          nRouteNo;
		BYTE       nStartMode;
		BYTE       nEndMode;
		BYTE       nEngineCount;
		char       engineId[10][12];
		DWORD      status[10];
		bool       bSelSecret;
		BYTE       nRunType;
		PasswayDrawInfo()
		{
			nRunType=255;
		}
	}m_currentOper;

	XMLInform  m_xmlinform;

protected: // 仅从序列化创建
	CTgEditView();
	DECLARE_DYNCREATE(CTgEditView)

// 实现
public:
	virtual ~CTgEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
protected:
	struct LateInfo
	{
		BYTE flag;
		ULONG shiftid;
		LateInfo()
		{
			flag=0;
			shiftid=0;
		}
		LateInfo(const LateInfo& rhs)
		{
			flag=rhs.flag;
			shiftid=rhs.shiftid;
		}
	};

	std::map<CString, LateInfo> m_earlylatetimelog;
	void SetLateEarlyTimeRd(LPCTSTR xml, BYTE val, ULONG shiftid);
	bool LoadEarlylatetimeXML();
	void SaveEarlylatetimeFile();
	void MutiTrainOper(LPCTSTR xml, ULONG nOperFlag);
// 生成的消息映射函数
protected:
	void ReqParament();
	DECLARE_MESSAGE_MAP()

	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) ;
	void OnDisplaySideReqChangeDlg(CLBTH::MsgChangeSideReq *pChangeSideReq, int station);
public:
	DECLARE_EVENTSINK_MAP()
	void APPEND_MARK(long nIndex, LPCTSTR xml);
	void UPDATE_MARK(long nIndex, LPCTSTR xml);
	void DELETE_MARK(long nIndex, LPCTSTR xml);
	void APPEND_TRAIN(long nIndex, LPCTSTR xml);
	void UPDATE_TRAIN(long nIndex, LPCTSTR xml);
	void TG_DELETE_TRAIN(long nIndex, LPCTSTR xml);
	void TG_LOG(LPCTSTR log);
	void SELECT_TRAIN(long train_index, long nFlag,short stano1, short stano2, long  date, long time);
	void SELECT_MARK(long mark_index, char type, char flag,short  stano1, short  stano2, long param);
	void TG_NOTICE(BYTE type, long nFlag, long nIndex,short station);
	
	void TG_FIRE_DELETE(LPCTSTR xml); 
	BOOL CheckUserTGRight();
public:
	void OnAddTmpTrainPassway(TmpTrainInfo* info);
	//BOOL  SwitchSchedule(int con_id,int schd_type,TIME starttime,TIME endtime);
	virtual void OnInitialUpdate();	
	virtual BOOL  InitScheduleView(CTG_ScheduleInfo::SCHEDULE_TYPE type,const char *datapath,BOOL query_mode);
public:
	afx_msg void OnAddTmpTrain();
	afx_msg void OnInsertNewTrain();
	afx_msg void OnEndInsertNewTrain();
	afx_msg void OnManualAddNewTrain();

	afx_msg void OnDelMultiTrain();
	
	afx_msg void OnAbsInhibitG();
	afx_msg void OnAbsSlowG();
	afx_msg void OnAbsEleFixG();
	afx_msg void OnAbsFixG();
	afx_msg void OnSelfCloseG();
	afx_msg void OnDrawGoon();
	afx_msg void OnDrawReturn();
	afx_msg void OnDrawAbstrainA();
	afx_msg void OnDrawAbstrainB();
	
	afx_msg void OnSideInhibitG();
	afx_msg void OnStationSlowG();
	afx_msg void OnSideTianchuangG();
	afx_msg void OnSideOccupy();
	afx_msg void OnTextNote();
	afx_msg void OnJishiZhujie();
	afx_msg void OnUniteTrain();
	afx_msg void OnRescueTrain();
	afx_msg void OnSplitTrain();
	afx_msg void OnCombineTrain();
	afx_msg void OnAbsPartRun();
	afx_msg void OnAttainEngineReturn();
	
	afx_msg void OnBypassFlag();
	afx_msg void OnShowWaveLine();
	afx_msg void OnShowErrorTag();
	afx_msg void OnShowTractionengine();
	afx_msg void OnShowEarlylateTime();
	afx_msg void OnShowTime();
	afx_msg void OnShowDistanceStone();
	afx_msg void OnShowConflict();
	afx_msg void OnShowReverse();
	afx_msg void OnSendStationPlan();
	afx_msg void OnSendNeighber();
	afx_msg void OnRecvNeigbourPlan();
	afx_msg void OnNeighberPlan();
	afx_msg void OnManualReportHistory();
	afx_msg void OnManualSelectBasicTrain(); 
	afx_msg void OnManualSelectShiftTrain();
	afx_msg void OnManualSelectTrain();
	afx_msg void OnManualSelectShiftMark();
	afx_msg void OnManualSelectBasicMark(); 
	afx_msg void OnDestroy();

	void OnGetBasicTrainFromOpms();
	void OnGetTrainList();
	void OnGetMarkList();
	void OnRecvTrainList();
	void OnRecvMarkList();
	void OnShiftLogin();
	void OnDeleteTrain();
	afx_msg void OnGetEngineTicketInfo();
	afx_msg void OnUpdateGetEngineTicketInfo(CCmdUI *pCmdUI);
	afx_msg void OnShiftGetShiftinfo();
	afx_msg void OnUpdateShiftGetShiftinfo(CCmdUI *pCmdUI);
	afx_msg void OnHatTrain();
	afx_msg void OnElecArmsSet();
	afx_msg void OnUpdateElecArmsSet(CCmdUI *pCmdUI);
	afx_msg void OnUpdateManualOperCheck(CCmdUI *pCmdUI);
	afx_msg void OnManualOperCheck();
	afx_msg void OnUpdateOperTGOPMS(CCmdUI *pCmdUI);
	afx_msg void OnUpdateOperTGMark(CCmdUI *pCmdUI);
	afx_msg void OnUpdateOperTGTrain(CCmdUI *pCmdUI);
	afx_msg void OnUpdateOperAddWorkTrain(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSaveBasicSideWorkInfo(CCmdUI *pCmdUI);
	
	afx_msg void OnUpdateSendPlan(CCmdUI *pCmdUI);
	afx_msg void OnDeleteAllShiftTrain();
	afx_msg void OnDeleteAllShiftMark();

	afx_msg void OnBackOper();
	afx_msg void OnUpdateBackOper(CCmdUI *pCmdUI);
	afx_msg void OnChangeTrainRoute();
	
	afx_msg void OnSaveToFiles();
	afx_msg void OnLoadFromFiles();
	afx_msg void OnDeleteAllBasic();

	void OnDeleteBasicTrains(int type);
	afx_msg void OnUpdateDeleteBasic(CCmdUI *pCmdUI);

	afx_msg void OnBasicGraphchange();
	afx_msg void OnUpdateBasicGraphchange(CCmdUI* pCmdUI);

	afx_msg void OnModifyBschd();
	afx_msg void OnUpdateModifyBschd(CCmdUI* pCmdUI);

	afx_msg void OnBasicNameing();
	afx_msg void OnUpdateBasicNameing(CCmdUI* pCmdUI);

	afx_msg void OnBasicDeleteName();
	afx_msg void OnUpdateBasicDeleteName(CCmdUI* pCmdUI);

	afx_msg void OnTrainPlanSideMan();
	afx_msg void OnUpdateTrainPlanSideMan(CCmdUI* pCmdUI);

	afx_msg void OnTrainPlanRouteMan();
	afx_msg void OnUpdateTrainPlanRouteMan(CCmdUI* pCmdUI);

	void OnTurnReality();
	void OnReceivePoint();
	void OnUpdateReceivePoint(CCmdUI* pCmdUI);

	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam); 
	afx_msg void OnBasicGraphsavetotext(); 
	void  OnUpdateEnableFreeAddTrain(CCmdUI *pCmdUI);
	void  OnEnableFreeAddTrain();
	void  OnUpdateGenNodeIndex(CCmdUI *pCmdUI);
	void  OnGenNodeIndex();
	void  OnDirectUpdateFromOPMS();
	void  OnUpdateOperRestore(CCmdUI *pCmdUI);
	afx_msg void OnDivideportLatetime();
	afx_msg void OnDeleteTrainByIndex();
	afx_msg void OnDeleteMark();
	afx_msg void OnDeleteInvalidTrain();

	afx_msg void OnSaveBasicSideWorkInfo();
	afx_msg void OnLoadBasicSideWorkInfo();

	afx_msg void OnPredictStpc();

	afx_msg void OnBatchAllocationSide();
	afx_msg void OnUpdateBatchAllocationSide(CCmdUI* pCmdUI);
private:
	bool   m_bEnableProcessTdmsShiftInfo;
	bool   m_bEnableProcessTdmsBasicInfo;
	BOOL   m_bEnableFreeAddTrain;
	CTG_TrainScheduleBase m_BaseOfBasicTmp; // 挑选TDMS基本图生成预览图时,临时保存列车
	bool   m_bSelTDMSTrainGenBasic; // 挑选TDMS基本图生成预览图
	std::vector<DWORD> recieved_tdms_trains;// 检查日班计划,预览基本图是否丢线

	BOOL   RequestBasicScheduleFromOPMS(int type=0);// type==1 常用基本图   type==0 预览其本图
	BOOL   ProcessXML_BasicScheduleTrain(const char *xml,const char *node);
	BOOL   ProcessXML_ShiftTrain(const char *xml,const char *node);
	BOOL   RequestShiftScheduleFromOPMS(TIME starttm,TIME endtm);
	void   DirectSetAutotrig(CTG_SingleTrain& train, const int rec_index);
	BOOL   ProcessXML_TrainCrew(const char *xml,const char *node);
	BOOL   ProcessXML_YingyeKediao(const char *xml, const char *node);
	BOOL   ProcessXML_Alert(const char *xml,const char *node);

	BOOL   ProcessXML_Kediao(const char *xml, const char *node);
	BOOL   ProcessXML_Yingye(const char *xml, const char *node);
	BOOL   ProcessXML_Ticket(const char *xml,const char *node);
	BOOL   ProcessXML_Crew(const char *xml,const char *node);
	BOOL   ProcessXML_TrainEngineCrew(const char *xml,const char *node);

	CTG_TrainScheduleBase* GetTrainScheduleBase();
	void AddTdmsTrainGuid(DWORD guid);
	void SingleTrainToWork();
	void BatchManualModifySide(TRAIN_INDEX train_index);
public:
	afx_msg void OnSendNeigbourPlan();
	
	afx_msg void OnMutiTrainMove();
	void InsertNewTrain();
	void InsertNewTrainWithEngineNo();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnAutoAdjustCol();
	afx_msg void OnAutoAdjustPass();
	afx_msg void OnOptAutoAdjustCollision();
	afx_msg void OnUpdateOptAutoAdjustCollision(CCmdUI *pCmdUI);
	afx_msg void OnAutoAdjustMulti();
	afx_msg void OnDirectSetAutotrig();
	afx_msg void OnUpdateDirectSetAutotrig(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);

	afx_msg void OnEditAllCzh();
	afx_msg void OnUpdateEditAllCzh(CCmdUI *pCmdUI);
	afx_msg void OnGenAllReturnTrainid();
	afx_msg void OnUpdateGenAllReturnTrainid(CCmdUI *pCmdUI);
	
	afx_msg void OnModifyShiftschd();
	afx_msg void OnUpdateModifyShiftschd(CCmdUI* pCmdUI);

	afx_msg void OnSetMultiSideLock();
	afx_msg void OnUpdateSetMultiSideLock(CCmdUI* pCmdUI);
	afx_msg void OnQuickModifyMultiTrains();
	
	afx_msg void OnSelTdmsTrainsGenBasic();
	afx_msg void OnUpdateSelTdmsTrainsGenBasic(CCmdUI* pCmdUI);
	
	afx_msg void OnShiftTrainToWork();
	afx_msg void OnUpdateShiftTrainToWork(CCmdUI* pCmdUI);

	afx_msg void OnBasicTrainToWork();
	afx_msg void OnUpdateBasicTrainToWork(CCmdUI* pCmdUI);

	afx_msg void OnTrainPlanCompareMan();
	afx_msg void OnUpdateTrainPlanCompareMan(CCmdUI* pCmdUI);
private:
	CStringArray tdms_ctc_alarm; //用于存放tdms数据转换成ctc数据时的报警信息
	SINGLE_TRAIN_GEN_TO_WORK m_singleTrainToWork;
};


#ifndef _DEBUG  // TestTgView.cpp 的调试版本
inline CTgDoc* CTgEditView::GetDocument() const
   { return reinterpret_cast<CTgDoc*>(m_pDocument); }
#endif

