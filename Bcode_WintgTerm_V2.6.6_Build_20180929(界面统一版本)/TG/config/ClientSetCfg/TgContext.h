#pragma once
#include <vector>
#include "afxtempl.h"
using namespace std;

class CTgContext
{
public:
	~CTgContext(void);
	CTgContext();
	
public:
	bool  LoadTGNoXML(CString pathname);

public:
	void  SetTrainFuncCtrlWord(DWORD word) { mTrainFuncCtrlWord = word;}
	DWORD GetTrainFuncCtrlWord(){ return mTrainFuncCtrlWord;}

	void  SetMarkFuncCtrlWord(DWORD word) { mMarkFuncCtrlWord = word;}
	DWORD GetMarkFuncCtrlWord(){ return mMarkFuncCtrlWord;}

	DWORD GetAppFuncCtrlWord(){ return mAppCtrlWord;}

public:
	// ---------------------卡控----------------------------
	bool IsCtrlModifyTrainStation(void) { return (m_lctrlword&0x00000001)? true:false;}

	// -------------------App Ctrl----------------------
	bool IsDrawStopSidePoint(void)   {	        return (mAppCtrlWord&0x00000001) ? true : false;}
	// 上海局要密码,其它局不要密码
	bool IsTechstopModifyNeedPwd(void)   {	    return (mAppCtrlWord&0x00000002) ? true : false;}
	// 上海局不需要弹出框,其它局需要
	bool IsEndDrawLineDlgNeed(void)   {	        return (mAppCtrlWord&0x00000004)  ? true : false;}
	// 是否分散显示多车次号 上海局否,其它局是
	bool IsShowMutiTrainSeparate(void)   {	    return (mAppCtrlWord&0x00000008) ? true : false;}

	// 是否集中显示多车次号于接入始发处 所有局均否
	bool IsShowMutiTrainCenter(void)   {	        return (mAppCtrlWord&0x00000010) ? true : false;}
	// 绘制运行线结束，单击右键生成运行线,弹出结束对话框 上海局不需要, 其它要
	bool IsRButtonGenTg(void)   {					return (mAppCtrlWord&0x00000020) ? true : false;}
	// 济南局默认非技术停点，其它局是默认技术停点
	bool IsSetTechStop(void){						return (mAppCtrlWord&0x00000040) ? true : false;}
	// 济南需动车检查,其它不需要.修改股道是否有动车组股道检查
	bool IsCheckCrhSide(void){						return (mAppCtrlWord&0x00000080) ? true : false;}

	// 有营业车站通过变到开,南昌局需要密码
	bool IsNoTechPassToStayNeedPwd(void){	        return (mAppCtrlWord&0x00000100) ? true : false;}
	// 有营业车站到开变通过,上海局需要密码
	bool IsNoTechStayToPassNeedPwd(void){	        return (mAppCtrlWord&0x00000200) ? true : false;}
	// 济南局有提示框，进路一排出车或客车到开变通过是否需要提示框
	bool IsStayToPassNeedInfoWnd(void){	    return (mAppCtrlWord&0x00000400) ? true : false;}
	// 济南局有提示框，进路一排出车或客车通过变到开是否需要提示框
	bool IsPassToStayNeedInfoWnd(void){	    return (mAppCtrlWord&0x00000800) ? true : false;}

