#pragma once
#include "mytoolbar.h"
#include "tgframe.h"

// CTGFrame 框架
class CShiftTgFrame : public CTgFrame
{
	DECLARE_DYNCREATE(CShiftTgFrame)
protected:
	CShiftTgFrame();           // 动态创建所使用的受保护的构造函数
	virtual ~CShiftTgFrame();
private:
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
};


