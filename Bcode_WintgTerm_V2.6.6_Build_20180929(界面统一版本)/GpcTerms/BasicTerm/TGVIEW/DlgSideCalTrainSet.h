#pragma once

struct TrainTypeNode
{
	int id;
	CString name;
};
class CDlgSideCalTrainSet : public CDialog
{
public:
	CDlgSideCalTrainSet(int section, CWnd* pParent = NULL);
	virtual ~CDlgSideCalTrainSet();

	enum { IDD = IDD_CAL_SIDE_TRAIN_SET};

protected:
	CButton m_btnSave;
	CListBox m_list1, m_list2, m_list3;
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSave();

	DECLARE_MESSAGE_MAP()
private:
	int m_section;
	int m_count;
	TrainTypeNode m_traintype[128];
public:
	afx_msg void OnDisplayTrainNoRange();
	afx_msg void OnAppendSelectTrainType();
	afx_msg void OnDeleteSelectTrainType();
};
