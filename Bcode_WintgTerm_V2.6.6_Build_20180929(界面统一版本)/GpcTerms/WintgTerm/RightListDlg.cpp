// CRightListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RightListDlg.h"
#include "function.h"
#include "messages.h"
#include "memshare.h"
#include "resource.h"
#include "widgets.h"

#include "RightPropertyDlg.h"
#include <baseMsg\msgid.h>
#include <baseMsg\gpc_msgid.h>
#include <BaseMsg/clbth.h>
#include ".\rightlistdlg.h"
#include "TgView.h"

extern CLoadGPCDef m_load;
/////////////////////////////////////////////////////////////////////////////
// CRightListDlg dialog
CRightListDlg::CRightListDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRightListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRightListDlg)
	//}}AFX_DATA_INIT
}


void CRightListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRightListDlg)
	DDX_Control(pDX,IDC_SECTION_LIST,m_right_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRightListDlg, CDialog)
	//{{AFX_MSG_MAP(CRightListDlg)
	ON_MESSAGE(UM_UNI_MESSAGE, OnRightListData)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ADD_RIGHT, OnBnClickedAddRight)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDC_UPDATE_RIGHT, OnBnClickedUpdateRight)
	ON_BN_CLICKED(IDC_DELETE_RIGHT, OnBnClickedDeleteRight)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRightListDlg message handlers
BOOL CRightListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgParamentReply,0);
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgHugeDataCtrl,0);

	m_right_list.SetExtendedStyle(LVS_REPORT|LVS_EX_FULLROWSELECT| LVS_EX_ONECLICKACTIVATE);
	CRect rect;
	m_right_list.GetClientRect(&rect);
	int width=rect.Width();
	m_right_list.InsertColumn(0,"区段号",LVCFMT_LEFT,width/10);
	m_right_list.InsertColumn(1,"区段名称",LVCFMT_LEFT,width/5);
	m_right_list.InsertColumn(2,"实体号",LVCFMT_LEFT,width/10);
	m_right_list.InsertColumn(3,"实体名称",LVCFMT_LEFT,width/5);
	m_right_list.InsertColumn(4,"具体权限",LVCFMT_LEFT,width*1/5);
	m_right_list.InsertColumn(5,"权限参数",LVCFMT_LEFT,width*2/5);

	SendMsgRightListQuery();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRightListDlg::SendMsgRightListQuery()
{
	//user.clear();
	//m_user_list.DeleteAllItems();

	CLBTH::MsgParamentRequest msg;
	msg.entity=0;
	msg.scope=0;
	msg.type_count=0;

	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if (server_id<=0) server_id=0;
	SendBMsg(&msg,server_id);

	GpcLogTrainMessage(0, 0,"Request Right List %d to SVR %d", msg.type, server_id);
}

afx_msg LRESULT CRightListDlg::OnRightListData(WPARAM wParam,LPARAM lParam)
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if (pMsgStruct->msg_id==MSGID_MsgHugeDataCtrl)
	{
		CLBTH::MsgHugeDataCtrl *pMsg= ( CLBTH::MsgHugeDataCtrl *)pMsgStruct->pmsg;
		if (pMsg->msgid != CLBTH::MsgParamentReply::getTypeIDStatic())
			return 0;

		if(pMsg->status == CLBTH::MsgHugeDataCtrl::MSG_BEGIN)
		{
			mVectRightList.clear();
		}
		else if (pMsg->status == CLBTH::MsgHugeDataCtrl::MSG_END)
		{
			ShowRightList();
		}
	}
	else if (pMsgStruct->msg_id==MSGID_MsgParamentReply)
	{
		CLBTH::MsgParamentReply *pMsg= ( CLBTH::MsgParamentReply *)pMsgStruct->pmsg;

		RIGHT_DEFINE rightUser;
		memset(&rightUser, 0, sizeof(RIGHT_DEFINE));

		if (pMsg->scope==0)
			return 0;

		rightUser.con_id = pMsg->scope;
		CString str;
		str = m_load.GetSectionNameById(pMsg->scope);
		strncpy(rightUser.con_name,str, 49);	 
		rightUser.con_name[49]=0;
		
		///////////////////////////////////////////////////////////////////
		rightUser.entity_id = pMsg->entity;
		GPCENTITY entityInfo;
		gpcdef.GetEntityInfo(pMsg->entity, entityInfo);
		strncpy(rightUser.entity_name, entityInfo.sName, 31);
		rightUser.entity_name[31]=0;

		for (int i=0; i<pMsg->type_count; i++)
		{
			if (pMsg->type[i] == (int)CLBTH::MsgParamentRequest::WINTG)
			{
				rightUser.role_right = pMsg->int_parament[i];
				strncpy(rightUser.role_param, pMsg->str_parament[i],2047);
				rightUser.role_param[2047]=0;
				mVectRightList.push_back(rightUser);
			}
		}

		//GpcLogTrainMessage(0, 0,"load user: code:%s name:%s job_id %d", tmpuser.user_id, tmpuser.user_name, tmpuser.user_property);
	}
	return 0;
}

