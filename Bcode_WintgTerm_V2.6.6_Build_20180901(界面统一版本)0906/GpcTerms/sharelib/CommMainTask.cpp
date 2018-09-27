#include "stdafx.h"
#include  "sysincl.h"
#include  "icommtask.h"
#include "ComWindow.h"
#include "tg_function_config.h"

#include <memshare.h>
#pragma comment(lib,"version.lib")

CComWindow g_cMainComWindow;
bool       g_inited;

long  g_lServerEntityID=0;
long  g_lLocalEntityID=0;

bool  InitComTask(long local_eid, const char *configfile, short nMSSPort) // 20150703 nMSSPort应有调用它的函数设置,即WintgTerm和LayoutTerm分别设置自己的监控端口
{
	g_cMainComWindow.CreateEx(0,			  
		AfxRegisterWndClass(NULL),
		"通信监控窗口",
		WS_OVERLAPPED|WS_CAPTION|WS_BORDER,
		0,0,480,210,
		AfxGetMainWnd()->GetSafeHwnd(),NULL,NULL);
	
	g_cMainComWindow.m_cUniComm.SetMainConfigFile(configfile);  
	g_cMainComWindow.m_cUniComm.SetCommTaskConfigFile(configfile); 
	g_cMainComWindow.m_cUniComm.SetLocalEntityID(local_eid);
	
	if (nMSSPort>0)
	{
		try
		{
			SetMSCommPortInit(nMSSPort);
			GpcLogSysMessage(0,"综合维护功能打开, 端口号为 %d", nMSSPort);
		}
		catch(...)
		{
			GpcLogSysMessage(0,"[Error] unicomm.ocx 不支持 综合维护功能");
			gpcdef.SetDisableMSSAlarmPort(); // 关闭端口
		}
	}

	g_inited=true;  // 放在这里是考虑start 中可能回调AddRela  2009.01.20
	g_cMainComWindow.m_cUniComm.Start();
	if(g_lServerEntityID>0)
		g_cMainComWindow.m_cUniComm.SetRelateEntityID(g_lServerEntityID); // 必须在Start() 以后调用,或者在STATUS==254时调用
	g_lLocalEntityID=local_eid;

	return g_inited;
}

bool EndComTask()
{
	if(g_inited)
	{ 
		GpcLogSysMessage(0,"Unicomm通信任务终止.");
		Sleep(200);
		g_cMainComWindow.m_cUniComm.Stop(); 
		g_cMainComWindow.DestroyWindow(); 
		g_inited=false;
	}
	return true;
}

bool RestartComTask()
{
	if(!g_inited)
		return false;
	g_cMainComWindow.m_cUniComm.Stop();
	g_cMainComWindow.m_cUniComm.Start();
	return true;
}

bool StopComTask(short restart_delay)
{
	if(!g_inited)
		return false;
	g_cMainComWindow.m_cUniComm.StopT(restart_delay);
	return true;
}

//默认自动LABEL,以本机ID发出
bool  SendCommMessage(long dest_id,short flag, BYTE *msgbuf,int msglen) 
{
	if(!g_inited)
		return false;

	BYTE *pdata=NULL;

	if(dest_id==0)
		dest_id=g_lServerEntityID;

	COleSafeArray byte_array;
	SAFEARRAYBOUND rgsabounds[1];
	rgsabounds[0].cElements=msglen; // 元素个数
	rgsabounds[0].lLbound =0;  // 数组从0开始
	byte_array.Create(VT_I1, 1, rgsabounds); // 1维

	byte_array.AccessData((LPVOID*)&pdata);

	if(pdata)
	{
		memcpy(pdata,msgbuf,msglen);
	}
	byte_array.UnaccessData();

	if(g_cMainComWindow.m_cUniComm.PostMessage(0,dest_id,0,flag,byte_array)) //默认自动LABEL,以本机ID发出
		return false;
	else
		return true;

}


bool  PostLogMessage(int type,int pri,const char *msg)
{
	if(!GetTGFunctionConfig()->IsSendLogToLogsvr())
		return false;

	if(!g_inited)
		return false;
	if(g_cMainComWindow.m_cUniComm.LogMessage(type,pri,g_lLocalEntityID,msg))
		return false;
	else
		return true;
}

