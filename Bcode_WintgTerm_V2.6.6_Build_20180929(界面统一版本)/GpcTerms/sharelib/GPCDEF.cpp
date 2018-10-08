
#include "stdafx.h"

#include <io.h>
#include <sys/stat.h>

#include "oscalls.h"
#include "defines.h"
#include "gpcdef.h"
#include "function.h"
//#include "passworddlg.h"
#include "IniFile.h"       ////new added 2004
#include <algorithm>

//add by chenzj
// #include "MacroSwitch.h"
//end add

#include "ConfigFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#import  <msxml.dll> named_guids
using namespace MSXML;
IMPLEMENT_SERIAL(CGpcdef,CObject,2)
 

static int TaskDefs[] = {0,
	UPDATE_BASIC|PLOT_TASK,	//a		//GPC_SCHEDULE 计划员台
	////COMM_TASK|VIEW_SDLAYOUT|(WINTG_TASK & ~UPDATE_BASIC & ~VIEW_GRAPH )|REPLAY_CONTROL|DISPATCH_TASK ,	            //b		//GPC_DISPATCHER 行车调度员
	(WINTG_TASK & ~UPDATE_BASIC & ~VIEW_GRAPH ) ,	//b		//GPC_DISPATCHER 调度员台
	(WINTG_TASK & ~UPDATE_BASIC & ~VIEW_GRAPH )|DISPATCH_TASK,					//c		//GPC_OPERATOR 操作员台
	UPDATE_HISTORY,	//d		//GPC_DISPATCHERS 调度长台
	0,					//e		//GPC_SCREEN_LAYOUT 背投
	WINTG_TASK,//	 for test		//f		//GPC_TRAIN 培训台
	0,					//g		//GPC_SERVICE 
	AFFAIR_TASK,					//h		//GPC_MAINTAIN 电务维修台
	PLOT_TASK,									//i		//GPC_PLOT 绘图台
	0,//					//j		//GPC_MACHINE 机调员台
	WINTG_TASK|PLOT_TASK|DISPATCH_TASK,//	k	//GPC_GOODS 货调员台  // K 用于施工调度和助调合并情况 2008.7.11
	REPORT_TASK|PLOT_TASK,						//l		//GPC_SCREEN_TG 统计员台
	AFFAIR_TASK,		//m		//GPC_AFFAIR_SERVER
	0,					//n		//GPC_SUPER_DISPATCH值班主任台
	0,					//o		//GPC_OTHER_DISPATCH
	0/*DIGITAL_MAP_TASK|WINTG_TASK|*/,      //p		//GPC_SUPER_MAINTAIN系统维护台
	WINTG_TASK|PLOT_TASK|DISPATCH_TASK,	//q	 //GPC_TEST  调试员台VIEW_LAYOUT|VIEW_GRAPH
	(WINTG_TASK & ~UPDATE_BASIC & ~VIEW_GRAPH )|DISPATCH_TASK,	 //r  综合维修调度员台,新增
    

};

//运行图相关的用高位的那个字节定义权限
static DWORD RightDefs[] ={
	0,                                                                        
    0 ,                                        //a 计划员
 	WINTG_EDIT | STATION_STORE_TRAIN | DRAW_MARK ,             //b 调度员
#ifdef BUREAU_SHANGHAI
	STATION_STORE_TRAIN  |  DISPATCH_NOTE_EDIT | ROUTE_OPERATE | DRAW_MARK,   //c 操作员---上海局宁武助调有MARK权限
#else
	STATION_STORE_TRAIN  |  DISPATCH_NOTE_EDIT | ROUTE_OPERATE,   //c 操作员
#endif
	WINTG_EDIT | DRAW_MARK,                         //d 调度长
	0 ,                         //e
	0 ,                         //f
	0 ,                         //g
	0 ,                         //h 电务维修
	0 ,                         //i
	0 ,                         //j
	WINTG_EDIT | DRAW_MARK |  DISPATCH_NOTE_EDIT |STATION_STORE_TRAIN  |  DISPATCH_NOTE_EDIT | ROUTE_OPERATE,  //k  // K 用于施工调度和助调合并情况 2008.7.11
	0 ,                         //l
	0 ,                         //m
	0 ,                         //n 值班主任
	0 ,                         //o 调试员
	0 ,                         //p 系统维护
    WINTG_EDIT | STATION_STORE_TRAIN | DRAW_MARK |  DISPATCH_NOTE_EDIT | ROUTE_OPERATE ,                         //q 调试员
#ifdef  BUREAU_NANCHANG
	WINTG_EDIT | DRAW_MARK |  DISPATCH_NOTE_EDIT,        // 综合维修调度员 // 南昌局允许施工调度修改计划和发送计划
#else
	/*WINTG_EDIT | */DRAW_MARK |  DISPATCH_NOTE_EDIT,        // 综合维修调度员
#endif

};
 

CGpcdef::CGpcdef()
{ 
	m_thisGpcEntity=0;
	m_nManualConvertActualTimeLength=0;
	m_bLimitModifyActualTime=false; // 不限制，可以修改
	m_bHaveRightManangRight=false; // 没有管理用户权限的权限
	memset(szPrinterName, 0, 64);
	
	m_nEnablePredict=0;
	m_nEnableMMPlanFromCAD=0;
	
	m_bRouteOkMark=false;
	m_bShowShowRouteTrig=false;
	m_bShowVirtualStation=false;
	m_bShowAutoAdjust=true;
	m_nDirectExecute=0;

	m_bShowKiloMeterButton=true;
	m_bShowTransInfoButton=false;

	m_nNewBasicDump=0;
	m_nShowSideNotConfirmed=0;
	m_nEnableBasicDumpTime=0;
	m_nInterfaceExitEntryNum=0;
	m_nTgOnce=0;
	m_nMSSAlarm=0;
	m_nMSSLayoutAlarm=0;
	m_RouteListFontsize = 0;
	m_AlarmFromSVR = 0;
	m_RouteFromSVR = 0;
	m_nNosynTimeoutTime=5*60; // 5分钟
	m_HilcMode = 0 ;
	m_nRunFastTimeCompareWithBasic = 0;

	for(int i=0;i<sizeof(m_nViewSchdTG)/sizeof(m_nViewSchdTG[0]);i++)
		m_nViewSchdTG[i]=1;

	m_bmpbitdeep = 16;
	m_nNeedSynDataToGpc=0;
	// 默认列控只读权限
	m_nTCCReadOnly = 0;
	m_nTCCExecute = 0;  // 0: 不能执行   1: 可以执行

	m_cmdRecvUnitSytle = 0;
	m_cmdSendByName    = 1;
	m_usCadOsSystemFlag = 0;
	m_bSendedLoginOk = FALSE;

	nUserIndex = 0;         
 
	nSPStationNum = 0;
	bGpcTrain = FALSE;
	m_bPasswordAndSetupChanged = FALSE;
    m_nTGLineType = 2;
 
	plotiteminfo.clear();
    sectionplotinfo.clear();
	m_vctSectionPhaseStation.clear();
	memset(spstationinfo,0,sizeof(spstationinfo));
	nEleModuleNum = 0;
 
	nGroupCnt = 0;   
	nEntityCnt = 0;   
	m_vctPlanReceicer.clear();
	memset(&pbinfo, 0x0, sizeof(pbinfo));
	memset(&pbinforoute, 0x0, sizeof(pbinforoute));
	memset(&logSvrInfo, 0x0, sizeof(logSvrInfo));
 
	memset(&dcSaveinfo,0,sizeof(DispatchCommandSave));
	
	m_printFormat = 0;
	m_TrainIDFormat=2;
	m_nCurrentSectNo=0;

	m_dcAuthorityNum = 0;
	m_nNeedAuthorityDCNum =0;

    m_rectLayoutPos.left = m_rectLayoutPos.top = m_rectLayoutPos.right = m_rectLayoutPos.bottom = 0;

	routeviewchangeinfo.bChangeBySectionSel=0;
	routeviewchangeinfo.bChangeByStationSel=0;
	routeviewchangeinfo.nSectionRouteViewPos=0;
	routeviewchangeinfo.nStationRouteViewPos=1;
	
	m_bTccCmdIDRequireServerConfirm = false;
	m_szTccVersionString = "C2";

	m_mapSwitchStation.clear();
	m_mapNotShowSideCtcStation.clear();
	m_mapMSSvrTgEntity.clear();

	m_bRouteListExecuteForceEnable = true;

	// 从...\xxx\cadsys.exe 变为
	//   ...\  主目录
	GetModuleFileName(NULL,m_szGPCWorkPath,256);

	char *pdest;
	pdest = strrchr( m_szGPCWorkPath, '\\' );	// 去掉"\\cadsys.exe"
	*pdest = 0;

	pdest = strrchr( m_szGPCWorkPath, '\\' );	// 去掉上一级目录xxx
	*pdest = 0;

	m_nWidePixel=0;
	m_bUseVirtualMemory=false;
	m_bModifyTsrStatus=false;
	m_bAutoMoveTime=false;
	m_nHistoryHour=12;
	m_bEnableAssistModifyPropTree=false;

	m_nEntryBySideSectionCount=0;
	memset(&mEntryBySideSection, 0, sizeof(mEntryBySideSection));
	

	memset(m_szAlarmFilterPassword, 0xFE, sizeof(m_szAlarmFilterPassword));
	m_nAlarmFilterBySta=0;
	m_bDragWindowEnable=m_bDragWindowShow=FALSE;
	m_nRouteTrigLast=0;
	m_bTgMsgToGateway=m_bOtherMsgToGateway=false;
	m_nTrainNoLevelCount=3;
	m_basicPlanReciever.clear();
}

CGpcdef::~CGpcdef()
{
	if(m_bPasswordAndSetupChanged == TRUE)
		SaveSetup();

	//释放打印编组信息列表
	plotiteminfo.clear();
    sectionplotinfo.clear();

    int i;
	
	for(i=0;i<nSPStationNum;i++)
        free(spstationinfo[i]);
	 
	for(i=0;i < nEleModuleNum;i++)
		free(elemodule[i]);
	m_vctSectionPhaseStation.clear();
}

int CGpcdef::LoadProfile(CString entitycfg)
{
	char szFileName[128];
	LoadLocoConfigFile();
	
	sprintf(szFileName,"%s\\%s",GetDataDir(),GPCFILENAME);
	if(!LoadPublicProfile(szFileName, entitycfg.GetBuffer()))//commen data
		return 0;
	
	sprintf(szFileName,"%s\\%s",GetDataDir(),GPCNOFILENAME);
	
	if(!LoadSingleProfile(szFileName)) //selfrelated data
		return 0;
	
	// complete user info
	return 1;
}

int CGpcdef::WintgTermLoadProfile(CString entitycfg)
{
	char szFileName[128];
	LoadLocoConfigFile();
	
	sprintf(szFileName,"%s\\%s",GetDataDir(),GPCFILENAME);
	if(!LoadPublicProfile(szFileName, entitycfg.GetBuffer()))//commen data
	{
		AfxMessageBox("加载用户配置文件(gpc.def)失败, 系统退出",MB_OK);
		return 0;
	}
	
	sprintf(szFileName,"%s\\%s",GetDataDir(),GPCNOFILENAME);
	
	if(!LoadSingleProfile(szFileName)) //selfrelated data
	{
		AfxMessageBox("加载用户配置文件(gpcno.def)失败, 系统退出",MB_OK);
		return 0;
	}

	sprintf(szFileName,"%s\\TG.def",GetDataDir());
	if(!LoadTGDefProfile(szFileName))
	{
		AfxMessageBox("加载用户配置文件(tg.def)失败, 系统退出",MB_OK);
	}

	LoadEntryBySide();
	LoadMmproUdpEntity();
	
	return 1;
}

int CGpcdef::GetSectionSelfStdFlag(int nSection)
{
	GPCSECTION *tmp = sectinfo.getContentFromID( nSection );
	if(NULL == tmp)
		return NULL;

	return tmp->nSelfStdFlag;

}

int CGpcdef::GetSectionNextStdFlag(int nSection)
{
	GPCSECTION *tmp = sectinfo.getContentFromID( nSection );
	if(NULL == tmp)
		return NULL;

	return tmp->nNextStdFlag;

}

int CGpcdef::LoadJDInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t=\n"; 
    
	token = strtok(ptr, seps);
	if(token==NULL)
		strcpy(m_JDServerAdd,"\\\\172.26.75.211\\jidiao\\loco\\export\\");
	else
		strcpy(m_JDServerAdd,token);

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		strcpy(m_JDServerUser,"Administrator");
	}
	else
	{
		strcpy(m_JDServerUser,token);
	}
	

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		m_JDServerPWD[0] = '\0';
	}
	else
	{
		strcpy(m_JDServerPWD,token);
	}
  
	return 1;

}

BOOL CGpcdef::SetSectionNextStdFlag(int nSection, int flag)
{
	GPCSECTION *tmp = sectinfo.getContentFromID( nSection );
	if(NULL == tmp)
		return FALSE;

	tmp->nNextStdFlag = flag;
	return TRUE;
}

char* CGpcdef::GetJDServerAddr(char *servAddr)
{
	if(servAddr != NULL)
		strcpy(servAddr,m_JDServerAdd);
	return m_JDServerAdd;
}

char* CGpcdef::GetJDServerUser(char *user)
{
	if(user != NULL)
		strcpy(user,m_JDServerUser);
	return m_JDServerUser;
}

char* CGpcdef::GetJDServerPWD(char *pwd)
{
	if(pwd != NULL)
		strcpy(pwd,m_JDServerPWD);
	return m_JDServerPWD;
}
/*
关闭程序，时候提示保存图片，第一班登陆，不管，第二班登陆，如果时间在18:30-20:00退出，而且
图片在本地不存在，那么提示保存。如果在20：00－21：00之间还没有保存，那么每隔5分钟提示一次，
强调9点以后将无法保存。图片在本地保存二个月，删除二个月以前的图片，
*/
//检查在本地是否有保存了当前登陆班的图片
BOOL CGpcdef::CheckPicExist(int secNo)
{
/*
if(GetLoginShiftid()%2 != 0)//第一班登陆，不管
		return TRUE;
*/
	CString filePath = GetPicFilePath(secNo);
	
	if((_access((char*)(LPCTSTR)filePath,0)) != -1)
	{
		//判断保存时间是否在18点以前
		struct _stat statDest;
		if(_stat((char*)(LPCTSTR)filePath,&statDest) != 0) //file doesn't exist
		{
			return FALSE;
		}
		else
		{
			struct tm *mtm = NULL;
			mtm = localtime(&statDest.st_mtime);
			if(mtm == NULL || mtm->tm_hour<18)
				return FALSE;
			
			return TRUE;

		}

		return TRUE;
	}		

	return FALSE;
}

//获得要保存的文件的文件名全路径
CString CGpcdef::GetPicFilePath(int secNo)
{
	CString filePath;	
	filePath = GetDataDir();
	if(filePath.Right(1) == "\\")
		filePath += "Pictures\\";
	else
		filePath += "\\Pictures\\";
	
	CString picName;
	picName.Format("%d%02d%s.Jpg",GetLoginShiftid(),secNo,GetSectionName(secNo));
	
	filePath += picName;
	
	return filePath;

}
int CGpcdef::GetSectionPhaseStorePrintRate(int secNo, int &printNum, float *prinRate)
{
	for(int i=0; i<m_vctSectionPhaseStation.size(); i++)
	{
		if(m_vctSectionPhaseStation[i].nSectionNo == secNo)
		{
			printNum = m_vctSectionPhaseStation[i].nStationNum;
			for(int k=0; k<printNum; k++)
				prinRate[k] = m_vctSectionPhaseStation[i].nStationPrintRate[k];
			return printNum;
		}
	}

	return 0;
}

int CGpcdef::GetPicturesTip()
{
	return m_nPicturesTip;
	
}

//车站的接入,交出口信息
int CGpcdef::LoadStationEntryInfo(char *ptr)   
{
 	char *token;
	char seps[]=" ,\t\\/=\n";
 
	token = strtok(ptr, seps);
	if(token==NULL) return 0;

	GPCSTATIONENTRY stationEntryInfo;
	
 
	//车站号
	stationEntryInfo.nStationNo=(int)atoi(token);      

    // load entrystation direction count;
	token = strtok(NULL, seps);
	if(token==NULL) 
		return 0;

	//方向个数 
	stationEntryInfo.nDirectionCount = (int)atoi(token);     
	
    // load every direction  list
	for(int i=0; i<stationEntryInfo.nDirectionCount&&i<20; i++)
	{
		token = strtok(NULL, seps);
		if(token==NULL) 
			return 0;
		//方向名称
		strncpy(stationEntryInfo.directionentryinfo[i].sDirectionName, token, 50); 
		stationEntryInfo.directionentryinfo[i].sDirectionName[49]=0;
		
		//入口数目
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
		stationEntryInfo.directionentryinfo[i].nInNum = (int)atoi(token);                            //入口数目
		//入口列表
		for(int j=0; j<stationEntryInfo.directionentryinfo[i].nInNum; j++)    
		{
			token = strtok(NULL, seps);
			if(token!=NULL)
				stationEntryInfo.directionentryinfo[i].nInList[j] = (int)atoi(token);                    //接入口的列表
			else 
				return 0;
		}

		//出口数目
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
		stationEntryInfo.directionentryinfo[i].nOutNum = (int)atoi(token);                            //入口数目
		//出口列表
		for(int j=0; j<stationEntryInfo.directionentryinfo[i].nOutNum; j++)    
		{
			token = strtok(NULL, seps);
			if(token!=NULL)
				stationEntryInfo.directionentryinfo[i].nOutList[j] = (int)atoi(token);                    //接入口的列表
		    else 
				return 0;
		}
	}

	m_vctStationEntryInfo.push_back(stationEntryInfo);
	
    return 1;
}
int CGpcdef::LoadInterfaceStationInfo(char *ptr)   //交接口车站的信息
{
	char *token;
	char seps[]=" ,\t\\/=\n";
 
	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;
	
	GPCINTERFACESTATION IFStation;
	
	//区段号
	IFStation.nSectionNo=(int)atoi(token);    
    	
	//车站数目
	token = strtok(NULL, seps);
	if(token==NULL) 
		return 0;
	int nStationNum = (int)atoi(token);

	//车站的列表
	IFStation.nStationNum=0;
	for(int i=0; i<nStationNum; i++)
	{
		token = strtok(NULL, seps);
		if(NULL != token)
		{
			IFStation.nStationList[IFStation.nStationNum] = (int)atoi(token);    
			IFStation.nStationNum++;
		}
	}

	m_vctIfStation.push_back(IFStation);
    return 1;  
}

int CGpcdef::LoadSectionPlotInfo(char * ptr)  //区段打印的编组信息
{
	char *token;
	char seps[]=" ,\t\\/=\n";
 
	GPCSECTIONPLOTITEM plotitem;

	//区段号
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	plotitem.nSectionNo=(int)atoi(token);    
	
	//栏目数目
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	int nPlotItemNum = (int)atoi(token);

	//栏目列表 
	plotitem.nSectionPlotItemNum=0;
	for(int i=0; i<nPlotItemNum&&i<10; i++)
	{
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
		
		plotitem.nSectionPlotItemList[plotitem.nSectionPlotItemNum]=(int)atoi(token);
		
		plotitem.nSectionPlotItemNum++;
	}
		
	sectionplotinfo.push_back(plotitem);
    
	return 1;
}

int CGpcdef::LoadErrorSideStationInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;

	GPCERRORSIDESTATION ErrorStation;

	//区段号
	ErrorStation.nSectionNo=(int)atoi(token);    

	//车站数目
	token = strtok(NULL, seps);
	if(token==NULL) 
		return 0;
	int nStationNum = (int)atoi(token);

	//车站的列表
	ErrorStation.nStationNum=0;
	for(int i=0; i<nStationNum&&i<64; i++)
	{
		token = strtok(NULL, seps);
		if(NULL != token)
		{
			ErrorStation.nStationList[ErrorStation.nStationNum] = (int)atoi(token);    
			ErrorStation.nStationNum++;
		}
	}

	m_vctErrorSideStation.push_back(ErrorStation);
	return 1;  
}

int CGpcdef::LoadNotThisSectionStationInfo(char *ptr) //设置非本台车站
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;

	GPCERRORSIDESTATION stations;

	//区段号
	stations.nSectionNo=atoi(token);    
	if(stations.nSectionNo<=0)
		return 0;
	//车站数目
	token = strtok(NULL, seps);
	if(token==NULL) 
		return 0;
	int nStationNum = atoi(token);

	//车站的列表
	stations.nStationNum=0;
	for(int i=0; i<nStationNum&&i<64; i++)
	{
		token = strtok(NULL, seps);
		if(NULL != token)
		{
			stations.nStationList[stations.nStationNum] = atoi(token);    
			stations.nStationNum++;
		}
	}
	
	for(int i=0; i<m_vctNotThisSectionStation.size(); i++)
	{
		if(m_vctNotThisSectionStation[i].nSectionNo==stations.nSectionNo)
		{
			break;
		}
	}
	if(i<m_vctNotThisSectionStation.size())
	{
		CString str;
		str.Format("gpc.def中区段%d配置了多个[NOT_THIS_SECTION_STATION]!", stations.nSectionNo);
		AfxMessageBox(str);
		return 0;
	}
	if(stations.nStationNum>0)
		m_vctNotThisSectionStation.push_back(stations);
	return 1;  
}

int CGpcdef::LoadPlotItemInfo(char * ptr)  //打印的编组信息
{

	char *token;
	char seps[]=" ,\t\\/=\n";
 
	token = strtok(ptr, seps);
	if(token==NULL) return 0;

	GPCPLOTITEM item;
	item.nPlotItemNum=(int)atoi(token);          //打印栏目编号
    
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	memset(item.sPlotItemName, 0, sizeof(item.sPlotItemName));
	strncpy(item.sPlotItemName, token, sizeof(item.sPlotItemName)-1);        //打印栏目名称

	plotiteminfo.push_back(item);

    return 1;
}


int CGpcdef::LoadUserInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	GPCUSER  tmp;
	
	// load console name
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	strcpy( tmp.ConsoleName, token );     //调度台名

	// load gpc id 
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	tmp.nGpcId = (int)atoi(token);      //gpcid
	
	// load user name
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strcpy( tmp.UserName, token );        //username
	
	// load user password
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strcpy( tmp.UserRealName, token );    //userRealname
	
	// load user identify
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	tmp.bIdentify = (int)(tolower(*token)-'a'+1);
	// assign user priority
	tmp.bPriority = TaskDefs[ tmp.bIdentify ];  //用户权限
	
	// load sections list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	int nSectionNum = (int)atoi(token);             //区段数目
	for(int i=0; i<nSectionNum;)
	{
		token = strtok(NULL, seps);
		if(token==NULL) return 0;

		//区段列表
		if(strchr(token, '-')) 
		{
			int nStart=0, nEnd=0;
			sscanf(token, "%d-%d", &nStart, &nEnd);
			for(int j=nStart; j<=nEnd; j++)
				tmp.nSectionList.push_back(j);  
			i += nEnd - nStart + 1;
		} 
		else 
		{
			tmp.nSectionList.push_back(atoi(token));
			i++;
		}
	}
	
	// load layout section's list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	int num = atoi(token);
	for(i=0; i<num; )
	{
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
		
		if(strchr(token, '-')) {
			int nStart=0, nEnd=0;
			sscanf(token, "%d-%d", &nStart, &nEnd);
			for(int j=nStart; j<=nEnd; j++)
				tmp.layout_section_list.push_back(j);
			i += nEnd - nStart + 1;
		} else {
			tmp.layout_section_list.push_back(atoi(token));
			i++;
		}
	}
	
 
	//登陆方式2005-03-21
	if(token = strtok(NULL, seps))
	{
		tmp.nLoginType = (int)atoi(token);
		if(tmp.nLoginType > 4 || tmp.nLoginType < 0)
			tmp.nLoginType = 0;
		//tmp.nCurDispathNo = (int)atoi(token);       //调度命令号
		//tmp.nOldDispathNo = (int)atoi(token);
	}
	else return 0;

	//确认gpc号码
	if(token = strtok(NULL, seps))
	{
		tmp.nAckGpcNo = (int)atoi(token);
	 
	}
	else return 0;
	
	tmp.nTDGpcNo = 0;
	if(token = strtok(NULL, seps))
	{
		tmp.nTDGpcNo = (int)atoi(token);
		if(tmp.nTDGpcNo!=0) // 0是无效值
		{
			if(tmp.nTDGpcNo < 0 || tmp.nTDGpcNo > 64)
			{
				CString str;
				str.Format("[USER]=%s, gpc%d 没有正确配置 列车索引生成号(配置值0<=X<=64,其中0表示配置无效)",   tmp.ConsoleName,  tmp.nGpcId);
				AfxMessageBox(str, MB_ICONWARNING);
				return 0;
			}
		}
   	}
    
	userinfo.saveContent( tmp );
	// success return
    return 1;
}

int CGpcdef::LoadViewSchdTG(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	for(int i=0;i<sizeof(m_nViewSchdTG)/sizeof(m_nViewSchdTG[0]);i++)
		m_nViewSchdTG[i]=1;

	// 日班计划图
	token = strtok(ptr, seps);
	if(token!=NULL) 
		m_nViewSchdTG[SCHD_TYPE::SHIFT] = atoi(token);

	// 施工符号
	token = strtok(NULL, seps);
	if(token!=NULL) 
		m_nViewSchdTG[SCHD_TYPE::MMPLAN] = atoi(token);

	// 基本图
	token = strtok(NULL, seps);
	if(token!=NULL) 
		m_nViewSchdTG[SCHD_TYPE::BASIC] = atoi(token);

	// 邻台图
	token = strtok(NULL, seps);
	if(token!=NULL) 
		m_nViewSchdTG[SCHD_TYPE::NEIGHBOUR] = atoi(token);

	// 历史图
	token = strtok(NULL, seps);
	if(token!=NULL) 
		m_nViewSchdTG[SCHD_TYPE::HISTORY] = atoi(token);

	// 列控命令管理
	token = strtok(NULL, seps);
	if(token!=NULL) 
		m_nViewSchdTG[SCHD_TYPE::TCC_MNG] = atoi(token);

	// 电力臂管理
	token = strtok(NULL, seps);
	if(token!=NULL) 
		m_nViewSchdTG[SCHD_TYPE::ELE_ARMS] = atoi(token);

	// 归档基本图
	token = strtok(NULL, seps);
	if(token!=NULL) 
		m_nViewSchdTG[SCHD_TYPE::BACK_UP_BASIC] = atoi(token);
	return 1;
}

int CGpcdef::LoadDefaultInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	token = strtok(ptr, seps);
	if(token==NULL) return 0;

	strcpy(szDefaultUser, token);
	 
	return 1;
}

int CGpcdef::LoadLayoutPos(char *ptr)
{
    char *token;
    char seps[]=" ,\t\\/=\n";

    token = strtok(ptr,seps);
    if( NULL==token ) return 0;
    m_rectLayoutPos.left = atol(token);

    for( int i=1; i<4; i++ )    // 读取后三个参数
    {
        token = strtok(NULL,seps);
        if( NULL==token ) return 0;

        switch(i)
        {
        case 1:
            m_rectLayoutPos.top = atol(token);
        	break;
        case 2:
            m_rectLayoutPos.right = m_rectLayoutPos.left + atol(token);
        	break;
        case 3:
            m_rectLayoutPos.bottom = m_rectLayoutPos.top + atol(token);
            break;
        default:
            break;
        }
    }

    return 1;
}

int CGpcdef::LoadLayoutPosForSingleStation(char *ptr)
{
    char *token;
    char seps[]=" ,\t\\/=\n";

    token = strtok(ptr,seps);
    if( NULL==token ) return 0;
    m_rectLayoutPosForSingleStation.left = atol(token);

    for( int i=1; i<4; i++ )    // 读取后三个参数
    {
        token = strtok(NULL,seps);
        if( NULL==token ) return 0;

        switch(i)
        {
        case 1:
            m_rectLayoutPosForSingleStation.top = atol(token);
        	break;
        case 2:
            m_rectLayoutPosForSingleStation.right = m_rectLayoutPosForSingleStation.left + atol(token);
        	break;
        case 3:
            m_rectLayoutPosForSingleStation.bottom = m_rectLayoutPosForSingleStation.top + atol(token);
            break;
        default:
            break;
        }
    }

    return 1;
}

