#pragma once


// CDlgMarkResult 对话框

class CDlgMarkResult : public CDialog
{
	DECLARE_DYNAMIC(CDlgMarkResult)
	CString  m_szCaption;
	CString  m_szInform;
public:
	CDlgMarkResult(const char *caption,const char *inform,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMarkResult();
	const char *GetInputValue();
	// 对话框数据
	enum { IDD = IDD_DLG_MMPLAN_MARK_RESULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();
	void OnOK();
	DECLARE_MESSAGE_MAP()
};
