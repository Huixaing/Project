#pragma once
using namespace std;
#include <map>
#include <vector>
struct TrainIdDesp
{
	int beginNo;
	int endNo;
	CString prefix;
	TrainIdDesp();
};

struct TrainAttribute
{
	int type;
	int priority;
	int run_type;
	CString name;
	std::vector<TrainIdDesp> trainid_desp;

	TrainAttribute();
	~TrainAttribute();
	TrainAttribute(const TrainAttribute& rhs);
	TrainAttribute& operator=(const TrainAttribute& rhs);
};

class TrainTypeCfg
{
public:
	TrainTypeCfg(void);
	virtual ~TrainTypeCfg(void);

public:
	bool ReadTrainType(CString filePath);
	int  GetTrainTypeByTrainId(const CString& trainid);

public:
	std::map<int, TrainAttribute> mTrainAttributeMap;
	std::map<int, CString> mPriorityNameMap;
	std::map<int, CString> mRunTypeNameMap;
	int default_train_type;
};