CSize CGpcdef::GetPageSize(int iPageID)
{
	CSize size;
	size.cx = 0;
	size.cy = 0;

	if( iPageID == 0 )
	{
		size.cx = stainfo[0].sizeDisp.cx;
		size.cy = stainfo[0].sizeDisp.cy;
		return size;
	}

	//GPCSTATION *tmp = stainfo_wsg.getContentFromID(iPageID);
	//ASSERT(NULL != tmp);
	//sizePage->cx = tmp->sizePage.cx;
	//sizePage->cy = tmp->sizePage.cy;
	for(int i=0; i<stainfo.size(); i++)
	{
		if(stainfo[i].nStation == iPageID)
		{
			size = stainfo[i].sizeDisp;
			break;
		}
	}
	return size;
}

int CGpcdef::LoadSectInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j, nStart, nEnd;

	GPCSECTION  tmp;
	memset(&tmp, 0, sizeof(GPCSECTION));

	// load section number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	tmp.nSection = (int)atoi(token);
	
	// load section name
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strcpy(tmp.szSectionName, token);
	
	// load station list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	tmp.nStationNum = (int)atoi(token);
	for(i=0; i<tmp.nStationNum;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			if(strchr(token, '-')) {
				sscanf(token, "%d-%d", &nStart, &nEnd);
				for(j=nStart; j<=nEnd; j++)
				tmp.nStationList[i++] = j;
			} else {
 				tmp.nStationList[i] = (int)atoi(token);
				i++;
			}
	}

	//load schedule on doubleclick about self station & next station 
	token = strtok(NULL, seps);
	if(token==NULL)
		tmp.nSelfStdFlag = 0;
	else
		tmp.nSelfStdFlag = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL)
		tmp.nNextStdFlag = 0;//(int)atoi(token);
	else
		tmp.nNextStdFlag = (int)atoi(token);
	//load plan receiver list
	token = strtok(NULL,seps);
	if(token == NULL) return 0;

	//下发阶段计划车站的列表
	tmp.nPlanReceiverCount = (int)atoi(token);
	for(i=0;i<tmp.nPlanReceiverCount;)
	{
		token = strtok(NULL,seps);
		if(token == NULL) return 0;
		if(strchr(token,'-'))
		{
			sscanf(token, "%d-%d", &nStart,&nEnd);
			for(j=nStart;j<=nEnd;j++)
				tmp.nPlanReceiverList[i++] = j;
		}
		else
		{
			tmp.nPlanReceiverList[i] = (int)atoi(token);
			i++;
		}
	}

	sectinfo.saveContent( tmp );
	// scuccess return
	return 1;
}

int CGpcdef::LoadSectRouteInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j,k, nStart, nEnd, nSection;
	
	// load section number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	nSection = (int)atoi(token);

	GPCSECTION *tmp = sectinfo.getContentFromID( nSection );
	if(NULL == tmp)
		return 0;

 	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	tmp->nRouteCount = (int)atoi(token);
 
	for(k = 0;k< tmp->nRouteCount;k++){
 		// load station list
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
		tmp->nRouteStationCount[k] = (int)atoi(token);
		for(i=0; i<tmp->nRouteStationCount[k];) {
			token = strtok(NULL, seps);
			if(token==NULL) return 0;
				if(strchr(token, '-')) {
					sscanf(token, "%d-%d", &nStart, &nEnd);
					for(j=nStart; j<=nEnd; j++)
					tmp->nRouteStationList[k][i] = j;
				} else {
 					tmp->nRouteStationList[k][i] = (int)atoi(token);
					i++;
				}
		}
	}

 	return 1;
}

int CGpcdef::LoadStationDeltaInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j,k, nStart, nEnd, nSection,nRouteCount,nRouteStationCount;
	
	// load section number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	nSection = (int)atoi(token);

	GPCSECTION *tmp = sectinfo.getContentFromID( nSection );
	if( NULL == tmp )
		return 0;

 	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	nRouteCount = (int)atoi(token);
 
	for(k = 0; k<tmp->nRouteCount; k++){
 		// load station list
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
		nRouteStationCount = (int)atoi(token);
		for(i=0; i<nRouteStationCount;) {
			token = strtok(NULL, seps);
			if(token==NULL) return 0;
				if(strchr(token, '-')) {
					sscanf(token, "%d-%d", &nStart, &nEnd);
					for(j=nStart; j<=nEnd; j++)
					tmp->nRouteStationDelta[k][i] = j;
				} else {
 					tmp->nRouteStationDelta[k][i] = (int)atoi(token);
					i++;
				}
		}
	}

 	return 1;
}

int CGpcdef::LoadDispSectInfo(char *ptr)
{

	char *token;
	char seps[]=" ,\t\\/=\n*";
	GPCDISPSECTINFO temp;
	int i,j,nStart, nEnd;                                                                                           

	// load DISPLAY SECTION ID
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	temp.nSectionNo = (int)atoi(token);
	// load DISPLAY SECTION name
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strcpy(temp.lpszSectionName,token);

	// 区段原始宽度与高度
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	temp.nOriginalSize.cx = atoi(token);
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	temp.nOriginalSize.cy = atoi(token);

	// 区段显示宽度与高度
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	temp.nDispSize.cx = atoi(token);
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	temp.nDispSize.cy = atoi(token);

	// load station list
	token = strtok(NULL, seps);                                                                                     
	if(token==NULL) return 0;                                                                                       
	
	int num = (int)atoi(token);                                                                           
	for(i=0; i<num;) {                                                                        
		token = strtok(NULL, seps);                                                                             
		if(token==NULL) return 0;                                                                               
		
		if(strchr(token, '-'))
		{
			sscanf(token, "%d-%d", &nStart, &nEnd);
			if(nStart <= nEnd)
			{
				for(j=nStart; j<=nEnd; j++)
				{
					temp.stationlist.push_back(j);
					i++;
				}
			}
			else
			{
				for(j=nStart; j>=nEnd; j--)
				{
					temp.stationlist.push_back(j);
					i++;
				}
			}
		}
		else
		{
			temp.stationlist.push_back( (int)atoi(token) );
			i++;
		}
	}                                                                                                               
	

	dispsectinfo.saveContent( temp );

	return 1;
}

//读取站场图管辖区段配置
int CGpcdef::LoadDispSectDominInfo(char *ptr)
{
char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j, nStart, nEnd;

	GPCSECTION  tmp;
	memset(&tmp, 0, sizeof(GPCSECTION));

	// load section number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	tmp.nSection = (int)atoi(token);
	
	// load section name
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strcpy(tmp.szSectionName, token);
	
	// load station list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	tmp.nStationNum = (int)atoi(token);
	for(i=0; i<tmp.nStationNum;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			if(strchr(token, '-')) {
				sscanf(token, "%d-%d", &nStart, &nEnd);
				for(j=nStart; j<=nEnd; j++)
				tmp.nStationList[i++] = j;
			} else {
 				tmp.nStationList[i] = (int)atoi(token);
				i++;
			}
	}

	//load schedule on doubleclick about self station & next station 
	token = strtok(NULL, seps);
	if(token==NULL)
		tmp.nSelfStdFlag = 0;
	else
		tmp.nSelfStdFlag = (int)atoi(token);
	
	token = strtok(NULL, seps);
	if(token==NULL)
		tmp.nNextStdFlag = 0;//(int)atoi(token);
	else
		tmp.nNextStdFlag = (int)atoi(token);
	//load plan receiver list
	token = strtok(NULL,seps);
	if(token == NULL) return 0;
	
	//下发阶段计划车站的列表
	tmp.nPlanReceiverCount = (int)atoi(token);
	for(i=0;i<tmp.nPlanReceiverCount;)
	{
		token = strtok(NULL,seps);
		if(token == NULL) return 0;
		if(strchr(token,'-'))
		{
			sscanf(token, "%d-%d", &nStart,&nEnd);
			for(j=nStart;j<=nEnd;j++)
				tmp.nPlanReceiverList[i++] = j;
		}
		else
		{
			tmp.nPlanReceiverList[i] = (int)atoi(token);
			i++;
		}
	}
	                                                                                                           
	

	dispdominsectinfo.saveContent( tmp );

	return 1;
}

void CGpcdef::GetSectionSizeOfDisp( SIZE* originalSize, SIZE* dispSize, int nSectionID )
{
	GPCDISPSECTINFO *tmp = dispsectinfo.getContentFromID( nSectionID );
	ASSERT( NULL != tmp );

	*originalSize = tmp->nOriginalSize;
	*dispSize = tmp->nDispSize;
}

int CGpcdef::LoadRouteInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int nStart, nEnd;


	GPCROUTE tmp;

	// load route number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	tmp.nRouteNo = (int)atoi(token);
 
	// load routeset number
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	tmp.nRouteSetNo = (int)atoi(token);

	// load station list
	token = strtok(NULL, seps);
	if(token==NULL)	return 0;

	tmp.nStationNum = (int)atoi(token);
	for(int i=0; i<tmp.nStationNum;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			if(strchr(token, '-')) {
				sscanf(token, "%d-%d", &nStart, &nEnd);
				if(nStart < nEnd)
					for(int j=nStart; j<=nEnd; j++)
						tmp.nStationList[i++] = j;
				else
					for(int j=nStart; j>=nEnd; j--)
						tmp.nStationList[i++] = j;
			} else {
				tmp.nStationList[i] = (int)atoi(token);
				i++;
			}
	}

	// load route direction
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	tmp.nRouteDirect = (int)atoi(token);
	tmp.spStation.nPrevDirect = tmp.nRouteDirect;
	tmp.spStation.nNextDirect = tmp.nRouteDirect;

	routeinfo_wsg.saveContent( tmp );
	// success return 
	return 1;
}



int CGpcdef::LoadStationSpaceInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int nRoute;

	// load route number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	nRoute = (int)atoi(token);
	GPCROUTE *tmp = routeinfo_wsg.getContentFromID(nRoute);
	if( NULL == tmp )
		return 0;

 	token = strtok(NULL, seps);
	if(token==NULL) return 0;
 
 	token = strtok(NULL, seps);
	if(token==NULL) return 0;

 	for(int i=0; i<tmp->nStationNum;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
 		tmp->nStationSpaceList[i] = (int)atoi(token);
		i++;
	}
	return 1;

/*
	// load route number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	nRoute = (int)atoi(token);
 
 	token = strtok(NULL, seps);
	if(token==NULL) return 0;
 
 	token = strtok(NULL, seps);
	if(token==NULL) 
		return 0;
 	for(int i=0; i<routeinfo[nRoute]->nStationNum;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
 		routeinfo[nRoute]->nStationSpaceList[i] = (int)atoi(token);
		i++;
	}
	return 1;
*/
}

int CGpcdef::LoadSPStationInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
 
    spstationinfo[nSPStationNum] = (GPCSPSTATION*)malloc(sizeof(GPCSPSTATION));
 	if(spstationinfo[nSPStationNum] == NULL)
 		return 0;
    
    spstationinfo[nSPStationNum]->nId = nSPStationNum;

 	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	spstationinfo[nSPStationNum] ->nStation = (int)atoi(token);
  
	// load routeset number
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	spstationinfo[nSPStationNum]->nStationNum = (int)atoi(token);

	for(int i=0; i< spstationinfo[nSPStationNum] ->nStationNum;i++) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
	    spstationinfo[nSPStationNum]->nStationList[i] = (int)atoi(token);
	}

 	token = strtok(NULL, seps);
	if(token==NULL) return 0;
   	strcpy(spstationinfo[nSPStationNum]->sStationName, token);


    nSPStationNum ++;
	return nSPStationNum;
}

int CGpcdef::FindUserName(const char *username)
{   
	if(NULL == username)
		return (int)GPC_USERNAME_ERROR;
	for(int i=0; i<userinfo.size(); i++)
	{
		if(!strcmp(username, userinfo[i].UserName))
			break;
	}
	if( i >= userinfo.size() )
		return (int)GPC_USERNAME_ERROR;
	else
		return i;
}

int CGpcdef::SetDefaultUser(const char *gpcname)
{
	int idx;
	if(gpcname)
		strcpy(szDefaultUser,gpcname);
    idx = FindUserName(szDefaultUser);
	if(idx != GPC_USERNAME_ERROR) 
		nUserIndex = idx;
	else
	{
		CString tmp;
		tmp.Format("gpc.def中没有定义[USER] %s", gpcname);
		AfxMessageBox(tmp);
	}
	return idx;
}

int CGpcdef::GetScreenNum()
{
	return m_nScreenNum;
}

CRect CGpcdef::GetLayoutPos()
{
    return m_rectLayoutPos;
}

CRect CGpcdef::GetLayoutPosForSingleStation()
{
    return m_rectLayoutPosForSingleStation;
}

char *CGpcdef::GetCurUserName()
{
	// current user name
	return userinfo[nUserIndex].UserName;	
}

char *CGpcdef::GetCurUserRealName()
{
	// current user name
	return userinfo[nUserIndex].UserRealName;	
}
 
int CGpcdef:: SetCurrentUserid(const char * useridx)
{
	strncpy(userinfo[nUserIndex].guserid, useridx, sizeof(userinfo[nUserIndex].guserid));
	return TRUE;
}

int CGpcdef::SetCurrentUsername(const char * username)
{
	strncpy(userinfo[nUserIndex].gCurrentUserName, username, sizeof(userinfo[nUserIndex].gCurrentUserName));
	return TRUE;
}

int  CGpcdef::SetCurrentUserpwd(const char* pwd)
{
	strncpy(userinfo[nUserIndex].Password,pwd,sizeof(userinfo[nUserIndex].Password));
	return TRUE;
}

char *CGpcdef:: GetCurrentUserid()  
{
	return  userinfo[nUserIndex].guserid;
}

char *CGpcdef::GetCurrentUsername()  
{
	return  userinfo[nUserIndex].gCurrentUserName;
}

char *CGpcdef::GetCurrentUserpwd()  
{
	return  userinfo[nUserIndex].Password;
}

unsigned long CGpcdef::GetLoginDate() //获取登陆日期
{
    return  userinfo[nUserIndex].gLoginDate;
}

int CGpcdef::GetLoginShiftid()      //获取登陆班别
{
    return  userinfo[nUserIndex].gLoginShiftid;
}

void CGpcdef::SetLoginDate(unsigned long itime) // 设置登陆日期
{
    userinfo[nUserIndex].gLoginDate=itime;
}
void CGpcdef::SetLoginShiftid(int inumber)     //设置登陆班别
{
    userinfo[nUserIndex].gLoginShiftid=inumber;
}

int CGpcdef::GetHistoryShiftid()    
{
    return userinfo[nUserIndex].nHistoryShiftid;

}
void CGpcdef::SetHistoryShiftid(int inumber)     
{
    userinfo[nUserIndex].nHistoryShiftid=inumber;
}

char* CGpcdef::GetPlotItemName(int inumber)       //获取打印栏目名称
{   
	for(int i=0; i<plotiteminfo.size(); i++)
	{
		if(plotiteminfo[i].nPlotItemNum == inumber)
			return plotiteminfo[i].sPlotItemName;
	}
	
	return "未知";
}

int CGpcdef::GetSectionPlotItemList(int sectionNum, int* sectionplotitemList, int max)
{
	int i=0;
	for(i=0; i< sectionplotinfo.size(); i++)
	{
		if(sectionplotinfo[i].nSectionNo==sectionNum)
		{
			int num = sectionplotinfo[i].nSectionPlotItemNum;
			for(int j=0; j < num && j<max; j++)
				sectionplotitemList[j] = sectionplotinfo[i].nSectionPlotItemList[j];
			
			if(j < num)
				return max;
			else
				return num;
		}
	}
    
	return 0;
}

int CGpcdef::GetInterfaceStationList(int sectionNo,int *StationList, int maxSize)
{
	for(int i=0;i<m_vctIfStation.size();i++)
	{
		if(m_vctIfStation[i].nSectionNo==sectionNo)
		{
			for(int j=0; j<m_vctIfStation[i].nStationNum&&j<maxSize; j++ )
				StationList[j] =m_vctIfStation[i].nStationList[j];
			return m_vctIfStation[i].nStationNum;
		}
	}
	
	return 0;    
}

int CGpcdef::GetErrorSideStationList(int sectionNo,int *StationList)
{
	for(int i=0;i<m_vctErrorSideStation.size();i++)
	{
		if(m_vctErrorSideStation[i].nSectionNo==sectionNo)
		{
			for(int j=0; j<m_vctErrorSideStation[i].nStationNum; j++ )
				StationList[j] =m_vctErrorSideStation[i].nStationList[j];
			return m_vctErrorSideStation[i].nStationNum;
		}
	}

	return 0;    
}

int CGpcdef::GetEntryStationInListDirec(int stationNo,int DirectNo,int *InList)
{
	for(int i=0; i<m_vctStationEntryInfo.size(); i++)
	{
		if(m_vctStationEntryInfo[i].nStationNo==stationNo)
		{
			int nDirectNum = m_vctStationEntryInfo[i].nDirectionCount;
			if(DirectNo >= nDirectNum)
				return 0;
	
			int nInNum = m_vctStationEntryInfo[i].directionentryinfo[DirectNo].nInNum;  
			for(int j=0;j<nInNum;j++)
				InList[j] = m_vctStationEntryInfo[i].directionentryinfo[DirectNo].nInList[j];

			return nInNum;
		}
	}
	
	return 0;
}
int CGpcdef::GetEntryStationOutListDirec(int stationNo,int DirectNo,int *OutList)
{
	for(int i=0; i<m_vctStationEntryInfo.size(); i++)
	{
		if(m_vctStationEntryInfo[i].nStationNo==stationNo)
		{
			int nDirectNum = m_vctStationEntryInfo[i].nDirectionCount;
			if(DirectNo >= nDirectNum)
				return 0;
	
			int nOutNum = m_vctStationEntryInfo[i].directionentryinfo[DirectNo].nOutNum;  
			for(int j=0;j<nOutNum;j++)
				OutList[j] = m_vctStationEntryInfo[i].directionentryinfo[DirectNo].nOutList[j];

			return nOutNum;
		}
	}
	
	return 0;
}

int  CGpcdef:: GetEntryStationDirectNum(int stationNo)
{
	for(int i=0; i<m_vctStationEntryInfo.size();i++)
	{
		if(m_vctStationEntryInfo[i].nStationNo==stationNo)
		{
			int nDirectNum = m_vctStationEntryInfo[i].nDirectionCount;
			return nDirectNum;
		}
	}

	return 0;
}

char *CGpcdef::  GetEntryStationDirectName(int stationNo,int DirecNo)
{
	for(int i=0; i<m_vctStationEntryInfo.size();i++)
	{
		if(m_vctStationEntryInfo[i].nStationNo==stationNo)
		{
			int nDirectNum = m_vctStationEntryInfo[i].nDirectionCount;
			if(DirecNo<nDirectNum)
			{
				return m_vctStationEntryInfo[i].directionentryinfo[DirecNo].sDirectionName;
			}
		}
	}

    return NULL;
}
//******************************************************************

char *CGpcdef::GetUserName(int useridx)
{
	return  userinfo[useridx].UserName;
}

char *CGpcdef::GetDefaultUserName()
{
	return  userinfo[nUserIndex].UserName;
}

char *CGpcdef::GetUserPassword(char *username)
{
	if (NULL != username)
	{
		int idx = FindUserName(username);
		if(idx != GPC_USERNAME_ERROR)
			return userinfo[idx].Password;
	}
	else
	{
		return userinfo[nUserIndex].Password;
	}
	return NULL;
}

char *CGpcdef::SetUserPassword(char* password)
{
	m_bPasswordAndSetupChanged = TRUE;
    strcpy(userinfo[nUserIndex].Password, password);
	return NULL;
}

int CGpcdef::GetUserIdentify()
{
	return userinfo[nUserIndex].bIdentify;
}



void CGpcdef::SetUserIdentifyAndPriotiry(int nID)
{
	userinfo[nUserIndex].bIdentify = nID;
	userinfo[nUserIndex].bPriority = TaskDefs[nID]; 
}

// Added by wsg, Sep 15, 2002, Only for StdLayout
int CGpcdef::GetUserPriority(int tmp)
{
	return GetUserIdentify();
}

int CGpcdef::GetUserPriority()
{
	return userinfo[nUserIndex].bPriority;
}

int CGpcdef::GetUserGpcId()
{
	return userinfo[nUserIndex].nGpcId;
}

long CGpcdef::GetUserSetup()
{
	return userinfo[nUserIndex].bSetup;
}

int CGpcdef::SetUserSetup(long lSetup)
{
	m_bPasswordAndSetupChanged = TRUE;
    
	userinfo[nUserIndex].bSetup = lSetup;
	return TRUE;
}

int CGpcdef::SetUserIndex(int useridx)
{
	if(useridx<0 || useridx>=userinfo.size())
		return 0;
	nUserIndex = useridx;
	return 1;
}

int CGpcdef::GetLineSpeed( )
{
    return 0x100000;//10000000;
}

 

int CGpcdef::GetStationNum()
{
	return stainfo.size();
}

int CGpcdef::GetStationNum(int nSection)
{
	GPCSECTION *tmp = sectinfo.getContentFromID( nSection );
	if( NULL == tmp )
		return 0;

	return tmp->nStationNum;
}

int CGpcdef::GetStationList(int& stationNum, int* stationList)
{
	ASSERT( NULL != stationList );

	if(stationNum < userinfo[nUserIndex].stationlist.size())
		return 0;	// not enough buffer

	stationNum = userinfo[nUserIndex].stationlist.size();
	for(int i=0; i<stationNum; i++)
		stationList[i] = userinfo[nUserIndex].stationlist[i];
	
	return stationNum;
}

int CGpcdef::GetStationList(int nSection, int& stationNum, int* stationList)
{
	if( NULL == stationList )
		return 0;

	GPCSECTION *tmp = sectinfo.getContentFromID( nSection );
	if( NULL == tmp || stationNum < tmp->nStationNum )
		return 0;

	stationNum = tmp->nStationNum;
	for(int i=0; i<stationNum; i++)
		stationList[i] = tmp->nStationList[i];
	return stationNum;
}

int CGpcdef::GetStationListofDisp(int nSection, vector<int>& sta_list)
{
	sta_list.clear();

	GPCDISPSECTINFO *tmp = dispsectinfo.getContentFromID( nSection );
	if( NULL != tmp )
		sta_list = tmp->stationlist;

	return sta_list.size();
}

int CGpcdef::GetStationListofDispDomin(int nSection, vector<int>& sta_list)
{
	sta_list.clear();
	int stationNum=0;
	GPCSECTION *tmp = dispdominsectinfo.getContentFromID( nSection );	
	if( NULL == tmp)
		return 0;
	
	stationNum = tmp->nStationNum;

	for(int i=0; i<stationNum; i++)
		sta_list.push_back(tmp->nStationList[i]);
		//sta_list.insert(tmp->nStationList[i]);

	return stationNum;

}

int CGpcdef::GetSectionCount()
{
	return userinfo[nUserIndex].nSectionList.size();
}

int CGpcdef::GetSectionList(int& sectionNum, int* sectionList)
{
	ASSERT( NULL != sectionList );

	if(sectionNum < userinfo[nUserIndex].nSectionList.size())
		return 0;	// not enough buffer

	sectionNum = userinfo[nUserIndex].nSectionList.size();
	for(int i=0; i<sectionNum; i++)
		sectionList[i] = userinfo[nUserIndex].nSectionList[i];

 	return sectionNum;
}

////获得from to section 的出入口车站个数
int CGpcdef::GetNeighberReceiverStaList(int nFromSectionNo,int nToSectionNo,int& nNeighberStaNum, int* nNeighberStaList, int* nNeighberStaList_nei)
{
	int max_count = nNeighberStaNum;
	nNeighberStaNum = 0;

	if( NULL == nNeighberStaList || NULL == nNeighberStaList_nei)
		return 0;

	GPCSECTION *tmp = sectinfo.getContentFromID( nFromSectionNo );

	if( NULL== tmp )
		return 0;

	if( tmp->nNeighberSendDataNum < 1)
	{
		return 0;
	}

	for(int i=0; i<tmp->nNeighberSendDataNum; i++)
	{
		if(tmp->NeighberSendData[i].nSectionFrom == nFromSectionNo &&
			tmp->NeighberSendData[i].nSectionTo  == nToSectionNo)
		{
			nNeighberStaNum = tmp->NeighberSendData[i].nStationNum;
			 
			if(nNeighberStaNum > max_count)
			{
				return 0;
			}

			for(int j=0; j< nNeighberStaNum;j++)
			{ 
				nNeighberStaList[j] = tmp->NeighberSendData[i].Station[j].nStationNoMiddle;
				nNeighberStaList_nei[j] = tmp->NeighberSendData[i].Station[j].nStationNoMiddle_neighber;
			}

			break;
		}
	}

  	return nNeighberStaNum;
}

//获得指定台间站的出入口列表
int CGpcdef:: GetNeighberReceiverStaEntryList(int nFromSectionNo,int nToSectionNo, int nMiddleStaNo,int& nEntryNum, int* nEntryList)
{
	int max_count = nEntryNum;
	nEntryNum = 0;

	if( NULL == nEntryList )
		return 0;

	GPCSECTION *tmp = sectinfo.getContentFromID( nFromSectionNo );

	if( NULL== tmp )
		return 0;

	if( tmp->nNeighberSendDataNum < 1)
		return 0;

	int nEntryExitNum = 0;
	for(int i=0; i<tmp->nNeighberSendDataNum; i++)
	{
		if(tmp->NeighberSendData[i].nSectionFrom == nFromSectionNo  
			&& tmp->NeighberSendData[i].nSectionTo  == nToSectionNo)
		{
			int nNeighberStaNum = tmp->NeighberSendData[i].nStationNum;

			for(int j=0; j< nNeighberStaNum;j++)
			{
				if(tmp->NeighberSendData[i].Station[j].nStationNoMiddle == nMiddleStaNo)
				{
					nEntryExitNum = tmp->NeighberSendData[i].Station[j].nEntryExitNum;

					if( nEntryExitNum > max_count)
					{
						return 0;
					}

					for(int n=0;n<nEntryExitNum; n++ )
						nEntryList[n] = tmp->NeighberSendData[i].Station[j].nEntryExit_List[n];

					break;
				}
			}

			//founded, then quit 
			//if(nEntryExitNum >0 )
			break;
		}

	}

	nEntryNum =  nEntryExitNum;
	return nEntryExitNum;
}

BOOL CGpcdef::IfSendNeighberStartOrTerminateTrain(int nFromSectionNo,int nToSectionNo, int nMiddleStaNo)
{
	GPCSECTION *tmp = sectinfo.getContentFromID( nFromSectionNo );

	if( NULL == tmp )
		return false;

	for(int i=0; i<tmp->nNeighberSendDataNum; i++)
	{
		if(tmp->NeighberSendData[i].nSectionFrom == nFromSectionNo  
			&& tmp->NeighberSendData[i].nSectionTo  == nToSectionNo)
		{
			for(int j=0; j< tmp->NeighberSendData[i].nStationNum;j++)
			{
				if (tmp->NeighberSendData[i].Station[j].nStationNoMiddle == nMiddleStaNo)
					return tmp->NeighberSendData[i].Station[j].nSendStartOrTerminateTrain;
			}
		}
	}

	return false;
}

BOOL CGpcdef::IfAcceptNeighberStartOrTerminateTrainSide(int nFromSectionNo,int nToSectionNo, int nMiddleStaNo)
{
	GPCSECTION *tmp = sectinfo.getContentFromID( nFromSectionNo );

	if( NULL == tmp )
		return false;

	for(int i=0; i<tmp->nNeighberSendDataNum; i++)
	{
		if(tmp->NeighberSendData[i].nSectionFrom == nFromSectionNo  
			&& tmp->NeighberSendData[i].nSectionTo  == nToSectionNo)
		{
			for(int j=0; j< tmp->NeighberSendData[i].nStationNum;j++)
			{
				if (tmp->NeighberSendData[i].Station[j].nStationNoMiddle == nMiddleStaNo)
					return tmp->NeighberSendData[i].Station[j].nAcceptStartOrTerminateTrainSide;
			}
		}
	}

	return false;
}
BOOL CGpcdef::IfAcceptThroughTrainSide(int nFromSectionNo,int nToSectionNo, int nMiddleStaNo)
{
	GPCSECTION *tmp = sectinfo.getContentFromID( nFromSectionNo );

	if( NULL == tmp )
		return false;

	for(int i=0; i<tmp->nNeighberSendDataNum; i++)
	{
		if(tmp->NeighberSendData[i].nSectionFrom == nFromSectionNo  
			&& tmp->NeighberSendData[i].nSectionTo  == nToSectionNo)
		{
			for(int j=0; j< tmp->NeighberSendData[i].nStationNum;j++)
			{
				if (tmp->NeighberSendData[i].Station[j].nStationNoMiddle == nMiddleStaNo)
					return tmp->NeighberSendData[i].Station[j].nAcceptThroughTrainSide;
			}
		}
	}

	return false;
}

