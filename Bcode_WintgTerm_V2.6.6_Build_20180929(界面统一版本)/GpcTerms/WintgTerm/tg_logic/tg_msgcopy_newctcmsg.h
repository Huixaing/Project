#ifndef BLOCK_CP_H
#define BLOCK_CP_H
#include "tg_work_schdule_holder.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include <baseMsg\msg_schd.h>
extern BOOL  BlockCopy(MsgSchdBlock& msg, const TG_ScheduleBlock& def);
extern BOOL  BlockCopy(TG_ScheduleBlock& def, const MsgSchdBlock& msg);
extern BOOL  TrainCopy(MsgTrainInfo& msg,         const TG_TrainInfo& def);
extern BOOL  TrainCopy(MsgTrainEngineInfo& msg,   const TG_TrainEngineInfo& def);
extern BOOL  TrainCopy(MsgTrainRecord& msg,       const TG_TrainRecord& rec);

extern BOOL  TrainCopy(TG_TrainInfo& def,       const MsgTrainInfo& msg);
extern BOOL  TrainCopy(TG_TrainEngineInfo& def, const MsgTrainEngineInfo& msg);
extern BOOL  TrainCopy(TG_TrainRecord& rec,     const MsgTrainRecord& msg);

extern BOOL  BlockCopy(MsgSchdBlock& msg,          const CTG_SingleMark& def);
extern BOOL  TrainCopy(MsgTrainInfo& msg,          const CTG_TrainInfo& def);
extern BOOL  TrainCopy(MsgTrainEngineInfo& msg,    const CTG_TrainEngineInfo& def);
extern BOOL  TrainCopy(MsgTrainRecord& msg,        const CTG_TrainRecord& rec);


extern BOOL  BlockCopy(CTG_SingleMark& def,        const MsgSchdBlock& msg);
extern BOOL  TrainCopy(CTG_TrainInfo& def,         const MsgTrainInfo& msg);
extern BOOL  TrainCopy(CTG_TrainEngineInfo& def,   const MsgTrainEngineInfo& msg);
extern BOOL  TrainCopy(CTG_TrainRecord& rec,       const MsgTrainRecord& msg);

void CopyStamp(TG_MsgStamp &dest,const  MsgSchdStamp&src);
void CopyStamp(MsgSchdStamp &dest,const TG_MsgStamp &src);

BOOL  TrainCopy(CTG_SingleTrain& train,const MsgTrain& msg);
BOOL  TrainCopy(MsgTrain& msg,const CTG_SingleTrain& train);

BOOL  TrainCopy(MsgTrainUpdateEngine &msg,const CTG_SingleTrain& train);
BOOL  TrainCopy(CTG_SingleTrain& train,const MsgTrainUpdateEngine &msg );

BOOL  TrainCopy(MsgTrainUpdateRecord &msg,const CTG_SingleTrain& train);
BOOL  TrainCopy(CTG_SingleTrain& train,const MsgTrainUpdateRecord &msg );

BOOL  TrainCopy(MsgTrainUpdateInfo &msg,const CTG_SingleTrain& train);
BOOL  TrainCopy(CTG_SingleTrain& train,const MsgTrainUpdateInfo &msg );

BOOL  TrainExtentCopy(CTG_TrainExtent& trainExtent,const MsgTrainExtent& msg);
BOOL  TrainExtentCopy(MsgTrainExtent& msg, const CTG_SingleTrain& train);
////////////////////////////////////////////////////////////////////////
BOOL  TrainCopyForOCX(MsgTrainOCX& msg,const CTG_SingleTrain& train);
BOOL  BlockCopyForOCX(MsgSchdBlockOCX& msg,  const CTG_SingleMark& def);
#endif
