#pragma once


// CDlgSimpleInput 对话框

class CDlgSimpleInput : public CDialog
{
	DECLARE_DYNAMIC(CDlgSimpleInput)
	CString  m_szCaption;
	CString  m_szInform;
	CString  m_szValue;
public:
	CDlgSimpleInput(const char *caption,const char *inform,const char *defvalue,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSimpleInput();
	const char *GetInputValue();
// 对话框数据
	enum { IDD = IDD_DLG_SIMPINPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();
	void OnOK();
	DECLARE_MESSAGE_MAP()
};
