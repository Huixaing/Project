#pragma once
#include "afxwin.h"

#include "resource.h"
#include "tgdoc.h"
#include "afxcmn.h"
// CModifyStationIndex 对话框

class CModifyStationIndex : public CDialog
{
	DECLARE_DYNAMIC(CModifyStationIndex)

public:
	CModifyStationIndex(CTGDoc *pDoc, CTG_TRAIN* train, CWnd* pParent);   // 标准构造函数
	virtual ~CModifyStationIndex();

// 对话框数据
	enum { IDD = IDD_DLG_MODIFY_TRAIN_INOUT_INDEX };

protected:
	afx_msg void OnBnClickedModify();
	afx_msg void OnBnClickedCancel();
	virtual void PostNcDestroy();
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()

protected:
	CComboBox m_inStationBox;
	CComboBox m_outStationBox;

private:
	CTGDoc *m_pDoc;
	CTG_TRAIN mTrain;
	void ShowTrain();

public:
	int m_nRecIndex;
	int m_train_index;
	CListCtrl m_list;
};