// AddReleaEntity应该在Start以后或者CommStatus为254时调用!!  // 2009.1.22
bool  AddReleaEntity(int entity_id)
{
	if(!g_inited)
		return false;
	return  g_cMainComWindow.m_cUniComm.SetRelateEntityID(entity_id);
}

bool  IsEntityActive(int entity_id)
{
	if(!g_inited)
		return false;
	return g_cMainComWindow.m_cUniComm.GetRelateEntityStatus(entity_id);
}

bool IsCommConnected()
{
	if(!g_inited)
		return false;
	if(g_cMainComWindow.m_cUniComm.GetCommStatus()>=2)
		return true;
	else
		return false;
}

int GetCommTaskStatus()
{
	if(!g_inited)
		return -1;
	else
		return g_cMainComWindow.m_cUniComm.GetCommStatus();
}

static bool g_bCommWindowVisible=false;
bool  IsCommWindowVisible()
{
	return g_bCommWindowVisible;
}

void  ShowCommWindow(bool visible)
{

	if(!g_inited)
		return;
	if(!g_bCommWindowVisible)
	{
		g_bCommWindowVisible=true;
		g_cMainComWindow.CenterWindow(); 
		g_cMainComWindow.ShowWindow(SW_SHOW);
	}
	else
	{
		g_bCommWindowVisible=false;;
		g_cMainComWindow.ShowWindow(SW_HIDE);
	}

}

int  GetChineseChar(BYTE* logbuf, int len)
{
	int flag=0;
	for(int i=0; i<len; i++)
	{
		BYTE ch = logbuf[i];
		if(ch>=128) // 汉字
		{
			flag++;
		}
	}

	// 完整汉字
	if(flag%2 == 0)
		return len;
	else
		return len-1;
}

void GpcLogTrainMessage(short process_id, long id, const char *fmt, ...)
{
	static char logbuf[81920];
	memset(logbuf, 0, sizeof(logbuf));

	va_list va;
	va_start(va,fmt);
	vsnprintf(logbuf,sizeof(logbuf)-1,fmt,va);
	va_end(va);

	char tmp[256];

	int nOffset = 0;
	int nLogLen = min(strlen(logbuf), sizeof(logbuf));
	while(nLogLen > nOffset)
	{
		memset(tmp, 0, sizeof(tmp));
		if(nLogLen-nOffset > 240)
		{
			int len = GetChineseChar((BYTE*)logbuf+nOffset, 240);
			memcpy(tmp, logbuf+nOffset, len);
			nOffset += len;
		}
		else
		{
			memcpy(tmp, logbuf+nOffset, nLogLen-nOffset);
			nOffset = nLogLen;
		}

		tmp[255]=0;
	
		sysprint(10,process_id,tmp);
	
		if(g_inited && GetTGFunctionConfig()->IsSendLogToLogsvr())
		{
			g_cMainComWindow.m_cUniComm.LogMessage(2,10,g_lLocalEntityID,tmp);
		}
	}
}

void GpcLogSysMessage(short process_id, const char *fmt, ...)
{
	char tmp[256];
	va_list va;
	va_start(va,fmt);
	vsnprintf(tmp,sizeof(tmp)-1,fmt,va);
	va_end(va);
	tmp[255]=0;

	sysprint(10,process_id,tmp);

	if(!GetTGFunctionConfig()->IsSendLogToLogsvr())
		return;

	if(!g_inited)
		return;
	g_cMainComWindow.m_cUniComm.LogMessage(1,10,g_lLocalEntityID, tmp);

}

void GpcLogDebugMessage(short process_id, const char *fmt, ...)
{
	char tmp[256];
	va_list va;
	va_start(va,fmt);
	vsnprintf(tmp,sizeof(tmp)-1,fmt,va);
	va_end(va);
	tmp[255]=0;

	sysprint(10,process_id,tmp);

	if(!GetTGFunctionConfig()->IsSendLogToLogsvr())
		return;

	if(!g_inited)
		return;
	g_cMainComWindow.m_cUniComm.LogMessage(0,10,g_lLocalEntityID, tmp);
}

