#pragma once
#include "utility/xmlview.h"

struct NodeSideInfo
{
	TRAIN_INDEX train_index;
	BYTE rec_index;
	BYTE a_side;
	BYTE d_side;
	CString trainid;
	CString s_aside, s_dside;
};

class  CSideCHGGrid:public CXMLDataGrid
{
private:
	struct PropertyInfoPara  m_result_def[10];
	int m_result_col_count;
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  AddSideDisplayRow(const NodeSideInfo& info, CString stationname, BOOL choice);
};

class CDlgStationReqChangeSide : public CDialog
{
public:
	CDlgStationReqChangeSide(int station, CWnd* pParent = NULL);
	virtual ~CDlgStationReqChangeSide();

	enum { IDD = IDD_DLG_STATION_REQ_CHANGE_SIDE};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	
	DECLARE_MESSAGE_MAP()

public:
	void AddTrainSideRecord(const NodeSideInfo& info);

private:
	CEdit m_station;
	CSideCHGGrid mGridCtrl;
public:
	int m_nStationNo;
	int m_count;
	NodeSideInfo siderecord[512];
};
