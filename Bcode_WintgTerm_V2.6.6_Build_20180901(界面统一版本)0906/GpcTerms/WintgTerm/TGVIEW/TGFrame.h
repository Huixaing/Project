#pragma once
#include "mytoolbar.h"
#include "tgbar.h"
#include "colorbtn.h"
// CTGFrame 框架
class CTgView;
class CTgFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CTgFrame)
	enum{TG_BUTTON_NUM=21};
protected:
	CTgFrame();           // 动态创建所使用的受保护的构造函数
	virtual ~CTgFrame();
	CTgView  *GetTgView();
	//CSplitterWnd m_cSplitter;
	CTgBar   m_tgBar;  // 显示列车属性!
	int      m_nFrameType;
protected:
	//  下面的工具条和按钮
	CMyToolBar m_wndToolBar;
	CColorButton *m_pButtons[TG_BUTTON_NUM]; 
	
	BOOL  AddButtonToBar(int index,const char *str,UINT id,UINT style=0,BOOL enable=TRUE);
	int   GetTotalButtonCount() {return 20;}
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowToolbar();
	afx_msg void OnShowPropertyWindow();
	
	afx_msg void OnUpdateShowPropertyWindow(CCmdUI* pCmdUI);
	LRESULT OnPropertyUpdate(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetFrameType(WPARAM wParam, LPARAM lParam);
	LRESULT OnUserSelectFrame(WPARAM wParam, LPARAM lParam);
	BOOL PreCreateWindow(CREATESTRUCT& cs);
	void OnDestroy();
public:
	virtual void ActivateFrame(int nCmdShow = -1);
public:
	void ShowPropertyWindow(BOOL show);
	void SetPropertyBarReadOnly(BOOL va) {m_tgBar.SetReadOnly(va);} 
	void SetFrameType(int tp) {m_nFrameType=tp;} // 0: only icon disp on tab, 1: title disp on tab
};


