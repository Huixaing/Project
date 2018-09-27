#pragma once
#include "traingrid.h"
#include "markgrid.h"
#include "tg_schedule_base.h"
#include "tg_basicpara.h"
#include "td/td_data_def.h"
// CDlgTrainList 对话框
class CWorkScheduleHolder;
//class CTG_TrainScheduleBase;
class CTgDocument;
class CWintgViewMMI;
class CDlgTrainList : public CDialog
{
	DECLARE_DYNAMIC(CDlgTrainList)

public:
	BOOL    m_bDirectlyRequestMode;  // 直接申请模式，不是按照计划通知

private:
	CTrainGridView m_grid;
	CXMLDataGrid   m_grid_plan;
	
	CMarkGridView  m_markgrid;
	CWorkScheduleHolder *m_pHolder;
	CTG_TrainScheduleBase *m_pSourceData;
	CTG_TrainScheduleBase *m_pData;
	CTG_ScheduleInfo       m_schd_info;
	CWnd                  *m_pWaitWnd;
	CTgDocument   *m_pDoc;
	CWintgViewMMI *m_pTgMMI;
	int    m_nCurrentSel;
	int    m_nCurrentSelPlan;

	CString m_szCaption;
	int     m_nGridType; // 0: train  1: mark

	CTD_PlanInform  m_plan_inform; // from opms
	
	bool m_bSelAll;

	// 新类型列车分割使用
	int  m_nSplitNum;
	int  m_cSplitStationList[NEW_SPLIT_ST_NUM];
	int  m_cSplitEntryList[NEW_SPLIT_ST_NUM];
	
public:
	int    m_nScheduleType;
	BOOL   m_bLoadFromSchedule;
	int    m_nDataChanged;
public:
	void   SetTgMMI(CWintgViewMMI *pmmi) {m_pTgMMI=pmmi;}
	BOOL   SetPlanInformXML(const char *token,const char *xml);
	CDlgTrainList(const char *caption,CTgDocument *pdoc,int sche_type,int type,CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CDlgTrainList();
	void   AddRequestTrainGUID(DWORD tid,const char *tno,BOOL local_add);
// 对话框数据
	enum { IDD = IDD_DLG_TRAIN_LIST };
   
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void         OnOK();
	void    OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult); 
	void    OnMarkGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult); 
	void    OnPlanGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult);
	void    OnBnClickedReq();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam); 
    DECLARE_MESSAGE_MAP()

	void ShowTrainInfo();
	void ShowMarkInfo();
	BOOL ProcessTrainMsgFromOPMS(const char *token,const char *content);
	BOOL ProcessReplyMsgFromOPMS(const char *token,const char *content);
	BOOL ProcessShiftInfoMsgFromOPMS(const char *token,const char *content);
	void RequestNewYLFromOPMS();
	void RefreshTrainList();
	void RequestShiftInfoFromOPMS(DWORD gtid);

	void RefreshPlanList();
	void RequestPlanDetail();
	void OnTimer(UINT_PTR IDEvent);
	void GetRecvCountRate(int &recved,int &tatol);
	void OnDestroy();
	void RefreshTrainList_Orig();

private:
	CStringArray tdms_ctc_alarm;
	struct SetInfo
	{
		int entry;
		int exit;
		int arrive; //由于货物列车分割后,不能保证首站有时间,该时间由前一段列车推出
		bool bFirst;//是否首段列车
	};
	int SetNewTrainByOldTrain(int nSection, const SetInfo& info, CTD_Train& xmltrain, CTG_SingleTrain& newtrain, CTG_SingleTrain *ptrain);
	BOOL SplitTDTrainx(CTD_Train &tdtrain, std::vector<CTD_Train>& train_list,const CTG_ScheduleInfo &schd,int entry_list[16], BOOL newflag) const;
	void SplitTDTrain(CTD_Train& train, CTD_Train& other_train,int rec_index) const;
	BOOL ProcessTrainMsgFromOPMS_NEW(const char *token,const char *content);
	void CheckAndSetTipToDdyFlag(CTG_SingleTrain& train);
public:
	afx_msg void OnBnClickedButton1();
};
