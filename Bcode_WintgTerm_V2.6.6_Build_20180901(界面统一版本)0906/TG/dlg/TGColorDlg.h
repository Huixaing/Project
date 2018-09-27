#pragma once
#include "afxcmn.h"


// CTGColorDlg 对话框

class CTGColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CTGColorDlg)

public:
	CTGColorDlg(CWnd* pParent);   // 标准构造函数
	virtual ~CTGColorDlg();

// 对话框数据
	enum { IDD = IDD_DLG_TG_COLOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_slider;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

public:
	BYTE mColor;
	afx_msg void OnNMThemeChangedSlider1(NMHDR *pNMHDR, LRESULT *pResult);
};