	// 西陇海特殊处理,西陇海必须设置它
	bool IsXLHSet(void){	                return (mAppCtrlWord&0x00001000) ? true : false;}
	// 选中运行线双击左键弹出进路设置对话框
	bool IsLBtnDblClkRouteSet() {	        return (mAppCtrlWord&0x00002000) ? true : false;}
	// 是否绘制进路准备好标志
	bool IsDrawRouteOkMark() {	            return (mAppCtrlWord&0x00004000) ? true : false;}
	// 是否显示反向行车车次
	bool IsShowReverseTrainId(){	        return (mAppCtrlWord&0x00008000) ? true : false;}
	// 统计早晚点圆圈显示位置 置位后显示在锐角 否则钝角
	bool IsEarlyLateTimeDrawRuijiao(){	    return (mAppCtrlWord&0x00010000) ? true : false;}
	// 是否允许多车选中
	bool IsCanSelMutiTrain(void){	        return (mAppCtrlWord&0x00020000) ? true : false;}
	// 是否电力机车显示电字
	bool IsElecEngineShowDianzi(void){	        return (mAppCtrlWord&0x00040000) ? true : false;}
	// 客车图定到开、实际通过是否统计发点
	bool IsTrainStayRealPassStatisticDepart(void){	        return (mAppCtrlWord&0x00080000) ? true : false;}
	// 选中列车早晚点圈是否加粗
	bool IsSelTrainBold(void){	        return (mAppCtrlWord&0x00100000) ? true : false;}
	// 根据上下行 区分技术停点三角符号位置
	bool IsTechStopFlagNotSame(void){	        return (mAppCtrlWord&0x00200000) ? true : false;}
	// 哈尔滨,列车晚点5分钟（含5分钟）以内，应显示红圈蓝字，5分钟以外显示为蓝圈蓝字
	bool IsHrbStaticEarlyLateTimeColor(void){	        return (mAppCtrlWord&0x00400000) ? true : false;}
	// 哈尔滨,运输信息在界面上显示为深蓝色
	bool IsHrbTransportColor(void){	        return (mAppCtrlWord&0x00800000) ? true : false;}
	// 哈尔滨,早晚点统计使用粗体
	bool IsHrbEarlyLateTimeBold(void){	        return (mAppCtrlWord&0x01000000) ? true : false;}
	// 自动调整
	bool IsEnableAutoAdjust(void){	        return (mAppCtrlWord&0x02000000) ? true : false;}
	// 动车、重点车股道修改卡控
	bool IsCheckCrhAndImpTrackChange(void){	        return (mAppCtrlWord&0x04000000) ? true : false;}
	
	// --------------------APP Ctrl Ex-----------------------
	bool IsInputAbsStopInfo(){	    return (mAppCtrlWordEx&0x00000001) ? true : false;}
	bool IsSetNoLateEarlyTime(){	    return (mAppCtrlWordEx&0x00000002) ? true : false;}
	bool IsUpAndDownTimeView(){	    return (mAppCtrlWordEx&0x00000004) ? true : false;}
	bool IsLimitNxtNodeBackPreNode(){	    return (mAppCtrlWordEx&0x00000008) ? true : false;}

    bool IsDrawLanzhouAbsInhibitMark(){	    return (mAppCtrlWordEx&0x00000010) ? true : false;}
	bool IsDrawLanzhouAbsSlowMark(){	    return (mAppCtrlWordEx&0x00000020) ? true : false;}
	bool IsGenLanzhouAbsEleFixMark(){	    return (mAppCtrlWordEx&0x00000040) ? true : false;}
	bool IsDrawLanzhouEngine(){	    return (mAppCtrlWordEx&0x00000080) ? true : false;}

	bool IsDrawSideNamePerFourHours(){	    return (mAppCtrlWordEx&0x00000100) ? true : false;}
	bool IsEnableTaiyuanCopyTrain(){	    return (mAppCtrlWordEx&0x00000200) ? true : false;}
	bool IsEnableShuaiguaManage(){	    return (mAppCtrlWordEx&0x00000400) ? true : false;}
	bool IsEnableModifySideByKeyboard(){	    return (mAppCtrlWordEx&0x00000800) ? false : true;}