void CRightListDlg::OnBnClickedClose()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}

void CRightListDlg::OnBnClickedAddRight()
{
	CRightPropertyDlg dlg;

	if (UpdateData() != TRUE)
		return;

	if (dlg.DoModal() == IDOK)
	{
		int i=0;

		RIGHT_DEFINE rightnew;
		memset(&rightnew, 0, sizeof(RIGHT_DEFINE));

		rightnew.con_id=dlg.m_con_id;
		rightnew.entity_id=dlg.m_entity_id;
		strncpy(rightnew.con_name,dlg.m_con_name, 49);
		rightnew.con_name[49]=0;
		strncpy(rightnew.entity_name,dlg.m_entity_name,31);
		rightnew.entity_name[31]=0;
		rightnew.role_right=dlg.m_parament;
		strncpy(rightnew.role_param,dlg.role_param,2047);
		rightnew.role_param[2047]=0;

		for(int i=0; i<mVectRightList.size(); i++)
		{
			if(mVectRightList[i].con_id==dlg.m_con_id && mVectRightList[i].entity_id==dlg.m_entity_id)
			{
				CString text;
				text.Format("此用户 %s(区段号%d) %s(实体号%d)已存在,请确认是否覆盖!",dlg.m_con_name,dlg.m_con_id,dlg.m_entity_name,dlg.m_entity_id);
				if (AfxMessageBox(text, MB_OKCANCEL)!=IDOK)
					return;
			}
		}
		
		CLBTH::MsgParamentOperation addmsg;
		addmsg.scope = rightnew.con_id;
		addmsg.entity = rightnew.entity_id;

		addmsg.type_count = 1;
		addmsg.type[0] = CLBTH::MsgParamentOperation::WINTG;
		addmsg.int_parament[0] = rightnew.role_right;
		strcpy(addmsg.str_parament[0],rightnew.role_param);
		addmsg.oper = CLBTH::MsgParamentOperation::PARAOPER_INSERT_UPDATE;

		CLBTH::Archive ar(CLBTH::Archive::store);
		addmsg.serialize(ar);

		long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
		if(server_id>0)
		{
			if(SendBMsg(&addmsg,server_id))
			{
				GpcLogTrainMessage(0, 0,"add user right:scope %d entity %d type %d int_parament %d str_parament:%s to svr %d", 
					addmsg.scope, addmsg.entity, addmsg.type[0], addmsg.int_parament[0], addmsg.str_parament[0], server_id);
			}
		}
		
		SendMsgRightListQuery();
	}
}

