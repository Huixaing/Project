#pragma once
#include "utility/xmlview.h"
class  CStationCheckGrid:public CXMLDataGrid
{
private:
	struct PropertyInfoPara  m_result_def[10];
	int m_result_col_count;
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  AddRow(int station, int b, int s, int w);
};

class CDlgSchdCompareStationSet : public CDialog
{
public:
	CDlgSchdCompareStationSet(int section, CWnd* pParent = NULL);
	virtual ~CDlgSchdCompareStationSet();

	enum { IDD = IDD_DLG_COMPARE_STATION_SET};

protected:
	CButton m_btnSave;
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSave();

	DECLARE_MESSAGE_MAP()
private:
	int m_section;
	CStationCheckGrid mGridCtrl;
	int m_station_count;
	int m_station_list[128];
};