	bool IsEnableInhibitDrawBlackArc(){	    return (mAppCtrlWordEx&0x00001000) ? true : false;}
	bool IsEnableNoDrawLineInNoSameSubTable(){	    return (mAppCtrlWordEx&0x00002000) ? true : false;}
	bool IsEnableReverseShowSelTrainRect(void){	    return (mAppCtrlWordEx&0x00004000) ? true : false;}
	// 同一分段内相同车站间是否画连线
	bool IsEnableLeapLineInSameSubTable(){	    return (mAppCtrlWordEx&0x00008000) ? true : false;}
	// 太原局到开变通过需要密码
	bool IsStayToPassNeedPwd(){	        return (mAppCtrlWordEx&0x00010000) ? true : false;}
	// 南宁局, 鼠标显示分界口正晚点
	bool IsShowStaticTimeAtMouse(){	    return (mAppCtrlWordEx&0x00020000) ? true : false;}
	// 南宁局, 正晚点统计完全按照配置文件
	bool IsStaticsTimeCompleteByTGDEF(){	    return (mAppCtrlWordEx&0x00040000) ? true : false;}
	// 广州局, 设置进路自触
	bool IsCanDirectSetRouteTriger(){	    return (mAppCtrlWordEx&0x00080000) ? true : false;}
	// 广州局, 显示、隐藏进路自触标志
	bool IsShowRouteStatusFlag(){	    return (mAppCtrlWordEx&0x00100000) ? true : false;}
	// 北京局, 显示基本图运行线闪烁
	bool IsFlashBasicTrain(){	    return (mAppCtrlWordEx&0x00200000) ? true : false;}
	// 广州局, 最小停车时间和技术停点联动
	bool IsEnableMinTimeTechStop(){	    return (mAppCtrlWordEx&0x00400000) ? true : false;}
	// 北京局, 折返车次显示后续车次
	bool IsShowZheFanTrainID(){	    return (mAppCtrlWordEx&0x00800000) ? true : false;}	
	//	广州局, 修改列车股道弹出调度命令窗口
	bool IsShowWarningDlgByChangeSide(){ return (mAppCtrlWordEx&0x01000000) ? true : false; }
	//  修改列车股道弹出的调度命令窗口直接生成调度命令
	bool IsGenDCMDByChangeSide(){  return (mAppCtrlWordEx&0x02000000) ? true : false; }
	//  济南局, 绘制跨区段的区间作业列车
	bool IsEnableDrawAbsWorkTrain(){  return (mAppCtrlWordEx&0x04000000) ? true : false; }
	//  广州局, 计划线参数窗口检查出入口联通性报警提示
	bool IsParaDlgCheckEntryConnect(){  return (mAppCtrlWordEx&0x08000000) ? true : false; }
	//  上海局, 车站页面扩大显示范围，防止站名过长
	bool IsWideStationView(){  return (mAppCtrlWordEx&0x10000000) ? true : false; }
	//  上海局, 不显示乘务悬浮窗
	bool IsNotShowTipText(void){	    return (mAppCtrlWordEx&0x20000000) ? true : false;}

	// --------------------Mark Ctrl-------------------
	bool IsCanCopyMark(void)   {	        return (mMarkFuncCtrlWord&0x00000001) ? true : false;}
	bool IsCanDeleteMark(void)   {	        return (mMarkFuncCtrlWord&0x00000002) ? true : false;}
	bool IsCanGenMarkDCmd(void)   {	        return (mMarkFuncCtrlWord&0x00000004) ? true : false;}
    // 太原局 直接修改起止时间
	bool IsDirectModifyBlockTime(void)   {	return (mMarkFuncCtrlWord&0x00000008) ? true : false;}
	// 沈阳局 增加施工符号的进行状态的操作
	bool IsCanModifyBlockStatus(void)	{	return (mMarkFuncCtrlWord&0x00000010) ? true : false;}
	// 沈阳局 增加施工符号自动按照上下行区分显示位置
	bool IsEnableDiffShowBlockPostion(void)	{	return (mMarkFuncCtrlWord&0x00000020) ? true : false;}
	
