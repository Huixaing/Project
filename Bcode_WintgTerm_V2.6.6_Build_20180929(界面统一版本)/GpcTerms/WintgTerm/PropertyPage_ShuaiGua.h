#pragma once
#include "afxwin.h"
#include "TrainRecordGrid.h"
class  CTgDocument;
class  CTG_SingleTrain;
// CPropertyPage_ShuaiGua 对话框

class CPropertyPage_ShuaiGua : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyPage_ShuaiGua)

public:
	CPropertyPage_ShuaiGua(CTgDocument *pdoc);
	virtual ~CPropertyPage_ShuaiGua();

// 对话框数据
	enum { IDD = IDD_DLG_SHUAIGUA_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	LRESULT OnSchdTrainSelected(WPARAM wp,LPARAM lp);
	void  OnAck();
	DECLARE_MESSAGE_MAP()

	CTrainRecordGridView m_grid;
	void OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult); 
	void  RefreshData();
	void  OnRefresh();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam); 
	void  RequestShiftInfoFromOPMS(DWORD gtid);
public:
	BOOL OnInitDialog();
	CEdit m_cShuaiGuaInfo;
	CEdit m_cImportantInfo;
	CEdit  m_cKeDiaoInfo;
	CEdit  m_cYingYeInfo;
	CEdit  m_cStopInfo;
	CTgDocument *m_pDoc;
	CTG_SingleTrain *m_pTrain;
	int   m_nScheduleType;
	int   m_nCurrentSelectRecordIndex;
	BOOL  m_bReadOnly;
	void  ClearData();
	CWnd *m_pNotifyWindow;
public:
	void SetNotifyWindow(CWnd *p) {m_pNotifyWindow=p;}
	void InformParent();
	afx_msg void OnCbnSelchangeComboStationNameSubao();
};
