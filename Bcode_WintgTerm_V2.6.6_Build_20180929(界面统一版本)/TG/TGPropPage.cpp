// TGPropPage.cpp : CTGPropPage 属性页类的实现。

#include "stdafx.h"
#include "TG.h"
#include "TGPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CTGPropPage, COlePropertyPage)



// 消息映射

BEGIN_MESSAGE_MAP(CTGPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// 初始化类工厂和 guid

IMPLEMENT_OLECREATE_EX(CTGPropPage, "TG.TGPropPage.1",
	0x4b566035, 0xd572, 0x4c02, 0x8f, 0x4f, 0x30, 0xc6, 0x4f, 0x82, 0xd2, 0x9f)



// CTGPropPage::CTGPropPageFactory::UpdateRegistry -
// 添加或移除 CTGPropPage 的系统注册表项

BOOL CTGPropPage::CTGPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_TG_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CTGPropPage::CTGPropPage - 构造函数

CTGPropPage::CTGPropPage() :
	COlePropertyPage(IDD, IDS_TG_PPG_CAPTION)
{
}



// CTGPropPage::DoDataExchange - 在页和属性间移动数据

void CTGPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CTGPropPage 消息处理程序
