#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "tgdoc.h"
// CProhibitMeetDlg 对话框
class CProhibitMeetDlg : public CDialog
{
	DECLARE_DYNAMIC(CProhibitMeetDlg)

public:
	CProhibitMeetDlg(CWnd* pParent);   // 标准构造函数
	virtual ~CProhibitMeetDlg();

// 对话框数据
	enum { IDD = IDD_DLG_PROHIBIT_MEET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// Generated message map functions
	//{{AFX_MSG(CProhibitMeetDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl m_list;
	CComboBox mBeginSta;
	CComboBox mEndSta;
	CComboBox mNoMeet;

	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnUpdate();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnSelListRow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	
	void SendTrainUpdate();
	void SetCenterPosition();

    CTG_TRAIN m_train;
	CTG_TRAIN* m_pTrain;
private:
	void ShowMeet();
	void SetValue();
	void ResetValue();

	int mBIdx, mEIdx;
	WORD nNotMeetFlag;

public:
	CTGDoc* m_pDoc;
	ULONG m_train_index;
protected:
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedCancel();
};
