#pragma once

#include "resource.h"
#include "tgdoc.h"

#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CManualReportHistory dialog
// CDlgCombineTrain 对话框

class CDlgCombineTrain : public CDialog
{
	DECLARE_DYNAMIC(CDlgCombineTrain)

public:
	CDlgCombineTrain(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCombineTrain();

// 对话框数据
	enum { IDD = IDD_COMBINE_TRAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strNewTrainId;
	CString m_strTrainId1;
	CString m_strTrainId2;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCombineOk();
	afx_msg void OnCombineCancel();
};
