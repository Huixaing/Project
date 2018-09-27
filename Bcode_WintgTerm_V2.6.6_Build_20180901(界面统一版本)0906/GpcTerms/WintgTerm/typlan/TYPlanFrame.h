#pragma once

#include "mytoolbar.h"
// CTYPlanFrame 框架

class CTYPlanFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CTYPlanFrame)
protected:
	CTYPlanFrame();           // 动态创建所使用的受保护的构造函数
	virtual ~CTYPlanFrame();

protected:
	DECLARE_MESSAGE_MAP()
	CMyToolBar m_wndToolBar;
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnClose();
};


