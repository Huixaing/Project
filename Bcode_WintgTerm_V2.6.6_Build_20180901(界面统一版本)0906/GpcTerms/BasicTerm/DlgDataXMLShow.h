#pragma once
#include "resource.h"
#include "tg_schedule_base.h"
// CDlgTestTD 对话框

class CDlgDataXMLShow : public CDialog
{
	DECLARE_DYNAMIC(CDlgDataXMLShow)

public:
	CDlgDataXMLShow(CTG_LogicObject *pobj, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDataXMLShow();

// 对话框数据
	enum { IDD = IDD_DIALOG_XML_SHOW };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
	
private:
	CTreeCtrl m_tree;
	CTG_LogicObject *m_pobj;
public:
	void DisplayXML();
};

