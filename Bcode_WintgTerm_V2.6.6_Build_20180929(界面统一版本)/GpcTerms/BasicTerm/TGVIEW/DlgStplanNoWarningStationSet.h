#pragma once
#include "utility/xmlview.h"
class  CStationSelectGrid:public CXMLDataGrid
{
private:
	struct PropertyInfoPara  m_result_def[10];
	int m_result_col_count;
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  AddRow(int station, int icheck);
};

class CDlgStplanNowarningStationSet : public CDialog
{
public:
	CDlgStplanNowarningStationSet(int section, int station_count, int m_station_list[256], CWnd* pParent = NULL);
	virtual ~CDlgStplanNowarningStationSet();

	enum { IDD = IDD_DLG_STPLAN_NOWARNING_STATION_SET};

protected:
	CButton m_btnSave;
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSave();

	DECLARE_MESSAGE_MAP()
private:
	int m_section;
	CStationSelectGrid mGridCtrl;
	int m_station_count;
	int m_station_list[256];
	int m_check_list[256];
};
