#include "stdafx.h"
#include  "sysincl.h"
#include  "icommtask.h"
#include "ComWindow.h"
#include "function.h"
#include "gpcdef.h"
#include "memshare.h"

void SaveLoginInfoToGPCDEF();

int   g_nUserLoginStatus=0;  //0: 未登录 1:登录成功 -1:登录失败(无法回复)

bool  g_bLoginWindowOpened=false; //登录窗口是否打开,用于判断是否自动重发第二阶段的LOGIN

int   g_nShiftID;  // 本地设置,用户输入
char  g_szUserID[64];   // 本地设置,用户输入
char  g_szPassword[64]; // 本地设置,用户输入 
char  g_szUserName[64]; //  ==g_szRemoteUserName
char  g_szRemoteUserName[64]; // 需要从服务器的replay消息中获取
char  g_szRemotePassword[64]; // 需要从服务器的replay消息中获取
bool  g_bEnableAutoRelogin=false;

bool  g_bCommServerMode=false;

void  SetCommServerMode(bool flag)
{
		g_bCommServerMode=flag;
}
bool  IsInCommServerMode()
{
	  return g_bCommServerMode;
}

void SetCurrentLoginInfo(const char *userid,const char *password,int shiftid)
{
    if(shiftid)
		g_nShiftID=shiftid;
	else
		g_nShiftID=Get_CurrentShiftid();

	strncpy(g_szUserID,userid,32);
	g_szUserID[31]=0;

	strncpy(g_szPassword,password,32);
	g_szPassword[31]=0;
	g_nUserLoginStatus=0;
}



bool RemoteSetUserInfo(const char *username,const char *pwd, bool bLoginDlgOpened)
{
	strncpy(g_szRemoteUserName,username,32);
	g_szRemoteUserName[32]=0;
	strncpy(g_szRemotePassword,pwd,32);
	g_szRemotePassword[32]=0;
	strcpy(g_szUserName,g_szRemoteUserName);
	if(stricmp(g_szPassword,"NOPASSWORD") &&  stricmp(g_szRemotePassword,g_szPassword)) 
		return false;
	else
	{
		if(!bLoginDlgOpened) // 2012.05.07登陆对话框不存在再保存, 否则由登陆对话框保存登陆用户信息
			SaveLoginInfoToGPCDEF();  // 此时密码检验通过，可以保存登录信息至gpcdef
		return true;
	}
}

bool IsLoginWindowOpened()
{
	return g_bLoginWindowOpened;
}
 
void  SetLoginWindowOpenFlag(bool flag)
{
		g_bLoginWindowOpened=flag;
}



bool  IsUserLogined()
{
	if(g_nUserLoginStatus==1)
		return true;
	else
		return false;
}
void LoginWindowSetAutoReloginEnabled(bool flag)
{
	strcpy(g_szPassword,"NOPASSWORD");  //因为登录窗口可能用IC卡验证密码,所以此处设置为NOPASSWORD
	g_bEnableAutoRelogin=flag;
}

bool AutoReloginEnabled()
{
	 return g_bEnableAutoRelogin;
}
void SaveLoginInfoToGPCDEF()
{
    gpcdef.SetCurrentUserid(g_szUserID);        //userid
	
	gpcdef.SetCurrentUsername(g_szUserName);
	
	gpcdef.SetUserPassword(g_szPassword);     //user password 
					
    gpcdef.SetLoginShiftid(g_nShiftID);        //login shiftid,e.g. 2002060701

	unsigned long m_usCurrentDate = (g_nShiftID/1000000 - 1990) * 380 +
						( g_nShiftID/10000-(g_nShiftID/1000000)*100 - 1) * 31 + 
						g_nShiftID/100 - (g_nShiftID/10000)*100  - 1 ;			 
	gpcdef.SetLoginDate(m_usCurrentDate);     //login date    

	g_bEnableAutoRelogin=true; //只有在存储了登录信息后，才能允许网络中断后自动登录
}

void  SetUserLoginStatus(int  flag)
{
	g_nUserLoginStatus=flag;

}

