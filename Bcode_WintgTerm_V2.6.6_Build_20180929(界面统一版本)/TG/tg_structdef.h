#ifndef _DEF_SCHEDULE_H
#define _DEF_SCHEDULE_H

/*  this file contains schedule related data struct  */
typedef short SIDE_ID;

typedef DWORD  TRAIN_INDEX;
typedef DWORD  TRAIN_KEY;
typedef BYTE CONID;
typedef char TRAIN_NO[10];

typedef long		TIME;				// 时间
typedef	DWORD		ENTITY_ID;			// 设备实体号
typedef WORD		STATION_NO;			// 站号

typedef short       ENTRY_ID;

#define MAX_STA_NAME_LEN 32

#define GetAnotherShiftId(shift_id)           (3 - shift_id)

// 列车折返位置 站内 站后
//bit1:0 通用版本, bit2无意义
//bit1:1 珠三角城际折返,bit2有意义,并且bit为1是站后折返 bit2为0是原地折返
#define IsNewReturn(side_utility)		   (((side_utility)&0x2) == 0x2)	
#define IsStationOutReturn(side_utility)    (((side_utility)&0x6) == 0x6)	//bit2
#define SetStationOutReturn(side_utility)   ((side_utility) &= ~0x6, (side_utility) |= 0x6)	//bit2  站后折返
#define SetStationReturn(side_utility)   ((side_utility) &= ~0x6, (side_utility) |= 0x2)	//bit2  原地折返
#define NotStationReturn(side_utility)   ((side_utility) &= ~0x6)	//bit2  无折返

#define IsNewNextTrainReturn(side_utility)    (((side_utility)&0x10) == 0x10)	//bit4
#define IsNextTrainOutReturn(side_utility)    (((side_utility)&0x30) == 0x30)	//bit2
#define SetNextTrainOutReturn(side_utility)   ((side_utility) &= ~0x30, (side_utility) |= 0x30)	//bit5  站后折返
#define SetNextTrainStationReturn(side_utility)   ((side_utility) &= ~0x30, (side_utility) |= 0x10)	//bit5  原地折返
#define NotNextStationReturn(side_utility)   ((side_utility) &= ~0x30)	//bit4  无折返


#define SetTccStatusNone(block)      ((block).dir = (0xCFFF&((block).dir))|0x0000)
#define SetTccStatusConfirm(block)   ((block).dir = (0xCFFF&((block).dir))|0x1000)
#define SetTccStatusExec(block)      ((block).dir = (0xCFFF&((block).dir))|0x2000)
#define SetTccStatusCancel(block)    ((block).dir = (0xCFFF&((block).dir))|0x3000)

#define IsTccStatusNone(block)	     ((0x3000&((block).dir)) == 0x0000)
#define IsTccStatusConfirm(block)	 ((0x3000&((block).dir)) == 0x1000)
#define IsTccStatusExec(block)	     ((0x3000&((block).dir)) == 0x2000)
#define IsTccStatusCancel(block)	 ((0x3000&((block).dir)) == 0x3000)

#define IsFlagGiveout(flag)         ((flag & 0x0007) ==  0x0001)
#define IsFlagTerminal(flag)        (((flag & 0x0007) ==  0x0002)  || ((flag & 0x0007) ==  0x0003))
#define IsFlagEnd(flag)             ((flag & 0x0007) ==  0x0003)
#define IsFlagKeep(flag)            ((flag & 0x0007) ==  0x0004)
#define IsFlagRelay(flag)           ((flag & 0x0007) ==  0x0005)
#define IsFlagEndInAbs(flag)        ((flag & 0x0007) ==  0x0006)
#define IsFlagDivide(flag)          ((flag & 0x0007) ==  0x0007)
#define IsFlagAccept(flag)          ((flag & 0x0018) ==  0x0008)
#define IsFlagStart(flag)           ((flag & 0x0018) ==  0x0010)
#define IsFlagReverse(flag)         (flag & 0x0020)
#define IsFlagCorner(flag)          (flag & 0x0040)
#define IsFlagStopSignal(flag)      ((flag & 0x0080) ==  0x0080)
#define IsFlagStopAbs(flag)         ((flag & 0x0100) ==  0x0100)
#define IsFlagAnLocReturn(flag)     (flag & 0x0200)
#define IsFlagPassNonMainSide(flag) ((flag & 0x0400) ==  0x0400)
#define IsFlagRescue(flag)          (flag & 0x0800)//11 bit 
#define IsFlagRescued(flag)         (flag & 0x1000)//12 bit
#define IsFlagStopAbsUp(flag)       (flag & 0x2000)
#define IsFlagStopAbsDown(flag)     (!(flag & 0x2000))
#define IsFlagUnitOtherTrain(flag)  (flag & 0x4000)   //bit 14
#define IsFalgBeUnitedByOther(flag) (flag & 0x8000)   //bit 15
#define IsFlagStopMainSide(flag)    (0)
#define IsFlagAbsTerminal(flag)     (flag & 0x00010000)
#define IsFlagAbsStart(flag)        (flag & 0x00020000)

