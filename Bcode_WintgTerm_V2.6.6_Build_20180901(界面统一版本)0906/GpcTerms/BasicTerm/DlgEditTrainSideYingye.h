#pragma once
#include "utility/xmlview.h"
#include "tg_schedule_base.h"
//操作Excel表格所需要的头文件
#include "CApplication.h"
#include "CWorkbook.h"
#include "CWorksheet.h"
#include "CWorkbooks.h"
#include "CWorksheets.h"
#include "CRange.h"



class  CStationListGridView:public CXMLDataGrid
{
private:
	struct PropertyInfoPara  m_train_record_def[16];
	int m_record_col_count;
	CTG_TrainScheduleBase *m_pTrainDataBase;
public:
	virtual void  BeginEditCell(int row,int col);
    virtual void  EndEditCell(int row,int col);

public:
	void  SetDataBase(CTG_TrainScheduleBase * pTrainDataBase) 
	{
		m_pTrainDataBase=pTrainDataBase;
	}

	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	BOOL  InsertRows(const char *xml,const char *rootnode);
};

class  CXlsListGridView:public CXMLDataGrid
{
private:
	struct PropertyInfoPara  m_train_record_def[16];
	int m_record_col_count;
public:
	virtual void  BeginEditCell(int row,int col);
    virtual void  EndEditCell(int row,int col);

public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	BOOL  InsertRows(const char *xml,const char *rootnode);
};

class CTgDocument;
// CDlgEditTrainSideYingye 对话框
class CDlgEditTrainSideYingye : public CDialog
{
public:
	CDlgEditTrainSideYingye(CWnd* pParent = NULL);
	virtual ~CDlgEditTrainSideYingye();
	
	enum { IDD = IDD_DLG_SIDE_YINGYE_EDIT};
	typedef struct tagStaSide{
		char xlsIndex[4];
		char xlsTrainId[10];
		char xlsYingye[4];
		char xlsSide[10];
		char xlsDate[64];
		char ctcSide[10];
	}STASIDE, *LPSTASIDE;

	typedef struct tagStaInfo{
		LPSTASIDE detail;
		int  count;
		char name[32];
	}STAINFO, *LPSTAINFO;
public:
	void InitDlgCtrlItem();
	void SetDocument(CTgDocument *pdoc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
	void  OnClose();
	void  OnCancel();

	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedReload();
	afx_msg void OnBnCheckStationSideAck();
	afx_msg void OnCbnSelchangeComboStation();
	CString ModifyStationSide(CString trainname, CString stryingye, int station, BYTE sideno, int row);
private:
	enum XLSDATTYPE{UNKOWN, INDEX, STATION, TRAINID, YINGYE, SIDE, DATE};
	typedef struct tagStationCellPos{
		char name[32];//车站名
		int  pos;//三列信息的cell位置
	}STATIONCELL, *LPSTATIONCELL;

	CTG_TrainRecord *GetRowTrainRecord(int row);
	bool  GetSelTrains(CTG_TrainPtrGroup& group, CString& str);
	

	BOOL LoadXlsFile(const char* lpstrfilename);
	void LoadXlsCell(CRange &mRange, CRange &usedRange, int nW, int nH);
	XLSDATTYPE GetCellDataType(CString &strCell)const;
	void ParseXlsCell(LPCTSTR strIndex, LPCTSTR strTrainID, const map<int, XLSDATTYPE> &cellType, vector<STATIONCELL> &cellSt, LPCTSTR strCell, int col);
	inline const LPSTATIONCELL CellInStation(const vector<STATIONCELL> &cellSt, int col)const;
	inline LPSTAINFO StationInInfo(const char *szName)const;
	inline BOOL CellInTrainID(const map<int, XLSDATTYPE> &cellTrainID, int col)const;
	void DeleteXlsData();
	void ShowXlsData(int nStaSel);
private:
	CComboBox mStationCombo;
	CStatic m_xls;
	CTgDocument *m_pDoc;
	CTG_TrainScheduleBase *m_pTrainDataBase;
	CXlsListGridView m_gridxls;

	int  station_count;
	int  station_list[256];
	int  m_nOldSelSta;

	vector<LPSTAINFO>	m_vecStaInfo;
public:

	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonChooseexcel();
};
