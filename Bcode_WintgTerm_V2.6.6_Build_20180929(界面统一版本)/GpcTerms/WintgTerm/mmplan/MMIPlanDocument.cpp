// MMIPlanDocument.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "MMIPlanDocument.h"


// CMMIPlanDocument

IMPLEMENT_DYNCREATE(CMMIPlanDocument, CDocument)

CMMIPlanDocument::CMMIPlanDocument()
{
}

BOOL CMMIPlanDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CMMIPlanDocument::~CMMIPlanDocument()
{
}


BEGIN_MESSAGE_MAP(CMMIPlanDocument, CDocument)
END_MESSAGE_MAP()


// CMMIPlanDocument 诊断

#ifdef _DEBUG
void CMMIPlanDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CMMIPlanDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMMIPlanDocument 序列化

void CMMIPlanDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CMMIPlanDocument 命令
