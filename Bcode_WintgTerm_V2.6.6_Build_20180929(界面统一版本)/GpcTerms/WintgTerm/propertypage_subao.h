#pragma once
#include "tgmsg.h"                  //消息和数据结构定义
#include "resource.h"
#include "GridCtrl_src/GridCtrl.h"  //包含表格控件的类
#include "messages.h"
#include "function.h"
#include "memshare.h"
#include <baseMsg/gpc_msgid.h>
#include ".\ext_src\CJFlatComboBox.h"  //office 风格的COMBOX
#include <BaseMsg/clbth.h>
#include "editex.hxx"
#include "utility/locoinput.h"
class CTgDocument;
class CPageSUBAO : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageSUBAO)

// Construction
public:
	int m_RecordsPerPrintPage;
	void PrintGroupInfo(CDC *pDC, CPrintInfo *pInfo);
	int GetSubaoArrayIndexByRowNo(int rowno);
	BOOL DeleteFiles(CString fileDir,time_t elapsedtime,CString  postfix);
	BOOL ReadSuBaoFromFile(const CString& localFilePath);
	BOOL OnRowDataUpdate();
	void SetDocument(CTgDocument * pdoc);
	int m_nSelectedSel;
	unsigned long select_shift_id,prev_shift_id,next_shift_id,curr_shift_id;
	int schd_type;
	CTgDocument * m_pDoc;

	void InitIdentify();
	unsigned long GetTimeidx_SelectedRow(int rownum);
	void ToFocusRow(int rowNum);
	unsigned long GetTimeidx();
	void SetBlankCells(int Start);
	BOOL Query_Subao(unsigned long select_shift_id);
	void ClearEdit();
	BOOL Set_Subao(int timeidx,ushort oper);
	BOOL Show_Subao(int num,int row);
	int GetFocusTrain(int timeidx);
 	CString ConvertShiftIdToString(unsigned long id);
	void ShowTrainInEdits(int nRow);

	CPageSUBAO();
	virtual ~CPageSUBAO();
	CGridCtrl *m_pGridCtrl;

	CEditEx m_edittemp1;
	CEditEx m_edittemp2;
	CEditEx m_edittemp3;
	CEditEx m_edittemp4;

	CLocoInputCtrl m_locoInputEngine1;
	CLocoInputCtrl m_locoInputEngine2;
	CLocoInputCtrl m_locoInputAttachEngine1;
	CLocoInputCtrl m_locoInputAttachEngine2;
	CLocoInputCtrl m_locoInputAttachEngine3;
	CLocoInputCtrl m_locoInputAttachEngine4;

	//CGridLCBZ *m_pGridCtrl;
    int		m_nFixCols;   //固定列
	int		m_nFixRows;   //固定行
	int		m_nCols;      //列
	int		m_nRows;      //行
	int nPlotItemList[10];       //各个编组栏目的编号
	int nPlotItemNum;

	DWORD  m_nCurrentTrainIndex;
    //CToolTipWnd m_ctrlTooltipWnd;
// Dialog Data
	//{{AFX_DATA(CPageSUBAO)
	enum { IDD = IDD_PROPPAGE_NOTE_GROUP };
	CComboBox	m_cmbShiftId;
	CButton	m_Frame1;
	CCJFlatComboBox	m_CboDirectionsCtrl;
	CCJFlatComboBox	m_CboDdtCtrl;
    CCJFlatComboBox m_CboDirectionCtrl;
	CString	m_train_leader_name;
	CString	m_train_id;
	short	m_total_count;
	CString	m_tail_no;
	short	m_ck_count;
	CString	m_cover;
	short	m_cz_count;
	CString	m_dest;
	CString	m_driver_name;
	short	m_gk_count;
	short	m_gz_count;
	short	m_k_count;
	short	m_o_count;
	short	m_pik_count;
	short	m_piz_count;
	short	m_pk_count;
	short	m_pz_count;
	short	m_s_count;
	CString	m_train_id_f;
	CString	m_find_result;
	float	m_cal_len;
	float	m_total_weight;
	BOOL	m_rowdatachanged;	//判断该行数据是否已经被改变，如果是，那么需要发送消息进行保存
	int		m_previousrow;		//上一次的行号
	CString	m_engine_id;
	CString	m_traction_engine1;
	CString	m_traction_engine2;
	CString	m_attach_engine1;
	CString	m_attach_engine2;
	CString	m_attach_engine3;
	CString	m_attach_engine4;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageSUBAO)
	public:
	virtual void OnOK();
	virtual BOOL OnApply();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SortSubaoByColIndex(int colNum);

	int Timeidx_Selected;
	void OnTimer(UINT_PTR IDEvent);
	void Calculate();
	// Generated message map functions
	//{{AFX_MSG(CPageSUBAO)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
    afx_msg void OnDelete();
    
	afx_msg void OnDirectionUp();
    afx_msg void OnDirectionDown();
	afx_msg void OnDirectionAll();

	afx_msg void OnNew();
	afx_msg void OnFindTrain();
	afx_msg void OnKillfocusKCount();
	afx_msg void OnKillfocusPkCount();
	afx_msg void OnKillfocusPzCount();
	afx_msg void OnKillfocusCkCount();
	afx_msg void OnKillfocusCzCount();
	afx_msg void OnKillfocusPikCount();
	afx_msg void OnKillfocusPizCount();
	afx_msg void OnKillfocusGkCount();
	afx_msg void OnKillfocusGzCount();
	afx_msg void OnKillfocusOCount();
	afx_msg void OnKillfocusSCount();
	afx_msg void OnChange();
	
	afx_msg void OnSelendokComboDdtname();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelendokComboDirections();
	afx_msg void OnSelchangeCmbShiftid();
	afx_msg void OnGridItemChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnSetfocusTrainId();
	afx_msg void OnSetfocusTrainIdF();
	afx_msg void OnSetfocusEngineType();
	afx_msg void OnSetfocusDriverName();
	afx_msg void OnSetfocusTrainLeaderName();
	afx_msg void OnBtnImport();
	afx_msg void OnPrintGroup();
	afx_msg void  OnDestroy();

	//}}AFX_MSG
   
	LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam);
	LRESULT OnSchdTrainSelected(WPARAM wp,LPARAM lp);
	DECLARE_MESSAGE_MAP()

	BOOL  ProcessMessage(CLBTH::MsgSubaoFormat *pBuf);
	int   GetRowByTrainIndex(DWORD index);
private:
	int GetActualRowNo(int curRowNo, int dir);
	int m_nSelectedSectionID;
	int m_nDefaultSectionID;
	int m_nCurrentRow;          
    int m_nDirection;
	int m_nUpCounts;           
	int m_nDownCounts;
	int m_nTrainCounts;
	BOOL m_bReadOnly;
private:
	CWnd *m_pNotifyWindow;
public:
	void InformParent();
	void SetNotifyWindow(CWnd *p) {m_pNotifyWindow=p;}

};