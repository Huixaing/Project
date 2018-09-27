#pragma once


#include "tcc_base.h"
#include "afxwin.h"
#include "afxdtctl.h"
#include <Tcc\Grid.h>
#include "resource.h"

namespace TCC_C3
{

class CDirectLimit : public CDialog
{
	DECLARE_DYNAMIC(CDirectLimit)

public:
	CDirectLimit(ITccClientInfoProvider *pInfoProvider, ITccAdviseSink *pAdviseSink, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDirectLimit();

// 对话框数据
	enum { IDD = IDD_DIRECT_LIMIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	RAW_STOCK_COMMAND m_cmd;
	BOOL  m_bCommandChecked;
	ITccClientInfoProvider *m_pInfoProvider;
	ITccAdviseSink *m_pAdviseSink;

private:
	CComboBox m_TccList;
	CButton m_btnNew, m_btnEdit, m_btnValid, m_btnSend;
	CString m_szCmdString;

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnNew();
	afx_msg void OnEdit();
	afx_msg void OnValid();
	afx_msg void OnSend();
	afx_msg void OnBnClickedBtnSketchMap();
	afx_msg void OnSketchMap();
};

}