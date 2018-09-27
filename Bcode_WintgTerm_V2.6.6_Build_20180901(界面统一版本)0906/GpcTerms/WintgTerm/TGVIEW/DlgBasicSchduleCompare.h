#pragma once
#include "SchduleCompare.h"
#include "tg_schedule_base.h"

class  CStationTimeListGridForBaisc:public CFindSchduleGrid
{
private:
	struct PropertyInfoPara  m_train_def[10];
	int m_train_col_count;
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  ShowTrainData(CString& xml);
};

class  CStationSideListGridForBaisc:public CFindSchduleGrid
{
private:
	struct PropertyInfoPara  m_train_def[10];
	int m_train_col_count;
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  ShowTrainData(CString& xml);
};

class  CStationRouteListGridForBaisc:public CFindSchduleGrid
{
private:
	struct PropertyInfoPara  m_result_def[12];
	int m_result_col_count;

public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  ShowTrainData(CString& xml);
};

class CDlgBasicSchduleCompare : public CDialog
{
public:
	CDlgBasicSchduleCompare(CWorkScheduleHolder* pHolder, CWnd* pParent=NULL);
	virtual ~CDlgBasicSchduleCompare();

	enum { IDD = IDD_DLG_SCHDULE_COMPARE_BASIC};
public:
	void InitDlgCtrlItem();
	
protected:
	CWorkScheduleHolder* m_pHolder;
	
	CButton m_btnReCheck;
	
	CButton m_btnGaoTie, m_btnPuKe, m_btnHuoche;
	CStatic m_static1, m_static2, m_static3, m_static4;
	CEdit m_edit1,m_edit2;

	void InitGridStation();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	virtual BOOL OnInitDialog();
	void  OnClose();
	void  OnCancel();
	
	DECLARE_MESSAGE_MAP()
	void ReCompare();
	void getCompareResultXml(CString& timexml,CString& sidexml,CString& routexml);
	void compareWithBasic();
	void generateObjInfo();
	void compareObjWithBasic(const ResultInfo& basicinfo, int idx);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLbnDblclkList2();
	bool IsCompareThisTrain(CTG_SingleTrain* ptrain);
private:
	void SetCompareStation();
	HWND m_hViewWnd;
	CStationSelGrid m_staSelGridCtrl;
	CStationTimeListGridForBaisc m_trainTimeGrid;
	CStationSideListGridForBaisc m_trainSideGrid;
	CStationRouteListGridForBaisc m_trainRouteGrid;
   
	int station_count;
	ResultList m_result[128];
};
