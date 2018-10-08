#if !defined(AFX_MYTREE_H__FC02ADE1_9C85_11D3_B4D4_10005ABA8A13__INCLUDED_)
#define AFX_MYTREE_H__FC02ADE1_9C85_11D3_B4D4_10005ABA8A13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyTree window
const int  TREEITEMNUM =  4096;
const int  TREEVALUEMAXLEN = 24;
const int  TREENAMEMAXLEN =  128;
#define  ID_TREE_INSERT  0xF000
#define  ID_TREE_DELETE  0xF001
#define  ID_TREE_EDIT    0xF002
class CMyTree : public CTreeCtrl
{
// Construction
public:
	CMyTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTree)
	//}}AFX_VIRTUAL

// Implementation
	int  m_flag;
	CImageList m_normallist,m_statelist;
public:
	int GetArmStationByArmNo(int armno, int varry[],int arraysize, CString& armname);
	void SetEditBarAt(int i);
	void SetEditable(int f){m_flag=f;}
	int GetAvaSonIndex(int parent);
	int GetAvaIndex();
	void Delete_1_Item(int i);
	void Startup();
	int GetItemByIndex(short int index[],short int indexnum);
	const char * GetWholeValue(int i);
	int  GetSelectedRelaValue();
	int  GetSubRelaValues(HTREEITEM handle,int varry[],int arraysize);
	int  GetSelectedSubTreeRelaValues(int varry[],int arraysize);
	void SaveToFile(const char *file);
	void LoadFromFile(const char *file,const char *root);
	int  Insert_1_Item(int parent, const char *name, const char *value);
	void  Reset();
	
	const char *GetSelectedCaption();
	
	const char *GetIDStr(int i);
	void  SetObjState(const char *id,int state);
	void  SetObjValue(const char *id,const char *value);
	void  SetObjStateByRelaValue(long rela_va,int state);
	void  SetObjStateByAlias(const char *alias,int state);
	virtual ~CMyTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyTree)
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnItemInsert();
	afx_msg void OnItemDelete();
	afx_msg void OnItemEdit();
	afx_msg void OnEditUpdated();
	afx_msg void OnEnable();
	afx_msg void OnDisable();
	DECLARE_MESSAGE_MAP()
private:
    struct{
	  HTREEITEM  handle;
	  int		 state;
	  short int  index[6];
	  char       indexnum;
	  short int  maxsonindex;
	  char       has_sub_tree;
	  long       rela_value;
	  char  value[TREEVALUEMAXLEN];
	  char  name[TREENAMEMAXLEN*2];
	  char  alias[TREENAMEMAXLEN];
	  bool	expand;
		}m_czs[TREEITEMNUM];
	int m_cznum;
	int  GetParent(int son);
	void SetTreeItems(void);
	HTREEITEM  InsertTreeItem(int i,HTREEITEM parent,HTREEITEM after); 	
	void UpdateParentState(int i);
	void ExpandAll(HTREEITEM hItem);
public:
	short SelectAndExpandObject(LPCTSTR alias, LPCTSTR strid, long rela_va);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTREE_H__FC02ADE1_9C85_11D3_B4D4_10005ABA8A13__INCLUDED_)




