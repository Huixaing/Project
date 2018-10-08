// TYPlanView.cpp : 实现文件
//

#include "stdafx.h"
#include "TYPlanView.h"
#include ".\typlanview.h"
#include "TGPspData.h"
#include "sysshare.h"
// CTYPlanView
extern int DB_GetEntityStatus(long eid);
extern BOOL ReadXMLFile(CString &dest,const char *xmlfile);

IMPLEMENT_DYNCREATE(CTYPlanView, CView)

CTYPlanView::CTYPlanView()
//	: CView(CTYPlanView::IDD)
{
}

CTYPlanView::~CTYPlanView()
{
}

BEGIN_MESSAGE_MAP(CTYPlanView, CView)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TYPLAN_TREE, OnTvnSelchangedTYPlanTree)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TYDCMD_LIST, OnLvnItemchangedTydcmdList)
	ON_WM_SIZE()
	ON_COMMAND(IDC_REQ_TY_BTN,OnReqTYFromCAD)
END_MESSAGE_MAP()


// CTYPlanView 诊断

#ifdef _DEBUG
void CTYPlanView::AssertValid() const
{
	CView::AssertValid();
}

void CTYPlanView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
CTgDocument* CTYPlanView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTgDocument)));
	return (CTgDocument*)m_pDocument;
}
#endif //_DEBUG


// CTYPlanView 消息处理程序

void CTYPlanView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if(lHint==TGDATA_TYPLAN_UPDATED)
	{
		TYplanInform *pobj=(TYplanInform *)pHint;
		if(pobj->change_flag== TYplanInform::TYPLAN_LIST_CHANGED)
			ShowTYPlanTree();
	}
}

void CTYPlanView::OnTvnSelchangedTYPlanTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM hSel=m_PlanTree.GetSelectedItem();
	if(hSel)
	{
		DWORD nSel=m_PlanTree.GetItemData(hSel);
		
		if(nSel<m_treeList.size())
		{
			TreeNode nd=m_treeList[nSel];

			m_DcmdInfo.SetWindowText("");
			if (nd.nLayer==1)
			{
				CString xml;
				ReadXMLFile(xml, nd.filename);
				UpdateShowDCMDList(NULL,NULL);
				UpdateShowPlanList(NULL,NULL);
				UpdateShowPlanList(xml,"ACK_SPEC_PLAN");

				m_TextInfo.SetWindowText("");
			}
			else if (nd.nLayer==2)
			{
				CString xml;
				ReadXMLFile(xml, nd.filename);
				UpdateShowDCMDList(NULL,NULL);
				UpdateShowPlanList(NULL,NULL);
				UpdateShowPlanList(xml,"ACK_SPEC_PLAN");
				
				m_TextInfo.SetWindowText("");

				UpdateShowDCMDList(NULL,NULL);
				UpdateShowDCMDList(xml,"ACK_SPEC_PLAN/NCMD");
			}
			else if (nd.nLayer==3)
			{
				CString xml;
				ReadXMLFile(xml, nd.filename);
				UpdateShowPlanList(NULL,NULL);
				UpdateShowPlanList(xml,"ACK_SPEC_PLAN");
				CTD_SPECPLAN spplan;
				spplan.LoadSPECPLANFromFile(xml,"ACK_SPEC_PLAN");

				//for (int m=0;m<plan.TYNOTspIFY.size();m++)
				//{
				//	if (spplan.TYNOTIFY[m].NGID == _atoi64(nd.layer3))
				//		break;
				//}
				if (nd.layer3<0)
					return;
				int m = nd.layer3;
				if (m<spplan.TYNOTIFY.size())
				{
					CString text,tmp;
					tmp.Format("通知单标题: %s \r\n",spplan.TYNOTIFY[m].NTITLE);
					text += tmp;

					CTime tm = CTime(spplan.TYNOTIFY[m].NTM);
					tmp.Format("通知单下达时间: %s \r\n", tm.Format("%Y-%m-%d %H:%M:%S"));
					text += tmp;

					tmp.Format("列车全局ID号: %u\r\n", spplan.TYNOTIFY[m].NGID);
					text += tmp;

					tmp = "================================================================================================================\r\n";
					text += tmp;

					text += spplan.TYNOTIFY[m].NCONTENT;

					m_TextInfo.SetWindowText(text);
				}

				UpdateShowDCMDList(NULL,NULL);
				UpdateShowDCMDList(xml,"ACK_SPEC_PLAN/NCMD");
			}
		}
	}
	*pResult = 0;
}

