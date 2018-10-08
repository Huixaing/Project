#ifndef TRAIN_CP_H
#define TRAIN_CP_H


extern short TrainCopy(MsgTrain& msg,           const cTrainTextDef& def);
extern short TrainCopy(MsgTrainInfo& msg,       const TrainInfo& def);
extern short TrainCopy(MsgTrainEngineInfo& msg, const TrainEngineInfo& def);
extern short TrainCopy(MsgTrainRecord& msg,     const TrainRecord& rec);

extern short TrainCopy(cTrainTextDef& def,   const MsgTrain& msg);
extern short TrainCopy(TrainInfo& def,       const MsgTrainInfo& msg);
extern short TrainCopy(TrainEngineInfo& def, const MsgTrainEngineInfo& msg);
extern short TrainCopy(TrainRecord& rec,     const MsgTrainRecord& msg);

extern short TrainCopy(cTrainTextDef& rec,   const MsgTrainUpdateRecord& msg);

#endif