#define IsFlagDivided(flag)         (flag & 0x00040000)
#define IsFlagCombine(flag)         (flag & 0x00100000)
#define IsFlagCombined(flag)        (flag & 0x00200000)
#define IsFlagAttainEngine(flag)    (flag & 0x00400000)
#define IsFlagTrainPartRun(flag)    (flag & 0x00800000)
#define IsFlagTrainWholeHelp(flag)  (flag & 0x01000000)

#define SetFlagGiveout(flag)         (flag &= 0xFFFFFFF8,flag|= 0x0001)
#define SetFlagTerminal(flag)        (flag &= 0xFFFFFFF8,flag|= 0x0002)
#define SetFlagEnd(flag)             (flag &= 0xFFFFFFF8,flag|= 0x0003)
#define SetFlagKeep(flag)            (flag &= 0xFFFFFFF8,flag|= 0x0004)
#define SetFlagRelay(flag)           (flag &= 0xFFFFFFF8,flag|= 0x0005)
#define SetFlagEndInAbs(flag)        (flag &= 0xFFFFFFF8,flag|= 0x0006)
#define SetFlagDivide(flag)          (flag &= 0xFFFFFFF8,flag|= 0x0007)
#define SetFlagAccept(flag)          (flag &= 0xFFFFFFE7,flag|= 0x0008)
#define SetFlagStart(flag)           (flag &= 0xFFFFFFE7,flag|= 0x0010)
#define SetFlagReverse(flag)         (flag |= 0x0020)
#define SetFlagCorner(flag)          (flag |= 0x0040)
#define SetFlagStopSignal(flag)      (flag |= 0x0080)
#define SetFlagStopAbs(flag)         (flag |= 0x0100)
#define SetFlagAnLocReturn(flag)     (flag |= 0x0200)
#define SetFlagPassNonMainSide(flag) (flag |= 0x0400)
#define SetFlagRescue(flag)          (flag |= 0x0800)
#define SetFlagRescued(flag)         (flag |= 0x1000)
#define SetFlagStopAbsUp(flag)       (flag |= 0x2000)
#define SetFlagStopAbsDown(flag)     (flag &= 0xFFFFDFFF)
#define SetFlagUnitOtherTrain(flag)  (flag |= 0x4000)
#define SetFlagBeUnitedByOther(flag) (flag |= 0x8000)
#define SetFlagStopMainSide(flag)    (flag)
#define SetFlagAbsTerminal(flag)     (flag |= 0x00010000)
#define SetFlagAbsStart(flag)        (flag |= 0x00020000)

#define ReSetFlagAbsTerminal(flag)   (flag &= ~0x00010000)
#define ReSetFlagAbsStart(flag)      (flag &= ~0x00020000)

#define SetFlagDivided(flag)         (flag |= 0x00040000)
#define SetFlagCombine(flag)         (flag |= 0x00100000)
#define SetFlagCombined(flag)        (flag |= 0x00200000)
#define SetFlagAttainEngine(flag)    (flag |= 0x00400000)
#define SetFlagTrainPartRun(flag)    (flag |= 0x00800000)
#define SetFlagTrainWholeHelp(flag)  (flag |= 0x01000000)

#define ClearFlagReverse(flag)         (flag &= ~0x0020)
#define ClearFlagCorner(flag)          (flag &= ~0x0040)
#define ClearFlagStopSignal(flag)      (flag &= ~0x0080)
#define ClearFlagStopAbs(flag)         (flag &= ~0x0100)
#define ClearFlagAnLocReturn(flag)     (flag &= ~0x0200)
#define ClearFlagPassNonMainSide(flag) (flag &= ~0x0400)
#define ClearFlagStopMainSide(flag)    (flag)
#define RemoveFlagStatus(flag)         (flag &= ~0x1F)

#define RemoveFlagLeaveStatus(flag)    (flag &= ((IsFlagEndInAbs(flag) || IsFlagDivide(flag)) ? 0xFFFFFFFF:0xFFFFFFF8))
#define RemoveFlagEnterStatus(flag)    (flag &= 0xFFFFFFE7)

#define ClearFlagUnitOtherTrain(flag)  (flag &= 0xFFFFBFFF)
#define ClearFlagBeUnitedByOther(flag) (flag &= 0xFFFF7FFF)
#define ClearFlagAbsTerminal(flag)     (flag &= ~0x00010000)
#define ClearFlagAbsStart(flag)        (flag &= ~0x00020000)
#define ClearFlagAttainEngine(flag)    (flag &= ~0x00400000)
#define ClearFlagTrainPartRun(flag)    (flag &= ~0x00800000)
#define ClearFlagTrainWholeHelp(flag)  (flag &= ~0x01000000)

