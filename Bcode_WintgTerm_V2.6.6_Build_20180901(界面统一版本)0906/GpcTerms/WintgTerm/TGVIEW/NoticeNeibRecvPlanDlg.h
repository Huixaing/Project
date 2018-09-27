#pragma once

#include "../GridCtrl_src/GridCtrl.h"
#include "../GridCtrl_src/GridCellCheck.h"
#include "../uneditableGridCell.h"

#include "resource.h"

#include <vector>
// CNoticeNeibRecvPlanDlg 对话框

class CNoticeNeibRecvPlanDlg : public CDialog
{
	DECLARE_DYNAMIC(CNoticeNeibRecvPlanDlg)

public:
	CNoticeNeibRecvPlanDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNoticeNeibRecvPlanDlg();

// 对话框数据
	enum { IDD = IDD_DLG_NOTICE_RECEIVE_NEIGPLAN };
private:
	void initialGridCtrl();
	void fillGrid();	
	
	CGridCtrl * m_pGridCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// Generated message map functions
	//{{AFX_MSG(CDlgMultiSelOperate)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	bool bSelAll;
	int m_nSelHour;
	CDWordArray  m_nSectionEntityId;
	CStringArray m_strSectionName;
	afx_msg void OnBnClickedSelallbtn();
};