int CGpcdef::GetDefaultPassway(int nFromSectionNo,int nToSectionNo, int nMiddleStaNo)
{
	GPCSECTION *tmp = sectinfo.getContentFromID( nFromSectionNo );

	if( NULL == tmp )
		return 0;

	for(int i=0; i<tmp->nNeighberSendDataNum; i++)
	{
		if(tmp->NeighberSendData[i].nSectionFrom == nFromSectionNo  
			&& tmp->NeighberSendData[i].nSectionTo  == nToSectionNo)
		{
			for(int j=0; j< tmp->NeighberSendData[i].nStationNum;j++)
			{
				if (tmp->NeighberSendData[i].Station[j].nStationNoMiddle == nMiddleStaNo)
					return tmp->NeighberSendData[i].Station[j].nPasswayNo;
			}
		}
	}

	return 0;
}

int CGpcdef::GetNeighberGpcList(int nCurrSectionNo,int& nNeighberGpcNum, int* nNeighberGpcList)
{
	if( NULL == nNeighberGpcList )
		return 0;

	GPCSECTION *tmp = sectinfo.getContentFromID( nCurrSectionNo );

	if( NULL== tmp )
		return 0;

 	nNeighberGpcNum = tmp->nNeighberGpcCount;
	for(int i=0; i<nNeighberGpcNum; i++)
		nNeighberGpcList[i] = tmp->nNeighberGpcList[i];

  	return nNeighberGpcNum;
}

int CGpcdef::GetNeighberSectionList(int nCurrSectionNo,int& nNeighberSectionNum, int* nNeighberSectionList)
{
	if( NULL == nNeighberSectionList )
		return 0;

	GPCSECTION *tmp = sectinfo.getContentFromID( nCurrSectionNo );
	if( NULL==tmp) 
		return 0;

 	nNeighberSectionNum = tmp->nNeighberCount;
	for(int i=0; i<nNeighberSectionNum; i++)
		nNeighberSectionList[i] = tmp->nNeighberList[i];

  	return nNeighberSectionNum;
}
//int CGpcdef::GetSectionRouteStationList(int sectionNum,int sectionRouteNo, int* sectionRouteStationList,int *sectionRouteStationDelta)
//{
//	if( NULL == sectionRouteStationList || NULL == sectionRouteStationDelta )
//		return 0;
//
//	GPCSECTION *tmp = sectinfo.getContentFromID( sectionNum );
//	if( NULL == tmp || sectionRouteNo > tmp->nRouteCount )	// ???
//		return 0;
//
//	for(int i=0; i<tmp->nRouteStationCount[sectionRouteNo]; i++){
//		sectionRouteStationList[i] = tmp->nRouteStationList[sectionRouteNo][i];
//		sectionRouteStationDelta[i] = tmp->nRouteStationDelta[sectionRouteNo][i];
//	}
//
// 	return tmp->nRouteStationCount[sectionRouteNo];
//} 
 
//int CGpcdef::GetSectionRouteMum(int sectionNum)
//{
//	if(sectionNum >= userinfo[nUserIndex].nSectionList.size())
//		return 0;

//	GPCSECTION *tmp = sectinfo.getContentFromID
//		( userinfo[nUserIndex].nSectionList[sectionNum] );
//	if( NULL == tmp )
//		return 0;

// 	return tmp->nRouteCount;

/*	这段代码有逻辑问题
	if(sectionNum > userinfo[nUserIndex]->nSectionNum)
		return 0;	// not enough buffer
 	return sectinfo[sectionNum]->nRouteCount;
*/
//}
 
int CGpcdef::GetSectionNum()
{
	return userinfo[nUserIndex].nSectionList.size();
}

int CGpcdef::GetFirstSection()
{
	if(nUserIndex < 0 || nUserIndex >= userinfo.size())
		nUserIndex = 0;
	if(userinfo[nUserIndex].nSectionList.size() > 0)
		return userinfo[nUserIndex].nSectionList[0];
	return 0;
}

char *CGpcdef::GetSectionName(int nSection)
{
	GPCSECTION *tmp = sectinfo.getContentFromID( nSection );

	static char stname[32]="未知区段"; 
	if(NULL == tmp)
		return stname;

	return tmp->szSectionName;
}

//lyc 2001/9/22
char *CGpcdef::GetStationName(int nStation,bool bTGName)
{
	static char stname[32]="管外"; 
	if(bTGName == TRUE)
	{
		for (int i = 0;i< nSPStationNum;i++)
		{
		   if(spstationinfo[i]->nStation == nStation)
			   return spstationinfo[i]->sStationName;
		}
	}
	GPCSTATION *tmp = stainfo.getContentFromID(nStation);
	if(NULL == tmp)
		return stname;
	return tmp->szStationName;
}

int CGpcdef::GetStationNoByName(const char *name,bool bTGName)
{
	//lyc
	if(bTGName == TRUE)
	{
		for (int i = 0;i< nSPStationNum;i++)
		{
			if(!stricmp(spstationinfo[i]->sStationName,name))
		      return spstationinfo[i]->nStation;
		}
	}
	for(int j=0;j<stainfo.size();j++)
	{
		GPCSTATION &tmp = stainfo[j];
		if(!stricmp(tmp.szStationName,name))
			return tmp.getID();
	}
	return -1;
}

char *CGpcdef::GetBureauName()
{
	return szBureauName;
}

bool  CGpcdef::GetStationProperty(int nStation)
{
	GPCSTATION *tmp = stainfo.getContentFromID(nStation);
	if(NULL == tmp)
		return FALSE;
	return (tmp->nProperty)>0 ? true:false;
}

bool  CGpcdef::IsCtcStation(int nStation)
{
	GPCSTATION *tmp = stainfo.getContentFromID(nStation);
	if(NULL == tmp)
		return FALSE;
	return (tmp->nProperty>0)?true:false;
}

bool  CGpcdef::GetFlagDrawSides(int nStation)
{
	GPCSTATION *tmp = stainfo.getContentFromID(nStation);
	if(NULL == tmp)
		return FALSE;
	return tmp->bDrawSides;
}

bool  CGpcdef::GetFlagDrawTrainTime(int nStation)
{
	GPCSTATION *tmp = stainfo.getContentFromID(nStation);
	if(NULL == tmp)
		return FALSE;
	return tmp->bDrawTrainTime;
}

bool  CGpcdef::SetFlagDrawSides(int nStation,bool flag)
{
	GPCSTATION *tmp = stainfo.getContentFromID(nStation);
	if(NULL == tmp)
		return FALSE;
	tmp->bDrawSides = flag;
	return TRUE;
}

char *CGpcdef::GetPlotterName()
{
	return szPlotterName;
}

char *CGpcdef::GetPrinterName()
{
	return szPrinterName;
}

char *CGpcdef::GetDataPathName()
{
//	strcpy(pathname, DATADIR);
	strcpy(pathname, GetDataDir());
	return pathname;
}

char *CGpcdef::GetRunningPathName()
{
	GetCurrentDirectory(64, pathname);
	return pathname;
}

char *CGpcdef::GetAffairDataPathName()
{
//	sprintf(pathname, "%s\\affair", DATADIR);
    sprintf(pathname, "%s\\affair", GetDataDir());
	return pathname;
}
char *CGpcdef::GetDispatchDataPathName()
{
//	sprintf(pathname, "%s\\affair", DATADIR);
    sprintf(pathname, "%s\\affair", GetDataDir());
	return pathname;
}
char *CGpcdef::GetLayoutDataPathName()
{
//	sprintf(pathname, "%s\\layout", DATADIR);
	sprintf(pathname, "%s\\layout", GetDataDir());
	return pathname;
}

char *CGpcdef::GetReplayUserName(bool dss)
{
	if(dss)
	{
		return pbinfo.userName;
	}
	else
	{
		return pbinforoute.userName;
	}
}

char *CGpcdef::GetReplayPasswd(bool dss)
{
	if(dss)
	{
		return pbinfo.passwd;
	}
	else
	{
		return pbinforoute.passwd;
	}
}

char *CGpcdef::GetRouteReplayUserName()
{
	return pbinforoute.userName;
}

char *CGpcdef::GetRouteReplayPasswd()
{
	return pbinforoute.passwd;
}
int CGpcdef::GetRemoteInfo(char *remotepath, char *serveraddr)
{
	strcpy(remotepath, szRemotePathName);
	strcpy(serveraddr, szServerAddress);
	return 1;
}

static BOOL isNumberInIntArray(int usNumber,int *usArray,int usCount)
{
	int i;

	if(usArray == NULL || usCount == 0)
		return FALSE;
	for(i=0;i < usCount;i++)
	{
		if(usArray[i] == usNumber)
			return TRUE;
	}
	return FALSE;
}

int CGpcdef::FindRouteInform(int nStationNum, int * nStationList,  GPCROUTE * * RouteInfo)
{
 	int count = 1,i,j,k,bFound,l;
	GPCROUTE *ri;

	if(nStationNum == 0 || nStationList == NULL || RouteInfo == NULL)
		return -1;
	RouteInfo[count - 1] = (GPCROUTE *)malloc(sizeof(GPCROUTE));
	if(RouteInfo[count - 1] == NULL)
		return -1;

	for(i=0; i <routeinfo_wsg.size(); i++)
	{// all route loop
		ri = &routeinfo_wsg[i];
		j = 0;
		do {
			RouteInfo[count - 1]->nStationNum = 0;
			for(;j < ri->nStationNum;j++)
			{
				if(isNumberInIntArray(ri->nStationList[j],nStationList,nStationNum))
					break;
			}
			if(j == ri->nStationNum)
				break; //all route station not in station list
			RouteInfo[count - 1]->nStationNum = 1;
			RouteInfo[count - 1]->nStationList[0] = ri->nStationList[j];
			// lyc 2001/9/20
	    	RouteInfo[count - 1]->nStationSpaceList[0] = ri->nStationSpaceList[j];
			j++;
 			for(;j < ri->nStationNum;j++)
			{
				if(!isNumberInIntArray(ri->nStationList[j],nStationList,nStationNum))
					break;
				RouteInfo[count - 1]->nStationList[RouteInfo[count - 1]->nStationNum] = ri->nStationList[j];
				// lyc 2001/9/20
	 			RouteInfo[count - 1]->nStationSpaceList[RouteInfo[count - 1]->nStationNum] = ri->nStationSpaceList[j];
 				RouteInfo[count - 1]->nStationNum += 1;
			}
		}while(RouteInfo[count - 1]->nStationNum == 1 && j < ri->nStationNum);
		if(j == ri->nStationNum && RouteInfo[count - 1]->nStationNum <= 1)
			continue; //all route station not in station list
		if(RouteInfo[count - 1]->nStationNum != 1)
		{
			bFound = 0;
			for(k=0;k < count - 1;k++)
			{
				// determine if route k in last route
				for(j=0;j < RouteInfo[k]->nStationNum;j++)
				{
					for(l=0;l < RouteInfo[count - 1]->nStationNum;l++)
					{
						if(RouteInfo[k]->nStationList[j] == 
							RouteInfo[count - 1]->nStationList[l])
							break;
					}
					if(l < RouteInfo[count - 1]->nStationNum)
						break;
				}
				if(j == RouteInfo[k]->nStationNum)
					continue; 
				if(j == 0)
				{
					for(;j < RouteInfo[k]->nStationNum;j++)
					{
						if(l == RouteInfo[count - 1]->nStationNum)
							break;
						if(RouteInfo[k]->nStationList[j] != 
							RouteInfo[count - 1]->nStationList[l])
							break;
						l += 1;
					}
					if(j == RouteInfo[k]->nStationNum)
					{
						//using last route replace route k
						if(RouteInfo[k]->nStationNum != RouteInfo[count - 1]->nStationNum)
						{
							RouteInfo[k]->nStationNum = RouteInfo[count - 1]->nStationNum;
							for(j=0;j < RouteInfo[count - 1]->nStationNum;j++){
								RouteInfo[k]->nStationList[j] = RouteInfo[count - 1]->nStationList[j];
							    // LYC 2001/9/20
    							RouteInfo[k]->nStationSpaceList[j] = RouteInfo[count - 1]->nStationSpaceList[j];
							}
							RouteInfo[k]->nRouteNo = ri->nRouteNo;
							RouteInfo[k]->nRouteSetNo = ri->nRouteSetNo;
							RouteInfo[k]->nRouteDirect = ri->nRouteDirect;
							if(ri->nRouteDirect == 3)
							{
								RouteInfo[k]->spStation = ri->spStation;
							}
						}
						bFound = 1;
						break;
					}
				}
				if(bFound)
					break;
				// determine if last route in route k
				for(j=0;j < RouteInfo[count - 1]->nStationNum;j++)
				{
					for(l=0;l < RouteInfo[k]->nStationNum;l++)
					{
						if(RouteInfo[k]->nStationList[l] == 
							RouteInfo[count - 1]->nStationList[j])
							break;
					}
					if(l < RouteInfo[k]->nStationNum)
						break;
				}
				if(j == RouteInfo[count - 1]->nStationNum)
					continue;
				if(j == 0)
				{
					for(;j < RouteInfo[count - 1]->nStationNum;j++)
					{
						if(l == RouteInfo[k]->nStationNum)
							break;
						if(RouteInfo[count - 1]->nStationList[j] != 
							RouteInfo[k]->nStationList[l])
							break;
						l += 1;
					}
					if(j == RouteInfo[count - 1]->nStationNum)
					{
						//already have such route
						bFound = 1;
						break;
					}
				}
			}
			if(bFound)
				continue;// have find such route
			RouteInfo[count - 1]->nRouteNo = ri->nRouteNo;
			RouteInfo[count - 1]->nRouteSetNo = ri->nRouteSetNo;
			RouteInfo[count - 1]->nRouteDirect = ri->nRouteDirect;
			if(ri->nRouteDirect == 3)
			{
				RouteInfo[count - 1]->spStation = ri->spStation;
			}
			count += 1;
			RouteInfo[count - 1] = (GPCROUTE *)malloc(sizeof(GPCROUTE));
			if(RouteInfo[count - 1] == NULL)
				return -1;
		}
	}

	
	free(RouteInfo[count - 1]);
	RouteInfo[count - 1] = NULL;

	return count - 1;
}

int CGpcdef::GetStationSideNum(int nStation)
{
	GPCSTATION *tmp = stainfo.getContentFromID(nStation);
	if(NULL == tmp)
		return 0;
	return tmp->nStationSideNum;
}

int CGpcdef::GetSectionFromStationNo(int lStationNo)
{
	int i,j;

	for(i=0; i<sectinfo.size(); i++)
	{
		for(j=0; j<sectinfo[i].nStationNum; j++)
		{
			if(lStationNo == sectinfo[i].nStationList[j])
				return(sectinfo[i].nSection);
		}
	}

	return -1;
}

CString CGpcdef::GetStationKiloMeter(int nStationNo)
{
	GPCSTATION *tmp = stainfo.getContentFromID(nStationNo);
	if (NULL != tmp)
	{
		return CString(tmp->fStaDistance);
	}

	return "0.0";
}

int CGpcdef::GetStationParameter(int nFirstStationNo, int nSecondStationNo,
	char* strStationName, CString &fDistance,float &fDeltaDistance,int flag)
{
	int nStationNo = (flag)?nFirstStationNo:nSecondStationNo;
	GPCSTATION *tmp = stainfo.getContentFromID(nStationNo);
	if (NULL == tmp)
	{
		CString stmp;
		stmp.Format("从GPC.DEF获取%d车站公里标信息失败！", nStationNo);
		//AfxMessageBox(stmp,MB_OK);

		sprintf(strStationName, "%d站", nStationNo);
		fDistance = "";
	}
	else
	{
		strcpy(strStationName, tmp->szStationName);
		fDistance = CString(tmp->fStaDistance);
	}

	return 1;
}

int CGpcdef::GetAbsRuntime(int usTrainType, int usSrcStationNo,
				int usDstStationNo, int usDirect, int usRunFlag)
{
	return 5;
}


int CGpcdef::CompleteWintgTermUserInfo()
{
	// fill station info in userinfo
	if(nUserIndex < 0 || nUserIndex >= userinfo.size())
		nUserIndex = 0;
	
	for(int j=0; j<userinfo[nUserIndex].nSectionList.size(); j++) 
	{
		int sectionNo = userinfo[nUserIndex].nSectionList[j];

		GPCSECTION *tmp = sectinfo.getContentFromID( sectionNo );
		if( NULL == tmp )
		{
			CString stmp;
			stmp.Format("[SECTION]没有定义%d号区段",sectionNo);
			AfxMessageBox(stmp);
			return 0;
		}

		for(int k = 0; k<tmp->nStationNum; k++)
		{
			if( find(userinfo[nUserIndex].stationlist.begin(),
					userinfo[nUserIndex].stationlist.end(),
					tmp->nStationList[k])
				== userinfo[nUserIndex].stationlist.end()
				)
			{
				userinfo[nUserIndex].stationlist.push_back(tmp->nStationList[k]);
			}
		}		
	}

	// success return
	return 1;
}

BOOL CGpcdef::IsRouteViewChangeByStation()
{
	return routeviewchangeinfo.bChangeByStationSel;

}
BOOL CGpcdef::IsRouteViewChangeBySection()
{

	return routeviewchangeinfo.bChangeBySectionSel;
}

int  CGpcdef::GetRouteViewStationPos()
{
		return routeviewchangeinfo.nStationRouteViewPos; 
}
int  CGpcdef::GetRouteViewSectionPos()
{
	return routeviewchangeinfo.nSectionRouteViewPos; 
}

char* CGpcdef::GetShareDataDir()
{
    sprintf(datapathname, "Z:\\DDMLShare",dcSaveinfo.sDriveName);  
	return datapathname;
}

BOOL CGpcdef::IsDDMLLocalDrive()
{
  if(dcSaveinfo.bSavetoLocalDrive)
	  return TRUE;
  else
	  return FALSE;
}

BOOL CGpcdef::IsDDMLSaveToServer()
{
  if(dcSaveinfo.bSavetoLocalDrive == 2)
	  return TRUE;
  else
	  return FALSE;
}

char* CGpcdef::GetDDMLDataDir(BOOL bDefalutFlag, int nSectionNo)
{
	if(dcSaveinfo.bSavetoLocalDrive)
		return GetDataDir();
	else
	{
		if(bDefalutFlag)
			sprintf(datapathname, "%s:\\section_%02d\\data",dcSaveinfo.sDriveName, GetDefaultSection() );  
		else
			sprintf(datapathname, "%s:\\section_%02d\\data",dcSaveinfo.sDriveName, nSectionNo );  
	}
    ////sprintf(datapathname, "Z:\\%s\\data",GetDefaultUserName());  
	return datapathname;   
}

void CGpcdef::SetGPCWorkDir(const char *path)
{
	strcpy(m_szGPCWorkPath, path);
}

char* CGpcdef::GetWorkPathName()
{
	return m_szGPCWorkPath;
}

const char *CGpcdef::GetXMLDir()
{
	sprintf(m_szGPCDataPath,"%s\\xml",m_szGPCWorkPath);		
	return m_szGPCDataPath;
}

char* CGpcdef::GetDataDir()
{
	sprintf(m_szGPCDataPath,"%s\\data",m_szGPCWorkPath);		
	return m_szGPCDataPath;
}

const char* CGpcdef::GetLogDir()  // CUIHU 2008.11.24
{
	sprintf(m_szGPCDataPath,"%s\\log",m_szGPCWorkPath);		
	return m_szGPCDataPath;
}

int CGpcdef::LoadSetup()
{
	ushort i = 0;
    //ushort j ;
	
	char username[32],password[32];
	long bSetup;

	FILE* fp;
	char seps[]=" ,=\t\n", *ptr, *substr;
	char szFileName[128], strLine[256];
 //	sprintf(szFileName,"%s\\%s",DATADIR,"setup.dat");
  	sprintf(szFileName,"%s\\%s",GetDataDir(),"setup.dat");
	if((fp=fopen(szFileName, "r"))==NULL)
 		return FALSE;

	while(!feof(fp)) {
		ptr = fgets(strLine, 256, fp);
		if (ptr == NULL) 
		continue;
		while (*ptr == ' ' || *ptr == '\t')
			ptr ++;
		if (*ptr == ';')	
		continue;
		if (*ptr == '\n')	
		continue;
		if(substr=strstr(ptr, "USERNAME")) {		
			sscanf(substr," USERNAME = %s PASSWORD = %s SETUP = %d\n",
				username,password,&bSetup);
            for(i=0; i<userinfo.size(); i++){
				if(!strcmp(username, userinfo[i].UserName))
					break;
			}
			if(i >= userinfo.size()) continue;     
			strcpy(userinfo[i].Password,password);
			userinfo[i].bSetup = bSetup;
		} else continue;
	}

	fclose(fp);
	return TRUE;
}

int CGpcdef::SaveSetup()
{
	ushort k = 0;
    //ushort j ;
	
	FILE* fp;
	char seps[]=" ,=\t\n";
	char szFileName[128];
 //	sprintf(szFileName,"%s\\%s",DATADIR,"setup.dat");
 	sprintf(szFileName,"%s\\%s",GetDataDir(),"setup.dat");
 	if((fp=fopen(szFileName, "w"))==NULL)
 		return FALSE;

	for(int i=0; i<userinfo.size(); i++){
       	fprintf(fp," USERNAME = %s PASSWORD = %s SETUP = %d\n",
			userinfo[i].UserName, userinfo[i].Password,userinfo[i].bSetup);
	}
	fflush(fp);
  
	fclose(fp);
	return TRUE;
}

void CGpcdef::ChangeUserPassword()
{
 /*    CPasswordDlg *dlg = new CPasswordDlg(CString(userinfo[nUserIndex].Password));
	 
	 if(dlg->DoModal() == IDOK){
		 char temp[32];
 	     strcpy(temp,(LPCSTR)dlg->m_new_ps1 );
		 SetUserPassword(temp);
	 }
 	 delete dlg;*/
}

int * CGpcdef::GetSubstation(int nStationNo, int & count)
{
	GPCSPSTATION *spinfo;

	for(int i=0;i < nSPStationNum;i++)
	{
		spinfo = spstationinfo[i];
		if(spinfo->nStation == nStationNo)
		{
			count = spinfo->nStationNum;
			return spinfo->nStationList;
		}
	}
	return NULL;
}

int CGpcdef::GetCurDispathNo()
{
	// current user name
	return userinfo[nUserIndex].nCurDispathNo;	
}

int CGpcdef::RestoreOdDispathNo()
{
	userinfo[nUserIndex].nCurDispathNo = userinfo[nUserIndex].nOldDispathNo;	
	return userinfo[nUserIndex].nCurDispathNo;	
} 

int CGpcdef::GetPBConfig(char * abLocalPath, char * abRemotePath,char *abServerAddress, char *mShareDir,bool dss)
{
	int theStatus = 1;
	
	PLAYBACKINFO *info ;
	if(dss==false)
		info = &pbinforoute;
	else
		info = &pbinfo;

	do {
		if(abLocalPath == NULL || abRemotePath == NULL ||
			abServerAddress == NULL)
			break;
		strcpy(abLocalPath,info->abLocalPlaybackPath);
		strcpy(abRemotePath,info->abRemotePlaybackPath);
		if(info->nActiveServerAddressIndex == -1 ||
			info->nActiveServerAddressIndex >= info->nServerAddressNum)
			info->nActiveServerAddressIndex = 0;
		strcpy(abServerAddress,info->abServerAddress[info->nActiveServerAddressIndex]);

		if(mShareDir != NULL)
		{
			strcpy(mShareDir,info->mPbShareDir);
		}


		theStatus = 0;
	}while(0);
	if(theStatus)
		return -1;
	return 0;
}


int CGpcdef::GetNextServerAddress(char *abServerAddress, int nIndex,bool dss)
{

	int theStatus = 1;
	PLAYBACKINFO* info;
	if(dss==false)
		info = &pbinforoute;
	else
		info = &pbinfo;

	do {
		if(info->nActiveServerAddressIndex == -1)
			info->nActiveServerAddressIndex = 0;
		if(info->nActiveServerAddressIndex + 1 >= info->nServerAddressNum)
		{
			info->nActiveServerAddressIndex = 0;
			nIndex = info->nActiveServerAddressIndex;
		} else {
			nIndex = info->nActiveServerAddressIndex + 1;
			info->nActiveServerAddressIndex += 1;
		}
		if(abServerAddress == NULL || nIndex >= info->nServerAddressNum)
			break;
		
		if(info->nActiveServerAddressIndex >= info->nServerAddressNum)
			info->nActiveServerAddressIndex = 0;
		strcpy(abServerAddress,info->abServerAddress[nIndex]);
		info->nActiveServerAddressIndex = nIndex;
		theStatus = 0;
	}while(0);
	if(theStatus)
		return -1;
	return 0;
}


int CGpcdef::GetNextLogServerAddress(char *abServerAddress, int nIndex)
{
	int theStatus = 1;
	LOGSVRINFO *info = &logSvrInfo;
	
	do {
		if(info->nActiveLogSvrAddrIndex == -1)
			info->nActiveLogSvrAddrIndex = 0;
		if(info->nActiveLogSvrAddrIndex + 1 >= info->nLogSvrAddrNum)
		{
			info->nActiveLogSvrAddrIndex = 0;
			nIndex = info->nActiveLogSvrAddrIndex;
		} else {
			nIndex = info->nActiveLogSvrAddrIndex + 1;
			info->nActiveLogSvrAddrIndex += 1;
		}
		if(abServerAddress == NULL || nIndex >= info->nLogSvrAddrNum)
			break;
		
		if(info->nActiveLogSvrAddrIndex >= info->nLogSvrAddrNum)
			info->nActiveLogSvrAddrIndex = 0;
		strcpy(abServerAddress, info->abLogSvrAddress[nIndex]);
		info->nActiveLogSvrAddrIndex = nIndex;
		theStatus = 0;
	}while(0);
	if(theStatus)
		return -1;
	return 0;
}



int CGpcdef::GetLogServerAddressNum( ) 
{
	LOGSVRINFO *info = &logSvrInfo;
	
	return info->nLogSvrAddrNum;
}


int CGpcdef::GetServerAddressNum( ) 
{
	PLAYBACKINFO *info = &pbinfo;

	return info->nServerAddressNum;
}

int CGpcdef::GetRouteServerAddressNum( ) 
{
	PLAYBACKINFO *info = &pbinforoute;

	return info->nServerAddressNum;
}

int CGpcdef::SetServerAddr(char *abServerAddress)
{
	int theStatus = 1;
	int i;
	PLAYBACKINFO *info = &pbinfo;

	do {
		if(abServerAddress == NULL || strlen(abServerAddress) == 0)
			break;
		if(strlen(abServerAddress) > 
			sizeof(info->abServerAddress[0]) / sizeof(info->abServerAddress[0][0]) )
			break;
		for(i=0;i < info->nServerAddressNum;i++)
		{
			if(strcmp(info->abServerAddress[i],abServerAddress) == 0)
			{
				info->nActiveServerAddressIndex = i;
				break;
			}
		}
		theStatus = 0;
	}while(0);
	if(theStatus)
		return -1;
	return 0;
}


int CGpcdef::LoadRouteServerInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";


	pbinforoute.nServerAddressNum = 0;

	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	strcpy(pbinforoute.abServerAddress[0],token);
	pbinforoute.nServerAddressNum = 1;
	// load page station num;
	while(token = strtok(NULL, seps))
	{
		if(token==NULL) return 0;
		strcpy(pbinforoute.abServerAddress[pbinforoute.nServerAddressNum],token);
		pbinforoute.nServerAddressNum += 1;
	}
	return 1;
}

int CGpcdef::LoadServerInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";


	pbinfo.nServerAddressNum = 0;

	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	strcpy(pbinfo.abServerAddress[0],token);
	pbinfo.nServerAddressNum = 1;
	// load page station num;
	while(token = strtok(NULL, seps))
	{
		if(token==NULL) return 0;
		strcpy(pbinfo.abServerAddress[pbinfo.nServerAddressNum],token);
		pbinfo.nServerAddressNum += 1;
	}
	return 1;
}

int CGpcdef::LoadLogSvrAddr(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	
    logSvrInfo.nLogSvrAddrNum = 0;
	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;
	strcpy(logSvrInfo.abLogSvrAddress[0], token);
	logSvrInfo.nLogSvrAddrNum = 1;

	while(token = strtok(NULL, seps))
	{
		if(token==NULL) 
			return 0;
		strcpy(logSvrInfo.abLogSvrAddress[logSvrInfo.nLogSvrAddrNum], token);
		logSvrInfo.nLogSvrAddrNum += 1;
	}
	return 1;
}

char* CGpcdef::GetLogSvrPath()
{
	return logSvrInfo.abRemoteLogSvrPath;
}

char* CGpcdef::GetLogLocalPath()
{
	return logSvrInfo.abLocalLogPath;
}

