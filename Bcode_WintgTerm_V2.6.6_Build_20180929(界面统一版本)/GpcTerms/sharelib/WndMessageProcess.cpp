#include "Stdafx.h"

#include "msgdispatch.h"
#include "icommtask.h"
#include "BaseMsg\clbth.h"
extern bool AutoReloginEnabled(); // defined in commtask.cpp


list<MSGPROCWNDSTRUCT> msgProcWndList;
static L3_MSG_HEADER g_curMsgHeader;

bool  RegisterMsgProcHandleWnd(HWND hwnd,int msgid,int flag)  //flag: 0 send_message  1: post_message
	{
		MSGPROCWNDSTRUCT x;
		x.hwnd=hwnd;
		x.msgid=msgid;
		x.flag =flag;
		msgProcWndList.push_back(x);
		return true;
}

bool UnRegisterMsgProcHandleWnd(HWND hwnd,int msgid)
	{
		list<MSGPROCWNDSTRUCT>::iterator iter = msgProcWndList.begin();
		while(iter != msgProcWndList.end())
		{
			MSGPROCWNDSTRUCT x=*iter;
			if(x.hwnd==hwnd && (msgid==x.msgid || msgid==-1))
				iter=msgProcWndList.erase(iter);
			else
				iter++;
		}
		return false;
	}
#define MAX_MSG_HANDLER_NUM 8
DIRECT_MSG_HANDLER  g_DirectMsgHandlers[MAX_MSG_HANDLER_NUM];
DIRECT_MSG_HANDLER  g_XMLDirectMsgHandlers[MAX_MSG_HANDLER_NUM];

bool  RegisterDirectMsgHandler(DIRECT_MSG_HANDLER p)
{
	for(int i=0;i<MAX_MSG_HANDLER_NUM;i++)
	{
		if(g_DirectMsgHandlers[i]==NULL)
		{
			g_DirectMsgHandlers[i]=p;
			return true;
		}
	}
	return false;
}


bool  UnRegisterDirectMsgHandler(DIRECT_MSG_HANDLER p)
{
	for(int i=0;i<MAX_MSG_HANDLER_NUM;i++)
	{
		if(g_DirectMsgHandlers[i]==p)
		{
			g_DirectMsgHandlers[i]=NULL;
			return true;
		}
	}
	return false;
}

bool  RegisterXMLDirectMsgHandler(DIRECT_MSG_HANDLER p)
{
	for(int i=0;i<MAX_MSG_HANDLER_NUM;i++)
	{
		if(g_XMLDirectMsgHandlers[i]==NULL)
		{
			g_XMLDirectMsgHandlers[i]=p;
			return true;
		}
	}
	return false;
}


bool  UnRegisterXMLDirectMsgHandler(DIRECT_MSG_HANDLER p)
{
	for(int i=0;i<MAX_MSG_HANDLER_NUM;i++)
	{
		if(g_XMLDirectMsgHandlers[i]==p)
		{
			g_XMLDirectMsgHandlers[i]=NULL;
			return true;
		}
	}
	return false;
}

bool  DeliverWndMessage(const L3_MSG_HEADER *msghdr,int msgid,const CLBTH::BaseMsg *pmsg,int flag) 
 {

   if(msgProcWndList.size() <= 0)
		return false;
	list<MSGPROCWNDSTRUCT>::iterator iter = msgProcWndList.begin();
	MSGPROCWNDSTRUCT x;
   	while(iter != msgProcWndList.end())
	{
		    x = *iter;
			if(!::IsWindow(x.hwnd))
			{
				iter=msgProcWndList.erase(iter); 
				continue;
			}
			else
			if(msgid==x.msgid)
				{
		    			UNI_MESSAGE_STRUCT msg_inform;
					    msg_inform.source_id=msghdr->source_id;
					    msg_inform.msg_id=msgid;
					    msg_inform.pmsg=pmsg;
						msg_inform.need_delete=0;
					    ::SendMessage(x.hwnd,UM_UNI_MESSAGE,0,(LPARAM)&msg_inform);
					
				}
			iter++;
	}
	return true;
 }

	
	
extern  int	LocalProcessCommMessage( const L3_MSG_HEADER *phdr, BYTE *msgbuf, int msglen); // defined in gpc_spec_proc.cpp


int ProcessCommMessage(const L3_MSG_HEADER *msghdr,BYTE  *ptr, int length)
{
	if(length)
	{
	    LocalProcessCommMessage(msghdr,ptr,length);
		for(int i=0;i<MAX_MSG_HANDLER_NUM;i++)
		{
			if(g_DirectMsgHandlers[i])
				(*g_DirectMsgHandlers[i])(msghdr->source_id,ptr,length);
		}
	}
	return 0;
}

int ProcessXMLCommMessage(const L3_MSG_HEADER *msghdr,BYTE  *ptr, int length)
{
	if(length)
	{
		for(int i=0;i<MAX_MSG_HANDLER_NUM;i++)
		{
			if(g_XMLDirectMsgHandlers[i])
				(*g_XMLDirectMsgHandlers[i])(msghdr->source_id,ptr,length);
		}
	}
	return 0;
}