#define IsTrainPass(rec)                  ((rec).arrive == (rec).depart && (rec).arrive_side == (rec).depart_side && (!IsFlagStart((rec).flag) && !IsFlagTerminal((rec).flag)))

#define IsDefinePass(rec)                 ((rec).stop_condition & 0x02)
#define IsForcePass(rec)                  ((rec).stop_condition & 0x04)
#define IsForceSide(rec)                  ((rec).stop_condition & 0x08)
#define IsForcePassByMainSide(rec)        ((rec).stop_condition & 0x10)

#define SetDefinePass(rec)                ((rec).stop_condition |= 0x02)
#define SetForcePass(rec)                 ((rec).stop_condition |= 0x04)
#define SetForceSide(rec)                 ((rec).stop_condition |= 0x08)
#define SetForcePassByMainSide(rec)       ((rec).stop_condition |= 0x10)

#define ReSetDefinePass(rec)              ((rec).stop_condition &= ~0x02)
#define ReSetForcePass(rec)               ((rec).stop_condition &= ~0x04)
#define ReSetForceSide(rec)               ((rec).stop_condition &= ~0x08)
#define ReSetForcePassByMainSide(rec)     ((rec).stop_condition &= ~0x10)

#define IsAutoMakeArriveRoute(stop_condition)        ((stop_condition & 0x20) != 0x20)
#define IsAutoMakeDepartRoute(stop_condition)        ((stop_condition & 0x40) != 0x40)
#define SetAutoMakeArriveRoute(stop_condition)       (stop_condition &= ~0x20)
#define SetAutoMakeDepartRoute(stop_condition)       (stop_condition &= ~0x40)
#define SetNotAutoMakeArriveRoute(stop_condition)    (stop_condition |= 0x20)
#define SetNotAutoMakeDepartRoute(stop_condition)    (stop_condition |= 0x40)

// 发车时机
#define IsMakeDepartRouteByMeet(stop_condition)      ((stop_condition & 0x80) == 0x00)
#define SetMakeDepartRouteByMeet(stop_condition)     (stop_condition &= ~0x80)
#define IsMakeDepartRouteByPlan(stop_condition)      ((stop_condition & 0x80) == 0x80)
#define SetMakeDepartRouteByPlan(stop_condition)     (stop_condition |= 0x80) 

// 是否压前压后
#define IsPressureOutStation(stop_condition)    (stop_condition & 0xC00)

// 压前
#define IsBeforePressure(stop_condition)        ((stop_condition & 0x400) == 0x400)
#define SetBeforePressure(stop_condition)       (stop_condition |= 0x400) 
#define ClearBeforePressure(stop_condition)     (stop_condition &= ~0x400) 

// 压后
#define IsAfterPressure(stop_condition)         ((stop_condition & 0x800) == 0x800)
#define SetAfterPressure(stop_condition)        (stop_condition |= 0x800) 
#define ClearAfterPressure(stop_condition)      (stop_condition &= ~0x800)

// 发车时机人工干预标志
#define IsManualSetTrigeRoute(stop_condition)   ((stop_condition & 0x1000) == 0x1000)
#define ManualSetTrigeRoute(stop_condition)     (stop_condition |= 0x1000)

// OPMS计划检查条件:禁止通过
#define IsMustNoPass(rec)     ((rec).stop_condition & 0x2000) 
#define SetFlagMustNoPass(rec)       ((rec).stop_condition |= 0x2000) 
#define ClearFlagMustNoPass(rec)     ((rec).stop_condition &= ~0x2000) 

// 股道和交路折返股道冲突,是否长显示股道,重点提示用
#define IsDispalySide(stop_condition)     (((stop_condition) & 0x4000) == 0x4000) 
#define SetFlagDispalySide(stop_condition)       ((stop_condition) |= 0x4000) 
#define ClearFlagDispalySide(stop_condition)     ((stop_condition) &= ~0x4000) 

// 修改交路股道, 是否长显示股道,重点提示用
#define IsDispalySide2(stop_condition)     (((stop_condition) & 0x8000) == 0x8000) 
#define SetFlagDispalySide2(stop_condition)       ((stop_condition) |= 0x8000) 
#define ClearFlagDispalySide2(stop_condition)     ((stop_condition) &= ~0x8000) 