// 默认不带序号发送
bool SendBMsg(CLBTH::BaseMsg* pMsgs[], int num,long dest_entity,short flag)
{
	if(dest_entity==0)
		dest_entity=g_lServerEntityID;

	static char msgbuf[80960];
	int rs = -1;
	int i = 0;
	int data_len;
	while(i < num)
	{
		CLBTH::Archive ar(CLBTH::Archive::store, sizeof(msgbuf), msgbuf);
		CLBTH::HRCODE hr;
		for(; i<num; i++)
		{
			hr = (*pMsgs)->serialize(ar);
			if(hr.failed())
			{
				if(hr.sys_code != CLBTH::HRCODE::E_WRITE_OUTOFBUFFER)
					return false;
				else
					break;
			}
			pMsgs ++;
		}
		data_len = ar.getCurrentPos();
		SendCommMessage(dest_entity,flag,(BYTE *)msgbuf,data_len);
		//	wfp.Write(msgbuf, data_len);
		//GpcLogTrainMessage(0, 0, "向实体 %d 发送 %d字节数据:", dest_entity, data_len);
		//CString tmp, log;
		//for(int i=0; i<data_len; i++)
		//{
		//	tmp.Format("%0.2X ", (BYTE)msgbuf[i]);
		//	log += tmp;
		//}
		//GpcLogTrainMessage(0, 0, "%s", log);
	}
	return true;
}

bool SendBMsg(CLBTH::BaseMsg* pOneMsg,long dest_entity,short  flag)
{
	CLBTH::BaseMsg* pMsgs[1];
	pMsgs[0]=pOneMsg;
	return SendBMsg(pMsgs,1,dest_entity,flag);
}

bool SendStringMessage(const char *token,const char *content,long eid)
{
	CLBTH::MsgStringMessage msg;
	msg.SetTokenContent(token,content);
	return SendBMsg(&msg,eid,0);
}
int  get_max_comm_length(long entity)
{
	return 4000;//20130428 下计划分包缩小 原值为4096
}
// 下述代码从服务器获取
bool  SendArMessage(long entity, CLBTH::Archive &ar,int flag)
{
	return SendCommMessage(entity,flag,ar.getBuffer(),ar.getCurrentPos());
}

bool CheckSendMessage(long entity, CLBTH::Archive &ar, short next_msglen,int flag)
{
	short max=get_max_comm_length(entity);
	short cur=ar.getCurrentPos();

	if (max < cur) 
	{
		//task.ErrLog("fatal err in relay message %d %d to e%d", max, cur, entity);
		//return false;
	}

	bool rs=TRUE;
	if (max < cur+next_msglen) 
	{
		if (!SendArMessage(entity,ar,flag))
			rs = false;
		ar.seek(CLBTH::Archive::BEGIN, 0);
	}
	return rs;
}
bool  CheckSendMessage(long entity, CLBTH::Archive &ar, const CLBTH::BaseMsg &msg,int flag)
{
	CLBTH::BaseMsg &t = (CLBTH::BaseMsg &)msg;
	size_t len = t.getObjectLength();
	/*
	if (SUCCESS == CONFIG.IfSupportMessageCheckLength()) 
	{
	Archive lar;
	lar << msg;
	if (len != lar.getCurrentPos()) 
	{
	task.ErrLog("construct msg %d err write_len %d cal %d", msg.getTypeIDVirtual(), lar.getCurrentPos(), len);
	len = lar.getCurrentPos();
	}
	}
	*/
	return CheckSendMessage(entity, ar, len,flag);
}


void SetDefaultServerEntityID(long eid)
{
	g_lServerEntityID=eid;
}


void  SetRelaEntityIDForStatusDetecting(int *pArray,int count)
{
	if(!g_inited)
		return;
	//g_cMainComWindow.m_cUniComm.ResetRelateEntityID();
	for(int i=0;i<count;i++)
	{
		g_cMainComWindow.m_cUniComm.SetRelateEntityID(*pArray++); 
	}
}

long  GetLocalEntityID()
{
	return g_lLocalEntityID;
}

