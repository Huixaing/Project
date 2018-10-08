#pragma once

#include "tg_logic\tg_schedule_base.h"
class CWintgViewMMI;
// CDlgDirectSetTrigFlag 对话框

class CDlgDirectSetTrigFlag : public CDialog
{
	DECLARE_DYNAMIC(CDlgDirectSetTrigFlag)

public:
	CDlgDirectSetTrigFlag(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDirectSetTrigFlag();

// 对话框数据
	enum { IDD = IDD_DLG_DIRECT_SET_AUTOTRIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	BOOL   OnInitDialog();
private:
	CWintgViewMMI   *m_pTGMMI;
	CTG_SingleTrain *m_pTrain;
	int              m_nRecIndex;

	CString          m_szCommStatusStr;
	int			 	m_nModifyArriveOk;
	int             m_nModifyDepartOk;
	int             m_nTryCount;
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnSetArrive();
	afx_msg void OnBnClickedBtnSetDepart();
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam);
	void    OnDestroy();
	void    SetTrain(CWintgViewMMI   *pmmi,CTG_SingleTrain *ptrain,int rec_index);
	void    RefreshData();
};
