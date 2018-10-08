#pragma once
#include "SchduleCompare.h"
#include "tg_schedule_base.h"

class  CStationTimeListGridForShift:public CFindSchduleGrid
{
private:
	struct PropertyInfoPara  m_train_def[10];
	int m_train_col_count;
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  ShowTrainData(CString& xml);
};

class  CStationSideListGridForShift:public CFindSchduleGrid
{
private:
	struct PropertyInfoPara  m_train_def[10];
	int m_train_col_count;
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  ShowTrainData(CString& xml);
};

class  CStationRouteListGridForShift:public CFindSchduleGrid
{
private:
	struct PropertyInfoPara  m_result_def[12];
	int m_result_col_count;

public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  ShowTrainData(CString& xml);
};

class CTgDocument;
class CDlgShiftSchduleCompare : public CDialog
{
public:
	CDlgShiftSchduleCompare(CWorkScheduleHolder* pHolder, CWnd* pParent=NULL);
	virtual ~CDlgShiftSchduleCompare();

	enum { IDD = IDD_DLG_SCHDULE_COMPARE_SHIFT};
public:
	void InitDlgCtrlItem();
	
protected:
	CWorkScheduleHolder* m_pHolder;

	CButton m_btnReCheck;
	
	CButton m_btnGaoTie, m_btnPuKe, m_btnHuoche;
	CButton m_btnRadioThreeTime, m_btnRadioAllViewTime;
	CStatic m_static1, m_static2, m_static3, m_static4;
	CEdit m_edit2;
		
	void InitGridStation();
	void GetCompareTimeRange(TIME& bTm, TIME& eTm);
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
	bool IsCompareThisTrain(CTG_SingleTrain* ptrain);
private:
	void SetCompareStation();
	HWND m_hViewWnd;
	CStationSelGrid m_staSelGridCtrl;
	CStationTimeListGridForShift m_trainTimeGrid;
	CStationSideListGridForShift m_trainSideGrid;
	CStationRouteListGridForShift m_trainRouteGrid;
   
	int station_count;
	ResultList m_result[128];
};
