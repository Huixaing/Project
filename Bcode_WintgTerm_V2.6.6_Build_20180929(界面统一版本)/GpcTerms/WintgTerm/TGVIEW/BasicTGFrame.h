#pragma once
#include "mytoolbar.h"
#include "tgframe.h"

// CTGFrame 框架
class CBasicTgFrame : public CTgFrame
{
	DECLARE_DYNCREATE(CBasicTgFrame)
protected:
	CBasicTgFrame();           // 动态创建所使用的受保护的构造函数
	virtual ~CBasicTgFrame();
private:
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
};