#define IsArrivePlan(rec)           (!((rec).adjust_status & 0x0001))
#define IsArriveActual(rec)         ((rec).adjust_status & 0x0001)
#define IsDepartPlan(rec)           (!((rec).adjust_status & 0X0002))
#define IsDepartActual(rec)         ((rec).adjust_status & 0x0002)
#define IsStatusRight(rec)          (!((rec).adjust_status & 0x0004))
#define IsStatusError(rec)          ((rec).adjust_status & 0x0004)
#define IsStatusCanAdjust(rec)      (!((rec).adjust_status & 0x0008))
#define IsStatusCannotAdjust(rec)   ((rec).adjust_status & 0x0008)
#define SetArrivePlan(rec)          ((rec).adjust_status &= 0xFFFE)
#define SetArriveActual(rec)        ((rec).adjust_status |= 0x0001)
#define SetDepartPlan(rec)          ((rec).adjust_status &= 0xFFFD)
#define SetDepartActual(rec)        ((rec).adjust_status |= 0x0002)
#define SetStatusRight(rec)         ((rec).adjust_status &= ~0x0004)
#define SetStatusError(rec)         ((rec).adjust_status |= 0x0004)
#define SetStatusCanAdjust(rec)     ((rec).adjust_status &= ~0x0008)
#define SetStatusCannotAdjust(rec)  ((rec).adjust_status |= 0x0008)

#define IsTechStopTrain(rec)        (((rec).adjust_status & 0x0010))
#define SetTechStopTrain(rec)       ((rec).adjust_status |= 0x0010)
#define ClearTechStopTrain(rec)     ((rec).adjust_status &= ~0x0010)

#define IsArriveLock(rec)           (((rec).adjust_status & 0x0100) == 0x0100)
#define IsDepartLock(rec)           (((rec).adjust_status & 0x0200) == 0x0200)
#define SetArriveLock(rec)          ((rec).adjust_status |= 0x0100)
#define SetDepartLock(rec)          ((rec).adjust_status |= 0x0200)
#define ReSetArriveLock(rec)        ((rec).adjust_status &= ~0x0100)
#define ReSetDepartLock(rec)        ((rec).adjust_status &= ~0x0200)

#define IsInterArriveSame (rec)     (!((rec).adjust_status & 0x0040))
#define IsInterArriveDiffer (rec)   ((rec).adjust_status & 0x0040)
#define SetInterArriveSame (rec)    ((rec).adjust_status &= ~0x0040))
#define SetInterArriveDiffer (rec)  ((rec).adjust_status |= 0x0040))
#define IsInterDepartSame (rec)     (!((rec).adjust_status & 0x0080))
#define IsInterDepartDiffer (rec)   ((rec).adjust_status & 0x0080)
#define SetInterDepartSame (rec)    ((rec).adjust_status &= ~0x0080))
#define SetInterDepartDiffer (rec)  ((rec).adjust_status |= 0x0080)

#define IsArriveActualByManual(rec) (!((rec).adjust_status & 0x0400) && ((rec).adjust_status & 0x0001))
#define IsArriveActualByAuto(rec)   (((rec).adjust_status & 0x0400) && ((rec).adjust_status & 0x0001))
#define IsDepartActualByManual(rec) (!((rec).adjust_status & 0x0800) && ((rec).adjust_status & 0x0002))
#define IsDepartActualByAuto(rec)   (((rec).adjust_status & 0x0800) && ((rec).adjust_status & 0x0002))
#define SetArriveActualByManual(rec) ((rec).adjust_status|=0x0001,(rec).adjust_status&= ~0x0400)
#define SetArriveActualByAuto(rec)   ((rec).adjust_status|=0x0001,(rec).adjust_status|=0x0400)
#define SetDepartActualByManual(rec) ((rec).adjust_status|=0x0002,(rec).adjust_status&= ~0x0800)
#define SetDepartActualByAuto(rec)   ((rec).adjust_status|=0x0002,(rec).adjust_status|=0x0800)

#define SetAdjustBeginSta(rec)          ((rec).adjust_status |= 0x1000)              //bit 12
#define SetAdjustNormalSta(rec)         ((rec).adjust_status &= ~0x1000)
#define IsAdjustBeginSta(rec)           ((rec).adjust_status &  0x1000)
#define SetNotSendPlanToNextCon(rec)    ((rec).adjust_status &= 0x9FFF)          //bit 13,14: 00
#define SetBeRejectedByNextCon(rec)     ((rec).adjust_status &= 0x9FFF, (rec).adjust_status |= 0x2000)   //bit 13,14: 01
#define SetBeReceivedByNextCon(rec)     ((rec).adjust_status &= 0x9FFF, (rec).adjust_status |= 0x4000)   //bit 13,14: 10
#define IsRejectedByNextCon(rec)        (((rec).adjust_status & 0x6000) == 0x2000)
#define IsReceivedByNextCon(rec)        (((rec).adjust_status & 0x6000) == 0x4000)
#define IsSendPlanToNextCon(rec)        (((rec).adjust_status & 0x6000) == 0x6000)

#define IsElectricEngineTrain(rec)        (((rec).adjust_status&0x8000) == 0x8000)
#define SetElectricEngineTrain(rec)       ((rec).adjust_status |= 0x8000)
#define ResetElectricEngineTrain(rec)     ((rec).adjust_status &= ~0x8000)