//  GPC 兼容方式登录
bool  SendMsgGpcAccessInfo(ushort uAccessStep) //  get userid from gpcdef
{
	CLBTH::MsgGpcAccessInfo mMsgGpcAccessInfo;
	int m_nCurrentSectionList[64];
	int m_nSectionNum  = sizeof(m_nCurrentSectionList) / sizeof(m_nCurrentSectionList[0]);
	gpcdef.GetSectionList(m_nSectionNum,m_nCurrentSectionList);
	int m_nCurrentSectionNo = m_nCurrentSectionList[0];

	strncpy(mMsgGpcAccessInfo.user_code,g_szUserID,sizeof(mMsgGpcAccessInfo.user_code)); // gpcdef.GetCurrentUserid());
    mMsgGpcAccessInfo.job_id = gpcdef.GetUserIdentify(); ///工作类型：计划员，助调
	mMsgGpcAccessInfo.gpc_id = gpcdef.GetConsoleid(); //唯一的,由username转换而来   
	mMsgGpcAccessInfo.con_id = m_nCurrentSectionNo; 

    mMsgGpcAccessInfo.step  = ( CLBTH::MsgGpcAccessInfo::ACCESS_STEP)uAccessStep;
	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if(server_id>0)
	   return SendBMsg(&mMsgGpcAccessInfo,server_id);
	else
	  return SendBMsg(&mMsgGpcAccessInfo,0);
}



static int	AddStationList(int nStationNo,int nAllStationNum,int *nAllStationList)
{
	int i;
	for(i=0;i < nAllStationNum;i++)
	{
		if(nAllStationList[i] == nStationNo)
			break;
	}
	if(i >= nAllStationNum)
	{
		nAllStationList[nAllStationNum] = nStationNo;
		return 0;
	}
	return -1;
}


bool SendMsgGpcLogin()
{ 
	GPCENTITY entityInfo;
	
	// get all station inform
	int nSectionNum,nSectionList[64];
	int nStationNum,nStationList[2048];
    int nAllStationNum=0,nAllEntityNum=0;
	int nAllStationList[2048],nAllStationEntityList[2048];
	memset(nAllStationList,0,sizeof(nAllStationList));
	memset(nAllStationEntityList,0,sizeof(nAllStationEntityList));
        
	nSectionNum = sizeof(nSectionList)/sizeof(nSectionList[0]);
	int rs=gpcdef.GetSectionList(nSectionNum,nSectionList);
	ASSERT( rs > 0);

	nAllStationNum = 0;

	// query all station in wintg section
	for(int i=0;i < nSectionNum;i++)
	{
		nStationNum = sizeof(nStationList)/sizeof(nStationList[0]);
		rs=gpcdef.GetStationList(nSectionList[i],nStationNum,nStationList);
		ASSERT(rs > 0);

		for(int j=0;j < nStationNum;j++)
		{
 			if(AddStationList(nStationList[j],nAllStationNum,nAllStationList) == 0)
			   nAllStationNum++;
		}
	}

	// query all station in layout page
	vector<int> sec_list;
	gpcdef.GetSectionListOfDisp(sec_list);
	for(i=0; i<sec_list.size(); i++)
	{
		vector<int> sta_list;
		gpcdef.GetStationListofDisp(sec_list[i], sta_list);

		for(int j=0;j < sta_list.size(); j++)
		{
			if(AddStationList(sta_list[j], nAllStationNum, nAllStationList) == 0)
				nAllStationNum++;
		}
	}
    
	nAllEntityNum = 0;
	for(i=0;i<nAllStationNum;i++)
	{
		if(!gpcdef.GetEntityInfo(STPC_WACHER,nAllStationList[i],entityInfo))
			continue;

		ASSERT(nAllEntityNum < sizeof(nAllStationEntityList)/sizeof(nAllStationEntityList[0]));
		nAllStationEntityList[nAllEntityNum++] = entityInfo.nEntityId;
	} 

	for(i=nAllStationNum;i<nAllStationNum*2;i++)
	{
		int nStationNo = nAllStationList[i-nAllStationNum];
		if(!gpcdef.GetEntityInfo(EQUIP_LIRC,nStationNo,entityInfo))
			continue;

		ASSERT(nAllEntityNum < sizeof(nAllStationEntityList)/sizeof(nAllStationEntityList[0]));
		nAllStationEntityList[nAllEntityNum++] = entityInfo.nEntityId;
	}
    
	//userid exists, then send login message: MsgGpcLogin->
    CLBTH::MsgGpcLogin mMsgGpcLogin;
  	
	ASSERT(nAllStationNum <= sizeof(mMsgGpcLogin.sta_list)/sizeof(mMsgGpcLogin.sta_list[0]));
	mMsgGpcLogin.sta_count = 0;
	for(i=0;i<nAllStationNum&&i<1024;i++)
	{
		mMsgGpcLogin.sta_list[i] = nAllStationList[i];
		mMsgGpcLogin.sta_count++;
	}

	ASSERT(nAllStationNum <= sizeof(mMsgGpcLogin.entity_list)/sizeof(mMsgGpcLogin.entity_list[0]));
    mMsgGpcLogin.entity_count = 0;
    for(i=0;i<nAllEntityNum&&i<2048;i++)
	{
		mMsgGpcLogin.entity_list[i] = nAllStationEntityList[i];
		mMsgGpcLogin.entity_count++;
	}

	int m_nCurrentSectionList[64];
	int m_nSectionNum  = sizeof(m_nCurrentSectionList) / sizeof(m_nCurrentSectionList[0]);
	rs=gpcdef.GetSectionList(m_nSectionNum,m_nCurrentSectionList);
	ASSERT(rs >0);

	int m_nCurrentSectionNo = m_nCurrentSectionList[0];
	

	mMsgGpcLogin.con_id = m_nCurrentSectionNo;  //区段  
	mMsgGpcLogin.job_id = gpcdef.GetUserIdentify(); ///工作类型：计划员，助调

	strcpy(mMsgGpcLogin.user_name,gpcdef.GetCurUserName());

	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if(server_id>0)
    	return SendBMsg(&mMsgGpcLogin,server_id);
	else
		return SendBMsg(&mMsgGpcLogin,0);
}