void SetMSCommPortInit(short port)
{
	g_cMainComWindow.m_cUniComm.SetnMSPort(port);
}
//容量属性范围10-20，其中：
//10 调整图与服务器同步状态 11 调整图与ocx同步状态
//12 日班计划与服务器同步状态 13 日班计划与ocx同步状态
//14-20 预留
void SetMSCapacityInit()
{
	g_cMainComWindow.m_cUniComm.SetMSAttribIndexRange(MS_ERROR_STATUS,10,20);
	g_cMainComWindow.m_cUniComm.SetMSAttribIndexRange(EVENT_STATUS,0,1);
}
//连接属性索引表: 20130419 hjz
//10---svr_db
//11---svr_mdb
//12---svr_sdb
//13---svr_basic
//14---svr_shift
//15---svr_history
//16---tdcommsvr
//17-100---所有svr_tg
void SetMSLinkInit(int Num)
{
	g_cMainComWindow.m_cUniComm.SetMSAttribIndexRange(LINK_STATUS,MS_LINK_STATUS_DB,MS_LINK_STATUS_TG+Num-1);
}
void SetMSCapacityAlarm(short AttribIndex, char* alarm, short flag)
{
	if(!g_inited)
		return;
	g_cMainComWindow.m_cUniComm.SetMSStatus(MS_ERROR_STATUS,0,AttribIndex,NULL,0,0,0,alarm,flag);
	//sysprint(LOG_LVL_SYS,0,"[WINTG]向综合维护系统发送报警信息,属性: %d, 状态: %s",AttribIndex, alarm);
}

void SetMSLinkAlarm(short AttribIndex, unsigned long Value, short flag)
{
	if(!g_inited)
		return;
	g_cMainComWindow.m_cUniComm.SetMSStatus(LINK_STATUS,0,AttribIndex,NULL,Value,0,0,NULL,flag);
	//sysprint(LOG_LVL_SYS,0,"[WINTG]向综合维护系统发送连接信息,属性: %d, 状态: %d",AttribIndex,Value);
}

void SetMSVersionAlarm(LPCTSTR versionStatus)
{
	if(!g_inited)
		return;
	CString tmp;
	tmp.Format("version");
	
	g_cMainComWindow.m_cUniComm.SetMSStatus(EVENT_STATUS,0,0,LPCTSTR(tmp),0,0,0,versionStatus,0);
	//sysprint(LOG_LVL_SYS,0,"[WINTG]向综合维护系统发送版本信息,版本: %s",versionStatus);
}
int	AddTGSectionList(int nSectionNo,int nTgSectionNum,int *nTgSectionList, int max);
int  GetMSLinkAttribIndexByEntity(long entity)
{
	if (entity<=0)
		return 0;

	if (entity == GetSpecialEntityID(SERVER_TYPE_DB,0))
		return MS_LINK_STATUS_DB;
	else if (entity == GetSpecialEntityID(SERVER_TYPE_DATASTORE,0))
		return MS_LINK_STATUS_MDB;
	else if (entity == GetSpecialEntityID(SERVER_TYPE_PARA,0))
		return MS_LINK_STATUS_SDB;	
	else if (entity == GetSpecialEntityID(SERVER_TYPE_BASIC_TG,0))
		return MS_LINK_STATUS_BASIC;
	else if (entity == GetSpecialEntityID(SERVER_TYPE_SHIFT_TG,0))
		return MS_LINK_STATUS_SHIFT;
	else if (entity == GetSpecialEntityID(SERVER_TYPE_HISTDATA,0))
		return MS_LINK_STATUS_HISTORY;
	else if (entity == GetSpecialEntityID(SERVER_TYPE_TDCOMM,0))
		return MS_LINK_STATUS_TD;
	else
	{
		int nTgSectionNum=0;
		nTgSectionNum=gpcdef.GetSvrTgEntityCnt();
	
		for(int k=0; k<nTgSectionNum; k++)
		{
			if (entity==gpcdef.GetSvrTgEntityIDByIndex(k))
				return gpcdef.GetSvrTgAttributNoByIndex(k);
		}
	}

	return 0;
}
