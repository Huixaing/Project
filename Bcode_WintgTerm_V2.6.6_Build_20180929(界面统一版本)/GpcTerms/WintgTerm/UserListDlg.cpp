// UserListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UserListDlg.h"
#include "function.h"
#include "tgmsg.h"
#include "messages.h"
#include "memshare.h"
#include "resource.h"
#include "widgets.h"

#include "UserPropertyDlg.h"
#include <baseMsg\msgid.h>
#include <baseMsg\gpc_msgid.h>
#include <BaseMsg/clbth.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserListDlg dialog

CUserListDlg::CUserListDlg(CWnd* pParent /*=NULL*/)
: CDialog(CUserListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserListDlg)
	m_cur_user_type = -1;
	//}}AFX_DATA_INIT
	user.clear();
}


void CUserListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserListDlg)
	DDX_Control(pDX, IDC_COMBO_USER_TYPE, m_user_type_list);
	DDX_Control(pDX, ID_MODIFY_USER, m_btn_modify);
	DDX_Control(pDX, ID_DEL_USER, m_btn_del);
	DDX_Control(pDX, ID_ADD_USER, m_btn_add);
	DDX_Control(pDX, IDC_LIST_USER, m_user_list);
	DDX_CBIndex(pDX, IDC_COMBO_USER_TYPE, m_cur_user_type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserListDlg, CDialog)
	//{{AFX_MSG_MAP(CUserListDlg)
	ON_BN_CLICKED(ID_DEL_USER, OnDelUser)
	ON_BN_CLICKED(ID_ADD_USER, OnAddUser)
	ON_BN_CLICKED(ID_MODIFY_USER, OnModifyUser)
	ON_CBN_SELCHANGE(IDC_COMBO_USER_TYPE, OnSelchangeComboUserType)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_USER, OnDblclkListUser)
	ON_MESSAGE(UM_UNI_MESSAGE, OnUserListData)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserListDlg message handlers
