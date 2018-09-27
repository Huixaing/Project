#if !defined(AFX_WINTGFORM_H__B94FD0D1_7C1F_11D1_9CF0_000021000CE6__INCLUDED_)
#define AFX_WINTGFORM_H__B94FD0D1_7C1F_11D1_9CF0_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WintgForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTGForm form view
 
#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "dlgwait.h"
#include "tgtype.h"
#include "tgdoc.h"
#include "ToolTipWnd.h"	
#include "BitmapFile.h"
#include "sys\timeb.h"

struct YTag
{
	int nTopY;
	int nBottomY;
};

struct MenuNode
{
	int  type;
	int  parent_id;
	int  id;
	char text[100];
	MenuNode()
	{
		type = 0;
		parent_id = 0;
		id = 0;
		memset(text, 0, sizeof(text));
	}
};

struct SelCzhTrainInfo
{
	int mLeftStation;
	int mRightStation;
	long mLeftTime;
	long mRightTime;
	CString mStrLeftTrainID;
	CString mStrRightTrainID;
	
	std::vector<CTG_TRAIN> m_focusCzhTrains;

	SelCzhTrainInfo()
	{
		Init();
	}

	void Init()
	{
		mLeftStation=0;
		mRightStation=0;
		mLeftTime=0;
		mRightTime=0;
		mStrLeftTrainID="";
		mStrRightTrainID="";
		m_focusCzhTrains.clear();
	}
};

class  CTGForm : public CScrollView
{
protected:
	CTGForm();
	DECLARE_DYNCREATE(CTGForm)

	int m_nStartMode,m_nEndMode,m_nSecretTrain,m_nRunType;
    CString m_strTrainId, m_strArrivalTrainId, m_strRetTrainId;
	void AutoChangeTrainID(CTG_TRAIN& single); 
   
 	CPoint m_pContinuePoint;
    CPoint m_pOldMousePoint;
    int m_nSelectedStationList[20];
    int m_nSelectedNodeIndexList[20];
 	
 // Attributes
public:
	TgTRAIN_RECORD* GetFoucsTrainRecord();

public:
    CRect m_rectOrgRec;

    CMenu hResourceMenu;
    CMenu *hPopupMenu;
	
	HCURSOR mHReceivePoint;
    CTGDoc *m_pDoc;
	std::vector<CTG_TRAIN>  m_sFocusOtherTrain;
  	CTG_TRAIN *m_sFocusTrain;
 	CTg_BLOCK *m_nFocusFigure;
	ULONG m_nFocusTrainIndex;
	ULONG m_nFocusBlockIndex;

	int m_nFocusRectIndex;
	int m_nFocusRecordIndex;
	//定期存成图片
	CBitmapFile m_bmpFile;
 
 // Operations
public:	

	void OnDrawNewTrainEnd();
	void RefreshRect(const CRect& rect, BOOL bErase = TRUE);
	CToolTipWnd m_pCfWnd;
	int m_nSubMarkType;

    void OnManualSelectBasicTrain();

	void OnDrawGoon();
	void OnDrawReturn();
	void OnTest();
	void CreatNoearlylatetimeFolder();//added by zhaoliang for no earlylatetime
	void CreatNoearlylatetimeFile();
	void DeleteNoearlylatetimeFile();
	void ShowNoearlylatetime();
	
 	bool FindTrain();
	CString m_strTmpTrain1,m_strTmpTrain2;
	CPoint m_nOldPoint;
	bool m_bCaptured;
	
	
	BOOL bShowTrainMenu;
	WORD m_nMarkType;
	
	WORD m_nMarkType4Inhibit; //0表示正常的区间封锁，1表示轻型车辆
 
    bool m_bFirstPoint,m_bSecondPoint;
	void DrawBox (CDC &dc,CRect &rect, BOOL xor = TRUE);
	void DrawLine (CDC &dc,CPoint &first,CPoint &second,BOOL xor = TRUE);
	
	void Redraw();
	void RedrawLayer();
	void RedrawLayer2();
	
	int m_nMouseDownStationNo;
	CPoint m_nMouseCurDownPoint;
		
