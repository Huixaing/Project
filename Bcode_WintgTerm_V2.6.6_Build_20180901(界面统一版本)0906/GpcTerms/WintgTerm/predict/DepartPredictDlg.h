#if !defined(AFX_IDD_DLG_PREDICT_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_)
#define AFX_IDD_DLG_PREDICT_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CancelWorkDlg.h : header file
//
#include "resource.h"
#include ".\predictgridview.h"
#include ".\stationpredict.h"

struct SaveInfo
{
	char trainno[32];
	int  flag; // 1:到 2：发
	BYTE state;
	long oper_tm;
	STATION_NO neib_station;
	STATION_NO this_station;
	SaveInfo()
	{
		ResetInfo();
	}
	void ResetInfo()
	{
		memset(trainno, 0, sizeof(trainno));
		flag=0;
		state=0;
		oper_tm=0;
		neib_station=0;
		this_station=0;
	}
};
/////////////////////////////////////////////////////////////////////////////
// CDepartPredictDlg dialog
class CTgDocument;
class CTrainDepartPredictHolder;
class CDepartPredictDlg : public CDialog
{
// Construction
public:
	CDepartPredictDlg(CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CDepartPredictDlg)
	enum { IDD = IDD_DLG_PREDICT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDepartPredictDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	int   OnCreate(LPCREATESTRUCT lpCreat);
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDepartPredictDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CComboBox m_comboxStation;
	CButton m_btnYugao, m_btnQuxiao, m_btnTongyi;
	CButton m_btnSelect;
	CListBox m_list;
	CPredictGridView m_grid;
	multimap<TIME, TrainPredict> mPlan;
	
	CTrainDepartPredictHolder * m_pPredictServer;
private:
	int m_shiftid;
	CTgDocument *m_pDoc;
		
private:
	int mSectionId;
	STATION_NO mStation;
	TRAIN_INDEX mTrainIndex;
	BYTE mRecIndex;
	
	// 20180817 当前选中的无法自动处理，需要调度员判断的消息
	SaveInfo m_current;
	// 20180817 所有无法自动处理，需要调度员判断的消息
	std::vector<SaveInfo> m_save_list;

	void OnSize(UINT nType,int cx,int cy);
	void OnUpdateStationList();
	void OnUpdateStPlan();
	void UpdateList();
public:
	void SetPredictInfo(CTgDocument *pdoc, long shiftid, int SectionId);
	afx_msg LRESULT OnSelRow(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnPredictUpdate(WPARAM wParam,LPARAM lParam);
	
	void SaveTrainNo(STATION_NO this_station, STATION_NO neib_station, CString str, BYTE state, long oper_tm, int flag);
	void DeleteSaveInfo(SaveInfo& info);
public:
	afx_msg void OnBnClickedYugao();
	afx_msg void OnBnClickedTongyi();
	afx_msg void OnBnClickedQuxiao();
	afx_msg void OnBnClickedSelect();
	afx_msg void OnCbnSelchangeStation();

	afx_msg void OnClose();
	afx_msg void OnLbnSelchangeList1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDD_DLG_PREDICT_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_)