BOOL CUserListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgUserManager,0);

	for (int i=0; i<USER_TYPE_COUNT; i++)
		m_user_type_list.AddString(USER_TYPE[i]);
	m_user_type_list.SetCurSel(1);
	CRect rect;
	m_user_list.GetClientRect(&rect);
	int width=rect.Width();
	m_user_list.InsertColumn(0,"用户ID",LVCFMT_LEFT,width/4);
	m_user_list.InsertColumn(1,"用户姓名",LVCFMT_LEFT,width/4);
	m_user_list.InsertColumn(2,"用户职务",LVCFMT_LEFT,width/4);

	SendMsgUserManagerQuery();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CUserListDlg::OnUserListData(WPARAM wParam,LPARAM lParam)
{	
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if(pMsgStruct->msg_id==MSGID_MsgUserManager)
	{
		CLBTH::MsgUserManager *pBuf = (CLBTH::MsgUserManager *)pMsgStruct->pmsg;
	
		USER_DEFINE tmpuser;
		memset(&tmpuser, 0, sizeof(USER_DEFINE));
		short rec_count = pBuf->count;
		for (int j=0; j<rec_count; j++)
		{
			strncpy(tmpuser.user_id,pBuf->data[j].user_code, 19);	 
			tmpuser.user_id[19]=0;
			strncpy(tmpuser.user_name,pBuf->data[j].user_name, 19); 
			tmpuser.user_name[19]=0;
			strncpy(tmpuser.user_pass,pBuf->data[j].user_pwd, 19);
			tmpuser.user_pass[19]=0;

			tmpuser.user_property = pBuf->data[j].job_id;
			tmpuser.nconsole_count = pBuf->data[j].console_count;
			for(int n=0;n<tmpuser.nconsole_count&&n<20;n++)
				tmpuser.ncon_list[n] = pBuf->data[j].con_list[n];

			user.push_back(tmpuser);

			GpcLogTrainMessage(0, 0,"load user: code:%s name:%s job_id %d", tmpuser.user_id, tmpuser.user_name, tmpuser.user_property);
		}

		ShowUserList();
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return TRUE;
}
void CUserListDlg::OnDelUser() 
{
	// TODO: Add your control notification handler code here
	if (UpdateData() != TRUE)
		return;	
	int sel_count;
	
	sel_count = m_user_list.GetSelectedCount();
	if (sel_count <= 0 )
		return;

	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if (AfxMessageBox("确认删除？", MB_OKCANCEL)==IDOK)
	{  
		CLBTH::MsgUserManager mMsgUserManager;
        mMsgUserManager.count = sel_count;
		mMsgUserManager.oper = CLBTH::MsgUserManager::USERMGR_DELETE;
		POSITION sel_user=m_user_list.GetFirstSelectedItemPosition();		
		for(int i=0; i<sel_count&&i<30; i++)
		{
			int sel_index=m_user_list.GetNextSelectedItem(sel_user);
			int nIndex = GetUser(m_user_list.GetItemText(sel_index,0).GetBuffer(10));
			strcpy(mMsgUserManager.data[i].user_code, user[nIndex].user_id);
			strcpy(mMsgUserManager.data[i].user_name, user[nIndex].user_name);
			strcpy(mMsgUserManager.data[i].user_pwd , user[nIndex].user_pass);
			mMsgUserManager.data[i].job_id = user[nIndex].user_property  ;	////		
			mMsgUserManager.data[i].console_count = user[nIndex].nconsole_count;
			for(int j=0; j< user[nIndex].nconsole_count; j++)		
				mMsgUserManager.data[i].con_list[j] = user[nIndex].ncon_list[j];

			GpcLogTrainMessage(0, 0,"delete user %s %s from svr %d", mMsgUserManager.data[i].user_code, mMsgUserManager.data[i].user_name, server_id);
		}
		CLBTH::Archive ar( CLBTH::Archive::store);
		mMsgUserManager.serialize(ar);

		if(server_id>0)
			SendBMsg(&mMsgUserManager,server_id);

		SendMsgUserManagerQuery();
	}
}

void CUserListDlg::OnAddUser() 
{
	CUserPropertyDlg dlg;
	
	if (UpdateData() != TRUE)
		return;

	dlg.m_propperty = m_cur_user_type;		
	if (dlg.DoModal() == IDOK)
	{
		int i=0;
		for (i=0; i< user.size(); i++)
		{
			if (!strcmp(user[i].user_id, dlg.m_user_id.GetBuffer(10)))
				break;
		}

		if (i<user.size())
		{
			CString text;
			text.Format("该用户ID已存在!!!!");
			AfxMessageBox(text);
			return;
		}

		USER_DEFINE usernew;
		memset(&usernew, 0, sizeof(USER_DEFINE));

		strcpy(usernew.user_id,dlg.m_user_id.GetBuffer(10));
		strcpy(usernew.user_name,dlg.m_user_name.GetBuffer(20));
		strcpy(usernew.user_pass,dlg.m_passwd.GetBuffer(20));
		usernew.user_property = dlg.m_propperty +1 ; ////
			
		m_user_type_list.SetCurSel(dlg.m_propperty);
		usernew.nconsole_count = 0;

		CLBTH::MsgUserManager mMsgUserManager;
		mMsgUserManager.count = 1;
		mMsgUserManager.oper = CLBTH::MsgUserManager::USERMGR_INSERT;

		strcpy(mMsgUserManager.data[0].user_code, usernew.user_id);
		strcpy(mMsgUserManager.data[0].user_name, usernew.user_name);
		strcpy(mMsgUserManager.data[0].user_pwd , usernew.user_pass);
		mMsgUserManager.data[0].job_id = usernew.user_property ;

		mMsgUserManager.data[0].console_count = usernew.nconsole_count;
		for(int j=0; j<20; j++)		
			mMsgUserManager.data[0].con_list[j] = 0;
		for(int j=0; j<usernew.nconsole_count; j++)		
			mMsgUserManager.data[0].con_list[j] = usernew.ncon_list[j];
		CLBTH::Archive ar( CLBTH::Archive::store);
		mMsgUserManager.serialize(ar);

		long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
		if(server_id>0)
			SendBMsg(&mMsgUserManager,server_id);

		GpcLogTrainMessage(0, 0,"add user %s %s to svr %d", mMsgUserManager.data[0].user_code, mMsgUserManager.data[0].user_name, server_id);

		SendMsgUserManagerQuery();
	}
}

void CUserListDlg::OnModifyUser() 
{
	CUserPropertyDlg dlg(false);

	int sel_count = m_user_list.GetSelectedCount();
	if (sel_count != 1 )
		return;

	POSITION sel_user=m_user_list.GetFirstSelectedItemPosition();
	int sel_index=m_user_list.GetNextSelectedItem(sel_user);
	for (int j=0; j<user.size(); j++)
	{
		if (!strcmp(user[j].user_id,m_user_list.GetItemText(sel_index,0).GetBuffer(10)))
			break;
	}

	if (j<user.size())
	{		
		dlg.m_user_id = CString(user[j].user_id);
		dlg.m_user_name = CString(user[j].user_name);
		dlg.m_passwd = dlg.m_re_passwd = CString(user[j].user_pass);
		dlg.m_propperty = user[j].user_property-1;
		
		if (dlg.DoModal() == IDOK)
		{
			strcpy(user[j].user_id,dlg.m_user_id.GetBuffer(10));
			strcpy(user[j].user_name,dlg.m_user_name.GetBuffer(20));
			strcpy(user[j].user_pass,dlg.m_passwd.GetBuffer(20));
			user[j].user_property = dlg.m_propperty +1; ////
			
			m_user_type_list.SetCurSel(dlg.m_propperty );
			user[j].nconsole_count = 0;
			
			CLBTH::MsgUserManager mMsgUserManager;
			mMsgUserManager.count = 1;
			mMsgUserManager.oper = CLBTH::MsgUserManager::USERMGR_CHANGE;

			strcpy(mMsgUserManager.data[0].user_code, user[j].user_id);
			strcpy(mMsgUserManager.data[0].user_name, user[j].user_name);
			strcpy(mMsgUserManager.data[0].user_pwd , user[j].user_pass);
			mMsgUserManager.data[0].job_id = user[j].user_property  ;

			mMsgUserManager.data[0].console_count = user[j].nconsole_count;
			for(int n=0; n<user[j].nconsole_count; n++)		
				mMsgUserManager.data[0].con_list[n] =  user[j].ncon_list[n];
			CLBTH::Archive ar( CLBTH::Archive::store);
			mMsgUserManager.serialize(ar);

			long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
			if(server_id>0)
				SendBMsg(&mMsgUserManager,server_id);
			GpcLogTrainMessage(0, 0,"modify user %s %s to svr %d", mMsgUserManager.data[0].user_code, mMsgUserManager.data[0].user_name, server_id);

			SendMsgUserManagerQuery();
		}
	}
}

void CUserListDlg::OnSelchangeComboUserType() 
{
	// TODO: Add your control notification handler code here
	if (UpdateData() != TRUE)
		return;	
	ShowUserList();
}

void CUserListDlg::ShowUserList()
{
	LVITEM	lvitem;
	int i=0;
	char str_user_type[20];

	m_user_list.DeleteAllItems();
	m_cur_user_type=m_user_type_list.GetCurSel() ;

	lvitem.mask=LVIF_TEXT;
	for(int j=0; j < user.size(); j++)
	{   //根据当前的选项来进行显示
		if (user[j].user_property == (m_cur_user_type +1))
		{			
			lvitem.iItem=i;
			lvitem.iSubItem=0;
			lvitem.pszText=user[j].user_id;
			lvitem.iItem = m_user_list.InsertItem(&lvitem);

			lvitem.iSubItem=1;
			lvitem.pszText=user[j].user_name;
			m_user_list.SetItem(&lvitem);

			lvitem.iSubItem=2;
			strcpy(str_user_type,USER_TYPE[m_cur_user_type]);
			lvitem.pszText=str_user_type;
			m_user_list.SetItem(&lvitem);

			CString str,tmp;
			for(int n=0;n< user[j].nconsole_count;n++)
			{    
				tmp.Format("%d,",user[j].ncon_list[n]);
				str +=tmp;
			}
			lvitem.iSubItem=3;
		
			strcpy(lvitem.pszText,str);
			m_user_list.SetItem(&lvitem);

			i++;
		}
	}	
}

void CUserListDlg::OnDblclkListUser(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnModifyUser();
	*pResult = 0;
}


void CUserListDlg::SendMsgUserManagerQuery()
{
	user.clear();
	m_user_list.DeleteAllItems();
   	CLBTH::MsgUserManagerQuery mMsgUserManagerQuery;
	
	CLBTH::Archive ar( CLBTH::Archive::store);
	mMsgUserManagerQuery.serialize(ar);
	
	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if(server_id>0)
		SendBMsg(&mMsgUserManagerQuery,server_id);

	GpcLogTrainMessage(0, 0,"req user to svr %d", server_id);
}

int  CUserListDlg::GetUser(char *user_code)
{
	for (int j=0; j<user.size(); j++)
	{
		if (!strcmp(user[j].user_id,user_code))
		{
			break;
		}
	}
	if(j>=user.size())	
		return -1;
	return j;
}

void CUserListDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	user.clear();
	DestroyWindow();
}

void CUserListDlg::PostNcDestroy() 
{
	delete this;
}
