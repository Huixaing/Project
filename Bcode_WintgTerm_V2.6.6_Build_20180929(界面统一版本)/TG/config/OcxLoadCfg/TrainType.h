#ifndef TRAIN_TYPE_H
#define TRAIN_TYPE_H
#include <vector>
#include <map>

using namespace std;

struct TRAIN_ID_DEFINE
{
	char	prefix[10]; 
	ULONG	start;
	ULONG	end;

	TRAIN_ID_DEFINE()
	{
		memset(prefix, 0, sizeof(prefix));
		start = 0;
		end = 0;
	}
};

struct TRAIN_ID_DEFINES
{
	int		id;
	char    name[21];
	int     run_type;	
	int		priority;
	int     loco_type;

	std::vector<TRAIN_ID_DEFINE> trainId;

	TRAIN_ID_DEFINES();
	~TRAIN_ID_DEFINES();
	TRAIN_ID_DEFINES(const TRAIN_ID_DEFINES& rhs);
	TRAIN_ID_DEFINES& operator=(const TRAIN_ID_DEFINES& rhs);
};


class  CTrainTypeInfo
{
public:
	CTrainTypeInfo();
	~CTrainTypeInfo();

public:
	void LoadTrain_TypeXML(const CString pathname);
	int  GetTrainTypeByTrainNO(CString trainid);
private:
	ULONG GetPrefixAndDigitalTrainId(const char *train_id, char *prefix);
	int m_default_train_type;
	std::map<int, TRAIN_ID_DEFINES> m_map_train_id_defines;
};
#endif