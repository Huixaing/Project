#pragma once
#include "utility/xmlview.h"
#include "tg_schedule_base.h"

#define SetRecordCountDifferent(train_valide)    (train_valide |= 0x0001)              //bit 0 节点数量不同
#define IsRecordCountDifferent(train_valide)     (train_valide & 0x0001)              //bit 0

#define SetPathDifferent(train_valide)          (train_valide |= 0x0002)              //bit 1 经由车站不同
#define IsPathDifferent(train_valide)           (train_valide & 0x0002)              //bit 1

#define SetTrainExistDifferent(train_valide)          (train_valide |= 0x0004)              //bit 2 列车不在基本图存在
#define IsTrainExistDifferent(train_valide)           (train_valide & 0x0004)              //bit 2

#define SetEntryDifferent(train_valide)          (train_valide |= 0x0008)              //bit 3 入口不同
#define IsEntryDifferent(train_valide)           (train_valide & 0x0008)              //bit 3

#define SetExitDifferent(train_valide)          (train_valide |= 0x0010)              //bit 4 出口不同
#define IsExitDifferent(train_valide)           (train_valide & 0x0010)              //bit 4

struct NodeTrain
{
	DWORD train_index;
	DWORD basic_index;
	DWORD train_valide;
	NodeTrain()
	{
		train_index=0;
		basic_index=0;
		train_valide=0;
	}
};

class  CTrainRouteCheckListGrid:public CXMLDataGrid
{
private:
	struct PropertyInfoPara  m_train_def[8];
	int m_train_col_count;
	CTG_TrainScheduleBase *m_pTrainDataBase;
	CTG_TrainScheduleBase *m_pBasicTrainDataBase;

public:
	void  SetScheduleBase(CTG_TrainScheduleBase *pTrainDataBase, CTG_TrainScheduleBase *pBasicTrainDataBase);
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	int  ShowTrainData(CTG_TrainPtrGroup *pGroup, int flag, CTG_ScheduleInfo::SCHEDULE_TYPE schd_type);
	
	
private:
	void ShowTrainDifferent();
	CString GetTrainResult(long train_index, long basic_index, CString trainid, CString& strStName, CString& strTm, DWORD side_valide); 

private:
	std::vector<NodeTrain> mlistTrain;

public:
	enum{
		SHOW_ALL_TRAIN=1,
		SHOW_WARNING_TRAIN=2,
	};
};


class  CTrainRouteCheckResultGrid:public CXMLDataGrid
{
private:
	struct PropertyInfoPara  m_result_def[10];
	int m_result_col_count;

public:
	void  SetTrainResult(CTG_SingleTrain *currTrain, CTG_SingleTrain *basicTrain);
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  ShowTrainData();
//
private:
	void ShowTrainsDifferent();
//	CString GetTrainResult(long train_index, CString trainid, DWORD side_valide); 

private:
	CTG_SingleTrain *m_currTrain;
	CTG_SingleTrain *m_basicTrain;
};


class CTgDocument;
class CDlgTrainRouteCheck : public CDialog
{
	DECLARE_DYNCREATE(CDlgTrainRouteCheck)
public:
	CDlgTrainRouteCheck(CWnd* pParent = NULL);
	virtual ~CDlgTrainRouteCheck();

	enum { IDD = IDD_DLG_ROUTE_CHECK};
public:
	void InitDlgCtrlItem();
	void SetDocument(CTgDocument *pdoc,CTG_ScheduleInfo::SCHEDULE_TYPE schd_type, int flag);

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
	afx_msg void OnBnClickedCheckTrain();
	afx_msg void OnChangeTrainResult(NMHDR *pNotifyStruct, LRESULT* pResult);
private:
	CTG_SingleTrain *GetRowTrainRecord(int row);
	bool  GetSelTrains(CTG_TrainPtrGroup& group, CString& str);
private:
	CTgDocument *m_pDoc;
	CTG_TrainScheduleBase *m_pTrainDataBase;
	CTG_TrainScheduleBase *m_pBasicDataBase;
	CTrainRouteCheckListGrid m_traingrid;
	CTrainRouteCheckResultGrid m_resultgrid;
	int         m_nFlag;
	CTG_ScheduleInfo::SCHEDULE_TYPE m_schd_type;
};
