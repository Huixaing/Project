#pragma once

#include "resource.h"
#include "afxwin.h"
#include "tg_autoadjust.h"
#include "utility/xmlview.h"
#include "tg_schedule_base.h"
#include "TgDocument.h"

// DlgBatchSideModify 对话框
class DlgBatchSideModify;
class CSideGridView: public CXMLDataGrid
{
private:
	DlgBatchSideModify* m_pDlg;
	struct PropertyInfoPara  m_train_record_def[16];
	int m_record_col_count;
public:
	virtual void  BeginEditCell(int row,int col);
    virtual void  EndEditCell(int row,int col);

public:
	void  SetParentDlg(DlgBatchSideModify* pDlg);
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  DisplaySideInfo(const char* xml,const char *rootnode);
};

class DlgBatchSideModify : public CDialog
{
	DECLARE_DYNAMIC(DlgBatchSideModify)

public:
	DlgBatchSideModify(const CTG_SingleTrain& train,CString strCaption, CTG_TrainScheduleBase *pBasicData, CTG_TrainScheduleBase *pData, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgBatchSideModify();

// 对话框数据
	enum { IDD = IDD_DLG_BATCH_SIDE_MODIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	CSideGridView m_sideGrid;
	CString m_strCaption;
	CTG_TrainScheduleBase* m_pData, *m_pBasicData;
	std::vector<int> m_thisStation;
public:
	CTG_SingleTrain m_train;
	
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	bool IsThisStation(int station);

public:
	void SetThisStation(std::vector<int>& thisStation);
	void DisplayTrainSide();
	bool ModifySide(int rec_index, int station, int sideno);
	void CheckSideInfo();
	afx_msg void OnBnClickedOk();
};