int CGpcdef::LoadLogSvrUp(char *ptr)
{
	char *token;
	char seps[]=" :=\n";
	
	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;
	strcpy(logSvrInfo.userName, token);
	
	token = strtok(NULL, seps);
	if(token == NULL)
		return 0;
	strcpy(logSvrInfo.passwd, token);
	
	return 1;
}

char* CGpcdef::GetLogSvrUserName()
{
	return logSvrInfo.userName;
}

char* CGpcdef::GetLogSvrPasswd()
{
	return logSvrInfo.passwd;
}

int CGpcdef::LoadReplayUserAndPasswd(char *ptr)
{
	char *token;
	char seps[]=" :=\n";

	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;
	strcpy(pbinfo.userName, token);

	token = strtok(NULL, seps);
	if(token == NULL)
		return 0;
	strcpy(pbinfo.passwd, token);
	
	return 1;
}

int CGpcdef::LoadRouteReplayUserAndPasswd(char *ptr)
{
	char *token;
	char seps[]=" :=\n";

	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;
	strcpy(pbinforoute.userName, token);

	token = strtok(NULL, seps);
	if(token == NULL)
		return 0;
	strcpy(pbinforoute.passwd, token);
	
	return 1;
}


int CGpcdef::LoadLimitModifyActualTime(char *ptr)
{
	m_nManualConvertActualTimeLength=0;
	
	char *token;
	char seps[]=" ,\t\\/=\n";
 
	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;
	m_nManualConvertActualTimeLength = atoi(token);
	if(m_nManualConvertActualTimeLength<0)
		m_nManualConvertActualTimeLength=0;
	
	if(m_nManualConvertActualTimeLength>0)
		m_bLimitModifyActualTime=true;
	else
		m_bLimitModifyActualTime=false;
	return 1;
}

int CGpcdef::LoadDispatcherCommandSaveInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
 
	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;
	dcSaveinfo.bSavetoLocalDrive = (BOOL)atoi(token);

	token = strtok(NULL, seps);
	if(token == NULL)
		return 0;
	strcpy(dcSaveinfo.sDriveName, token);
  
	return 1;
}
int CGpcdef::LoadIntValue(char *ptr, short& value)
{
	int v, rs = LoadIntValue(ptr, v);
	if (!rs)
		return 0;

	value = (short)v;
	return rs;
}
int CGpcdef::LoadIntValue(char *ptr, int &value)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
 
	value = 0;
	
	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;

	value = atoi(token);

	return 1;	
}

int CGpcdef::LoadIntValue(char *ptr, int& value, int& value2)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
 
	value = 0;
	value2 = 0;

	token = strtok(ptr, seps);
	if(token==NULL) 
		return 1;

	value = atoi(token);

	token = strtok(NULL, seps);
	if(token == NULL)
		return 1;
	value2 = atoi(token);

	return 1;	
}

int CGpcdef::LoadRouteViewIfChangebyStation(char *ptr)
{

	char *token;
	char seps[]=" ,\t\\/=\n";
 
	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;
	routeviewchangeinfo.bChangeBySectionSel = (BOOL)atoi(token);

	token = strtok(NULL, seps);
	if(token == NULL)
		return 0;
	routeviewchangeinfo.bChangeByStationSel = (BOOL)atoi(token);
 
	//没有配置时按缺省处理，而不报错
	token = strtok(NULL, seps);
	if(token == NULL)
		return 1;
	routeviewchangeinfo.nSectionRouteViewPos = atoi(token);

	token = strtok(NULL, seps);
	if(token == NULL)
		return 1;
	routeviewchangeinfo.nStationRouteViewPos = atoi(token);

	return 1;
}
int CGpcdef::LoadDispatcherCommandInfo(char *ptr)
{
	int nStartST,nEndST;
	char *token;
	char seps[]=" ,\t\\/=\n";

	GPCDCINFO tmp;

	memset(&tmp, 0, sizeof(GPCDCINFO));
	
	token = strtok(ptr,seps);
	if(token == NULL) return 0;
	tmp.no = (int)atoi(token);

 	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strcpy(tmp.dispatchercommandname, token);

	//load cmd no
	token = strtok(NULL,seps);
	if(token==NULL)return 0;
	int uStartCommandNo,uEndCommandNo;
	if(strchr(token,'-'))
	{
		sscanf(token,"%d-%d",&uStartCommandNo,&uEndCommandNo);
		itoa(uStartCommandNo,tmp.sStartCommandNo ,10);
		itoa(uEndCommandNo,tmp.sEndCommandNo,10);
	}
	
 
	//load send station lists
	token = strtok(NULL, seps);
	if(token==NULL)return 0;
	tmp.RcvStaitionNum = (int)atoi(token);
	for(int k=0; k<tmp.RcvStaitionNum; k++)
	{
		token = strtok(NULL,seps);
		if(token == NULL) return 0;
		if(strchr(token,'-'))
		{
			sscanf(token,"%d-%d",&nStartST,&nEndST);
			int count = k + nEndST - nStartST + 1;
			for(int j = k;j<count;j++)
			{
				tmp.RcvStationList[j] = nStartST;
				nStartST++;
			}
			k = j - 1;
		}
		else
			tmp.RcvStationList[k] = (int)atoi(token);
	}
	
	//load leader gpc no
	token = strtok(NULL,seps);
	if(token == NULL) return 0;
	tmp.nOnDutyLeader = (int)atoi(token);
	
	//load send gpc list
	token = strtok(NULL, seps);
	if(token==NULL)return 0;
	tmp.RcvGpcNum = (int)atoi(token);
	for( k=0; k<tmp.RcvGpcNum; k++)
	{
		token = strtok(NULL,seps);
		if(token == NULL) return 0;
		if(strchr(token,'-'))
		{
			sscanf(token,"%d-%d",&nStartST,&nEndST);
			int count = k + nEndST - nStartST + 1;
			for(int j = k;j<count;j++)
			{
				tmp.RcvGpcList[j] = nStartST;
				nStartST++;
			}
			k = j - 1;
		}
		else
			tmp.RcvGpcList[k] = (int)atoi(token);
	}
	//load send gpc property list
	token = strtok(NULL, seps);
	if(token==NULL)return 0;
	int nRecvNum = (int)atoi(token);

	for( k=0; k<nRecvNum; k++)
	{
		token = strtok(NULL,seps);
		if(token == NULL)  
		{
            tmp.RcvGpcProperty[k] =0;
			continue;
		}
		tmp.RcvGpcProperty[k] = (int)atoi(token);
	}
	
	dcinfo_wsg.saveContent( tmp );
	return 1;

 
}
int CGpcdef::GetPhaseNoteReceiverList(int sectionno,int recvlist[],int limit)
{
	GPCDCINFO *tmp = dcinfo_wsg.getContentFromID( sectionno );
	if( NULL == tmp )
		return -1;
	for(int i=0;i<tmp->RcvStaitionNum;i++)
		recvlist[i]=tmp->RcvStationList[i];
	return tmp->RcvStaitionNum;
}

ushort CGpcdef::GetRcvDCStationList(int no,int i)
{
	GPCDCINFO *tmp = dcinfo_wsg.getContentFromID( no );
	if( NULL == tmp )
		return -1;

	return tmp->RcvStationList[i];

 
}

ushort CGpcdef::GetRcvDCStationNum(int no)
{//第no号调度台的车站数量
	GPCDCINFO *tmp = dcinfo_wsg.getContentFromID( no );
	if( NULL == tmp )
		return 0;

		return tmp->RcvStaitionNum;

 
}

ushort CGpcdef::GetDispatcherCommandOndutyGpcNo(int nGpcNo)
{
	for(int i= 0; i < dcinfo_wsg.size(); i++)
	{
		if(dcinfo_wsg[i].no == nGpcNo )
			return dcinfo_wsg[i].nOnDutyLeader;
	}
	return -1;
}

ushort CGpcdef::GetDispatchNoFromDefaultUser()
{
	ushort usDispatchNo;
	char *ptr;
	ptr = szDefaultUser;
	ptr = strtok(szDefaultUser,"gpc");
	if(ptr == NULL)
	{
		sysprint(0,0,"error default user");
		return -1;
	}
	usDispatchNo = atoi(ptr);
	return usDispatchNo;

}

ushort CGpcdef::GetAllStationNum()
{
	return stainfo.size();
}
ushort CGpcdef::GetAllStationList(int i)
{
	return (ushort)(stainfo[i].nStation);
}
 
int CGpcdef::GetConsoleid()
{
    CString DispatchStationName;    // 调度台名称
	int consoleid;
    DispatchStationName.Format ("%s",GetCurUserName());
 
	ASSERT( DispatchStationName.Remove('g')==1 );
    ASSERT( DispatchStationName.Remove('p')==1 );
    ASSERT( DispatchStationName.Remove('c')==1) ;

	consoleid=atoi(DispatchStationName);
	ASSERT(consoleid>=0);
	return consoleid;
}

char *CGpcdef::GetCurUserConsoleName()
{
	// current user Console name
	return userinfo[nUserIndex].ConsoleName;	
}

 
BOOL CGpcdef::ShowDirectionString( const int iSectionID, const int iStationID, const char *dirStr )
{
	//ASSERT(NULL != dirStr);
	if(NULL == dirStr)	return FALSE;

	for( int i=0; i<dirStringInfo.size(); i++ )
	{
		if( dirStringInfo[i].iSectionID == iSectionID
			&& dirStringInfo[i].iStationID == iStationID )
		{
			if( strstr( dirStr, dirStringInfo[i].sDirString ) )
				return TRUE;
		}
	}

	return FALSE;
}


int	CGpcdef::GetSignalRadius()
{
	return abs( signalRadius.getContent() );
}

int CGpcdef::GetLineThickness()
{
	if( abs( lineThickness.getContent() ) <= 0)
		return 2;
	else
		return abs( lineThickness.getContent() );
}

 
 

GPCPAGE *CGpcdef::GetPageInform(int nPageID)
{
	return pageinfo.getContentFromID( nPageID );
}

int CGpcdef::GetDefaultSection()
{
	if(nUserIndex < 0 || nUserIndex >= userinfo.size())
		nUserIndex = 0;

	if(userinfo[nUserIndex].nSectionList.size() > 0)
		return userinfo[nUserIndex].nSectionList[0];
	return 0;
}

int CGpcdef::GetGpcSectionNum()
{
	if(nUserIndex < 0 || nUserIndex >= userinfo.size())
		nUserIndex = 0;

	return userinfo[nUserIndex].nSectionList.size();
}

unsigned long CGpcdef::GetCurTimeShiftId(ushort uProcessId)
{
	CTime t=CTime::GetCurrentTime();    //Initiae global time;
	int nYear  = t.GetYear();
	int nMonth = t.GetMonth();
	int nDay   = t.GetDay();
	int nHour  = t.GetHour();
	int nbanbie;        //1 or 2
	long usDate = (nYear - 1990) * 380 + (nMonth - 1) * 31 + nDay - 1;
	if(nHour>=18)
	{
		nbanbie=1;
		CTime tm;
		tm=CTime(long_time(OsGetNextDate(uProcessId,usDate),0));
		nDay=tm.GetDay();
		nYear=tm.GetYear();
		nMonth=tm.GetMonth();
	}
	else if(nHour<6)
	{
		nbanbie=1;
		nDay=t.GetDay();
	}
	else
	{  
		nbanbie=2;
		nDay=t.GetDay();
	}
	unsigned long nshiftid=nYear*1000000 + nMonth*10000 + nDay*100 + nbanbie;
    return nshiftid;

}

unsigned long CGpcdef::GetCurTimeShiftId(int nYear,int nMonth,int nDay,int nHour,int uProcessId)
{
	int nbanbie;        //1 or 2
	long usDate = (nYear - 1990) * 380 + (nMonth - 1) * 31 + nDay - 1;
	if(nHour>=18)
	{
		nbanbie=1;
		CTime tm;
		tm=CTime(long_time(OsGetNextDate(uProcessId,usDate),0));
		nDay=tm.GetDay();
		nYear=tm.GetYear();
		nMonth=tm.GetMonth();
	}
	else if(nHour<6)
	{
		nbanbie=1;
	}
	else
	{  
		nbanbie=2;
	}
	unsigned long nshiftid=nYear*1000000 + nMonth*10000 + nDay*100 + nbanbie;
    return nshiftid;
}
BOOL CGpcdef::GetOperatorRight(DWORD right)
{ 
	if((RightDefs[GetUserIdentify()] & right) == right)
	  return TRUE;		
	else 
		return FALSE;
}

BOOL CGpcdef::GetOperatorPrior()
{
	//return value = false indicate  having no operator priority
	//return value  = true indicate  having operator priority
	BOOL returnValue = FALSE;
	switch(GetUserIdentify())
	{
	case GPC_DISPATCHER:
	case GPC_TEST:
		returnValue = TRUE;
		break;
	case GPC_SCHEDULE:
	case GPC_OPERATOR:
	case GPC_SUPER_OPER:
	case GPC_DISPATCHERS:
	case GPC_SCREEN_LAYOUT:
	case GPC_TRAIN:
	case GPC_SERVICE:
	case GPC_MAINTAIN:
	case GPC_PLOT:
	case GPC_MACHINE:
	case GPC_SCREEN_TG:
	case GPC_AFFAIR_SERVER:
	case GPC_SUPER_DISPATCH:
	case GPC_OTHER_DISPATCH:
	case GPC_SUPER_MAINTAIN:
		returnValue = FALSE;
		break;
	default:
		returnValue = FALSE;
		break;
	}
	return returnValue;
}

bool  CGpcdef::IsShowStManualSetSide() 
{
	if (m_nShowSideNotConfirmed>0) 
		return true;    
	return false;
}
bool  CGpcdef::IsNewBasicDump() 
{
	if (m_nNewBasicDump>0) 
		return true;    
	return false;
}

bool  CGpcdef::IsEnableMMPlanFromCAD() 
{
	if (m_nEnableMMPlanFromCAD>0) 
		return true;    
	return false;
}

bool  CGpcdef::IsEnablePredict() 
{
	if (m_nEnablePredict>0) 
		return true;    
	return false;
}

bool  CGpcdef::IsEnableBasicDumpTime()
{
	if (m_nEnableBasicDumpTime>0) 
		return true;    
	return false;
}
//基本图的修改权限
//return value = false indicate  having no operator priority
//return value  = true indicate  having operator priority
BOOL CGpcdef::GetOperatorPrior4Basic()
{
	if (m_nBasicSchedulePrio) 
		return TRUE;    
	return FALSE;
}

void CGpcdef::SetManualConvertActualTimeLength(int tm)
{
	m_nManualConvertActualTimeLength=tm;
}

int CGpcdef::GetManualConvertActualTimeLength()
{
	return m_nManualConvertActualTimeLength;
}

bool CGpcdef::IsInhibitManualConvertActualTime()
{    
	return m_bLimitModifyActualTime;
}

void CGpcdef::SetManualConvertActualTime(bool b)
{
	m_bLimitModifyActualTime=b;
}

BOOL CGpcdef::IsHaveRightManangRight() const
{
	return m_bHaveRightManangRight;
}

void CGpcdef::SetRightManangRight()
{
	m_bHaveRightManangRight=true;
}


int CGpcdef::GetDCmdVisualHours(int& reqtimelength)
{    
	reqtimelength = m_nDcmdReqHours;
	return m_nDcmdVisualHours;
}
int CGpcdef::GetSectionPhaseStoreStationList(int sectionNo,int& stationNum, int* stationList)
{
	for(int i=0; i<m_vctSectionPhaseStation.size(); i++)
	{
		if(m_vctSectionPhaseStation[i].nSectionNo == sectionNo)
		{
			stationNum = m_vctSectionPhaseStation[i].nStationNum;
			for(int k=0; k<stationNum; k++)
				stationList[k] = m_vctSectionPhaseStation[i].nStationList[k];
			return stationNum;
		}
	}

	stationNum = 0;
	return 0;
}
//关于发邻台数据的详细配置
int CGpcdef::LoadSecNeighborReceiverInfo(char *ptr)
{
    if(ptr == NULL) return 0;
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i=0, j=0;
	static int firstInit = 1;
	GPCSECTION*  sec = NULL;

	//1 发送台号
    token = strtok(ptr, seps);
	if(token==NULL) return 0;
	int nSectionFrom = atoi(token);
 
	// 2 接收台号
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
    int nSectionTo = atoi(token);

	// 3 台间站数目
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
    int nNum= atoi(token);

	// 4 台间站车站号
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	int nStationNoMiddle = atoi(token);

	sec = sectinfo.getContentFromID(nSectionFrom);
	if(sec == NULL)
	{
		CString msg;
		msg.Format("gpc.def中找不到台%d的配置", nSectionFrom);
		AfxMessageBox(msg);
		return 0;
	}

	if(sec->nNeighberSendDataNum >= 6)
		return 1;
	for(i=0; i < sec->nNeighberSendDataNum; i++)
	{
		if(sec->NeighberSendData[i].nSectionTo == nSectionTo)
		{
			for(int j=0; j < sec->NeighberSendData[i].nStationNum;j++)
			{
				if(sec->NeighberSendData[i].Station[j].nStationNoMiddle == nStationNoMiddle)
					return 1;
			}
			break;
		}
	}
	if(firstInit)
	{
		firstInit = 0;
		sec->nNeighberSendDataNum=1;
		sec->NeighberSendData[i].nStationNum = 0;
		sec->NeighberSendData[i].nSectionFrom = nSectionFrom;
		sec->NeighberSendData[i].nSectionTo = nSectionTo;
	}
	else
	{
		for(i=0; i < sec->nNeighberSendDataNum && i<6; i++)
		{
			if(sec->NeighberSendData[i].nSectionFrom == nSectionFrom &&
				sec->NeighberSendData[i].nSectionTo == nSectionTo)
			{
				break;
			}
		}
		if(i == sec->nNeighberSendDataNum)
		{
			sec->nNeighberSendDataNum++;
			sec->NeighberSendData[i].nStationNum = 0;
			sec->NeighberSendData[i].nSectionFrom = nSectionFrom;
			sec->NeighberSendData[i].nSectionTo = nSectionTo;
		}
	}
    
	if(sec->nNeighberSendDataNum >= sizeof(sec->NeighberSendData)/sizeof(sec->NeighberSendData[0]))
	{
		CString msg;
		msg.Format("gpc.def中[SECNEIGHBOR_RECEIVER]发送台%d 接收台%d 的发邻台配置超出了预定个数32个", nSectionFrom, nSectionTo);
		AfxMessageBox(msg);
		return 1;
	}
	
	if(sec->NeighberSendData[i].nStationNum >= sizeof(sec->NeighberSendData[i].Station)/sizeof(sec->NeighberSendData[i].Station[0]))
	{
		CString msg;
		msg.Format("gpc.def中[SECNEIGHBOR_RECEIVER]发送台%d 接收台%d 的发邻台配置,最多支持32个台间站,超出了预定32个", nSectionFrom, nSectionTo);
		AfxMessageBox(msg);
		return 0;
	}
    
	j = sec->NeighberSendData[i].nStationNum;

	

    // 4 出入口数目/出入口列表
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	int nEntryExitNum = atoi(token);

	if(nEntryExitNum >= sizeof(sec->NeighberSendData[i].Station[j].nEntryExit_List)/sizeof(sec->NeighberSendData[i].Station[j].nEntryExit_List[0]))
	{
		CString msg;
		msg.Format("gpc.def中[SECNEIGHBOR_RECEIVER]发送台%d 接收台%d 台间站%d的出入口配置,最多支持32个,超出了预定32个", nSectionFrom, nSectionTo);
		AfxMessageBox(msg);
		return 0;
	}

	sec->NeighberSendData[i].Station[j].nStationNoMiddle = nStationNoMiddle;
	sec->NeighberSendData[i].Station[j].nEntryExitNum = nEntryExitNum;

	for(int n=0;  n<sec->NeighberSendData[i].Station[j].nEntryExitNum; n++ )
	{
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
		sec->NeighberSendData[i].Station[j].nEntryExit_List[n] = atoi(token);
	}

	// 5 是否向邻台发送 a由台间站向本台始发 b本台至台间站终到的列车(0否1是), 
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	sec->NeighberSendData[i].Station[j].nSendStartOrTerminateTrain = atoi(token);

	// 6 是否接受6中列出的情况下邻台发送过来的股道(0否1是)
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	sec->NeighberSendData[i].Station[j].nAcceptStartOrTerminateTrainSide = atoi(token);

	// 7 是否接受邻台发送过来经过两台的列车股道(0否1是)
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	sec->NeighberSendData[i].Station[j].nAcceptThroughTrainSide = atoi(token);

	// 8 台间站 在邻台的站号
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	sec->NeighberSendData[i].Station[j].nStationNoMiddle_neighber = atoi(token);
	
	// 9 台间站 在邻台的默认径路号
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	sec->NeighberSendData[i].Station[j].nPasswayNo = atoi(token);

	sec->NeighberSendData[i].nStationNum++;

	return 1;
}
	 

int CGpcdef::LoadSecNeighborInfo(char *ptr)
{
	if(ptr == NULL)
		return 0;

	char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j, nStart, nEnd;
	GPCSECTION*  sec = NULL;

	// load section number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	
	sec = sectinfo.getContentFromID(atoi(token)) ;
	if(sec == NULL)
		return 0;
	
	//邻台说明
	token = strtok(NULL, seps);
	if(token==NULL) return 0;

	// load neighber section  list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
 
	sec->nNeighberCount = (int)atoi(token);
	for(i=0; i<sec->nNeighberCount;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			if(strchr(token, '-')) {
				sscanf(token, "%d-%d", &nStart, &nEnd);
				for(j=nStart; j<=nEnd; j++)
				sec->nNeighberList[i++] = j;
			} else {
 				sec->nNeighberList[i] = (int)atoi(token);
				i++;
			}
	}
 
	// load neighber gpc list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	sec->nNeighberGpcCount = (int)atoi(token);
	for(i=0; i<sec->nNeighberGpcCount;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			if(strchr(token, '-')) {
				sscanf(token, "%d-%d", &nStart, &nEnd);
				for(j=nStart; j<=nEnd; j++)
					sec->nNeighberGpcList[i++] = j;
			} else {
 				sec->nNeighberGpcList[i] = (int)atoi(token);
				i++;
			}
	}

 	// load neighber property list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	 
	sec->nNeighberCount = (int)atoi(token);
	for(i=0; i<sec->nNeighberCount;)
	{
    	token = strtok(NULL, seps);
    	if(token==NULL) return 0;
	
	    sec->nNeighberPropertyList[i] = (int)atoi(token);
	    i++;	  
	}

  	// load iewPlan neighber list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	sec->nViewPlanNeighberCount = (int)atoi(token);
	for(i=0; i<sec->nViewPlanNeighberCount;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			if(strchr(token, '-')) {
				sscanf(token, "%d-%d", &nStart, &nEnd);
				for(j=nStart; j<=nEnd; j++)
				sec->nViewPlanNeighberList[i++] = j;
			} else {
 				sec->nViewPlanNeighberList[i] = (int)atoi(token);
				i++;
			}
	}

	// load nViewPlanNeighber neighber gpc list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	sec->nViewPlanNeighberGpcCount = (int)atoi(token);
	for(i=0; i<sec->nViewPlanNeighberGpcCount;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			if(strchr(token, '-')) {
				sscanf(token, "%d-%d", &nStart, &nEnd);
				for(j=nStart; j<=nEnd; j++)
					sec->nViewPlanNeighberGpcList[i++] = j;
			} else {
 				sec->nViewPlanNeighberGpcList[i] = (int)atoi(token);
				i++;
			}
	}
	
	// scuccess return
	return 1;

}
int CGpcdef::LoadSectionPhaseStoreStation(char * ptr)  //区段打印的编组信息
{	
	char *token;
	char seps[]=" ,\t\\/=\n";
 
	int nStart,nEnd,j;
    //section No;
	token = strtok(ptr, seps);
	if(token==NULL)
		return FALSE;

	GPCSECTIONPHASESTORETRAIN phasestation;
    phasestation.nSectionNo = (int)atoi(token);

    // load station num;
	token = strtok(NULL, seps);
	int stationNum  =  (int)atoi(token);  
	if(stationNum <0 || stationNum >128)  
		return FALSE;

	phasestation.nStationNum = stationNum;      
	// load station list;
	for(int i=0; i<stationNum&&i<128;)
	{
		token = strtok(NULL,seps);
		if(token == NULL) 
			return FALSE;
		if(strchr(token,'-'))
		{
			sscanf(token,"%d-%d",&nStart,&nEnd);
			if(nStart < nEnd)
			{
				for(j=nStart; j<=nEnd&&i<128; j++)
				{
					phasestation.nStationList[i++] = j;
				}
			}
			else
			{
				for(j=nStart; j>=nEnd&&i<128; j--)
				{
					phasestation.nStationList[i++] = j;
				} 
			}
		}
		else
		{
			phasestation.nStationList[i++] = (int)atoi(token);
		}
	}

	//开始加载站存车的打印配置，0表示剩下的平均分配
	//预先给各个车站平均分配
	float averageRate = (float)1.0/stationNum;
	int *rateFlag = new int[stationNum];//如果对应的车站已经有了比例，那么该值置TRUE
	for(i=0; i<stationNum; i++)
	{
		phasestation.nStationPrintRate[i] = averageRate;
		rateFlag[i] = FALSE;
	}
	
	float stationTotalRate = 0.0;
	float stationRate = 0.0;
	int stationNo = 0;
	
	while((token = strtok(NULL,seps)) != NULL)
	{
		if(strchr(token,'-'))
		{
			sscanf(token,"%d-%f",&stationNo,&stationRate);
			if(stationNo == 0)//平均分配
			{
				break;
			}

			//读取各站对应的比例，赋到对应的数组
			for(i=0; i<stationNum; i++)
			{
				if(phasestation.nStationList[i] == stationNo)
				{
					phasestation.nStationPrintRate[i] = stationRate;
					stationTotalRate += stationRate;
					rateFlag[i] = TRUE;
				}
			}
		}
		else
		{
			break;
		}
	} 


	//判断有多少个站已经赋了初值，如果0个，那么平均分配，否则将剩下的平均分配
	int rateCount = 0;
	for(i=0; i<stationNum; i++)
	{
		if(rateFlag[i])
		{
			rateCount++;
		}
	}
	if(rateCount>0)
	{
		averageRate = (1.0-stationTotalRate)/(stationNum-rateCount);
		for(i=0; i<stationNum; i++)
		{
			if(!rateFlag[i])
			{
				phasestation.nStationPrintRate[i] = averageRate;
			}
		}
	}

	m_vctSectionPhaseStation.push_back(phasestation);
	delete[] rateFlag;
	
	return TRUE;
}
int CGpcdef::LoadEleModule(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	int tempInt;
    
	//区间号
	token = strtok(ptr, seps);
	if(token==NULL)
		return FALSE;
    int section = (int)atoi(token); 
	if(section <0 || section >20)
		return FALSE;
	elemodule[nEleModuleNum] = (GPCELECTRONICMODULE*)malloc(sizeof(GPCELECTRONICMODULE));
	memset((void*)elemodule[nEleModuleNum],0x00,sizeof(GPCELECTRONICMODULE));
	if(elemodule[nEleModuleNum] == NULL)
		return FALSE;

  	elemodule[nEleModuleNum]->nSection = section;

    // load 区间名字;
	token = strtok(NULL, seps);
	if(token==NULL)
		return FALSE;
	strcpy(elemodule[nEleModuleNum]->szSectionName,token);
	
	// 电力臂个数
	token = strtok(NULL, seps);
	if(token==NULL)
		return FALSE;
	tempInt =  atoi(token);
	elemodule[nEleModuleNum]->nElectronicHandNum = tempInt;


	for(int i=0; i<elemodule[nEleModuleNum]->nElectronicHandNum;i++)
	{

		token = strtok(NULL,seps);
		if(token == NULL) 
			return FALSE;
		
		if(sscanf(token,"%d|%d|%d|%d|%d|%d",&elemodule[nEleModuleNum]->nStationStartNo[i]
			,&elemodule[nEleModuleNum]->nStationEndNo[i]
			,&elemodule[nEleModuleNum]->nStartKilo[i]
			,&elemodule[nEleModuleNum]->nStartMeter[i]
			,&elemodule[nEleModuleNum]->nEndKilo[i]
			,&elemodule[nEleModuleNum]->nEndMeter[i]) == EOF)
			return FALSE;
	}
	nEleModuleNum++;
	return TRUE;

}

