#include "stdafx.h"
#include <stdio.h>
#include "tg_function_config.h"
#include "inifile.h"
#include "memshare.h" // for gpcdef
#include "DlgConfirm.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

NeibStationPlan::NeibStationPlan()
{
	nSectionNo=0;
	vecNeibStation.clear();
}
NeibStationPlan::~NeibStationPlan()
{
	nSectionNo=0;
	vecNeibStation.clear();
}
NeibStationPlan::NeibStationPlan(const NeibStationPlan& rhs)
{
	nSectionNo = rhs.nSectionNo;
	vecNeibStation.clear();
	vecNeibStation.assign(rhs.vecNeibStation.begin(), rhs.vecNeibStation.end());
}
NeibStationPlan& NeibStationPlan::operator=(const NeibStationPlan& rhs)
{
	if(this == &rhs)
		return *this;

	nSectionNo = rhs.nSectionNo;
	vecNeibStation.clear();
	vecNeibStation.assign(rhs.vecNeibStation.begin(), rhs.vecNeibStation.end());

	return *this;
}

CTGFunctionConfig::CTGFunctionConfig()
{
	m_bBasicStopSetNoTechStop=false;
	m_bShiftStopSetNoTechStop=false;
	m_bFreedrawStopSetNoTechStop=false;
	m_bNeibPlanPassengerTrainAddNotByBasic=false;
	m_bNeibPlanTrainAddOnlyByShift=false;
	m_bPassengerTrainEndSetNotTechStop=false;
	m_bPassengerTrainStartSetNotTechStop=false;
	m_bSelBackupSideByCfg=false;
	m_bShiftTrainSelFirstBackupSide=false;

	m_IfChoseMainSideWhilePass=FALSE;
	m_IfCalculateBasicTrainType=FALSE;
	m_NeiConsiderPassenger=FALSE;
	m_IfSetDefaultTechStop=FALSE;
	m_IfdChoseSideWhileAllFull=FALSE;
	m_nSuperSpaceTrainType=0xFF;
	m_nDefaultRunTime=0;
	m_nDefaultStopExtra=0;
	m_nDefaultStartExtra=0;
	m_nSectionCount=0;
	m_szDataPath[0]=0;
	m_bDefaultAccept=FALSE;
	m_bDefaultTerminate=FALSE;
	m_bLocalLogSendStPlan=FALSE;
	m_SendWholePlanCronTimes=3;
	m_bDefaultSendPlanClose=FALSE;
	strcpy(m_szTrainBZCXurl,"no file");
	strcpy(m_szWsDCMDurl1,"");
	strcpy(m_szWsDCMDurl2,"");
	m_nAutoShift1Time=8*60; //默认8:00 早换班
	m_nAutoShift2Time=20*60; //默认20:00 晚换班
	m_bMMPlanWSEnabled=FALSE;
	m_bMMPlanEditable=FALSE;
	m_nFileSaveFlag=1;
	//m_nFileKeepTimeHour=48;
	m_nFileKeepTimeHour=100;
	m_bDefaultTrigByPlan=TRUE;
	m_nSplitDefCount=0;
	m_nAutoNotifyDefCount=0;
	m_bCheckManualOper=FALSE;
	m_bDebugTDMsg=FALSE;
	
	m_nFJKStationCount=0;
	m_bCheckStPlanWithBasicSchd=true;
	m_bNewNeibModeAddTrain=true;
	m_bEnableOPMSNodeIndex=false;
	m_bStationBlockNotCheckSide=FALSE;
	m_bEnableOPMSBasicSchdSide=FALSE;
	m_bEnableOPMSShiftSchdSide=FALSE;
	m_bIsGoodTrainUseOPMSSide=true;

	m_bIsBasicValidSideByBasicSchd=false;
	m_bIsBasicValidSideByCalculate=false;

	m_bIsCalBasicSchdExitEntry=true;
	m_bIsCalShiftSchdExitEntry=true;

	m_bEnableTempAddTrain=FALSE;
	m_bEnablePasstoStayModifyArivalTime=TRUE;
	m_bEnableSelPasswayByDirectTrain=FALSE;
	m_bEnablePlanNoticeDCmd=FALSE;
	m_bDisEnableWSSaveDCmd=FALSE;
	m_bEnablePrintNoteTrainStopOneHour=FALSE;
	m_bEnableSelectTrainByLanzhou=FALSE;
	m_bEnableInputEngineId=FALSE;
	m_bEnableFreeDrawSetBasicTrainPlanTime=FALSE;
	
	m_bUnknowTrainStPlanRunType=FALSE;
	m_bCheckCtcModeStaCtrlStPlan=true;
	m_bCheckAllStationStPlan=TRUE;
	m_bCheckTrainMeetByNoMeet=FALSE;
	
	m_bDisenableSheetTab=FALSE;
	m_bDisenableSheetTabExit=FALSE;
	m_bEnableLanzhouEnginePlot=FALSE;
	m_bSecretTrainToSta=false;
	m_bEnableCheckPlatform=false;
	
	memset(m_szTrainId, 0, sizeof(m_szTrainId)/sizeof(m_szTrainId[0]));
	m_dcmd_prefix = 0;
	m_nDefaultSendPlanTRange=3;
	m_nDefaultNeibPlanTRange=3;
	m_bStplanDefaultSelLirc=false;
	m_bGetEngineFromPreConID=false;
	m_bSendStplanUseName=true;
	m_bEnableDeleteFromTmisBlock=true;
	
	m_bRemoveChineseCharSetTrainType=true;
	m_bEngineChangeGiveoutTip=false;
	m_bStplanSelectAllRecEntity=false;
	m_bTrainPassToStopUseMainSide=false;
	m_bTrainIdUseArriveId=true;
	
	m_bTaiYuanNeigbourPlan=false;
	m_bChangeRouteTip=false;
	
	m_bGoodTrainUseBasicSide=false;
	m_bSTIQueryBianzu=true;

	m_bIsSubaoShowSpace=false;
	m_bAddTrainShiftAndBasic=false;
	m_bEnableThirtyByPlan=false;
	m_bEnableLockWholeTrainSide=false;
	
	m_bDeleteRepeatDeleteStplan=true;
	m_bPlaySoundNeiPlan=false;
	m_bPlaySoundCrew=false;
	m_bEnableCzh=false;
	m_bEnableNewCzh=false;
	m_nAdjustAcceptRelation=1;
	m_nNeibplanAcceptRelation=1;
	m_bChangeTrainID=true;
	m_bEnableTDMS5=false;
	m_vecCheckPlanSection.clear();
	m_neigStationPlan.clear();
    
	m_nSetPassengerTrainNoTechStopLockSide=false;
	m_bSetStplanSpackRankByStation = false;
	m_bNeibPlanConsiderSameTrain = false;

	m_bStplanTrainIndexSaveInList = true;
	m_bCheckExitValueSetDefualtExit = false;
	m_bCheckEntryValueSetDefualtEntry = false;

	m_AutoAdjustOperFlag=0; //默认不自动调整
	m_AutoAdjustStepFlag=(1<<ADJUST_FLAGBIT_BLOCK); // 调整时默认考虑施工封锁

	m_nDepartDelayTimeOfBlockedABS=300;  // 默认区间封锁结束后，延时5分钟可发车
	m_vecNoChangeSideWhileStopPass.clear();

	m_bQuebaoQueryHaveChinese=false;
	m_bShowArriveTrainid=true;
	m_notUseSyNeibPlan.clear();

	m_bDisEnablDeleteRecordWhileLimitModifyActual=false;
	m_bClosePollingCommStatus=false;

	m_bCalExitEntryByTrainId=false;
	
	m_bHasNoLimitStation=false;

	m_bUseStringKiloMeter=false;
	m_bGoodTrainNotTechStopNeedPwd=false;

	m_bTrainSideUseBasicSide=true;

	m_bEnableRecvAllAdjustPlan=false;
	m_bIsAlarmOnlyCheckCRHTrainSide=false;
	m_bIsAlarmNotCheckSpackTrainMeetInfo=false;

	m_bIsNeibplanOnlyUseOPMSSchd=false;
	m_bShowMMPlanSkyLightButton=false;
	m_bShowMMPlanMaintainButton=false;

	m_nCheckElecSideWhenPassToStop=0;
	m_bEnablePlotMainSide=false;
	m_bQuestionStationPlanSelState=true;
	m_bEnableLogTDMessage=true;
	m_bQuestionStationPlanPassengerSta=false;

	m_bEnableNeibPlanFreightSameToPassenger=false;

	memset(m_szGSoapAuthName, 0, sizeof(m_szGSoapAuthName));
	memset(m_szGSoapUrl, 0, sizeof(m_szGSoapUrl));
	memset(m_szGSoapUrl2, 0, sizeof(m_szGSoapUrl2));
	mbSupportInterCityStPlan=false;
	m_bEnableRecvAdjustPlanTime=false;

	m_nNeibPlanAddNewTrainMode=0;
	m_bModifyTrainActAndGTIDSame=false;
	m_bAcceptTrainID=false;
		
	m_bNewModeLoginRefresh=false;
	m_nReportEarlyTime=0;

	m_bConsiderStplanSide=false;
	m_bCheckAndSetAlarm=false;
	m_nSectionStaEntryCount=0;   // 济南局,晏城--晏城北出入口设置配置
	memset(&m_specialEntry, 0, sizeof(m_specialEntry));
	m_otherBureaSection.clear();

	m_bEnableExactNeibPlanMode=false;
	m_nExactNeibPlanModeCrhStopTime=0;  // 动车组停车时分
	m_nExactNeibPlanModeOtherStopTime=0; // 普速列车停车时分
	m_bAcceptImportNote=false;
	m_bGiveChoicePathChange=false;
	m_nBasicPreHour=6;
	m_nBasicBackHour=6;
	m_bSetPreBasicTechStop=false;
	m_bNeedCompareTrainExpand=true;
	m_bNeedWaitMarkEnd=true;
	m_bGpcSendReqOtherMsg=true;
	m_bSendLogToLogsvr=false;
	m_bBasicDeletePassengerFreight=false; // 济南局,基本图可以分客车、货车分别删除
	m_bGenAllMarkByDeleteExist=false; //
	m_bConsiderTwicePassSameStation=false;
	m_bCompatibleOldStPlan=true;
	m_bEnableModifyGID=false;
	m_bCheckShiftTrainAddTime=false;
	m_NotSelAdjustPlan=false;
	m_bUsingWorkGoodMode=true;

	m_bBasicShowCompareDlg=false;
	m_bShiftShowCompareDlg=false;

	m_NotCheckSection.clear();
	m_bNeibPlanShowAcceptInfoOrder=false;
	m_bShowColorNeibPlanTip=false;
	m_bQuerySchdClose=true;

	m_bEnableSelGenBasicTrain=false;

	m_bDisableAutoTrainNo=false;
	m_bEraseBeforeModifyTrain=false;
	m_bNotMeetBySpeed=false;
	m_bDisableModifyPlanTm=true;
	m_bTrainNoAutoChgChoice=false;
	m_bCheckSideFull=false;
	m_bEnableScadElecArm=false;
	m_bCheckTDMSSide=true;
	m_bZeroGIDPasswayChange=false;
	m_bFormatView=false; // 默认不显示为总公司统一界面
	m_bCheckRelationSideCrash=false;
	m_bIsFindTrainByViewRange=false; // 如何报警重复车次, false:按照所在班次范围, true:可见范围
	m_bBatchAddTrainFindExistTrain=false;
}

const char *CTGFunctionConfig::GetDataPath() const
{
	static char tmp[256];
	sprintf(tmp,"%s/data",m_szDataPath);
	return tmp;
}
int   CTGFunctionConfig::GetDefaultRunTime() const
{
	return m_nDefaultRunTime*60;
}
int   CTGFunctionConfig::GetDefaultStopExtra() const
{
	return m_nDefaultStopExtra*60;

}

int   CTGFunctionConfig::GetDefaultStartExtra() const
{
	return m_nDefaultStartExtra*60;
}

BOOL  CTGFunctionConfig::IfCalculateBasicTrainType()  const
{
	return m_IfCalculateBasicTrainType;
}

BOOL  CTGFunctionConfig::NeiConsiderPassenger()  const
{
	return m_NeiConsiderPassenger;
}

BOOL  CTGFunctionConfig::IfSetDefaultTechStop() const
{
	return m_IfSetDefaultTechStop;
}

int   CTGFunctionConfig::GetSuperSpaceTrainType() const
{
	return m_nSuperSpaceTrainType;
}

BOOL  CTGFunctionConfig::IfdChoseSideWhileAllFull() const
{
	return m_IfdChoseSideWhileAllFull;
}

BOOL  CTGFunctionConfig::IfChoseMainSideWhilePass() const
{
	return m_IfChoseMainSideWhilePass;
}

BOOL  CTGFunctionConfig::IfCheckManualOper() const
{
	return m_bCheckManualOper;
}

void  CTGFunctionConfig::SetCheckManualOper(BOOL va) 
{
	m_bCheckManualOper=va;
}

BOOL  CTGFunctionConfig::IsDefaultAccept() const
{
	return m_bDefaultAccept;
}
BOOL  CTGFunctionConfig::IsDefaultTerminate() const
{
	return m_bDefaultTerminate;
}

BOOL CTGFunctionConfig::IsLocalLogSendStPlan() const
{
	return m_bLocalLogSendStPlan;
}
 
int CTGFunctionConfig::GetSendWholePlanCronTimes() const
{
	return m_SendWholePlanCronTimes;
}

BOOL CTGFunctionConfig::IsDefaultSendPlanClose() const
{
	return m_bDefaultSendPlanClose;
}

int  CTGFunctionConfig::GetAutoShift1Time() const
{
	return m_nAutoShift1Time;
}

int  CTGFunctionConfig::GetAutoShift2Time() const
{
	return m_nAutoShift2Time;
}

BOOL CTGFunctionConfig::IsMMPlanWSEnabled() const
{
	return m_bMMPlanWSEnabled;
}

BOOL CTGFunctionConfig::IsMMPlanEditable() const
{
	return m_bMMPlanEditable;
}

WORD CTGFunctionConfig::GetFileSaveFlag() const
{
	return m_nFileSaveFlag;
}

int  CTGFunctionConfig::GetFileKeepTimeHour() const
{
	return m_nFileKeepTimeHour;
}

BOOL CTGFunctionConfig::IsDefaultTrigByPlan() const
{
	return m_bDefaultTrigByPlan;
}
bool CTGFunctionConfig::IsEnableOPMSNodeIndex() const
{
	return m_bEnableOPMSNodeIndex;
}

BOOL CTGFunctionConfig::IsEnableOPMSBasicSchdSide(int nSection) const
{
	if(m_bEnableOPMSBasicSchdSide)
	{
		for(int i=0; i<m_vNotUseBasicSideSection.size(); i++)
		{
			if(m_vNotUseBasicSideSection[i]==nSection)
				return FALSE;
		}

		return TRUE;
	}
	return FALSE;
}

BOOL CTGFunctionConfig::IsEnableOPMSShiftSchdSide(int nSection) const
{
	if(m_bEnableOPMSShiftSchdSide)
	{
		for(int i=0; i<m_vNotUseShiftSideSection.size(); i++)
		{
			if(m_vNotUseShiftSideSection[i]==nSection)
				return FALSE;
		}

		return TRUE;
	}
	return FALSE;
}

bool  CTGFunctionConfig::IsGoodTrainUseOPMSSide(int train_type) const
{
	for(int i=0; i<m_vConfigGoodTrainTypesOfUseOPMSSide.size(); i++)
	{
		if(m_vConfigGoodTrainTypesOfUseOPMSSide[i]==train_type)
		{
			return true;
		}
	}
	return m_bIsGoodTrainUseOPMSSide;
}

bool  CTGFunctionConfig::IsPessagerTrainNotUseOPMSSide(int section, int train_type)
{
	bool bNotUseOPMSSide=false;
	if(mCalSideTrainType.nSection!=section)
	{
		LoadCalSideTrainType(section);
		mCalSideTrainType.nSection = section;
	}
	for(int n=0; n<mCalSideTrainType.nCount; n++)
	{
		if(mCalSideTrainType.nTrainType[n]==train_type)
		{
			bNotUseOPMSSide=true;
			break;
		}
	}

	return bNotUseOPMSSide;
}

bool  CTGFunctionConfig::IsBasicValidSideByBasicSchd() const
{
	return m_bIsBasicValidSideByBasicSchd;
}

bool  CTGFunctionConfig::IsBasicValidSideByCalculate() const
{
	return m_bIsBasicValidSideByCalculate;
}

bool  CTGFunctionConfig::IsCheckStPlanWithBasicSchd() const
{
	return m_bCheckStPlanWithBasicSchd;
}

bool CTGFunctionConfig::IsNewNeibModeAddTrain() const
{
	return m_bNewNeibModeAddTrain;
}

bool CTGFunctionConfig::IsCalBasicSchdExitEntry() const
{
	if(m_bEnableOPMSNodeIndex) // 前提要求opms送节点站序
		return m_bIsCalBasicSchdExitEntry;
	else
		return false;
}

bool CTGFunctionConfig::IsCalShiftSchdExitEntry() const
{
	if(m_bEnableOPMSNodeIndex) // 前提要求opms送节点站序
		return m_bIsCalShiftSchdExitEntry;
	else
		return false;
}

BOOL CTGFunctionConfig::IsEnableTempAddTrain() const
{
	return m_bEnableTempAddTrain;
}

BOOL CTGFunctionConfig::IsEnablePasstoStayModifyArivalTime() const
{
	return m_bEnablePasstoStayModifyArivalTime;
}

BOOL CTGFunctionConfig::IsFreeDrawSetBasicTrainPlanTime() const
{ 
	return m_bEnableFreeDrawSetBasicTrainPlanTime;
}

BOOL CTGFunctionConfig::IsEnableUnknowTrainStPlanRunType() const
{ 
	return m_bUnknowTrainStPlanRunType;
}

bool CTGFunctionConfig::IsCheckCtcModeStaCtrlStPlan() const
{ 
	return m_bCheckCtcModeStaCtrlStPlan;
}

BOOL CTGFunctionConfig::IsCheckAllStationStPlan() const
{
	return m_bCheckAllStationStPlan;
}

BOOL CTGFunctionConfig::IsCheckTrainMeetByNoMeet() const
{
	return m_bCheckTrainMeetByNoMeet;
}

BOOL CTGFunctionConfig::IsDisenableSheetTab() const
{
	return m_bDisenableSheetTab;
}

BOOL CTGFunctionConfig::IsDisenableSheetTabExit() const
{
	return m_bDisenableSheetTabExit;
}

BOOL CTGFunctionConfig::IsEnableSelPasswayByDirectTrain() const
{
	return m_bEnableSelPasswayByDirectTrain;
}

BOOL CTGFunctionConfig::IsEnableSendPlanNoticeDCmd() const
{
	return m_bEnablePlanNoticeDCmd;
}

BOOL CTGFunctionConfig::IsDisEnableWSSaveDCmd() const
{
	return m_bDisEnableWSSaveDCmd;
}

BOOL CTGFunctionConfig::IsEnablePrintNoteTrainStopOneHour() const
{
	return m_bEnablePrintNoteTrainStopOneHour;
}

BOOL  CTGFunctionConfig::IsEnableSelectTrainByLanzhou() const
{
	return m_bEnableSelectTrainByLanzhou;
}

BOOL  CTGFunctionConfig::IsEnableInputEngineId() const
{
	return m_bEnableInputEngineId;
}

BOOL  CTGFunctionConfig::IsEnableLanzhouEnginePlot() const
{
	return m_bEnableLanzhouEnginePlot;
}

BOOL CTGFunctionConfig::IsStationBlockNotCheckSide() const
{
	return m_bStationBlockNotCheckSide;
}

BOOL CTGFunctionConfig::IsNeibPlanConsiderSameTrain() const
{
	return m_bNeibPlanConsiderSameTrain;
}

BOOL CTGFunctionConfig::IsStplanTrainIndexSaveInList() const
{
	return m_bStplanTrainIndexSaveInList;
}

BOOL CTGFunctionConfig::IsCheckExitValueSetDefualtExit() const
{
	return m_bCheckExitValueSetDefualtExit;
}

BOOL CTGFunctionConfig::IsCheckEntryValueSetDefualtEntry() const
{
	return m_bCheckEntryValueSetDefualtEntry;
}

int CTGFunctionConfig::GetAutoAdjustOperFlag() const
{
	return m_AutoAdjustOperFlag;
}

void  CTGFunctionConfig::SetAutoAdjustOperFlag(int va) 
{
	m_AutoAdjustOperFlag=va;
}

int  CTGFunctionConfig::GetTmisSectionID(int con_id,char *dispid,char *jhdispid,char *kddispid)
{
	for(int i=0;i<m_nSectionCount;i++)
	{
	  if(m_aSectionList[i].section_id==con_id)
	  {
		  if(dispid)
			  strcpy(dispid,m_aSectionList[i].disid);
		  if(jhdispid)
			  strcpy(jhdispid,m_aSectionList[i].jhdisid);
		  if(kddispid)
			  strcpy(kddispid,m_aSectionList[i].kddisid);
		  return m_aSectionList[i].tmis_id;
	  }
	}
	return -1;
}

bool CTGFunctionConfig::IsCheckPlanValid(int con_id)
{ 
	for(int i=0; i<m_vecCheckPlanSection.size(); i++)
	{
		if(m_vecCheckPlanSection[i] == con_id)
			return false;
	}

	return true;
}

int  CTGFunctionConfig::GetSectionIDByTmisID(int tmis_id,char *dispid)
{
	for(int i=0;i<m_nSectionCount;i++)
	{
	  if(m_aSectionList[i].tmis_id==tmis_id)
	  {
		  if(dispid)
			  strcpy(dispid,m_aSectionList[i].disid);
		  return m_aSectionList[i].section_id;
	  }
	}
	return -1;
}

bool CTGFunctionConfig::IsNeibPlanNotAddNewTrainIFSourceNotShift() const
{ 
	if(m_nNeibPlanAddNewTrainMode==2)
		return true;
	return false;
}
	
bool CTGFunctionConfig::IsNeibPlanNotAddNewTrain() const
{ 
	if(m_nNeibPlanAddNewTrainMode==1)
		return true;
	return false;
}

