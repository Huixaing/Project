#pragma once

#include "GridCtrl/GridCtrl.h"

class CCZHGridView : public CGridCtrl
{
	DECLARE_DYNAMIC(CCZHGridView)

	struct Field
	{
		char  caption[32];
		char  attr_name[32];
		DWORD param;
	};

public:
	CCZHGridView(void);
	virtual ~CCZHGridView(void);

public:
	BOOL  CreateGrid(CString& xmlfield, CWnd *parent, const RECT &rect, UINT ctrlid);
	
	virtual void  BeginEditCell(int row,int col);
	virtual void  EndEditCell(int row,int col);

	void  SrcollToRow(UINT row);
	BOOL  AddRowDataFromXML(CString& xml, char* rootnode);
	void  AutoWidth();
private:
	BOOL  OnBeginEdit(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL  OnEndEdit(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL  OnSelectChanged(NMHDR* pNMHDR, LRESULT* pResult);
	
	void  GetHeaderFromXML(CString xml);
private:
	Field m_fields[20];
	UINT  m_fields_num;
	CWnd* m_parent;
	BOOL  m_bSingleLine;
	BOOL  m_bRowSelectable;
	char  seltrainid[20];
private:
	// Generated message map functions
	//{{AFX_MSG(CCZHGridView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