GPCELECTRONICMODULE* CGpcdef::GetEleModule(int sectionNo)
{
	for(int i=0; i<nEleModuleNum; i++)
	{
		if(elemodule[i]->nSection == sectionNo)
			return elemodule[i];
	}
	return NULL;

}
int CGpcdef::LoadDispatchSectInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j, nStart, nEnd;

	GPCSECTION  tmp;

	// load section number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	tmp.nSection = (int)atoi(token);
	
	// load section name
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strcpy(tmp.szSectionName, token);
	
	// load station list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	tmp.nStationNum = (int)atoi(token);
	for(i=0; i<tmp.nStationNum;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			if(strchr(token, '-')) {
				sscanf(token, "%d-%d", &nStart, &nEnd);
				for(j=nStart; j<=nEnd; j++)
				tmp.nStationList[i++] = j;
			} else {
 				tmp.nStationList[i] = (int)atoi(token);
				i++;
			}
	}

	// load neighber list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	tmp.nNeighberCount = (int)atoi(token);
	for(i=0; i<tmp.nNeighberCount;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			if(strchr(token, '-')) {
				sscanf(token, "%d-%d", &nStart, &nEnd);
				for(j=nStart; j<=nEnd; j++)
				tmp.nNeighberList[i++] = j;
			} else {
 				tmp.nNeighberList[i] = (int)atoi(token);
				i++;
			}
	}

	// load neighber gpc list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	tmp.nNeighberGpcCount = (int)atoi(token);
	for(i=0; i<tmp.nNeighberGpcCount;) {
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			if(strchr(token, '-')) {
				sscanf(token, "%d-%d", &nStart, &nEnd);
				for(j=nStart; j<=nEnd; j++)
					tmp.nNeighberGpcList[i++] = j;
			} else {
 				tmp.nNeighberGpcList[i] = (int)atoi(token);
				i++;
			}
	}

	sectinfo_dispatch.saveContent( tmp );
	// scuccess return
	return 1;

}

char *CGpcdef::GetDispatchSectionName(int nSection)
{
	GPCSECTION *tmp = sectinfo_dispatch.getContentFromID( nSection );

	if(NULL == tmp)
		return NULL;

	return tmp->szSectionName;
}

int CGpcdef::GetDispatchStationNum(int nSection)
{
	GPCSECTION *tmp = sectinfo_dispatch.getContentFromID( nSection );

	if( NULL == tmp )
		return 0;

	return tmp->nStationNum;
}

int CGpcdef::GetDispatchStationList(int nSection, int& stationNum, int* stationList)
{
	int max_count = stationNum;

	//stationNum = 0;
	if( NULL == stationList )
		return 0;

	GPCSECTION *tmp = sectinfo_dispatch.getContentFromID( nSection );
	if( NULL == tmp || stationNum < tmp->nStationNum )
		return 0;

	stationNum = tmp->nStationNum;
	if (stationNum > max_count) stationNum = max_count;

	for(int i=0; i<stationNum; i++)
		stationList[i] = tmp->nStationList[i];

	return stationNum;
}

//车站接收者列表
int CGpcdef::LoadPlanReceiverInfo(char *ptr)
{
	char seps[]=" ,\t\\/=\n";

	int index=0,stationNo=0,match=0;
	char entity[128];CString sEntity;  // 20090908 cuihu change 16 to 64  because TDCOMMSVR  entity name too long
	char *substr;
	int traintype=0;
	
	if(substr=strstr(ptr, "="))
	{
 	    sscanf(substr, "=%d,", &index );		 
		if(index<0) return -1;
	}
    if(substr=strstr(substr, "station_id"))
	{	 	
		substr=strstr(substr, "=");
		sscanf(substr, "=%d,", &stationNo );
	}
	if(substr=strstr(substr, "entity_name"))
	{	 	
		substr=strstr(substr, "=");
		sscanf(substr, "=%s", &entity );	
	}
    sEntity.Format("%s",entity);
	sEntity.Remove(',');
	strcpy(entity,sEntity);
	if(substr=strstr(substr, "train_type"))
	{	 
		substr=strstr(substr, "=");
		sscanf(substr, "=%d,", &traintype );
	}

	std::vector<int> vetEntry;
	if(substr=strstr(substr, "entry"))
	{	 
		substr=strstr(substr, "=");
		//sscanf(substr, "=%d,", &entry );
		CString tmp = substr;
		int n = tmp.Find(',');
		tmp = tmp.Left(n);
		char* str = tmp.GetBuffer();

		char seps[]=" \t\\/=\n";
		char* token = strtok(str,seps);
		while(token!=NULL)
		{
		    int entry_no = (int)atoi(token);
			vetEntry.push_back(entry_no);

 			token = strtok(NULL, seps);
		}
	}
  
	std::vector<int> vetExit;
	if(substr=strstr(substr, "exit"))
	{	 
		substr=strstr(substr, "=");
		//sscanf(substr, "=%d,", &exit );
		CString tmp = substr;
		int n = 0;
		int m = tmp.Find(',');
		if(m==-1)
			n = tmp.Find(';');
		else
			n = m;
		tmp = tmp.Left(n);
		char* str = tmp.GetBuffer();

		char seps[]=" \t\\/=\n";
		char* token = strtok(str,seps);
		while(token!=NULL)
		{
		    int exit_no = (int)atoi(token);
			vetExit.push_back(exit_no);

 			token = strtok(NULL, seps);
		}
	}

	if(substr=strstr(substr, "match"))
	{	 	
		substr=strstr(substr, "=");
		sscanf(substr, "=%d,", &match );
	}

	GPCPLANRECEIVER recver;
	
	recver.nindex = index;
	recver.nstationNo = stationNo;
	recver.match=match;
	strncpy(recver.sentity,entity,sizeof(recver.sentity)-1);
   	recver.ntraintye = traintype;
	recver.nEntryExit = 0;
	
	int nEntryExit = (vetEntry.size() >= vetExit.size()) ? vetEntry.size():vetExit.size();
	for(int i=0; i<nEntryExit; i++)
	{
		if(i>=64)
		{
			break;
		}

		int entry=-1, exit=-1;
		if(i < vetEntry.size())
			entry = vetEntry[i];
		else
			entry = -1;

		if(i < vetExit.size())
			exit = vetExit[i];
		else
			exit = -1;

		recver.entry_exit[i].entry = entry;   
		recver.entry_exit[i].exit = exit;
		recver.nEntryExit++;
	}

	m_vctPlanReceicer.push_back(recver);

	return 1;
}
  
int CGpcdef::LoadBasicPlanReceiverInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j, nStart, nEnd;

	BasicPlanReciever rec;

	// load section number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	rec.section = (int)atoi(token);

	// load station list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	int count = (int)atoi(token);
	rec.count=0;
	for(i=0; i<count;) 
	{
		token = strtok(NULL, seps);
		if(token==NULL) 
			return 0;
		if(strchr(token, '-')) 
		{
			sscanf(token, "%d-%d", &nStart, &nEnd);
			for(j=nStart; j<=nEnd; j++)
			{
				rec.reciever[rec.count] = j;
				rec.count++;
				i++;
			}
		} 
		else 
		{
 			rec.reciever[rec.count] = atoi(token);
			rec.count++;
			i++;
		}
	}
	m_basicPlanReciever.push_back(rec);
	return 1;
}

int CGpcdef::GetSectionPlanReceivers(int nSectionNo, int *nRecList)
{
	GPCSECTION *tmp = sectinfo.getContentFromID( nSectionNo );
	if( NULL == tmp  ) 
		return -1;

	int nRecNum=0;
	nRecNum = tmp->nPlanReceiverCount;
	for(int i=0; i<nRecNum; i++)
	{
		nRecList[i] = tmp->nPlanReceiverList[i];
	}
    
	return nRecNum;

}

int CGpcdef::GetStationPlanReceivers(int nStationNo, int *nRecList)
{
	if(nStationNo<0) return -1;
	int nRecNum = 0;
	for(int i=0;i<m_vctPlanReceicer.size();i++)
	{
		if(m_vctPlanReceicer[i].nstationNo != nStationNo)
			continue;
		{
			nRecList[nRecNum] = m_vctPlanReceicer[i].nindex;
			nRecNum++;
		}
	}
 	
	return nRecNum;

}

int CGpcdef::GetReceiverInfo(int nRecIndex,struct GPCPLANRECEIVER &rec)
{
	for (int i=0; i<m_vctPlanReceicer.size(); i++)
	{
		if (m_vctPlanReceicer[i].nindex == nRecIndex)
		{
			rec = m_vctPlanReceicer[i];
			return 0;
		}
	}

     return -1;
}

bool CGpcdef::LoadEntity(char* entitycfg)
{ 
	char  szFileName[256];
	sprintf(szFileName,"%s\\%s",GetDataDir(), entitycfg);
    CIniFile t;
    int theStatus =1;
	do{
		t.OpenFile(szFileName);		
		if(!t.IsOK())
			break;
        if(t.GotoSection("GROUP"))
		{
			
			char key[32],content[256];		
			while(t.GetContent(key,content,256)) 
			{
				int  nGroupId;
				char  express[64];
				char name[64];
				
				nGroupId = atoi(key);
				if(nGroupId <0)
					break;
				if(sscanf(content,"%s  %s",express,name)==2)
				{			 
					strncpy(groupIdInfo[nGroupCnt].sExpression,express,sizeof(groupIdInfo[nGroupCnt].sExpression));
					strncpy(groupIdInfo[nGroupCnt].sName,name,sizeof(groupIdInfo[nGroupCnt].sName));
					groupIdInfo[nGroupCnt].nGroupId = nGroupId;
					nGroupCnt++;
				}
			}
		}
		if(t.GotoSection("ENTITY"))
		{
			char key[64],content[256];		
			while(t.GetContent(key,content,256)) 
			{
				int nEntityId;
				char sExpression[32];
				char sName[32];
				int nGroupId;
				int  nStby;
				int  nSpecial1;
				int  nSpecial2;
								
				nEntityId = atoi(key);
				if(nEntityId <0)
					break;
				if(sscanf(content,"%s %s %d %d %d %d",
					                sExpression,sName,&nGroupId,&nStby,&nSpecial1,&nSpecial2)==6)
				{			 
					entity[nEntityCnt].nEntityId =  nEntityId;
					strcpy(entity[nEntityCnt].sExpression,sExpression);
					strcpy(entity[nEntityCnt].sName,sName);
					entity[nEntityCnt].nGroupId = nGroupId;
					entity[nEntityCnt].nStby = nStby;
					entity[nEntityCnt].nSpecial1 = nSpecial1;
					entity[nEntityCnt].nSpecial2 = nSpecial2;

					nEntityCnt++;
					if(nEntityCnt >= 4096)
					{
						sysprint(0,0,"读取配置文件%s失败！",szFileName);
						AfxMessageBox("配置文件entity.cfg读取越界！");
						break;
					}

				}
			}
			
		}
		theStatus = 0;
	}while(0);

    if(theStatus)
		return false;
	else
	return true;
}

//获取所有自律机实体
void CGpcdef::GetLircEnityList(vector<int>& vec)
{
	for(int i = 0 ;i<4095;i++)
	{
		if(entity[i].nGroupId == 102)
			vec.push_back(entity[i].nEntityId);
	}

}
//获取所有rt实体号
void CGpcdef::GetRtEntityList(vector<int>& vec)
{
	for(int i = 0 ; i<4095;i++)
	{		
		if( entity[i].nGroupId==101 )
		{
			if( entity[i].nSpecial1 == 1 || entity[i].nSpecial1 == 0 )
			{
				if(entity[i].nStby==2)//加此判断是为了过滤掉trsterm，因为目前现场配置中有些tsrterm的组号为101
				vec.push_back(entity[i].nEntityId);			
			}
		}
	}

}


bool CGpcdef::GetEntityInfo(int nEntityId, GPCENTITY &entityInfo)
{
   if(nEntityId <0 ) return false;
   for(int i=0;i<nEntityCnt;i++)
   {
	   if(entity[i].nEntityId ==nEntityId)
	   {
		   entityInfo = entity[i];
		   return true;
	   }
   }
   if(i>=nEntityCnt)
	   return false;
   return false;
}

bool CGpcdef::GetEntityInfo(const char *sEntityExpress, GPCENTITY &entityInfo)
{
   if(strlen(sEntityExpress) <0 ) return false;
   for(int i=0;i<nEntityCnt;i++)
   {
	   if(!strcmp(entity[i].sExpression, sEntityExpress) ) ////sExpression
	   {
		   entityInfo = entity[i];
		   return true;
	   }  
   }
   
   if(i>=nEntityCnt)
	   return false;
   return false;
}

bool CGpcdef::GetEntityInfo(GPCENTITY &entityInfo, char *sEntityName)
{
   if(strlen(sEntityName) <0 ) return false;
   for(int i=0;i<nEntityCnt;i++)
   {
	   if(!strcmp(entity[i].sName, sEntityName) ) ////sExpression
	   {
		   entityInfo = entity[i];
		   return true;
	   }  
   }
   
   if(i>=nEntityCnt)
	   return false;
   return false;
}

bool CGpcdef::GetGroupInfo(int nGroupId, GPCGROUP &groupInfo)
{
   if(nGroupId <0 ) return false;
   for(int i=0;i<nGroupCnt;i++)
   {
	   if(groupIdInfo[i].nGroupId == nGroupId)
	   {
		   groupInfo = groupIdInfo[i];
		   return true;
	   }
   }
   if(i>=nGroupCnt)
	   return false;
	return false;
}

bool CGpcdef::GetEntityInfo(int nGroupid, int nStNo, GPCENTITY &entityInfo)
{
	// add by zzg in nanchang 20081209---如果找不到，则返回错误
	memset(&entityInfo, 0x00, sizeof(entityInfo));

	if(nGroupid <0  || nStNo<=0) return false;
	for(int i=0;i<nEntityCnt;i++)
	{
		if(entity[i].nGroupId ==nGroupid 
			&& entity[i].nSpecial1 ==nStNo )
		{
			entityInfo = entity[i];
			return true;
		}
	}
	return false;
}

bool CGpcdef::GetGpcEntityInfo(int gpcid, GPCENTITY &entityInfo)
{
	// add by zzg in nanchang 20081209---如果找不到，则返回错误
	memset(&entityInfo, 0x00, sizeof(entityInfo));

	for(int i=0;i<nEntityCnt;i++)
	{
		if((entity[i].nGroupId == GPC_DISPACHER 
			|| entity[i].nGroupId == GPC_OPERATER
			|| entity[i].nGroupId == GPC_VICEDISPACHER
			|| entity[i].nGroupId == SYSTEM_ADM)
			&& entity[i].nSpecial2 ==gpcid )
		{
			entityInfo = entity[i];
			return true;
		}
	}
	return false;
}

// 根据特征查找ENTITY, 主要用于查找合适的服务器进程 
bool CGpcdef::GetEntityInfo(int nGroupid, int special1,int special2, GPCENTITY &entityInfo)
{
	memset(&entityInfo, 0x00, sizeof(entityInfo));

	for(int i=0;i<nEntityCnt;i++)
	{
		if(entity[i].nGroupId ==nGroupid 
			&& entity[i].nSpecial1 ==special1
			&& entity[i].nSpecial2 ==special2)
		{
			entityInfo = entity[i];
			return true;
		}
	}
	return false;
}


bool CGpcdef::GetEntityInfobyName(char *sEntityName, GPCENTITY &entityInfo)
{ 
	if(strlen(sEntityName) <0 ) return false;
	for(int i=0;i<nEntityCnt;i++)
	{
		if(!strcmp(entity[i].sName, sEntityName) ) ////sExpression
		{
			entityInfo = entity[i];
			return true;
		}  
	}
	
	return false;

}

BOOL CGpcdef::LoadSingleProfile(char* szFileName)
{
	// set default value
	m_nShowSideNotConfirmed=0;
	m_nNewBasicDump=0;
	m_nEnablePredict=0;
	m_nEnableMMPlanFromCAD=0;
	m_nEnableBasicDumpTime=0;
	m_nTgOnce=0;
	m_bmpbitdeep = 16;
	m_nNeedSynDataToGpc=0;
	nTenicalStop = 0;
	m_nDcmdVisualHours = 24;
	m_nDcmdReqHours = 48;
	
	m_nBasicSchedulePrio = 0;
	m_nUpdateTGDataMemory = 0;
	m_nPreSchdNum=5;
	m_nPrintElecTrainWidget = 1; // 默认打印
	m_nDcmdAuthorType = 0;
	m_nDcmdSendMode = 2; // 默认择机下发

	// 默认列控只读权限
	m_nTCCReadOnly = 0; // 0: 只读(查询) 1: 可读写(查询\编辑\删除)
    m_nTCCExecute = 0;  // 0: 不能执行   1: 可以执行
	// read config
	FILE *fp=NULL;
	int result=1;
	char *ptr, strLine[512];
	
	if((fp=fopen(szFileName, "r"))==NULL)
		return 0;
	// read file
	while (!feof(fp))	{
		ptr = fgets(strLine, sizeof(strLine), fp);
		if (ptr == NULL) 
			continue;
		if(strstr(ptr, "[POSITION] = 1") )
		{int i=0;		}
		while (*ptr == ' ' || *ptr == '\t')
			ptr ++;
		if (*ptr == ';')	
			continue;
		if (*ptr == '\n')	
			continue;
		
		if (*ptr == '[') 
		{ // compared to keyword 
            
			if (!strncmp(ptr, GPCNO_ONLY_ONE_TG, strlen(GPCNO_ONLY_ONE_TG)))
				result=LoadIntValue(ptr+strlen(GPCNO_ONLY_ONE_TG), m_nTgOnce);

			else if (!strncmp(ptr, GPCNO_WINTG_SET_MSS_PORT, strlen(GPCNO_WINTG_SET_MSS_PORT)))
				result=LoadIntValue(ptr+strlen(GPCNO_WINTG_SET_MSS_PORT), m_nMSSAlarm);

			else if (!strncmp(ptr, GPCNO_WINTG_NOSYN_TIME, strlen(GPCNO_WINTG_NOSYN_TIME)))
			{
				int value=0;
				result=LoadIntValue(ptr+strlen(GPCNO_WINTG_NOSYN_TIME), value);
				if(value>=1)
					m_nNosynTimeoutTime=value*60;
			}
			else if (!strncmp(ptr, GPCNO_LAYOUT_SET_MSS_PORT, strlen(GPCNO_LAYOUT_SET_MSS_PORT)))
				result=LoadIntValue(ptr+strlen(GPCNO_LAYOUT_SET_MSS_PORT), m_nMSSLayoutAlarm);

			else if (!strncmp(ptr, GPCNO_ALARM_SOURCE, strlen(GPCNO_ALARM_SOURCE)))
				m_AlarmFromSVR = atoi(ptr+strlen(GPCNO_ALARM_SOURCE)+1);
			else if (!strncmp(ptr, GPCNO_ROUTE_SOURCE, strlen(GPCNO_ROUTE_SOURCE)))
				m_RouteFromSVR = atoi(ptr+strlen(GPCNO_ROUTE_SOURCE)+1);
	
			else if (!strncmp(ptr, GPCNO_RUNTIME_COMPARE_WITH_BASIC, strlen(GPCNO_RUNTIME_COMPARE_WITH_BASIC)))
				m_nRunFastTimeCompareWithBasic = atoi(ptr+strlen(GPCNO_RUNTIME_COMPARE_WITH_BASIC)+1);

			else if (!strncmp(ptr, GPCNO_ROUTELIST_FONTSIZE, strlen(GPCNO_ROUTELIST_FONTSIZE)))
				m_RouteListFontsize = atoi(ptr+strlen(GPCNO_ROUTELIST_FONTSIZE)+1);
 
			else if (!strncmp(ptr, GPCNO_TG_CTRL, strlen(GPCNO_TG_CTRL)))
				result=LoadViewSchdTG(ptr+strlen(GPCNO_TG_CTRL));

			else if (!strncmp(ptr, GPC_DEFAULT, strlen(GPC_DEFAULT)))    //缺省用户信息 [DEFAULT]
				result=LoadDefaultInfo(ptr+strlen(GPC_DEFAULT));
			
            else if (!strncmp(ptr, GPC_LAYOUT_POS, strlen(GPC_LAYOUT_POS)))    //Layout的显示区域 [LAYOUTPOS]
				result=LoadLayoutPos(ptr+strlen(GPC_LAYOUT_POS));
            else if (!strncmp(ptr, GPC_LAYOUT_POS_STA, strlen(GPC_LAYOUT_POS_STA)))    //Layout的显示区域 [LAYOUTPOS]
				result=LoadLayoutPosForSingleStation(ptr+strlen(GPC_LAYOUT_POS_STA));

			else if (!strncmp(ptr, GPC_SCREENINFO, strlen(GPC_SCREENINFO))) 
				m_nScreenNum = atoi(ptr+strlen(GPC_SCREENINFO)+1);
            else if (!strncmp(ptr, GPC_PICTURESTIP, strlen(GPC_PICTURESTIP))) 
				m_nPicturesTip = atoi(ptr+strlen(GPC_PICTURESTIP)+1);
			else if (!strncmp(ptr, GPC_PRINTFORMAT, strlen(GPC_PRINTFORMAT))) 
				m_printFormat = atoi(ptr+strlen(GPC_PRINTFORMAT)+1);
			else if (!strncmp(ptr, GPC_TRAINIDFORMAT, strlen(GPC_TRAINIDFORMAT))) 
				m_TrainIDFormat = atoi(ptr+strlen(GPC_TRAINIDFORMAT)+1);
					
		    else if (!strncmp(ptr, GPC_TRAIN_LEAP_LINE_FLAG, strlen(GPC_TRAIN_LEAP_LINE_FLAG)))  //sunjf 2005 06 17
				m_nTrainLeapLineFlag  = atoi(ptr+strlen(GPC_TRAIN_LEAP_LINE_FLAG)+1);
			
			else if (!strncmp(ptr, GPC_LINE_WIDTH, strlen(GPC_LINE_WIDTH)))  //sunjf 2005 06 17
				m_nLineWidth  = atoi(ptr+strlen(GPC_LINE_WIDTH)+1);	
		    else if (!strncmp(ptr,GPC_DISPATCHERCOMMANDSAVE,strlen(GPC_DISPATCHERCOMMANDSAVE)))  //调度命令的保存目录
				result=LoadDispatcherCommandSaveInfo(ptr+strlen(GPC_DISPATCHERCOMMANDSAVE)); 
			 
		    else if (!strncmp(ptr,GPC_ROUTE_LIST_VIEW_CHANGE_BY_STATION,strlen(GPC_ROUTE_LIST_VIEW_CHANGE_BY_STATION)))  //进路序列的车站随站场画面的变化
				result=LoadRouteViewIfChangebyStation(ptr+strlen(GPC_ROUTE_LIST_VIEW_CHANGE_BY_STATION)); 
			else if (!strncmp(ptr,GPC_TENICAL_STOP_DRAWWAY,strlen(GPC_TENICAL_STOP_DRAWWAY)))  //技术停点的绘制方式
				result=LoadIntValue(ptr+strlen(GPC_TENICAL_STOP_DRAWWAY), nTenicalStop); 
			
			// 是否能够从服务器获取施工和特运
			else if (!strncmp(ptr, GPCNO_ENABLE_MMPLAN_SVR, strlen(GPCNO_ENABLE_MMPLAN_SVR)))
				result=LoadIntValue(ptr+strlen(GPCNO_ENABLE_MMPLAN_SVR), m_nEnableMMPlanFromCAD); 
			
			// 是否有权限修改实际点
			else if (!strncmp(ptr, GPCNO_LIMIT_MODIFY_ACTUAL_TIME, strlen(GPCNO_LIMIT_MODIFY_ACTUAL_TIME)))
				result=LoadLimitModifyActualTime(ptr+strlen(GPCNO_LIMIT_MODIFY_ACTUAL_TIME)); 
	
			// 是否有权限办理闭塞
			else if (!strncmp(ptr, GPCNO_PREDICT, strlen(GPCNO_PREDICT)))
				result=LoadIntValue(ptr+strlen(GPCNO_PREDICT), m_nEnablePredict); 

			// 是否启用新基本图转储
			else if (!strncmp(ptr, GPCNO_BASIC_SCHEDULE_DUMP, strlen(GPCNO_BASIC_SCHEDULE_DUMP)))
				result=LoadIntValue(ptr+strlen(GPCNO_BASIC_SCHEDULE_DUMP), m_nNewBasicDump);

			// 显示车站未确认股道
			else if (!strncmp(ptr, GPCNO_SHOW_SIDE_NOT_CONFIRM, strlen(GPCNO_SHOW_SIDE_NOT_CONFIRM)))
				result=LoadIntValue(ptr+strlen(GPCNO_SHOW_SIDE_NOT_CONFIRM), m_nShowSideNotConfirmed);
			
			// 是否启用定时更换基本图
			else if (!strncmp(ptr, GPCNO_BASIC_SCHEDULE_DUMP_TIME, strlen(GPCNO_BASIC_SCHEDULE_DUMP_TIME)))
				result=LoadIntValue(ptr+strlen(GPCNO_BASIC_SCHEDULE_DUMP_TIME), m_nEnableBasicDumpTime);
				
			else if (!strncmp(ptr, GPCNO_BASICSCHEDULEPRIORITY, strlen(GPCNO_BASICSCHEDULEPRIORITY)))  // 是否有权限修改基本图 (是:1 否:0)
				result=LoadIntValue(ptr+strlen(GPCNO_BASICSCHEDULEPRIORITY), m_nBasicSchedulePrio, m_nUpdateTGDataMemory); 
	
			else if (!strncmp(ptr, GPCNO_BASICSCHEDULESYN, strlen(GPCNO_BASICSCHEDULESYN)))  // 是否同步数据到GPC内存 (是:1 否:0)
				result=LoadIntValue(ptr+strlen(GPCNO_BASICSCHEDULESYN), m_nNeedSynDataToGpc); 

			else if (!strncmp(ptr, GPCNO_BMPBIT_DEEP, strlen(GPCNO_BMPBIT_DEEP)))  // 是否同步数据到GPC内存 (是:1 否:0)
			{
				result=LoadIntValue(ptr+strlen(GPCNO_BMPBIT_DEEP), m_bmpbitdeep); 
				if(m_bmpbitdeep != 8 && m_bmpbitdeep != 16 && m_bmpbitdeep != 32)
					m_bmpbitdeep = 16;
			}
						
			else if (!strncmp(ptr, GPC_PRINT_ELEC_WIDGET, strlen(GPC_PRINT_ELEC_WIDGET)))      // 配置是否打印电力机车方框
				result=LoadIntValue(ptr+strlen(GPC_PRINT_ELEC_WIDGET), m_nPrintElecTrainWidget);  
			
			else if (!strncmp(ptr, GPCNO_PRESCHDNUM, strlen(GPCNO_PRESCHDNUM)))      // 调度员向前登录班次次数
			{
				result=LoadIntValue(ptr+strlen(GPCNO_PRESCHDNUM), m_nPreSchdNum);  
				if(m_nPreSchdNum>2)
					m_nPreSchdNum=2;
			}
			
			 // 值班主任授权采用何种模式
		   else if (!strncmp(ptr, GPC_DISPATCHERCOMMAND_AUTHORITY_TYPE, strlen(GPC_DISPATCHERCOMMAND_AUTHORITY_TYPE)))      
			   result=LoadIntValue(ptr+strlen(GPC_DISPATCHERCOMMAND_AUTHORITY_TYPE), m_nDcmdAuthorType);
		   // 模板模式值班主任授权模式
		   else if(!strncmp(ptr,GPC_DISPATCHERCOMMAND_AUTHORITY,strlen(GPC_DISPATCHERCOMMAND_AUTHORITY)))
			   result = LoadDCAuthorityInfo(ptr + strlen(GPC_DISPATCHERCOMMAND_AUTHORITY)); 
		   // 值班主任授权下发模式--1: 立即 2: 择机
		   else if(!strncmp(ptr,GPC_DISPATCHERCOMMAND_SEND_MODE,strlen(GPC_DISPATCHERCOMMAND_SEND_MODE)))
			   result=LoadIntValue(ptr+strlen(GPC_DISPATCHERCOMMAND_SEND_MODE), m_nDcmdSendMode);  
		   
		   // 列控执行权限设置
		   else if(!strncmp(ptr,GPC_TCC_DCMD_EXECUTE_RIGHT,strlen(GPC_TCC_DCMD_EXECUTE_RIGHT)))
			   result=LoadTCCRight(ptr+strlen(GPC_TCC_DCMD_EXECUTE_RIGHT));

		   else if(!strncmp(ptr, GPCNO_USE_VIRTUAL_MEMORY, strlen(GPCNO_USE_VIRTUAL_MEMORY)))
		   {
			   int value;
			   result=LoadIntValue(ptr+strlen(GPCNO_USE_VIRTUAL_MEMORY), value); 
			   if(value>0)
				   m_bUseVirtualMemory=true;
		   }
		   else if(!strncmp(ptr, GPCNO_MODIFY_TSR_STATUS, strlen(GPCNO_MODIFY_TSR_STATUS)))
		   {
			   int value;
			   result=LoadIntValue(ptr+strlen(GPCNO_MODIFY_TSR_STATUS), value); 
			   if(value)
				   m_bModifyTsrStatus=true;
		   }
		   else if(!strncmp(ptr, GPCNO_WIDE_PIXEL, strlen(GPCNO_WIDE_PIXEL)))
		   {
			   int value=0;
			   result=LoadIntValue(ptr+strlen(GPCNO_WIDE_PIXEL), value); 
			   if(value>0)
				   m_nWidePixel=value;
		   }
		   else if(!strncmp(ptr, GPCNO_TRAIN_NO_LEVEL_COUNT, strlen(GPCNO_TRAIN_NO_LEVEL_COUNT))) 
		   {
			   int value=0;
			   result=LoadIntValue(ptr+strlen(GPCNO_TRAIN_NO_LEVEL_COUNT), value); 
			   if(value==4 || value==5)
				   m_nTrainNoLevelCount=value;
		   }
		   else if (!strncmp(ptr, GPCNO_TGMSG_TO_GATEWAY, strlen(GPCNO_TGMSG_TO_GATEWAY)))
		   {
			   int value=0;
			   result=LoadIntValue(ptr+strlen(GPCNO_TGMSG_TO_GATEWAY), value); 
			   if(value>0)
				   m_bTgMsgToGateway=true;
		   }
		   else if (!strncmp(ptr, GPCNO_OTHERMSG_TO_GATEWAY, strlen(GPCNO_OTHERMSG_TO_GATEWAY)))
		   {
			   int value=0;
			   result=LoadIntValue(ptr+strlen(GPCNO_OTHERMSG_TO_GATEWAY), value); 
			   if(value>0)
				   m_bOtherMsgToGateway=true;
		   }  
		   else if(!strncmp(ptr, GPCNO_AUTO_MOVE_MODE, strlen(GPCNO_AUTO_MOVE_MODE)))
		   {
			   result=LoadAutoMoveConfig(ptr+strlen(GPCNO_AUTO_MOVE_MODE)); 
		   }
		}
		else 
		{
			CString tmp;
			tmp.Format("gpcno.def ']'或 '['不匹配! %s", ptr);
			AfxMessageBox(tmp);
			fclose(fp);
			return 0;
		}
		if(!result) 
		{
			fclose(fp);
			return 0;
		}
	}
	
	// close gpcno.def 
	fclose(fp);
	
	if(m_nDcmdAuthorType == 1 && m_dcAuthorityNum == 0)
	{
		CString  prompt;
		prompt.Format("配置文件%s中, 值班主任授权采用模版授权模式,但是没有对应配置项%s", szFileName, GPC_DISPATCHERCOMMAND_AUTHORITY);
		sysprint(0,0, "%s",prompt);
		return 0;
	}
	// success return
	return 1;
}