void CRightListDlg::OnBnClickedUpdateRight()
{
	// TODO: 在此添加控件通知处理程序代码
	CRightPropertyDlg dlg(false);

	if (UpdateData() != TRUE)
		return;

	int sel_count = m_right_list.GetSelectedCount();
	if (sel_count != 1 )
		return;

	int sel_index=m_right_list.GetSelectionMark();
	if (sel_index<mVectRightList.size())
	{		
		dlg.m_con_id = mVectRightList[sel_index].con_id;
		dlg.m_con_name = CString(mVectRightList[sel_index].con_name);

		dlg.m_entity_id = mVectRightList[sel_index].entity_id;
		dlg.m_entity_name = CString(mVectRightList[sel_index].entity_name);
		dlg.m_parament = mVectRightList[sel_index].role_right;
		strncpy(dlg.role_param,mVectRightList[sel_index].role_param,2047);
		dlg.role_param[2047]=0;
		if (dlg.DoModal() == IDOK)
		{
			int i=0;

			RIGHT_DEFINE rightnew;
			memset(&rightnew, 0, sizeof(RIGHT_DEFINE));

			rightnew.con_id=dlg.m_con_id;
			rightnew.entity_id=dlg.m_entity_id;

			strncpy(rightnew.con_name,dlg.m_con_name, 49);
			rightnew.con_name[49]=0;
			strncpy(rightnew.entity_name,dlg.m_entity_name,31);
			rightnew.entity_name[31]=0;
			
			rightnew.role_right=dlg.m_parament;
			strncpy(rightnew.role_param,dlg.role_param,2047);
			rightnew.role_param[2047]=0;

			CLBTH::MsgParamentOperation modifymsg;
			modifymsg.scope = rightnew.con_id;
			modifymsg.entity = rightnew.entity_id;

			modifymsg.type_count = 1;
			modifymsg.type[0] = CLBTH::MsgParamentOperation::WINTG;
			modifymsg.int_parament[0] = rightnew.role_right;
			strcpy(modifymsg.str_parament[0],rightnew.role_param);
			modifymsg.oper = CLBTH::MsgParamentOperation::PARAOPER_INSERT_UPDATE;

			CLBTH::Archive ar(CLBTH::Archive::store);
			modifymsg.serialize(ar);

			long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
			if(server_id>0)
			{
				if(SendBMsg(&modifymsg,server_id))
				{
					GpcLogTrainMessage(0, 0,"modify user right:scope %d entity %d type %d int_parament %d str_parament:%s to svr %d", 
							modifymsg.scope, modifymsg.entity, modifymsg.type[0], modifymsg.int_parament[0], modifymsg.str_parament[0], server_id);
				}
			}
			SendMsgRightListQuery();
		}
	}
}

void CRightListDlg::OnBnClickedDeleteRight()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel_count = m_right_list.GetSelectedCount();
	if (sel_count <= 0 )
		return;

	if (AfxMessageBox("确认删除？", MB_OKCANCEL)==IDOK)
	{  
		CLBTH::MsgParamentOperation delmsg;
		delmsg.oper = CLBTH::MsgParamentOperation::PARAOPER_DELETE;
		delmsg.type_count = 1;
		delmsg.type[0]=CLBTH::MsgParamentRequest::WINTG;
		POSITION sel_user=m_right_list.GetFirstSelectedItemPosition();		
		for(int i=0; i<sel_count&&i<mVectRightList.size(); i++)
		{
			int sel_index=m_right_list.GetNextSelectedItem(sel_user);
			delmsg.scope = mVectRightList[sel_index].con_id;
			delmsg.entity = mVectRightList[sel_index].entity_id;
			
			CLBTH::Archive ar(CLBTH::Archive::store);
			delmsg.serialize(ar);

			long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
			if(server_id>0) 
			{
				if(SendBMsg(&delmsg,server_id))
				{
					GpcLogTrainMessage(0, 0,"delete user right:scope %d entity %d to svr %d", 
						delmsg.scope, delmsg.entity, server_id);
				}
			}
		}
		SendMsgRightListQuery();
	}
}


void CRightListDlg::ShowRightList()
{
	LVITEM	lvitem;
	m_right_list.DeleteAllItems();

	lvitem.mask=LVIF_TEXT;
	for(int index=0; index < mVectRightList.size(); index++)
	{   //根据当前的选项来进行显示
		CString str;
		lvitem.iItem=index;
		lvitem.iSubItem=0;
		str.Format("%d",mVectRightList[index].con_id);
		lvitem.pszText=str.GetBuffer();
		lvitem.iItem = m_right_list.InsertItem(&lvitem);

		lvitem.iSubItem=1;
		lvitem.pszText=mVectRightList[index].con_name;
		m_right_list.SetItem(&lvitem);

		lvitem.iSubItem=2;
		str.Format("%d",mVectRightList[index].entity_id);
		lvitem.pszText=str.GetBuffer();
		m_right_list.SetItem(&lvitem);

		lvitem.iSubItem=3;
		lvitem.pszText=mVectRightList[index].entity_name;
		m_right_list.SetItem(&lvitem);

		lvitem.iSubItem=4;
		if (mVectRightList[index].role_right&0x0020)
			str="允许修改实际点";
		else
			str="禁止修改实际点";
		lvitem.pszText=str.GetBuffer();
		m_right_list.SetItem(&lvitem);

		lvitem.iSubItem=5;
		lvitem.pszText=mVectRightList[index].role_param;
		m_right_list.SetItem(&lvitem);
	}	
}