// 分界口车站
#define TG_SetFJKStationFlag(rec)        ((rec).adjust_status |= 0x10000)              //bit 16
#define TG_SetNoFJKStationFlag(rec)        ((rec).adjust_status &= ~0x10000)              //bit 16
#define TG_IsFJKStationFlag(rec)           ((rec).adjust_status & 0x10000)              //bit 16

// 台间车站
#define TG_SetNeighbourStationFlag(rec)        ((rec).adjust_status |= 0x20000)              //bit 17
#define TG_SetNoNeighbourStationFlag(rec)        ((rec).adjust_status &= ~0x20000)        //bit 17
#define TG_IsNeighbourStationFlag(rec)           ((rec).adjust_status & 0x20000)              //bit 17

// 自动设置折返车次
#define IsAutoSetReturnTrainId(rec)     (((rec).adjust_status&0x00040000) == 0x00040000)	//bit 18
#define SetAutoReturnTrainId(rec)       ((rec).adjust_status |= 0x00040000)					//bit 18
#define ResetAutoReturnTrainId(rec)     ((rec).adjust_status &= ~0x00040000)				//bit 18

// 3.0卡控基本图出入口
#define TG_IsNotCheckTrainExitEntry(rec)   (((rec).adjust_status&0x80000) == 0x80000)	//bit 19
#define TG_NotCheckTrainExitEntry(rec)	   ((rec).adjust_status |= 0x80000)					//bit 19
#define TG_CheckTrainExitEntry(rec)		   ((rec).adjust_status &= ~0x80000)				//bit 19

// 3.0不卡控基本图股道
#define TG_IsNotCheckTrainSide(rec)          (((rec).adjust_status & 0x100000) == 0x100000)  // bit 20
#define TG_NotCheckTrainSide(rec)            ((rec).adjust_status |= 0x100000)
#define TG_CheckTrainSide(rec)			     ((rec).adjust_status &= ~0x100000)

#define IsSpecifyAbsRunTime(rec)              ((rec)->spec_run_time)
#define SetSpecifyAbsRunTime(rec, ltime)      ((rec)->spec_run_time = ltime)

#define SetRunFastSlowTime(rfstime, t)  (rfstime = (((short)(t/60)>127 ||(short)(t/60)<-127)? 0 :(((short)(t/60)<=0)?(short)(t/60)*(-1):(t/60+128))))
#define InitRunFastSlowTIme(rfstime)    (rfstime = 0)

#define IsPermitEarlyDepart(rec)           ((short)(rec).early_depart_time != (short)0x0000)
#define SetCanNotEarlyDepart(rec)          ((rec).early_depart_time = 0x0000)
#define IsNoLimitEarlyDepart(rec)          ((rec).early_depart_time == (short)0x8000)
#define SetNoLimitEarlyDepart(rec)         ((rec).early_depart_time =  (short)0x8000)
#define SetPermitEarlyDepart(rec, ltime)   ((rec).early_depart_time = (ltime - (rec).plan_depart)/60)
#define GetEarlyDepartLimit(rec)           (!IsPermitEarlyDepart(rec)?(rec).plan_depart:IsNoLimitEarlyDepart(rec)?NULL_TIME:(rec).early_depart_time*60+(rec).plan_depart)
#define SetArriveRouteClear(rec)      ((rec).route_flag &= 0xFC)
#define SetDepartRouteClear(rec)      ((rec).route_flag &= 0xF3)
#define SetRouteClear(rec)            (SetArriveRouteClear(rec), SetDepartRouteClear(rec))

#define IsArriveRouteClear(rec)      (((rec).route_flag & 0x03) == 0x00)
#define IsArriveRouteDoing(rec)      (((rec).route_flag & 0x03) == 0x01)
#define IsArriveRouteOk(rec)         (((rec).route_flag & 0x03) == 0x02)
#define IsArriveRouteOccupy(rec)     (((rec).route_flag & 0x03) == 0x03)
#define IsDepartRouteClear(rec)      (((rec).route_flag & 0x0C) == 0x00)
#define IsDepartRouteDoing(rec)      (((rec).route_flag & 0x0C) == 0x04)
#define IsDepartRouteOk(rec)         (((rec).route_flag & 0x0C) == 0x08)
#define IsDepartRouteOccupy(rec)     (((rec).route_flag & 0x0C) == 0x0C)
#define IsRouteClear(rec)            (IsArriveRouteClear(rec) && IsDepartRouteClear(rec))
#define TG_IsTrainIntoSide(rec)      (((rec).route_flag & 0x10) == 0x10)

