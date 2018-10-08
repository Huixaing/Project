#pragma once
#include "mytoolbar.h"
#include "mmbar.h"
#include "mmdcmdbar.h"
#include "mmtrnbar.h"
#include "mmjieshibar.h"
// CMMIPlanFrame 框架

class CMMIPlanFrame : public CMDIChildWnd
{
	CMMBar  m_bar;
	CMMJieshiBar m_jieshibar;
	CMMTrainInfoBar m_trnbar;
	CMMDcmdBar m_dcmdbar;
	CMyToolBar m_wndToolBar;
///////////////////////////
	CButton m_toolBarButtonRecvFromOPMS;
	CButton m_toolBarButtonSaveToDB;
	CButton m_toolBarButtonClear;
	BOOL  AddButtonToBar(CButton& btn,const CString& str,UINT id,CRect &rect);
///////////////////////////
	DECLARE_DYNCREATE(CMMIPlanFrame)
protected:
	CMMIPlanFrame();           // 动态创建所使用的受保护的构造函数
	virtual ~CMMIPlanFrame();
	BOOL PreCreateWindow(CREATESTRUCT& cs);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();

protected:
	DECLARE_MESSAGE_MAP()
public:
	void ShowInfoInBar(const char *str,const char *node);
	void ShowDCMDInfoInBar(const char *str,const char *node);
	void ShowJieShiInBar(const char *str,const char *node);
	void ShowTrnInfoInBar(const char *xml,const char *node);
	void OnGetMinMaxInfo(MINMAXINFO *lpMMI);
	void ReDockBars();
	void Create1Bar(CSizingControlBar *pbar,UINT id);
	///
	void DockControlBarLeftOf(CSizingControlBar *dest_bar,CSizingControlBar *ref_bar,DWORD dw);
	afx_msg void OnMplanViewInfo();
	afx_msg void OnUpdateMplanViewInfo(CCmdUI *pCmdUI);
	afx_msg void OnMplanViewDcmd();
	afx_msg void OnUpdateMplanViewDcmd(CCmdUI *pCmdUI);
	afx_msg void OnMplanViewTrainInfo();
	afx_msg void OnUpdateMplanViewTrainInfo(CCmdUI *pCmdUI);
	afx_msg void OnMplanViewJieshi();
	afx_msg void OnUpdateMplanViewJieshi(CCmdUI *pCmdUI);
};