//格式[AUTO_MOVE_MODE]=是否启用自动移动模式(0:不启用,默认值 1:启用)
// [AUTO_MOVE_MODE]=1
int CGpcdef::LoadAutoMoveConfig(char *ptr)
{
	char *token;
	char seps[]=" ,\t/=\n"; 
    
	// 是否启用自动移动模式
	token = strtok(ptr, seps);
	if(token == NULL)
		return 0;	
	int value = atoi(token);
	if(value>0)
	{
		 m_bAutoMoveTime=true;
	}
	else
	{
		 m_bAutoMoveTime=false;
	}

	return 1;
}

int CGpcdef::LoadTCCRight(char* ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	m_nDirectExecute=0;

	token = strtok(ptr, seps);
	if(token==NULL) 
	{
		m_nTCCReadOnly = 0;
	}
	else
	{
		m_nTCCReadOnly = atoi(token);
	}

	token = strtok(NULL, seps);
	if(token == NULL)
	{
		m_nTCCExecute = 0;
	}
	else
	{
		m_nTCCExecute = atoi(token);
	}

	token = strtok(NULL, seps);
	if(token == NULL)
	{
		m_nDirectExecute = 0;
	}
	else
	{
		m_nDirectExecute = atoi(token);
	}

	return TRUE;
}

BOOL CGpcdef::LoadPublicProfile(char* szFileName, char* EntityName)
{
	FILE *fp=NULL;
	int result=1;
	char *ptr, strLine[2048];
    
	m_nDCmdNoReqMsgType = 0;
	m_nDCmdMenuStytle = 0;

	memset(m_strSTIName, 0, sizeof(m_strSTIName));
	memset(m_strSTIPath, 0, sizeof(m_strSTIPath));
	memset(m_tcc_cfg, 0, sizeof(m_tcc_cfg));
	
 	if((fp=fopen(szFileName, "r"))==NULL)
		return 0;
	// read file
	INT	lineNo = 0;
 	while (!feof(fp))	{
		lineNo ++;
		memset(strLine, 0, sizeof(strLine));
		ptr = fgets(strLine, 2048, fp);
		if (ptr == NULL) 
		continue;
		while (*ptr == ' ' || *ptr == '\t')
			ptr ++;
		if (*ptr == ';' || *ptr == 0 || *ptr == '\n')
			continue;

		if (*ptr == '[') { // compared to keyword
            //PLOTITEM
			if (!strncmp(ptr, GPC_USER, strlen(GPC_USER)))                //用户列表信息 [USER]
			   	result=LoadUserInfo(ptr+strlen(GPC_USER));

			else if(!strncmp(ptr, GPC_YUGAO_STATION, strlen(GPC_YUGAO_STATION)))    //预告车站配置  
			{
				result=LoadYugaoStation(ptr+strlen(GPC_YUGAO_STATION));
			}
			else if (!strncmp(ptr, GPC_TG_PIC_SAVE, strlen(GPC_TG_PIC_SAVE)))    //打印的配置信息  
			{
				result=LoadFtpInfo(ptr+strlen(GPC_TG_PIC_SAVE));
			}

			else if (!strncmp(ptr, GPC_STI_NAME, strlen(GPC_STI_NAME)))  
			{
				char *token;
				char seps[]=" ,\t\\/=\n";
 
				memset(m_strSTIName, 0, sizeof(m_strSTIName));
				ptr = ptr+strlen(GPC_STI_NAME);
				token = strtok(ptr, seps);
				if(token==NULL) 
					result=0;
				else
				{
					result=1;
					strncpy(m_strSTIName, token, sizeof(m_strSTIName)-1);
					m_strSTIName[sizeof(m_strSTIName)-1]=0;
				}
			}

			else if (!strncmp(ptr, GPC_STI_PATH, strlen(GPC_STI_PATH)))
			{
				char *token;
				char seps[]=" \t=\n";
 
				memset(m_strSTIPath, 0, sizeof(m_strSTIPath));
				ptr = ptr+strlen(GPC_STI_PATH);
				token = strtok(ptr, seps);
				if(token==NULL) 
					result=0;
				else
				{
					result=1;
					strncpy(m_strSTIPath, token, sizeof(m_strSTIPath)-1);
					m_strSTIPath[sizeof(m_strSTIPath)-1]=0;
				}
			}
			else if (!strncmp(ptr, GPC_SWITCH_STATION, strlen(GPC_SWITCH_STATION)))
				result=LoadSectionSwitchStation(ptr+strlen(GPC_SWITCH_STATION));
							
			else if (!strncmp(ptr, GPC_NOT_SHOW_SIDE_CTC_STATION, strlen(GPC_NOT_SHOW_SIDE_CTC_STATION)))
				result=LoadNotShowSideCtcStation(ptr+strlen(GPC_NOT_SHOW_SIDE_CTC_STATION));
			
			else if (!strncmp(ptr, GPC_PLOTITEM, strlen(GPC_PLOTITEM)))    //打印的配置信息  
			   	result=LoadPlotItemInfo(ptr+strlen(GPC_PLOTITEM));

		    else if (!strncmp(ptr, GPC_SECTIONPLOTITEM, strlen(GPC_SECTIONPLOTITEM)))    //打印的配置信息  
			   	result=LoadSectionPlotInfo(ptr+strlen(GPC_SECTIONPLOTITEM));

            else if (!strncmp(ptr, GPC_STATIONENTRY, strlen(GPC_STATIONENTRY)))   //车站接入,交出口的列表
			   	result=LoadStationEntryInfo(ptr+strlen(GPC_STATIONENTRY)); 

			else if (!strncmp(ptr, GPC_INTERFACESTATION, strlen(GPC_INTERFACESTATION)))   //分界口车站列表
			   	result=LoadInterfaceStationInfo(ptr+strlen(GPC_INTERFACESTATION)); 

			else if (!strncmp(ptr, GPC_ERRORSIDESTATION, strlen(GPC_ERRORSIDESTATION)))   //设置问股的车站列表
				result=LoadErrorSideStationInfo(ptr+strlen(GPC_ERRORSIDESTATION));
			
			else if (!strncmp(ptr, GPC_NOT_THIS_STATION, strlen(GPC_NOT_THIS_STATION)))   //设置非本台车站
				result=LoadNotThisSectionStationInfo(ptr+strlen(GPC_NOT_THIS_STATION));
			
			else if (!strncmp(ptr, GPC_SECTION_PHASE_STORE_STATION, strlen(GPC_SECTION_PHASE_STORE_STATION)))   //区间暂存车站信息***
		     	result=LoadSectionPhaseStoreStation(ptr+strlen(GPC_SECTION_PHASE_STORE_STATION)); 
	
			else if (!strncmp(ptr, GPC_SECTION, strlen(GPC_SECTION)))
				result=LoadSectInfo(ptr+strlen(GPC_SECTION));

			// 增加 沈阳局特殊处理 多个调度台合并 以GPC号确认阶段计划车站范围
			else if (!strncmp(ptr, GPC_SPECIAL_STPLAN, strlen(GPC_SPECIAL_STPLAN)))
				result=LoadSpecialGPCStplanInfo(ptr+strlen(GPC_SPECIAL_STPLAN));
			else if (!strncmp(ptr, GPC_SPECIAL_STPLAN_GROUP, strlen(GPC_SPECIAL_STPLAN_GROUP)))
				result=LoadSpecialGPCStplanGroup(ptr+strlen(GPC_SPECIAL_STPLAN_GROUP));

			else if (!strncmp(ptr, GPC_SECNEIGHBOR, strlen(GPC_SECNEIGHBOR)))
				result=LoadSecNeighborInfo(ptr+strlen(GPC_SECNEIGHBOR));
			//发邻台的台间站详细配置
           else if (!strncmp(ptr, GPC_SECNEIGHBOR_RECEIVER, strlen(GPC_SECNEIGHBOR_RECEIVER)))
				result=LoadSecNeighborReceiverInfo(ptr+strlen(GPC_SECNEIGHBOR_RECEIVER));
			//gpc plan receiver 
			else if (!strncmp(ptr, GPC_PLAN_RECEIVER, strlen(GPC_PLAN_RECEIVER)))
				result=LoadPlanReceiverInfo(ptr+strlen(GPC_PLAN_RECEIVER));		 

			else if (!strncmp(ptr, GPC_SECTION_BASIC_PLAN, strlen(GPC_SECTION_BASIC_PLAN)))
				result=LoadBasicPlanReceiverInfo(ptr+strlen(GPC_SECTION_BASIC_PLAN));	

			else if (!strncmp(ptr, GPC_SECTION_DISPTACH, strlen(GPC_SECTION_DISPTACH)))
				result=LoadDispatchSectInfo(ptr+strlen(GPC_SECTION_DISPTACH));

			else if (!strncmp(ptr, GPC_SECTIONROUTE, strlen(GPC_SECTIONROUTE)))
				result=LoadSectRouteInfo(ptr+strlen(GPC_SECTIONROUTE));

			else if (!strncmp(ptr, GPC_DISPSECTION, strlen(GPC_DISPSECTION))) 
				result=LoadDispSectInfo(ptr+strlen(GPC_DISPSECTION));

			else if (!strncmp(ptr, GPC_DISPSECTION_DOMINATION, strlen(GPC_DISPSECTION_DOMINATION))) 
				result=LoadDispSectDominInfo(ptr+strlen(GPC_DISPSECTION_DOMINATION));
			
			else if (!strncmp(ptr, GPC_SCREENINFO, strlen(GPC_SCREENINFO))) 
				m_nScreenNum = atoi(ptr+strlen(GPC_SCREENINFO)+1);

			else if (!strncmp(ptr,GPC_DISPATCHERCOMMAND,strlen(GPC_DISPATCHERCOMMAND)))
				result=LoadDispatcherCommandInfo(ptr+strlen(GPC_DISPATCHERCOMMAND));
	        else if (!strncmp(ptr,GPC_DISPATCHERCOMMANDSAVE,strlen(GPC_DISPATCHERCOMMANDSAVE)))
				result=LoadDispatcherCommandSaveInfo(ptr+strlen(GPC_DISPATCHERCOMMANDSAVE));
		    else if (!strncmp(ptr, GPC_BUREAUNAME, strlen(GPC_BUREAUNAME))) 
				sscanf(ptr+strlen(GPC_BUREAUNAME)+1, "%s\n",szBureauName);
			else if (!strncmp(ptr, GPC_REMOTEPATH, strlen(GPC_REMOTEPATH))) 
				sscanf(ptr+strlen(GPC_REMOTEPATH)+1, "%s\n", pbinfo.abRemotePlaybackPath);
			else if (!strncmp(ptr, GPC_REMOTEPATH_ROUTE, strlen(GPC_REMOTEPATH_ROUTE))) 
				sscanf(ptr+strlen(GPC_REMOTEPATH_ROUTE)+1, "%s\n", pbinforoute.abRemotePlaybackPath);

			//add by cjr 回放问题修改
			else if (!strncmp(ptr, GPC_PBSHAREDIR, strlen(GPC_PBSHAREDIR))) 
			{
				sscanf(ptr+strlen(GPC_PBSHAREDIR)+1, "%s\n", pbinforoute.mPbShareDir);
				sscanf(ptr+strlen(GPC_PBSHAREDIR)+1, "%s\n", pbinfo.mPbShareDir);
			}

			else if (!strncmp(ptr, GPC_CAD_OS_SYSTEM, strlen(GPC_CAD_OS_SYSTEM))) 
 				m_usCadOsSystemFlag = atoi(ptr+strlen(GPC_CAD_OS_SYSTEM)+1);

			else if (!strncmp(ptr, GPC_LOCALPATH, strlen(GPC_LOCALPATH))) 
			{
				sscanf(ptr+strlen(GPC_LOCALPATH)+1, "%s\n", pbinfo.abLocalPlaybackPath);
				strcpy(pbinforoute.abLocalPlaybackPath,pbinfo.abLocalPlaybackPath);				
			}
			else if (!strncmp(ptr, GPC_SERVERADDR, strlen(GPC_SERVERADDR))) 
				result = LoadServerInfo(ptr + strlen(GPC_SERVERADDR));
			else if (!strncmp(ptr, GPC_SERVERADDR_ROUTE, strlen(GPC_SERVERADDR_ROUTE))) 
				result = LoadRouteServerInfo(ptr + strlen(GPC_SERVERADDR_ROUTE));
			
			else if (!strncmp(ptr, GPC_REPLAYSVRUP_ROUTE, strlen(GPC_REPLAYSVRUP_ROUTE)))
				result = LoadRouteReplayUserAndPasswd(ptr + strlen(GPC_REPLAYSVRUP_ROUTE));

			else if (!strncmp(ptr, GPC_REPLAYSVRUP, strlen(GPC_REPLAYSVRUP)))
				result = LoadReplayUserAndPasswd(ptr + strlen(GPC_REPLAYSVRUP));

			else if(!strncmp(ptr, GPC_LOGLOCALPATH, strlen(GPC_LOGLOCALPATH)))
				sscanf(ptr+strlen(GPC_LOGLOCALPATH)+1, "%s\n", logSvrInfo.abLocalLogPath);
			else if(!strncmp(ptr, GPC_LOGSVRADDR, strlen(GPC_LOGSVRADDR)))
				result = LoadLogSvrAddr(ptr + strlen(GPC_LOGSVRADDR));
			else if(!strncmp(ptr, GPC_LOGSVRPATH, strlen(GPC_LOGSVRPATH)))
				sscanf(ptr+strlen(GPC_LOGSVRPATH)+1, "%s\n", logSvrInfo.abRemoteLogSvrPath);
			else if(!strncmp(ptr, GPC_LOGSVRUP, strlen(GPC_LOGSVRUP)))
				result = LoadLogSvrUp(ptr + strlen(GPC_LOGSVRUP));
	
  			else if (!strncmp(ptr, GPC_ROUTE, strlen(GPC_ROUTE))) 
   				result=LoadRouteInfo(ptr+strlen(GPC_ROUTE));
 			else if (!strncmp(ptr, GPC_STATION_SPACE, strlen(GPC_STATION_SPACE))) //LYC
 				result=LoadStationSpaceInfo(ptr+strlen(GPC_STATION_SPACE));
			else if (!strncmp(ptr, GPC_SPSTATION, strlen(GPC_SPSTATION))) //LYC
				result=LoadSPStationInfo(ptr+strlen(GPC_SPSTATION));
			
			else if(!strncmp(ptr, GPC_PLOTTERNAME, strlen(GPC_PLOTTERNAME)))
				sscanf(ptr+strlen(GPC_PLOTTERNAME)+1, "%s\n", szPlotterName);
			else if(!strncmp(ptr, GPC_PRINTERNAME,strlen(GPC_PRINTERNAME)))
				sscanf(ptr + strlen(GPC_PRINTERNAME) + 1,"%s\n", szPrinterName);
			//太原 20101106 为了获取完整的打印机名称（即包含空格的）
			else if(!strncmp(ptr, GPC_PRINTERNAME_TY,strlen(GPC_PRINTERNAME_TY)))
			{
				char *sPterSeps = "=\n";
				char *sPterNameTmp = strtok(ptr, sPterSeps);
				sPterNameTmp = strtok(NULL, sPterSeps);
				memset(szPrinterName,0,sizeof(szPrinterName));
				strncpy(szPrinterName, sPterNameTmp, sizeof(szPrinterName));
			}

			else if(!strncmp(ptr, GPC_DAPING_SERVER,strlen(GPC_DAPING_SERVER)))
				result = GetDapingServerAddr.loadContent(ptr+strlen(GPC_DAPING_SERVER));
			else if(!strncmp(ptr, GPC_WINVNC_OPTION,strlen(GPC_WINVNC_OPTION)))
				result = GetWinvncOptions.loadContent(ptr+strlen(GPC_WINVNC_OPTION));
			else if(!strncmp(ptr, GPC_TMIS_SERVER_NAME,strlen(GPC_TMIS_SERVER_NAME)))
				result = GetTmisServerName.loadContent(ptr+strlen(GPC_TMIS_SERVER_NAME));
			else if(!strncmp(ptr, GPC_SERVER_OS,strlen(GPC_SERVER_OS)))
				result = IsHighByteFirst.loadContent(ptr+strlen(GPC_SERVER_OS));
			else if(!strncmp(ptr, GPC_PURETEXTSTATION,strlen(GPC_PURETEXTSTATION)))
				result = IsPureTextStation.loadContent(ptr+strlen(GPC_PURETEXTSTATION));

			else if	(!strncmp(ptr, GPC_STATION, strlen(GPC_STATION))) 
				result=stainfo.loadContent(ptr+strlen(GPC_STATION));

			else if(!strncmp(ptr, GPC_DIRECTIONSTRING, strlen(GPC_DIRECTIONSTRING)))
				result = dirStringInfo.loadContent(ptr + strlen(GPC_DIRECTIONSTRING));
			else if(!strncmp(ptr, GPC_SIGNALRADIUS, strlen(GPC_SIGNALRADIUS)))
				result = signalRadius.loadContent(ptr + strlen(GPC_SIGNALRADIUS));
			else if(!strncmp(ptr, GPC_LINETHICKNESS, strlen(GPC_LINETHICKNESS)))
				result = lineThickness.loadContent(ptr + strlen(GPC_LINETHICKNESS));
			else if(!strncmp(ptr,GPC_PAGE,strlen(GPC_PAGE)))
				result = pageinfo.loadContent(ptr + strlen(GPC_PAGE));
		   else if(!strncmp(ptr,GPC_ELECTRONIC_MODULE,strlen(GPC_ELECTRONIC_MODULE)))
				result = LoadEleModule(ptr + strlen(GPC_ELECTRONIC_MODULE));


		   else if(!strncmp(ptr,TCC_CMDID_REQUIRE_SERVER_CONFIRM,strlen(TCC_CMDID_REQUIRE_SERVER_CONFIRM)))
			   result = LoadTccCmdIDRequireServerConfirm(ptr + strlen(TCC_CMDID_REQUIRE_SERVER_CONFIRM));

		   else if(!strncmp(ptr,TCC_VERSION,strlen(TCC_VERSION)))
			   result = LoadTccVersion(ptr + strlen(TCC_VERSION));

		   // 调度命令号码请求使用的消息类型
		   else if (!strncmp(ptr, GPC_DCMD_NO_REQ_MSG_TYPE, strlen(GPC_DCMD_NO_REQ_MSG_TYPE)))      
			   result=LoadIntValue(ptr+strlen(GPC_DCMD_NO_REQ_MSG_TYPE), m_nDCmdNoReqMsgType);  

		   else if(!strncmp(ptr,TCC_DEFINE,strlen(TCC_DEFINE)))
			   result = LoadTccDefine(ptr + strlen(TCC_DEFINE));
		   else if(!strncmp(ptr,INTERFACESTATION_EXIT_ENTRY,strlen(INTERFACESTATION_EXIT_ENTRY)))
			   result = LoadInterfaceExitEntry(ptr + strlen(INTERFACESTATION_EXIT_ENTRY));
			else if (!strncmp(ptr, GPC_ROUTELIST_EXECUTE_FORCE_ENABLE, strlen(GPC_ROUTELIST_EXECUTE_FORCE_ENABLE)))
			{
				int val = 0;
				result=LoadIntValue(ptr+strlen(GPC_ROUTELIST_EXECUTE_FORCE_ENABLE), val);
				if(val != 1)
					m_bRouteListExecuteForceEnable = false;
			}
			//按站报警设置密码
			else if (!strncmp(ptr,ALARMFILTER_PASSWORD,strlen(ALARMFILTER_PASSWORD)))
			{
				GpcDataTmpl<CString>	linePwd;
				linePwd.loadContent(ptr+strlen(ALARMFILTER_PASSWORD));
				strcpy(m_szAlarmFilterPassword, linePwd.getContent());
			}
			else if (!strncmp(ptr,ALARMFILTER_BYSTA,strlen(ALARMFILTER_BYSTA)))
			{
				GpcDataTmpl<int>	linePwd;
				linePwd.loadContent(ptr+strlen(ALARMFILTER_BYSTA));
				m_nAlarmFilterBySta = linePwd.getContent();
			}
			else if (!strncmp(ptr,GPC_DRAGWINDOW_SHOW,strlen(GPC_DRAGWINDOW_SHOW)))
			{
				GpcDataTmpl<int>	linePwd;
				linePwd.loadContent(ptr+strlen(GPC_DRAGWINDOW_SHOW));
				m_bDragWindowShow = linePwd.getContent();
			}
			else if(!strncmp(ptr,GPC_DRAGWINDOW_DEFAULT,strlen(GPC_DRAGWINDOW_DEFAULT)))
			{
				GpcDataTmpl<int>	linePwd;
				linePwd.loadContent(ptr+strlen(GPC_DRAGWINDOW_DEFAULT));
				m_bDragWindowEnable = linePwd.getContent();
			}
			else if (!strncmp(ptr, GPC_HILC_MODE, strlen(GPC_HILC_MODE)))
				m_HilcMode = atoi(ptr+strlen(GPC_HILC_MODE)+1);
			else if (!strncmp(ptr, GPC_ROUTE_TRIG_LAST, strlen(GPC_ROUTE_TRIG_LAST)))
				m_nRouteTrigLast = atoi(ptr+strlen(GPC_ROUTE_TRIG_LAST)+1);
			else
		   {
			   CString tmp = strLine;
			   if(tmp.GetLength() > 200)
				   tmp = "";
			   sysprint(0,0,"warning gpc.def中%d行遇到未处理的配置:", lineNo, tmp);
			   sysprint(0,0,tmp.GetBuffer());
			   //result = 0;
		   }
		}
		else
			result = 0;
		if(!result)
		{
			CString  prompt;
			prompt.Format("配置文件%s第 %d 行有错", szFileName, lineNo);
			AfxMessageBox(prompt);
			fclose(fp);
			return 0;
		}
	}

	// close gpc.def 
	fclose(fp);
	
	if(!LoadEntity(EntityName))
	{
		AfxMessageBox("加载用户配置文件(entity.cfg)失败, 系统退出",MB_OK);
		CString str;
		str.Format("实体配置文件:%s", EntityName);
		AfxMessageBox(str,MB_OK);
		return 0;
	}
	// success return
	return 1; 
}


LPCSTR CGpcdef::GetSectionNameOfDisp(int nSectionID)
{
	GPCDISPSECTINFO *tmp = dispsectinfo.getContentFromID( nSectionID );
	
	if(NULL == tmp)
		return NULL;
	
	return tmp->lpszSectionName;
}

LPCSTR CGpcdef::GetSectionNameOfDispDomain(int nSectionID)
{
	GPCSECTION *tmp = dispdominsectinfo.getContentFromID( nSectionID );
	
	if(NULL == tmp)
		return NULL;
	
	return tmp->szSectionName;
}

int CGpcdef::GetSectionListOfDisp(vector<int>& sec_list)
{
	sec_list.clear();
	sec_list = userinfo[nUserIndex].layout_section_list;
	return sec_list.size();
}

 
int CGpcdef::GetTmisLoginShiftid()      //获取登陆班别
{
    return  userinfo[nUserIndex].gTmisLoginShiftid;

    //return userinfo[nUserIndex]->gLoginShiftid;
}
void CGpcdef::SetTmisLoginShiftid(int inumber)     //设置登陆班别
{
    userinfo[nUserIndex].gTmisLoginShiftid=inumber;

    //userinfo[nUserIndex]->gLoginShiftid=inumber;
}
int CGpcdef::GetLoginType()
{
	return userinfo[nUserIndex].nLoginType;	
	//return m_loginType;

}
int CGpcdef::GetPrintFormat()
{
	return m_printFormat;

}
 
 int CGpcdef::GetNeighberSectionPropertyList(int nCurrSectionNo,int& nNeighberSectionNum, int* nNeighberSectionPropertyList)
{
	if( NULL == nNeighberSectionPropertyList )
		return 0;

	GPCSECTION *tmp = sectinfo.getContentFromID( nCurrSectionNo );
	if( NULL==tmp)// || nNeighberSectionNum < tmp->nNeighberCount )
		return 0;

 	nNeighberSectionNum = tmp->nNeighberCount;
	for(int i=0; i<nNeighberSectionNum; i++)
		nNeighberSectionPropertyList[i] = tmp->nNeighberPropertyList[i];

  	return nNeighberSectionNum;
}
 

int CGpcdef::GetNeighberViewPlanSectionList(int nCurrSectionNo,int& nNeighberSectionNum, int* nNeighberSectionList)
{
	if( NULL == nNeighberSectionList )
		return 0;

	GPCSECTION *tmp = sectinfo.getContentFromID( nCurrSectionNo );
	if( NULL==tmp) 
		return 0;

 	nNeighberSectionNum = tmp->nViewPlanNeighberCount;
	for(int i=0; i<nNeighberSectionNum; i++)
		nNeighberSectionList[i] = tmp->nViewPlanNeighberList[i];

  	return nNeighberSectionNum;
}

int CGpcdef::GetNeighberViewPlanGpcList(int nCurrSectionNo,int& nNeighberGpcNum, int* nNeighberGpcList)
{
	if( NULL == nNeighberGpcList )
		return 0;

	GPCSECTION *tmp = sectinfo.getContentFromID( nCurrSectionNo );
	if( NULL== tmp )
		return 0;

 	nNeighberGpcNum = tmp->nViewPlanNeighberGpcCount;
	for(int i=0; i<nNeighberGpcNum; i++)
		nNeighberGpcList[i] = tmp->nViewPlanNeighberGpcList[i];

  	return nNeighberGpcNum;
}
 
 int CGpcdef::GetStationListofPhase(int nSection, int& stationNum, int* stationList)
{
	if( NULL == stationList )
		return 0;

	GPCSECTION *tmp = sectinfo.getContentFromID( nSection );
	if( NULL == tmp || stationNum < tmp->nStationNumPhase )
		return 0;

	stationNum = tmp->nStationNumPhase;
	for(int i=0; i<stationNum; i++)
		stationList[i] = tmp->nStationListPhase[i];
	return stationNum;
}