#define SetRecordNormal(rec)             ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x00)
#define SetRecordChange(rec)             ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x01)
#define SetRecordInsert(rec)             ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x02)
#define SetRecordSendBack(rec)           ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x03)
#define IsRecordNormal(rec)              (!((rec).db_flag & 0x03))
#define IsRecordChange(rec)              (((rec).db_flag & 0x03) == 1)
#define IsRecordInsert(rec)              (((rec).db_flag & 0x03) == 2)
#define IsRecordSendBack(rec)            (((rec).db_flag & 0x03) == 3)

#define SetRecordSaveIntoHo(rec)         ((rec).db_flag |= 0x04)
#define SetRecordNotSaveIntoHo(rec)      ((rec).db_flag &= ~0x04)
#define IsRecordSaveIntoHo(rec)          ((rec).db_flag & 0x04)

#define SetSideFromBasicSchedule(rec)              ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x00)
#define SetSideFromAdditionSchedule(rec)           ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x01)
#define SetSideFromStaticDefine(rec)               ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x02)
#define SetSideFromDispatcher(rec)                 ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x03)
#define SetSideFromAutoAdjust(rec)                 ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x04)
#define SetSideFromStaWatcherChangePlan(rec)       ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x05)
#define SetSideFromStaWatcherReportActual(rec)     ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x06)
#define SetSideFromLircChangePlan(rec)             ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x07)
#define SetSideFromLircReportActual(rec)           ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x08)
#define SetSideFromPathWay(rec)                    ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x09)

#define IsSideFromBasicSchedule(rec)               (((rec).data_source & 0x0F) == 0)
#define IsSideFromAdditionSchedule(rec)            (((rec).data_source & 0x0F) == 1)
#define IsSideFromStaticDefine(rec)                (((rec).data_source & 0x0F) == 2)
#define IsSideFromDispatcher(rec)                  (((rec).data_source & 0x0F) == 3)
#define IsSideFromAutoAdjust(rec)                  (((rec).data_source & 0x0F) == 4)
#define IsSideFromStaWatcherChangePlan(rec)        (((rec).data_source & 0x0F) == 5)
#define IsSideFromStaWatcherReportActual(rec)      (((rec).data_source & 0x0F) == 6)
#define IsSideFromLircChangePlan(rec)              (((rec).data_source & 0x0F) == 7)
#define IsSideFromLircReportActual(rec)            (((rec).data_source & 0x0F) == 8)
#define IsSideFromPathWay(rec)                     (((rec).data_source & 0x0F) == 9)

#define SetRecTimeFromDefault(rec, ad)              ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0000:0x0000))
#define SetRecTimeFromAutoAdjust(rec, ad)           ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0010:0x0100))
#define SetRecTimeFromLircChangePlan(rec, ad)       ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0020:0x0200))
#define SetRecTimeFromDispatcherTurnActual(rec, ad) ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0030:0x0300))
#define SetRecTimeFromStaWatcherChangePlan(rec, ad) ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0040:0x0400))
#define SetRecTimeFromStaWatcherReport(rec, ad)     ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0050:0x0500))
#define SetRecTimeFromLircReportActual(rec, ad)     ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0060:0x0600))
#define SetRecTimeFromLircReportMatch(rec, ad)      ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0070:0x0700))
#define SetRecTimeFromSimpleAdjust(rec, ad)         ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0080:0x0800))

#define IsRecTimeFromDefault(rec, ad)               (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0000:0x0000))
#define IsRecTimeFromAutoAdjust(rec, ad)            (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0010:0x0100))
#define IsRecTimeFromLircChangePlan(rec, ad)        (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0020:0x0200))
#define IsRecTimeFromDispatcherTurnActual(rec, ad)  (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0030:0x0300))
#define IsRecTimeFromStaWatcherChangePlan(rec, ad)  (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0040:0x0400))
#define IsRecTimeFromStaWatcherReport(rec, ad)      (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0050:0x0500))
#define IsRecTimeFromLircReportActual(rec, ad)      (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0060:0x0600))
#define IsRecTimeFromLircReportMatch(rec, ad)       (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0070:0x0700))
#define IsRecTimeFromSimpleAdjust(rec, ad)          (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0080:0x0800))

#define SetPlanTimeFromBasicSchedule(rec, ad)       ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x00000:0x00000))
#define SetPlanTimeFromAdditionSchedule(rec, ad)    ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x01000:0x08000))
#define SetPlanTimeFromAutoCreate(rec, ad)          ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x02000:0x10000))
#define SetPlanTimeFromManualSet(rec, ad)           ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x03000:0x18000))

#define IsPlanTimeFromBasicSchedule(rec, ad)        (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x00000:0x00000))
#define IsPlanTimeFromAdditionSchedule(rec, ad)     (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x01000:0x08000))
#define IsPlanTimeFromAutoCreate(rec, ad)           (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x02000:0x10000))
#define IsPlanTimeFromManualSet(rec, ad)            (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x03000:0x18000))