void CTYPlanView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	ShowTYPlanTree();
	GetParentFrame()->SetWindowText("特运计划");
}

int CTYPlanView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect(0,0,0,0);
	m_PlanTree.Create(WS_CHILD|WS_BORDER|WS_VISIBLE|TVS_LINESATROOT|TVS_HASLINES|TVS_HASBUTTONS|TVS_SHOWSELALWAYS,rect,this,IDC_TYPLAN_TREE);
	m_PlanTree.DeleteAllItems();

	m_PlanList.CreateGrid(this,rect,IDC_TYPLAN_LIST,g_typlan_def,count_of_typlan_def());
	m_PlanList.ExpandColumnsToFit();

	m_DcmdList.CreateGrid(this,rect,IDC_TYDCMD_LIST,g_typlan_dcmd_def,count_of_typlan_dcmd_def());
	m_DcmdList.ExpandColumnsToFit();

	m_TextInfo.Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY|WS_BORDER,rect,this,IDC_TY_TEXT);
	m_DcmdInfo.Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY|WS_BORDER,rect,this,IDC_DCMD_TEXT);

	return 0;
}

BOOL CTYPlanView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& prect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	CView::Create(lpszClassName, lpszWindowName, dwStyle, prect, pParentWnd, nID, pContext);

	return true;
}

void CTYPlanView::UpdateShow(const char *xml,const char *node)
{
	;
}
void CTYPlanView::UpdateShowPlanTree(const char *xml,const char *node)
{
	;
}
void CTYPlanView::UpdateShowPlanList(const char *xml,const char *node)
{
	if(!xml)
	{
		m_PlanList.DeleteNonFixedRows();
		m_PlanList.Refresh();
		m_PlanList.SetWindowText("");
		return;
	}
	m_PlanList.InsertRows(xml,node);
	m_PlanList.ShowWindow(SW_SHOW);
}
void CTYPlanView::UpdateShowDCMDList(const char *xml,const char *node)
{
	if(!xml)
	{
		m_DcmdList.DeleteNonFixedRows();
		m_DcmdList.Refresh();
		m_DcmdList.SetWindowText("");
		return;
	}
	m_DcmdList.InsertRows(xml,node);
	m_DcmdList.ShowWindow(SW_SHOW);

}

void CTYPlanView::OnLvnItemchangedTydcmdList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_GRIDVIEW *pNMLV=(NM_GRIDVIEW *)pNMHDR;
	if(pNMLV->iRow>1)
	{
		int col=m_DcmdList.GetColumnByID(ID_PD_TYPLAN_DCMD_CONTENT);
		if(col>=0)
		{
			m_DcmdInfo.SetWindowText(m_DcmdList.GetCell(pNMLV->iRow,col)->GetText());
		}
	}
	*pResult = 0;
}

void  CTYPlanView::ShowTYPlanTree()
{
	m_PlanTree.DeleteAllItems();
	m_treeList.clear();
	GetDocument()->ClearAllTYPlan();

	CString filepath=gpcdef.GetXMLDir();
	CFileFind  fileFind;
	
	CString  filenameprefix;
	filenameprefix.Format("%s\\TYPLAN-*-*-*.xml",filepath);
	BOOL flag = fileFind.FindFile(filenameprefix);
	while(flag)
	{
		flag = fileFind.FindNextFile();

		if(fileFind.IsDirectory())
			continue;

		CString strFileName = fileFind.GetFileName();
		CString strName=strFileName;
		int n=strName.Find('-');
		strName=strName.Right(strName.GetLength()-n-1);
		n=strName.Find('-');
		CString strID=strName.Left(n);
		strName=strName.Right(strName.GetLength()-n-1);
		n=strName.Find('-');
		CString strOrderID=strName.Left(n);
		CString strRootName="特运计划"+strID;
		
		TreeNode node;
		int x= strFileName.Find('.');
		strFileName = strFileName.Left(x);
		strncpy(node.filename, strFileName, 63);
		strncpy(node.layer1, strID, 15);
		node.nLayer=1;
		HTREEITEM hTreeItem=GetTYPlanItem(strRootName, node);
		strncpy(node.layer2, strOrderID, 15);
		AddTYPlanChildItem(hTreeItem, strOrderID, node);
	}

	m_PlanTree.ShowWindow(SW_SHOW);
}

