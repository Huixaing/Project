#pragma once
#include <map>
#include <vector>
#include <afxtempl.h>
using namespace std;
struct LOCOL_TYPE
{
	int  type;

	char pref[10];
	char name[20];
	char szType[5];

	int  casco_id; // bit7 标识电力车
	
	LOCOL_TYPE()
	{
		type=0;
		casco_id=0;
		memset(pref, 0, sizeof(pref));
		memset(name, 0, sizeof(name));
		memset(szType, 0, sizeof(szType));
	}
};

struct CON_ID_LOCOL
{
	int con_id;
	std::vector<LOCOL_TYPE> vecLocol;
};

class CLocolType
{
public:
	CLocolType(void);
	virtual ~CLocolType(void);

public:
	bool LoadLocolTypeXml(CString path);

	CString GetLocoName(int con_id, CString pref);
	CString GetLocoType(int con_id, CString pref);
	int     GetCascoId(int con_id, CString pref);

	bool    GetLocoInfo(int con_id, CString pref, CString& name, CString& type, int& cascoId);
	int		GetLocoAllPref(int con_id, CStringArray& aryName, CStringArray& arypref, CWordArray& aryid, CWordArray& aryCascoid);

private:
	bool ReadFile(CString pathname);

public:
	std::map<int, CON_ID_LOCOL> mapLocol; 

};
