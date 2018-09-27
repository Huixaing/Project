#if !defined(AFX_JIAOJIECHESTATISTICDLG_H__B89E7DEA_CE18_40A4_A1C5_A5B0111D8D99__INCLUDED_)
#define AFX_JIAOJIECHESTATISTICDLG_H__B89E7DEA_CE18_40A4_A1C5_A5B0111D8D99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JiaojiecheStatisticDlg.h : header file
//
#include "resource.h"
#include "memshare.h"
#include "MySuperGrid.h"
#include "TgDocument.h"

struct TYPE_STATISTIC_INFO
{
	//列车类型
	BYTE  train_type;   
	char train_text[64];

	//实际接入数
	short real_in_count;  
	//计划接入数      
	short plan_in_count;        
	

	//实际交出数
	short real_out_count;        
	//计划交出
	short plan_out_count;        
	
	TYPE_STATISTIC_INFO()
	{
		memset(this, 0, sizeof(TYPE_STATISTIC_INFO));
	}
};

class STA_STATISTIC_INFO
{
public:
	int nStationNo;
	// 方向号
	int nDirectNo;
	std::map<int, TYPE_STATISTIC_INFO> mapTrainDetail;
	
	STA_STATISTIC_INFO()	
	{
		nStationNo=0;
		nDirectNo=0;
		mapTrainDetail.clear();
	}
};

class CJiaojiecheStatisticDlg : public CDialog
{
// Construction
public:
	CJiaojiecheStatisticDlg(CTgDocument *pTgDoc, int ntSectionNo, CWnd* pParent=NULL);   // standard constructor

public:
// Dialog Data
	//{{AFX_DATA(CJiaojiecheStatisticDlg)
	enum { IDD = IDD_DLG_JIAOJIECHE_STATISTIC };
	CComboBox	m_DirectionCtrl;
	CStatic	m_Frame;
	CComboBox	m_StationCtrl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJiaojiecheStatisticDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DrawBottomExplain(CDC *pDC,CRect&rect,int xStep,int yStep);
	

	// Generated message map functions
	//{{AFX_MSG(CJiaojiecheStatisticDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelComboStation();
		
	afx_msg void OnStatistic();
	afx_msg void OnPrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ShowStatisticInfo();
	void SetTrains();
	afx_msg void OnBnClickedUpTrain();
	afx_msg void OnBnClickedDownTrain();
	int  GetTrainTypeDesp(MSXML::IXMLDOMNodePtr pNode, int layer);
    void PrintEveryPageInfo(CDC *pDC, CPrintInfo *pInfo);
private:
	int nStationNum;
	int StationList[64];  //交接口车站的列表
    
	std::map<int, TYPE_STATISTIC_INFO> mTmpMapTrainDetail;
	
	STA_STATISTIC_INFO m_staStatisticInfo;
	int flag_up; 
	int flag_down;    
	CBrush m_brGrnBrush;
	CMySuperGrid m_List;
	std::vector<DirNode> aryPlotTrainText;
private:
	CTgDocument *m_pTgDoc;
	int m_nCurrentSectionNo;
	std::vector<CTG_SingleTrain> m_vecSchdTrains;	
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JIAOJIECHESTATISTICDLG_H__B89E7DEA_CE18_40A4_A1C5_A5B0111D8D99__INCLUDED_)
