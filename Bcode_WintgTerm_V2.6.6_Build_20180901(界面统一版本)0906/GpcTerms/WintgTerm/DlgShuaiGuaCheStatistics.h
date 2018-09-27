#if !defined(AFX_DLGSHUAIGUACHESTATISTICS_H__FA761A94_A698_466D_8D85_806140B02E16__INCLUDED_)
#define AFX_DLGSHUAIGUACHESTATISTICS_H__FA761A94_A698_466D_8D85_806140B02E16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgShuaiGuaCheStatistics.h : header file
//
#include "MyGridCtrl.h"
#include "SingleTrain4Statistics.h"
#include "StatisticsStoreStructureEx.h" //存放调度台数据的结构
#include "HistorySchdDlg.h"			//弹出选择选择历史图日期选项，暂时使用
#include "tgmsg.h"

#include "StatisticsSectionInfo.h"

#define SGCMAXSTATIONUM 50

extern CStatisticsSectionInfo* g_StatisticsSecInfo;
/*

typedef struct {
	int nDispatcherNo;	//调度台编号
	int nStationNum;	//该调度台下面车站个数
	int nStationList[SGCMAXSTATIONUM];	//该调度台下面车站列表
}StatisticsDispatchInfo;

typedef struct{
	int nStationNum;			//该区段要统计的车站列表个数
	int nStationList[10];		//该区段要统计的车站列表，在该车站终到或者始发的列车要进行统计
	int nSubSectionNum;			//该区段下面的子区段个数
	int nSubSectionStart[10];	//该区段下面的子区段列表的起始车站编号
	int nSubSectionEnd[10];		//该区段下面的子区段列表的终止车站编号
}StatisticsSubSectionInfo;

typedef struct {
	int nSectionNo;			//区段编号
	char sSectionName[20];	//区段名称
	int nSectionDistance;	//该区段的公里数
	int nStationNum;		//该区段的车站数目
	int nDispatcherNum;		//该区段下面调度台个数
	StatisticsDispatchInfo dispInfo[SGCMAXDIAPATCHNUMPERSECTION];	//该区段下面每个调度台的详细信息
	StatisticsSubSectionInfo subSectionInfo;

}StatisticsSection;

typedef struct{
	int nNumToLoad;				//要调阅的数据个数
	int nNumLoaded;				//已经调阅的数据个数
	int nDispNoList[20];		//要调阅的调度台号
	long nShiftDateList[20];	//要调阅的时间
} LoadListStructure;

*/




/////////////////////////////////////////////////////////////////////////////
// CDlgShuaiGuaCheStatistics dialog

class CDlgShuaiGuaCheStatistics : public CDialog
{
// Construction
public:
	
	
	CDlgShuaiGuaCheStatistics(CWnd* pParent = NULL);   // standard constructor
	~CDlgShuaiGuaCheStatistics();	// standard desconstructor
	
	CMyGridCtrl *m_pGridCtrl;	//存放甩挂车数据的表格

	CDC m_MemDC;				//为了在甩挂车内容水平滚动条滚动时候显示对应的标题栏所创建的
	CBitmap m_MemBitmap;		//内存位图，存放整个的标题栏
	CBitmap* m_pOldBitmap;		//老的内存位图
	int m_MemBitmapWidth;		//位图宽度，和gridctrl空间的宽度相同
	int m_MemBitmapHeight;		//位图高度，和外面文本框的高度相同
	
	int m_CurMinCol;			//每次水平滚动的时候显示的第一个列的列号，从0开始，用来定位我们的标题栏位置
	SCROLLINFO m_scInfo;		//滚动的时候发过来的滚动信息暂时没有用
	
	
	int m_TitleColNum;			//标题栏的总列数 = m_HeadColNum+m_RearColNum+m_stationNum
	int m_HeadColNum;			//前头上的列数
	int m_RearColNum;			//尾部的列数
	
	int m_StationNum;			//车站数目

	_TCHAR* m_HeadColName[10];	//标题栏车站名字前面的标题
	_TCHAR* m_RearColName[10];	//标题栏车站名字后面的标题
	_TCHAR m_StationColName[SGCMAXSTATIONUM][10];	//标题栏车站的名字

	int m_SingleStationColWidth;					//每列车站的名字的宽度
	int m_TitleColWidth[SGCMAXSTATIONUM+20];		//各列对应的宽度，对应标题栏

	int m_GridColNum;								//Grid控件的总列数 = m_HeadColNum+m_RearColNum+m_stationNum*2
	int m_GridColWidth[2*SGCMAXSTATIONUM+20];		//各列对应的宽度，对应grid控件

	int m_xoffset;									//画标题栏的时候列要平移得像素暂时没有用
	
	StatisticsSection* m_CurStatisticsSection;		//当前选中区段指针 
	int m_CurStatisticsSectionNo;					//当前选中区段的编号

		//定义几个存放列车数据的变量
	CStatisticsStoreStructureEx m_datastore;		//存放所有数据的一个结构
	CShiftDataPerDispStation* m_curShiftDataStore;	//存放当前要接收数据的结构

	
// Dialog Data
	//{{AFX_DATA(CDlgShuaiGuaCheStatistics)
	enum { IDD = IDD_DLG_SHUAIGUASTATISTICS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgShuaiGuaCheStatistics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL SetDefaultVal();
	BOOL SetGridColWidth();
	int ComputeSourceX();
	//BOOL LoadStatisticsSectionInfo();
	BOOL DrawStationNameTitle(CDC* pDC,CString DrawString, CRect DrawRect);
	BOOL DrawHeadRearTitle(CDC* pDC,CString DrawString,CRect DrawRect);
	BOOL DrawMemTitle(CDC* pDC);
	void ShowRecordInGrid();
	//int GetStationListofDisp(int sectionNo,int dispNo,int& stationNum,int* stationList);
	// Generated message map functions
	//{{AFX_MSG(CDlgShuaiGuaCheStatistics)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg LRESULT OnHScrollStatisticsContent(WPARAM scrolllen,LPARAM scrollinfo);
	afx_msg void OnButtonClose();
	afx_msg void OnClose();
	afx_msg void OnStatsticsSgcRemoteload();
	afx_msg LRESULT OnSchdAddTrain(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSchdStatus(WPARAM wParam,LPARAM lParam);
	afx_msg void OnStatisticsLoadschedule();
	afx_msg void OnSetFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	afx_msg void OnStatisticsSection(UINT message);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSHUAIGUACHESTATISTICS_H__FA761A94_A698_466D_8D85_806140B02E16__INCLUDED_)
