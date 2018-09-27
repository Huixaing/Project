#ifndef TRAIN_TYPE_H
#define TRAIN_TYPE_H
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <direct.h>

using namespace std;


class RUN_TYPES
{
	public:
	      short id;
	      string name; 
	public:
		RUN_TYPES();
		RUN_TYPES(const RUN_TYPES& sSource);
		RUN_TYPES& operator=(const RUN_TYPES& sSource);
		~RUN_TYPES();
	private:
		void init();  		 
};

class TRAIN_TYPES_ID_NAME_RUNTYPE_PRI
{
public:
	BYTE  id;
	string name; 
	BYTE  run_type;
	BYTE  priority;
	BYTE  loco_type; // 0: NA  0x01: 电力  0x02: 非电力,0x04:超限
public:
	TRAIN_TYPES_ID_NAME_RUNTYPE_PRI();
	TRAIN_TYPES_ID_NAME_RUNTYPE_PRI(const TRAIN_TYPES_ID_NAME_RUNTYPE_PRI& sSource);
	TRAIN_TYPES_ID_NAME_RUNTYPE_PRI& operator=(const TRAIN_TYPES_ID_NAME_RUNTYPE_PRI& sSource);
	~TRAIN_TYPES_ID_NAME_RUNTYPE_PRI();
private:
	void init();  		 
};

class PRIORITIES
{
public:
	short id;
	string name; 
public:
	PRIORITIES();
	PRIORITIES(const PRIORITIES& sSource);
	PRIORITIES& operator=(const PRIORITIES& sSource);
	~PRIORITIES();
private:
	void init();  		 
};

class TRAIN_TYPES
{
public:
	string	prefix; 
	ULONG	start;
	ULONG	end;
	BYTE	    id;
	string       name;
	BYTE    	run_type;	
	BYTE	    priority;
public:
	TRAIN_TYPES();
	TRAIN_TYPES(const TRAIN_TYPES& sSource);
	TRAIN_TYPES& operator=(const TRAIN_TYPES& sSource);
	~TRAIN_TYPES();
private:
	void init();  		 
};


class  CGetTrain_TypeInfor
{
public:
	CGetTrain_TypeInfor();
	~CGetTrain_TypeInfor();
	bool GetPrioFromTrainId(const char* train_id, BYTE* train_type_id, BYTE* run_type_id, BYTE* prioritiy_id);
	const char* GetTrainTypeName(const BYTE train_type_id);
	const char* GetRunTypeName(const BYTE run_type_id);
	const char* GetPrioTypeName(const BYTE prioritiy_id);
	bool GetRunTypeNameArray(vector <string> &vec);
	bool GetTrainTypeNameArray(vector <string> &vec);
	bool GetPrioTypeNameArray(vector <string> &vec);
	void LoadTrain_TypeXML(const string pathname);
	// following by cuihu,用于检索所有类型
	const char* GetTrainTypeName(int index,int *value_id);
	const char* GetRunTypeName(int index, int *value_id);
	const char* GetPrioTypeName(int index,int *value_id);

	BOOL GetTrainTypeDef(BYTE  train_type,BYTE *run_type,BYTE *prio,BYTE *locotype);

	const char* GetAbsTimeTypeName(int index, int *value_id);
	bool  IsEnableABSTimeType();
	int   GetSpecRunTypeName(CStringArray& name_list, CWordArray& run_type_list);
	const char* GetAbsRunTypeName(const BYTE run_type_id);

private:
	
	void init(); 
	bool getDataDir(char * const strPath); 
	static ULONG GetPrefixAndDigitalTrainId(const char *train_id, char *prefix);

private:
	std::map<short, TRAIN_TYPES> m_map_TRAIN_TYPES;
	std::map<short, PRIORITIES> m_map_PRIORITIES;
	std::map<short, RUN_TYPES> m_map_RUN_TYPES;
	std::map<short, RUN_TYPES> m_map_ABS_TIME_TYPES;
	std::map<BYTE,TRAIN_TYPES_ID_NAME_RUNTYPE_PRI> m_map_TRAIN_TYPE_DETAILS;
	BYTE m_default_train_type;
	USHORT train_type_Quantity;
};
#endif