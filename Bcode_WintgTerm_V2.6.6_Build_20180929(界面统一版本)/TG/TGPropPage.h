#pragma once

// TGPropPage.h : CTGPropPage 属性页类的声明。


// CTGPropPage : 有关实现的信息，请参阅 TGPropPage.cpp。

class CTGPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CTGPropPage)
	DECLARE_OLECREATE_EX(CTGPropPage)

// 构造函数
public:
	CTGPropPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_TG };

// 实现
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 消息映射
protected:
	DECLARE_MESSAGE_MAP()
};

