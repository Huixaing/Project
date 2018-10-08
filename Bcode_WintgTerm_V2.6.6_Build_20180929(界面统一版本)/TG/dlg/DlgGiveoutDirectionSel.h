#pragma once
#include "resource.h"
#include "gridctrl.h"
#include "gridcellcheck.h"
#include "tgdoc.h"
#include "uneditableGridCell.h"

// CDlgGiveoutDirectionSel 对话框

class CDlgGiveoutDirectionSel : public CDialog
{
	DECLARE_DYNAMIC(CDlgGiveoutDirectionSel)

public:
	CDlgGiveoutDirectionSel(CTGDoc *pDoc, CWnd* pParent, int nFlag, std::vector<int>& abs_line_list, int station);
	CDlgGiveoutDirectionSel(CTGDoc *pDoc, CWnd* pParent, int nFlag, int abs_line_list[], int count, int station);
	CDlgGiveoutDirectionSel(CTGDoc *pDoc, CTG_TRAIN *single,CWnd* pParent, int nFlag, int rec_index);   // 标准构造函数
	virtual ~CDlgGiveoutDirectionSel();

// 对话框数据
	enum { IDD = IDD_DLG_GIVEOUT_DIRECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

	int m_station;
	int abs_line_num;
    int abs_lineno[128];

	CGridCtrl * m_pGridCtrl;
	CTGDoc *m_pDoc;
	CTG_TRAIN *m_nTrain;

    int abs_suited;
	int abs_line_suited[128];
	CString sLineName[128];
	int exit_list[128];
	

	afx_msg void OnBnClickedOk();
private:
	void initialGridCtrl();
	void InitAbsLineNo();
	void InitExitLineNo();
	void InitExitLineNo4();
	void InitExitLineNo5();
	void InitEntryLineNo();
	void InitEntryLineNo5();
	void InitAbsLineNo8();
	void FillGrid();
private:
	int m_nFlag;
	int m_nFoucsRecIndex;
public:
	int nSelDirect;

	std::vector<int> exit_lines;
	std::vector<int> entry_lines;
};
