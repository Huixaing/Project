#pragma once
#include "tgmsg.h"                  //消息和数据结构定义
#include "resource.h"
#include "GridCtrl_src/GridCtrl.h"  //包含表格控件的类
#include "messages.h"
#include "function.h"
#include "memshare.h"
#include "Tooltipwnd.h"
#include ".\ext_src\CJFlatComboBox.h"  //office 风格的COMBOX
#include "Dlgmainfrm.h"
#include "afxdtctl.h"
#include "editex.hxx"
#include ".\GridCtrl_src\gridcellcheck.h"
//#include "utility/pspview.h"
#include "utility/xmlview.h"
class CTgDocument;

////////////// 行车概况////////
/////////////////////////////////////////////////////////////////////////////
// CShiguOutlineDlg dialog
// CDispatcherCommandModule dialog
 
#define ACCIDENT_MODULE_NUM		256
struct _AccidentModuleDetails
{
	char no[8];//类型编号
	char type_name[256];//类型名称
	char module_content[1800];//模板内容
};
struct _AccidentModule
{
	int module_num;//模板中的数
	struct _AccidentModuleDetails details[ACCIDENT_MODULE_NUM];
};
#define MAX_ACCIDENT 10

class CShiguOutlineDlg : public CPropertyPage
{
// Construction
public:
	CShiguOutlineDlg(CTgDocument   *pdoc);
	virtual ~CShiguOutlineDlg();

public:
	CGridCtrl *m_pGridCtrl;
	
	CXMLDataGrid m_pspCtrl;
    int		m_nFixCols;   //固定列
	int		m_nFixRows;   //固定行
	int		m_nCols;      //列
	int		m_nRows;      //行
	int     nAccident;
	int     CurrentRow;
	int     m_nSchdtype;   
	int 	m_RecordsPerPrintPage ;
	int     m_nSectionID,m_nShiftID;
	ACCIDENT_INFO accident[MAX_ACCIDENT];
     _AccidentModule sAM;
// Dialog Data
	//{{AFX_DATA(CShiguOutlineDlg)
	enum { IDD = IDD_DLG_SHIGU_OUTLINE };
	CComboBox	m_Ctrlmodule;
	CEdit	m_EditDetail;
	CStatic	m_Frame;
	CString	m_title;
	CString	m_detail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShiguOutlineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitIdentify();
	bool   ReadModuleFile(char *lpszFileName);
	void SortModuleFile();
	void PrintEveryPageInfo(CDC *pDC, CPrintInfo *pInfo);
	int GetMaxIndex();
    void Query_Accident();
	void Del_Accident(ushort nIndex);
	void Add_OR_Change_Accident(int type,ushort nIndex);
	// Generated message map functions
	//{{AFX_MSG(CShiguOutlineDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddNewDetail();
	afx_msg void OnDblclkList();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnPrint();
	afx_msg void OnSelendokCmbModule();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam); 
	DECLARE_MESSAGE_MAP()
private:
	void SendMsgAccidentOper(ushort nOpetype,ushort nIndex);
	BOOL ProcessMessage(CLBTH::MsgAccidentOper *pmsg);
	CWnd *m_pNotifyWindow;
	CTgDocument   *m_pDoc;
	CString  m_xmlcontent;
public:
	void SetNotifyWindow(CWnd *p) {m_pNotifyWindow=p;}
	void InformParent();
};


