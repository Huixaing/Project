#if !defined(AFX_RECEIVENEIGHBERPLANSHEET_H__87097944_0589_44EF_97E6_7430675415CC__INCLUDEDXX_)
#define AFX_RECEIVENEIGHBERPLANSHEET_H__87097944_0589_44EF_97E6_7430675415CC__INCLUDEDXX_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

#include <afxcmn.h>
#include "../PropTable/PropTree.h"
#include "ext_src/iectrl.h"
#include "utility/ctc_data_type_def.h"
#include "utility/mybutton.h"
class CTG_LogicObject;
class CTgView;
class CTG_SingleMark;
class CTG_SingleTrain;
class CTgDocument;
struct PropertyInfoParaTmp{
	int flag;   // 0:节点 1:STATIC 2: EDIT 3: LIST,4:BOOL
	int id;
	char caption[24];
};

#define TRAIN_PROPERTY_TAB_COUNT  2
#define TABINDEX_TRAIN_SUM     0
#define TABINDEX_TRAIN_INFO    1
#define TABINDEX_TRAIN_ASM     3

/////////////////////////////////////////////////////////////////////////////

class CSheetTGProperty : public CStatic
{
	DECLARE_DYNAMIC(CSheetTGProperty)
	CTgView  *m_pTgView;
	const CTG_LogicObject  *m_pCurrentObject;
	int m_nSelStationNo;
	int m_nSelStationRecIndex;
	int m_nSectionID;
	long m_nCurrentTrainIndex;
	CTabCtrl  m_tabctrl;
	
	CPropTree		m_trainInfoControl;
	CPropTree		m_sumInfoControl;
	BOOL            m_bReadOnly;
	enum  PSP_TYPE{PSP_TYPE_NULL=0,PSP_TYPE_MARK=1,PSP_TYPE_TRAIN=2,PSP_TYPE_SUM=3};
	PSP_TYPE        m_PspType;

	CTgDocument *m_pDoc;

	// Construction
public:
	CSheetTGProperty();
	// Attributes
public:
	BOOL CreateMe(const RECT &rect,CWnd *pParentWnd);
	void SetReadOnly(BOOL va);
	void SetDocument(CTgDocument *p) {m_pDoc=p;}
	void UpdateShowThis(CTgView *pview,const CTG_LogicObject *pobj,int para,BOOL switchobject, int conid);
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheetTGProperty)
	//}}AFX_VIRTUAL

	// Implementation
public:
	void OnSize(UINT nType,int cx,int cy);
	void OnClose();
	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnPropertyChange(WPARAM wParam, LPARAM lParam);
	void OnSelchangeTabctrl(NMHDR* pNMHDR, LRESULT* pResult);// 
	void OnTrainInfoItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);  // 属性框修改通知
	virtual ~CSheetTGProperty();

	void SetCurPage(int page_index);
	// Generated message map functions
protected:
	//{{AFX_MSG(CSheetTGProperty)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL  UpdatePropertyData(const CTG_LogicObject  *pobj);
	BOOL  CreateProperty(PSP_TYPE type);
	BOOL  CreateProperty(CPropTree *pPropTree,struct PropertyInfoPara *pInfo,int count);
	BOOL  UpdateTrainProperty(CPropTree *pPropTree,struct PropertyInfoPara *pInfo,int infoCount,const CTG_LogicObject  *pobj);
	BOOL  UpdateLocalData(const CTG_LogicObject *pobj);
	BOOL  InitStationProperty();

private:
	void  CommboAddString(CPropTreeItemCombo* pCombo, CTC_DATA_DEF datatp, int id);
	bool  TGPSP_SetItemIntValue(CTG_LogicObject *pobj,char* cap, int id, int station, long value);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECEIVENEIGHBERPLANSHEET_H__87097944_0589_44EF_97E6_7430675415CC__INCLUDED_)
