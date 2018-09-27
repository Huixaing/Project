#pragma once

#include ".\Logo.h"
// CQuitSystem 对话框

class CQuitSystem : public CDialog
{
	DECLARE_DYNAMIC(CQuitSystem)

public:
	CQuitSystem(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CQuitSystem();

// 对话框数据
	enum { IDD = IDD_DIALOG_QUIT };
	CBitmap mBmp;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	CLogo m_logoExit;
};
