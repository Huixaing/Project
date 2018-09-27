
#ifndef __TCC_GPC_INFOPROVIDER_H__
#define __TCC_GPC_INFOPROVIDER_H__

#include "tcc.h"



#pragma once

namespace TCC
{
class CTccGpcInfoProvider : public ITccClientInfoProvider
{
public:
	CTccGpcInfoProvider();
	virtual ~CTccGpcInfoProvider();
		
public:
	bool IsTCCReadOnly(){ return m_bReadOnly;}
	bool IsTCCExecute(){ return m_bExecute;}
	bool IsCTCGPC(){ return m_bCTCGpc;}
	BOOL LoadTccLineConfig();
	void SetTCCRight(int SectionNo, bool bReadOnly, bool bExecute, bool bCTCGpc);

public:
	static void LoadTccLineNameConfig(CString strFilename);

private:
	bool m_bReadOnly;
	bool m_bExecute;
	bool m_bCTCGpc;
	int  m_nCurrentSectionNo;  // 
	INT  m_nSecNo;

public:
	// TCC数据目录(如../data/DDML/tcc), 用于存放Tcc原始命令
	virtual CString GetTccDataDirectory();

	// TCC配置目录(如../data/tcc), 用于存放Tcc的配置数据
	virtual CString GetTccConfigDirectory();

	// 得到车站列表，返回值为实际取得的列表数目
	virtual void GetStationList(std::vector<TCC::stano_t>& staList);
	// 根据站号得到站名
	virtual CString GetStationName(TCC::stano_t nStaID);

	// 得到发令单位和发令人
	virtual CString GetOrgSender();
	virtual CString GetPersonSender();

	virtual WORD  GetConsoleID();

	// 判断nStaID与nStaID2 是否相邻站
	virtual BOOL IsLinkedStation(TCC::stano_t nStaID, TCC::stano_t nStaID2);
	// 站nStaID2相对于站nStaID的区间方向性(上行方向, 下行方向)
	virtual TCC::dir_t GetStationDirection(
						TCC::stano_t nStaID, 
						TCC::stano_t nStaID2);
	// 得到站nStaID的相邻站列表
	virtual void GetLinkedStationList(TCC::stano_t nStaID, std::vector<TCC::stano_t>& staList);
	// 根据站号, Tcc线路号和方向(即区间)得到相邻站
	virtual TCC::stano_t  GetLinkedStation(
		TCC::stano_t nStaID, 
		TCC::lineid_t nTccLineID,
		TCC::dir_t dir);

	// 取得站nStaID的TccLine列表
	virtual void GetTccLineList(
					TCC::stano_t nStaID, 
					std::vector<TCC::lineid_t>& lines);
	// 取得站nStaID的TccLine列表中与站nStaID2相连的那些TccLine
	virtual void GetTccLineList(
					TCC::stano_t nStaID,
					TCC::stano_t nStaID2,
					std::vector<TCC::lineid_t>& lines);

	virtual void GetTccBlockLineList(TCC::stano_t nStaID, std::vector<TCC::lineid_t>& lines);
	virtual CString GetTccBlockLineName(TCC::stano_t nStaID, TCC::lineid_t nTccLineID);
	virtual BOOL IsTccBlockLine(TCC::stano_t nStaID, TCC::lineid_t nTccLineID);

	// 取得站nStaID中与站nStaID2的线路nTccLineID2相匹配的TccLine
	virtual TCC::lineid_t GetTccLine(
					TCC::stano_t nStaID,
					TCC::stano_t nStaID2,
					TCC::lineid_t nTccLineID2);
	// 取得站nStaID的nTccLineID的名称
	virtual CString GetTccLineName(TCC::stano_t nStaID, TCC::lineid_t nTccLineID);
	// 取得目标站nStaID的TCC线路nTccLineID所跨区间
	// DIR_UP 上行区间
	// DIR_DOWN 下行区间
	// DIR_DUAL 上下行区间
	// DIR_UNKNOWN 线路号不存在
	virtual TCC::dir_t GetTccLineAbsNum(TCC::stano_t nStaID, TCC::lineid_t nTccLineID);
	// 取得TccLine的方向值(上下行或双向)
	virtual TCC::dir_t GetTccLineDirection(TCC::stano_t nStaID, TCC::lineid_t nTccLineID);

	// 取得内部线路号
	virtual int GetLineID(TCC::stano_t nTccID, TCC::lineid_t nTccLineID);
	// 根据公里标取得相关站
	std::vector<TCC::stano_t> GetTccList(int builtin_id, TCC::kmmark_t nKmBegin, TCC::kmmark_t nKmEnd);

	// 校验站号的合法性
	virtual BOOL ValidateStationID(TCC::stano_t nStaID);

	virtual BOOL IsStationListChanged();
};

}
#endif