int LocalProcessEntityStatus(int status)
{
	return 0;
}

extern  bool RemoteSetUserInfo(const char *username,const char *pwd, bool bLoginDlgOpened);

int	LocalProcessCommMessage( const L3_MSG_HEADER *phdr, BYTE *msgbuf, int msglen)
{
	
	CLBTH::Archive lar( CLBTH::Archive::load, msglen,msgbuf);
	CLBTH::MSGHEAD msghead;
	msghead.length=0;
	while(!lar.eof()) {
		msghead = CLBTH::BaseMsg::getMsgHead(lar);
		if(msghead.length < CLBTH::MSGHEAD::SIZEOF || msghead.length > lar.getCount()) {
			break;
		}
		switch(msghead.id)
		{
		 case MSGID_MsgGpcAccessReply:
			{
				CLBTH::MsgGpcAccessReply  msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);
				BOOL  login_err=TRUE;
				CString err_desc;
				switch(msg.status)
				{
				case CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_OK:
					{
						bool bLoginDlgOpened=IsLoginWindowOpened();
						if(RemoteSetUserInfo(msg.user_name,msg.pwd,bLoginDlgOpened)) // return false : password check err!
						{
							if(!bLoginDlgOpened) //只有在登录窗口没有打开的情况下，才自动发
							{
								SendMsgGpcAccessInfo( CLBTH::MsgGpcAccessInfo::PWD_OK); 
								//////////!!!!!!!!!!!!!等待1s为了让服务器启动服务进程 !!!!!!!
								Sleep(1000);
							}
							login_err=FALSE;
						}
						else
						{
							err_desc="用户密码错误! 请重新登录!";
						}
					}
					 break;
				case CLBTH::MsgGpcAccessReply::GPC_ACCESS_SERVER_READY:
					 login_err=FALSE;
					 SendMsgGpcLogin();
 				  	 SetUserLoginStatus(1);
				     break;						
				case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_NOT_EXIST:
					 err_desc="此用户不存在,请重新登录!";
					 break;
			     case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_INVALID_GPC_ID:
                     err_desc="用户代码不正确,请重新登录!";
					break;
				 case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_JOBID_NOT_EXIST:
					  err_desc="用户工作权限不存在,请联系管理员!";
		            break;
	             case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_CONID_NOT_EXIST:
		              err_desc="用户对应的调度台不存在,请联系管理员!";
		           break;
	            case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_GPCID_ALREADY_ONLINE:
		              err_desc="用户重复登陆，请联系管理员!";
		           break;
	             case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_CONID_DISMATCH:
	              	 err_desc="用户不能操作本台,请重新登录!";
		            break;
	             case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_USERID_JOBID_DISMATCH:
		            err_desc="用户代码和权限不匹配,请重新登录!";
	             	 break;
	             //case  CLBTH::MsgGpcAccessReply::GPC_ACCESS_UNKNOWN_MESSAGE:
				 case CLBTH::MsgGpcAccessReply::GPC_LOGIN_CHECK_OK:
					 login_err=FALSE;
					 break;
				 default:
		             err_desc="未知消息错误,请联系管理员!";
		             break;		 
					
				}
				if(login_err && !IsLoginWindowOpened())  // 如果有登录窗口,则无需此处提示
				{
					bool status=false;
					int deley_time=0;
					if(msg.status==CLBTH::MsgGpcAccessReply::GPC_ACCESS_GPCID_ALREADY_ONLINE)  //用户已登录可能延后重试可恢复
					{
						deley_time=15;
						status=StopComTask(deley_time); //此处不能EndCommTask!
					    AfxMessageBox(err_desc);						
					}
					else
					{
						status=StopComTask(deley_time); //此处不能EndCommTask!
						AfxMessageBox(err_desc);
						SetUserLoginStatus(-1);
					}
					GpcLogSysMessage(0,"unicomm延后连接,status=%d delay_time=%d %s",status,deley_time,err_desc);
				}
			} 
			break;
		 case MSGID_MsgParamentReply:
			 {
				 CLBTH::MsgParamentReply msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
				 break;
			 }
		 case MSGID_MsgHugeDataCtrl:
			 {
				 CLBTH::MsgHugeDataCtrl msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 if (msg.msgid==MSGID_MsgParamentReply)
				 {
					DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
				 }
				 break;
			 }
		 case MSGID_MsgParamentOperation:
			 {
				 CLBTH::MsgParamentOperation msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
				 break;
			 }
		 case MSGID_MsgAlarm:
			 {
				CLBTH::MsgAlarm msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);
			}
			break;
		case MSGID_MsgAlarmExd:
			{
				CLBTH::MsgAlarmExd msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);
			}
			break;
		case MSGID_MsgNotePadFormat:
			{
				CLBTH::MsgNotePadFormat msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);
			}
			break;
		case MSGID_MsgAccidentOper:
			{
				CLBTH::MsgAccidentOper msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);
			}
			break;
		case MSGID_MsgSimpleStationCarCtrl:
			{
				CLBTH::MsgSimpleStationCarCtrl msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgSimpleStationCarFormat:
			 {
			 	CLBTH::MsgSimpleStationCarFormat msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgSubaoFormat:
			 {
			 	CLBTH::MsgSubaoFormat msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgStringMessage:
			 {
			 	CLBTH::MsgStringMessage msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }	
			 break;
		 case MSGID_MsgElecArmAnswer:			 
			 {
			 	CLBTH::MsgElecArmAnswer msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }	
			 break;
		 case MSGID_MsgElecArmSet:
			 {
				CLBTH::MsgElecArmSet msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgConvertBasicSchd:			 
			 {
			 	CLBTH::MsgConvertBasicSchd msg;
				CLBTH::HRCODE hrcode=msg.serialize(lar);
				if(hrcode.failed()) 
					break;
				DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }	
			break;
		 case MSGID_MsgStccCommandList:
			 {
				 CLBTH::MsgStccCommandList msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			break;
		 case MSGID_MsgStccReport:
			 {
				 CLBTH::MsgStccReport msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgStccCommandSendResult:
			 {
				 CLBTH::MsgStccCommandSendResult msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgTccCommand:
			 {
				 CLBTH::MsgTccCommand msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgTccCommandData2:
			 {
				 CLBTH::MsgTccCommandData2 msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgStccCommandList2:
			 {
				 CLBTH::MsgStccCommandList2 msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgStccReport2:
			 {
				 CLBTH::MsgStccReport2 msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgStccCommandSendResult2:
			 {
				 CLBTH::MsgStccCommandSendResult2 msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgTccCommand2:
			 {
				 CLBTH::MsgTccCommand2 msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;	 
		  case MSGID_MsgStccCommandIDRequest:
			 {
				 CLBTH::MsgStccCommandIDRequest msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;	 
		 case MSGID_MsgGiveDDyName:
			 {
				 CLBTH::MsgGiveDDyName msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;	 
		 case MSGID_MsgWordPhasePlanStatus:
			 {
				 CLBTH::MsgWordPhasePlanStatus msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		 case MSGID_MsgWordPhasePlanSend:
			 {
				 CLBTH::MsgWordPhasePlanSend msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;
		case MSGID_MsgUserManager:
			 {
				 CLBTH::MsgUserManager msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }	
			 break;	 
		 case MSGID_MsgStPlanAutoTrigUpdateResult:
			 {
				 CLBTH::MsgStPlanAutoTrigUpdateResult msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }	
			 break;	
		 case MSGID_MsgBasicScheduleSet:
			 {
				 CLBTH::MsgBasicScheduleSet msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }	
			 break;	
		 case MSGID_MsgConvertBasicSchdNew:
			 {
				 CLBTH::MsgConvertBasicSchdNew msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }	
			 break;	
		 case MSGID_MsgChangeSideReq:
			 {
				 CLBTH::MsgChangeSideReq msg;
				 CLBTH::HRCODE hrcode=msg.serialize(lar);
				 if(hrcode.failed()) 
					 break;
				 DeliverWndMessage(phdr,msghead.id,&msg,0);  // msg in heap!
			 }
			 break;	
		 default:
			if(lar.seek(CLBTH::Archive::CURRENT, msghead.length) != msghead.length)
				break; 
		}
	}
	return 0;
}

static void ProcessBrokenLog()
{
////////////////////////////////////////////2009.03.06///////// 判断一分钟内中断次数超过 MAX_BROKEN_LOG，则终止通信
#define MAX_BROKEN_LOG  5   // 60s 内中断5次
#define BROKEN_LOG_RANGE 60 // 60 s
static time_t   tmBrokenLog[MAX_BROKEN_LOG];

	int i;
	time_t curtm=time(NULL);
	GpcLogSysMessage(0,"通信中断");

	for(i=0;i<MAX_BROKEN_LOG;i++)
	{
	   if(tmBrokenLog[i]>0 && (curtm-tmBrokenLog[i])>BROKEN_LOG_RANGE)  // 记录已经超出范围，则复位
		  tmBrokenLog[i]=0;
	   if(tmBrokenLog[i]==0)
		   break;
	}
	if(i<MAX_BROKEN_LOG)
	{
		tmBrokenLog[i]=curtm; //记录本次中断时间
	}
	else
	{
		bool status=StopComTask(0);
		GpcLogSysMessage(0,"通信状态不稳定，60秒内连续中断次数=%d status=%d", i,status);
		AfxMessageBox("通信状态不稳定，请联系系统管理人员!");
	}
}

 //////////////////////////////////////////////////////////////

int LocalProcessConnectStatus(int status)
{
  if(status<2)  // broken
	  ProcessBrokenLog();

  if(IsInCommServerMode()) // 在通信服务器连接模式下,无需发送GPCACCESSINFO
	  return 0;
  if(status==2 && AutoReloginEnabled()) //  连接重新建立,自动发送登录
  {
	SendMsgGpcAccessInfo(CLBTH::MsgGpcAccessInfo::WANT_PWD);
  }
   return 0;
}

