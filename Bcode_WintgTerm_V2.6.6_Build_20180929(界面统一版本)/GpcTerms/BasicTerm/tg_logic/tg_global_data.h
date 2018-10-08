#pragma once
#include <vector>
#include "caddata.h"
// 保存与section无关的数据
class CCADDataServer;
class CGlobalTGData{
       vector<CADDATA_ABSTIME>  m_listAbsTime;
	   vector<CAD_TRAIN_TYPE_INFO> m_arrayTrainType;
       

	   BOOL m_bDataOK;
  public:
	CGlobalTGData();
	~CGlobalTGData();
  public:
	  BOOL DataOK() {return m_bDataOK;}
      BOOL ProcessServerMessage(int msg_type,const char *msg,int msglen);
	  BOOL LoadFromDatabase(CCADDataServer *pDatabase);
};


BOOL CGlobalTGData::LoadFromDatabase(CCADDataServer *pDb_svr)
{
	pDb_svr->Query_ReadTrainType();
	pDb_svr->Query_ReadPasswayInfo(con_id);
	delete pw;
	pw=new CDlgWait("正在从数据库中读取区间运行时分信息......");
	pDb_svr->Query_ReadAbsTimeInfo(con_id); 

}