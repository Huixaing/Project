#pragma once
#include "afxwin.h"
#include "mytree.h"

class  CMyListBox : public CListBox{
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct); 
	virtual int  CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
};


// CDlgElecArms 对话框
#define MAX_ARM_NUM  256

class CDlgElecArms : public CDialog
{
	DECLARE_DYNAMIC(CDlgElecArms)

public:
	CDlgElecArms(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgElecArms();

// 对话框数据
	enum { IDD = IDD_DIG_ARMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void    SetSectionID(int nSectionId,BOOL readonly);
	CMyTree    m_treeArms;
	BOOL       m_bReadOnly;
private:
    char m_szTreeRoot[16];
	struct{
		int  nArmNo;
		unsigned char nStatus;
		int  nbAgree;
	}m_aArmConfigs[MAX_ARM_NUM];
	int m_nArmCount;
	int m_aStationList[128];
	int m_nStationCount;
	int m_nSectionID;
	BOOL  LoadArmConfig(const char *file);
	void  ChangeArmStatus(BOOL enable);
	void  ChangeArmStatus(int armno, BOOL enable);
	void  ChangeArmStatus(int armno, WORD station, BOOL enable);

public:
	virtual BOOL OnInitDialog();
    void    RefreshList();
	afx_msg void OnBnClickedSetEnable();
	afx_msg void OnBnClickedSetDisable();
	afx_msg void OnBnClickedCancel();
	//CComboBox m_cboStationList;
	BOOL m_bSendToStation;
	afx_msg void OnNcDestroy();
	afx_msg void OnBnClickedRefreshRemote();
	int m_nStationIndex;
	afx_msg void OnTvnSelchangedListArms(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam); 
	void ReceiveMsgElecArmSet(CLBTH::MsgElecArmSet  *pArm); 
};
