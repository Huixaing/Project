#ifndef _DEF_BLOCK_DEF_H
#define _DEF_BLOCK_DEF_H
/* */
#include "schedule.h"

/* 内部数据存贮定义 */
struct cBlockDef : public ScheduleBlock
{
private:
	TRAIN_INDEX     m_BlockIndex;

public:
	TIME  m_LastBroadCastTime;

	cBlockDef();
	TRAIN_INDEX GetBlockIndex() const;
	void SetBlockIndex(TRAIN_INDEX x);

	void   Init();
	short  IsValid();
	short  IfInTimeScope(TIME stime, TIME etime) const;
};

#endif /** THE END **/
