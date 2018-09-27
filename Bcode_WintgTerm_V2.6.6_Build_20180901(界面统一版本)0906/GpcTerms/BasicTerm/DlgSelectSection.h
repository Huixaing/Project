#pragma once


// CDlgSelectSection 对话框

class CDlgSelectSection : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectSection)

public:
	CDlgSelectSection(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSelectSection();
	int   m_nNeighborOfSection;
	int   m_nSectionNo;
// 对话框数据
	enum { IDD = IDD_SELECT_SECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	BOOL OnInitDialog(void);
public:
	afx_msg void OnBnClickedOk();
};
