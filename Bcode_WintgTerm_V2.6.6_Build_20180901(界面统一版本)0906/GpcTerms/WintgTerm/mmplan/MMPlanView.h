// TestTgView.h : CMMPlanView 类的接口
//


#pragma once

#include "TgDocument.h"
#include "markgrid.h"
#include <afxdtctl.h>
#include "../td/td_data_def.h"
#include "../utility/gen_mytree.h"


class CMMPlanView : public CView
{

public:
protected: // 仅从序列化创建
	CMMPlanView();
	DECLARE_DYNCREATE(CMMPlanView)

// 属性
public:
	CTgDocument* GetDocument() const;

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
// 实现
public:
	virtual ~CMMPlanView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

protected:
	CButton         m_bStartDate;
	CButton         m_bSecondDate;
	CComboBox       m_comboSubPlanID;

	CDateTimeCtrl   m_startTimeCtrl;
	CDateTimeCtrl   m_endTimeCtrl;

	CButton         m_bUseTime;
	CButton         m_cmdRefresh;

	CButton			m_bOnlyShowMark;
	CButton			m_bOnlyShowSkyLight;

	CStatic         m_capRiJHH;
	CEdit           m_edRiJHH;
	BOOL  CreatePlanCtrl(int row);

	CMyGenTree        m_mplan_tree;

protected:
	void  GetTitle(CString &title);
public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, int cx, int cy);
	void OnDestroy();
	void OnTimer(UINT idevent);
	void OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	void OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult); 
		
public:
	virtual void OnInitialUpdate();
public:
	void    OnRecvFromOPMS();
	void    OnSaveToDB();
	
	
	void    OnGetInfo();
	void    OnGetDCMD();
	void    OnTransDCMD();
	void    OnGenMark();
	void    OnGetJieShi();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam);
	void    OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	void    OnRecoverGenMark();
	void    OnCancelGenMark();
private:
	 CString GetPlanID();
	long   m_nEntityOfDCMD;

	void  ShowMPlanList(int type = 0); //type 显示符号类型:0:全部 1:封锁/慢行 2:天窗
	void  ShowMPlanInfo(const char *xml,const char *node);
	void  ShowMPlanInfo(const char *subid);
    void  ShowMPlanDCMDInfo(int index);
	void  ShowMPlanDCMDInfo(const char *planid);
	void  ShowMPlanDate();
	void  CancelMPlanWork(int index);
	void  TDRetCancelMPlanWorkResult(const CString& planid, int nResult);
	void  TDRetRecoverMPlanWorkResult(const CString& planid, int nResult);
public:
	afx_msg void OnMplanJieshi();
	afx_msg void OnMplanDcmd();
		
	afx_msg void OnDate1Click();
	afx_msg void OnDate2Click();
	afx_msg void OnMplanGetallInfo();
	afx_msg void OnSubIDChanged();
	afx_msg void OnMmplanSelectDcmd();
	
	afx_msg void OnUseTimeClick();
	afx_msg void OnMPlanRefresh();
	afx_msg void OnTransDCMDAll();
	afx_msg void OnGenMarkAll();

	afx_msg void OnOnlyShowMarkList();
	afx_msg void OnOnlyShowSkylightList();

	void OnUpdateEnable(CCmdUI* pCmdUI);
	BOOL OnEraseBkgnd(CDC* pCDC);
	void InsertMMPlanToTree(CMMPlanListInfo *mPlan, int type); // 2010.3.20 增加施工计划至TREE
	void OnPaint();
};

