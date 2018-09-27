#pragma once

// MyGenTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyGenTree window
#define  TREEITEMNUM   1024
#define  TREEVALUEMAXLEN 64
#define  TREENAMEMAXLEN  512

#define  ID_TREE_INSERT  0xF000
#define  ID_TREE_DELETE  0xF001
#define  ID_TREE_EDIT    0xF002

class CMyGenTree : public CTreeCtrl
{
// Construction
public:
	CMyGenTree();
	virtual ~CMyGenTree();

// Implementation
public:
	int GetAvaSonIndex(int parent);
	int GetAvaIndex();
	
	void Startup();
		
	int  GetSelectedRelaValue();
	int  Insert_1_Item(int parent, const char *name, const char *value,int rela_value);
	int  Insert_1_Item(const char *parentvalue, const char *name, const char *value,int rela_value);
	int  GetIndexByValue(const char *value);
	void  Reset();
	
	CString GetSelectedValue();
	const char *GetIDStr(int i);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyGenTree)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnItemInsert();
	afx_msg void OnItemDelete();
	afx_msg void OnItemEdit();
	afx_msg void OnEditUpdated();
	DECLARE_MESSAGE_MAP()
private:
    struct
	{
	  HTREEITEM  handle;
	  int		 state;
	  short int  index[6];
	  char       indexnum;
	  short int  maxsonindex;
	  char       has_sub_tree;
	  long       rela_value;
	  char  value[TREEVALUEMAXLEN];
	  char  name[TREENAMEMAXLEN];
	  bool	expand;
	}m_czs[TREEITEMNUM];

	int m_cznum;
public:
	int   GetItemTotalCount() {return m_cznum;}
	const char *   GetItemValueByIndex(int i) {return m_czs[i].value;}
	int   GetItemRelaValueByIndex(int i){return m_czs[i].rela_value;}
	short SelectAndExpandObjectByValue(LPCTSTR value);

	int  GetParent(int son);
	HTREEITEM  InsertTreeItem(int i,HTREEITEM parent,HTREEITEM after); 	
	void ExpandAll(HTREEITEM hItem);
};

/////////////////////////////////////////////////////////////////////////////




