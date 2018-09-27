
#ifndef __TCC_C3_GPC_INFOPROVIDER_H__
#define __TCC_C3_GPC_INFOPROVIDER_H__

#include "tcc.h"


#pragma once

namespace TCC_C3
{

class CTccGpcInfoProvider : public ITccClientInfoProvider
{
public:
	CTccGpcInfoProvider();
	virtual ~CTccGpcInfoProvider();

public:
	bool IsTCC2ReadOnly(){ return m_bReadOnly;}
	bool IsTCC2Execute(){ return m_bExecute;}
	bool IsCTC2GPC(){ return m_bCtcGpc;}
	BOOL LoadTccLineConfig();
	void SetTCCRight(int SectionNo, bool bReadOnly, bool bExecute, bool bCTCGpc);

public:
	static void LoadTccLineNameConfig(CString strFilename);

private:
	bool m_bReadOnly;
	bool m_bExecute;
	bool m_bCtcGpc;

	int  m_nCurrentSectionNo;  // 
	INT  m_nSecNo;

public:
	// TCC数据目录(如../data/DDML/tcc), 用于存放Tcc原始命令
	virtual CString GetTccDataDirectory();

	// TCC配置目录(如../data/tcc), 用于存放Tcc的配置数据
	virtual CString GetTccConfigDirectory();

	// 得到车站列表，返回值为实际取得的列表数目
	virtual void GetStationList(std::vector<stano_t>& staList);
	// 根据站号得到站名
	virtual CString GetStationName(stano_t nStaID);

	// 得到发令单位和发令人
	virtual CString GetOrgSender();
	virtual CString GetPersonSender();

	virtual WORD  GetConsoleID();
	virtual DWORD GetCurrentEntityID();

	// 校验站号的合法性
	virtual BOOL ValidateStationID(stano_t nStaID);

	virtual BOOL IsStationListChanged();

	// 得到可用命令号数值范围
	//virtual BOOL  IsValidCommandID(cmdid_t nCmdID);
	//virtual cmdid_t GetInvalidCommandID();
	//virtual cmdid_t GetMinCommandID();
	//virtual cmdid_t GetMaxCommandID();
	//virtual cmdid_t GetNextCommandID(cmdid_t nCmdID);
};

};

#endif
