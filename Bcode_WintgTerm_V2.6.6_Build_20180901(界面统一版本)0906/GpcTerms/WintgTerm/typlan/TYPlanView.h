#pragma once

#include "resource.h"
#include "TgDocument.h"
#include "../utility/gen_mytree.h"
#include "../utility/xmlview.h"
#include "../td/td_data_def.h"
// CTYPlanView 窗体视图

class CTYPlanView : public CView
{
	DECLARE_DYNCREATE(CTYPlanView)

protected:
	CTYPlanView();           // 动态创建所使用的受保护的构造函数
	virtual ~CTYPlanView();

private:
	struct TreeNode
	{
		char filename[64];
		char layer1[16];
		char layer2[16];
		int  layer3;
		int  nLayer;

		TreeNode()
		{
			nLayer=0;
			layer3=0;
			memset(this, 0, sizeof(TreeNode));
		}
	};

	std::vector<TreeNode> m_treeList;

public:
	enum { IDD = IDD_TYPLANVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
private:
	CTreeCtrl m_PlanTree;
	CEdit m_TextInfo;
	CEdit m_DcmdInfo;
	CXMLDataGrid m_PlanList;
	CXMLDataGrid m_DcmdList;
public:
	CTgDocument* GetDocument() const;
	void    OnReqTYFromCAD();

public:
	void UpdateShow(const char *xml,const char *node);
	void UpdateShowPlanTree(const char *xml,const char *node);
	void UpdateShowPlanList(const char *xml,const char *node);
	void UpdateShowDCMDList(const char *xml,const char *node);

	void ShowTYPlanTree();

	HTREEITEM GetTYPlanItem(CString strName, TreeNode& node);
	void AddTYPlanChildItem(HTREEITEM hTreeItem, CString strName, TreeNode& node);
protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* /*pDC*/);
public:
	afx_msg void OnTvnSelchangedTYPlanTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnItemchangedTydcmdList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