	// ---------------------TG Ctrl---------------------
	bool IsCanCopyTrain(void)   {	        return (mTrainFuncCtrlWord&0x00000001)  ? true : false;}
	bool IsCanRenameTrain(void)   {	        return (mTrainFuncCtrlWord&0x00000002) ? true : false;}
	bool IsCanAdjustTrain(void)   {	        return (mTrainFuncCtrlWord&0x00000004) ? true : false;}
	bool IsCanSetNebSectionRefuseTrain(void)   {	        return (mTrainFuncCtrlWord&0x00000008) ? true : false;}
	bool IsCanModifyTrainInOutMark(void)   {	        return (mTrainFuncCtrlWord&0x00000010) ? true : false;}
	bool IsCanAddTransportInfo(void)   {	        return (mTrainFuncCtrlWord&0x00000020) ? true : false;}
	bool IsCanCalcEarlylateTime(void)   {	        return (mTrainFuncCtrlWord&0x00000040) ? true : false;} // no use
	bool IsCanStopTrainAbs(void)   {	        return (mTrainFuncCtrlWord&0x00000080) ? true : false;}
	bool IsCanModifyRoute(void)   {	        return (mTrainFuncCtrlWord&0x00000100) ? true : false;}
	bool IsCanModifySide(void)   {	        return (mTrainFuncCtrlWord&0x00000200) ? true : false;}
	bool IsCanModifyZFTrainId(void)   {	        return (mTrainFuncCtrlWord&0x00000400) ? true : false;}
	bool IsCanHatTrain(void)   {	        return (mTrainFuncCtrlWord&0x00000800) ? true : false;}
	bool IsCanEngineTrain(void)   {	        return (mTrainFuncCtrlWord&0x00001000) ? true : false;}
	bool IsCanModifyDepartSJ(void)   {	        return (mTrainFuncCtrlWord&0x00002000) ? true : false;}
	bool IsCanDeleteTrain(void)   {	        return (mTrainFuncCtrlWord&0x00004000) ? true : false;}
	bool IsCanPASSToStayCnvt(void)   {	        return (mTrainFuncCtrlWord&0x00008000) ? true : false;}
	bool IsCanTechStop(void)   {	        return (mTrainFuncCtrlWord&0x00010000) ? true : false;}
    bool IsCannotMeetTrain(void){	        return (mTrainFuncCtrlWord&0x00020000) ? true : false;}
	bool IsCanSetTrigeRouteFlag(void){	    return (mTrainFuncCtrlWord&0x00040000) ? true : false;}
	bool IsCanSetFjkLateTime(void){	    return (mTrainFuncCtrlWord&0x00080000) ? true : false;}
	bool IsCanSetPressure(void){	    return (mTrainFuncCtrlWord&0x00100000) ? true : false;} // no use
	bool IsCanMutiTrainRecStayToPass(void){	    return (mTrainFuncCtrlWord&0x00200000) ? true : false;}
	bool IsCanBianzu(void){	    return (mTrainFuncCtrlWord&0x00400000) ? true : false;} // no use
	bool IsCanAutoAjust(void){	    return (mTrainFuncCtrlWord&0x00800000) ? true : false;}
	
		
	// 调试开关
	bool IsCanModifyStationIdx(void) { return (mDebugCtrlWord&0x00000001) ? true : false;}
public:
	//wintg_operate
	bool IsSecretTrainNotVisible(void) { return (m_wintg_oper_right&0x8) ?  true : false;}

public:
	void SetLimitModifyTimeLen(int b){ m_nLimitTimeLen = b;}
	int  GetLimitModifyTimeLen(void) {return m_nLimitTimeLen;}
	void SetLimitModifyActTimeFlag(int limit)
	{ 
		if(limit>0)
			m_bLimitModifyActTime = true;
		else
			m_bLimitModifyActTime = false;
	}
	bool IsLimitModifyTime() {return m_bLimitModifyActTime;}
	
	void SetShiftSchdCanModify(int b){ m_nModifyShiftSchd = b;}
	int  GetShiftSchdCanModify(void) {return m_nModifyShiftSchd;}

	void SetBasicSchdCanModify(int b){ m_nModifyBasicSchd = b;}
	int  GetBasicSchdCanModify(void) {return m_nModifyBasicSchd;}

	void SetOperateRight(BYTE right){ m_oper_right = right;}
	BYTE GetOperateRight()
	{ 
		if(m_nModifyBasicSchd>0)
			return m_oper_right;
		else
			return 0;
	}
	void SetWinTGOperateRight(DWORD right){ m_wintg_oper_right = right;}
	
	void SetLeapLineFlag(BYTE flag){m_nLeapLineFlag = flag;}
	BYTE GetLeapLineFlag(){return m_nLeapLineFlag;}

	void SetDrawTenicalStopMark(int drawflag){ m_nDrawTenicalStop = drawflag;}
	int  GetDrawTenicalStopMark(){return m_nDrawTenicalStop;}

	void SetDrawRouteOkFlag(int flag){ m_nRouteOkFlag = flag;}
	int  GetDrawRouteOkFlag(){return m_nRouteOkFlag;}
	void GetDrawRouteOkJFColor(COLORREF& jcolor, COLORREF& fcolor) {jcolor=m_jcolor; fcolor=m_fcolor;}
	CString GetDataPath(){return m_datapath;}
	void	SetDataPath(LPCSTR path){ m_datapath = path;}
	
	void SetDrawCurrTimeBar(bool draw){m_bDrawTimebar = draw;}
	bool IsDrawCurrTimeBar(){return m_bDrawTimebar;}

	void SetBackgroudBlock(USHORT dlt) {backgroudblock = dlt;}
	USHORT GetBackgroudBlock() {return backgroudblock;}

	void SetKaCtrlFlag(DWORD flag){ m_lctrlword = flag;}

	bool IsCanGenNodeIndex() { return m_genUpdateNodeIndex;}

	void SetLateEarlyTime(int nFlag) {m_nLateEarlyTime = nFlag;}
	int  GetLateEarlyTime() {return m_nLateEarlyTime;}

	void SetLineWidth(int nFlag) {m_nLineWidth = nFlag;}
	int  GetLineWidth() {return m_nLineWidth;}
	