HTREEITEM CTYPlanView::GetTYPlanItem(CString strName, TreeNode& node)
{
	HTREEITEM item=m_PlanTree.GetRootItem();
	while(item)
	{
		CString str=m_PlanTree.GetItemText(item);
		if(str==strName)
			return item;

		item=m_PlanTree.GetNextSiblingItem(item);
	}

	int lParam=m_treeList.size();
	m_treeList.push_back(node);

	HTREEITEM pitem = m_PlanTree.InsertItem(strName.GetBuffer(0), TVI_ROOT);
	m_PlanTree.SetItemData(pitem, lParam);
	return pitem;
}

void CTYPlanView::AddTYPlanChildItem(HTREEITEM hTreeItem, CString strName, TreeNode& node)
{	
	CString xml;
	ReadXMLFile(xml, node.filename);

	CTD_SPECPLAN spplan;
	spplan.LoadSPECPLANFromFile(xml,"ACK_SPEC_PLAN");

	int lParam=m_treeList.size();
	node.nLayer=2;
	m_treeList.push_back(node);
	HTREEITEM hTreeItem1=m_PlanTree.InsertItem(spplan.TNO.GetBuffer(0), hTreeItem);
	m_PlanTree.SetItemData(hTreeItem1, lParam);

	CString filepath;
	filepath.Format("%s\\%s",gpcdef.GetXMLDir(), node.filename);
	
	for (int m=0;m<spplan.TYNOTIFY.size();m++)
	{
		CString text;
		text.Format("%u",spplan.TYNOTIFY[m].NGID);
		node.layer3=m;
		node.nLayer=3;

		int lParam1=m_treeList.size();
		m_treeList.push_back(node);

		HTREEITEM hTreeItem2 = m_PlanTree.InsertItem(spplan.TYNOTIFY[m].NTITLE.GetBuffer(0), hTreeItem1);
		m_PlanTree.SetItemData(hTreeItem2, lParam1);
	}

	int nID=atoi(node.layer1);
	int index=GetDocument()->FindTYPlanByMsgID(nID);
	if (index<0)
		index=GetDocument()->AddTYPlan(nID, FALSE, spplan.DATE);
	
	return;
}

void CTYPlanView::OnDraw(CDC* /*pDC*/)
{
	// TODO: 在此添加专用代码和/或调用基类
}

void CTYPlanView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (cy<100)
		return;

	// TODO: 在此处添加消息处理程序代码

	m_PlanTree.MoveWindow(2, 5,cx*0.32, cy-5); 
	m_PlanList.MoveWindow(cx*0.33, 5, cx*0.66, (cy-5)*0.33);
	m_TextInfo.MoveWindow(cx*0.33, 5+(cy-5)*0.33, cx*0.66, (cy-5)*0.33);
	m_DcmdList.MoveWindow(cx*0.33, 5+(cy-5)*0.66, cx*0.66*0.66, (cy-5)*0.33);
	m_DcmdInfo.MoveWindow(cx*0.33+cx*0.66*0.66, 5+(cy-5)*0.66, cx*0.66*0.33+10, (cy-5)*0.33);

	m_PlanList.ExpandColumnsToFit();
	m_DcmdList.ExpandColumnsToFit();

}

static CTime lastTm1 = CTime::GetCurrentTime();
void CTYPlanView::OnReqTYFromCAD()
{
	if(gpcdef.IsEnableMMPlanFromCAD())
	{
		CTime tm = CTime::GetCurrentTime();
		if(abs(long(tm.GetTime() - lastTm1.GetTime())) < 8) // 避免快速点击获取
			return;
		lastTm1=tm;

		TIME st,et;
		GetDocument()->GetMMPlanRange(st,et); 
		CTime cst=st;
		CTime cet=cst+CTimeSpan(1,0,0,0);
		et=cet.GetTime(); 

		long eid=GetSpecialEntityID(SERVER_TYPE_MMPLAN); 
		if(DB_GetEntityStatus(eid)!=1)
		{
			MessageBox("请注意:\n\t本调度终端和应用服务器连接中断, 不能获取特运计划!!", "提示", MB_OK|MB_ICONWARNING);
			return;
		}

		GetDocument()->RequestSPECFromCAD(st,et);
	}
}