// 车站股道修改程序确认股道
#define IsStManualSetSide(data_source)            ((data_source & 0x80000000) == 0x80000000)

#define SetTrainFromBasicSchedule(coninfo)            ((coninfo).train_source = 0)
#define SetTrainFromAdditionSchedule(coninfo)         ((coninfo).train_source = 1)
#define SetTrainFromReportTrue(coninfo)               ((coninfo).train_source = 2)
#define SetTrainFromReportFake(coninfo)               ((coninfo).train_source = 3)
#define SetTrainFromFreeDraw(coninfo)                 ((coninfo).train_source = 4)
#define SetTrainFromPathWay(coninfo)                  ((coninfo).train_source = 5)
#define SetTrainFromProbe(coninfo)                    ((coninfo).train_source = 6)
#define SetTrainFromInformalCreate(coninfo)           ((coninfo).train_source = 7)

#define IsBasicScheduleTrain(coninfo)                 (0 == (coninfo).train_source ? 1 : 0)
#define IsAdditionScheduleTrain(coninfo)              (1 == (coninfo).train_source ? 1 : 0)
#define IsReportTrueTrain(coninfo)                    (2 == (coninfo).train_source ? 1 : 0)
#define IsReportFakeTrain(coninfo)                    (3 == (coninfo).train_source ? 1 : 0)
#define IsFreeDrawTrain(coninfo)                      (4 == (coninfo).train_source ? 1 : 0)
#define IsPathWayCreateTrain(coninfo)                 (5 == (coninfo).train_source ? 1 : 0)
#define IsProbeCreateTrain(coninfo)                   (6 == (coninfo).train_source ? 1 : 0)
#define IsInformalCreateTrain(coninfo)                (7 == (coninfo).train_source ? 1 : 0)

#define GetAdjustBeginSta(conptr)             ((conptr)->adjust_begin_sta & ~0x0080)
#define IfAdjustBeginStaManualSet(conptr)     (!((conptr)->adjust_begin_sta & 0x0080))
#define IfAdjustBeginStaAutoSet(conptr)       ((conptr)->adjust_begin_sta & 0x0080)
#define SetAdjustBeginStaAutoSet(conptr)      ((conptr)->adjust_begin_sta |= 0x0080)
#define GetAdjustEndSta(conptr)               ((conptr)->adjust_end_sta)

#define  SetConsoleTrainNotExistSubao(conptr)  ((conptr)->status &= ~0x7)   //bit 0-2
#define  IsConsoleTrainExistSubao(conptr)      ((conptr)->status & 0x7) 

#define  SetSubaoFromStation(conptr)           ((conptr)->status &= ~0x7,(conptr)->status |= 0x0001)
#define  SetSubaoFromPreConsole(conptr)        ((conptr)->status &= ~0x7,(conptr)->status |= 0x0002)
#define  SetSubaoFromDispatcher(conptr)        ((conptr)->status &= ~0x7,(conptr)->status |= 0x0003)
#define  SetSubaoFromWireless(conptr)          ((conptr)->status &= ~0x7,(conptr)->status |= 0x0004)
#define  IsSubaoFromStation(conptr)            (((conptr)->status & 0x7) == 0x1)
#define  IsSubaoFromPreConsole(conptr)         (((conptr)->status & 0x7) == 0x2)
#define  IsSubaoFromDispatcher(conptr)         (((conptr)->status & 0x7) == 0x3)
#define  IsSubaoFromWireless(conptr)           (((conptr)->status & 0x7) == 0x4)

#define SetTrainSecret(conptr)          ((conptr).status |= 0x8)
#define SetTrainNotSecret(conptr)       ((conptr).status &= ~0x8)
#define IsTrainSecret(conptr)           (((conptr).status & 0x8) == 0x8)

#define SetTrainFromShift(conptr)		((conptr).status |= 0x10)
#define ResetTrainFromShift(conptr)		((conptr).status &= ~0x10)
#define IsTrainFromShift(conptr)		(((conptr).status & 0x10) == 0x10)

// 该位已经使用
#define SetNoLateEarlayTime(conptr)     ((conptr).status |= 0x20)
#define ResetNoLateEarlayTime(conptr)   ((conptr).status &= ~0x20)
#define IsNoLateEarlayTime(conptr)      (((conptr).status & 0x20) == 0x20)


