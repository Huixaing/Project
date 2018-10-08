// DlgTestTD.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "tg_BinXml.h"
#include "ext_src/XmlNodeWrapper.h"
#include "DlgDataXMLShow.h"

// CDlgDataXMLShow 对话框
IMPLEMENT_DYNAMIC(CDlgDataXMLShow, CDialog)
CDlgDataXMLShow::CDlgDataXMLShow(CTG_LogicObject *pobj, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDataXMLShow::IDD, pParent)
{
	m_pobj=pobj;
}

CDlgDataXMLShow::~CDlgDataXMLShow()
{
}

void CDlgDataXMLShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_XML, m_tree);
}

BEGIN_MESSAGE_MAP(CDlgDataXMLShow, CDialog)
END_MESSAGE_MAP()

BOOL CDlgDataXMLShow::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_pobj!=NULL)
	{
		DisplayXML();
	}
	return TRUE;
}

//////// XML 树形控件设置

static void ParseNode(CTreeCtrl &treectrl,IDispatch *pNode,HTREEITEM hParent)
{
	CXmlNodeWrapper node(pNode);
	int img,imgSel;
	CString str;
	if (node.NodeType() == "element")
	{
		img = 0;
		imgSel = 1;
		str = node.Name();
	}
	else
	{
		img = imgSel = 2;
		str = node.GetText();
	}

	HTREEITEM hItem = treectrl.InsertItem(str,img,imgSel,hParent);
	treectrl.SetItemData(hItem,(DWORD)pNode);

	for (int i = 0; i < node.NumAttributes(); i++)
	{
		HTREEITEM hAttribItem = treectrl.InsertItem(node.GetAttribName(i) + " = " + node.GetAttribVal(i),3,4,hItem);
	}

	for (i = 0; i < node.NumNodes(); i++)
	{
		ParseNode(treectrl,node.GetNode(i),hItem);
	}
}

static CXmlDocumentWrapper m_xmlDoc;
void CDlgDataXMLShow::DisplayXML()
{
	m_tree.ShowWindow(SW_HIDE);
	m_tree.DeleteAllItems();

	CString xml;
	if (m_pobj->GetObjectType()==TG_OBJ_MARK)	
	{
		CTG_SingleMark mark=*(CTG_SingleMark*)m_pobj;
		xml=toXMLString(mark);
	}
	else if (m_pobj->GetObjectType()==TG_OBJ_TRAIN)	
	{
		CTG_SingleTrain train=*(CTG_SingleTrain*)m_pobj;
		xml=toXMLString(train);
	}
	else
	{
		return;
	}

	m_xmlDoc.LoadXML(xml);
	m_tree.DeleteAllItems();
	ParseNode(m_tree,m_xmlDoc.AsNode(),TVI_ROOT);

	m_tree.ShowWindow(SW_SHOW);
	m_tree.Expand(m_tree.GetRootItem(), TVE_EXPAND);
}
