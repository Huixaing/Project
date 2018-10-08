#pragma once
#include "mytoolbar.h"
#include "tabwnd.h"
// CTgTabFrame 框架
class CTgView;
class CTgTabFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CTgTabFrame)
protected:
	CTgTabFrame();           // 动态创建所使用的受保护的构造函数
	virtual ~CTgTabFrame();
	CView  *GetBasicTGView();
	CView  *GetShiftTGView();
	CView  *GetWorkTGView();
	CTabWnd m_tabWnd;
private:
	//  下面的工具条和按钮
	CMyToolBar m_wndToolBar;
	//CMyToolBar m_wndToolBar_bottom;
 	//CComboBox m_toolBarButton; 
	CButton m_toolBarButtonUp; 
	CButton m_toolBarButtonDown; 
	CButton m_toolBarButtonMixed; 

	CButton m_toolBarButtonPassenger; 
	CButton m_toolBarButtonFreight; 
	CButton m_toolBarButtonAllTrain; 

	CButton m_toolBarButtonReceivePoint; 
	CButton m_toolBarButtonPointAdjust; 
//add new
	CButton m_toolBarButtonWidth;
	CButton m_toolBarButtonSmall;
	CButton m_toolBarButtonTurnReality;
	CButton m_toolBarButtonSendStaPlan;
	CButton m_toolBarButtonSendNeighber;
	CButton m_toolBarButtonRecvPlan;

	CButton m_toolBarButtonInsertNewTrn;
	CButton m_toolBarButtonDrawNewTrnEnd;
	CButton m_toolBarButtonNewDC;
	CButton m_toolBarButtonDCManage;
	CButton m_toolBarButtonBasicTrn;
	CButton m_toolBarButtonNote;

	BOOL  AddButtonToBar(CButton& btn,CString& str,UINT id,CRect &rect);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowToolbar();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	void OnSize(UINT nType,int cx,int cy);
	virtual void ActivateFrame(int nCmdShow = -1);
	BOOL  InitScheduleView(int shift_id,const char *datapath);
private:
	afx_msg void OnSendPlan();
	afx_msg void OnRecieveNeighber();
	afx_msg void OnRecieveBasicPlan();
	afx_msg void OnNewDDML();
	afx_msg void OnDDMLQuery();
};