int CGpcdef::GetLeapLineFlag()
{
	return m_nTrainLeapLineFlag; //sunjf 2005 06 17

}

int CGpcdef::GetLineWidth()
{
	return m_nLineWidth; //sunjf 2005 06 17

}

//设置当前用户运行图上正在使用的区段，返回上一个值
int CGpcdef::SetCurrentSectNo(int sect_no)
{
	int prev = m_nCurrentSectNo;
	m_nCurrentSectNo = sect_no;
    return prev;	
}

int CGpcdef::GetCurrentSectNo()
{
	return m_nCurrentSectNo;
}

void CGpcdef::SetTccCurrentSectNo(int con_id)
{
	m_current_con_id = con_id;
}

int  CGpcdef::GetTccCurrentSectNo()
{
	return m_current_con_id;
}
int CGpcdef::LoadLocoConfigFile()
{
	char szFileName[128];
	sprintf(szFileName,"%s\\%s",GetDataDir(),LOCOFILENAME);
	
	m_locolist.clear();

	int t_nReturnValue = 0;
	CConfigFile  cfgfile;
	do
	{
		if(cfgfile.SetFileName(szFileName) < 0)
			break;
		if(cfgfile.GetSelectSection(LOCO_TYPE) < 0)
			break;
		char temp[512], locotype[32],loconame[32],id[32];
		while (cfgfile.GetSectionDataLine(temp) >= 0 )
		{
			ZeroMemory(locotype,sizeof(locotype));
			ZeroMemory(loconame,sizeof(loconame));
			ZeroMemory(id,sizeof(id));
			cfgfile.GetDataName(temp,locotype);
			if(cfgfile.GetDataValue(temp,0,id)>=0 && cfgfile.GetDataValue(temp,1,loconame)>=0)
			{
				LOCOTYPE item;
				ZeroMemory(&item, sizeof(item));
				strncpy(item.type,locotype,sizeof(item.type)-1);
				strncpy(item.name,loconame,sizeof(item.name)-1);
				item.id=atoi(id);

				m_locolist.push_back(item);
			}
		}
		cfgfile.CloseFile();
		if(m_locolist.size() >0)
			t_nReturnValue = 1;
	}
	while(0);
	return t_nReturnValue;
}

int CGpcdef::GetLocoTypeByIndex(int index, char *type, char *name, int &id)
{
	if(index < 0 || index >=  m_locolist.size())
		return FALSE;

	strcpy(type,m_locolist[index].type);
	strcpy(name,m_locolist[index].name);
	id = m_locolist[index].id;

	return TRUE;	
}
int CGpcdef::GetLocoTypeById(int id, char *type, char *name)
{

	for(int i=0;i<  m_locolist.size(); i++)
	{
		if(id == m_locolist[i].id)
		{
			strcpy(type,m_locolist[i].type);
			strcpy(name,m_locolist[i].name);
			return TRUE;
		}
		 
	}
	return FALSE;	
}

CString CGpcdef::GetLocoNameByType(CString no)
{
	CString name = no;
	for(int i=0;i<  m_locolist.size(); i++)
	{
		if(no == m_locolist[i].type)
		{
			name = m_locolist[i].name;
			break;
		}
		 
	}
	return name;	
}

CString CGpcdef::GetEngineDesp(CString loco_id)
{
	CString strNewEngine = loco_id;
	if(loco_id.GetLength() <= 3)
		return strNewEngine;

	CString no   = loco_id.Left(3);
	CString name = GetLocoNameByType(no);
	CString strEngineId = loco_id.Right(loco_id.GetLength()-3);
    if(no != name)
		strNewEngine = name + "/" + strEngineId;

	return strNewEngine;
}

int CGpcdef::loadFromTrainTypeInfo()
{
#define TRAINTYPENAME		"Traintype.CFG"
	char szFileName[128]; 	char *token;
	sprintf(szFileName,"%s\\%s",GetDataDir(),TRAINTYPENAME);

	TRAINTYPEINFO item;
	m_train_type.clear();
	 int t_nReturnValue = 0;
	CConfigFile  cfgfile;
	do
	{
		if(cfgfile.SetFileName(szFileName) < 0)
			break;
		if(cfgfile.GetSelectSection(GPCDEF_TRAINTYPE) < 0)
			break;
		char ptr[512];
		while (cfgfile.GetSectionDataLine(ptr) >= 0 )
		{

			char pre[10];
			memset(pre, 0x0, sizeof(pre));

			token = strtok(ptr, ",");
			if(NULL == token)
				return FALSE;
			if(sscanf(token, "PREFIX=%s", pre))
				strcpy(item.strPrefix , pre);

			token = strtok(NULL, ",");
			if(NULL == token)
				return  FALSE;
			while(*token && !isdigit(*token)) token++;
			int nStart = atoi(token);
			item.nStart = nStart;
			//sscanf(token, "START_TRAIN_NUM=%d", &nStart);

			token = strtok(NULL, ",");
			if(NULL == token)
				return FALSE;
			while(*token && !isdigit(*token)) token++;
			int nEnd = atoi(token);
			item.nEnd = nEnd;
			//sscanf(token, "END_TRAIN_NUM=%d", &nEnd);

			token = strtok(NULL, ",");
			if(NULL == token)
				return FALSE;
			while(*token && !isdigit(*token)) token++;
			int nType = atoi(token);
			item.nType = nType;
			//sscanf(token, "TYPE=%d", &nType);

			token = strtok(NULL, ",");
			if(NULL == token)
				return FALSE;
			while(*token && !isdigit(*token)) token++;
			int nRunType = atoi(token);
			item.nRunType = nRunType;
			//sscanf(token, "RUN_TYPE=%d", &nRunType);

			token = strtok(NULL, ",");
			if(NULL == token)
				return FALSE;
			while(*token && !isdigit(*token)) token++;
			int nPriority = atoi(token);
			item.nPriority = nPriority;
			//sscanf(token, "PRIORITY=%d", &nPriority);

			static int nNumber = 0;
			int nID = nNumber ++;
	        item.nID = nID;
			m_train_type.push_back(item);

		}

		cfgfile.CloseFile();
		if(m_train_type.size() >0)
			t_nReturnValue = 1;
	}
	while(0);

	return t_nReturnValue;


 
}

BOOL CGpcdef::IsFreightTrain(char *strTrainNo)
{

	CString prefix;
	CString tmp;
	const char *p = strTrainNo;
	while(*p && !isdigit((unsigned char)*p) && *p!='0')
	{
		tmp.Format("%c",  *p);
			prefix += tmp;
		p++;
	}
	int n = atoi(p);

	for(int i=0; i<m_train_type.size(); i++)
	{
		if(!strcmp(m_train_type[i].strPrefix, prefix)

			 &&  n >= m_train_type[i].nStart
			 && n <= m_train_type[i].nEnd)
		{
			return m_train_type[i].nType != 0;
		}
	}
	return TRUE;
}
 int CGpcdef::GetTenicalStopDrawWay()
{

	return  nTenicalStop;
}
int CGpcdef::GetUserDestineGpcNo(int nSourceGpcNo)
{
 
	if(nSourceGpcNo <=0) 
		return 0;

	for(int i=0; i<userinfo.size(); i++)
	{
		if(nSourceGpcNo == userinfo[i].nGpcId)
			break;
	}
	if( i >= userinfo.size() )
       return 0;
	else
	   return userinfo[i].nAckGpcNo;
	 
}

int CGpcdef::GetTdIdByUserID(int nSourceGpcNo)
{
	for(int i=0; i<userinfo.size(); i++)
	{
		if(nSourceGpcNo == userinfo[i].nGpcId)
			break;
	}
	if( i >= userinfo.size() )
		return 0;
	else
		return userinfo[i].nTDGpcNo;
}

int CGpcdef::LoadDCAuthorityInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t/=\n"; 
    int nCmdNoCount = 0;
    int nGPCCount = 0;

	if(m_dcAuthorityNum >= MAX_DIRECTOR_NUM)
	{
		return 0;
	}

	token = strtok(ptr, seps);
	if(token==NULL) 
	{
		nGPCCount = 0;
	}
	else
	{
		nGPCCount = atoi(token);
	}
	
	if(nGPCCount == 0)
		return 0;
	for(int idx=0; idx < nGPCCount; idx++)
	{
		token = strtok(NULL,seps);
		if(token == NULL) 
			return 0;
		m_dcAuthority[m_dcAuthorityNum].aryGPC.Add(atol(token));
	}
	
	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		nCmdNoCount = 0;
	}
	else
	{
		nCmdNoCount = atoi(token);
	}
	
	if(nCmdNoCount == 0)
		return 0;
	for(int idx=0; idx < nCmdNoCount; idx++)
	{
		token = strtok(NULL,seps);
		if(token == NULL) 
			return 0;
		if(strchr(token,'-'))
		{
			int nStart = 0, nEnd=0, tmp = 0;
			sscanf(token,"%d-%d",&nStart,&nEnd);
			if(nStart > nEnd)
			{
				tmp = nStart;
				nStart = nEnd;
				nEnd = tmp;
			}

			for(int j = nStart;j <= nEnd; j++)
			{
				CString strcmdno;
				strcmdno.Format("%d", j);
				m_dcAuthority[m_dcAuthorityNum].aryCmdTempleteNo.Add(strcmdno);
				idx++;
			}
		}
		else
		{
			CString strcmdno = token;
			m_dcAuthority[m_dcAuthorityNum].aryCmdTempleteNo.Add(strcmdno);
		}
	}

	token = strtok(NULL, seps);
	if(token != NULL) 
	{
		m_dcAuthority[m_dcAuthorityNum].desp = token;
	}
	
	m_dcAuthorityNum ++;
	
	return 1;
}

int CGpcdef::GetDCAuthorityList(CString templeteno, int &gpccount, int *gpclist)
{
	int i = -1;	
	gpccount = 0;
	for(i=0; i<m_dcAuthorityNum; i++)
	{
		for(int idx = 0; idx < m_dcAuthority[i].aryCmdTempleteNo.GetCount(); idx++)
		{
			if(templeteno == m_dcAuthority[i].aryCmdTempleteNo[idx])
			{
				gpccount = m_dcAuthority[i].aryGPC.GetCount();
				if(gpccount == 0)
				{
					break;
				}
				for(int tmp = 0; tmp < gpccount; tmp++)
				{
					*(gpclist + tmp) = (int)m_dcAuthority[i].aryGPC[tmp];
				}
				return 1;
			}
		}
	}

	// 
	int max_gpc_idx = 0, idx=0;
	for(i=0; i<m_dcAuthorityNum; i++)
	{
		if(max_gpc_idx < m_dcAuthority[i].aryGPC.GetCount())
		{
			max_gpc_idx = m_dcAuthority[i].aryGPC.GetCount();
			idx = i;
		}
	}

	if(max_gpc_idx == 0)
		return -1;

	gpccount = m_dcAuthority[idx].aryGPC.GetCount();
	for(int tmp = 0; tmp < gpccount; tmp++)
	{
		*(gpclist + tmp) = (int)m_dcAuthority[idx].aryGPC[tmp];
	}
	return 0;
}

int CGpcdef::SetNeedAuthorityDCNum(int num)
{
   m_nNeedAuthorityDCNum = num;
   return 1;
}

int CGpcdef::GetNeedAuthorityDCNum()
{
   return m_nNeedAuthorityDCNum;
}

int CGpcdef::LoadTccCmdIDRequireServerConfirm(char *ptr)
{
	char *token;
	char seps[]=" ,\t/=\n"; 
    
	token = strtok(ptr, seps);
	
	if(token)
		m_bTccCmdIDRequireServerConfirm = atoi(token) ? true : false;
	
	return 1;
}

BOOL CGpcdef::IsTccDefineInit(int con_id)
{
	const TCC_CFG_NEW* pTccCfg=GetTCCCfg_Const(con_id);
	if(pTccCfg)
		return TRUE;

	return FALSE;
}

bool CGpcdef::IsTccCmdIDRequireServerConfirm(int type)
{
	int con_id = this->GetTccCurrentSectNo();
	
	const TCC_CFG_NEW* pTccCfg=GetTCCCfg_Const(con_id);
	if(pTccCfg)
	{
		CString strType=pTccCfg->tcc_type;
		if(strType=="C4")
		{
			if(pTccCfg->tcc_cfg[0].tcc_type==type)
				return pTccCfg->tcc_cfg[0].confirm_tcc_cmdid;
			else if(pTccCfg->tcc_cfg[1].tcc_type==type)
				return pTccCfg->tcc_cfg[1].confirm_tcc_cmdid;
		}
		else
		{
			return pTccCfg->tcc_cfg[0].confirm_tcc_cmdid;
		}
	}

	return false;
}
bool CGpcdef::IsReInitTCC(int type)
{
	int con_id = this->GetTccCurrentSectNo();

	const TCC_CFG_NEW* pTccCfg=GetTCCCfg_Const(con_id);
	if(pTccCfg)
	{
		CString strType=pTccCfg->tcc_type;
		if(strType=="C4")
		{
			if(pTccCfg->tcc_cfg[0].tcc_type==type)
				return pTccCfg->tcc_cfg[0].bReInitTcc;
			else if(pTccCfg->tcc_cfg[1].tcc_type==type)
				return pTccCfg->tcc_cfg[1].bReInitTcc;
		}
		else
		{
			return pTccCfg->tcc_cfg[0].bReInitTcc;
		}
	}

	return false;
}

bool CGpcdef::IsCanCancelLimitDcmdDirect(int type)
{
	int con_id = this->GetTccCurrentSectNo();

	const TCC_CFG_NEW* pTccCfg=GetTCCCfg_Const(con_id);
	if(pTccCfg)
	{
		CString strType=pTccCfg->tcc_type;
		if(strType=="C4")
		{
			if(pTccCfg->tcc_cfg[0].tcc_type==type)
				return pTccCfg->tcc_cfg[0].bIsCanCancelDirect;
			else if(pTccCfg->tcc_cfg[1].tcc_type==type)
				return pTccCfg->tcc_cfg[1].bIsCanCancelDirect;
		}
		else
		{
			return pTccCfg->tcc_cfg[0].bIsCanCancelDirect;
		}
	}

	return false;
}

bool CGpcdef::IsCTCGpc()
{
	int con_id = this->GetTccCurrentSectNo();
	
	int ctc_gpc=0;
	const TCC_CFG_NEW* pTccCfg=GetTCCCfg_Const(con_id);
	if(pTccCfg)
	{
		ctc_gpc = pTccCfg->tdcs_or_ctc;
	}

	if(ctc_gpc != 0)
		return true;

	return false;
}

// 何时将串内容校验一下
int CGpcdef::LoadTccVersion(char *ptr)
{
	char *token;
	char seps[]=" ,\t/=\n"; 
    
	token = strtok(ptr, seps);
	
	if(token)
		m_szTccVersionString = token;
	
	return 1;
}

CString  CGpcdef::GetTccVersionString()
{
	int con_id = this->GetTccCurrentSectNo();
	const TCC_CFG_NEW* pTccCfg=GetTCCCfg_Const(con_id);
	if(pTccCfg)
	{
		return pTccCfg->tcc_type;
	}
	
	return "C2";
}

CString CGpcdef::GetTccCfgDir(int type)
{
	int con_id = this->GetTccCurrentSectNo();
	
	const TCC_CFG_NEW* pTccCfg=GetTCCCfg_Const(con_id);
	if(pTccCfg)
	{
		CString strType=pTccCfg->tcc_type;
		if(strType=="C4")
		{
			if(pTccCfg->tcc_cfg[0].tcc_type==type)
				return pTccCfg->tcc_cfg[0].tcc_dir;
			else if(pTccCfg->tcc_cfg[1].tcc_type==type)
				return pTccCfg->tcc_cfg[1].tcc_dir;
		}
		else
		{
			return pTccCfg->tcc_cfg[0].tcc_dir;
		}
	}

	return "";
}

int  CGpcdef::GetNeibTCCType(int type)
{
	int con_id = this->GetTccCurrentSectNo();
	
	const TCC_CFG_NEW* pTccCfg=GetTCCCfg_Const(con_id);
	if(pTccCfg)
	{
		CString strType=pTccCfg->tcc_type;
		if(strType=="C4")
		{
			if(pTccCfg->tcc_cfg[0].tcc_type==type)
				return pTccCfg->tcc_cfg[0].neib_tcc_type;
			else if(pTccCfg->tcc_cfg[1].tcc_type==type)
				return pTccCfg->tcc_cfg[1].neib_tcc_type;
		}
		else
		{
			return pTccCfg->tcc_cfg[0].neib_tcc_type;
		}
	}

	return 0;
}
	
CString CGpcdef::GetNeibTccCfgDir(int type)
{
	int con_id = this->GetTccCurrentSectNo();
	
	const TCC_CFG_NEW* pTccCfg=GetTCCCfg_Const(con_id);
	if(pTccCfg)
	{
		CString strType=pTccCfg->tcc_type;
		if(strType=="C4")
		{
			if(pTccCfg->tcc_cfg[0].tcc_type==type)
				return pTccCfg->tcc_cfg[0].neib_tcc_dir;
			else if(pTccCfg->tcc_cfg[1].tcc_type==type)
				return pTccCfg->tcc_cfg[1].neib_tcc_dir;
		}
		else
		{
			return pTccCfg->tcc_cfg[0].neib_tcc_dir;
		}
	}

	return "";
}

bool CGpcdef::GetStaList(int type, int& num, int stano[])
{
	int con_id = this->GetTccCurrentSectNo();

	const int *pstano=NULL;
	int stanum=0;
	const TCC_CFG_NEW* pTccCfg=GetTCCCfg_Const(con_id);
	if(pTccCfg)
	{
		CString strType=pTccCfg->tcc_type;
		if(strType=="C4")
		{
			if(pTccCfg->tcc_cfg[0].tcc_type==type)
			{
				stanum = pTccCfg->tcc_cfg[0].sta_num;
				pstano = pTccCfg->tcc_cfg[0].stano;
			}
			else if(pTccCfg->tcc_cfg[1].tcc_type==type)
			{
				stanum = pTccCfg->tcc_cfg[1].sta_num;
				pstano = pTccCfg->tcc_cfg[1].stano;
			}
		}
		else
		{
			stanum = pTccCfg->tcc_cfg[0].sta_num;
			pstano = pTccCfg->tcc_cfg[0].stano;
		}
	}

	if(pstano!=NULL && stanum>0)
	{
		int k=0;
		for(k=0; k<stanum && k<num; k++)
		{
			stano[k] = pstano[k];
		}
		num = k;
		return true;
	}

	return false;
}

int CGpcdef::LoadTccDefine(char *ptr)
{
	char *token;
	char seps[]=" ,\t/=\n"; 
    
	TCC_CFG_NEW tccnew;
	
	// 台号
	token = strtok(ptr, seps);
	if(token == NULL)
		return 0;	
	tccnew.con_id = atoi(token);
	
	// 台名
	token = strtok(NULL,seps);
	if(token == NULL)
		return 0;
	strncpy(tccnew.con_name, token, sizeof(tccnew.con_name)-1);
	tccnew.con_name[sizeof(tccnew.con_name)-1] = 0;
	
	TCC_CFG tcc;
	tcc.bReInitTcc = false;
	tcc.bIsCanCancelDirect = true;

	// TCC级别
	token = strtok(NULL,seps);
	if(token == NULL)
		return 0;
	tcc.tcc_type=atoi(token);
	if(tcc.tcc_type!=2 && tcc.tcc_type!=3)
	{
		AfxMessageBox("gpc.def中列控配置[TCC_DEFINE]的 列控级别配置不正确,只能为2或3");
		return 0;
	}
	sprintf(tccnew.tcc_type, "C%d", tcc.tcc_type);
	
	// GPC是否可以执行TCC
	token = strtok(NULL,seps);
	if(token == NULL)
		return 0;
	tccnew.tdcs_or_ctc = atoi(token);
	
	// 服务器确认命令号
	token = strtok(NULL,seps);
	if(token == NULL)
		return 0;
	tcc.confirm_tcc_cmdid = (atoi(token) > 0) ? true: false;

	// 列控配置目录
	token = strtok(NULL,seps);
	if(token == NULL)
		return 0;
	strncpy(tcc.tcc_dir, token, sizeof(tcc.tcc_dir));
	tcc.tcc_dir[sizeof(tcc.tcc_dir)-1] = 0;
	
	// 列控车站
	token = strtok(NULL,seps);
	tcc.sta_num = atoi(token);
    for(int k=0; k<tcc.sta_num; k++)
	{
		token = strtok(NULL,seps);
		if(token == NULL) 
			return 0;
		if(strchr(token,'-'))
		{
			int nStartST, nEndST;
			sscanf(token,"%d-%d",&nStartST,&nEndST);
			int count = k + nEndST - nStartST + 1;
			for(int j = k;j<count;j++)
			{
				tcc.stano[j] = nStartST;
				nStartST++;
			}
			k = j - 1;
		}
		else
		{
			tcc.stano[k] = (int)atoi(token);
		}
	}

	// 是否可以多次初始化列控中心
	token = strtok(NULL,seps);
	if(token != NULL)
		tcc.bReInitTcc = (atoi(token) > 0) ? true: false;
	
	// 是否可以直接限速
	token = strtok(NULL,seps);
	if(token != NULL)
	    tcc.bIsCanCancelDirect = (atoi(token) > 0) ? true: false;

	// 相关列控信息
	tcc.neib_tcc_type=0;
	token = strtok(NULL,seps);
	if(token != NULL)
	    tcc.neib_tcc_type = atoi(token);

	// 列控配置目录
	memset(tcc.neib_tcc_dir, 0, sizeof(tcc.neib_tcc_dir));
	token = strtok(NULL,seps);
	if(token != NULL)	
	{
		strncpy(tcc.neib_tcc_dir, token, sizeof(tcc.neib_tcc_dir));
		tcc.neib_tcc_dir[sizeof(tcc.neib_tcc_dir)-1] = 0;
	}
	if((tcc.neib_tcc_type>0) && (tcc.neib_tcc_type != 2) && (tcc.neib_tcc_type != 3))
	{
		memset(tcc.neib_tcc_dir, 0, sizeof(tcc.neib_tcc_dir));
		CString str;
		str.Format("gpc.def中列控配置[TCC_DEFINE]中%d台的列控配置错误, 相关列控类型配置只能为2或3", tccnew.con_id);
		AfxMessageBox(str);
		return 0;
	}


	TCC_CFG_NEW* pTccCfg=GetTCCCfg(tccnew.con_id);
	if(pTccCfg==NULL)
	{
		TCC_CFG_NEW *pNewTccCfg=GetNewTCCCfg();
		if(pNewTccCfg)
		{
			*pNewTccCfg=tccnew;
			pNewTccCfg->tcc_cfg[0]=tcc;
		}
	}
	else
	{
		if(strcmp(pTccCfg->tcc_type, tccnew.tcc_type)==0)
		{
			CString str;
			str.Format("gpc.def中列控配置[TCC_DEFINE]中%d台的列控配置重复, 请检查除掉重复部分", tccnew.con_id);
			AfxMessageBox(str);
			return 0;
		}
		else
		{
			CString cur=pTccCfg->tcc_type;
			if(cur=="C4")
			{
				CString str;
				str.Format("gpc.def中列控配置[TCC_DEFINE]中%d台的列控配置错误, 只能存在两种类型列控, 并且只能配置两次", tccnew.con_id);
				AfxMessageBox(str);
				return 0;
			}
			else
			{
				strcpy(pTccCfg->tcc_type, "C4");
				pTccCfg->tcc_cfg[1]=tcc; 
				for(int i=0; i<pTccCfg->tcc_cfg[0].sta_num; i++)
				{
					int stano=pTccCfg->tcc_cfg[0].stano[i];
					for(int j=0; j<pTccCfg->tcc_cfg[1].sta_num; j++)
					{
						if(pTccCfg->tcc_cfg[1].stano[j]==stano)
						{
							CString str;
							str.Format("gpc.def中列控配置[TCC_DEFINE]中%d台的列控配置错误, 不同类型列控之间不能存在相同车站", tccnew.con_id);
							AfxMessageBox(str);
							return 0;
						}
					}
				}
			}
		}
	}
	
	return 1;
}

const GPCSTATION* CGpcdef::GetStationInfo(short nStaID)
{
	return stainfo.getContentFromID(nStaID);
}

GPCSECTION CGpcdef::GetSectionInfo(short nNum)
{
	return sectinfo[nNum];
}

short CGpcdef::GetRouteCnt()
{
	return routeinfo_wsg.size();
}

GPCROUTE CGpcdef::GetRouteInfo(short nNum)
{
	return routeinfo_wsg[nNum];
}

void CGpcdef::SetSidesOfStation(int stano, STATION_SIDES_INFO& info)
{
	mapStationSideInfo.insert(make_pair(stano, info));
}

bool CGpcdef::GetSidesOfStation(int stano, STATION_SIDES_INFO& info)
{
	std::map<int, STATION_SIDES_INFO>::iterator it = mapStationSideInfo.find(stano);
	if(it != mapStationSideInfo.end())
	{
		info = it->second;
		return true;
	}

	return false;
}

BOOL CGpcdef::IsTgOnlyOne()
{
	return (m_nTgOnce>0)? TRUE: FALSE;
}

BOOL CGpcdef::IsFuncMenuViewTgSchd(int type)
{
	return (m_nViewSchdTG[type]==1)?TRUE:FALSE;
}

//;格式[DEFAULT_ENTRY]=区段, 车站, 上行出口,下行入口,上行入口,下行出口, 站序
// [INTERFACESTATION_EXIT_ENTRY]=1,   24,	 1,  1,  0, 0
int CGpcdef::LoadInterfaceExitEntry(char *ptr)
{
	char *token;
	char seps[]=" ,\t/=\n"; 
    
	int con_id;
	StationExitEntry tmp;
	// 区段
	token = strtok(ptr, seps);
	if(token == NULL)
		return 0;	
	con_id = atoi(token);

	// 车站
	token = strtok(NULL, seps);
	if(token == NULL)
		return 0;	
	tmp.station = atoi(token);

	// 上行出口
	token = strtok(NULL, seps);
	if(token == NULL)
		return 0;	
	tmp.up_exit = atoi(token);

	// 下行入口
	token = strtok(NULL, seps);
	if(token == NULL)
		return 0;	
	tmp.down_entry = atoi(token);

	// 上行入口
	token = strtok(NULL, seps);
	if(token == NULL)
		return 0;	
	tmp.up_entry = atoi(token);

	// 下行出口
	token = strtok(NULL, seps);
	if(token == NULL)
		return 0;	
	tmp.down_exit = atoi(token);

	// 站序
	tmp.index=255;
	token = strtok(NULL, seps);
	if(token != NULL)	
		tmp.index = atoi(token);

	int max_con = sizeof(nInterfaceExitEntry)/sizeof(nInterfaceExitEntry[0]);
	int i=0;
	for(i=0; i<max_con; i++)
	{
		if(nInterfaceExitEntry[i].con_id == con_id)
			break;

		if(nInterfaceExitEntry[i].con_id==0)
		{
			nInterfaceExitEntry[i].con_id = con_id;
			nInterfaceExitEntry[i].num=0;
			memset(nInterfaceExitEntry[i].station_exit_entry, 0, sizeof(nInterfaceExitEntry[i].station_exit_entry));
			m_nInterfaceExitEntryNum++;
			break;
		}
	}
	if(i>=max_con)
		return 0;

	int index = nInterfaceExitEntry[i].num;
	if(index < sizeof(nInterfaceExitEntry[0].station_exit_entry)/sizeof(nInterfaceExitEntry[0].station_exit_entry[0])-1)
	{
		nInterfaceExitEntry[i].station_exit_entry[index].station=tmp.station;
		nInterfaceExitEntry[i].station_exit_entry[index].index=tmp.index;
		nInterfaceExitEntry[i].station_exit_entry[index].up_exit=tmp.up_exit;
		nInterfaceExitEntry[i].station_exit_entry[index].up_entry=tmp.up_entry;
		nInterfaceExitEntry[i].station_exit_entry[index].down_entry=tmp.down_entry;
		nInterfaceExitEntry[i].station_exit_entry[index].down_exit=tmp.down_exit;
		nInterfaceExitEntry[i].num++;
	}
	
	return 1;
}

