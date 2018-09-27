#pragma once
#include "SchduleCompare.h"
#include "tg_schedule_base.h"

class  CStationTimeListGridForWork:public CFindSchduleGrid
{
private:
	struct PropertyInfoPara  m_train_def[10];
	int m_train_col_count;
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  ShowTrainData(CString& xml);
};

class  CStationSideListGridForWork:public CFindSchduleGrid
{
private:
	struct PropertyInfoPara  m_train_def[10];
	int m_train_col_count;
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  ShowTrainData(CString& xml);
};


class  CStationRouteListGridForWork:public CFindSchduleGrid
{
private:
	struct PropertyInfoPara  m_result_def[12];
	int m_result_col_count;

public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  ShowTrainData(CString& xml);
};


class CDlgWorkSchduleCompare : public CDialog
{
public:
	CDlgWorkSchduleCompare(CWorkScheduleHolder* pHolder, CWnd* pParent=NULL);
	virtual ~CDlgWorkSchduleCompare();

	enum { IDD = IDD_DLG_SCHDULE_COMPARE_WORK};
public:
	void InitDlgCtrlItem();
	
protected:
	CWorkScheduleHolder* m_pHolder;
	
	CButton m_btnReCheck;

	CButton m_btnGaoTie, m_btnPuKe, m_btnHuoche;
	CButton m_btnRadioThreeTime, m_btnRadioAllViewTime;
	CStatic m_static1, m_static2, m_static3, m_static4;
	
	CEdit m_edit1;
	
	void SetCompareStation();
	void InitGridStation();
	void GetCompareTimeRange(TIME& btm, TIME& etm, TIME& sTm);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	virtual BOOL OnInitDialog();
	void  OnClose();
	void  OnCancel();
	
	DECLARE_MESSAGE_MAP()
	
	void ReCompare();
	void getCompareResultXml(CString& timexml,CString& sidexml,CString& routexml);
	void compareWithBasic();
	void compareWithShift(TIME bTM, TIME eTM);
	void generateObjInfo();
	void compareBasicWithWork(const ResultInfoForWork& basicinfo, int idx);
	void compareShiftWithWork(const ResultInfoForWork& shiftinfo, int idx);
	bool IsCompareThisTrain(CTG_SingleTrain* ptrain);
private:
	CTgDocument *m_pDoc;
	HWND m_hViewWnd;
	CStationSelGrid m_staSelGridCtrl;
	CStationTimeListGridForWork m_trainTimeGrid;
	CStationSideListGridForWork m_trainSideGrid;
	CStationRouteListGridForWork m_trainRouteGrid;

	int station_count;
	ResultListForWork m_result[128];
};
