#pragma once

#include "GridCtrl_src\gridcellcheck.h"
#include "wintgdoc.h"
#include "uneditableGridCell.h"

// CDlgGiveoutDirectionSel 对话框

class CDlgGiveoutDirectionSel : public CDialog
{
	DECLARE_DYNAMIC(CDlgGiveoutDirectionSel)

public:
	CDlgGiveoutDirectionSel(CWintgDoc *pDoc, CSingleTrain *single,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgGiveoutDirectionSel();

// 对话框数据
	enum { IDD = IDD_DLG_GIVEOUT_DIRECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

public:
	int m_nCount;
	CGridCtrl * m_pGridCtrl;
	CWintgDoc *m_pDoc;
	CSingleTrain *m_nTrain;

	void initialGridCtrl();
	void fillGrid();
    int abs_suited ;
	int abs_line_suited[32];
	CString sLineName[32];
	int exit_list[32];
	int nSelExit;
	afx_msg void OnBnClickedOk();
};
