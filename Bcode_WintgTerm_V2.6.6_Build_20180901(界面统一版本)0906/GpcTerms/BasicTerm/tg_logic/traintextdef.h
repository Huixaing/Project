#ifndef _DEF_TRAIN_TEXT_DEF_H
#define _DEF_TRAIN_TEXT_DEF_H

#include "traindef.h"

/* cTrainDef用于内存保存数据，为了减少内存空间的使用将TrainRecord
 * 内的两个Text定义到TrainRecord外部。但是cTrainDef作为参数传递时
 * 较为不便，因此定义如下数据结构，直接包含Text信息 */

struct cTrainTextDef : public cTrainDef
{
	const    int m_MaxRecCount;
	const    int m_MaxTextLength;

public:
	short*    m_pIndexList;
	char*     m_pAbsText;
	char*     m_pStationText;

public:
	cTrainTextDef();
	~cTrainTextDef();

	char*  GetAbsText(int index) const;
	char*  GetStationText(int index) const;
};


#endif /** THE END **/