	void SetShowLocoNo() {m_bShowTractionEngine = !m_bShowTractionEngine;}
	bool GetShowLocoNo() { return m_bShowTractionEngine; }

	void SetShowTrainType(int flag) {m_nShowTrainFlag = flag;}
	int  GetShowTrainType() { return m_nShowTrainFlag; }

	void SetShowTrainDirect(int flag) {m_nShowDirectFlag = flag;}
	int  GetShowTrainDirect() { return m_nShowDirectFlag; }

	DWORD GetColorManualReal() {return m_nColorManualReal;}

	bool IsRouteOkInhibitModifySchd() { return (m_nRouteOkInhibitModifySchd>0)?true:false;}
	bool IsMoveAfterTime() { return (m_nMoveAfterTm>0)? true:false;}
	bool IsShowSideEntryNoPassFlag(COLORREF& clr);

	void SetEnableLockSide(int enable);
	bool IsEnableLockSide() { return m_bEnableLockSide;}

	void SetElecDrawGreenDecorate(int enable);
	bool IsElecDrawGreenDecorate() { return m_bElecDrawGreenDecorate;}
		
	void EnableCZH(int enable);
	bool IsEnableCZH() { return m_bEnableCZH;}

	void EnableNewCZH(int enable);
	bool IsEnableNewCZH() { return m_bEnableNewCZH;}

	void EnableSetCZHReturnTrainID(int enable);
	bool IsSetCZHReturnTrainID() { return m_bSetReturnTrainID;}
	
	void EnableInterCityStplan(int enable);
	bool IsEnableInterCityStplan() { return mSupportInterCityStPlan;}

	void SetMoveSameCZTrain(bool b);
	bool IsMoveSameCZTrain();

	bool IsSwtichStation(int station);
	void AddSwtichStation(int station);

	bool IsNotShowSideCtcStation(int station);
	void AddNotShowSideCtcStation(int station);

	// 有多个符合接入条件的入口,是否弹出选择对话框
	bool IsShowAcceptChioceDlg(){	return m_bIsShowAcceptChioceDlg;}

	bool IsAutoAddBlockEndTime(int type);
	int  GetAutoAddBlockEndTime();

	void LockSideNotTechStop(int b);
	bool IsLockSideNotTechStop() {return m_nLockSideNotTechStop;}	
	bool IsOnlyShowSelLine() {return m_bOnlyShowSelLine;}

	void SetProhibitDeleteRealTimeFlag(short b);
	bool IsProhibitDeleteRealTime() {return m_bProhibitDeleteRealTime;}

	int  GetADTimeStytle(){return m_nADStytle;}

	bool IsErrorSideStation(int station);
	void AddErrorSideStation(int station);
private:
	DWORD mAppWordTable[65];
	DWORD mAppExWordTable[65];
private:
	int nAutoAddBlockEndTime;
	int nAutoAddBlockTypeCount;
	int AutoAddBlockType[3];
	// 有多个符合接入条件的入口,是否弹出选择对话框
	bool m_bIsShowAcceptChioceDlg;
	// 卡控标志
	ULONG m_lctrlword;
	// 操作权限
	BYTE  m_oper_right;
	// 特殊操作权限
	DWORD  m_wintg_oper_right;
	// 基本图修改开关
	int  m_nModifyBasicSchd;
    // 日班计划修改
	int  m_nModifyShiftSchd;
	// 跳跃车站绘制标志 1: 无标记 2: 接入交出 3: 连线
	BYTE m_nLeapLineFlag;
	// 是否绘制技术停点标志 --- 0: 不绘制技术停点标志 1: 技术停点 绘制▽ 2: 非技术停点 绘制▽
	int m_nDrawTenicalStop;
	// 进路准备好标志----0: 不绘制 1: 紫色线 2: 圆圈 3:接黑 发绿
	int m_nRouteOkFlag;

	// 0:十分格只显示分%10 1:十分格显示秒 2:小时格
	int m_nADStytle; 
	
	COLORREF m_jcolor;
	COLORREF m_fcolor;

	// 早晚点统计
	int m_nLateEarlyTime;
	// 粗线
	int m_nLineWidth;
	//是否显示机车号
	bool m_bShowTractionEngine; 	
	
	// 显示列车类型 1: 客车 2: 货车 3: 客货车
	int m_nShowTrainFlag;
	// 显示列车方向 1: 上行 2: 下行 3: 上下行
	int m_nShowDirectFlag;

	// 数据路径
	CString m_datapath;
	// 运行图类型
	BYTE m_tgtype;


