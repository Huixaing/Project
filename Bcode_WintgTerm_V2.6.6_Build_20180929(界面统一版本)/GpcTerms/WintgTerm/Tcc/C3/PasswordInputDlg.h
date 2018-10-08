#pragma once


#include "afxwin.h"
#include "resource.h"

namespace TCC_C3
{

class CPasswordInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CPasswordInputDlg)

public:
	CPasswordInputDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPasswordInputDlg();

// 对话框数据
	enum { IDD = IDD_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

private:
	CString m_szPasswd;

public:
	virtual BOOL OnInitDialog();

	CString GetPassword() { return m_szPasswd; }
};

}