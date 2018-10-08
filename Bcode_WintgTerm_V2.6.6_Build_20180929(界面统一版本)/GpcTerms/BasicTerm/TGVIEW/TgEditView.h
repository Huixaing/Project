// TestTgView.h : CTgView 类的接口
//


#pragma once

#include "tgview.h"
#include "DlgMultiSelOperate.h"
#include "DlgTrainRecord.h"
#include "DlgTrainRouteCheck.h"
#include "DlgBasicSchduleCompare.h"
#include "DlgEditTrainSideYingye.h"

class CTgEditView : public CTgView
{
	CDlgTrainRecord *m_pTrainSideDlg;
	CDlgTrainRouteCheck *m_pTrainRouteDlg;
	CDlgBasicSchduleCompare *m_pBasicSchdCompareDlg;
	CDlgEditTrainSideYingye *m_pSideYingyeDlg;
	
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

	void MutiTrainOper(LPCTSTR xml, ULONG nOperFlag);
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) ;
public:
	DECLARE_EVENTSINK_MAP()
	
	void APPEND_TRAIN(long nIndex, LPCTSTR xml);
	void UPDATE_TRAIN(long nIndex, LPCTSTR xml);
	void TG_DELETE_TRAIN(long nIndex, LPCTSTR xml);
	void SELECT_TRAIN(long train_index, long nFlag,short stano1, short stano2, long  date, long time);
	void TG_FIRE_DELETE(LPCTSTR xml);
public:
	virtual void OnInitialUpdate();	
	virtual BOOL InitScheduleView(CTG_ScheduleInfo::SCHEDULE_TYPE type,const char *datapath,BOOL query_mode);
public:
	afx_msg void OnInsertNewTrain();
	afx_msg void OnEndInsertNewTrain();

	afx_msg void OnDelMultiTrain();
	
	afx_msg void OnDrawGoon();
	afx_msg void OnDrawReturn();
	afx_msg void OnDrawAbstrainA();
	afx_msg void OnDrawAbstrainB();
		
	afx_msg void OnBypassFlag();
	afx_msg void OnShowWaveLine();
	afx_msg void OnShowErrorTag();
	
	afx_msg void OnShowTime();
	afx_msg void OnShowDistanceStone();
	afx_msg void OnShowConflict();
	afx_msg void OnShowReverse();
	afx_msg void OnDestroy();


	afx_msg void OnDeleteAllBasic();

	afx_msg void OnBasicGraphchange();

	afx_msg void OnModifyBschd();

	afx_msg void OnBasicNameing();

	afx_msg void OnBasicDeleteName();

	afx_msg void OnTrainPlanSideMan();
	afx_msg void OnTrainPlanRouteMan();
	afx_msg void OnSideYingyeEdit();
	afx_msg void OnTrainPlanCompareMan();
	afx_msg void OnUpdateEnableEdit(CCmdUI* pCmdUI);

	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam); 
	
	afx_msg void  OnUpdateGenNodeIndex(CCmdUI *pCmdUI);
	afx_msg void  OnGenNodeIndex();
	
	afx_msg void OnDeleteTrainByIndex();
	afx_msg void OnDeleteInvalidTrain();

	afx_msg void OnLoadBasicSideWorkInfo();

private:
	bool   m_bEnableProcessTdmsBasicInfo;
	BOOL   m_bEnableFreeAddTrain;
	CTG_TrainScheduleBase m_BaseOfBasicTmp; // 挑选TDMS基本图生成预览图时,临时保存列车
	
	std::vector<DWORD> recieved_tdms_trains;// 检查日班计划,预览基本图是否丢线

	BOOL   RequestBasicScheduleFromOPMS(int type=0);// type==1 常用基本图   type==0 预览其本图
	BOOL   ProcessXML_BasicScheduleTrain(const char *xml,const char *node);
		
	void AddTdmsTrainGuid(DWORD guid);
	void OnDeleteBasicTrains(int type);
	void OnGetBasicTrainFromOpms();
	
public:
	void InsertNewTrain();
	void InsertNewTrainWithEngineNo();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	afx_msg void OnTimer(UINT nIDEvent);

	afx_msg void OnEditAllCzh();

	afx_msg void OnGenAllReturnTrainid();
		
	afx_msg void OnSelTdmsTrainsGenBasic();

private:
	CStringArray tdms_ctc_alarm; //用于存放tdms数据转换成ctc数据时的报警信息
};


#ifndef _DEBUG  // TestTgView.cpp 的调试版本
inline CTgDoc* CTgEditView::GetDocument() const
   { return reinterpret_cast<CTgDoc*>(m_pDocument); }
#endif