	// 背景块数
	USHORT  backgroudblock;
    // 是否绘制时间线
	bool m_bDrawTimebar;
	

	// 运行线功能字
	DWORD mTrainFuncCtrlWord;   
	// 图标功能字
	DWORD mMarkFuncCtrlWord; 
	// 应用功能字
	DWORD mAppCtrlWord;  
	// 调试开关
	DWORD mDebugCtrlWord;
	// 节点生成
	bool m_genUpdateNodeIndex;
	// 兰州扩充功能
	DWORD mAppCtrlWordEx;

	// 人工报点颜色显示不同
	DWORD m_nColorManualReal;

	// 是否启用股道锁定
	bool m_bEnableLockSide;
	// true: 电力车画绿线  false:内燃车画绿线 
	bool m_bElecDrawGreenDecorate;
	
	// 车组管理
	bool m_bEnableCZH;
	bool m_bEnableNewCZH;
	bool m_bSetReturnTrainID;
	// 是否平移同车组后续列车
	bool m_bIsMoveSameCZTrain;
	// 同车组连接线 1:细线 2:粗线
	int m_nCZConnectLineSize;
	COLORREF m_clrCZConnectLine;

	// 实际点可修改时间范围
	int  m_nLimitTimeLen;
	// 是否限制修改实际点,只有在限制修改实际点时,
	bool m_bLimitModifyActTime;

	// 旅客列车的非技术停点默认锁定股道
	bool m_nLockSideNotTechStop;
	// 是否只显示选中车站线号
	bool m_bOnlyShowSelLine;
	// 封闭运行图时，禁止删除实际点 0：不禁止 1：禁止
	bool m_bProhibitDeleteRealTime;
	// 20150623是否支持珠三角城际计划要求
	bool mSupportInterCityStPlan; // true:珠三角 false:通用路局计划
	
public:
	//  广州局,每小时显示股道名字
	bool m_bDrawSideNamePerHours;
	//  广州局,股道展开,始终显示股道
	bool m_bDefaultShowSide;
	//  广州局,同交路其他列车是否加粗显示
	bool m_bRelationTrainBold;
	//  广州局,修改列车交路是否以重点车形式醒目显示
	bool m_bModifyRelationSetTrainImport;
	// 站内施工符号显示配置
	int m_nStationBlockShowMode; // 0: 车站展开与否始终显示 1:只要车站展开时才显示
	// 进路触发标志
	int m_nRouteTrigerMode; // 0: 不显示进路触发标志 1: 显示进路触发标志
	// 进路完成触发,是否可以修改计划
	int m_nRouteOkInhibitModifySchd;
	// 进路触发完成修改计划点是否平移后续点
	int m_nMoveAfterTm;
	// 显示股道出入口不同标志--股道颜色不同
	int m_nShowSideEntryNoPass;
	// 鼠标挪动运行线及显示方式: 0:整分挪线,不显示秒级 1:默认值,秒级挪线,整分显示 2:秒级挪线,秒级显示
	int m_nMouseMoveLineFlag;
	// 未知列车是否显示"未知列车"字样 0:不显示 1:默认值,显示
	int m_nDisplayUnknownText;
	// 是否优化列车矩形
	bool m_bVerifyTrainRect;
	// 上下扩大范围配置
	int m_nZoomRangeX;
	int m_nZoomRangeY;
	COLORREF m_clrNoPassSide;

	std::vector<int> swtichStation;
	std::vector<int> notshowsidectcstation;
	std::vector<int> errorSideStation;

private:
	int m_nCurrentPixelPerMinute;
	
public:
	void SetPixelPerMinute(int nPixelPerMinute);
	int GetPixelPerMinute(void);
	int IncreateCurrentUnit(short count);
	int DecreateCurrentUnit(short count);

	void AddCrhStopStation(int station);
	bool IsCrhStopStation(int station);

	void GetCZHLineCfg(int &width, COLORREF& clr) { width=m_nCZConnectLineSize; clr=m_clrCZConnectLine;}
	int GetMouseMoveLineFlag() { return m_nMouseMoveLineFlag;}
	int IsDisplayUnknownText() { return m_nDisplayUnknownText;}
	bool IsVerifyTrainRect() { return m_bVerifyTrainRect;}
	int GetZoomRangeX() { return m_nZoomRangeX;}
	int GetZoomRangeY() { return m_nZoomRangeY;}
private:
	std::vector<int> mCrhStopStation; // 动车段配置

};