BOOL  CTGFunctionConfig::InitData(CString strPath)
{
	m_vctPlanTip.clear();
	CString cfgfile = strPath + "\\data\\tg_logic.cfg";
	CIniFile t;
	t.OpenFile(cfgfile);
	if(!t.IsOK())
	{
		CString tmp;
		tmp.Format("加载用户配置文件(%s)失败！", cfgfile);
		AfxMessageBox(tmp,MB_OK);
		return FALSE;
	}
	LoadTgdef();

	int tmp;
	char tmpstr[512];
	if(t.GetKeyValue("PARA","LOGIC_SECTION",tmpstr,sizeof(tmpstr)))
	{
		char *p=strtok(tmpstr,", ");
		while(p && m_nSectionCount<MAX_LOGIC_SECTION_NUM)
		{
			m_aSectionList[m_nSectionCount].section_id=atoi(p);
			m_aSectionList[m_nSectionCount].tmis_id=m_aSectionList[m_nSectionCount].section_id;
			m_aSectionList[m_nSectionCount].disid[0]=0;
			m_aSectionList[m_nSectionCount].jhdisid[0]=0;
			m_aSectionList[m_nSectionCount].kddisid[0]=0;
			m_nSectionCount++;
			p=strtok(NULL,", ");
		}
	}
	else
	{
		CString tmp;
		tmp.Format("加载用户配置文件(%s),获取PARA-LOGIC_SECTION失败！", cfgfile);
		AfxMessageBox(tmp,MB_OK);
		return FALSE;
	}
	
	strncpy(m_szDataPath, strPath.GetBuffer(), sizeof(m_szDataPath)-1);
	m_szDataPath[sizeof(m_szDataPath)-1] = 0;
	
	memset(m_szTrainId, 0, sizeof(m_szTrainId)/sizeof(m_szTrainId[0]));
	if(t.GetKeyValue("PARA","TRAIN_ID",tmpstr,sizeof(tmpstr)))
	{
		strncpy(m_szTrainId,tmpstr,24);
		m_szTrainId[24]=0;
	}
	
	if(t.GetKeyValue("PARA","BZCX_URL",tmpstr,sizeof(tmpstr)))
	{
		strcpy(m_szTrainBZCXurl,tmpstr);
	}
	if(t.GetKeyValue("PARA","WSDCMD_URL1",tmpstr,sizeof(tmpstr)))
	{
		strncpy(m_szWsDCMDurl1,tmpstr,sizeof(m_szWsDCMDurl1)-1);
	}
	if(t.GetKeyValue("PARA","WSDCMD_URL2",tmpstr,sizeof(tmpstr)))
	{
		strncpy(m_szWsDCMDurl2,tmpstr,sizeof(m_szWsDCMDurl2)-1);
	}
	tmp=t.GetIntKeyValue("PARA","AutoShift1Time");
	if(tmp>0)
		m_nAutoShift1Time=tmp*60;
	tmp=t.GetIntKeyValue("PARA","AutoShift2Time");
	if(tmp>0)
		m_nAutoShift2Time=tmp*60;
	
	// 调度命令前缀
	tmp =t.GetIntKeyValue("PARA","DCMD_PREFIX");
	if(tmp >= 0)
		m_dcmd_prefix = tmp;

	if(t.GetKeyValue("GSOAP","Authority",tmpstr,sizeof(tmpstr)))
	{
		strncpy(m_szGSoapAuthName,tmpstr,sizeof(m_szGSoapAuthName)-1);
		m_szGSoapAuthName[sizeof(m_szGSoapAuthName)-1]=0;
	}
	if(t.GetKeyValue("GSOAP","URL",tmpstr,sizeof(tmpstr)))
	{
		strncpy(m_szGSoapUrl,tmpstr,sizeof(m_szGSoapUrl)-1);
		m_szGSoapUrl[sizeof(m_szGSoapUrl)-1]=0;
	}
	if(t.GetKeyValue("GSOAP","URL2",tmpstr,sizeof(tmpstr)))
	{
		strncpy(m_szGSoapUrl2,tmpstr,sizeof(m_szGSoapUrl2)-1);
		m_szGSoapUrl2[sizeof(m_szGSoapUrl2)-1]=0;
	}
	
	if(t.GetIntKeyValue("FUNCTION","ChoseMainSideWhilePass")>0)
		m_IfChoseMainSideWhilePass=TRUE;
	if(t.GetIntKeyValue("FUNCTION","CalculateBasicTrainType")>0)
		m_IfCalculateBasicTrainType=TRUE;

	if(t.GetIntKeyValue("FUNCTION","QuebaoQueryHaveChinese")>0)
		m_bQuebaoQueryHaveChinese=true;

	if(t.GetIntKeyValue("FUNCTION","NotShowArriveTrainid")>0)
		m_bShowArriveTrainid=false;
	
	if(t.GetIntKeyValue("FUNCTION","NeiConsiderPassenger")>0)
		m_NeiConsiderPassenger=TRUE;

	if(t.GetIntKeyValue("FUNCTION","SetDefaultTechStop")>0)
		m_IfSetDefaultTechStop=TRUE;
	if(t.GetIntKeyValue("FUNCTION","ChoseSideWhileAllFull")>0)
		m_IfdChoseSideWhileAllFull=TRUE;	
	if((tmp=t.GetIntKeyValue("FUNCTION","SuperSpaceTrainType"))>=0)
		m_nSuperSpaceTrainType=tmp;
	if((tmp=t.GetIntKeyValue("FUNCTION","DefaultRunTime"))>=0)
		m_nDefaultRunTime=tmp;
	if((tmp=t.GetIntKeyValue("FUNCTION","DefaultStopExtra"))>=0)
		m_nDefaultStopExtra=tmp;
	if((tmp=t.GetIntKeyValue("FUNCTION","DefaultStartExtra"))>=0)
		m_nDefaultStartExtra=tmp;
	if((tmp=t.GetIntKeyValue("FUNCTION","LocalLogSendStPlan"))>=0)
		m_bLocalLogSendStPlan=TRUE;
	if((tmp=t.GetIntKeyValue("FUNCTION","SendWholePlanCronTimes"))>=0)
		m_SendWholePlanCronTimes=tmp;
	if ((tmp=t.GetIntKeyValue("FUNCTION","StplanReportEarlyTime"))>=0)
		m_nReportEarlyTime=tmp;
	if((tmp=t.GetIntKeyValue("FUNCTION","DefaultSendPlanClose"))>0)
		m_bDefaultSendPlanClose=TRUE;
	else
        m_bDefaultSendPlanClose=FALSE;

	m_nDefaultSendPlanTRange=3;
	if((tmp=t.GetIntKeyValue("FUNCTION","DefaultSendPlanTRange"))>0)
		m_nDefaultSendPlanTRange=tmp;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","DefaultNeibPlanTRange"))>0)
		m_nDefaultNeibPlanTRange=tmp;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","SecretTrainPlan"))>0)
		m_bSecretTrainToSta=true;
	else
        m_bSecretTrainToSta=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","BasicStopSetNoTechStop"))>0)
		m_bBasicStopSetNoTechStop=true;
	else
        m_bBasicStopSetNoTechStop=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","ShiftStopSetNoTechStop"))>0)
		m_bShiftStopSetNoTechStop=true;
	else
        m_bShiftStopSetNoTechStop=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","FreedrawStopSetNoTechStop"))>0)
		m_bFreedrawStopSetNoTechStop=true;
	else
        m_bFreedrawStopSetNoTechStop=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","NeibPlanPassengerTrainAddNotByBasic"))>0)
	{
		m_bNeibPlanTrainAddOnlyByShift=false; // 开启南昌模式,关闭济南模式,二者不共存
		m_bNeibPlanPassengerTrainAddNotByBasic=true;
	}	
	
	if((tmp=t.GetIntKeyValue("FUNCTION","NeibPlanAddTrainOnlyByShift"))>0)
	{
		m_bNeibPlanTrainAddOnlyByShift=true;  // 开启济南模式,关闭南昌模式,二者不共存
		m_bNeibPlanPassengerTrainAddNotByBasic=false;
	}	

	tmp=t.GetIntKeyValue("FUNCTION","NeibPlanAddNewTrainMode");
	if(tmp==1)
		m_nNeibPlanAddNewTrainMode = 1;
	else if(tmp==2)
		m_nNeibPlanAddNewTrainMode = 2;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","PassengerTrainStartSetNotTechStop"))>0)
		m_bPassengerTrainStartSetNotTechStop=true;

	if((tmp=t.GetIntKeyValue("FUNCTION","PassengerTrainEndSetNotTechStop"))>0)
		m_bPassengerTrainEndSetNotTechStop=true;

	if((tmp=t.GetIntKeyValue("FUNCTION","SelBackupSideByCfg"))>0)
		m_bSelBackupSideByCfg=true;
		
	if((tmp=t.GetIntKeyValue("FUNCTION","ShiftTrainSelFirstBackupSide"))>0)
	{
		m_bShiftTrainSelFirstBackupSide=true;
		m_bSelBackupSideByCfg=true;
	}

	if ((tmp=t.GetIntKeyValue("FUNCTION","GoodTrainNotTechStopNeedPwd"))>0)
		m_bGoodTrainNotTechStopNeedPwd=true;
	else
		m_bGoodTrainNotTechStopNeedPwd=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableCheckPlatform"))>0)
		m_bEnableCheckPlatform=true;
	else
        m_bEnableCheckPlatform=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","MMPlanWSEnabled"))>0)  
        m_bMMPlanWSEnabled=TRUE;
	else
		m_bMMPlanWSEnabled=FALSE;

	if((tmp=t.GetIntKeyValue("FUNCTION","MMPlanEditable"))>0)
        m_bMMPlanEditable=TRUE;
	else
		m_bMMPlanEditable=FALSE;
	if((tmp=t.GetIntKeyValue("FUNCTION","FileSaveFlag"))>=0)
        m_nFileSaveFlag=tmp;
	if((tmp=t.GetIntKeyValue("FUNCTION","FileKeepTimeHour"))>=0)
        m_nFileKeepTimeHour=tmp;

	if((tmp=t.GetIntKeyValue("FUNCTION","DefaultTrigByPlan"))==0)
        m_bDefaultTrigByPlan=FALSE;
	else
		m_bDefaultTrigByPlan=TRUE;

	if((tmp=t.GetIntKeyValue("FUNCTION","CheckManualOper"))>0)
		m_bCheckManualOper=TRUE;
	else
		m_bCheckManualOper=FALSE;

	if((tmp=t.GetIntKeyValue("FUNCTION","CheckAndSetAlarm"))>0)
		m_bCheckAndSetAlarm=true;
	else
		m_bCheckAndSetAlarm=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableOPMSNodeIndex"))>0)
		m_bEnableOPMSNodeIndex=true;
	else
		m_bEnableOPMSNodeIndex=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","OldNeibModeAddTrain"))>0)
		m_bNewNeibModeAddTrain=false;
	else
		m_bNewNeibModeAddTrain=true;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","NotCheckStPlanWithBasicSchd"))>0)
		m_bCheckStPlanWithBasicSchd=false;
	else
		m_bCheckStPlanWithBasicSchd=true;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","NotCalBasicSchdExitEntry"))>0)
		m_bIsCalBasicSchdExitEntry=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","NotCalShiftSchdExitEntry"))>0)
		m_bIsCalShiftSchdExitEntry=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableOPMSBasicSchdSide"))>0)
		m_bEnableOPMSBasicSchdSide=TRUE;
	else
		m_bEnableOPMSBasicSchdSide=FALSE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableOPMSShiftSchdSide"))>0)
		m_bEnableOPMSShiftSchdSide=TRUE;
	else
		m_bEnableOPMSShiftSchdSide=FALSE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","GoodTrainNotUseOPMSSide"))>0)
		m_bIsGoodTrainUseOPMSSide=false;
	else
		m_bIsGoodTrainUseOPMSSide=true;

	if((tmp=t.GetIntKeyValue("FUNCTION","EnableValidBasicSideBySchd"))>0)
		m_bIsBasicValidSideByBasicSchd=true;
	else
		m_bIsBasicValidSideByBasicSchd=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","EnableValidBasicSideByCalcu"))>0)
		m_bIsBasicValidSideByCalculate=true;
	else
		m_bIsBasicValidSideByCalculate=false;

	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableTempAddTrain"))>0)
		m_bEnableTempAddTrain=TRUE;
	else
		m_bEnableTempAddTrain=FALSE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","DisEnableWSSaveDCmd"))>0)
		m_bDisEnableWSSaveDCmd=TRUE;
	else
		m_bDisEnableWSSaveDCmd=FALSE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnablePrintNoteTrainStopOneHour"))>0)
		m_bEnablePrintNoteTrainStopOneHour=TRUE;
	else
		m_bEnablePrintNoteTrainStopOneHour=FALSE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableSelectTrainByLanzhou"))>0)
		m_bEnableSelectTrainByLanzhou=TRUE;
	else
		m_bEnableSelectTrainByLanzhou=FALSE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableLanzhouEnginePlot"))>0)
		m_bEnableLanzhouEnginePlot=TRUE;
	else
		m_bEnableLanzhouEnginePlot=FALSE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableInputEngineId"))>0)
		m_bEnableInputEngineId=TRUE;
	else
		m_bEnableInputEngineId=FALSE;

	if((tmp=t.GetIntKeyValue("FUNCTION","EnableSendPlanNoticeDCmd"))>0)
		m_bEnablePlanNoticeDCmd=TRUE;
	else
		m_bEnablePlanNoticeDCmd=FALSE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableSelPasswayByDirectTrain"))>0)
		m_bEnableSelPasswayByDirectTrain=TRUE;
	else
		m_bEnableSelPasswayByDirectTrain=FALSE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnablePasstoStayNoModifyArivalTime"))>0)
		m_bEnablePasstoStayModifyArivalTime=FALSE;
	else
		m_bEnablePasstoStayModifyArivalTime=TRUE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","UnknowTrainStPlanRunType"))>0)
		m_bUnknowTrainStPlanRunType=TRUE;
	else
		m_bUnknowTrainStPlanRunType=FALSE;

	if((tmp=t.GetIntKeyValue("FUNCTION","NoCheckCtcModeStaCtrlStPlan"))>0)
		m_bCheckCtcModeStaCtrlStPlan=false;
	else
		m_bCheckCtcModeStaCtrlStPlan=true;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","OnlyCheckCTCStationStPlan"))>0)
		m_bCheckAllStationStPlan=FALSE;
	else
		m_bCheckAllStationStPlan=TRUE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableFreeDrawSetBasicTrainPlanTime"))>0)
		m_bEnableFreeDrawSetBasicTrainPlanTime=TRUE;
	else
		m_bEnableFreeDrawSetBasicTrainPlanTime=FALSE;

	if((tmp=t.GetIntKeyValue("FUNCTION","CheckTrainMeetByNoMeet"))>0)
		m_bCheckTrainMeetByNoMeet=TRUE;
	else
		m_bCheckTrainMeetByNoMeet=FALSE;
	

	if((tmp=t.GetIntKeyValue("FUNCTION","StationBlockNotCheckSide"))>0)
		m_bStationBlockNotCheckSide=TRUE;
	else
		m_bStationBlockNotCheckSide=FALSE;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","DisenableSheetTab"))>0)
		m_bDisenableSheetTab=TRUE;
	else
		m_bDisenableSheetTab=FALSE;

	if((tmp=t.GetIntKeyValue("FUNCTION","DisenableSheetTabExit"))>0)
		m_bDisenableSheetTabExit=TRUE;

	if((tmp=t.GetIntKeyValue("FUNCTION","StplanDefaultSelLirc"))>0)
		m_bStplanDefaultSelLirc=true;
	else
		m_bStplanDefaultSelLirc=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","StplanDefaultSelAllRecEntity"))>0)
		m_bStplanSelectAllRecEntity=true;
	else
		m_bStplanSelectAllRecEntity=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","NoRemoveChineseCharSetTrainType"))>0)
		m_bRemoveChineseCharSetTrainType=false;
	else
		m_bRemoveChineseCharSetTrainType=true;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","PassToStopUseMainSide"))>0)
		m_bTrainPassToStopUseMainSide=true;
	else
		m_bTrainPassToStopUseMainSide=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","TrainIdUseArriveId"))<=0)
		m_bTrainIdUseArriveId=false;
	else
		m_bTrainIdUseArriveId=true;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","TaiYuanNeigbourPlan"))>0)
		m_bTaiYuanNeigbourPlan=true;
	else
		m_bTaiYuanNeigbourPlan=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","IsCheckRouteforCrhAndImp"))>0)
		m_bChangeRouteTip=true;
	else
		m_bChangeRouteTip=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","GoodTrainSideSameToPlan"))>0)
		m_bGoodTrainUseBasicSide=true;
	else
		m_bGoodTrainUseBasicSide=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","NotSTIQueryBianzu"))>0)
		m_bSTIQueryBianzu=false;
	else
		m_bSTIQueryBianzu=true;

	if((tmp=t.GetIntKeyValue("FUNCTION","EnableLockTrainSide"))>0)
		m_bEnableLockWholeTrainSide=true;
	else
		m_bEnableLockWholeTrainSide=false;

		
	if((tmp=t.GetIntKeyValue("FUNCTION","CrewPlaySound"))>0)
		m_bPlaySoundCrew=true;
	else
		m_bPlaySoundCrew=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","NeiPlanPlaySound"))>0)
		m_bPlaySoundNeiPlan=true;
	else
		m_bPlaySoundNeiPlan=false;

	
	if((tmp=t.GetIntKeyValue("FUNCTION","DeleteRepeatDeleteStplan"))>0)
		m_bDeleteRepeatDeleteStplan=false;
	else
		m_bDeleteRepeatDeleteStplan=true;
	
	tmp=t.GetIntKeyValue("FUNCTION","EnableCzh");
	if(tmp==1)
	{
		m_bEnableCzh=true;
		m_bEnableNewCzh=false;
	}
	else if(tmp==2)
	{
		m_bEnableCzh=false;
		m_bEnableNewCzh=true;
	}
	else
	{
		m_bEnableCzh=false;
		m_bEnableNewCzh=false;
	}

	if((tmp=t.GetIntKeyValue("FUNCTION","NotSetCzhTrainID"))>0)
	    m_bChangeTrainID=false;
	else
		m_bChangeTrainID=true;

	tmp=t.GetIntKeyValue("FUNCTION","AcceptAdjustTrainRelation");
	if(tmp==-1 || tmp==1)
		m_nAdjustAcceptRelation=1;
	else
		m_nAdjustAcceptRelation=0;
			
	tmp=t.GetIntKeyValue("FUNCTION","AcceptNeibplanTrainRelation");
	if(tmp==-1 || tmp==1)
		m_nNeibplanAcceptRelation=1;
	else
		m_nNeibplanAcceptRelation=0;
	
	if(m_bEnableNewCzh)
	{
		m_bChangeTrainID=false;
	}
	else
	{
		m_nNeibplanAcceptRelation=0;
		m_nAdjustAcceptRelation=0;
	}
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableTDMS5"))>0)
		m_bEnableTDMS5=true;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","EnableThirtyByPlan"))>0)
		m_bEnableThirtyByPlan=true;
	else
		m_bEnableThirtyByPlan=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","FreedrawAddTrainShiftBasic"))>0)
		m_bAddTrainShiftAndBasic=true;
	else
		m_bAddTrainShiftAndBasic=false;
	

	if((tmp=t.GetIntKeyValue("FUNCTION","EngineChangeGiveoutTip"))>0)
		m_bEngineChangeGiveoutTip=true;
	else
		m_bEngineChangeGiveoutTip=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","GetEngineFromPreConID"))>0)
		m_bGetEngineFromPreConID=true;
	else
		m_bGetEngineFromPreConID=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","StplanSendDdyCode"))>0)
		m_bSendStplanUseName=false;
	else
		m_bSendStplanUseName=true;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","DisenableDeleteFromTmisBlock"))>0)
		m_bEnableDeleteFromTmisBlock=false;
	else
		m_bEnableDeleteFromTmisBlock=true;

	if((tmp=t.GetIntKeyValue("FUNCTION","NeibPlanConsiderSameTrain"))>0)
		m_bNeibPlanConsiderSameTrain=true;
	else
		m_bNeibPlanConsiderSameTrain=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","StplanTrainIndexNotSaveInList"))>0)
		m_bStplanTrainIndexSaveInList=false;
	else
		m_bStplanTrainIndexSaveInList=true;

	if((tmp=t.GetIntKeyValue("FUNCTION","CheckExitValueSetDefualtExit"))>0)
		m_bCheckExitValueSetDefualtExit=true;
	else
		m_bCheckExitValueSetDefualtExit=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","CheckEntryValueSetDefualtEntry"))>0)
		m_bCheckEntryValueSetDefualtEntry=true;
	else
		m_bCheckEntryValueSetDefualtEntry=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","AutoAdjustOperFlag"))>0)
		m_AutoAdjustOperFlag=tmp;
	else
		m_AutoAdjustOperFlag=0;

	if((tmp=t.GetIntKeyValue("FUNCTION","AutoAdjustStepFlag"))>0)
		m_AutoAdjustStepFlag=tmp;

	if((tmp=t.GetIntKeyValue("FUNCTION","DepartDelayTimeOfBlockedABS"))>0)
		m_nDepartDelayTimeOfBlockedABS=tmp;

	//广州局要求，设置旅客列车办客站默认锁定股道
	if((tmp=t.GetIntKeyValue("FUNCTION","SetPassengerTrainNoTechStopLockSide"))>0)
		m_nSetPassengerTrainNoTechStopLockSide=true;
	else
		m_nSetPassengerTrainNoTechStopLockSide=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","EnableShowVirtualStation"))>0)
		gpcdef.SetShowVirtualStation(true);
	else
		gpcdef.SetShowVirtualStation(false);

	if((tmp=t.GetIntKeyValue("FUNCTION","DisenableShowKiloMeterButton"))>0)
		gpcdef.SetShowKiloMeterButton(false);
	else
		gpcdef.SetShowKiloMeterButton(true);

	if((tmp=t.GetIntKeyValue("FUNCTION","ShowTransInfoButton"))>0)
		gpcdef.SetShowTransInfoButton(true);
	else
		gpcdef.SetShowTransInfoButton(false);

	if((tmp=t.GetIntKeyValue("FUNCTION","BasicSideNotByPrevBasicSchedule"))>0)
		m_bTrainSideUseBasicSide=false;
	else
		m_bTrainSideUseBasicSide=true;

	if((tmp=t.GetIntKeyValue("FUNCTION","EnableRecvAllAdjustPlan"))>0)
		m_bEnableRecvAllAdjustPlan=true;
	else
		m_bEnableRecvAllAdjustPlan=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","IsAlarmOnlyCheckCRHTrainSide"))>0)
		m_bIsAlarmOnlyCheckCRHTrainSide=true;
	else
		m_bIsAlarmOnlyCheckCRHTrainSide=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","IsAlarmNotCheckSpackTrainMeet"))>0)
		m_bIsAlarmNotCheckSpackTrainMeetInfo=true;
	else
		m_bIsAlarmNotCheckSpackTrainMeetInfo=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","CheckElecSideWhenPassToStopFlag"))>0)
		m_nCheckElecSideWhenPassToStop=tmp;

	if((tmp=t.GetIntKeyValue("FUNCTION","NotShowAutoAdjustOperFlag"))>0)
		gpcdef.SetShowAutoAdjust(false);
	else
		gpcdef.SetShowAutoAdjust(true);

	if((tmp=t.GetIntKeyValue("FUNCTION","IsSubaoDlgShowSpace"))>0)
		m_bIsSubaoShowSpace=true;
	else
		m_bIsSubaoShowSpace=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","IsEnableRecvAdjustPlanTime"))>0)
		m_bEnableRecvAdjustPlanTime=true;
	else
		m_bEnableRecvAdjustPlanTime=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","ModifyTrainActAndGTIDSame"))>0)
		m_bModifyTrainActAndGTIDSame=true;
	else
		m_bModifyTrainActAndGTIDSame=false;

	if((tmp=t.GetIntKeyValue("FUNCTION","AcceptAdjustTrainID"))>0)
		m_bAcceptTrainID=true;
	else
		m_bAcceptTrainID=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","ZeroGIDPasswayChange"))>0)
		m_bZeroGIDPasswayChange=true;
	else
		m_bZeroGIDPasswayChange=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","NewModeLoginRefresh"))>0)
		m_bNewModeLoginRefresh=true;
	else
		m_bNewModeLoginRefresh=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","StplanNextPreSideSet"))>0)
		m_bConsiderStplanSide=true;
	else
		m_bConsiderStplanSide=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","CheckRelationSideCrash"))>0)
		m_bCheckRelationSideCrash=true;
	else
		m_bCheckRelationSideCrash=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","AddTrainFindByViewRange"))>0)
		m_bIsFindTrainByViewRange=true;
	else
		m_bIsFindTrainByViewRange=false;
	
	if((tmp=t.GetIntKeyValue("FUNCTION","BatchAddTrainFindExistTrain"))>0)
		m_bBatchAddTrainFindExistTrain=true;
	else
		m_bBatchAddTrainFindExistTrain=false;
	
	//列车本站无超限时,下发阶段计划中超限等级是否按照车站超限级别下发
	if((tmp=t.GetIntKeyValue("FUNCTION","SetStplanSpackRankByStation"))>0)
		m_bSetStplanSpackRankByStation=true;
	else
		m_bSetStplanSpackRankByStation=false;

	//太原局,封闭运行图禁止删除运行线 和 修改接入、始发、交出和终到
	if((tmp=t.GetIntKeyValue("FUNCTION","NotDeleteRecordLimitModifyActual"))>0)
		m_bDisEnablDeleteRecordWhileLimitModifyActual=true;
	else
		m_bDisEnablDeleteRecordWhileLimitModifyActual=false;

	//处理运行图与commsvr闪断问题
	if((tmp=t.GetIntKeyValue("FUNCTION","ClosePollingCommStatus"))>0)
		m_bClosePollingCommStatus=true;
	else
		m_bClosePollingCommStatus=false;

	//处理OPMS接收计划处理首站的入口和末站出口按照整车车次单双号判断
	if((tmp=t.GetIntKeyValue("FUNCTION","ExitEntryByTrainId"))>0)
		m_bCalExitEntryByTrainId=true;
	else
		m_bCalExitEntryByTrainId=false;

	if ((tmp=t.GetIntKeyValue("FUNCTION","UseStringKiloMeter"))>0)
		m_bUseStringKiloMeter=true;
	else
		m_bUseStringKiloMeter=false;

	// (上海局)邻台计划新增列车时,仅支持OPMS最新表现形式的运行线
	if ((tmp=t.GetIntKeyValue("FUNCTION","IsNeibplanOnlyUseOPMSNewYL"))>0)
		m_bIsNeibplanOnlyUseOPMSSchd=true;
	else
		m_bIsNeibplanOnlyUseOPMSSchd=false;

	// (南昌局)施工计划界面增加两个按钮,分别过滤施工和天窗
	if ((tmp=t.GetIntKeyValue("FUNCTION","ShowMMPlanSkyLightButton"))>0)
		m_bShowMMPlanSkyLightButton=true;
	else
		m_bShowMMPlanSkyLightButton=false;

	// (太原局)施工计划界面增加两个按钮,分别过滤施工计划和维修计划
	if ((tmp=t.GetIntKeyValue("FUNCTION","ShowMMPlanMaintainButton"))>0)
		m_bShowMMPlanMaintainButton=true;
	else
		m_bShowMMPlanMaintainButton=false;

	// 北京局要求 打印运行图时能够加粗正线股道
	if ((tmp=t.GetIntKeyValue("FUNCTION","EnablePlotMainSide"))>0)
		m_bEnablePlotMainSide=true;
	else
		m_bEnablePlotMainSide=false;
	
	if ((tmp=t.GetIntKeyValue("FUNCTION","AcceptOPMSImportNote"))>0)
		m_bAcceptImportNote=true;
	else
		m_bAcceptImportNote=false;
	
	if ((tmp=t.GetIntKeyValue("FUNCTION","GiveChoiceWhenPathChange"))>0)
		m_bGiveChoicePathChange=true;
	else
		m_bGiveChoicePathChange=false;
	
	// 沈阳局要求 不提示接收计划钩选情况报警
	if ((tmp=t.GetIntKeyValue("FUNCTION","NotQuestionStationPlanSelState"))>0)
		m_bQuestionStationPlanSelState=false;
	else
		m_bQuestionStationPlanSelState=true;

	// 上海局徐明要求 阶段计划提醒是否下发客调旅服系统
	if ((tmp=t.GetIntKeyValue("FUNCTION","QuestionStationPlanPassengerSta"))>0)
		m_bQuestionStationPlanPassengerSta=true;
	else
		m_bQuestionStationPlanPassengerSta=false;

	if ((tmp=t.GetIntKeyValue("FUNCTION","DisenableLogTDMesaage"))>0)
		m_bEnableLogTDMessage=false;
	else
		m_bEnableLogTDMessage=true;

	// 广州局要求 邻台计划中货车与客车相同方式进行处理
	if ((tmp=t.GetIntKeyValue("FUNCTION","EnableNeibPlanFreightSameToPassenger"))>0)
		m_bEnableNeibPlanFreightSameToPassenger=true;
	else 
		m_bEnableNeibPlanFreightSameToPassenger=false;

	// 珠三角城际计划支持配置
	if ((tmp=t.GetIntKeyValue("FUNCTION","SupportInterCityStPlan"))>0)
		{mbSupportInterCityStPlan=true;		m_bEnableCzh=true;}
	else 
		mbSupportInterCityStPlan=false;

	if ((tmp=t.GetIntKeyValue("FUNCTION","GpcNotSendReqOtherMsg"))==1)
		m_bGpcSendReqOtherMsg=false;

	if ((tmp=t.GetIntKeyValue("FUNCTION","SendLogToLogsvr"))==1)
		m_bSendLogToLogsvr=true;

	if ((tmp=t.GetIntKeyValue("FUNCTION","EnableBasicDeletePassengerFreight"))==1)
		m_bBasicDeletePassengerFreight = true;

	if ((tmp=t.GetIntKeyValue("FUNCTION","EnableGenAllMarkByDeleteExist"))==1)
		m_bGenAllMarkByDeleteExist = true;

	if ((tmp=t.GetIntKeyValue("FUNCTION","ConsiderTwicePassSameStation"))==1)
		m_bConsiderTwicePassSameStation = true;

	if ((tmp=t.GetIntKeyValue("FUNCTION","CompatibleCTC3StPlan"))==1)
		m_bCompatibleOldStPlan = false;
	
	if ((tmp=t.GetIntKeyValue("FUNCTION","EnableModifyGID"))==1)
		m_bEnableModifyGID = true;

	if ((tmp=t.GetIntKeyValue("FUNCTION","CheckShiftTrainAddTime"))==1)
		m_bCheckShiftTrainAddTime = true;

	if ((tmp=t.GetIntKeyValue("FUNCTION","NotSelAdjustPlan"))==1)
		m_NotSelAdjustPlan = true;
	
	if ((tmp=t.GetIntKeyValue("FUNCTION","NotUsingWorkGoodMode"))==1)
		m_bUsingWorkGoodMode = false;

	if ((tmp=t.GetIntKeyValue("FUNCTION","BasicShowCompareDlg"))==1)
		m_bBasicShowCompareDlg = true;

	if ((tmp=t.GetIntKeyValue("FUNCTION","ShiftShowCompareDlg"))==1)
		m_bShiftShowCompareDlg = true;

	if ((tmp=t.GetIntKeyValue("FUNCTION","EnableSelGenBasicTrain"))==1)
		m_bEnableSelGenBasicTrain = true;
	
	if ((tmp=t.GetIntKeyValue("FUNCTION","ShowColorNeibPlanTip"))==1)
		m_bShowColorNeibPlanTip = true;
	
	if ((tmp=t.GetIntKeyValue("FUNCTION","NeibPlanShowAcceptInfoOrder"))==1)
		m_bNeibPlanShowAcceptInfoOrder = true;

	if ((tmp=t.GetIntKeyValue("FUNCTION","QuerySchdNotClose"))==1)
		m_bQuerySchdClose = false;
	
	//// 上海局 精准邻台计划模式 开关
	if(t.GetKeyValue("FUNCTION","ExactNeibPlanModeMinStopTime",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		m_nExactNeibPlanModeCrhStopTime = atoi(pintstr);

		pintstr=strtok(NULL,", ");
		m_nExactNeibPlanModeOtherStopTime = atoi(pintstr);
	
		if (m_nExactNeibPlanModeCrhStopTime>0 || m_nExactNeibPlanModeOtherStopTime>0)
			m_bEnableExactNeibPlanMode=true;
	}
	
	//// 北京局 按照基本图勾连列车时
	if(t.GetKeyValue("FUNCTION","BasicCZHFindTrain",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		m_nBasicPreHour = atoi(pintstr);
		if(m_nBasicPreHour<0)
			m_nBasicPreHour=6;

		pintstr=strtok(NULL,", ");
		m_nBasicBackHour = atoi(pintstr);
		if(m_nBasicBackHour<0)
			m_nBasicBackHour=6;
	}

	tmp=t.GetIntKeyValue("FUNCTION","SetPreBasicWorkFlag");
	if (tmp==1)
		m_bSetPreBasicTechStop=true;
	
	tmp=t.GetIntKeyValue("FUNCTION","NotCompareTrainExpand");
	if (tmp==1)
		m_bNeedCompareTrainExpand=false;

	tmp=t.GetIntKeyValue("FUNCTION","NotNeedWaitMarkEnd");
	if (tmp==1)
		m_bNeedWaitMarkEnd=false;
	
	tmp=t.GetIntKeyValue("FUNCTION","EraseBeforeModifyTrain");
	if (tmp==1)
		m_bEraseBeforeModifyTrain=true;
	else
		m_bEraseBeforeModifyTrain=false;

	tmp=t.GetIntKeyValue("FUNCTION","NotMeetBySpeed");
	if (tmp==1)
		m_bNotMeetBySpeed=true;
	else
		m_bNotMeetBySpeed=false;

	tmp=t.GetIntKeyValue("FUNCTION","EnableModifyPlanTm");
	if (tmp==1)
		m_bDisableModifyPlanTm=false;
	else
		m_bDisableModifyPlanTm=true;

	tmp=t.GetIntKeyValue("FUNCTION","StplanTrainNoAutoChgChoice");
	if (tmp==1)
		m_bTrainNoAutoChgChoice=true;
	else
		m_bTrainNoAutoChgChoice=false;
	
	tmp=t.GetIntKeyValue("FUNCTION","CheckStationSideFull");
	if (tmp==1)
		m_bCheckSideFull=true;
	else
		m_bCheckSideFull=false;
	
	tmp=t.GetIntKeyValue("FUNCTION","EnableScadaElecArm");
	if (tmp==1)
		m_bEnableScadElecArm=true;
	else
		m_bEnableScadElecArm=false;
	
	tmp=t.GetIntKeyValue("FUNCTION","CheckTDMSShiftSide");
	if (tmp==0)
		m_bCheckTDMSSide=false;
	
	/////////// 到开<-->通过时不变更股道 ////////////////
	m_vecNoChangeSideWhileStopPass.clear();
	if(t.GetKeyValue("FUNCTION","NoChangeSideWhileStopPass",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_vecNoChangeSideWhileStopPass.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}

	/////////// 内燃机车画绿线台号 ////////////////
	m_vecConDieselLocomotiveDrawGreen.clear();
	if(t.GetKeyValue("FUNCTION","DieselLocomotiveDrawGreen",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_vecConDieselLocomotiveDrawGreen.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}

	////////////////////////邻台计划交互配置//////////////////////////////
	//金丽温客专台接收金温货线台
	//5=18, 22, 5550, 1/2, 2075, 2074
	if(t.GotoSection("NEIBPLAN6"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			NeibStationList x;
			char *px=strtok(content,"/, ");
			if(px)
				x.nThisSection=atoi(px); 
			else
				break;

			px=strtok(NULL,"/, ");
			if(px)
				x.nNeibSection=atoi(px);
			else
				break;

			px=strtok(NULL,"/, ");
			if(px)
				x.n_PreStation=atoi(px);
			else
				break;

			px=strtok(NULL,"/, ");
			if(px)
				x.station=atoi(px);
			else
				break;

			px=strtok(NULL,"/, ");
			if(px)
				x.n_NextStation=atoi(px);
			else
				break;

			px=strtok(NULL,"/, ");
			if(px)
				x.entry=atoi(px);
			else
				x.entry=0;

			px=strtok(NULL,"/, ");
			if(px)
				x.exit=atoi(px);
			else
				x.exit=0;

			m_neibRecvAllStation.push_back(x);
		}
	}
	GpcLogTrainMessage(0,5, "[NEIBPLAN6]");
	for(int i=0; i<m_neibRecvAllStation.size(); i++)
	{
		NeibStationList x=m_neibRecvAllStation[i];
		GpcLogTrainMessage(0,5, "%d=%d, %d, %d, %d, %d, %d/%d", i, x.nThisSection, x.nNeibSection,
			x.n_PreStation, x.station, x.n_NextStation, x.entry, x.exit);
	}
	//[NEIBPLAN5]
	//0=21, 1, 2075, 2/3/4, 5550/2101/0, 2073/2102/0, 2071/2103/0 
	if(t.GotoSection("NEIBPLAN5"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			NeibAdjustExtern x;
			x.neib_count=0;
			char *px=strtok(content,"/, ");
			if(px)
				x.nThisSection=atoi(px); 
			else
				break;

			px=strtok(NULL,"/, ");
			if(px)
				x.nNeibSection=atoi(px);
			else
				break;
			
			px=strtok(NULL,"/, ");
			if(px)
				x.nStation=atoi(px);
			else
				break;

			px=strtok(NULL,"/, ");
			if(px)
				x.exit_count=atoi(px);
			else
				break;

			for(int i=0; i<x.exit_count; i++)
			{
				px=strtok(NULL,"/, ");
				if(px)
					x.exit[i]=atoi(px);
				else
					break;
			}
			if(i<x.exit_count)
			{
				break;
			}

			px=strtok(NULL,"/, ");
			if(px)
				x.nNextStation=atoi(px);
			else
				break;
			px=strtok(NULL,"/, ");
			if(px)
				x.nPassway=atoi(px);
			else
				break;
			px=strtok(NULL,"/, ");
			if(px)
				x.nNodeIndex=atoi(px);
			else
				break;

			px=strtok(NULL,"/, ");
			while(px!=NULL)
			{
				x.neib_station[x.neib_count]=atoi(px);
				
				px=strtok(NULL,"/, ");
				if(px)
					x.passway_list[x.neib_count]=atoi(px);
				else
				{
					x.neib_count=0;
					break;
				}
				px=strtok(NULL,"/, ");
				if(px)
					x.index_list[x.neib_count]=atoi(px);
				else
				{
					x.neib_count=0;
					break;
				}

				x.neib_count++;
				if(x.neib_count>=sizeof(x.neib_station)/sizeof(x.neib_station[0]))
				{
					px=NULL;
				}
				else
				{
					px=strtok(NULL,"/, ");
				}
			}
			
			if(x.nThisSection>0 && x.nNeibSection>0 && x.nStation>0)
			{
				m_neibAdjustStation.push_back(x);
			}
		}
	}
	//0=21, 1, 2075*5550*2101*0, 2073*2102*0, 2071*2103*0 
	GpcLogTrainMessage(0,5, "[NEIBPLAN5]");
	for(int i=0; i<m_neibAdjustStation.size(); i++)
	{
		NeibAdjustExtern x=m_neibAdjustStation[i];
		CString str;
		str.Format(", %d",x.exit_count);
		for(int n=0;n<x.exit_count;n++)
		{
			CString tmp;
			tmp.Format("/%d", x.exit[n]);
			str+=tmp;
		}
		for(int n=0;n<x.neib_count;n++)
		{
			CString tmp;
			tmp.Format(", %d*%d*%d", x.neib_station[n], x.passway_list[n], x.index_list[n]);
			str+=tmp;
		}
		GpcLogTrainMessage(0,5, "%d=%d, %d, %d*%d*%d*%d  %s", 
			i,x.nThisSection, x.nNeibSection,
			x.nStation, x.nNextStation, x.nPassway, x.nNodeIndex, 
			str);
	}

	if(t.GotoSection("TD_SPLIT_EXTERN"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			SplitTrainExtern x;
			x.station_pair_count=0;
			char *px=strtok(content,", ");
			if(px)
				x.nThisSection=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.position_station=atoi(px);
			else
				continue;
		
			px=strtok(NULL,"*, ");
			while(px!=NULL)
			{
				x.station[x.station_pair_count]=atoi(px);
				px=strtok(NULL,"*, ");
				if(px)
				{
					x.station_exit[x.station_pair_count]=atoi(px);
				}
				else
				{
					x.station_pair_count=0;
					break;
				}
				px=strtok(NULL,"*, ");
				if(px)
				{
					x.next_station[x.station_pair_count]=atoi(px);
				}
				else
				{
					x.station_pair_count=0;
					break;
				}
				px=strtok(NULL,"*, ");
				if(px)
				{
					x.next_station_entry[x.station_pair_count]=atoi(px);
				}
				else
				{
					x.station_pair_count=0;
					break;
				}
				
				x.station_pair_count++;
				if(x.station_pair_count>=sizeof(x.station)/sizeof(x.station[0]))
				{
					px=NULL;
				}
				else
				{
					px=strtok(NULL,"*, ");
				}
			}
			
			if(x.nThisSection>0 && x.station_pair_count>=1 && x.position_station>0)
			{
				m_NewSplitList.push_back(x);
			}
		}
	}
	/////////// 对于特定车站下达阶段计划时，增加下一站邻站名称 ////////////////
	if(t.GotoSection("PLANTIP"))
	{
	  char key[10];
	  char content[128];
	  while(t.GetContent(key,content,128)) 
	  {
		  PlanNoteTip x;
		  char *px=strtok(content,", ");
		  if(px)
			  x.conid=atoi(px); 
		  else
			  continue;

		  px=strtok(NULL,", ");
		  if(px)
			  x.stano=atoi(px);
		  else
			  continue;

		  px=strtok(NULL,", ");
		  if(px)
			  x.nxtstano=atoi(px);
		  else
			  continue;

		  m_vctPlanTip.push_back(x);
	  }
	}
	
	/////////// 使用opms股道的货车类型配置 ////////////////
	m_vConfigGoodTrainTypesOfUseOPMSSide.clear();
	if(t.GetKeyValue("FUNCTION","UseOPMSSideGoodTrainTypeList",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_vConfigGoodTrainTypesOfUseOPMSSide.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}
	
	/////////// 不接收opms基本图股道的调度台 ////////////////
	m_vNotUseBasicSideSection.clear();
	if(t.GetKeyValue("FUNCTION","NotUseBasicSideSection",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_vNotUseBasicSideSection.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}

	/////////// 不接收opms日版计划股道的调度台 ////////////////
	m_vNotUseShiftSideSection.clear();
	if(t.GetKeyValue("FUNCTION","NotUseShiftSideSection",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_vNotUseShiftSideSection.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}
	
	/////////// 下发阶段计划检查台配置 ////////////////
	if(t.GetKeyValue("FUNCTION","NoCheckPlanValid",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_vecCheckPlanSection.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}
	
	/////////// 允许始发站技术停点的客车类型配置 ////////////////
	m_PessengerTrainStartTechType.clear();
	if(t.GetKeyValue("FUNCTION","PassengerTrainStartTechStop",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_PessengerTrainStartTechType.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}
	/////////// 允许终到站技术停点的客车类型配置 ////////////////
	m_PessengerTrainEndTechType.clear();
	if(t.GetKeyValue("FUNCTION","PassengerTrainEndTechStop",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_PessengerTrainEndTechType.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}
	/////////// 允许有非技术停点的货车类型配置 ////////////////
	m_GoodTrainType.clear();
	if(t.GetKeyValue("FUNCTION","GoodTrainHaveTechStop",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_GoodTrainType.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}
	m_UseOPMSTrainType.clear();
	if(t.GetKeyValue("FUNCTION","UseOPMSTimeTrainType",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_UseOPMSTrainType.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}

	///////////////////////配置外局区段号，用于邻台计划//////////////////////
	if(t.GetKeyValue("FUNCTION","OtherBureaSection",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_otherBureaSection.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}
	
	if(t.GetKeyValue("FUNCTION","NotCheckSection",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_NotCheckSection.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}
	
	/////////// CTC车站货车待避超过30分钟按照计划触发进路 ////////////////
	if(t.GetKeyValue("FUNCTION","ThirtyByPlanTrigSections",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_vecSectionThirtyByPlan.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}

	/////////// 路用列车类型配置 ////////////////
	if(t.GetKeyValue("FUNCTION","NotCheckTrains",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_aInsideUsedTrainTypes.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}

	////// 配置不受沈阳模式邻台配置文件(neibplan.xml)控制的邻台计划,比如哈大使用通用邻台处理
	if(t.GetKeyValue("FUNCTION","NormalNeibPlanSection",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_notUseSyNeibPlan.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}

	/////////// 自由增加列车类型配置 ////////////////
	if(t.GetKeyValue("FUNCTION","FreeAddTrains",tmpstr,sizeof(tmpstr)))
	{
		char *pintstr=strtok(tmpstr,", ");
		while(pintstr)
		{
			m_aFreeAddTrainTypes.push_back(atoi(pintstr));
			pintstr=strtok(NULL,", ");
		}
	}
	/// 读入TMIS区段配置
	for(int i=0;i<m_nSectionCount;i++)
	{
		char key[10];
		char content[128];
		sprintf(key,"%d",m_aSectionList[i].section_id);
		if(t.GetKeyValue("TD_SECTION",key,content,sizeof(content)))
		{
			char *ptmp=strtok(content,", ");
			if(ptmp)
				m_aSectionList[i].tmis_id=atoi(ptmp);
			ptmp=strtok(NULL,", ");
			if(ptmp)
				strncpy(m_aSectionList[i].disid,ptmp,sizeof(m_aSectionList[i].disid)-1);
			ptmp=strtok(NULL,", ");
			if(ptmp)
				strncpy(m_aSectionList[i].jhdisid,ptmp,sizeof(m_aSectionList[i].jhdisid)-1);
			ptmp=strtok(NULL,", ");
			if(ptmp)
				strncpy(m_aSectionList[i].kddisid,ptmp,sizeof(m_aSectionList[i].kddisid)-1);
		}
	}
	/// 分界口车站配置
	m_nFJKStationCount=0;
	if(t.GetKeyValue("TD_SECTION","FJK_STATION",tmpstr,sizeof(tmpstr)))
	{
		char *ptok=strtok(tmpstr,", ");
		while(ptok && m_nFJKStationCount<MAX_FJK_STAT_NUM)
		{
			m_aFJKStations[m_nFJKStationCount]=atoi(ptok);
			ptok=strtok(NULL,", ");
			m_nFJKStationCount++;
		}
	}
	/////////// 分割列车配置
	m_nSplitDefCount=0;
	for(int i=0;i<MAX_SPLIT_DEF_NUM;i++)
	{
		char key[10];
		char content[128];
		sprintf(key,"%d",i);
	    if(t.GetKeyValue("TD_SPLIT",key,content,sizeof(content)))
		{
			char *px=strtok(content,", \t");
			if(px)
				m_aSplitTrainDef[m_nSplitDefCount].section_id=atoi(px); 
			else
				m_aSplitTrainDef[m_nSplitDefCount].section_id=0;

			px=strtok(NULL,", \t");
			if(px)
				m_aSplitTrainDef[m_nSplitDefCount].station1=atoi(px); 
			else
				m_aSplitTrainDef[m_nSplitDefCount].station1=0;

			px=strtok(NULL,", \t");
			if(px)
				m_aSplitTrainDef[m_nSplitDefCount].station2=atoi(px); 
			else
				m_aSplitTrainDef[m_nSplitDefCount].station2=0;
			px=strtok(NULL,", \t");
			if(px)
				m_aSplitTrainDef[m_nSplitDefCount].station1next=atoi(px);
			else
				m_aSplitTrainDef[m_nSplitDefCount].station1next=-1;
			px=strtok(NULL,", \t");
			if(px)
				m_aSplitTrainDef[m_nSplitDefCount].station2pre=atoi(px);
			else
				m_aSplitTrainDef[m_nSplitDefCount].station2pre=-1;

			// 新增出入口配置,用于列车切割无法默认正确时使用
			px=strtok(NULL,", \t");
			if(px)
				m_aSplitTrainDef[m_nSplitDefCount].first_entry=atoi(px); // 前段列车入口
			else
				m_aSplitTrainDef[m_nSplitDefCount].first_entry=-1;
			px=strtok(NULL,", \t");
			if(px)
				m_aSplitTrainDef[m_nSplitDefCount].first_exit=atoi(px);  // 前段列车出口
			else
				m_aSplitTrainDef[m_nSplitDefCount].first_exit=-1;
			px=strtok(NULL,", \t");
			if(px)
				m_aSplitTrainDef[m_nSplitDefCount].second_entry=atoi(px);  // 后段列车入口
			else
				m_aSplitTrainDef[m_nSplitDefCount].second_entry=-1;
			px=strtok(NULL,", \t");
			if(px)
				m_aSplitTrainDef[m_nSplitDefCount].second_exit=atoi(px);   // 后段列车出口
			else
				m_aSplitTrainDef[m_nSplitDefCount].second_exit=-1;

			m_nSplitDefCount++;
		}
	}

	GpcLogTrainMessage(0,5, "[TD_SPLIT]");
	GpcLogTrainMessage(0,5, ";序号=区段号, 车站1, 车站2, 车站1下一站, 车站2上一站, 前段入口, 前段出口, 后段入口, 后段出口"); 
	for(int i=0;i<m_nSplitDefCount;i++)
	{
		char content[512];
		memset(content, 0, sizeof(content));
		sprintf(content, "%d=%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d", i, m_aSplitTrainDef[i].section_id, 
			 m_aSplitTrainDef[i].station1, m_aSplitTrainDef[i].station2, m_aSplitTrainDef[i].station1next, m_aSplitTrainDef[i].station2pre,
			 m_aSplitTrainDef[i].first_entry, m_aSplitTrainDef[i].first_exit, m_aSplitTrainDef[i].second_entry, m_aSplitTrainDef[i].second_exit);

		GpcLogTrainMessage(0,5, "%s", content);
	}

	if(t.GotoSection("STATION_NEXTSTATION_LINENO"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			Entry_Exit_Default defau;
			char *px=strtok(content,", ");
			if(px)
				defau.station=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				defau.nextstation=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				defau.lineno=atoi(px);
			else
				defau.lineno=-1;

			default_entry_vet.push_back(defau);
		}
	}

	///////////// 限制to_conid计算的台号 //////////////
	if(t.GotoSection("TO_CONID_LIMIT"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			ToLimitCal cal;

			char *px=strtok(content,", ");
			if(px)
				cal.conid=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				cal.station=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				cal.nextstation=atoi(px);
			else
				continue;

			if(cal.conid>0 && cal.station>0 && cal.nextstation>0)
				limit_section.push_back(cal);
		}
	}
	
	///////////// 不计算from_conid配置 //////////////
	if(t.GotoSection("FROM_CONID_LIMIT"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			FromNoCal cal;
			
			char *px=strtok(content,", ");
			if(px)
				cal.conid=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				cal.station=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				cal.entry=atoi(px);
			else
				continue;

			if(cal.conid>0 && cal.station>0 && cal.entry>0)
				notcalfromconid.push_back(cal);
		}
	}

	///////////// 本钱点统计 //////////////
	if(t.GotoSection("FJK_STATISTIC_LATETIME"))
	{
	  char key[10];
	  char content[128];
	  while(t.GetContent(key,content,128)) 
	  {
		  FJK_LATE_TIME x;
		  char *px=strtok(content,", ");
		  if(px)
			  x.station=atoi(px); 
		  
		  px=strtok(NULL,", ");
		  if(px)
			  x.method=atoi(px);
		  
  	      m_vecFjkStatisticLateTime.push_back(x);
	  }
	}

	///////////某站某条股道通过<-->到开股道不变/////////////
	if(t.GotoSection("STATION_SIDE_NOT_CHANGE"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			int con_id=-1;
			StationSideNotChange x;
			char *px=strtok(content,", ");
			if(px)
				con_id=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.station=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.flag=atoi(px);
			else
				continue;

			x.side_count=0;
			px=strtok(NULL,", ");
			while(px&&x.side_count<sizeof(x.side)/sizeof(x.side[0]))
			{
				x.side[x.side_count]=atoi(px);
				x.side_count++;
				px=strtok(NULL,", ");
			}
			
			if(x.flag!=1 && x.flag!=2 && x.flag!=3)
			{
				GpcLogSysMessage(0, "[STATION_SIDE_NOT_CHANGE] con_id %d station %d 客货车配置 flag=%d 不合法, 合法值为 1:客车 2:货车 3:客货车", 
						con_id, x.station, x.flag);
				continue;
			}

			if(x.side_count==0 || x.side_count>10)
			{
				GpcLogSysMessage(0, "[STATION_SIDE_NOT_CHANGE] con_id %d station %d flag=%d 股道没有配置或多于10个", 
						con_id, x.station, x.flag);
				continue;
			}

			std::map<int, SectionStationNotChange>::iterator it = side_not_change.find(con_id);
			if(it != side_not_change.end())
			{
				int count=it->second.station_count;
				it->second.info[count].flag=x.flag;
				it->second.info[count].station=x.station;
				it->second.info[count].side_count=x.side_count;
				it->second.station_count++;

				CString sidelist="",tmp;
				for(int a=0; a<x.side_count; a++)
				{
					it->second.info[count].side[a]=x.side[a];

					if(a==0)
						sidelist.Format("%d", x.side[a]);
					else
					{
						tmp.Format(",%d", x.side[a]);
						sidelist+=tmp;
					}
				}
				
				GpcLogSysMessage(0, "[STATION_SIDE_NOT_CHANGE] section %d station %d flag:%d sidelist:%s", 
						con_id, x.station, x.flag, sidelist);
			}
			else
			{
				SectionStationNotChange section;

				section.station_count=1;
				section.info[0].flag=x.flag;
				section.info[0].station=x.station;
				section.info[0].side_count=x.side_count;
				
				CString sidelist="",tmp;
				for(int a=0; a<x.side_count; a++)
				{
					section.info[0].side[a]=x.side[a];
					if(a==0)
						sidelist.Format("%d", x.side[a]);
					else
					{
						tmp.Format(",%d", x.side[a]);
						sidelist+=tmp;
					}
				}

				GpcLogSysMessage(0, "[STATION_SIDE_NOT_CHANGE] section %d station %d flag:%d sidelist:%s", 
						con_id, x.station, x.flag, sidelist);

				side_not_change.insert(make_pair(con_id, section));
			}
		}
	}
	
	//////////设置列车在某站某出入口电力内燃属性////////////
	if(t.GotoSection("DEFAULT_ELEC_SET"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			int con_id=-1;
			StationDefaultElec x;
			char *px=strtok(content,", ");
			if(px)
				con_id=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.flag=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.station=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.entry=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.exit=atoi(px);
			else
				continue;

			if((x.flag!=0 && x.flag!=1) || (x.entry==0 && x.exit==0) || x.station<=0)
				continue;

			std::map<int, SectionDefaultElec>::iterator it = default_elec.find(con_id);
			if(it != default_elec.end())
			{
				for(int a=0; a<it->second.station_count; a++)
				{
					if(it->second.default_elec[a].station==x.station &&
						it->second.default_elec[a].exit==x.exit &&
						it->second.default_elec[a].entry==x.entry&&
						it->second.default_elec[a].flag==x.flag)
						break;
				}

				if(a>=it->second.station_count)
				{
					it->second.default_elec[a].station=x.station;
					it->second.default_elec[a].exit=x.exit;
					it->second.default_elec[a].entry=x.entry;
					it->second.default_elec[a].flag=x.flag;
					it->second.station_count++;

					GpcLogSysMessage(0, "[DEFAULT_ELEC_SET] section %d set %d station:%d entry:%d exit:%d", 
						con_id, x.flag, x.station, x.entry, x.exit);
				}
			}
			else
			{
				SectionDefaultElec sec;
				sec.con_id = con_id;
				sec.default_elec[0].station=x.station;
				sec.default_elec[0].exit=x.exit;
				sec.default_elec[0].entry=x.entry;
				sec.default_elec[0].flag=x.flag;
				sec.station_count=1;

				GpcLogSysMessage(0, "[DEFAULT_ELEC_SET] section %d set %d station:%d entry:%d exit:%d", 
						con_id, x.flag, x.station, x.entry, x.exit);

				default_elec.insert(make_pair(con_id, sec));
			}
		}
	}
	m_nSectionStaEntryCount=0;
	if(t.GotoSection("SET_STATION_ENTRY"))  // 济南局,晏城--晏城北出入口设置配置
	{
		char key[10];
		char content[256];
		while(t.GetContent(key,content,255)) 
		{
			StaEntry xx;
			char *px=strtok(content,"/, ");
			if(px)
				xx.section=atoi(px); 
			else
				continue;

			px=strtok(NULL,"/, ");
			if(px)
				xx.flag=atoi(px);
			else
				continue;

			int count=0;
			px=strtok(NULL,"/, ");
			if(px)
				count=atoi(px);
			else
				continue;

			for(int i=0; i<count; i++)
			{
				px=strtok(NULL,"/, ");
				if(px)
					xx.station[i]=atoi(px);
				else
					break;

				px=strtok(NULL,"/, ");
				if(px)
					xx.entry[i]=atoi(px);
				else
					break;

				px=strtok(NULL,"/, ");
				if(px)
					xx.exit[i]=atoi(px);
				else
					break;
			}
			if(i>=count)
			{
				xx.count=count;
				for(int k=0; k<64; k++)
				{
					if(m_specialEntry[k].section==0)
					{
						m_nSectionStaEntryCount++;
						m_specialEntry[k].section=xx.section;
						m_specialEntry[k].count=1;
						m_specialEntry[k].entry[0]=xx;
						break;
					}
					if(m_specialEntry[k].section==xx.section)
					{
						int n=m_specialEntry[k].count;
						if(n<16)
						{
							m_specialEntry[k].entry[n]=xx;
							m_specialEntry[k].count++;							
						}
						break;
					}
				}
			}
		}
	}
	GpcLogTrainMessage(0,5, "[SET_STATION_ENTRY]");
	for(int i=0; i<m_nSectionStaEntryCount&&i<64; i++)
	{
		for(int n=0; n<m_specialEntry[i].count; n++)
		{
			StaEntry xx=m_specialEntry[i].entry[n];
			CString str, tmp;
			str.Format("%d=%d,%d,%d", n, xx.section, xx.flag, xx.count);
			for(int k=0; k<xx.count; k++)
			{
				tmp.Format(",%d/%d/%d", xx.station[k], xx.entry[k], xx.exit[k]);
				str+=tmp;
			}
			GpcLogTrainMessage(0,5, "%s", str);
		}
	}
	/////////// 邻台计划获取配置 ////////////////
	GpcLogTrainMessage(0,5, "\n\n================邻台配置 开始===================");
	GpcLogTrainMessage(0,5, "[NEIBPLAN]");
	GpcLogTrainMessage(0,5, ";no=neib_section, this_section, first_station, prev_station, next_station, node_index, passway, nWholeMatch, accept_ternimal_train, next_next_station, neib_station, control_prev_abs, consider_start_train"); 
				
	if(t.GotoSection("NEIBPLAN"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			NeibPlanStruct x;
			char *px=strtok(content,", ");
			if(px)
				x.source_section=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.dest_section=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.first_station=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.prev_station=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.next_station=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.node_index=atoi(px);
			else
				x.node_index=255;

			px=strtok(NULL,", ");
			if(px)
				x.passway=atoi(px);
			else
				x.passway=0;

			px=strtok(NULL,", ");
			if(px)
				x.nWholeMatch=atoi(px);
			else
				x.nWholeMatch=1;

			px=strtok(NULL,", ");
			if(px)
				x.accept_ternimal_train=atoi(px);
			else
				x.accept_ternimal_train=0;

			px=strtok(NULL,", ");
			if(px)
				x.next_next_station=atoi(px);
			else
				x.next_next_station=0;
			
			px=strtok(NULL,", ");
			if(px)
				x.neib_station=atoi(px);
			else
				x.neib_station=0;

			px=strtok(NULL,", ");
			if(px)
				x.control_prev_abs=atoi(px);
			else
				x.control_prev_abs=0;
			
			px=strtok(NULL,", ");
			if(px)
			{
				x.consider_start_train=atoi(px);
				if(x.consider_start_train!=0)
					x.consider_start_train=1;
			}
			else
			{
				if(x.first_station==x.neib_station) // neib_station 必定为正，即本台始发
					x.consider_start_train = 0; // 0:不去邻台查找
				else
					x.consider_start_train = 1; // 1:去邻台查找
			}
			m_aNeibPlanConfig.push_back(x);
			GpcLogTrainMessage(0,5, "%d=%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d", 
					m_aNeibPlanConfig.size()-1, x.source_section, x.dest_section, x.first_station, x.prev_station, x.next_station, x.node_index, x.passway,x.nWholeMatch, x.accept_ternimal_train, 
					x.next_next_station, x.neib_station, x.control_prev_abs, x.consider_start_train);
		}
	}

	/////////// 邻台管辖车站 ////////////////
	if(t.GotoSection("NEIB_SECTION_STATION"))
	{
		char key[10];
		char content[256];
		while(t.GetContent(key,content,256)) 
		{
			int this_section=0;
			int station=0, nIsThisSection=-1;
			
			char *px=strtok(content,", ");
			if(px)
				this_section=atoi(px);
		
			px=strtok(NULL,", ");
			if(px)
				station=atoi(px);

			px=strtok(NULL,", ");
			if(px)
				nIsThisSection=atoi(px);
			
			if(station>0&&this_section>0&&nIsThisSection>=0)
			{
				std::map<int, NeibSectionStation>::iterator it = m_neibSectionStation.find(this_section);
				if(it == m_neibSectionStation.end())
				{
					NeibSectionStation info;
					info.station_count=1;
					info.station_no[0]=station;
					info.this_section[0]=nIsThisSection;
					m_neibSectionStation[this_section]=info;
				}
				else
				{
					int count=it->second.station_count, k=0;
					for(k=0; k<count; k++)
					{
						if(it->second.station_no[k]==station)
							break;
					}
					if(k>=count)
					{
						if(count<16)
						{
							it->second.station_no[count]=station;
							it->second.this_section[count]=nIsThisSection;
							it->second.station_count++;
						}
						else
						{
							GpcLogSysMessage(0, "非本台管辖车站配置太多,一个台最多支持16个. 丢弃[NOT_THIS_SECTION_STATION] %d= %d ", this_section, station);
						}
					}
				}
			}
			else
			{
				GpcLogSysMessage(0, "非本台管辖车站配置非法 %d= %d  %d", this_section, station,nIsThisSection);
			}
		}
	}
	GpcLogTrainMessage(0,5, "[NEIB_SECTION_STATION]=台号, 车站, 管辖(0:非本台管辖 1:本台管辖");

	std::map<int, NeibSectionStation>::iterator it = m_neibSectionStation.begin();
	while(it!=m_neibSectionStation.end())
	{
		for(int k=0; k<it->second.station_count; k++)
		{
			GpcLogTrainMessage(0,5, "%d=%d, %d, %d", k, it->first, it->second.station_no[k], it->second.this_section[k]);
		}
		it++;
	}

	/////////// 基于出口的邻台计划获取配置 ////////////////
	GpcLogTrainMessage(0,5, "[NEIBPLAN2]");
	if(t.GotoSection("NEIBPLAN2"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			NeibPlanEntryStruct x;
			char *px=strtok(content,", ");
			if(px)
				x.dest_section=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.source_section=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.fjk_station=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.exit=atoi(px);
			else
				continue;
			px=strtok(NULL,", ");
			if(px)
				x.passway=atoi(px);
			else
				x.passway=0;
			px=strtok(NULL,", ");
			if(px)
				x.node_index=atoi(px);
			else
				x.node_index=255;

			m_aNeibPlanEntryConfig.push_back(x);
			GpcLogTrainMessage(0,5, "%d=this_section:%d neib_section:%d fjk_station:%d exit:%d passway:%d node_index:%d bqd_entry:%d ", 
				m_aNeibPlanEntryConfig.size(), x.dest_section, x.source_section, x.fjk_station, x.exit, x.passway, x.node_index, x.bqd_entry);
		}
	}

	/////////// 基于出口\入口的邻台计划获取配置 ////////////////
	GpcLogTrainMessage(0,5, "[NEIBPLAN4]");
	if(t.GotoSection("NEIBPLAN4"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			NeibPlanEntryStruct x;
			char *px=strtok(content,", ");
			if(px)
				x.dest_section=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.source_section=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.fjk_station=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.exit=atoi(px);
			else
				continue;
			px=strtok(NULL,", ");
			if(px)
				x.passway=atoi(px);
			else
				x.passway=0;
			px=strtok(NULL,", ");
			if(px)
				x.node_index=atoi(px);
			else
				x.node_index=255;

			px=strtok(NULL,", ");
			if(px)
				x.bqd_entry=atoi(px);
			else
				x.bqd_entry=-1;

			px=strtok(NULL,", ");
			if(px)
				x.entry=atoi(px);
			else
				x.entry=-1;

			m_aNeibPlanEntryConfig.push_back(x);
			GpcLogTrainMessage(0,5, "%d=this_section:%d neib_section:%d fjk_station:%d exit:%d passway:%d node_index:%d bqd_entry:%d entry:%d", 
				m_aNeibPlanEntryConfig.size(), x.dest_section, x.source_section, x.fjk_station, x.exit, x.passway, x.node_index, x.bqd_entry, x.entry);
		}
	}

	m_aNeibPlanMode.clear();
	for(int i=0; i<m_aNeibPlanConfig.size(); i++)
	{
		NeibPlanMode x;
		x.this_section=m_aNeibPlanConfig[i].dest_section;
		x.neib_section=m_aNeibPlanConfig[i].source_section;
		x.mode=2;
		
		for(int k=0; k<m_aNeibPlanMode.size(); k++)
		{
			if(m_aNeibPlanMode[k].this_section==x.this_section && m_aNeibPlanMode[k].neib_section==x.neib_section)
			{
				break;
			}
		}
		if(k>=m_aNeibPlanMode.size())
			m_aNeibPlanMode.push_back(x);
	}

	for(int i=0; i<m_aNeibPlanEntryConfig.size(); i++)
	{
		NeibPlanMode x;
		x.this_section=m_aNeibPlanEntryConfig[i].dest_section;
		x.neib_section=m_aNeibPlanEntryConfig[i].source_section;
		x.mode=1;
		
		for(int k=0; k<m_aNeibPlanMode.size(); k++)
		{
			if(m_aNeibPlanMode[k].this_section==x.this_section && m_aNeibPlanMode[k].neib_section==x.neib_section)
			{
				if(m_aNeibPlanMode[k].mode==2)
				{
					CString str;
					str.Format("请注意: 邻台计划配置错误, %d 收 %d 邻台只能按照一种邻台配置 要么出入口 要么前后站关系,检查tg_logic.cfg",
							x.this_section, x.neib_section);
					AfxMessageBox(str);
					return FALSE;
				}
				m_aNeibPlanMode[k].mode=1;
				break;
			}
		}
		if(k>=m_aNeibPlanMode.size())
			m_aNeibPlanMode.push_back(x);
	}

	if(t.GotoSection("NEIBPLAN_MODE"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			NeibPlanMode x;
			char *px=strtok(content,", ");
			if(px)
				x.this_section=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.neib_section=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				x.mode=atoi(px);

			for(int i=0; i<m_aNeibPlanMode.size(); i++)
			{
				if(m_aNeibPlanMode[i].this_section==x.this_section && m_aNeibPlanMode[i].neib_section==x.neib_section)
				{
					m_aNeibPlanMode[i].mode=x.mode;
				}
			}

			if(i>=m_aNeibPlanMode.size())
				m_aNeibPlanMode.push_back(x);
		}
	}

	GpcLogTrainMessage(0,5, "[NEIBPLAN_MODE]=本台,邻台,邻台模式(1:出入口 2:前后站)");
	for(int i=0; i<m_aNeibPlanMode.size(); i++)
	{
		GpcLogTrainMessage(0,5, "%d=%d, %d, %d",i, m_aNeibPlanMode[i].this_section, m_aNeibPlanMode[i].neib_section, m_aNeibPlanMode[i].mode);
	}
	GpcLogTrainMessage(0,5, "\n================邻台配置 结束===================\n");
	/////////// 自动提示配置
	m_nAutoNotifyDefCount=0;
	for(int i=0;i<MAX_NOTIY_COUNT;i++)
	{
		char key[10];
		char content[128];
		sprintf(key,"%d",i);
	    if(t.GetKeyValue("NOTIFY",key,content,sizeof(content)))
		{
			int day_time;
			char notify_str[MAX_NOTIFY_STRLEN];
			if(sscanf(content,"%d %s",&day_time,notify_str)==2)
			{
				m_aAutoNotifyDef[m_nAutoNotifyDefCount].day_time=day_time;
				strncpy(m_aAutoNotifyDef[m_nAutoNotifyDefCount].notify_str,notify_str,sizeof(m_aAutoNotifyDef[m_nAutoNotifyDefCount].notify_str)-1);
				m_nAutoNotifyDefCount++;
			}
		}
	}

	/////////// 默认出入口配置 ////////////////
	if(t.GotoSection("DEFAULT_ENTRY"))
	{
	  char key[10];
	  char content[128];
	  while(t.GetContent(key,content,128)) 
	  {
		  StationDefaultEntry x;
		  x.station=atoi(key);
		  do
		  {
		  char *px=strtok(content,", ");
		  if(px)
			  x.in_down_entry=atoi(px); 
		  else
			 break;
		  px=strtok(NULL,", ");
		  if(px)
			  x.in_up_entry=atoi(px);
		  else
			 break;
		  px=strtok(NULL,", ");
		  if(px)
			  x.out_down_entry=atoi(px);
		  else
			 break;
		  px=strtok(NULL,", ");
		  if(px)
			  x.out_up_entry=atoi(px);
		  else
			 break;
		  }while(0);
		  m_aStationDefaultEntryConfig.push_back(x);
	  }
	}

	/////////// 台间车站阶段计划下达配置 ///////
	if(t.GotoSection("NEIBSTATION"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			int nSection = 0;
			NeibStation neib;
			char *px=strtok(content,", ");
			if(px)
				nSection=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				neib.nNeibStaNo=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				neib.nStationNo=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				neib.nNextNextStationNo=atoi(px); 
			else
				neib.nNextNextStationNo=0;

			px=strtok(NULL,", ");
			if(px)
				neib.nPreStation=atoi(px); 
			else
				neib.nPreStation=0;
			

			std::map<int, NeibStationPlan>::iterator it = m_neigStationPlan.find(nSection);
			if(it != m_neigStationPlan.end())
			{
				it->second.vecNeibStation.push_back(neib);
			}
			else
			{
				NeibStationPlan plan;
				plan.nSectionNo = nSection;
				plan.vecNeibStation.push_back(neib);
				m_neigStationPlan.insert(make_pair(plan.nSectionNo, plan));
			}
		}
	}
	
	/////////// TDMD调度命令类型转换 ////////////////
	m_vecDCMDType.clear();
	if(t.GotoSection("TDMS_DCMD_TYPE"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			DcmdTypeCnt dcmdType;
			long tmis_type = 0, dcmd_templete_no=0;
			char *px=strtok(content,", ");
			if(px)
				dcmdType.cascoType=atol(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
			{	
				strncpy(dcmdType.tmisType, px, sizeof(dcmdType.tmisType)-1);
				dcmdType.tmisType[sizeof(dcmdType.tmisType)-1] = 0;
			}
			else
			{	
				memset(dcmdType.tmisType, 0, sizeof(dcmdType.tmisType));
			}

			m_vecDCMDType.push_back(dcmdType);
		}
	}

	/////////// 使用TDMS计划线别号的车站配置 ////////////////
	m_UserTDMSLineStation.clear();
	if(t.GotoSection("USE_TDMS_LINE_STATION"))
	{
		char key[10];
		char content[128];
		while(t.GetContent(key,content,128)) 
		{
			USE_TDMS_LINE_STATION tdms_station;
			
			char *px=strtok(content,", ");
			if(px)
				tdms_station.con_id=atoi(px); 
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				tdms_station.station_id1=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				tdms_station.station_id2=atoi(px);
			else
				continue;

			px=strtok(NULL,", ");
			if(px)
				tdms_station.station_id1_nd=atoi(px);
			else
				tdms_station.station_id1_nd=255;

			px=strtok(NULL,", ");
			if(px)
				tdms_station.station_id2_nd=atoi(px);
			else
				tdms_station.station_id2_nd=255;

			m_UserTDMSLineStation.push_back(tdms_station);
		}
	}
	

	t.CloseFile();
	
	LoadStplanWarnCfg(strPath + "\\data\\new_stplan_warning_class.xml");
	LoadNeibPlanSYXML(strPath + "\\data\\neibplan.xml");
	LoadNeibPlanReverseXML(strPath + "\\data\\neibplan_update.xml");
	LoadChangeTrainidXML(strPath + "\\data\\changetrainid.xml");
	LoadTrainDepartCheckXML(strPath + "\\data\\traindepartcheck.xml");
	
	for(int i=0; i<m_notUseSyNeibPlan.size(); i++)// 配置不受沈阳模式邻台配置文件(neibplan.xml)控制的邻台计划,比如哈大使用通用邻台处理
	{
	  GpcLogSysMessage(0, "不受沈阳模式邻台配置文件(neibplan.xml)控制的邻台计划的调度区段:%d", m_notUseSyNeibPlan[i]);
	}
	

	m_bHasNoLimitStation = LoadNoLimitStitonXml(strPath + "\\data\\no_limit_station.xml");
	return TRUE;
}

// 按照列车类型配置到开--通过变更时不变更股道的列车类型
bool CTGFunctionConfig::IsChangeSideWhileStopPass(int train_type) 
{
	// 仅仅配置-1, 表示所有车到开--通过均不变更股道
	if(m_vecNoChangeSideWhileStopPass.size() == 1)
	{
		if(m_vecNoChangeSideWhileStopPass[0]==-1)
		{
			return false;
		}
	}
	for(int i=0; i<m_vecNoChangeSideWhileStopPass.size();i++)
	{
		if(m_vecNoChangeSideWhileStopPass[i]==train_type)
		{
			return false;
		}
	}
	return true;
}

CGpcdef *CTGFunctionConfig::GetGPCDEFx()  
{
	return &gpcdef;
}
BOOL CTGFunctionConfig::MatchSplitTrain(int con_id,int station1,int station2, int *station1next, int *station2pre, int* first_entry, int* first_exit, int* second_entry, int* second_exit)
{
	if(station1next==NULL || station2pre==NULL)
		return FALSE;

	for(int i=0;i<m_nSplitDefCount;i++)
	{
		if(m_aSplitTrainDef[i].section_id==con_id
			&& m_aSplitTrainDef[i].station1==station1
			&& m_aSplitTrainDef[i].station2==station2)
		{
			*station1next=m_aSplitTrainDef[i].station1next;
			*station2pre=m_aSplitTrainDef[i].station2pre;
			*first_entry=m_aSplitTrainDef[i].first_entry;
			*first_exit=m_aSplitTrainDef[i].first_exit;
			*second_entry=m_aSplitTrainDef[i].second_entry;
			*second_exit=m_aSplitTrainDef[i].second_exit;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL  CTGFunctionConfig::GetNotifyDesc(int index,int *daytime,char *desc,int desclen)
{
	if(index<0 || index>=m_nAutoNotifyDefCount)
		return FALSE;
	*daytime=m_aAutoNotifyDef[index].day_time;
	strncpy(desc,m_aAutoNotifyDef[index].notify_str,desclen-1);
	return TRUE;
}

int  CTGFunctionConfig::GetNeibStations(int nSection, int station, std::vector<NeibStation>& vecNeibStaPlan)
{
	vecNeibStaPlan.clear();

	std::map<int, NeibStationPlan>::iterator it = m_neigStationPlan.find(nSection);
	if(it != m_neigStationPlan.end())
	{
		for(int i=0; i<it->second.vecNeibStation.size(); i++)
		{
			if(station == it->second.vecNeibStation[i].nNeibStaNo)
			{
				vecNeibStaPlan.push_back(it->second.vecNeibStation[i]);
			}
		}
	}
	
	return 0;
}

BOOL  CTGFunctionConfig::IsFJKStation(int station) const
{
	for(int i=0;i<m_nFJKStationCount;i++)
	{
		if(m_aFJKStations[i]==station)
			return TRUE;
	}
	return FALSE;
}

// 返回值是匹配车站中第一个车站的索引,该索引可能是台间站,也可能是台间站的前一站
int CTGFunctionConfig::MatchNeibPlan(int source_section,int dest_section,int stationlist[],int stationcount,int& matched_rec_count, bool& bHavePreStation,int& nodeindex,int& passway,bool& bWholeMatch, bool& bAcceptTerminalTrain, int &neib_station, int &control_prev_abs)
{
	vector<NeibPlanStruct> neibPlanConfig;
	GetNeibPlanConfig(source_section, dest_section, neibPlanConfig);

	for(int i=0;i<neibPlanConfig.size();i++)
	{
		NeibPlanStruct x=neibPlanConfig[i];
		if(x.source_section!=source_section  || x.dest_section!=dest_section)
			continue;

        int  m=0,n=0;
		int  tmp_station[4];
		int  tmp_count=0;
		tmp_station[0]=x.prev_station;
		tmp_station[1]=x.first_station;
		tmp_station[2]=x.next_station;
		tmp_station[3]=x.next_next_station;
		if(x.next_station==0)
			tmp_count=2;
		else
		{
			if(x.next_next_station==0)
				tmp_count=3;
			else
				tmp_count=4;
		}
		bHavePreStation=true;
		if(x.prev_station==0)
		{
			bHavePreStation=false;
			tmp_count--;
			tmp_station[0]=tmp_station[1];
			tmp_station[1]=tmp_station[2];
			tmp_station[2]=tmp_station[3];
			tmp_station[3]=0;
		}
				
		while(m<tmp_count && n<stationcount)
		{
			if(tmp_station[m]==stationlist[n])
			{
				m++;
			}
			else
			{
				if(m>0)
				{
				   m=0;
				   continue;
				}
			}
			n++;
		}

		if(m==tmp_count)
		{
			matched_rec_count=m;
			passway=x.passway;
			nodeindex=x.node_index;
			bWholeMatch=(x.nWholeMatch>0)?true:false;
			bAcceptTerminalTrain=(x.accept_ternimal_train>0)?true:false;
			neib_station=x.neib_station;
			control_prev_abs=x.control_prev_abs;
			return n-m;
		}
	}
	return -1;
}

int  CTGFunctionConfig::GetNeibFJKStations(int dest_section,int source_section,int stationlist[],int limit)
{
	int cnt=0;
	if(IsSyNeighbourPlan(dest_section))
	{
		const NeibConsolePlanSY* pNeibConsolePlan=GetNeibConsolePlan(dest_section, source_section);
		if(pNeibConsolePlan)
		{
			for(int c=0; c<pNeibConsolePlan->neibplan_count; c++)
			{
				int station=pNeibConsolePlan->neib_plan[c].first_station;
				for(int i=0; i<cnt; i++)
				{
					if(station==stationlist[i])
					{
						break;
					}
				}

				if(i>=cnt)
				{
					stationlist[cnt++]=station;
				}

				station=pNeibConsolePlan->neib_plan[c].fjk_station;
				for(int i=0; i<cnt; i++)
				{
					if(station==stationlist[i])
					{
						break;
					}
				}

				if(i>=cnt)
				{
					stationlist[cnt++]=station;
				}
			}
		}

		const NeibConsolePlanReverse* pNeibConsolePlanReverse=GetNeibPlan_Reverse(dest_section, source_section);
		if(pNeibConsolePlanReverse)
		{
			for(int c=0; c<pNeibConsolePlanReverse->neibplan_num; c++)
			{
				int station=pNeibConsolePlanReverse->neib_plan[c].station;
				for(int i=0; i<cnt; i++)
				{
					if(station==stationlist[i])
					{
						break;
					}
				}

				if(i>=cnt)
				{
					stationlist[cnt++]=station;
				}
			}
		}
	}
	else
	{
		int nMode=GetNeibPlanMode(dest_section, source_section);
		if(nMode==2)
		{
			for(int i=0;i<m_aNeibPlanConfig.size();i++)
			{
				NeibPlanStruct x=m_aNeibPlanConfig[i];
				if(x.dest_section==dest_section && x.source_section==source_section)
				{
					int station=x.first_station;
					if(x.neib_station!=0)
					{
						station=abs(x.neib_station);
					}
					for(int c=0; c<cnt; c++)
					{
						if(station==stationlist[c])
						{
							break;
						}
					}

					if(c>=cnt)
					{
						stationlist[cnt++]=station;
					}
				}
			}
		}
		else if(nMode==1)
		{
			for(int i=0;i<m_aNeibPlanEntryConfig.size();i++)
			{
				NeibPlanEntryStruct x=m_aNeibPlanEntryConfig[i];
				if(x.dest_section==dest_section && x.source_section==source_section)
				{
					int station=x.fjk_station;
					for(int c=0; c<cnt; c++)
					{
						if(station==stationlist[c])
						{
							break;
						}
					}

					if(c>=cnt)
					{
						stationlist[cnt++]=station;
					}
				}
			}
		}
		else if(nMode==3)
		{
			for(int i=0; i<m_neibAdjustStation.size(); i++) 
			{
				if((m_neibAdjustStation[i].nThisSection==dest_section) && (m_neibAdjustStation[i].nNeibSection==source_section))
				{
					stationlist[cnt++]=m_neibAdjustStation[i].nStation;
				}
			}
		}
		else if(nMode==4)
		{
			for(int i=0; i<m_neibRecvAllStation.size(); i++) 
			{
				if((m_neibRecvAllStation[i].nThisSection==dest_section) && (m_neibRecvAllStation[i].nNeibSection==source_section))
				{
					int station=m_neibRecvAllStation[i].station;
					for(int c=0; c<cnt; c++)
					{
						if(station==stationlist[c])
						{
							break;
						}
					}

					if(c>=cnt && cnt<limit)
					{
						stationlist[cnt++]=station;
					}
				}
			}
		}
	}
	return cnt;
}

/* exit > 0 出口匹配后需要，  <0 入口*-1出口匹配后过滤 */
BOOL  CTGFunctionConfig::MatchNeibPlanByEntry(int source_section,int dest_section,int station,int exit,int *nodeinex,int *passway,int entry) // 返回值成功失败
{
	if(!exit) return FALSE;

	for(int i=0;i<m_aNeibPlanEntryConfig.size();i++)
	{
		NeibPlanEntryStruct x=m_aNeibPlanEntryConfig[i];

		if (!(x.dest_section==dest_section && x.source_section==source_section && x.fjk_station==station)) 
			continue;

		if (x.entry>0) // 单纯通过入口比较
		{
			if (x.entry == entry)
			{
				if(passway)
					*passway=x.passway;
				if(nodeinex)
					*nodeinex=x.node_index;
				return TRUE;
			}
		}
		else
		{
			if(x.exit == exit && x.exit!=0)
			{
				if(passway)
					*passway=x.passway;
				if(nodeinex)
					*nodeinex=x.node_index;
				return TRUE;
			}
		}
	}

	return FALSE;
}

bool CTGFunctionConfig::IsSyNeighbourPlan(int conid)
{
	if(console_neibplan.size()>0)
	{
		for(int i=0; i<m_notUseSyNeibPlan.size(); i++)
		{
			if(m_notUseSyNeibPlan[i]==conid)
				return false;
		}
		return true;
	}
	return false;
}


BOOL  CTGFunctionConfig::GetDefaultEntry(int station,StationDefaultEntry &default_entry) const
{
	for(int i=0;i<m_aStationDefaultEntryConfig.size();i++)
	{
		if(m_aStationDefaultEntryConfig[i].station==station)
		{
			default_entry=m_aStationDefaultEntryConfig[i];
			return TRUE;
		}
	}
	return FALSE;
}

int   CTGFunctionConfig::GetDefaultEntry(int station, BOOL down_direction,BOOL accept_in) const
{
	StationDefaultEntry  entry_config;
	if(!GetDefaultEntry(station,entry_config))
		return 0;
	if(down_direction && accept_in)
		return entry_config.in_down_entry;
	if(down_direction && !accept_in)
		return entry_config.out_down_entry;
	if(!down_direction && accept_in)
		return entry_config.in_up_entry;
	if(!down_direction && !accept_in)
		return entry_config.out_up_entry;
	return 0;

}

BOOL  CTGFunctionConfig::GoodsTrainHaveTechStop(int train_type) const
{
	for(int i=0;i<m_GoodTrainType.size();i++)
	{
		if(m_GoodTrainType[i]==train_type)
			return TRUE;
	}
	return FALSE;
}

BOOL  CTGFunctionConfig::PassengerTrainStartTechStop(int train_type) const
{
	for(int i=0;i<m_PessengerTrainStartTechType.size();i++)
	{
		if(m_PessengerTrainStartTechType[i]==train_type)
			return TRUE;
	}
	return FALSE;
}

BOOL  CTGFunctionConfig::PassengerTrainEndTechStop(int train_type) const
{
	for(int i=0;i<m_PessengerTrainEndTechType.size();i++)
	{
		if(m_PessengerTrainEndTechType[i]==train_type)
			return TRUE;
	}
	return FALSE;
}

BOOL  CTGFunctionConfig::NeedToCheckTGOper(int train_type) const
{
	for(int i=0;i<m_aInsideUsedTrainTypes.size();i++)
	{
		if(m_aInsideUsedTrainTypes[i]==train_type)
			return FALSE;
	}
	return TRUE;
}
BOOL  CTGFunctionConfig::IsFreeAddTrain(int train_type) const
{
	for(int i=0;i<m_aFreeAddTrainTypes.size();i++)
	{
		if(m_aFreeAddTrainTypes[i]==train_type)
			return TRUE;
	}
	return FALSE;
}

long  CTGFunctionConfig::GetDcmdTempleteNo(CString type, CString subType)
{
	for(int i=0; i<m_vecDCMDType.size(); i++)
	{
		if(type == m_vecDCMDType[i].tmisType)
			return m_vecDCMDType[i].cascoType;
	}

	return m_dcmd_prefix;
}

BOOL  CTGFunctionConfig::IsSendPlanTip(int con_id, int sta, int nxtsta) const
{
	for(int i=0; i<m_vctPlanTip.size(); i++)
	{
		if(m_vctPlanTip[i].conid == con_id 
		  && m_vctPlanTip[i].stano == sta 
		  && m_vctPlanTip[i].nxtstano == nxtsta)
		{
			return TRUE;
		}
	}

	return FALSE;
}

CONID  CTGFunctionConfig::GetTrainRunFromConid(int src_conid, int fjk_staid, int exit, int entry)
{
	for(int i=0; i<m_aNeibPlanEntryConfig.size(); i++)
	{
		if(m_aNeibPlanEntryConfig[i].dest_section == src_conid && 
			m_aNeibPlanEntryConfig[i].fjk_station == fjk_staid &&
			m_aNeibPlanEntryConfig[i].exit == exit &&
			((m_aNeibPlanEntryConfig[i].bqd_entry<0) || 
			((m_aNeibPlanEntryConfig[i].bqd_entry>0) && (m_aNeibPlanEntryConfig[i].bqd_entry==entry))))
		{
			GpcLogTrainMessage(0,5, "[FROM_TO_SET] 满足配置:this_conid %d neib_conid %d fjksta %d bqd_entry %d exit %d",
				m_aNeibPlanEntryConfig[i].dest_section,
				m_aNeibPlanEntryConfig[i].source_section,
				m_aNeibPlanEntryConfig[i].fjk_station,
				m_aNeibPlanEntryConfig[i].bqd_entry,
				m_aNeibPlanEntryConfig[i].exit);

			return m_aNeibPlanEntryConfig[i].source_section;
		}
	}
	
	return NULL_CONID;
}

CONID   CTGFunctionConfig::GetTrainRunToConid(int src_conid, int fjk_staid, int exit)
{
	for(int i=0; i<m_aNeibPlanEntryConfig.size(); i++)
	{
		if(m_aNeibPlanEntryConfig[i].source_section == src_conid && 
			m_aNeibPlanEntryConfig[i].fjk_station == fjk_staid &&
			m_aNeibPlanEntryConfig[i].exit == exit)
		{
			GpcLogTrainMessage(0, 5, "[FROM_TO_SET] 满足配置:this_conid %d neib_conid %d fjksta %d bqd_entry %d exit %d",
				m_aNeibPlanEntryConfig[i].dest_section,
				m_aNeibPlanEntryConfig[i].source_section,
				m_aNeibPlanEntryConfig[i].fjk_station,
				m_aNeibPlanEntryConfig[i].bqd_entry,
				m_aNeibPlanEntryConfig[i].exit);
			return m_aNeibPlanEntryConfig[i].dest_section;
		}
	}
	
	return NULL_CONID;
}

int  CTGFunctionConfig::GetFjkStatisticMethod(int station)
{
	for(int i=0; i<m_vecFjkStatisticLateTime.size(); i++)
	{
		if(m_vecFjkStatisticLateTime[i].station == station)
		{
			return m_vecFjkStatisticLateTime[i].method;
		}
	}

	return 0;
}

int  CTGFunctionConfig::GetAutoAdjustStepFlag(int bit) const
{
	return ((m_AutoAdjustStepFlag>>bit)&0x01);
}


void CTGFunctionConfig::SetAutoAdjustStepFlag(int bit,int va)
{
	if(va)
		m_AutoAdjustStepFlag|=1<<bit;
	else
		m_AutoAdjustStepFlag&=~(1<<bit);
}

bool CTGFunctionConfig::IsDieselLocomotiveDrawGreen(int con_id)
{
	for(int i=0; i< m_vecConDieselLocomotiveDrawGreen.size(); i++)
	{
		if(con_id == m_vecConDieselLocomotiveDrawGreen[i])
			return true;
	}

	return false;
}

int   CTGFunctionConfig::GetFJKStationPreList_SY(int this_section, int neib_section, int id, int station[], int limit, int& accept_side, int& accept_stop, int& accept_arrive_depart)
{
	int cnt=0;
	const NeibPlanSY* pNeibPlan=GetNeibPlanSY(this_section, neib_section, id);
	if(pNeibPlan)
	{
		accept_side = pNeibPlan->accept_side;
		accept_stop = pNeibPlan->accept_stop;
		accept_arrive_depart=pNeibPlan->accept_arrive_depart;
		for(int i=0; (i<pNeibPlan->station_count) && (i<limit-1); i++)
		{
			station[cnt]=pNeibPlan->station_list[i];
			cnt++;
		}
	
		for(int i=cnt-1; i>=0; i--)
		{
			if(station[i] != pNeibPlan->fjk_station)
			{
				station[i]=0; cnt--;
			}
			else
			{
				break;
			}
		}
	}

	return cnt;
}

const NeibConsolePlanSY* CTGFunctionConfig::GetNeibConsolePlan(int this_section,int neib_section)
{
	for(int i=0;i<console_neibplan.size();i++)
	{
		if(console_neibplan[i].this_section!=this_section)
			continue;

		for(int k=0; k<console_neibplan[i].neib_console_count; k++)
		{
			if(console_neibplan[i].neib_console[k].neib_section==neib_section)
			{
				return &console_neibplan[i].neib_console[k];
			}
		}
	}

	return NULL;
}

const NeibPlanSY* CTGFunctionConfig::GetNeibPlanSY(int this_section, int neib_section, int id)
{
	const NeibConsolePlanSY* pNeibConsolePlan=GetNeibConsolePlan(this_section, neib_section);
	if(pNeibConsolePlan)
	{
		if(pNeibConsolePlan->neibplan_count>id && id>=0)
			return &(pNeibConsolePlan->neib_plan[id]);
	}

	return NULL;
}

const NeibPlanReverse*  CTGFunctionConfig::GetStationList_Reverse(int this_section, int neib_section, int id)
{
	const NeibConsolePlanReverse* pNeibConsole=GetNeibPlan_Reverse(this_section, neib_section);
	if(pNeibConsole)
	{
		if(pNeibConsole->neibplan_num>id && id>=0)
			return &(pNeibConsole->neib_plan[id]);
	}

	return NULL;
}

const NeibConsolePlanReverse*  CTGFunctionConfig::GetNeibPlan_Reverse(int this_section,int neib_section)
{
	for(int k=0; k<console_reverse_neibplan.size();k++)
	{
		if(console_reverse_neibplan[k].this_section==this_section)
		{
			for(int i=0; i<console_reverse_neibplan[k].neib_console_num; i++)
			{
				if(console_reverse_neibplan[k].neib_console[i].neib_section==neib_section)
				{
					return &(console_reverse_neibplan[k].neib_console[i]);
				}
			}
		}
	}
	
	return NULL;
}

int GetExitEntry(const CString& strentry, int exit_entry[], const int maxlimit)
{
	CString tmp,str=strentry;
	if(str=="")
		return 0;

	int num=0;
	do
	{
		int pos=str.Find('*',0);
		if(pos==-1)
		{
			exit_entry[num]=atoi(str);
			num++;
			break;
		}
		tmp=str.Left(pos);
		exit_entry[num]=atoi(tmp);
		num++;

		str=str.Right(str.GetLength()-pos-1);
	}while(maxlimit>num);

	return num;
}

int GetStationIndex(const CString& strindex, int index[], const int maxlimit)
{
	CString tmp,str=strindex,tmp1;
	if(str=="")
		return 0;

	int num=0;
	do
	{
		int in_index=0,out_index=0;
		int pos=str.Find('*',0);
		if(pos==-1)
		{
			pos=str.Find('|',0);
			if(pos==-1)
			{
				in_index=atoi(str);
				out_index=in_index;
			}
			else
			{
				tmp=str.Left(pos);
				str=str.Right(str.GetLength()-pos-1);

				in_index=atoi(tmp);
				out_index=atoi(str);
			}
			index[num]=in_index;
			num++;
			index[num]=out_index;
			num++;
			break;
		}
		tmp=str.Left(pos);
		str=str.Right(str.GetLength()-pos-1);
		
		pos=tmp.Find('|',0);
		if(pos==-1)
		{
			in_index=atoi(tmp);
			out_index=in_index;
		}
		else
		{
			tmp1=tmp.Left(pos);
			tmp=tmp.Right(tmp.GetLength()-pos-1);

			in_index=atoi(tmp1);
			out_index=atoi(tmp);
		}

		index[num]=in_index;
		num++;
		index[num]=out_index;
		num++;
	}while(maxlimit>num);

	return num;
}

bool  CTGFunctionConfig::LoadNeibPlanReverseXML(const CString pathname)
{
	console_reverse_neibplan.size();
	
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
	MSXML::IXMLDOMNodePtr pRootNode,pChildNode;
	

	pRootNode = pXMLDoc->selectSingleNode( "REVERSE_NEIB_PLAN");
	if(pRootNode == NULL)
	{
		return false;
	}

	GpcLogSysMessage(0, "调整邻台计划配置");
	// THIS_CONSOLE
	MSXML::IXMLDOMNodeListPtr list_console = pRootNode->selectNodes( "THIS_CONSOLE");
	for(int m = 0; m < list_console->Getlength(); m++)
	{
		MSXML::IXMLDOMNodePtr pNode_ConsolePlan = list_console->Getitem(m);
		if(pNode_ConsolePlan==NULL)
			continue;
		
		ConsoleNeibPlan console;		
		pChildNode = pNode_ConsolePlan->selectSingleNode("@con_id");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			console.this_section = atoi(CString(v.bstrVal));	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		int neib_console_num=0;
		MSXML::IXMLDOMNodeListPtr list_neib_console = pNode_ConsolePlan->selectNodes( "NEIB_CONSOLE");
		for(int p=0; p<list_neib_console->Getlength(); p++)
		{
			MSXML::IXMLDOMNodePtr pNode_NeibConsolePlan = list_neib_console->Getitem(p);
			if(pNode_NeibConsolePlan==NULL)
				continue;

			pChildNode = pNode_NeibConsolePlan->selectSingleNode("@con_id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				console.neib_console[neib_console_num].neib_section = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			int neibplan_num=0;
			MSXML::IXMLDOMNodeListPtr list_neibplan = pNode_NeibConsolePlan->selectNodes( "RULE");
			for(int q=0; q<list_neibplan->Getlength(); q++)
			{
				MSXML::IXMLDOMNodePtr pNode_Station = list_neibplan->Getitem(q);
				if(pNode_Station==NULL)
					continue;
				
				NeibPlanReverse x;

				pChildNode = pNode_Station->selectSingleNode("@flag");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.flag_count=GetExitEntry(CString(v.bstrVal), x.flag, sizeof(x.flag)/sizeof(x.flag[0]));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				pChildNode = pNode_Station->selectSingleNode("@ctc_station");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.station = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				x.station_count=0;
				pChildNode = pNode_Station->selectSingleNode("@this_station_list");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.station_count=GetExitEntry(CString(v.bstrVal), x.station_list, sizeof(x.station_list)/sizeof(x.station_list[0]));
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				int station_index_count=0;
				pChildNode = pNode_Station->selectSingleNode("@station_index");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					
					station_index_count=GetStationIndex(CString(v.bstrVal), x.station_index_list, sizeof(x.station_index_list)/sizeof(x.station_index_list[0]));
						
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				
				x.only_accept_depart=1;
				pChildNode = pNode_Station->selectSingleNode("@only_accept_depart");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					
					x.only_accept_depart=atoi(CString(v.bstrVal));
						
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				x.accept_stop=1;
				pChildNode = pNode_Station->selectSingleNode("@accept_stop");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					
					x.accept_stop=atoi(CString(v.bstrVal));
						
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				

				x.station_pos=-1;
				for(int i=0; i<x.station_count; i++)
				{
					if(x.station==x.station_list[i])
					{
						x.station_pos=i;
						break;
					}
				}
				
				if(x.flag_count<=0||x.flag_count>=3)
					continue;
				
				if(x.station_count==0||x.station_pos==-1)
					continue;

				if(2*x.station_count!=station_index_count)
				{
					CString text;
					text.Format("data\\neibplanreverse.xml中配置错误: THIS_CONSOLE=%d NEIB_CONSOLE=%d 的配置计划中有错误, 本台车站个数 %d 和车站站序数目 %d, 不等", 
						console.this_section, console.neib_console[neib_console_num].neib_section, x.station_count, station_index_count/2);
					continue;
				}
					
				console.neib_console[neib_console_num].neib_plan[neibplan_num]=x;
				neibplan_num++;
			}
			console.neib_console[neib_console_num].neibplan_num=neibplan_num;
			neib_console_num++;
		}
		console.neib_console_num=neib_console_num;

		for(int i=0; i<console_reverse_neibplan.size(); i++)
		{
			if(console_reverse_neibplan[i].this_section==console.this_section)
				break;
		}

		if(i<console_reverse_neibplan.size())
		{
			CString text;
			text.Format("请注意: CONSOLE con_id=\"%d\" 计划配置重复, 检查data\\neibplanreverse.xml", console.this_section);
			AfxMessageBox(text);
			return false;
		}

		GpcLogSysMessage(0, "-------------------------------------");
		for(int a=0; a<console.neib_console_num; a++)
		{
			for(int b=0; b<console.neib_console[a].neibplan_num; b++)
			{
				CString flaglist="", tmp;
				for(int n=0; n<console.neib_console[a].neib_plan[b].flag_count; n++)
				{
					if(n==0)
						flaglist.Format("%d", console.neib_console[a].neib_plan[b].flag[n]);
					else
					{
						tmp.Format("*%d", console.neib_console[a].neib_plan[b].flag[n]);
						flaglist+=tmp;
					}
				}

				CString stationlist="";
				for(int n=0; n<console.neib_console[a].neib_plan[b].station_count; n++)
				{
					if(n==0)
						stationlist.Format("%d", console.neib_console[a].neib_plan[b].station_list[n]);
					else
					{
						tmp.Format("*%d", console.neib_console[a].neib_plan[b].station_list[n]);
						stationlist+=tmp;
					}
				}

				GpcLogSysMessage(0, "%d=this_section=%d,neib_section=%d,ctc_station=%d, accept_stop=%d, only_accept_depart=%d, flag=%s, stationlist:%s", 
					b,console.this_section, console.neib_console[a].neib_section, 
					console.neib_console[a].neib_plan[b].station,
					console.neib_console[a].neib_plan[b].accept_stop,
					console.neib_console[a].neib_plan[b].only_accept_depart,
					flaglist,stationlist);
			}
		}

		console_reverse_neibplan.push_back(console);
	}

	return true;
}
				
bool  CTGFunctionConfig::IsEnableThirtyByPlan(const int con_id)
{
	if(m_bEnableThirtyByPlan)
		return true;

	for(int i=0; i<m_vecSectionThirtyByPlan.size(); i++)
	{
		if(con_id==m_vecSectionThirtyByPlan[i])
			return true;
	}
	
	return false;
}

// 沈阳正向邻台计划配置
bool  CTGFunctionConfig::LoadNeibPlanSYXML(const CString pathname)
{
	console_neibplan.size();
	
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
	MSXML::IXMLDOMNodePtr pRootNode,pChildNode;
	

	pRootNode = pXMLDoc->selectSingleNode( "NEIB_PLAN");
	if(pRootNode == NULL)
	{
		return false;
	}

	GpcLogSysMessage(0, "邻台计划配置");
	// THIS_CONSOLE
	MSXML::IXMLDOMNodeListPtr list_console = pRootNode->selectNodes( "THIS_CONSOLE");
	for(int m = 0; m < list_console->Getlength(); m++)
	{
		MSXML::IXMLDOMNodePtr pNode_ConsolePlan = list_console->Getitem(m);
		if(pNode_ConsolePlan==NULL)
			continue;
		
		ConsoleNeibPlanSY console;		
		pChildNode = pNode_ConsolePlan->selectSingleNode("@con_id");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			console.this_section = atoi(CString(v.bstrVal));	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		int neib_console_num=0;
		MSXML::IXMLDOMNodeListPtr list_neib_console = pNode_ConsolePlan->selectNodes( "NEIB_CONSOLE");
		for(int p=0; p<list_neib_console->Getlength(); p++)
		{
			MSXML::IXMLDOMNodePtr pNode_NeibConsolePlan = list_neib_console->Getitem(p);
			if(pNode_NeibConsolePlan==NULL)
				continue;

			pChildNode = pNode_NeibConsolePlan->selectSingleNode("@con_id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				console.neib_console[neib_console_num].neib_section = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			int neibplan_num=0;
			MSXML::IXMLDOMNodeListPtr list_neibplan = pNode_NeibConsolePlan->selectNodes( "RULE");
			for(int q=0; q<list_neibplan->Getlength(); q++)
			{
				MSXML::IXMLDOMNodePtr pNode_Rule = list_neibplan->Getitem(q);
				if(pNode_Rule==NULL)
					continue;
				
				NeibPlanSY x;

				x.first_station=-1;
				pChildNode = pNode_Rule->selectSingleNode("@first_station");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.first_station = atoi(CString(v.bstrVal));						
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				x.fjk_station=-1;
				pChildNode = pNode_Rule->selectSingleNode("@fjk_station");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.fjk_station = atoi(CString(v.bstrVal));						
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				x.accept_side=1;
				pChildNode = pNode_Rule->selectSingleNode("@accept_side");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.accept_side = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				x.station_count=0;
				pChildNode = pNode_Rule->selectSingleNode("@station_list");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.station_count=GetExitEntry(CString(v.bstrVal), x.station_list, sizeof(x.station_list)/sizeof(x.station_list[0]));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				for(int i=0; i<sizeof(x.station_index_list)/sizeof(x.station_index_list[0]); i++)
					x.station_index_list[i]=-1;

				pChildNode = pNode_Rule->selectSingleNode("@station_index_list");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					GetStationIndex(CString(v.bstrVal), x.station_index_list, sizeof(x.station_index_list)/sizeof(x.station_index_list[0]));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				x.passway=0;
				pChildNode = pNode_Rule->selectSingleNode("@passway");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.passway = atol(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				x.exit_count=0;
				pChildNode = pNode_Rule->selectSingleNode("@last_station_exit");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.exit_count=GetExitEntry(CString(v.bstrVal), x.exit, sizeof(x.exit)/sizeof(x.exit[0]));
					SysFreeString(v.bstrVal);
					v.Clear();
				}


				x.flag=0;
				pChildNode = pNode_Rule->selectSingleNode("@flag");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.flag = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				x.accept_stop=1;
				pChildNode = pNode_Rule->selectSingleNode("@accept_stop");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.accept_stop = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				x.accept_arrive_depart=0;
				pChildNode = pNode_Rule->selectSingleNode("@accept_arrive_depart");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					x.accept_arrive_depart = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				
				if(x.exit_count<=0||x.station_count<=0)
				{
					CString text;
					text.Format("data\\neibplan.xml中配置错误: THIS_CONSOLE=%d NEIB_CONSOLE=%d 的配置计划中有错误, last_station_exit 或 station_list 没有配置", 
							console.this_section, console.neib_console[neib_console_num].neib_section);

					GpcLogSysMessage(0, "%s", text);
					continue;
				}

				if(x.first_station!=x.station_list[0])
				{
					CString text;
					text.Format("data\\neibplan.xml中配置错误: THIS_CONSOLE=%d NEIB_CONSOLE=%d 的配置计划中有错误, first_station=%d, 但是实际车站序列中首站为%d，不相同不正确", 
							console.this_section, console.neib_console[neib_console_num].neib_section, x.first_station, x.station_list[0]);

					GpcLogSysMessage(0, "%s", text);
					continue;
				}

				for(int a=0; a<x.station_count; a++)
				{
					if(x.station_list[a]==x.fjk_station)
						break;
				}

				if(a>=x.station_count)
				{
					CString text;
					text.Format("data\\neibplan.xml中配置错误: THIS_CONSOLE=%d NEIB_CONSOLE=%d 的配置计划中有错误, 分界车站 %d 不在 station_list中", 
							console.this_section, console.neib_console[neib_console_num].neib_section,x.fjk_station);

					GpcLogSysMessage(0, "%s", text);
					continue;
				}

				if(x.station_index_list[0]==-1 || x.station_index_list[1]==-1)
				{
					CString text;
					text.Format("data\\neibplan.xml中配置错误: THIS_CONSOLE=%d NEIB_CONSOLE=%d 的配置计划中有错误, 没有配置车站站序", 
							console.this_section, console.neib_console[neib_console_num].neib_section);

					GpcLogSysMessage(0, "%s", text);
					continue;
				}

				console.neib_console[neib_console_num].neib_plan[neibplan_num]=x;
				neibplan_num++;
			}
			console.neib_console[neib_console_num].neibplan_count=neibplan_num;
			neib_console_num++;
		}
		console.neib_console_count=neib_console_num;

		for(int i=0; i<console_neibplan.size(); i++)
		{
			if(console_neibplan[i].this_section==console.this_section)
				break;
		}

		if(i<console_neibplan.size())
		{
			CString text;
			text.Format("请注意: THIS_CONSOLE con_id=\"%d\" 计划配置重复, 检查data\\neibplan.xml", console.this_section);
			AfxMessageBox(text);
			return false;
		}

		GpcLogSysMessage(0, "-------------------------------------");
		for(int a=0; a<console.neib_console_count; a++)
		{
			for(int b=0; b<console.neib_console[a].neibplan_count; b++)
			{
				CString exitlist="", tmp;
				for(int n=0; n<console.neib_console[a].neib_plan[b].exit_count; n++)
				{
					if(n==0)
						exitlist.Format("%d", console.neib_console[a].neib_plan[b].exit[n]);
					else
					{
						tmp.Format("*%d", console.neib_console[a].neib_plan[b].exit[n]);
						exitlist+=tmp;
					}
				}

				CString stationlist="", station_index_list="";
				for(int n=0; n<console.neib_console[a].neib_plan[b].station_count; n++)
				{
					if(n==0)
					{
						stationlist.Format("%d", console.neib_console[a].neib_plan[b].station_list[n]);
						if(console.neib_console[a].neib_plan[b].station_index_list[2*n]==console.neib_console[a].neib_plan[b].station_index_list[2*n+1])
							station_index_list.Format("%d", console.neib_console[a].neib_plan[b].station_index_list[2*n]);
						else
							station_index_list.Format("%d|%d", console.neib_console[a].neib_plan[b].station_index_list[2*n],console.neib_console[a].neib_plan[b].station_index_list[2*n+1]);
					}
					else
					{
						tmp.Format("*%d", console.neib_console[a].neib_plan[b].station_list[n]);
						stationlist+=tmp;

						if(console.neib_console[a].neib_plan[b].station_index_list[2*n]>=0 || console.neib_console[a].neib_plan[b].station_index_list[2*n+1]>=0)
						{
							if(console.neib_console[a].neib_plan[b].station_index_list[2*n]==console.neib_console[a].neib_plan[b].station_index_list[2*n+1])
								tmp.Format("*%d", console.neib_console[a].neib_plan[b].station_index_list[2*n]);
							else
								tmp.Format("*%d|%d", console.neib_console[a].neib_plan[b].station_index_list[2*n],console.neib_console[a].neib_plan[b].station_index_list[2*n+1]);
							station_index_list+=tmp;
						}
					}
				}

				GpcLogSysMessage(0, "%d=this_section=%d,neib_section=%d,fjk_station=%d, accept_side=%d, accept_stop=%d, accept_arrive_depart=%d, stationlist:%s, station_index_list:%s, exit=%s, passway=%d, flag=%d", 
					b,console.this_section, console.neib_console[a].neib_section, 
					console.neib_console[a].neib_plan[b].fjk_station,
					console.neib_console[a].neib_plan[b].accept_side,
					console.neib_console[a].neib_plan[b].accept_stop,
					console.neib_console[a].neib_plan[b].accept_arrive_depart,
					stationlist, station_index_list,exitlist,
					console.neib_console[a].neib_plan[b].passway,
					console.neib_console[a].neib_plan[b].flag);
			}
		}

		console_neibplan.push_back(console);
	}

	return true;
}
   
const ConsoleChangeId* CTGFunctionConfig::GetConsoleChangeId(int con_id)
{
	std::map<int, ConsoleChangeId>::iterator it = console_change_trainid.find(con_id);
	if(it == console_change_trainid.end())
		return NULL;

	return &(it->second);
}

// 沈阳变更车次配置
bool  CTGFunctionConfig::LoadChangeTrainidXML(const CString pathname)
{
	console_neibplan.size();
	
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
	MSXML::IXMLDOMNodePtr pRootNode,pChildNode;
	

	pRootNode = pXMLDoc->selectSingleNode( "ROOT");
	if(pRootNode == NULL)
	{
		return false;
	}

	GpcLogSysMessage(0, "车次变更配置");
	// THIS_CONSOLE
	MSXML::IXMLDOMNodeListPtr list_console = pRootNode->selectNodes( "CONSOLE");
	for(int m = 0; m < list_console->Getlength(); m++)
	{
		MSXML::IXMLDOMNodePtr pNode_ConsolePlan = list_console->Getitem(m);
		if(pNode_ConsolePlan==NULL)
			continue;
		
		ConsoleChangeId console;		
		pChildNode = pNode_ConsolePlan->selectSingleNode("@con_id");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			console.con_id = atoi(CString(v.bstrVal));	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		int num=0;
		MSXML::IXMLDOMNodeListPtr list_neib_console = pNode_ConsolePlan->selectNodes( "RULE");
		for(int p=0; p<list_neib_console->Getlength()&&num<10; p++)
		{
			MSXML::IXMLDOMNodePtr pNode_NeibConsolePlan = list_neib_console->Getitem(p);
			if(pNode_NeibConsolePlan==NULL)
				continue;

			ChangeTrainId info;
			pChildNode = pNode_NeibConsolePlan->selectSingleNode("@flag");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				info.flag = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			pChildNode = pNode_NeibConsolePlan->selectSingleNode("@station");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				info.station = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			pChildNode = pNode_NeibConsolePlan->selectSingleNode("@pre_station");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				info.pre_station = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			pChildNode = pNode_NeibConsolePlan->selectSingleNode("@next_station");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				info.next_station = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			pChildNode = pNode_NeibConsolePlan->selectSingleNode("@entry");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				info.entry_count=GetExitEntry(CString(v.bstrVal), info.entry, sizeof(info.entry)/sizeof(info.entry[0]));
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			pChildNode = pNode_NeibConsolePlan->selectSingleNode("@method");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				info.method = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			info.text[0]=0;
			pChildNode = pNode_NeibConsolePlan->selectSingleNode("@text");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				CString text = v.bstrVal;	
				SysFreeString(v.bstrVal);
				v.Clear();

				if(text!="")
				{
					strncpy(info.text, text, 19);
					info.text[19]=0;
				}
			}

			console.info[num]=info;
			num++;
		}
		console.info_count=num;

		console_change_trainid.insert(make_pair(console.con_id, console));

		GpcLogSysMessage(0, "-------------------------------------");
		for(int a=0; a<console.info_count; a++)
		{
			CString exitlist="", tmp;
			for(int n=0; n<console.info[a].entry_count; n++)
			{
				if(n==0)
					exitlist.Format("%d", console.info[a].entry[0]);
				else
				{
					tmp.Format("*%d", console.info[a].entry[n]);
					exitlist+=tmp;
				}
			}

			GpcLogSysMessage(0, "section=%d,flag=%d,method=%d,station=%d,pre_station=%d,next_station=%d,after=%s", 
				console.con_id, console.info[a].flag,  console.info[a].method,
				console.info[a].station,console.info[a].pre_station, console.info[a].next_station, exitlist);
		}
	}

	return true;
}

int  CTGFunctionConfig::SetElecTrain(int con_id, int station, int exit, int entry)
{
	std::map<int, SectionDefaultElec>::iterator it = default_elec.find(con_id);
	if(it==default_elec.end())
		return -1;

	for(int a=0; a<it->second.station_count; a++)
	{
		if(it->second.default_elec[a].station==station && it->second.default_elec[a].entry==entry && entry>0)
			return it->second.default_elec[a].flag;

		if(it->second.default_elec[a].station==station && it->second.default_elec[a].exit==exit && exit>0)
			return it->second.default_elec[a].flag;
	}

	return -1;
}

bool CTGFunctionConfig::IsSideNotChange(int con_id, int station, int side, int flag)
{
	std::map<int, SectionStationNotChange>::iterator it = side_not_change.find(con_id);
	if(it==side_not_change.end())
		return false;

	for(int a=0; a<it->second.station_count; a++)
	{
		if(it->second.info[a].station==station && ((it->second.info[a].flag==3) || (it->second.info[a].flag==flag)))
		{
			for(int s=0; s<it->second.info[a].side_count; s++)
			if(it->second.info[a].side[s]==side)
				return true;
		}
	}

	return false;
}

bool CTGFunctionConfig::LoadTrainDepartCheckXML(const CString pathname)
{
	section_check_depart.size();
	
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
	MSXML::IXMLDOMNodePtr pRootNode,pChildNode;
	

	pRootNode = pXMLDoc->selectSingleNode( "ROOT");
	if(pRootNode == NULL)
	{
		return false;
	}

	GpcLogSysMessage(0, "列车发车检查报警配置");
	// CONSOLE
	MSXML::IXMLDOMNodeListPtr list_console = pRootNode->selectNodes( "CONSOLE");
	for(int m = 0; m < list_console->Getlength(); m++)
	{
		MSXML::IXMLDOMNodePtr pNode_Console = list_console->Getitem(m);
		if(pNode_Console==NULL)
			continue;
		
		int con_id=0;
		pChildNode = pNode_Console->selectSingleNode("@con_id");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			con_id = atoi(CString(v.bstrVal));	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		if(con_id<=0)
			continue;

		MSXML::IXMLDOMNodeListPtr list_warning = pNode_Console->selectNodes( "WARNING_STATION");
		for(int p=0; p<list_warning->Getlength(); p++)
		{
			MSXML::IXMLDOMNodePtr pNode_Warning = list_warning->Getitem(p);
			if(pNode_Warning==NULL)
				continue;

			int station_warning=0, station_check=0;
			pChildNode = pNode_Warning->selectSingleNode("@id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				station_warning = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			MSXML::IXMLDOMNodeListPtr list_check = pNode_Warning->selectNodes( "CHECK_STATION");
			for(int p=0; p<list_check->Getlength(); p++)
			{
				MSXML::IXMLDOMNodePtr pNode_Check = list_check->Getitem(p);
				if(pNode_Check==NULL)
					continue;

				// id
				pChildNode = pNode_Check->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					station_check = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				if(station_warning>0&&station_check>0)
				{
					std::map<int, SectionCheckTrainDepart>::iterator it = section_check_depart.find(con_id);
					if(it!=section_check_depart.end())
					{
						for(int a=0; a<it->second.count; a++)
						{
							if((it->second.check[a].station_warning==station_warning) &&
							   (it->second.check[a].station_check==station_check))
							{
								break;
							}
						}

						if(a>=it->second.count && a<sizeof(it->second.check)/sizeof(it->second.check[0]))
						{
							it->second.check[a].station_warning=station_warning;
							it->second.check[a].station_check=station_check;
							it->second.count++;
						}
					}
					else
					{
						SectionCheckTrainDepart depart;
						depart.con_id=con_id;
						depart.count=1;
						depart.check[0].station_warning=station_warning;
						depart.check[0].station_check=station_check;
						
						section_check_depart.insert(make_pair(con_id, depart));
					}
				}
			}
		}
	}

	std::map<int, SectionCheckTrainDepart>::iterator it = section_check_depart.begin();
	for(;it!=section_check_depart.end();it++)
	{
		for(int i=0; i<it->second.count; i++)
		{
			GpcLogSysMessage(0, "section=%d,warning_station=%d,check_station=%d", 
								it->second.con_id, 
								it->second.check[i].station_warning,
								it->second.check[i].station_check);
		}
	}

	return true;
}

bool CTGFunctionConfig::GetTrainDepartCheckInfo(int con_id, int station, std::vector<int>& list)
{
	list.clear();

	std::map<int, SectionCheckTrainDepart>::iterator it = section_check_depart.find(con_id);
	if(it!=section_check_depart.end())
	{
		for(int c=0; c<it->second.count; c++)
		{
			if(it->second.check[c].station_warning==station)
			{
				list.push_back(it->second.check[c].station_check);
			}
		}
	}

	if(list.size()>0)
		return true;

	return false;
}

// 用于邻台计划 true:外局区段 false:本局区段
bool CTGFunctionConfig::IsOtherBureaSection(int sectionno)
{
	for(int i = 0; i < m_otherBureaSection.size();i++)
	{
		if (sectionno == m_otherBureaSection[i])
		{
			return true;
		}
	}
	return false;
}

// 上海局新邻台计划模式下获取本台停车时分 type:0 普速列车 1 动车组
int CTGFunctionConfig::GetNeibScheduleMinStopTime_Shanghai(BOOL isCrh)
{
	int min_stop_time =0;
	if (isCrh)
		min_stop_time = m_nExactNeibPlanModeCrhStopTime*60; 
	else
		min_stop_time = m_nExactNeibPlanModeOtherStopTime*60; 

	return min_stop_time;
}

bool CTGFunctionConfig::UseOPMSTrainTimeTrainType(int type)
{
	for(int i = 0; i < m_UseOPMSTrainType.size();i++)
	{
		if (type == m_UseOPMSTrainType[i])
		{
			return true;
		}
	}
	return false;
}

bool  CTGFunctionConfig::IsLimitSectionCalToConid(int conid, int station, int nextstation)
{
	for(int i=0; i<limit_section.size(); i++)
	{
		if(limit_section[i].conid==conid && limit_section[i].station==station && limit_section[i].nextstation==nextstation)
			return true;
	}

	return false;
}

bool  CTGFunctionConfig::IsNotCalFromConid(int conid, int station, int entry)
{
	for(int i=0; i<notcalfromconid.size(); i++)
	{
		if(notcalfromconid[i].conid==conid && notcalfromconid[i].station==station && notcalfromconid[i].entry==entry)
			return true;
	}

	return false;
}

bool CTGFunctionConfig::IsThisSectionStation(int this_section, int station)
{
	std::map<int, NeibSectionStation>::iterator it = m_neibSectionStation.find(this_section);
	if(it == m_neibSectionStation.end())
		return false;

	for(int i=0; i<it->second.station_count; i++)
	{
		if(it->second.station_no[i]==station && it->second.this_section[i]==1)
		{
			return true;
		}
	}

	return false;
}

bool CTGFunctionConfig::IsNotThisSectionStation(int this_section, int station)
{
	std::map<int, NeibSectionStation>::iterator it = m_neibSectionStation.find(this_section);
	if(it == m_neibSectionStation.end())
		return false;

	for(int i=0; i<it->second.station_count; i++)
	{
		if(it->second.station_no[i]==station && it->second.this_section[i]==0)
		{
			return true;
		}
	}

	return false;
}

UINT CTGFunctionConfig::GetNotThisSectionStation(int this_section, int station[], int nMax)
{
	std::map<int, NeibSectionStation>::iterator it = m_neibSectionStation.find(this_section);
	if(it == m_neibSectionStation.end())
		return 0;

	UINT count=0;
	for(UINT i=0; i<it->second.station_count && i<nMax; i++)
	{
		if( it->second.this_section[i]==0)
		{
			station[count]=it->second.station_no[i];
			count++;
		}
	}

	return count;
}

int CTGFunctionConfig::GetTrainDirection(CString strTrainId)
{
	int nDirection=-1;

	int len = strTrainId.GetLength();
	for(int i= len-1;i>=0;i--)
	{
		char lastchar = strTrainId.GetAt(i);
		if(lastchar >= '0' && lastchar <= '9')
		{
			if(lastchar%2 == 0)
				nDirection = 0;//UP_DIRECT;
			else
				nDirection = 1;//DOWN_DIRECT;
			break;
		}
	}

	return nDirection;
}

bool CTGFunctionConfig::IsNoLimitStation(int this_section, int station)
{
	std::map<int, NoLimitSectionStation>::iterator it = m_noLimitStationList.find(this_section);
	if(it == m_noLimitStationList.end())
		return false;

	for(int i=0; i<it->second.station_count; i++)
	{
		if(it->second.station_no[i]==station)
			return true;
	}

	return false;
}

UINT CTGFunctionConfig::GetNoLimitStationList(int this_section, int station[], int nMax)
{
	std::map<int, NoLimitSectionStation>::iterator it = m_noLimitStationList.find(this_section);
	if(it == m_noLimitStationList.end())
		return 0;

	for(UINT i=0; i<it->second.station_count && i<nMax; i++)
		station[i]=it->second.station_no[i];

	return it->second.station_count;
}

bool CTGFunctionConfig::LoadNoLimitStitonXml(CString fileName)
{	
	m_noLimitStationList.clear();

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
		return false;

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( fileName ) ) )
		return false; 

	MSXML::IXMLDOMNodePtr pNode,pRootNode,pChildNode;

	pRootNode = pXMLDoc->selectSingleNode( "SECTION_LIST");
	if(pRootNode == NULL)
		return false;

	MSXML::IXMLDOMNodeListPtr pSectionList = pRootNode->selectNodes("SECTION");
	if(NULL != pSectionList)
	{
		for(int idx = 0; idx < pSectionList->Getlength(); idx++)
		{
			_variant_t v;
			int sectionid=0;
			pNode = pSectionList->Getitem(idx)->selectSingleNode("@id");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				sectionid = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			if (sectionid>0)
			{
				NoLimitSectionStation sectionstation;
				MSXML::IXMLDOMNodeListPtr pStationList = pSectionList->Getitem(idx)->selectNodes("STATION");
				if (pStationList!=NULL)
				{
					for (int i=0;i<pStationList->Getlength();i++)
					{
						pNode = pStationList->Getitem(i);
						if(pNode)
						{
							v = pNode->GetnodeTypedValue();
							sectionstation.station_no[i] = atoi(CString(v.bstrVal));
							SysFreeString(v.bstrVal);
							v.Clear();
						}
					}

					sectionstation.station_count = i;
					m_noLimitStationList.insert(make_pair(sectionid, sectionstation));
				}
			}
		}
	}

	if (m_noLimitStationList.size()>0)
		return true;
	else
		return false;
}

int CTGFunctionConfig::GetNeibPlanMode(int this_section, int neib_section)
{
	for(int i=0; i<m_neibRecvAllStation.size(); i++) 
	{
		if((m_neibRecvAllStation[i].nThisSection==this_section) && (m_neibRecvAllStation[i].nNeibSection==neib_section))
		{
			return 4;
		}
	}

	for(int i=0; i<m_neibAdjustStation.size(); i++) 
	{
		if((m_neibAdjustStation[i].nThisSection==this_section) && (m_neibAdjustStation[i].nNeibSection==neib_section))
		{
			return 3;
		}
	}

	for(int i=0; i<m_aNeibPlanMode.size(); i++)
	{
		if(m_aNeibPlanMode[i].this_section==this_section && m_aNeibPlanMode[i].neib_section==neib_section)
			return m_aNeibPlanMode[i].mode;
	}

	return 0;
}

bool CTGFunctionConfig::IsThisSectionHasTDMSLineStation(int section)
{
	std::vector<USE_TDMS_LINE_STATION>::iterator it = m_UserTDMSLineStation.begin();
	for (;it!=m_UserTDMSLineStation.end();it++)
	{
		if (it->con_id==section)
			return true;
	}

	return false;
}

bool CTGFunctionConfig::IsStationUseTDMSLine(int section, int station1, int station2, int station1_nd, int station2_nd)
{
	bool bMatchConfig=false;
	std::vector<USE_TDMS_LINE_STATION>::iterator it = m_UserTDMSLineStation.begin();
	for (;it!=m_UserTDMSLineStation.end();it++)
	{
		if((it->con_id == section) && (it->station_id1==station1)&&(it->station_id2==station2))
		{
			if((it->station_id1_nd!=255) && (it->station_id2_nd!=255))
			{
				if(it->station_id1_nd==station1_nd && it->station_id2_nd==station2_nd)
				{
					bMatchConfig=true;
					break;
				}
			}
			else if(it->station_id1_nd!=255)
			{
				if(it->station_id1_nd==station1_nd)
				{
					bMatchConfig=true;
					break;
				}
			}
			else if(it->station_id2_nd!=255)
			{
				if(it->station_id2_nd==station2_nd)
				{
					bMatchConfig=true;
					break;
				}
			}
			else
			{
				bMatchConfig=true;
				break;
			}
		}
	}
	
	return bMatchConfig;
}

int CTGFunctionConfig::GetNeibStationList(int nThisSection, int nNeibSection, NeibStationList ns[], int max_size) const
{
	int count=0;
	for(int i=0; i<m_neibRecvAllStation.size()&&count<max_size; i++) 
	{
		if((m_neibRecvAllStation[i].nThisSection==nThisSection) && (m_neibRecvAllStation[i].nNeibSection==nNeibSection))
		{
			ns[count]=m_neibRecvAllStation[i];
			count++;
		}
	}
	return count;
}

int  CTGFunctionConfig::GetNeibEqualStation(int nThisSection, int nNeibSection, NeibAdjustExtern neibPlanStation[], int max_size) const
{
	int count=0;
	for(int i=0; i<m_neibAdjustStation.size()&&count<max_size; i++) 
	{
		if((m_neibAdjustStation[i].nThisSection==nThisSection) && (m_neibAdjustStation[i].nNeibSection==nNeibSection))
		{
			neibPlanStation[count]=m_neibAdjustStation[i];
			count++;
		}
	}
	return count;
}

int CTGFunctionConfig::GetNewSplitStation(int nThisSection, int station_list[], int entry_list[], int max_size) const
{
	int count=0;
	for(int i=0; i<m_NewSplitList.size(); i++) 
	{
		if(m_NewSplitList[i].nThisSection==nThisSection)
		{
			for(int k=0; k<m_NewSplitList[i].station_pair_count; k++)
			{
				station_list[count]=m_NewSplitList[i].station[k];
				entry_list[count]  =m_NewSplitList[i].station_exit[k];
				count++;
				station_list[count]=m_NewSplitList[i].next_station[k];
				entry_list[count]  =m_NewSplitList[i].next_station_entry[k];
				count++;

				if(count+2>=max_size)
				{
					break;
				}
			}
		}
	}
	return count;
}

// 济南局,晏城--晏城北出入口设置配置
bool CTGFunctionConfig::GetStaEntry(int section, SectionStaEntry& entry)
{
	for(int i=0; i<m_nSectionStaEntryCount&&i<64; i++)
	{
		if(m_specialEntry[i].section==section)
		{
			entry=m_specialEntry[i];
			return true;
		}
	}

	return false;
}

void CTGFunctionConfig::GetWarnIdLeval(int level, WarnIdLeval& cfg) 
{ 
	cfg.InitWarnIdLeval();
	if(level==1 || level==2 || level==3)
	{
		cfg=m_stplanLeval[level];
	}
}

void  CTGFunctionConfig::LoadStplanWarnCfg(const CString pathname)
{
	for(int i=0; i<sizeof(m_stplanLeval)/sizeof(m_stplanLeval[0]); i++)
	{
		m_stplanLeval[i].InitWarnIdLeval();
	}
	strncpy(m_stplanLeval[1].text, "一级报警", 255);
	m_stplanLeval[1].clr=RGB(255,165,0);
	m_stplanLeval[1].id_count=0;

	strncpy(m_stplanLeval[2].text, "二级报警", 255);
	m_stplanLeval[2].clr=RGB(255,255,0);
	m_stplanLeval[2].id_count=0;

	strncpy(m_stplanLeval[3].text, "其他报警", 255);
	m_stplanLeval[3].clr=RGB(255,255,255);
	m_stplanLeval[3].id_count=0;

	// 铁总1-4(一级报警)
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=36;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=1;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]= 3;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=2;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]= 4;	m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=3;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]= 5;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=4;
	m_stplanLeval[1].id_count++;
    // 铁总5-7(二级报警)
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]= 8;  m_stplanLeval[2].tz_id[m_stplanLeval[2].id_count]=5;
	m_stplanLeval[2].id_count++;
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]= 9;  m_stplanLeval[2].tz_id[m_stplanLeval[2].id_count]=6;
	m_stplanLeval[2].id_count++;
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]=10;  m_stplanLeval[2].tz_id[m_stplanLeval[2].id_count]=7;
	m_stplanLeval[2].id_count++;

	// 铁总8-10(一级报警)
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=11;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=8;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=13;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=9;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=14;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=10;
	m_stplanLeval[1].id_count++;

	// 铁总11-12(二级报警)
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]=15;  m_stplanLeval[2].tz_id[m_stplanLeval[2].id_count]=11;
	m_stplanLeval[2].id_count++;
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]=16;  m_stplanLeval[2].tz_id[m_stplanLeval[2].id_count]=12;
	m_stplanLeval[2].id_count++;

	// 铁总13-25(一级报警)
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=17;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=13;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=18;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=14;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=19;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=15;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=20;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=16;
	m_stplanLeval[1].id_count++;

	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=21;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=17;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=22;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=18;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=23;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=19;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=24;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=20;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=25;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=21;
	m_stplanLeval[1].id_count++;

	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=26;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=22;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=27;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=23;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=28;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=24;
	m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=29;  m_stplanLeval[1].tz_id[m_stplanLeval[1].id_count]=25;
	m_stplanLeval[1].id_count++;
	
	// 铁总26-27(二级报警)
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]=37;  m_stplanLeval[2].id_count++;
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]=38;  m_stplanLeval[2].id_count++;

	// CTC扩充(一级报警)
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]= 1;  m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]= 2;  m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]= 6;  m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]= 7;  m_stplanLeval[1].id_count++;
	m_stplanLeval[1].id[m_stplanLeval[1].id_count]=35;  m_stplanLeval[1].id_count++;

	// CTC扩充(二级报警)
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]=30;  m_stplanLeval[2].id_count++;
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]=31;  m_stplanLeval[2].id_count++;
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]=32;  m_stplanLeval[2].id_count++;
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]=33;  m_stplanLeval[2].id_count++;
	m_stplanLeval[2].id[m_stplanLeval[2].id_count]=34;  m_stplanLeval[2].id_count++;

	// CTC扩充(三级报警)	
	m_stplanLeval[3].id[m_stplanLeval[3].id_count]=12;  m_stplanLeval[3].id_count++;

	bool bReadFromFile=true;
	MSXML::IXMLDOMNodePtr pRootNode;
	do
	{
		MSXML::IXMLDOMDocumentPtr pXMLDoc;
		HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
		if(FAILED(hr))
		{
			_com_error er(hr);
			AfxMessageBox(er.ErrorMessage());
			bReadFromFile=false;
			break;
		}

		if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
		{
			bReadFromFile=false;
			break;
		}
		
		pRootNode = pXMLDoc->selectSingleNode( "stplan_warn");
		if(pRootNode == NULL)
		{
			bReadFromFile=false; 
			break;
		}
	}while(0);

	GpcLogSysMessage(0, "铁总统一界面阶段计划报警配置");
	if(bReadFromFile)
	{
		MSXML::IXMLDOMNodePtr pChildNode;
		// level
		MSXML::IXMLDOMNodeListPtr list_level = pRootNode->selectNodes( "level/warn");
		for(int p=0; p<list_level->Getlength(); p++)
		{
			MSXML::IXMLDOMNodePtr pNode_Level = list_level->Getitem(p);
			if(pNode_Level==NULL)
				continue;

			int prio=0;
			pChildNode = pNode_Level->selectSingleNode("@prio");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				prio = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			CString text;
			pChildNode = pNode_Level->selectSingleNode("@text");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				text = v.bstrVal;	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			int nColor=0;
			pChildNode = pNode_Level->selectSingleNode("@color");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();

				CString rgb(v.bstrVal);
				int R,G,B;
				sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
				nColor = RGB(R,G,B);	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			if(1==prio || 2==prio || 3==prio)
			{
				strncpy(m_stplanLeval[prio].text, text.GetBuffer(), 255);
				m_stplanLeval[prio].text[255]=0;
				m_stplanLeval[prio].clr=nColor;
			}
		}

		// config
		MSXML::IXMLDOMNodeListPtr list_warn = pRootNode->selectNodes( "config/warn");
		for(int m = 0; m < list_warn->Getlength(); m++)
		{
			MSXML::IXMLDOMNodePtr pNode_Warn = list_warn->Getitem(m);
			if(pNode_Warn==NULL)
				continue;
			
			int id=0, prio=0;
			CString example, type;
			pChildNode = pNode_Warn->selectSingleNode("@id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				id = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode_Warn->selectSingleNode("@prio");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				prio = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode_Warn->selectSingleNode("@type");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				type = v.bstrVal;	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode_Warn->selectSingleNode("@example");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				example = v.bstrVal;	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
						
			if(1<=prio && prio<=3 && id>0)
			{
				for(int n=1; n<=4; n++)
				{
					CString strPrio="";
					for(int i=0; i<m_stplanLeval[n].id_count; i++)
					{
						if(m_stplanLeval[n].id[i]==id && m_stplanLeval[n].tz_id[i]==0 && n!=prio)
						{
							m_stplanLeval[n].id[i]=0;
							strPrio = m_stplanLeval[n].text;
						}
					}
					if(strPrio!="")
					{
						for(int i=0; i<m_stplanLeval[prio].id_count; i++)
						{
							if(m_stplanLeval[prio].id[i]==id)
							{
								break;
							}
						}
						if(i>=m_stplanLeval[prio].id_count)
						{
							m_stplanLeval[prio].id[m_stplanLeval[prio].id_count]=id;  m_stplanLeval[prio].id_count++;
							GpcLogSysMessage(0, "用户修改%s-%d为%s", strPrio, id, m_stplanLeval[prio].text);
						}
					}
				}
			}
		}
	}

	for(int n=1; n<=3; n++)
	{
		CString includes, tmp;
		for(int i=0; i<m_stplanLeval[n].id_count; i++)
		{
			if(i==0)
				tmp.Format("%d", m_stplanLeval[n].id[i]);
			else
				tmp.Format(",%d", m_stplanLeval[n].id[i]);
			includes+=tmp;
		}
		GpcLogSysMessage(0, "[%s]=%s", m_stplanLeval[n].text, includes);
	}
	GpcLogSysMessage(0, "阶段计划报警配置END");
	return;
}

int CTGFunctionConfig::GetLineNo(int station, int nextstation)
{
	for(int i=0; i<default_entry_vet.size(); i++)
	{
		if(default_entry_vet[i].station==station && default_entry_vet[i].nextstation==nextstation)
		{
			return default_entry_vet[i].lineno;
		}
	}

	return -1;
}

bool CTGFunctionConfig::IsCheckNeibTrains(int section)
{
	bool bCheckNeibTrains=true;
	for(int n=0; n<m_NotCheckSection.size(); n++)
	{
		if(m_NotCheckSection[n]==section)
		{
			bCheckNeibTrains=false;
			break;
		}
	}

	return bCheckNeibTrains;
}

// 上海局新邻台计划获取邻台配置
void CTGFunctionConfig::GetNeibStructs(int this_section, int neib_section, std::vector<NeibPlanParam>& neibplan_param_vect)
{
	neibplan_param_vect.clear();
	vector<NeibPlanStruct> neibPlanConfig;
	GetNeibPlanConfig(neib_section, this_section, neibPlanConfig);

	for(size_t i=0;i<neibPlanConfig.size();i++)
	{
		NeibPlanStruct x=neibPlanConfig[i];
		if(x.dest_section==this_section && x.source_section==neib_section)
		{
			if (x.neib_station==0)
				continue;

			NeibPlanParam p;
			memset(&p, 0, sizeof(NeibPlanParam));

			p.first_station_index	=	x.node_index;
			p.first_station			=	x.first_station;		
			p.next_station			=	x.next_station;
			p.next_next_station		=	x.next_next_station;
			p.neib_station			=	abs(x.neib_station);
			p.consider_start_train  =   x.consider_start_train;
			for(size_t n=0; n<neibplan_param_vect.size(); n++)
			{
				if(neibplan_param_vect[n].first_station_index==p.first_station_index &&
					neibplan_param_vect[n].first_station==p.first_station &&
					neibplan_param_vect[n].next_station==p.next_station &&
					neibplan_param_vect[n].next_next_station==p.next_next_station &&
					neibplan_param_vect[n].neib_station==p.neib_station &&
					neibplan_param_vect[n].consider_start_train==p.consider_start_train)
				{
					break;
				}
			}
			if(n>=neibplan_param_vect.size())
			{
				neibplan_param_vect.push_back(p);
			}
		}
	}
}

#define GPC_SECTION_NODE     ("[SECTION_NODE]")
BOOL CTGFunctionConfig::LoadTgdef()
{
	char *ptr, szFileName[128], strLine[513];
	FILE *fp;
	char *token;
	char seps[]=" ,\t\\/=\n";
	sprintf(szFileName,"%s\\tg.def",gpcdef.GetDataDir());
	if((fp=fopen(szFileName, "r"))==NULL)
		return FALSE;
	if (fp == NULL)  // bug
		return FALSE;
	while (!feof(fp))
	{
		if(!(ptr = fgets(strLine, 512, fp)))
			break;

		//151,	1,	2,	海口,	                1488,	2/14881/14881,	        2/14881/14881,	        800,	0,	0,	0/2/2/2/2,	3
		if (*ptr == '[' && !strncmp(ptr, GPC_SECTION_NODE, strlen(GPC_SECTION_NODE)) )
		{//read route info 
			ptr = ptr + strlen(GPC_SECTION_NODE);
			
			// section no
			token = strtok(ptr, seps);
			if(token==NULL)
				break;
			int section=atoi(token);
			
			NodeDefine define;
			// sub_section
			token = strtok(NULL, seps);
			if(token==NULL) 
			{
				AfxMessageBox("tg.def配置错误");
				return FALSE;
			}

			token = strtok(NULL, seps);
			if(token==NULL) 
			{
				AfxMessageBox("tg.def配置错误");
				return FALSE;
			}
			define.index=atoi(token);

			//name
			token = strtok(NULL, seps);
			if(token==NULL) 
			{
				AfxMessageBox("tg.def配置错误");
				return FALSE;
			}

			//staton number
			token = strtok(NULL, seps);
			if(token==NULL) 
			{
				AfxMessageBox("tg.def配置错误");
				return FALSE;
			}
			define.station=atoi(token);
			define.entry_line_num=0;
			define.exit_line_num=0;

			// down line number
			token = strtok(NULL, seps);
			if(token==NULL) 
			{
				AfxMessageBox("tg.def配置错误");
				return FALSE;
			}
			int down_line_count =  (int)atoi(token);
			for(int i = 0; i < down_line_count; i++) 
			{
				token = strtok(NULL, seps);
				if(token!=NULL) 
				{
					int down_line_list = (int)atoi(token);
					define.exit_line[i]=down_line_list;
				}
			}
			define.exit_line_num=down_line_count;

			//up line number
			token = strtok(NULL, seps);
			if(token==NULL) 
			{
				AfxMessageBox("tg.def配置错误");
				fclose(fp);
				return FALSE;
			}
			int up_line_count =  (int)atoi(token);
			//up abs line list
			for(int i = 0; i < up_line_count; i++) 
			{
				token = strtok(NULL, seps);
				if(token!=NULL) 
				{
					int up_line_list = (int)atoi(token);
					define.entry_line[i]=up_line_list;
				}
			}
			define.entry_line_num=up_line_count;

			//
			map<int, SectionDefine>::iterator it = m_tg_define.find(section);
			if(it==m_tg_define.end())
			{
				SectionDefine sec;
				sec.con_id=section;
				sec.stdef_num=1;
				sec.stinfo[0]=define;
				m_tg_define[section]=sec;
			}
			else
			{
				int num=it->second.stdef_num;
				if(num<128)
				{
					it->second.stinfo[num]=define;
					it->second.stdef_num++;
				}
			}
		}
	}

	fclose(fp); // bug not close file

	return TRUE;
}

bool CTGFunctionConfig::VerifyStationWithIndex(int section, int station, int index)
{
	map<int, SectionDefine>::iterator it = m_tg_define.find(section);
	if(it!=m_tg_define.end())
	{
		for(int i=0; i<it->second.stdef_num; i++)
		{
			if(it->second.stinfo[i].index==index && it->second.stinfo[i].station==station)
				return true;
		}
		if(i>=it->second.stdef_num)
		{
			return false;
		}
	}
	return true;
}

bool CTGFunctionConfig::checkConfig()
{
	CStringArray list;
	for(size_t i=0;i<m_aNeibPlanConfig.size();i++)
	{
		NeibPlanStruct x=m_aNeibPlanConfig[i];
		if(!VerifyStationWithIndex(x.dest_section, x.first_station, x.node_index+1))
		{
			CString str;
			str.Format("[NEIBPLAN]=%d,%d,%d,%d,%d配置中配置的首站站序和首站在tg.def上站序不一致",
				x.source_section, x.dest_section,x.first_station,x.prev_station,x.next_station);
			list.Add(str);
		}
	}
	
	for(size_t i=0;i<m_aNeibPlanConfig.size();i++)
	{
		NeibPlanStruct x=m_aNeibPlanConfig[i];
		if(x.first_station==0)
		{
			CString str;
			str.Format("[NEIBPLAN]=%d,%d,%d,%d,%d配置中:首站为零",
				x.source_section, x.dest_section,x.first_station,x.prev_station,x.next_station);
			list.Add(str);
		}
	}

	for(size_t i=0;i<m_aNeibPlanConfig.size();i++)
	{
		NeibPlanStruct x=m_aNeibPlanConfig[i];
		if(x.next_station==0 && x.next_next_station!=0)
		{
			CString str;
			str.Format("[NEIBPLAN]=%d,%d,%d,%d,%d,%d配置中没有下一站,更是没有下下一站",
				x.source_section, x.dest_section,x.first_station,x.prev_station,x.next_station,x.next_next_station);
			list.Add(str);
		}
	}

	for(size_t i=0;i<m_aNeibPlanConfig.size();i++)
	{
		NeibPlanStruct x=m_aNeibPlanConfig[i];
		if(x.next_station==0 && x.next_next_station==0 && x.prev_station==0)
		{
			if(x.nWholeMatch==0)
			{
				CString str;
				str.Format("[NEIBPLAN]=%d,%d,%d,%d,%d,%d配置中没有前一站,下一站,下下一站,必须完全匹配",
					x.source_section, x.dest_section,x.first_station,x.prev_station,x.next_station,x.next_next_station);
				list.Add(str);
			}
		}
	}

	for(size_t i=0;i<m_aNeibPlanConfig.size();i++)
	{
		NeibPlanStruct x=m_aNeibPlanConfig[i];
		int neib_station=abs(x.neib_station);
		if(x.prev_station==neib_station)
		{
			CString str;
			str.Format("[NEIBPLAN]=%d,%d,%d,%d,%d,%d配置,台间站不能是前一站",
				x.source_section, x.dest_section,x.first_station,x.prev_station,x.next_station,x.next_next_station);
			list.Add(str);
		}
	}

	for(size_t i=0;i<m_aNeibPlanConfig.size();i++)
	{
		NeibPlanStruct x=m_aNeibPlanConfig[i];
		int neib_station=abs(x.neib_station);
		if(x.first_station==neib_station && x.next_station==neib_station && x.next_next_station==neib_station)
		{
			CString str;
			str.Format("[NEIBPLAN]=%d,%d,%d,%d,%d,%d配置,台间站既不是首站,也不是下一站，也不是下下一站",
				x.source_section, x.dest_section,x.first_station,x.prev_station,x.next_station,x.next_next_station);
			list.Add(str);
		}
	}

	
	GpcLogTrainMessage(0, 0, "[USE_TDMS_LINE_STATION]");
	std::vector<USE_TDMS_LINE_STATION>::iterator it = m_UserTDMSLineStation.begin();
	for (;it!=m_UserTDMSLineStation.end();it++)
	{
		CString str;
		str.Format("序号=%d,%d,%d,%d,%d",
					it->con_id, it->station_id1, it->station_id2, 
					it->station_id1_nd, it->station_id2_nd);
		GpcLogTrainMessage(0, 0, "%s", str);
	}
	it = m_UserTDMSLineStation.begin();
	for (;it!=m_UserTDMSLineStation.end();it++)
	{
		if(it->station_id1_nd!=255)
		{
			if(!VerifyStationWithIndex(it->con_id, it->station_id1, it->station_id1_nd+1))
			{
				CString str;
				str.Format("[USE_TDMS_LINE_STATION]=%d,%d,%d,%d,%d中车站%d配置站序和它在tg.def上站序不一致",
					it->con_id, it->station_id1, it->station_id2, 
					it->station_id1_nd, it->station_id2_nd,it->station_id1);
				list.Add(str);
			}
		}

		if(it->station_id2_nd!=255)
		{
			if(!VerifyStationWithIndex(it->con_id, it->station_id2, it->station_id2_nd+1))
			{
				CString str;
				str.Format("[USE_TDMS_LINE_STATION]=%d,%d,%d,%d,%d中%d配置站序和它在tg.def上站序不一致",
					it->con_id, it->station_id1, it->station_id2, 
					it->station_id1_nd, it->station_id2_nd, it->station_id2);
				list.Add(str);
			}
		}
	}
	
	if(list.GetCount()>0)
	{
		CDlgConfirm dlg("配置报警提示", list);
		dlg.DoModal();
	}
	return true;
}

int  CTGFunctionConfig::GetThisSectionStation(int con_id, int station_list[128], int total)
{
	int count=0;
	map<int, SectionDefine>::iterator it = m_tg_define.find(con_id);
	if(it!=m_tg_define.end())
	{
		for(int m=0; m<it->second.stdef_num; m++)
		{
			int station=it->second.stinfo[m].station;

			int n=0;
			for(n=0; n<count; n++)
			{
				if(station_list[n]==station)
					break;
			}
			if(n>=count && count<total)
			{
				station_list[count]=station;
				count++;
			}
		}
	}
	return count;
}

bool CTGFunctionConfig::LoadCompareStationList(int section, CheckStationsOfSection& compare_stations)
{
	compare_stations.Init();
	CString strFileName;
	strFileName.Format("%s\\user_cfg\\CompareStationList-%d.xml", gpcdef.GetDataDir(), section);
	
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( strFileName ) ) )
	{
		return false; 
	}

	MSXML::IXMLDOMNodePtr pRoot = pXMLDoc->selectSingleNode( "root");
	if(pRoot == NULL)
	{
		return false;
	}

	int count=0;
	_variant_t v;
	MSXML::IXMLDOMNodePtr pNode, pChild;
	MSXML::IXMLDOMNodeListPtr pStationList = pRoot->selectNodes("station");
	for(int n = 0; n <pStationList->Getlength(); n++)
	{
		int station=0;
		pChild = pStationList->Getitem(n)->selectSingleNode( "@no" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			CString ss=v.bstrVal;
			station=atoi(ss);
			
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		int bcheck=0;
		pChild = pStationList->Getitem(n)->selectSingleNode( "@basic" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			CString ss=v.bstrVal;
			int tmp=atoi(ss);
			if(tmp==0)
				bcheck=0;
			else
				bcheck=1;

			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		int scheck=0;
		pChild = pStationList->Getitem(n)->selectSingleNode( "@shift" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			CString ss=v.bstrVal;
			int tmp=atoi(ss);
			if(tmp==0)
				scheck=0;
			else
				scheck=1;

			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		int wcheck=0;
		pChild = pStationList->Getitem(n)->selectSingleNode( "@work" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			CString ss=v.bstrVal;
			int tmp=atoi(ss);
			if(tmp==0)
				wcheck=0;
			else
				wcheck=1;

			SysFreeString(v.bstrVal);
			v.Clear();
		}
		StationCheckNode node;
		node.station=station;
		node.setbcheck=bcheck;
		node.setscheck=scheck;
		node.setwcheck=wcheck;

		compare_stations.station_list[count]=node;
		count++;
	}
	compare_stations.count=count;

	return true;
}

// 前后站关系邻台计划,将简单条件放在复杂条件后面
void CTGFunctionConfig::GetNeibPlanConfig(int neib_section,int this_section, vector<NeibPlanStruct>& neibPlanConfig)
{
	neibPlanConfig.clear();
	
	vector<NeibPlanStruct> neibPlanConfig2;
	for(int i=0;i<m_aNeibPlanConfig.size();i++)
	{
		NeibPlanStruct x=m_aNeibPlanConfig[i];
		if(x.source_section!=neib_section || x.dest_section!=this_section)
			continue;

		// 接收单点车配置
		if(x.prev_station==0 && x.next_station==0)
		{
			neibPlanConfig2.push_back(x);
		}
		else
		{
			neibPlanConfig.push_back(x);
		}
	}
	for(int i=0;i<neibPlanConfig2.size();i++)
	{
		neibPlanConfig.push_back(neibPlanConfig2[i]);
	}
}

bool CTGFunctionConfig::GetSectionConfig(int section, SectionDefine& node_section)
{
	map<int, SectionDefine>::iterator it = m_tg_define.find(section);
	if(it != m_tg_define.end())
	{
		node_section = it->second;
		return true;
	}

	return false;
}

bool CTGFunctionConfig::LoadStplanNoWarningStation(int section, StationStplanNotWarning& stplan_cfg)
{
	stplan_cfg.Init();
	CString strFileName;
	strFileName.Format("%s\\user_cfg\\NoWarningStplanStationList-%d.xml", gpcdef.GetDataDir(), section);
	
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( strFileName ) ) )
	{
		return false; 
	}

	MSXML::IXMLDOMNodePtr pRoot = pXMLDoc->selectSingleNode( "root");
	if(pRoot == NULL)
	{
		return false;
	}

	int count=0;
	_variant_t v;
	MSXML::IXMLDOMNodePtr pNode, pChild;
	MSXML::IXMLDOMNodeListPtr pStationList = pRoot->selectNodes("station");
	for(int n = 0; n <pStationList->Getlength(); n++)
	{
		pNode = pStationList->Getitem(n);
		if(pNode==NULL)
			continue;

		int station=0, sel=0;
		pChild = pNode->selectSingleNode( "@no" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			CString ss=v.bstrVal;
			station=atoi(ss);
			
			SysFreeString(v.bstrVal);
			v.Clear();
		}

		pChild = pNode->selectSingleNode( "@sel" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			CString ss=v.bstrVal;
			sel=atoi(ss);
			
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		if(sel==1 && station>0)
		{
			stplan_cfg.station_lists[count]=station;
			count++;
		}
	}
	stplan_cfg.station_count=count;

	return true;
}

void CTGFunctionConfig::LoadCalSideTrainType(int section)
{
	mCalSideTrainType.Init();

	CString strFileName;
	strFileName.Format("%s\\user_cfg\\CalSideTrainList-%d.xml", gpcdef.GetDataDir(), section);
	
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		return;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( strFileName ) ) )
	{
		return; 
	}

	MSXML::IXMLDOMNodePtr pRoot = pXMLDoc->selectSingleNode( "root");
	if(pRoot == NULL)
	{
		return;
	}

	int count=0;
	_variant_t v;
	MSXML::IXMLDOMNodePtr pNode, pChild;
	MSXML::IXMLDOMNodeListPtr pTrainList = pRoot->selectNodes("TRAIN");
	for(int n = 0; n <pTrainList->Getlength(); n++)
	{
		int train_type=-1;
		pNode = pTrainList->Getitem(n);
		if(pNode==NULL)
			continue;

		pChild = pNode->selectSingleNode( "@train_type" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			CString ss=v.bstrVal;
			train_type=atoi(ss);
			
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		if(train_type != -1)
		{
			mCalSideTrainType.nTrainType[count] = train_type;
			count++;
		}
	}
	mCalSideTrainType.nCount=count;

	return;
}