	int m_bMouseDown;
	int m_nMouseDownPosition;
    BYTE m_InitColor;
	
private:
	CDC     m_Layer1;
	CBitmap m_bitmap1;
	CBitmap *m_pBitmapLayer1;

	CDC     m_Layer2; // 下行车
	CBitmap m_bitmap2;
	CBitmap *m_pBitmapLayer2;
	
	CDC     m_Layer3; // 上下行车
	CBitmap m_bitmap3;
	CBitmap *m_pBitmapLayer3;
	
	CDC     m_Layer4; // 上行车
	CBitmap m_bitmap4;
	CBitmap *m_pBitmapLayer4;

	CString m_strTimeString;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTGForm)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ShowTipMsg(CPoint pt, CString msg);
	
	void OnChangeArrivalDepartTime(WORD nMinute);
	CRect m_absDragRect, m_absTmpDragRect;
	CRect m_TrainsSelectedRect;
	
	virtual ~CTGForm();
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CTGForm)
 	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnAddNewTrainGraph(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnAddNewTrainGraphXml(WPARAM wParam,LPARAM lParam);
	
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
 	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	
	afx_msg void OnDestroy();
	afx_msg void OnAddtrainAbsStop();
	afx_msg void OnAddtrainStop();
    afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMarkProperty();
	afx_msg void OnMarkTimespan(); //太原 修改施工符号起止时间

	afx_msg void OnLockStationSide();
	afx_msg void OnLockTrainSide();
	
	afx_msg void OnPressureAfter();
	afx_msg void OnPressureBefore();
	
	afx_msg void OnTrainData();
	afx_msg void OnContinueDraw();
	afx_msg void OnDepartByPlan();
	afx_msg void OnDepartByMeet();
	afx_msg void OnStopDraw();
	afx_msg void OnChangeTracks(UINT message);
	afx_msg void OnAutoGenDCmd(UINT message);
	afx_msg void OnChangeTrack();
	afx_msg void OnBatchChangeTrack();
	afx_msg void OnSetInhibitTrainPass();
	afx_msg void OnChangeTrainid();
	afx_msg void OnStopAbs();
	afx_msg void OnStopAbsCancel();
	afx_msg void OnStopSignal();
	afx_msg void OnStopSignalCancel();
	afx_msg void OnTransportInfo();
	afx_msg void OnCannotMeet();
	afx_msg void OnFindDDML();
	afx_msg void OnCancelWork();
	afx_msg void OnQueryQuebao();
	afx_msg void OnMutiTrainRecStayToPass();
	afx_msg void OnTrainWorkInfo();
	afx_msg void OnTrainSubaoInfo();
	afx_msg void OnTrainBianzuInfo();
	afx_msg void OnManageShuaigua();
	afx_msg void OnChangeRoute();
	afx_msg void OnMarkWorkInfo();
	afx_msg void OnModifyPlanPara();
	afx_msg void OnTrainAttribute();
	afx_msg void OnStayToPass();
	afx_msg void OnPassToStay();
	afx_msg void OnAddtrainDeliver();
	afx_msg void OnAddtrainCancel();

	afx_msg void OnTrainEngineInfo();
	
	afx_msg void OnTrainsInSameCZHShow();
	afx_msg void OnTrainCZHEdit();
	afx_msg void OnTrainCZHJiechu();
	afx_msg void OnGenReturnTrainIdByCZH();

	afx_msg void OnMultiTrainsCZHEdit();
	afx_msg void OnGenTrainReturnTrainIDAll();

	afx_msg void OnShowBasicPlanInfo();
	afx_msg void OnDirectSetTrigeRoute();
	
	afx_msg void OnDeleteTrain();
	afx_msg void OnReplaceTrainid();
	afx_msg void OnStatisticArrival();
	afx_msg void OnStatisticDepart();
	afx_msg void OnStatisticBoth();
	afx_msg void OnNoStatistic();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void ManageGuaJiChe();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnStopDepart();
	afx_msg void OnMarkContinueLining();
	afx_msg void OnStopTemp();
	afx_msg void OnMiddleDivTrain();
	afx_msg void OnReplaceHattrainid();
	afx_msg void OnStatisticWhole();
	
	afx_msg void OnMarkCopy();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnStopDrawUp();
	afx_msg void OnStopDrawMiddile();
	afx_msg void OnStopDrawBottom();
	afx_msg void OnTechStop( );
	
	afx_msg void OnMarkAutoMoveStart();
	afx_msg void OnMarkAutoMoveStop();
	afx_msg void OnSetTGColor();
	afx_msg void OnRefusedTrain();
	afx_msg void OnMiddleStart();
	afx_msg void OnMiddleFetch();
	afx_msg void OnDeleteAbs();

	afx_msg void OnOnlyShowMark();
	afx_msg void OnOnlyShowTrain();
	afx_msg void OnShowMarkAndTrain();

	afx_msg void OnSetMarkActiveStatus(UINT nID);

	afx_msg void ShowUpDirectTrain();
	afx_msg void ShowDownDirectTrain();
	afx_msg void ShowDualDirectTrain();

	afx_msg void ShowPassagerTrain();
	afx_msg void ShowGoodsTrain();
	afx_msg void ShowAllTrain();

	afx_msg void OnExitDeadState();
	afx_msg void OnModifyTrainIndex();
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnAutoAjustTrain(UINT nID);
	afx_msg void OnBlockAdjustTrain();
	afx_msg void OnDrawAbsWorkTrain(UINT nID);

	afx_msg void OnInStationReturn();
	afx_msg void OnOutStationReturn();
	afx_msg void OnNotStationReturn();
	afx_msg void OnNotStartReturn();
	afx_msg void OnKKBasicSide();
	afx_msg void OnKKBasicEntry();
	afx_msg void OnShowSingleTrain();  	// 设置单趟车模式
	//}}AFX_MSG
	

	 	 
	afx_msg LRESULT OnMenuAddMarkGraph(WPARAM wParam,LPARAM lParam);

	
	afx_msg LRESULT OnTrainsOperator(WPARAM wParam,LPARAM lParam); 
	
	DECLARE_MESSAGE_MAP()
private:
	BOOL AddNewTrainGraphCommon();
	void OperateTrainsSecleted(void);
	void DrawWorkTrain(CString dlgText, int flag);
	BOOL newTrainSimple(CPoint point,int insert_type);
	BOOL newTrainSimple_ABS(CPoint point,int insert_type);
	BOOL newTrainSimple_ABS_B(CPoint point,int insert_type);
	void WarningStplan(TgTRAIN_RECORD& lastRec);

	int  m_lastUpdateMinute;
	int  m_lastUpdateHour;
	bool m_stop_train_abs;
	int  m_stop_train_abs_direct;
	WORD m_stop_train_abs_time;
public:
	afx_msg void OnSetDeleteMultiTrain();
	afx_msg void OnSetSelMultiTrain();
	afx_msg void OnQuickModifyMultiTrain();
	afx_msg void OnShowBeginTimePoint6();
	afx_msg void OnShowBeginTimePoint12();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRouteAutoMark();
	afx_msg void OnMarkCopyVDown();
	afx_msg void OnMarkCopyVUp();
	afx_msg void OnDefinePass();
	
	afx_msg void OnDrawAbstrainA();
	afx_msg void OnDrawAbstrainB();
	afx_msg void OnTrainCopy();

	afx_msg void OnSaveTrainXML();
	afx_msg void OnSetSecretTrain();
	afx_msg void OnSetNoLateEarlyTime();
		
	afx_msg void OnSetFenjiekouLateTime();
	void OnModifytrainStart();
	void OnModifytrainAccept();

	void OnModifytrainDeliver();
	void OnModifytrainTerminal();

	void OnAbsInhibitG();//区间封锁
	void OnAbsSlowG();	 //区间缓行
	void OnAbsEleFixG(); //区间电网检修
	void OnAbsTotalFix(); //区间综合天窗检修
 	void OnSelfCloseG(); //区间变更闭塞方式
	void OnAbsInhibitG2();//其它标注
	
	void OnNewLanzhouAbsFixMark();
	void OnSideInhibitG(); // 站内封锁
	void OnStationSlowG(); // 站内慢行
	void OnSideTianchuangG();// 站内天窗修
	void OnSideOccupy(); // 站内股道占用

	void OnTextNote(); // 文字注释
	void OnJishiZhujie(); // 记事注解 
	
	// 显示机车位置 
	void OnShowtractionNo();
	void OnShowtractionTop();
	void OnShowtractionMiddle();
	void OnShowtractionBottom();

	void SaveTGPicture(CString pathname);

	void OnTurnReality();

	void OnSelectSection(int conid);
	void OnShowWholeScreen(BYTE nFlag);

	void OnThreeLine();
	void SetStatic(BYTE flag);
	void CurrentSelObjectLostFoucs(int flag);

	void OnAbsPartRun();
	void OnAttainEngineReturn();
private:
	bool m_bUniteTrainStatus;
	bool m_bRescueTrainStatus;
	
	void UniteTrains(CPoint point);
	void RescueTrain(CPoint point);	
	void OnSplitTrain(void);
	void OnSplitTrain(const CString& trainid);
	void OnSplitTrain(const CString& trainid1, const CString& trainid2);

	void OnCombineTrain(void);
	void OnCombineTrain(const CString& trainid1, const CString& trainid2);

public:
	void SetMyCtrl(CTGCtrl* ctrl);
	void SetMyDataManage(CTG_Mange* manage);
    void OnLiveDlgWindow(ULONG nFlag);
	void DisplyTrainByDirection();
	void MoveTGOCXOneStep();
private:
	void SetGraphNoteMarkPos(CTg_BLOCK* block);
    int CalcAixixY();
	void TrytoModifyOtherTrain(CTG_TRAIN* pfocus_train);
	void SendUpdateOtherTrain(void);
private:
    CTG_Mange* myPDataManage;
	CTGCtrl* m_pctrl;
 	
    YTag AixisY[MAX_SECTION_NUM];
	
public:
	void SetScrollPosition(long pos, ULONG train_index = 0);
	BOOL MouseWheel(UINT nFlags, short zDelta, CPoint pt);
	bool SetEntryDefine(const int nStation, const int nSection, CTG_TRAIN* train, int& nEntry, int& nExit, int RecIndex=0);
private:
	void DrawSideNamePerFourHour(CDC* pDC, int nY, CString& sideName);
	
	
	void OnDrawStationAixis(CDC* pDC, const CRect& DrawRect,ULONG lColor);
	void OnDrawTimeAixis(CDC* pDC, const CRect& DrawRect,ULONG lColor);
	
	
	bool IsSelFocusTrainByPoint(CPoint point);
	void SetReceivePointState(CTG_TRAIN& train, int sel_index);
	bool FocusAfterActualPoint(UINT nFocusRecIndex, CTG_TRAIN& train);
	void MouseRecievePoint(CPoint point);
private:
	
	void DrawStationAixis(CDC *pDC,ULONG lColor);
	void DrawVirtulStationAixis(CDC* pDC, ULONG lColor);

	void DrawTimeAixisExt(CDC* pDC, ULONG lColor);
	
	void OnDrawTimeAixisExt(CDC* pDC, const CRect& DrawRect,ULONG lColor);
	void OnDrawBackgrondExt(CDC* pDC, const CRect& DrawRect);
	void OnMoveLayerOneStep();
	void OnMoveLayerOneStep2();
public:
	void OnNewShiftBanci();
	short OnZoomGraph(int nZoomFlag, short count);
	CString CheckTrainSideByRelation(CTG_TRAIN& train);
	
private:
	void SetSlowFastTime(int nTimeOffset, TgTRAIN_RECORD *prev);
	void SetSlowFastTime(int nTimeOffset, TgTRAIN_RECORD *prev, TRAIN_INDEX train_index, TgTRAIN_RECORD *prev_next);
	void CheckYingyeButStationPass(CStringArray& alarm, int rec_index, TgTRAIN_RECORD *rec, CTG_TRAIN& train);
	void MouseModifyBlock(const CPoint point);
	void MouseModifyTrain(const CPoint point);
	void MouseModifyBasicTrain(const CPoint point);
	void MouseModifyMutiTrain(const CPoint point);
	CString GetBlockDcmdDesp(const CTg_BLOCK* block, const int parent_type, const int sub_type, int &stytle);
	CString GetDcmdDesp(CTG_TRAIN* train, TgTRAIN_RECORD *record, int& stytle, CString PlanArrSide, CString PlanDepSide, CString ArrSide, CString DepSide);
public:
	void EraseTrain(const CTG_TRAIN& train);
	void EraseRegion(const CRect& region);
	void DrawTrain(CTG_TRAIN& train, bool bFlag);

	void EraseBlock(const CTg_BLOCK& block);
	void DrawBlock(CTg_BLOCK& block);
	void RefreshTrain(const CTG_TRAIN* ptrain);
	
	void DrawSelTrainCZHLine();
	void ClearTrainCZHLine(TRAIN_CHEZUHAO* pTrainsCZH);
	
private:
	void OffsetFocusSameCZHTrain(long offset);
	bool CheckFocusCZHRelation(CString& str);
	bool CheckTrainCZHValid(CTG_TRAIN* train, CString& str);
	bool CheckCZHRelation(int nTimeOffset, bool bCheckTrainCZHRelation, CPoint point);
	void SelFocusCzhTrains();
	bool CheckTrainRelationValid(CTG_TRAIN* train, CString& str);
	bool CheckTrainRelation(int nTimeOffset, bool bCheckTrainCZHRelation, CPoint point);
private:
	SelCzhTrainInfo m_focusCzhTrains;
	void SendCzhOtherTrainsUpdate(DWORD train_index);
public:
	bool SetMouseSelRecord(int nFlag,int sub_section=-1);
	void ModifySideByKeyboard(int nFlag);
	void ModifyTrainArlDepTimeByKeyboard(int nFlag);
	void ModifyBlockTimeByKeyboard(int nFlag);

	bool LoadAutoGenDCmdXml();
private:
	DWORD m_timePress;
	std::vector<ULONG> m_SelectedTrains;
	std::vector<MenuNode> m_menu;
	std::map<int, int> m_map_menuid;
	std::map<int, int> m_map_menuparentid;
		
	CPoint mMutiMoveLastPoint, mOrgMutiMoveLastPoint;
	CTG_TRAIN m_trainbasicflash;
	void FlashTrain();

	void SetBlockAbsLineByBlockDirect(std::vector<int> list_absno, CTg_BLOCK& block);
	void CheckMarkMove();
	
	void ChangeStationStayOrPass();
	bool IsInhibitModifyRouteOkSchdTM();
private:
	void OnContextMenuFormat(CWnd* pWnd, CPoint point);
	void ReLayoutBmpPageSize(CClientDC& dc);
	void DrawCzhOtherTrain(CDC* pDC, ULONG train_index);
	void RefreshCzhOtherTrain();
	void SetTrainEntry(CTG_TRAIN* train, int default_entry);
	void SetTrainExit(CTG_TRAIN* train, int default_exit);
	int  GetAbsStopGiveoutExit(std::vector<int>& line_vector, int ndirect, int recIndex, int& nExit);
	int  GetStaStopGiveoutExit(CTG_TRAIN* train, std::vector<int>& line_vector, int& nExit, int recIndex, bool& bSelAllLine);
	int  GetStaStopAcceptEntry(CTG_TRAIN* train, std::vector<int>& line_vector, int& nEntry, int recIndex);
public:
	std::vector<CTG_TRAIN> m_selTrains;
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
public:
	//广州局,修改办客标志旅客列车股道时生成调度命令
	void OnAutoGenDCmd2(CString TrainID, TgTRAIN_RECORD *record, CString PlanArrSide, CString PlanDepSide, CString ArrSide, CString DepSide);
	void MessageBoxByChangeSide(TgTRAIN_RECORD *record, CString ArrSide, CString DepSide);

	void ResetFocusTrain();
	void SetRightScrollPosition();
	
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINTGFORM_H__B94FD0D1_7C1F_11D1_9CF0_000021000CE6__INCLUDED_)