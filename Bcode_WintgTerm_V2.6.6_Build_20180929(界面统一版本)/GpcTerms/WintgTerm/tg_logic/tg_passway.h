#ifndef PASSWAY_FILE_H
#define PASSWAY_FILE_H
#include "StdAfx.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <direct.h>

using namespace std;
const char CONFIG_FILE_DIR[] = "config";
const char PASSWAYCFG[] = "passway.xml";
class Passway_Sta
{
	public:
		short Sta_id;              //站号
		short Entry;               //入口
		string  Arrive_Side;    // 到达股道
		string  Depart_Side;    // 出发股道
		short  Exit;            // 出口
		int    in_sta_index;    // 接入站序
		int    out_sta_index;   // 交出站序
		int    change_trainid;  // 是否变化车次号
	public:
		Passway_Sta();
		Passway_Sta(const Passway_Sta& sSource);
		Passway_Sta& operator=(const Passway_Sta& sSource);
		~Passway_Sta();
	private:
		void init();  			
};
class CSingle_Passway
{
     public:
		short Con_id;
		short Passway_id;
		string  Passway_Name;
		std::vector<Passway_Sta> m_vector_Passway_Sta_info;
	public:
		CSingle_Passway();
		CSingle_Passway(const CSingle_Passway& sSource);
		CSingle_Passway& operator=(const CSingle_Passway& sSource);
		~CSingle_Passway();
	private:
	   void init();          
};

class CConsole_Passway
{
public:
	short Con_id;
	std::map<short, CSingle_Passway> m_map_Console_Passway;
public:
	CConsole_Passway();
	CConsole_Passway(const CConsole_Passway& sSource);
	CConsole_Passway& operator=(const CConsole_Passway& sSource);
	~CConsole_Passway();
private:
	void init();          
};

class CBureau_Passway
{
public:
	std::map<short, CConsole_Passway> m_map_Bureau_Passway;
public:
	CBureau_Passway();
	CBureau_Passway(const CBureau_Passway& sSource);
	CBureau_Passway& operator=(const CBureau_Passway& sSource);
	~CBureau_Passway();
private:
	void init();          
};

class  CGetPasswayInfor
{
public:
	CGetPasswayInfor();
	~CGetPasswayInfor();
	bool GetSinglePassway(short con_id,short  PasswayId,CSingle_Passway &Single_Passway);
	bool GetConsolePassway(short Console_id,CConsole_Passway &Console_Passway);
	bool GetConsolePasswayArray(const short Console_id,short &Passway_Quantity, short Passway_id[], string Passway_Name[]);
	bool GetSinglePasswayContent_From_PasswayId(const short  PasswayId, const short con_id, string &PasswayName,
																						   short &Station_Quantity,short sta_id[], short entry[],string arrive_side[],
																					       string  depart_side[], short exit[]);
	bool  LoadPasswayXML(const string pathname);
private:
	bool GetConsolePassway_Conid_To_CConsole_Passway(const short con_id,CConsole_Passway &console_passway);
	void GetPassway_PasswayId_To_Single_Passway();
	bool getDataDir(char * const strPath);
	void print(); 
private:
	CBureau_Passway m_Bureau_Passway;
};

#endif