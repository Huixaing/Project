#pragma once


// CDlgSelectShift 对话框
class CTgDocument;
class CDlgSelectShift : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectShift)
	CTgDocument *m_pDoc;
	int m_nNextShiftID;
public:
	CDlgSelectShift(CWnd* pParent, int nShiftID);   // 标准构造函数
	virtual ~CDlgSelectShift();


// 对话框数据
	enum { IDD = IDD_DLG_SELECT_SHIFT };
 
	int   m_nShiftID;
	
	TIME  m_ntStartTime;
	TIME  m_ntEndTime;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	
public:
	void OnOK();
};
