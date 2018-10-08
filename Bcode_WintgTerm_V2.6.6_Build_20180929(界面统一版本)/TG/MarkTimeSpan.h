#pragma once
#include "afxdtctl.h"


// CMarkTimeSpan 对话框

class CMarkTimeSpan : public CDialog
{
	DECLARE_DYNAMIC(CMarkTimeSpan)

public:
	CMarkTimeSpan(CWnd* pParent = NULL);   // 标准构造函数
	CMarkTimeSpan(time_t s_time, time_t e_time, CWnd* pParent = NULL);
	virtual ~CMarkTimeSpan();

// 对话框数据
	enum { IDD = IDD_DLG_MARK_TIMESPAN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnDtnDatetimechangeDateSt(NMHDR *pNMHDR, LRESULT *pResult);
	CDateTimeCtrl st_date;
	CDateTimeCtrl et_date;
	CDateTimeCtrl st_time;
	CDateTimeCtrl et_time;

public:
	
	void getTime(time_t &s_time, time_t &e_time);



private:
	time_t m_s_time, m_e_time;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDtnDatetimechangeSgtimeSt(NMHDR *pNMHDR, LRESULT *pResult);
};