/////////////////////////前级级的处理 /////////

BOOL  CallMessageLoop() 
{
	MSG msg;
    BOOL bRet; 
    if((bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
    { 
        if (bRet == -1)
        {
           return FALSE;
        }
        else
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
        }
    } 
	return TRUE;
}
 



int AutoLogin(const char *userid, const char *username, const char *password,int shiftid) // 自动登录,免操作
{
	if(IsInCommServerMode())  // 通信服务器模式下，暂时不登陆
		return 0;
    bool no_send_msg;
	SetCurrentLoginInfo(userid,password,shiftid);   

	time_t waitfrom=time(NULL);
	no_send_msg=true;
	while(CallMessageLoop())  // 等待commwindow接收消息
	{
		if(no_send_msg && IsCommConnected())
		{
			SendMsgGpcAccessInfo(CLBTH::MsgGpcAccessInfo::WANT_PWD);
			no_send_msg=false;
		}
        time_t curtm=time(NULL);
		if(curtm-waitfrom>5)
			break;
		if(g_nUserLoginStatus!=0)
			break;
	}
	if(g_nUserLoginStatus==0) // 超时未登录时也应该保存当前的用户信息,否则采用IGSVR进入后没有用户信息
         SaveLoginInfoToGPCDEF();  // 当登录成功后,在收到USRID_OK的回应后,就进行保存了,所以无需此处调用
	return g_nUserLoginStatus;
}

// 下述代码暂时无用,如果每次调用 gpcdef.GetEntityInfo效率低的话,那么可提前读入
/*
#define  MAX_SERVER_TYPE_NUM 24
static struct{
	int  type;
	long entity_id;
}g_server_ids[MAX_SERVER_TYPE_NUM];
int g_server_num=0;

bool  InitServerIDs()
{
	return true;
}
*/
//long  GetSpecialEntityID(int GPCTERM_TYPE_DCMD,int con_id)
long  GetSpecialEntityID(int type,int con_id)
{
	if(!IsInCommServerMode())
		return 0;
	GPCENTITY entityInfo;
	if(type==SERVER_TYPE_MGATE)
	{
		if(gpcdef.GetEntityInfo(101,type,con_id,entityInfo))  // con_id 可匹配
		{
			return entityInfo.nEntityId; 
		}
		if(gpcdef.GetEntityInfo(101,type,0,entityInfo))  // con_id 不匹配
		{
			return entityInfo.nEntityId; 
		}
		return -1;
	}
	if(type==GPCTERM_TYPE_DCMD) // 获取当前用户对应的调度命令终端ENTITY
	{
		char *curuserid=gpcdef.GetDefaultUserName(); 
		if(!gpcdef.GetEntityInfo(curuserid,entityInfo))
			return -1;
		if(gpcdef.GetEntityInfo(5,entityInfo.nSpecial1,entityInfo.nSpecial2,entityInfo))  // 
		{
			return entityInfo.nEntityId; 
		}
		return -1;
	}
	if(type==SERVER_TYPE_GSMR) // 获取当前用户对应的调度命令终端ENTITY
	{
	   if(gpcdef.GetEntityInfo(150,0,con_id,entityInfo))  // con_id 可匹配
	   {
		return entityInfo.nEntityId; 
	   }
	   if(gpcdef.GetEntityInfo(150,0,0,entityInfo))  // con_id 不匹配
	   {
		return entityInfo.nEntityId; 
	   }
	   return -1;
	}
	if(type==SERVER_TYPE_MMPLAN)
	{
	   if(gpcdef.GetEntityInfo(101,type,con_id,entityInfo))  // con_id 可匹配
	   {
		return entityInfo.nEntityId; 
	   }
	   if(gpcdef.GetEntityInfo(101,type,0,entityInfo))  // con_id 不匹配
	   {
		return entityInfo.nEntityId; 
	   }
	   return -1;
	}
	if(type==SERVER_TYPE_PREDICT) // 转发服务
	{
	   if(gpcdef.GetEntityInfo(101, type, con_id, entityInfo))  // con_id 可匹配
	   {
		return entityInfo.nEntityId; 
	   }
	   if(gpcdef.GetEntityInfo(101, type, 0, entityInfo))  // con_id 不匹配
	   {
		return entityInfo.nEntityId; 
	   }
	   return -1;
	}

	if(type==SERVER_TYPE_GATEWAY)
	{
		if (gpcdef.GetEntityInfo(101,SERVER_TYPE_GATEWAY,0,entityInfo))
			return entityInfo.nEntityId;
		else
			return -1;
	}
    
	if(type==SERVER_TYPE_TG && gpcdef.IsTgMsgToGateway())
	{
		if (gpcdef.GetEntityInfo(101,SERVER_TYPE_GATEWAY,0,entityInfo))
			return entityInfo.nEntityId;
	}
	if((type==SERVER_TYPE_BASIC_TG || type==SERVER_TYPE_HISTDATA || type==SERVER_TYPE_SHIFT_TG || type==SERVER_TYPE_DB || type==SERVER_TYPE_DATASTORE) && gpcdef.IsOtherMsgToGateway())
	{
		if (gpcdef.GetEntityInfo(101,SERVER_TYPE_GATEWAY,0,entityInfo))
			return entityInfo.nEntityId;
	}
	// 获取服务器entity
	if(gpcdef.GetEntityInfo(101,type,con_id,entityInfo))  // con_id 可匹配
	{
		return entityInfo.nEntityId; 
	}
	if(gpcdef.GetEntityInfo(101,type,0,entityInfo))  // con_id 不匹配
	{
		return entityInfo.nEntityId; 
	}
	if(gpcdef.GetEntityInfo(101,SERVER_TYPE_GEN,con_id,entityInfo))  // con_id 可匹配
	{
		return entityInfo.nEntityId; 
	}
	if(gpcdef.GetEntityInfo(101,SERVER_TYPE_GEN,0,entityInfo))  // con_id 可匹配
	{
		return entityInfo.nEntityId; 
	}
	return 0;
}

bool IsRtServer(int entityid)
{
	GPCENTITY RtSvrInfo;
	gpcdef.GetEntityInfo(entityid,RtSvrInfo);

	if( RtSvrInfo.nGroupId==101 )
	{
		if( RtSvrInfo.nSpecial1 == SERVER_TYPE_RT )
			return true;
		else if( 0 == RtSvrInfo.nSpecial1 && entityid == GetSpecialEntityID(SERVER_TYPE_RT ) )
			return true;
	}

	return false;
}