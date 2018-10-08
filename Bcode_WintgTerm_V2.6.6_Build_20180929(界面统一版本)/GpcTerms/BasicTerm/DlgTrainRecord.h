#pragma once
#include "utility/xmlview.h"
#include "tg_schedule_base.h"

#define SetPassNotMainSide(side_valide)          (side_valide |= 0x0001)              //bit 0
#define IsPassNotMainSide(side_valide)           (side_valide & 0x0001)              //bit 0

#define SetSideDisconnectEntry(side_valide)          (side_valide |= 0x0002)              //bit 1
#define IsSideDisconnectEntry(side_valide)           (side_valide & 0x0002)              //bit 1

#define SetSideNotSame(side_valide)          (side_valide |= 0x0004)              //bit 2
#define IsSideNotSame(side_valide)           (side_valide & 0x0004)              //bit 2

#define SetNotHighPlatform(side_valide)          (side_valide |= 0x0008)              //bit 3
#define IsNotHighPlatform(side_valide)           (side_valide & 0x0008)              //bit 3

#define SetNoPlatform(side_valide)          (side_valide |= 0x0010)              //bit 4
#define IsNoPlatform(side_valide)           (side_valide & 0x0010)              //bit 4

#define SetNotStopThisClassTrain(side_valide)          (side_valide |= 0x0020)              //bit 5
#define IsNotStopThisClassTrain(side_valide)           (side_valide & 0x0020)              //bit 5

#define SetNotStopCRHTrain(side_valide)          (side_valide |= 0x0040)              //bit 6
#define IsNotStopCRHTrain(side_valide)           (side_valide & 0x0040)              //bit 6

#define SetNotGiveSide(side_valide)          (side_valide |= 0x0080)              //bit 7
#define IsNotGiveSide(side_valide)           (side_valide & 0x0080)              //bit 7

#define SetNotSpaceTrain(side_valide)          (side_valide |= 0x0100)              //bit 8
#define IsNotSpaceTrain(side_valide)           (side_valide & 0x0100)              //bit 8

#define SetNotCheckSide(side_valide)          (side_valide |= 0x80000000)              //bit 31
#define IsNotCheckSide(side_valide)           (side_valide & 0x80000000)              //bit 31

struct NodeRD
{
	DWORD train_index;
	int   station;
	int   rec_index;
	int   arrive_side;
	int   depart_side;
	int   arrive_depart_same;
	int   status;
	DWORD   side_valide;
	NodeRD()
	{
		train_index=0;
		station=0;
		rec_index=-1;
		arrive_side=-1;
		depart_side=-1;
		arrive_depart_same=0;
		status=0;
		side_valide=0;
	}
};

class  CTrainListGridView:public CXMLDataGrid
{
private:
	struct PropertyInfoPara  m_train_record_def[16];
	int m_record_col_count;
	CTG_TrainScheduleBase *m_pTrainDataBase;
	CTG_TrainScheduleBase *m_pBasicTrainDataBase;
public:
	virtual void  BeginEditCell(int row,int col);
    virtual void  EndEditCell(int row,int col);

public:
	void  SetScheduleBase(CTG_TrainScheduleBase *pTrainDataBase, CTG_TrainScheduleBase *pBasicTrainDataBase);
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid, bool bShiftOrPreBasic);
	int  ShowTrainData(CTG_TrainPtrGroup *pGroup, int station, int flag);
	
private:
	void ShowNotAckStationSide();
	CString GetStationTrainSide(CTG_TrainRecord *prec, int rec_index, long train_index, DWORD side_valide,CStringArray& list); 
	void CheckSideValid(CTG_SingleTrain *ptrain, int n, CTG_TrainRecord *prec, DWORD& side_valide);
public:
	bool m_bCheckShiftTrainAddTime;
	int stationNum;
	int stationNo[128];
	int exitNullSide[128];

private:
	std::vector<NodeRD> mlistRD;

public:
	enum{
		SHOW_ST_SIDE=1,
		CHECH_SIDE_VALID=2,
		CHECH_ST_MAN_CONFIRM_SIDE=3,
	};
};

class CTgDocument;
// CDlgTrainRecord 对话框
class CDlgTrainRecord : public CDialog
{
public:
	CDlgTrainRecord(CTG_ScheduleInfo::SCHEDULE_TYPE schd_type, CWnd* pParent = NULL);
	virtual ~CDlgTrainRecord();
	
	enum { IDD = IDD_DLG_SIDE_RECORD};
public:
	void InitDlgCtrlItem();
	void SetDocument(CTgDocument *pdoc, int flag);
	bool IsNeedCheckStation(int station) const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void  OnSize(UINT nType,int cx,int cy);
	void  OnClose();
	void  OnCancel();
	int   OnCreate(LPCREATESTRUCT lpCreat);
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedReload();
	afx_msg void OnBnCheckStationSideAck();
	afx_msg void OnCbnSelchangeComboStation();
	afx_msg void OnBnClickedCheckTrainSide();
private:
	CTG_TrainRecord *GetRowTrainRecord(int row);
	bool  GetSelTrains(CTG_TrainPtrGroup& group, CString& str);
private:
	CTG_ScheduleInfo::SCHEDULE_TYPE m_schd_type;
	CComboBox mStationCombo;
	CTgDocument *m_pDoc;
	CTG_TrainScheduleBase *m_pTrainDataBase;
	CTG_TrainScheduleBase *m_pBasicDataBase;
	CTrainListGridView m_grid;
	int         m_nFlag;
	int         m_selStation;
	int  station_count;
	int  station_list[256];
};
