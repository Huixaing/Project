#if !defined(TccCommandSketchMapDlg_H)
#define TccCommandSketchMapDlg_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TccCommandSketchMapDlg.h : header file
//

#include "tcc.h"
#include "LS_SketchMap.h"

namespace TCC_C3
{

// CTccCommandSketchMapDlg 对话框

class CTccCommandSketchMapDlg : public CDialog
{
	DECLARE_DYNAMIC(CTccCommandSketchMapDlg)

public:
	CTccCommandSketchMapDlg(ITccClientInfoProvider *pInfoProvider,
		CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTccCommandSketchMapDlg();
//	CScrollBar SCrollBar;
// 对话框数据
	enum { IDD = IDD_DLG_SKETCH_MAP };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	ITccClientInfoProvider *m_pInfoProvider;
	
	
	// Generated message map functions
	//{{AFX_MSG(CTccCommandSketchMapDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	afx_msg void OnClickedBtnBeginLimit();
	afx_msg void OnClickedBtnEndLimit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetKmMark(int builtin_id, kmmark_t nKmBegin, kmmark_t nKmEnd);
	CLS_SketchMap m_SketchMap;
	BOOL m_BtnEn;   //是否显示按钮可以改变当前的限速区段
	//virtual BOOL OnInitDialog();
};

}

#endif //if !defined(TccCommandSketchMapDlg_H)