bool CGpcdef::GetInterfaceStation(int con_id, WORD station, BYTE index, BYTE& up_exit, BYTE& down_entry, BYTE& up_entry, BYTE& down_exit)
{
	for(int i=0; i<m_nInterfaceExitEntryNum; i++)
	{
		if(nInterfaceExitEntry[i].con_id == con_id)
		{
			for(int k=0; k<nInterfaceExitEntry[i].num; k++)
			{
				if(nInterfaceExitEntry[i].station_exit_entry[k].station == station)
				{
					int cfg_index=nInterfaceExitEntry[i].station_exit_entry[k].index;

					if(((cfg_index==(index+1)) && (cfg_index!=255)) || (cfg_index==255))
					{
						up_exit = nInterfaceExitEntry[i].station_exit_entry[k].up_exit;
						down_entry = nInterfaceExitEntry[i].station_exit_entry[k].down_entry;
						up_entry = nInterfaceExitEntry[i].station_exit_entry[k].up_entry;
						down_exit = nInterfaceExitEntry[i].station_exit_entry[k].down_exit;

						return true;
					}
				}
			}
		}
	}
	up_exit=0;
	down_entry=0;
	up_entry=0;
	down_exit=0;
	return false;
}

int CGpcdef::LoadFtpInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
 
	memset(&mFtpInfo, 0, sizeof(FtpInfo));

	token = strtok(ptr, seps);
	if(token==NULL) 
		return 0;
	strncpy(mFtpInfo.ip, token, 16);

	token = strtok(NULL, seps);
	if(token==NULL) 
		return 0;
	strncpy(mFtpInfo.dirName, token, 99);

	token = strtok(NULL, seps);
	if(token==NULL) 
		return 0;
	strncpy(mFtpInfo.user, token, 19);

	token = strtok(NULL, seps);
	if(token==NULL) 
		return 0;
	strncpy(mFtpInfo.pwd, token, 19);

	token = strtok(NULL, seps);
	if(token==NULL) 
		mFtpInfo.port=21;
	else
		mFtpInfo.port=atoi(token);

	mFtpInfo.init=1;
    return 1;
}

bool CGpcdef::GetFtpLoginInfo(FtpInfo& ftpInfo)
{
	ftpInfo=mFtpInfo;

	return (ftpInfo.init==1);
}

bool CGpcdef::IsNotThisSectionStation(int sectionNo, int stationNo)
{
	for(int i=0;i<m_vctNotThisSectionStation.size();i++)
	{
		if(m_vctNotThisSectionStation[i].nSectionNo==sectionNo)
		{
			for(int j=0; j<m_vctNotThisSectionStation[i].nStationNum; j++ )
			{
				if (m_vctNotThisSectionStation[i].nStationList[j] == stationNo)
					return true;
			}
		}
	}

	return false;  
}

int CGpcdef::GetTGStationListForCheck(int con_id, int max_station_list, int stationList[])
{
	int stationNum=0;
	std::map<int, TGSectionStation>::iterator it = mapTGSectionStation.find(con_id);
	if(it != mapTGSectionStation.end())
	{
		for(int i=0; i<it->second.num; i++)
		{
			if (IsNotThisSectionStation(con_id, it->second.station[i]))
			{
				continue;
			}

			if(stationNum<max_station_list)
			{
				stationList[stationNum] = it->second.station[i];
				stationNum++;
			}
			else
			{
				stationNum=-1;
				break;
			}
		}
	}

	return stationNum;
}

bool CGpcdef::LoadTGDefProfile(CString pathname)
{
	char seps[]=" ,\t\\/=\n";

	FILE *fp=NULL;

	char *ptr, strLine[1024];
	if((fp=fopen(pathname, "r"))==NULL)
		return false;

	// read file
	int m_nTgErrorline = 0;
	while (!feof(fp))	
	{
		ptr = fgets(strLine, 1024, fp);
		m_nTgErrorline++;
		if (ptr == NULL) 
			continue;
		while (*ptr == ' ' || *ptr == '\t')
			ptr ++;
		if (*ptr == ';')	
			continue;
		if (*ptr == '\n')	
			continue;

		if (*ptr == '[' && !strncmp(ptr, "[SECTION_NODE]", strlen("[SECTION_NODE]")) ) 
		{ 
			ptr = ptr + strlen("[SECTION_NODE]");

			// section no 
			char* token = strtok(ptr, seps);
			if(token==NULL) 
				break;

			int nSectionNo = (int)atoi(token);

			token = strtok(NULL, seps);
			if(token==NULL) 
				break;
			
			token = strtok(NULL, seps);
			if(token==NULL) 
				break;
			
			token = strtok(NULL, seps);
			if(token==NULL) 
				break;
			
			//staton number
			token = strtok(NULL, seps);
			if(token==NULL) 
				break;
			int station_number =  (int)atoi(token);

			std::map<int, TGSectionStation>::iterator it = mapTGSectionStation.find(nSectionNo);
			if(it != mapTGSectionStation.end())
			{
				for(int i=0; i<it->second.num; i++)
				{
					if(it->second.station[i] == station_number)
						break;
				}

				if(i>=it->second.num && i<128)
				{
					it->second.station[i] = station_number;
					it->second.num++;
				}
			}
			else
			{
				TGSectionStation sta;
				sta.num = 1;
				sta.station[0] = station_number;

				mapTGSectionStation.insert(make_pair(nSectionNo, sta));
			}
		}
	}
		
	fclose(fp);

	return true;
}

const TCC_CFG_NEW *CGpcdef::GetTCCCfg_Const(int conid)
{
	for(int i=0; i<64; i++)
	{
		if(m_tcc_cfg[i].con_id == conid)
			return &m_tcc_cfg[i];
	}

	return NULL;
}

TCC_CFG_NEW *CGpcdef::GetTCCCfg(int conid)
{
	for(int i=0; i<64; i++)
	{
		if(m_tcc_cfg[i].con_id == conid)
			return &m_tcc_cfg[i];
	}

	return NULL;
}

TCC_CFG_NEW *CGpcdef::GetNewTCCCfg()
{
	for(int i=0; i<64; i++)
	{
		if(m_tcc_cfg[i].con_id == 0)
			return &m_tcc_cfg[i];
	}

	return NULL;
}

void CGpcdef::ClearC3TccLineName()
{
	mapC3TccLineName.clear();
}

void CGpcdef::AddC3TccLineName(long lineNo, CString& lineName)
{
	mapC3TccLineName.insert(make_pair(lineNo, lineName));
}
	
CString CGpcdef::GetC3TccLineName(long lineNo)
{
	if(0 >= lineNo)
		return "";

	std::map<long,  CString>::iterator it= mapC3TccLineName.find(lineNo);
	if(it != mapC3TccLineName.end())
	{
		return (*it).second;
	}

	return "";
}

void CGpcdef::ClearC2TccLineName()
{
	mapC2TccLineName.clear();
}

void CGpcdef::AddC2TccLineName(long lineNo, CString& lineName)
{
	mapC2TccLineName.insert(make_pair(lineNo, lineName));
}
	
CString CGpcdef::GetC2TccLineName(long lineNo)
{
	if(0 >= lineNo)
		return "";

	std::map<long,  CString>::iterator it= mapC2TccLineName.find(lineNo);
	if(it != mapC2TccLineName.end())
	{
		return (*it).second;
	}

	return "";
}

void CGpcdef::SetShowRouteTrigFlag(int set)
{
	if(set>0)
		m_bShowShowRouteTrig=true;
	else
		m_bShowShowRouteTrig=false;
}

void CGpcdef::SetShowVirtualStation(bool set)
{
	m_bShowVirtualStation=set;
}

void CGpcdef::SetShowKiloMeterButton(bool set)
{
	m_bShowKiloMeterButton=set;
}

void CGpcdef::SetShowTransInfoButton(bool set)
{
	m_bShowTransInfoButton=set;
}

void CGpcdef::SetShowAutoAdjust(bool set)
{
	m_bShowAutoAdjust=set;
}

void CGpcdef::SetShowRouteOkMark(int set)
{
	if(set>0)
		m_bRouteOkMark=true;
	else
		m_bRouteOkMark=false;
}

int  CGpcdef::LoadSectionSwitchStation(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j, nStart, nEnd;

	SwitchStation  tmp;
	memset(&tmp, 0, sizeof(SwitchStation));

	// load section number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	int section = (int)atoi(token);
		
	// load switch station list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	int StationNum = (int)atoi(token);
	for(i=0; i<StationNum;) 
	{
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			
		if(strchr(token, '-')) 
		{
			sscanf(token, "%d-%d", &nStart, &nEnd);
			for(j=nStart; j<=nEnd; j++)
				tmp.station[i++] = j;
		} 
		else 
		{
 			tmp.station[i] = (int)atoi(token);
			i++;
		}
	}
	tmp.num=i;

	m_mapSwitchStation.insert(make_pair(section, tmp));

	return 1;
}

int  CGpcdef::GetSectionSwitchStation(int con_id, int list[])
{
	int num=0;
	std::map<int,SwitchStation>::iterator it = m_mapSwitchStation.find(con_id);
	if(it != m_mapSwitchStation.end())
	{
		for(int i=0; i<it->second.num; i++)
		{
			num++;
			list[i]=it->second.station[i];
		}
	}

	return num;
}

int  CGpcdef::LoadNotShowSideCtcStation(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j, nStart, nEnd;

	SwitchStation  tmp;
	memset(&tmp, 0, sizeof(SwitchStation));

	// load section number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	int section = (int)atoi(token);
		
	// load switch station list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	int StationNum = (int)atoi(token);
	for(i=0; i<StationNum;) 
	{
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
			
		if(strchr(token, '-')) 
		{
			sscanf(token, "%d-%d", &nStart, &nEnd);
			for(j=nStart; j<=nEnd; j++)
				tmp.station[i++] = j;
		} 
		else
		{
 			tmp.station[i] = (int)atoi(token);
			i++;
		}
	}
	tmp.num=i;

	m_mapNotShowSideCtcStation.insert(make_pair(section, tmp));

	return 1;
}

int  CGpcdef::GetNotShowSideCtcStation(int con_id, int list[])
{
	int num=0;
	std::map<int,SwitchStation>::iterator it = m_mapNotShowSideCtcStation.find(con_id);
	if(it != m_mapNotShowSideCtcStation.end())
	{
		for(int i=0; i<it->second.num; i++)
		{
			num++;
			list[i]=it->second.station[i];
		}
	}

	return num;
}


bool CGpcdef::IsRouteListExecuteForceEnable()
{
	return m_bRouteListExecuteForceEnable;
}

int CGpcdef::LoadYugaoStation(char* ptr)
{
	char *token;
	char seps[]=" ,[]\t\\/=\n";
 
	//区段号
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	int nSectionNo=(int)atoi(token);    
	
	//区段名称
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	CString name=token;

	//本台车站
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	int station=(int)atoi(token);  

	//本台车站的邻站
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	int station_neib=(int)atoi(token);  

	std::map<int, YugaoStation>::iterator it=m_mapYugaoStation.find(nSectionNo);
	if(it!=m_mapYugaoStation.end())
	{
		if(it->second.num>=128)
		{
			CString text;
			text.Format("gpc.def中预告车站[YUGAO_STATION]=%d %s区段, 配置预告车站对不能大于127", nSectionNo, name);
			AfxMessageBox(text);
			return 0;
		}
		it->second.station[it->second.num]=station;
		it->second.neib_station[it->second.num]=station_neib;
		it->second.num++;
	}
	else
	{
		//车站列表
		YugaoStation yugaosta;
		yugaosta.num=1;
		yugaosta.station[0]=station;
		yugaosta.neib_station[0]=station_neib;
	
		m_mapYugaoStation.insert(make_pair(nSectionNo, yugaosta));
	}
	
	return 1;
}

int CGpcdef::GetYugaoStation(int conid, int capity, unsigned short list_station[], unsigned short list_neib_station[])
{
	int count=0;
	std::map<int, YugaoStation>::iterator it=m_mapYugaoStation.find(conid);
	if(it!=m_mapYugaoStation.end())
	{
		for(int i=0; i<it->second.num; i++)
		{
			STATION_NO station		=it->second.station[i];
			STATION_NO neib_station =it->second.neib_station[i];

			for(int n=0; n<count; n++)
			{
				if(list_station[n]==station && list_neib_station[n]==neib_station)
				{
					break;
				}
			}
			if(n>=count && count<capity)
			{
				list_station[count]		=station;
				list_neib_station[count]=neib_station;
				count++;
			}
		}
	}

	return count;
}

bool CGpcdef::AddSvrTgEntityIntoMap(MSSvrTgEntity svrtg)
{
	if (svrtg.AttributNo>0 && svrtg.SectionNo>0 && svrtg.EntityID>0)
	{
		m_mapMSSvrTgEntity.push_back(svrtg);
		return true;
	}
	return false;
}

long CGpcdef::GetSvrTgEntityIDByIndex(int index)
{
	if (index<m_mapMSSvrTgEntity.size() && index>=0)
		return m_mapMSSvrTgEntity[index].EntityID;
	else
		return 0;
}

long CGpcdef::GetSvrTgAttributNoByIndex(int index)
{
	if (index<m_mapMSSvrTgEntity.size() && index>=0)
		return m_mapMSSvrTgEntity[index].AttributNo;
	else
		return 0;
}

void CGpcdef::EnableAssistantModifyPropTree(BOOL va)
{
	if(va)
		m_bEnableAssistModifyPropTree = true;
}

// 判断兰州局 某站的某方向 的+或者 -机车 是否显示  add_or_sub： 0+ 1-
bool CGpcdef::IsLocoNoDisplay(int conid, int station, int arrival_index, int depart_index, int direction, int add_or_sub)
{
	LocoNoDisplay disp;
	for (int i=0; i<m_loco_no_display_list.size(); i++)
	{
		disp = m_loco_no_display_list[i];
		if (disp.nSection == conid && disp.nStation == station && disp.nArrivalIndex == arrival_index
			&& disp.nDepartIndex == depart_index && disp.nDirect == direction)
		{
			if (add_or_sub==0)
				return disp.nAddShow;
			else
				return disp.nSubShow;
		}
	}
	return true;
}

bool CGpcdef::IsThisSectionHaveLineNoBySide(int section)
{
	for(int i=0; i<m_nEntryBySideSectionCount; i++)
	{
		if(mEntryBySideSection[i].section==section)
		{
			return true;
		}
	}

	return false;
}

bool CGpcdef::GetStationExitBySide(int section, int station1, int station2, int side1, int& exit, int& entry)
{
	for(int i=0; i<m_nEntryBySideSectionCount; i++)
	{
		if(mEntryBySideSection[i].section==section)
		{
			EntryBySideSection& con=mEntryBySideSection[i];
			for(int n=0; n<con.count; n++)
			{
				if(con.info[n].station1==station1 && con.info[n].station2==station2)
				{
					for(int k=0; k<con.info[n].side_num; k++)
					{
						if(con.info[n].side[k]==side1)
						{
							exit=con.info[n].exit;
							entry=con.info[n].entry;
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

bool CGpcdef::GetStationEntryBySide(int section, int station1, int station2, int side2, int& exit, int& entry)
{
	for(int i=0; i<m_nEntryBySideSectionCount; i++)
	{
		if(mEntryBySideSection[i].section==section)
		{
			EntryBySideSection& con=mEntryBySideSection[i];
			for(int n=0; n<con.count; n++)
			{
				if(con.info[n].station1==station2 && con.info[n].station2==station1)
				{
					for(int k=0; k<con.info[n].side_num; k++)
					{
						if(con.info[n].side[k]==side2)
						{
							exit=con.info[n].entry;
							entry=con.info[n].exit;
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

void CGpcdef::LoadEntryBySide()
{
	m_nEntryBySideSectionCount=0;
	memset(&mEntryBySideSection, 0, sizeof(mEntryBySideSection));

	CString pathname=this->GetDataDir();
	pathname += "\\EntryBySide.xml";
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}
	
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		return; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "BUREAU");
	if(pRootNode == NULL)
		return;

	// CON_ID
	MSXML::IXMLDOMNodeListPtr list = pRootNode->selectNodes( "CON_ID");
	if(NULL == list)
		return;

	for(int idx = 0; idx < list->Getlength(); idx++)
	{
		pNode = list->Getitem(idx);
		if(NULL == pNode)
			continue;
			
		EntryBySideSection absdesp;
		absdesp.count=0;
		absdesp.section=0;
		// @id
		int con_id=0;
		pChildNode = pNode->selectSingleNode("@id");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			con_id = atoi(CString(v.bstrVal));	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		absdesp.section=con_id;
		if(absdesp.section==0)
			continue;
		
		MSXML::IXMLDOMNodeListPtr liststa = pNode->selectNodes( "STATION");
		for(int idx = 0; idx < liststa->Getlength(); idx++)
		{
			pNode = liststa->Getitem(idx);
			int id=0, nextid=0;
			// @id
			pChildNode = pNode->selectSingleNode("@id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				id = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode("@next_id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				nextid = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			if(id==0 || nextid==0)
				continue;
				
			// 出入口个数为0
			MSXML::IXMLDOMNodeListPtr listexit = pNode->selectNodes( "ENTRY_SIDE");
			for(int idx = 0; idx < listexit->Getlength(); idx++)
			{
				int exit=0, entry=0;
				pNode = listexit->Getitem(idx);
				// @exit
				pChildNode = pNode->selectSingleNode("@exit");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					exit = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				// @entry
				pChildNode = pNode->selectSingleNode("@entry");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					entry = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				if(exit==0 || entry==0)
					continue;
				
				EntryBySide ss;
				ss.station1=id;
				ss.station2=nextid;
				ss.exit=exit;
				ss.entry=entry;
				ss.side_num=0;
				for(int a=0; a<64; a++)
					ss.side[a]=0;
				MSXML::IXMLDOMNodeListPtr listside = pNode->selectNodes( "SIDE");
				for(int idx = 0; idx < listside->Getlength(); idx++)
				{
					int side=0;
					pNode = listside->Getitem(idx);
					// @entry1
					pChildNode = pNode->selectSingleNode("@id");
					if(pChildNode)
					{
						_variant_t v;
						v = pChildNode->GetnodeTypedValue();
						side = atoi(CString(v.bstrVal));	
						SysFreeString(v.bstrVal);
						v.Clear();
					}
					if(side>0)
					{
						ss.side[ss.side_num]=side;
						ss.side_num++;
					}
				}
			
				if(ss.side_num>0)
				{
					absdesp.info[absdesp.count]=ss;
					absdesp.count++;
				}
			}
		}

		if(m_nEntryBySideSectionCount<64 && absdesp.count>0)
		{
			mEntryBySideSection[m_nEntryBySideSectionCount]=absdesp;
			m_nEntryBySideSectionCount++;
		}
	}
}

int CGpcdef::LoadSpecialGPCStplanInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j, nStart, nEnd;

	SpecialGPCStplanInfo  tmp;
	memset(&tmp, 0, sizeof(SpecialGPCStplanInfo));

	// load gpc id
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	strcpy( tmp.sGpcId, token );        //username

	// load section number
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	tmp.nSection = (int)atoi(token);

	// 调度台名
	token = strtok(NULL, seps);

	//下发阶段计划车站的列表
	token = strtok(NULL, seps);
	tmp.nPlanGroupCount = (int)atoi(token);
	for(i=0;i<tmp.nPlanGroupCount;)
	{
		token = strtok(NULL,seps);
		if(token == NULL) return 0;
		if(strchr(token,'-'))
		{
			sscanf(token, "%d-%d", &nStart,&nEnd);
			for(j=nStart;j<=nEnd;j++)
				tmp.nPlanGroupList[i++] = j;
		}
		else
		{
			tmp.nPlanGroupList[i] = (int)atoi(token);
			i++;
		}
	}

	special_stplanInfo_list.push_back(tmp);
	// scuccess return
	return 1;
}


int CGpcdef::LoadSpecialGPCStplanGroup(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j, nStart, nEnd;

	SpecialGPCStplanGroup  tmp;
	memset(&tmp, 0, sizeof(SpecialGPCStplanGroup));

	// load index id
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	tmp.nIndex = (int)atoi(token); 

	// load display name
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strcpy( tmp.sDiscription, token );

	//下发阶段计划车站的列表
	token = strtok(NULL, seps);
	tmp.nPlanReceiverCount = (int)atoi(token);
	for(i=0;i<tmp.nPlanReceiverCount;)
	{
		token = strtok(NULL,seps);
		if(token == NULL) return 0;
		if(strchr(token,'-'))
		{
			sscanf(token, "%d-%d", &nStart,&nEnd);
			for(j=nStart;j<=nEnd;j++)
				tmp.nPlanReceiverList[i++] = j;
		}
		else
		{
			tmp.nPlanReceiverList[i] = (int)atoi(token);
			i++;
		}
	}

	special_stplanGroup_list.push_back(tmp);
	// scuccess return
	return 1;
}


int CGpcdef::GetSpecialGPCList(int section, int *RecerverList)
{
	int count=0;
	for (int i=0; i< special_stplanInfo_list.size(); i++)
	{
		SpecialGPCStplanInfo tmp;
		tmp = special_stplanInfo_list[i];
		if (section==special_stplanInfo_list[i].nSection
			&& !strncmp(special_stplanInfo_list[i].sGpcId, GetCurUserName(), sizeof(special_stplanInfo_list[i].sGpcId)))
		{
			for (int index=0; index<special_stplanInfo_list[i].nPlanGroupCount; index++)
			{
				int group_index = special_stplanInfo_list[i].nPlanGroupList[index];
				SpecialGPCStplanGroup* tmp;
				tmp = GetSpcialGroupByIndex(group_index);
				if(tmp!=NULL)
				{
					for(int ndx=0; ndx< tmp->nPlanReceiverCount; ndx++)
					{
						RecerverList[count+ndx] = tmp->nPlanReceiverList[ndx];
					}
					count += tmp->nPlanReceiverCount;
				}				
			}
			break;
		}
	}
	return count;
}

SpecialGPCStplanGroup* CGpcdef::GetSpcialGroupByIndex(int index)
{
	for(int i=0; i<special_stplanGroup_list.size(); i++)
	{
		if(special_stplanGroup_list[i].nIndex == index)
			return &special_stplanGroup_list[i];
	}

	return NULL;
}

int CGpcdef::GetSpecialGPCGroupList(int section, int *GroupList)
{
	int count=0;
	for (int i=0; i< special_stplanInfo_list.size(); i++)
	{
		SpecialGPCStplanInfo tmp;
		tmp = special_stplanInfo_list[i];
		if (section==special_stplanInfo_list[i].nSection
			&& !strncmp(special_stplanInfo_list[i].sGpcId, GetCurUserName(), sizeof(special_stplanInfo_list[i].sGpcId)))
		{
			count = tmp.nPlanGroupCount;
			for (int index=0; index<special_stplanInfo_list[i].nPlanGroupCount; index++)
			{
				GroupList[index] = special_stplanInfo_list[i].nPlanGroupList[index];			
			}
			break;
		}
	}
	return count;
}

CString CGpcdef::GetSpecialGroupNameByIndex(int index)
{
	for(int i=0; i<special_stplanGroup_list.size(); i++)
	{
		if(special_stplanGroup_list[i].nIndex == index)
			return special_stplanGroup_list[i].sDiscription;
	}

	return "";
}


bool CGpcdef::LoadNotMeetBySpeed()
{
	CString pathname=this->GetDataDir();
	pathname += "\\can_not_meet.xml";
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		return false; 
	}

	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "CAN_NOT_MEET_TRAIN_LIST");
	if(pRootNode == NULL)
		return false;

	// CAN_NOT_MEET_TYPE
	MSXML::IXMLDOMNodeListPtr list = pRootNode->selectNodes( "CAN_NOT_MEET_TYPE");
	if(NULL == list)
		return false;

	for(int i=0; i<8; i++)
		mNotMeetInfo[i].Init();
	m_nNotMeetInfoCount=0;

	for(int idx = 0; idx < list->Getlength(); idx++)
	{
		pNode = list->Getitem(idx);
		if(NULL == pNode)
			continue;

		NotMeetInfo notmeetinfo;
		notmeetinfo.Init();
		
		pChildNode = pNode->selectSingleNode("@id");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			notmeetinfo.nNoMeetFlag = atoi(CString(v.bstrVal));	
			notmeetinfo.nNoMeetFlag = notmeetinfo.nNoMeetFlag<<8;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	
		CString tmp;
		pChildNode = pNode->selectSingleNode("@text");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			tmp = CString(v.bstrVal);	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		notmeetinfo.sName = tmp;
		
		pChildNode = pNode->selectSingleNode("@short_text");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			tmp = CString(v.bstrVal);	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		notmeetinfo.sShortName = tmp;

		int count=0;
		MSXML::IXMLDOMNodeListPtr listtype = pNode->selectNodes( "TRAIN_TYPE");
		for(int idx = 0; idx < listtype->Getlength(); idx++)
		{
			pNode = listtype->Getitem(idx);
			int id=-2;
			// @id
			pChildNode = pNode->selectSingleNode("@id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				id = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			if(id==-2)
				continue;

			notmeetinfo.nTrainTypeList[count]=id;
			count++;
		}
		notmeetinfo.nTrainTypeCount=count;
		
		if(m_nNotMeetInfoCount>=4)
		{
			break;
		}
		mNotMeetInfo[m_nNotMeetInfoCount]=notmeetinfo;
		m_nNotMeetInfoCount++;
	}

	if(m_nNotMeetInfoCount<=0)
		return false;

	return true;
}

int  CGpcdef::GetBasicPlanStation(int section, int station_list[])
{
	int count=0;
	for(int i=0; i<m_basicPlanReciever.size(); i++)
	{
		if(m_basicPlanReciever[i].section==section)
		{
			GPCPLANRECEIVER rec;
			for(int k=0; k<m_basicPlanReciever[i].count; k++)
			{
				memset(&rec, 0x00, sizeof(rec));
				GetReceiverInfo(m_basicPlanReciever[i].reciever[k],rec);
				
				int m=0;
				for(m=0;m<count;m++)
				{
					if(station_list[m]==rec.nstationNo)
					{
						break;
					}
				}
				if(m>=count && count<128)
				{
					station_list[count]=rec.nstationNo;
					count++;
				}
			}
		}
	}

	return count;
}

bool  CGpcdef::IsBasicPlanIndex(int section, int index_rec)
{
	bool bFind = false;
	for(int i=0; i<m_basicPlanReciever.size(); i++)
	{
		if(m_basicPlanReciever[i].section==section)
		{
			for(int k=0; k<m_basicPlanReciever[i].count; k++)
			{
				if(index_rec==m_basicPlanReciever[i].reciever[k])
				{
					bFind = true;
					break;
				}
			}
			break;
		}
	}

	return bFind;
}

// 加载综合监控端口配置文件mmproc_udp_list.cfg
void CGpcdef::LoadMmproUdpEntity()
{ 
	char  szFileName[256];
	sprintf(szFileName,"%s\\mmproc_udp_list.cfg",GetDataDir());
    CIniFile t;
    int theStatus =1;
	t.OpenFile(szFileName);		
	if(!t.IsOK())
		return;
	//实体号=端口
    if(t.GotoSection("UDP_PORT"))
	{
		char key[64],content[256];		
		while(t.GetContent(key,content,256)) 
		{
			UDP_ENTITY relation;
			relation.entity=atoi(key);
			relation.udp_port=atoi(content);
			
			if(relation.udp_port>0 && relation.entity>0)
			{	
				m_listUdpEntity.push_back(relation);
			}
		}
	}
}
 
void CGpcdef::SetThisMMPortByEntity(long eid)
{
	for(int i=0; i<m_listUdpEntity.size(); i++)
	{
		if(m_listUdpEntity[i].entity == eid)
		{
			m_nMSSAlarm=m_listUdpEntity[i].udp_port;
			break;
		}
	}
}

void CGpcdef::LoadNotMeetByType()
{
	for(int i=0; i<8; i++)
		mNotMeetInfo[i].Init();
	m_nNotMeetInfoCount=5;
	mNotMeetInfo[0].SetNotMeetInfo(0x01,"动车", "动");
	mNotMeetInfo[1].SetNotMeetInfo(0x02,"直特", "直");
	mNotMeetInfo[2].SetNotMeetInfo(0x04,"客车", "客");
	mNotMeetInfo[3].SetNotMeetInfo(0x08,"列车", "列");
	mNotMeetInfo[4].SetNotMeetInfo(0x10,"超限", "超");
}

bool CGpcdef::getNotMeetInfoByNoMeetFlag(WORD no_meet_flag, NotMeetInfo& info) const
{
	bool bFind=false;
	for(int i=0; i<m_nNotMeetInfoCount&&i<8; i++)
	{
		WORD wdFlag=mNotMeetInfo[i].nNoMeetFlag;
		if(wdFlag==(wdFlag&no_meet_flag))
		{
			info=mNotMeetInfo[i];
			bFind=true;
			break;
		}
	}

	return bFind;
}

bool CGpcdef::getNotMeetInfoByNo(BYTE idx, NotMeetInfo& info) const
{
	bool bFind=false;
	if(idx<m_nNotMeetInfoCount)
	{
		info=mNotMeetInfo[idx];
		bFind=true;
	}

	return bFind;
}