#define  SetDieselEngineExt(enginetype)		    ((enginetype) &= 0xF0)                        //bit 0-3 
#define  SetElecEngineExt(enginetype)		    ((enginetype) &= 0xF0, (enginetype)|= 0x01) 
#define  IsDieselEngineExt(enginetype)		    (((enginetype) & 0x0F) == 0x00) 
#define  IsElecEngineExt(enginetype)		    (((enginetype) & 0x0F) == 0x01) 
#define  SetTractionEngineExt(enginetype)	    ((enginetype) &= 0xCF)                        //bit 4,5 
#define  SetAttachEngineExt(enginetype)		    ((enginetype) &= 0xCF, (enginetype)|= 0x10)
#define  IsTractionEngineExt(enginetype)	    (((enginetype) & 0x30) == 0x00) 
#define  IsAttachEngineExt(enginetype)		    (((enginetype) & 0x30) == 0x10) 
#define  SetEngineFromDispatcher(enginetype)    ((enginetype) &= 0x3F)                        //bit 6,7 
#define  SetEngineFromSubao(enginetype)         ((enginetype) &= 0x3F, (enginetype)|= 0x40) 
#define  IsEngineFromDispatcher(enginetype)     (((enginetype) & 0xC0) == 0x00) 
#define  IsEngineFromSubao(enginetype)          (((enginetype) & 0xC0) == 0x40) 

#define IsCheckNCWayStop(rec)          ((rec.flag & 0x03) == 0x01)
#define IsCheckNCWayPass(rec)          ((rec.flag & 0x03) == 0x02)
#define IsNoCheckNCWayStopPass(rec)    ((rec.flag & 0x03) == 0x00)
#define IsCheckNCWayEntry(rec)         (rec.entry  != 0xFF)
#define IsCheckNCWayExit(rec)          (rec.exit  != 0xFF)
#define IsCheckNCWaySide(rec)          (rec.side  != 0xFF)

#define IsCheckNCWayStart(recs)              (((recs).flag & 0x0003) == 0x0001)
#define IsCheckNCWayAccept(recs)             (((recs).flag & 0x0003) == 0x0002)
#define IsNoCheckNCWayStartAccept(recs)      (((recs).flag & 0x0003) == 0x0000)
#define IsCheckNCWayTerminal(recs)           (((recs).flag & 0x000c) == 0x0001)
#define IsCheckNCWayGiveout(recs)            (((recs).flag & 0x000c) == 0x0002)
#define IsNoCheckNCWayTerminalGiveout(recs)  (((recs).flag & 0x000c) == 0x0000)
#define IsCheckNCWayActual(recs)             (((recs).flag & 0x0030) == 0x0010)
#define IsCheckNCWayPlan(recs)               (((recs).flag & 0x0030) == 0x0020)
#define IsNoCheckNCWayActualPlan(recs)       (((recs).flag & 0x0030) == 0x0000)

#define IsCheckNCWaySame(recs)               (((recs).flag & 0x00C0) == 0x0000 || ((recs).flag & 0x00C0) == 0x0040)
#define ISCheckNCWayEmbed(recs)              (((recs).flag & 0x00C0) == 0x0000 || ((recs).flag & 0x00C0) == 0x0080)


#define GetTrainRecordDir(rec)            ((rec).entry != 0xFF ? TrainDirection((rec).station, (rec).entry, TRAIN_INTO_STATION) : TrainDirection((rec).station, (rec).exit, TRAIN_OUT_STATION))

#define GetValidADTime(rec)           ((!(rec).arrive || NULL_TIME == (rec).arrive) ? (rec).depart:(rec).arrive)
#define GetValidDATime(rec)           ((!(rec).depart || NULL_TIME == (rec).depart) ? (rec).arrive:(rec).depart)
#define SetArriveTimeOff(rec, toff)   ((rec).arrive += (NULL_TIME == (rec).arrive ? 0 : (int)(toff)))
#define SetDepartTimeOff(rec, toff)   ((rec).depart += (NULL_TIME == (rec).depart ? 0 : (int)(toff)))
#define SetTrainRecordTimeOff(rec, toff)   (SetArriveTimeOff(rec, toff), SetDepartTimeOff(rec, toff))

#define IsInhibitStation(schdblock)                ((schdblock).position & 0x03)
#define IsInhibitFirstStation(schdblock)           (((schdblock).position & 0x01) == 0x01)
#define IsInhibitSecondStation(schdblock)          (((schdblock).position & 0x02) == 0x02)

#define QUALIFY_TRAININDEX(tindex)    if (0 == (tindex) || (TRAIN_INDEX)-1 == (tindex)) (tindex) = NULL_TRAININDEX;
#define QUALIFY_STATION(sta)          if (0 == (sta)    || (STATION_NO)-1  == (sta))    (sta)    = NULL_STATION; 
#define QUALIFY_ENTRY(entry)          if (0 == (entry)  || (ENTRY)-1 == (entry))        (entry)  = NULL_ENTRY; 
#define QUALIFY_SIDE(side)            if (0 == (side)   || (SIDE)-1  == (side))         (side)   = NULL_SIDE; 

// shoud be same with gpc
#define QUALIFY_TIME(ltime)           (ltime /60 *60)

#define  IS_VALID_ENTRY(x)  (x>0 && x<100)
#define  IS_VALID_SIDE(x)  (x>0 && x<255)
#endif /** THE END **/

