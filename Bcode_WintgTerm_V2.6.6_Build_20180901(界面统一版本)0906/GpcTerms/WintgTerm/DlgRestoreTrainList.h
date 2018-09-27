#pragma once

#include "traingrid.h"
#include "markgrid.h"
#include "tg_schedule_base.h"
// CDlgRestoreTrainList 对话框
#include "utility/xmlview.h"
class CTrainInfoGrid:public CXMLDataGrid
{
	enum 
	{
		DATA_OPER_ADD=1,
		DATA_OPER_UPDATE=2,
		DATA_OPER_DELETE=3
	};
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	int   InsertTrain(const char *id,CString& operstr,const CTG_SingleTrain *ptrain);
};

class CDlgRestoreTrainList : public CDialog
{
	DECLARE_DYNAMIC(CDlgRestoreTrainList)

	CTrainInfoGrid  m_grid;
	CMarkGridView  m_mark_grid;
	BOOL            m_bAllSelected;
public:
	CDlgRestoreTrainList(CTG_TrainScheduleBase *psourcebase, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRestoreTrainList();

// 对话框数据
	enum { IDD = IDD_DLG_RESTORE_TRAIN };
	CTG_TrainScheduleBase  m_trainbase;
	CTG_TrainScheduleBase  *m_pSourceDataBase;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnQuery();
	afx_msg void OnBnClickedChkAll();
	afx_msg void OnBnClickedOk();
	BOOL    OnInitDialog(); 
};
